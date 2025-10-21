//game.hpp

#pragma once

#include "network.hpp"
#include "raylib.h"
#include "entities.hpp"

namespace meteor {
	struct game {
		int MAP_WIDTH = 900, MAP_HEIGHT = 900, MAP_X_POSITION = 0, MAP_Y_POSITION = 0;

		void set_map_size(Rectangle rec);

		void init();
		void update();
		void draw();


		Rectangle m_background_map = { 0, 0, 0, 0 };
	};
}
