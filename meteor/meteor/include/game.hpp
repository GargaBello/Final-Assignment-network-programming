//game.hpp

#pragma once

#include "network.hpp"
#include "raylib.h"
#include "entities.hpp"

namespace meteor {
	struct game {
		game() = default;
		int MAP_WIDTH = 900, MAP_HEIGHT = 900, MAP_X_POSITION = 20, MAP_Y_POSITION = 20, SPAWN_AREA_OFFSET = 8;
		int MAX_PLAYERS = 4;

		void set_map_size(Rectangle rec);
		void create_terrain() {
			for (int x = MAP_X_POSITION += SPAWN_AREA_OFFSET; x < MAP_WIDTH; x += 10) {
				std::vector<terrain> vector = std::vector<terrain>();

				for (int y = MAP_Y_POSITION += SPAWN_AREA_OFFSET; y < MAP_HEIGHT; y += 10) {
					terrain ter;
					ter.m_origin.x = (float)x;
					ter.m_origin.y = (float)y;
					vector.push_back(ter);
				}

				m_terrain.push_back(vector);
			}
		}

		void init() {
			create_terrain();
		}

		void update() {
			while (!WindowShouldClose()) {
				m_tick += 1;


				draw();
			}
		}

		void draw() {
			for (int i = 0; i < m_terrain.size(); i++) {
				for (int j = 0; j < m_terrain[i].size(); j++) {
					Rectangle rec = { m_terrain[i][j].m_origin.x, m_terrain[i][j].m_origin.y, (float)m_terrain[i][j].RECTANGLE_WIDTH, (float)m_terrain[i][j].RECTANGLE_HEIGHT };
					m_terrain[i][j].set_size(rec);
					DrawRectanglePro(m_terrain[i][j].m_size_rec, m_terrain[i][j].m_origin, 0, GRAY);
				}
			}
		}

		int32 m_tick = 0;

		Rectangle m_background_map = { 0, 0, 0, 0 };

		std::vector<entity> m_entities;
		std::vector<std::vector<terrain>> m_terrain;
		
	};
}
