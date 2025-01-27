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

#ifndef XENOLITH_NODES_XLSPRITE_H_
#define XENOLITH_NODES_XLSPRITE_H_

#include "XLNode.h"
#include "XLResourceCache.h"
#include "XLVertexArray.h"

namespace stappler::xenolith {

class Sprite : public Node {
public:
	virtual ~Sprite() { }

	virtual bool init();
	virtual bool init(StringView);
	virtual bool init(Rc<Texture> &&);

	virtual void setTexture(StringView);
	virtual void setTexture(Rc<Texture> &&);

	virtual void visit(RenderFrameInfo &, NodeFlags parentFlags) override;
	virtual void draw(RenderFrameInfo &, NodeFlags flags) override;

	virtual void onEnter(Scene *) override;

	virtual void setColorMode(const ColorMode &mode);
	virtual const ColorMode &getColorMode() const { return _colorMode; }

protected:
	virtual MaterialInfo getMaterialInfo() const;
	virtual Vector<const gl::ImageData *> getMaterialImages() const;
	virtual void updateColor() override;
	virtual void updateVertexes();

	String _textureName;
	Rc<Texture> _texture;
	VertexArray _vertexes;

	bool _flippedX = false;
	bool _flippedY = false;
	bool _rotated = false;
	Rect _textureRect = Rect(0.0f, 0.0f, 1.0f, 1.0f);

	uint64_t _materialId = 0;
	bool _materialDirty = true;

	Color4F _tmpColor;
	ColorMode _colorMode;
};

}

#endif /* XENOLITH_NODES_XLSPRITE_H_ */
