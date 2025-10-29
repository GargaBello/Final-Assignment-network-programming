//game.cpp

#include "game.hpp"

namespace meteor {

	void set_map_size(Rectangle rec) {

	}

	void create_terrain(int width, int height, int posx, int posy, int offset, game game) {
		for (int x = posx += offset; x < width; x++) {
			std::vector<terrain> vector = std::vector<terrain>();

			for (int y = posy += offset; y < height; y++) {
				terrain ter;
				vector.push_back(ter);
			}
			
			game.m_terrain.push_back(vector);
		}
	}

	void init() {
		game game; 

		create_terrain(game.MAP_WIDTH, game.MAP_HEIGHT, game.MAP_X_POSITION, game.MAP_Y_POSITION, game.SPAWN_AREA_OFFSET, game);
	}

	void update() {
		
	}

	void draw() {
		
	}
}