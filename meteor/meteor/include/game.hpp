//game.hpp

#pragma once

#include "network.hpp"
#include "raylib.h"
#include "entities.hpp"

namespace meteor {
	struct game {
		int MAP_WIDTH = 900, MAP_HEIGHT = 900, MAP_X_POSITION = 20, MAP_Y_POSITION = 20, SPAWN_AREA_OFFSET = 8;

		void set_map_size(Rectangle rec);
		void create_terrain(int width, int height, int posx, int posy, int offset);

		void init();
		void update();
		void draw();


		Rectangle m_background_map = { 0, 0, 0, 0 };

		std::vector<std::vector<terrain>> all_terrain;
	};
}
