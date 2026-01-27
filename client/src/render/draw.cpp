#include "draw.h"

namespace render {

	void DrawList::clear() {
		_rects.clear();
		_lines.clear();
	}

	void DrawList::addLine(const core::Vec2& a, const core::Vec2& b, const core::Color& c) {
		_lines.push_back({ a, b, c });
	}

	void DrawList::addBox(const core::Rect& r, const core::Color& c) {
		addLine({ r.x, r.y }, { r.x + r.w, r.y }, c);
		addLine({ r.x, r.y + r.h }, { r.x + r.w, r.y + r.h }, c);
		addLine({ r.x, r.y }, { r.x, r.y + r.h }, c);
		addLine({ r.x + r.w, r.y }, { r.x + r.w, r.y + r.h }, c);
	}

	void DrawList::addRect(const core::Rect& r, const core::Color& c) {
		_rects.push_back(RectCmd{ r, c });
	}

}