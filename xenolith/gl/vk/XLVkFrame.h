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

#ifndef XENOLITH_GL_VK_XLVKFRAME_H_
#define XENOLITH_GL_VK_XLVKFRAME_H_

#include "XLVkAllocator.h"
#include "XLGlFrame.h"

namespace stappler::xenolith::vk {

class FrameHandle : public gl::FrameHandle {
public:
	virtual ~FrameHandle() { }

	bool init(gl::Loop &, gl::Swapchain &swapchain, gl::RenderQueue &, uint64_t order, uint32_t gen, bool readyForSubmit = false);
	bool init(gl::Loop &, gl::RenderQueue &, uint64_t order, uint32_t gen);

	const Rc<DeviceMemoryPool> &getMemPool() const { return _memPool; }

	Rc<SwapchainSync> acquireSwapchainSync();
	void invalidateSwapchain();

protected:
	Rc<SwapchainSync> _swapchainSync;
	Rc<DeviceMemoryPool> _memPool;
};

}

#endif /* XENOLITH_GL_VK_XLVKFRAME_H_ */
