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

#ifndef XENOLITH_GL_VK_XLVKOBJECT_H_
#define XENOLITH_GL_VK_XLVKOBJECT_H_

#include "XLVkDevice.h"
#include "XLGlObject.h"

namespace stappler::xenolith::vk {

class DeviceMemory : public gl::Object {
public:
	virtual ~DeviceMemory() { }

	bool init(Device &dev, VkDeviceMemory);

	VkDeviceMemory getMemory() const { return _memory; }

protected:
	VkDeviceMemory _memory = VK_NULL_HANDLE;
};

class Image : public gl::ImageObject {
public:
	virtual ~Image() { }

	bool init(Device &dev, VkImage, const gl::ImageInfo &); // non-owining image wrapping
	bool init(Device &dev, VkImage, const gl::ImageInfo &, Rc<DeviceMemory> &&); // owning image wrapping

	VkImage getImage() const { return _image; }

	void setPendingBarrier(const VkImageMemoryBarrier &);
	const VkImageMemoryBarrier *getPendingBarrier() const;
	void dropPendingBarrier();

protected:
	Rc<DeviceMemory> _memory;
	VkImage _image = VK_NULL_HANDLE;
	std::optional<VkImageMemoryBarrier> _barrier;
};

class Buffer : public gl::BufferObject {
public:
	virtual ~Buffer() { }

	bool init(Device &dev, VkBuffer, const gl::BufferInfo &, Rc<DeviceMemory> &&);

	VkBuffer getBuffer() const { return _buffer; }

	void setPendingBarrier(const VkBufferMemoryBarrier &);
	const VkBufferMemoryBarrier *getPendingBarrier() const;
	void dropPendingBarrier();

protected:
	Rc<DeviceMemory> _memory;
	VkBuffer _buffer = VK_NULL_HANDLE;
	std::optional<VkBufferMemoryBarrier> _barrier;
};

class ImageView : public gl::ImageView {
public:
	virtual ~ImageView() { }

	bool init(Device &dev, VkImage, VkFormat format);
	bool init(Device &dev, const gl::ImageAttachmentDescriptor &desc, Image *);
	bool init(Device &dev, Image *, const gl::ImageViewInfo &);

	VkImageView getImageView() const { return _imageView; }

protected:
	VkImageView _imageView = VK_NULL_HANDLE;
};

class Sampler : public gl::Sampler {
public:
	virtual ~Sampler() { }

	bool init(Device &dev, const gl::SamplerInfo &);

	VkSampler getSampler() const { return _sampler; }

protected:
	VkSampler _sampler = VK_NULL_HANDLE;
};

}

#endif /* XENOLITH_GL_VK_XLVKOBJECT_H_ */
