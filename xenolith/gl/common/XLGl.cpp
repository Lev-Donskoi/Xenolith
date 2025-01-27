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

#include "XLDefine.h"

#ifdef XL_LOOP_DEBUG
#define XL_LOOP_LOG(...) log::vtext("Gl::Loop", __VA_ARGS__)
#else
#define XL_LOOP_LOG(...)
#endif

#ifdef XL_FRAME_DEBUG
#define XL_FRAME_LOG(...) log::vtext("Gl::Frame", __VA_ARGS__)
#else
#define XL_FRAME_LOG(...)
#endif

#include "XLGlDevice.cc"
#include "XLGlCommandList.cc"
#include "XLGlLoop.cc"
#include "XLGlView.cc"
#include "XLGlObject.cc"
#include "XLGlResource.cc"
#include "XLGlAttachment.cc"
#include "XLGlRenderQueue.cc"
#include "XLGlMaterial.cc"
#include "XLGlInstance.cc"
#include "XLGlFrame.cc"
#include "XLGlRenderPass.cc"
#include "XLGlUtils.cc"
#include "XLGlSwapchain.cc"
