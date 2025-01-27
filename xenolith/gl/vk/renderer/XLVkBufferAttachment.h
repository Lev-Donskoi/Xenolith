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

#ifndef XENOLITH_GL_VK_RENDERER_XLVKBUFFERATTACHMENT_H_
#define XENOLITH_GL_VK_RENDERER_XLVKBUFFERATTACHMENT_H_

#include "XLVkFramebuffer.h"
#include "XLVkSync.h"
#include "XLGlAttachment.h"

namespace stappler::xenolith::vk {

class DeviceBuffer;
class RenderPassHandle;

class BufferAttachment : public gl::BufferAttachment {
public:

protected:
};

class BufferAttachmentHandle : public gl::AttachmentHandle {
public:
	virtual ~BufferAttachmentHandle();

	virtual bool writeDescriptor(const RenderPassHandle &, const gl::PipelineDescriptor &,
			uint32_t, bool, VkDescriptorBufferInfo &) { return false; }
};

class TexelAttachmentHandle : public gl::AttachmentHandle {
public:
	virtual ~TexelAttachmentHandle();

	virtual VkBufferView getDescriptor(const RenderPassHandle &, const gl::PipelineDescriptor &,
			uint32_t, bool) { return VK_NULL_HANDLE; }
};

class VertexBufferAttachment : public gl::BufferAttachment {
public:
	virtual ~VertexBufferAttachment();

protected:
	virtual Rc<gl::AttachmentHandle> makeFrameHandle(const gl::FrameHandle &) override;
};

class VertexBufferAttachmentHandle : public BufferAttachmentHandle {
public:
	virtual ~VertexBufferAttachmentHandle();

	virtual bool submitInput(gl::FrameHandle &, Rc<gl::AttachmentInputData> &&) override;

	virtual bool isDescriptorDirty(const gl::RenderPassHandle &, const gl::PipelineDescriptor &,
			uint32_t, bool isExternal) const override;

	virtual bool writeDescriptor(const RenderPassHandle &, const gl::PipelineDescriptor &,
			uint32_t, bool, VkDescriptorBufferInfo &) override;

	const Rc<DeviceBuffer> &getVertexes() const { return _vertexes; }
	const Rc<DeviceBuffer> &getIndexes() const { return _indexes; }

	virtual void writeVertexes(gl::FrameHandle &fhandle);

protected:
	virtual bool loadVertexes(gl::FrameHandle &, const Rc<gl::VertexData> &);

	Device *_device = nullptr;
	Rc<DeviceQueue> _transferQueue;

	Rc<Fence> _fence;
	Rc<CommandPool> _pool;

	Rc<DeviceBuffer> _vertexes;

	Rc<DeviceBuffer> _indexesStaging;
	Rc<DeviceBuffer> _indexes;

	Rc<gl::VertexData> _data;
};

}

#endif /* XENOLITH_GL_VK_RENDERER_XLVKBUFFERATTACHMENT_H_ */
