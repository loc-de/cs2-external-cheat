#pragma once
#include "../../game/context.h"
#include "../../../render/draw.h"
#include "../../../core/types.h"
#include "../../../core/config/config.h"

namespace features {

	class Esp {
	public:
		explicit Esp();

		void run(const game::Context&, render::DrawList&, const core::Extent&);

	private:
		struct BoundingBox {
			core::Vec2 pos;
			core::Vec2 size;
			bool valid;
		};

		BoundingBox calcBoundingBox(const game::Entity&, const game::LocalPlayer&, const core::Extent&);
		void drawBox(const BoundingBox&, const core::Color&, render::DrawList&);
		void drawHealthBar(const BoundingBox&, int, render::DrawList&);

		const core::config::EspConfig& _cfg;
	};

}