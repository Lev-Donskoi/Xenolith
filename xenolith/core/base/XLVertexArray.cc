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

#include "XLVertexArray.h"

namespace stappler::xenolith {

VertexArray::Quad & VertexArray::Quad::setTextureRect(const Rect &texRect,
		float texWidth, float texHeight, bool flippedX, bool flippedY, bool rotated) {

	gl::Vertex_V4F_V4F_T2F2U *data = const_cast<gl::Vertex_V4F_V4F_T2F2U *>(vertexes.data());

	float texLeft = texRect.origin.x / texWidth;
	float texRight = (texRect.origin.x + texRect.size.width) / texWidth;
	float texTop = texRect.origin.y / texHeight;
	float texBottom = (texRect.origin.y + texRect.size.height) / texHeight;

	if (flippedX) {
		std::swap(texLeft, texRight);
	}

	if (flippedY) {
		std::swap(texTop, texBottom);
	}

	if (!rotated) {
		 // tl bl tr br
		data[0].tex = Vec2(texLeft, texTop);
		data[1].tex = Vec2(texLeft, texBottom);
		data[2].tex = Vec2(texRight, texTop);
		data[3].tex = Vec2(texRight, texBottom);
	} else {
		 // tl bl tr br
		data[0].tex = Vec2(texLeft, texTop);
		data[1].tex = Vec2(texRight, texTop);
		data[2].tex = Vec2(texLeft, texBottom);
		data[3].tex = Vec2(texRight, texBottom);
	}

	return *this;
}

VertexArray::Quad & VertexArray::Quad::setTexturePoints(
		const Vec2 &tl, const Vec2 &bl, const Vec2 &tr, const Vec2 &br, float texWidth, float texHeight) {

	gl::Vertex_V4F_V4F_T2F2U *data = const_cast<gl::Vertex_V4F_V4F_T2F2U *>(vertexes.data());

	 // tl bl tr br
	data[0].tex = Vec2(tl.x / texWidth, tl.y / texHeight);
	data[1].tex = Vec2(bl.x / texWidth, bl.y / texHeight);
	data[2].tex = Vec2(tr.x / texWidth, tr.y / texHeight);
	data[3].tex = Vec2(br.x / texWidth, br.y / texHeight);
	return *this;
}

VertexArray::Quad & VertexArray::Quad::setGeometry(const Vec4 &pos, const Size &size, const Mat4 &transform) {
	const float x1 = pos.x;
	const float y1 = pos.y;

	const float x2 = x1 + size.width;
	const float y2 = y1 + size.height;
	const float x = transform.m[12];
	const float y = transform.m[13];

	const float cr = transform.m[0];
	const float sr = transform.m[1];
	const float cr2 = transform.m[5];
	const float sr2 = -transform.m[4];

	// d - c
	// |   |
	// a - b

	const float ax = x1 * cr - y1 * sr2 + x;
	const float ay = x1 * sr + y1 * cr2 + y;

	const float bx = x2 * cr - y1 * sr2 + x;
	const float by = x2 * sr + y1 * cr2 + y;

	const float cx = x2 * cr - y2 * sr2 + x;
	const float cy = x2 * sr + y2 * cr2 + y;

	const float dx = x1 * cr - y2 * sr2 + x;
	const float dy = x1 * sr + y2 * cr2 + y;

	gl::Vertex_V4F_V4F_T2F2U *data = const_cast<gl::Vertex_V4F_V4F_T2F2U *>(vertexes.data());

	 // tl bl tr br
	data[0].pos = Vec4( dx, dy, pos.z, pos.w);
	data[1].pos = Vec4( ax, ay, pos.z, pos.w);
	data[2].pos = Vec4( cx, cy, pos.z, pos.w);
	data[3].pos = Vec4( bx, by, pos.z, pos.w);

	return *this;
}

VertexArray::Quad & VertexArray::Quad::setGeometry(const Vec4 &pos, const Size &size) {
	const float x1 = pos.x;
	const float y1 = pos.y;

	const float x2 = x1 + size.width;
	const float y2 = y1 + size.height;

	gl::Vertex_V4F_V4F_T2F2U *data = const_cast<gl::Vertex_V4F_V4F_T2F2U *>(vertexes.data());

	// (x1, y2) - (x2, y2)
	// |          |
	// (x1, y1) - (x2, y1)

	 // tl bl tr br
	data[0].pos = Vec4( x1, y2, pos.z, 1.0);
	data[1].pos = Vec4( x1, y1, pos.z, 1.0);
	data[2].pos = Vec4( x2, y2, pos.z, 1.0);
	data[3].pos = Vec4( x2, y1, pos.z, 1.0);

	return *this;
}

VertexArray::Quad & VertexArray::Quad::setColor(const Color4F &color) {
	gl::Vertex_V4F_V4F_T2F2U *data = const_cast<gl::Vertex_V4F_V4F_T2F2U *>(vertexes.data());
	data[0].color = color;
	data[1].color = color;
	data[2].color = color;
	data[3].color = color;
	return *this;
}

VertexArray::Quad & VertexArray::Quad::setColor(SpanView<Color4F> colors) { // tl bl tr br
	if (colors.size() != 4) {
		return *this;
	}

	gl::Vertex_V4F_V4F_T2F2U *data = const_cast<gl::Vertex_V4F_V4F_T2F2U *>(vertexes.data());
	data[0].color = colors[0];
	data[1].color = colors[1];
	data[2].color = colors[2];
	data[3].color = colors[3];
	return *this;
}

VertexArray::Quad & VertexArray::Quad::setColor(std::initializer_list<Color4F> &&colors) { // tl bl tr br
	return setColor(SpanView<Color4F>(colors.begin(), colors.size()));
}

bool VertexArray::init(uint32_t bufferCapacity, uint32_t indexCapacity) {
	_data = Rc<gl::VertexData>::alloc();
	_data->data.reserve(bufferCapacity);
	_data->indexes.reserve(indexCapacity);
	return true;
}

void VertexArray::reserve(uint32_t bufferCapacity, uint32_t indexCapacity) {
	if (_copyOnWrite) {
		copy();
	}

	if (_data->data.capacity() > bufferCapacity) {
		_data->data.reserve(bufferCapacity);
	}
	if (_data->indexes.capacity() > indexCapacity) {
		_data->indexes.reserve(indexCapacity);
	}
}

const Rc<gl::VertexData> &VertexArray::pop() {
	_copyOnWrite = true;
	return _data;
}

void VertexArray::clear() {
	if (_copyOnWrite) {
		copy();
	}

	_data->data.clear();
	_data->indexes.clear();
}

VertexArray::Quad VertexArray::addQuad() {
	if (_copyOnWrite) {
		copy();
	}

	auto firstVertex = _data->data.size();
	auto firstIndex = _data->indexes.size();

	_data->data.resize(_data->data.size() + 4);
	_data->indexes.resize(_data->indexes.size() + 6);

	// 0 - 2
	// |   |
	// 1 - 3
	//
	// counter-clockwise:

	_data->indexes[firstIndex + 0] = firstVertex + 0;
	_data->indexes[firstIndex + 1] = firstVertex + 1;
	_data->indexes[firstIndex + 2] = firstVertex + 2;
	_data->indexes[firstIndex + 3] = firstVertex + 3;
	_data->indexes[firstIndex + 4] = firstVertex + 2;
	_data->indexes[firstIndex + 5] = firstVertex + 1;

	return Quad({
		SpanView<gl::Vertex_V4F_V4F_T2F2U>(_data->data.data() + firstVertex, 4),
		SpanView<uint32_t>(_data->indexes.data() + firstIndex, 6),
				firstVertex, firstIndex});
}

VertexArray::Quad VertexArray::getQuad(size_t firstVertex, size_t firstIndex) {
	if (_copyOnWrite) {
		copy();
	}

	return Quad({
			SpanView<gl::Vertex_V4F_V4F_T2F2U>(_data->data.data() + firstVertex, 4),
			SpanView<uint32_t>(_data->indexes.data() + firstIndex, 6),
					firstVertex, firstIndex});
}

void VertexArray::updateColor(const Color4F &color) {
	if (_copyOnWrite) {
		copy();
	}

	for (auto &it : _data->data) {
		it.color = color;
	}
}

void VertexArray::copy() {
	if (_copyOnWrite) {
		auto data = Rc<gl::VertexData>::alloc();
		data->data = _data->data;
		data->indexes = _data->indexes;
		_data = data;
		_copyOnWrite = false;
	}
}

}
