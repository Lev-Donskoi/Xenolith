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

#ifndef XENOLITH_GL_COMMON_XLGLOBJECT_H_
#define XENOLITH_GL_COMMON_XLGLOBJECT_H_

#include "XLGl.h"

namespace stappler::xenolith::gl {

class TextureSet;

class ObjectInterface {
public:
	using ClearCallback = void (*) (Device *, ObjectType, void *);

	virtual ~ObjectInterface() { }
	virtual bool init(Device &, ClearCallback, ObjectType, void *ptr);
	virtual void invalidate();

	ObjectType getType() const { return _type; }

protected:
	ObjectType _type;
	Device *_device = nullptr;
	ClearCallback _callback = nullptr;
	void *_ptr = nullptr;
};


class NamedObject : public NamedRef, public ObjectInterface {
public:
	virtual ~NamedObject();
};


class Object : public Ref, public ObjectInterface {
public:
	virtual ~Object();
};


class Pipeline : public NamedObject {
public:
	virtual ~Pipeline() { }

	virtual StringView getName() const override { return _name; }

protected:
	String _name;
};


class Shader : public NamedObject {
public:
	static void inspectShader(SpanView<uint32_t>);

	virtual ~Shader() { }

	virtual StringView getName() const override { return _name; }
	virtual ProgramStage getStage() const { return _stage; }

protected:
	virtual void inspect(SpanView<uint32_t>);

	String _name;
	ProgramStage _stage = ProgramStage::None;
};


class RenderPassImpl : public NamedObject {
public:
	virtual ~RenderPassImpl() { }

	virtual StringView getName() const override { return _name; }

protected:
	String _name;
};

class Framebuffer : public Object {
public:
	virtual ~Framebuffer() { }

protected:
	Rc<RenderPassImpl> _renderPass;
	Vector<Rc<ImageView>> imageViews;
};

class ImageObject : public Object {
public:
	virtual ~ImageObject() { }

	virtual bool init(Device &, ClearCallback, ObjectType, void *ptr) override;

	const ImageInfo &getInfo() const { return _info; }
	uint64_t getIndex() const { return _index; }

	ImageViewInfo getViewInfo(const ImageViewInfo &) const;

protected:
	ImageInfo _info;

	uint64_t _index = 1; // 0 stays as special value
};

class ImageView : public Object {
public:
	virtual ~ImageView() { }

	virtual bool init(Device &, ClearCallback, ObjectType, void *ptr) override;

	const Rc<ImageObject> &getImage() const { return _image; }
	const ImageViewInfo &getInfo() const { return _info; }

	void setLocation(uint32_t set, uint32_t desc) {
		_set = set;
		_descriptor = desc;
	}

	uint32_t getSet() const { return _set; }
	uint32_t getDescriptor() const { return _descriptor; }
	uint64_t getIndex() const { return _index; }

protected:
	ImageViewInfo _info;
	Rc<ImageObject> _image;

	uint32_t _set = 0;
	uint32_t _descriptor = 0;

	// all ImageViews are atomically indexed for descriptor caching purpose
	uint64_t _index = 1; // 0 stays as special value
};

class BufferObject : public Object {
public:
	virtual ~BufferObject() { }

	const BufferInfo &getInfo() const { return _info; }
	uint64_t getSize() const { return _info.size; }

protected:
	BufferInfo _info;
};


class Sampler : public Object {
public:
	virtual ~Sampler() { }

	const SamplerInfo &getInfo() const { return _info; }

	void setIndex(uint32_t idx) { _index = idx; }
	uint32_t getIndex() const { return _index; }

protected:
	uint32_t _index = 0;
	SamplerInfo _info;
};

struct MaterialImageSlot {
	Rc<ImageView> image;
	uint32_t refCount = 0;
};

struct MaterialLayout {
	Vector<MaterialImageSlot> slots;
	uint32_t usedSlots = 0;
	Rc<TextureSet> set;
};

class TextureSet : public Object {
public:
	virtual ~TextureSet() { }

	virtual void write(const MaterialLayout &);

protected:
	uint32_t _count = 0;
	Vector<uint64_t> _layoutIndexes;
};

}

#endif /* XENOLITH_GL_COMMON_XLGLOBJECT_H_ */
