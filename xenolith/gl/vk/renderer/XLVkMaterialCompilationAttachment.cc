/**
 Copyright (c) 2021 Roman Katuntsev <sbkarr@stappler.org>

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 **/

#include "XLVkMaterialCompilationAttachment.h"
#include "XLVkFrame.h"
#include "XLVkObject.h"
#include "XLVkBuffer.h"

namespace stappler::xenolith::vk {

MaterialCompilationAttachment::~MaterialCompilationAttachment() { }

Rc<gl::AttachmentHandle> MaterialCompilationAttachment::makeFrameHandle(const gl::FrameHandle &handle) {
	return Rc<MaterialCompilationAttachmentHandle>::create(this, handle);
}

MaterialCompilationAttachmentHandle::~MaterialCompilationAttachmentHandle() { }

bool MaterialCompilationAttachmentHandle::setup(gl::FrameHandle &handle) {
	return true;
}

bool MaterialCompilationAttachmentHandle::submitInput(gl::FrameHandle &handle, Rc<gl::AttachmentInputData> &&data) {
	if (auto d = data.cast<gl::MaterialInputData>()) {
		handle.performOnGlThread([this, d = move(d)] (gl::FrameHandle &handle) {
			_inputData = d;
			_originalSet = _inputData->attachment->getMaterials();
			handle.setInputSubmitted(this);
		}, this);
		return true;
	}
	return false;
}

void MaterialCompilationAttachmentHandle::setOutput(const Rc<gl::MaterialSet> &out) {
	_outputSet = out;
}

MaterialCompilationRenderPass::~MaterialCompilationRenderPass() { }

bool MaterialCompilationRenderPass::init(StringView name) {
	if (RenderPass::init(name, gl::RenderPassType::Generic, gl::RenderOrderingHighest, 1)) {
		_queueOps = QueueOperations::Transfer;
		return true;
	}
	return false;
}

bool MaterialCompilationRenderPass::inProgress(const gl::MaterialAttachment *a) const {
	auto it = _inProgress.find(a);
	if (it != _inProgress.end()) {
		return true;
	}
	return false;
}

void MaterialCompilationRenderPass::setInProgress(const gl::MaterialAttachment *a) {
	_inProgress.emplace(a);
}

void MaterialCompilationRenderPass::dropInProgress(const gl::MaterialAttachment *a) {
	_inProgress.erase(a);
}

bool MaterialCompilationRenderPass::hasRequest(const gl::MaterialAttachment *a) const {
	auto it = _requests.find(a);
	if (it != _requests.end()) {
		return true;
	}
	return false;
}

void MaterialCompilationRenderPass::appendRequest(const gl::MaterialAttachment *a, Vector<Rc<gl::Material>> &&req) {
	auto it = _requests.find(a);
	if (it == _requests.end()) {
		it = _requests.emplace(a, Map<gl::MaterialId, Rc<gl::Material>>()).first;
	}

	for (auto &m : req) {
		auto iit = it->second.find(m->getId());
		if (iit == it->second.end()) {
			it->second.emplace(m->getId(), move(m));
		} else {
			iit->second = move(m);
		}
	}
}

Rc<gl::MaterialInputData> MaterialCompilationRenderPass::popRequest(const gl::MaterialAttachment *a) {
	auto it = _requests.find(a);
	if (it != _requests.end()) {
		Rc<gl::MaterialInputData> ret = Rc<gl::MaterialInputData>::alloc();
		ret->attachment = a;
		ret->materials.reserve(it->second.size());
		for (auto &m : it->second) {
			ret->materials.emplace_back(m.second);
		}
		_requests.erase(it);
		return ret;
	}

	return nullptr;
}

void MaterialCompilationRenderPass::clearRequests() {
	_requests.clear();
}

uint64_t MaterialCompilationRenderPass::incrementOrder() {
	auto ret = _order;
	++ _order;
	return ret;
}

Rc<gl::RenderPassHandle> MaterialCompilationRenderPass::makeFrameHandle(gl::RenderPassData *data, const gl::FrameHandle &handle) {
	return Rc<MaterialCompilationRenderPassHandle>::create(*this, data, handle);
}

void MaterialCompilationRenderPass::prepare(gl::Device &) {
	for (auto &it : _data->descriptors) {
		if (auto a = dynamic_cast<MaterialCompilationAttachment *>(it->getAttachment())) {
			_materialAttachment = a;
		}
	}
}

MaterialCompilationRenderPassHandle::~MaterialCompilationRenderPassHandle() { }

void MaterialCompilationRenderPassHandle::addRequiredAttachment(const gl::Attachment *a, const Rc<gl::AttachmentHandle> &h) {
	RenderPassHandle::addRequiredAttachment(a, h);
	if (a == ((MaterialCompilationRenderPass *)_renderPass.get())->getMaterialAttachment()) {
		_materialAttachment = (MaterialCompilationAttachmentHandle *)h.get();
	}
}

Vector<VkCommandBuffer> MaterialCompilationRenderPassHandle::doPrepareCommands(gl::FrameHandle &handle, uint32_t index) {
	auto table = _device->getTable();
	auto buf = _pool->allocBuffer(*_device);
	auto layout = _device->getTextureSetLayout();

	auto &originalData = _materialAttachment->getOriginalSet();
	auto &inputData = _materialAttachment->getInputData();

	// create new material set generation
	auto data = inputData->attachment->cloneSet(originalData);

	auto buffers = updateMaterials(handle, data, inputData->materials);

	QueueOperations ops = QueueOperations::None;
	for (auto &it : inputData->attachment->getRenderPasses()) {
		ops |= ((RenderPass *)it->renderPass.get())->getQueueOps();
	}

	auto q = _device->getQueueFamily(ops);
	if (!q) {
		return Vector<VkCommandBuffer>();
	}

	// transition images and build buffer
	VkCommandBufferBeginInfo beginInfo { };
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	beginInfo.pInheritanceInfo = nullptr;

	if (table->vkBeginCommandBuffer(buf, &beginInfo) != VK_SUCCESS) {
		return Vector<VkCommandBuffer>();
	}

	VkBufferCopy indexesCopy;
	indexesCopy.srcOffset = 0;
	indexesCopy.dstOffset = 0;
	indexesCopy.size = buffers.stagingBuffer->getSize();

	table->vkCmdCopyBuffer(buf, buffers.stagingBuffer->getBuffer(), buffers.targetBuffer->getBuffer(), 1, &indexesCopy);

	if (q->index == _pool->getFamilyIdx()) {
		VkBufferMemoryBarrier bufferBarrier({
			VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER, nullptr,
			VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
			VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
			buffers.targetBuffer->getBuffer(), 0, VK_WHOLE_SIZE
		});

		table->vkCmdPipelineBarrier(buf, VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0,
				0, nullptr,
				1, &bufferBarrier,
				0, nullptr);
	} else {
		VkBufferMemoryBarrier bufferBarrier({
			VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER, nullptr,
			VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
			_pool->getFamilyIdx(), q->index,
			buffers.targetBuffer->getBuffer(), 0, VK_WHOLE_SIZE
		});

		table->vkCmdPipelineBarrier(buf, VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0,
				0, nullptr,
				1, &bufferBarrier,
				0, nullptr);

		buffers.targetBuffer->setPendingBarrier(bufferBarrier);
	}

	if (table->vkEndCommandBuffer(buf) == VK_SUCCESS) {
		data->setBuffer(move(buffers.targetBuffer), move(buffers.ordering));
		_materialAttachment->setOutput(data);
		return Vector<VkCommandBuffer>{buf};
	}
	return Vector<VkCommandBuffer>();
}

}
