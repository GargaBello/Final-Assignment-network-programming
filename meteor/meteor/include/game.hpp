//game.hpp

#pragma once

#include "network.hpp"
#include "raylib.h"
#include "entities.hpp"
#include "helper_functions.hpp"

namespace meteor {

	constexpr uint8
		MAP_WIDTH         = 16,
		MAP_HEIGHT        = 16,
		MAP_X_POSITION    = 20,
		MAP_Y_POSITION    = 20,
		SPAWN_AREA_OFFSET = 8;

	constexpr double
		BOMB_FUSE_TIME = 3.0,
		BOMB_PLACE_INTERVAL_MIN = 1.0;

	constexpr uint32
		BOMB_FUSE_TICKS = (uint32)(BOMB_FUSE_TIME * (double)TICK_RATE),
		BOMB_COOLDOWN_TICKS = (uint32)(1 * (double)TICK_RATE),
		NAME_LENGTH_MAX = 16;

	constexpr int
		MAX_PLAYERS = 4,
		ACTIONS_BUFFER_LENGTH = 12,
		STATE_HISTORY_LENGHT = 30;

	static void vec2_to_tile(const Vector2& pos, uint8& x, uint8& y) {
		uint8 new_x = (uint8)pos.x;
		uint8 new_y = (uint8)pos.y;

		x = new_x;
		y = new_y;
	}


	static bool valid_tile(const uint8 x, const uint8 y) {
		if (x >= tilemap::WIDTH
			|| y >= tilemap::HEIGHT
			|| (x + y * tilemap::WIDTH) >= tilemap::COUNT) return false;
		else return true;
	}

	static uint32 coord_to_index(const uint8 x, const uint8 y) {
		if (!valid_tile(x, y)) return UINT32_MAX;
		return (x + y * tilemap::WIDTH);
	}

	static void index_to_coord(const uint32 index, uint8& x, uint8& y) {
		if (index >= tilemap::COUNT) return;
		y = (uint8)index / tilemap::WIDTH;
		x = (uint8)index % tilemap::WIDTH;
	}

	enum class player_actions : uint8 {
		INVALID,
		STAND_STILL,
		MOVE_RIGHT,
		MOVE_LEFT,
		MOVE_UP,
		MOVE_DOWN,
		PLACE_BOMB
	};

	struct background {

		background() = default;

		const int BACKGROUND_WIDTH  = 600,
				  BACKGROUND_HEIGHT = 600,
				  LINE_THICKNESS    = 5;
		
		const Vector2 MAP_POS = { 250, 100 };

		Rectangle m_background_map = { MAP_POS.x, MAP_POS.y, (float)BACKGROUND_WIDTH, (float)BACKGROUND_HEIGHT };

		void draw() {
			DrawRectangleRec(m_background_map, DARKGREEN);
			DrawRectangleLinesEx(m_background_map, (float)LINE_THICKNESS, BLACK);
		}
	};

	struct terrain_map : background {

		terrain_map() = default;
		const float ARRAY_WIDTH  = 6.0f,
				    ARRAY_HEIGHT = 6.0f;

		const float LINE_OFFSET = 10,
			        TERRAIN_OFFSET = 5;

		bool m_terrain_map_bools[6][6] = 
		{{false, false, true, true, false, false}
		,{false, true,  true, true, true,  false}
		,{true,  true,  true, true, true,  true	}
		,{true,  true,  true, true, true,  true	}
		,{false, true,  true, true, true,  false}
		,{false, false, true, true, false, false}};

		terrain m_terrain_map[6][6] = {};

		void create_terrain_map() {
			for (int x = 0; x < ARRAY_WIDTH; x++) {
				for (int y = 0; y < ARRAY_HEIGHT; y++) {
					if (m_terrain_map_bools[x][y]) {
						terrain terrain;
						terrain.m_hit = false;
						terrain.m_origin = { ((MAP_POS.x) + ((float)x) * terrain.RECTANGLE_SIDE_LENGTH), ( (MAP_POS.y) + ((float)y) * terrain.RECTANGLE_SIDE_LENGTH)};
						terrain.m_size_rec = { terrain.m_origin.x, terrain.m_origin.y, terrain.RECTANGLE_SIDE_LENGTH - TERRAIN_OFFSET, terrain.RECTANGLE_SIDE_LENGTH - TERRAIN_OFFSET };
						m_all_terrain.push_back(terrain);
					}
				}
			}
		}
	};

	struct tilemap {
		static constexpr uint32 TILE_SIZE = 32;
		static constexpr uint8
			WIDTH = 16,
			HEIGHT = 16;
		static constexpr Vector2 SIZE_V = Vector2(WIDTH, HEIGHT);

		static constexpr int COUNT = WIDTH * HEIGHT;
		static constexpr int TILEMAP_BYTES =
			WIDTH * HEIGHT / 8
			+ (((WIDTH * HEIGHT) % 8) == 0 ? 0 : 1);


		tilemap() = default;

		uint8 m_tiles[TILEMAP_BYTES];

		bool is_tile_active(const uint8 x, const uint8 y) const;
		bool is_tile_active(const uint32 index) const;

		void set_tile(const uint8 x, const uint8 y, bool value);

		
	};

	struct game_state {
		game_state() = default;

		player        m_players[MAX_PLAYERS] = {};
		bomb		  m_bombs[MAX_PLAYERS] = {};
		tilemap		  m_tilemap = {};

		const player& get_player(const int index) const;
		const bomb& get_bomb(const int index) const;

		const tilemap& get_tilemap() const;

		const bool is_default() const;
	};


	struct game {
		static constexpr int
		ACTIONS_BUFFER_LENGTH = 12,
			STATE_HISTORY_LENGHT = 30;

		enum class status : uint8 {
			INVALID,
			PRE_GAME,
			IN_GAME,
			POST_GAME
		};

		game() = default;

		void set_map_size(Rectangle rec);
		/*void create_terrain() {
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
		}*/

		void init() {
			//create_terrain();
			m_map.create_terrain_map();
		}

		void update() {
			m_tick++;

			draw();
		}

		void draw() {
			m_background.draw();

			for (int i = 0; i < m_all_bombs.size(); i++) {
				if (m_all_bombs[i].m_explosion_tick > 0) {
					DrawRectangle(m_all_bombs[i].m_position.x, m_all_bombs[i].m_position.y, m_all_bombs[i].RECTANGLE_SIDE_LENGTH, m_all_bombs[i].RECTANGLE_SIDE_LENGTH);
				}
			}

			for (int i = 0; i < m_all_players.size(); i++) {
				if (m_all_players[i].m_hit == false) {
					DrawRectangle(m_all_players[i].m_position.x, m_all_players[i].m_position.y, m_all_players[i].RECTANGLE_SIDE_LENGTH, m_all_players[i].RECTANGLE_SIDE_LENGTH, RED);
				}
			}

			for (int i = 0; i < m_all_terrain.size(); i++) {
				if (m_all_terrain[i].m_hit == false) {
					DrawRectangleRec(m_all_terrain[i].m_size_rec, DARKBROWN);
				}
			}

		}

		std::vector<entity>				  m_entities;
		std::vector<std::vector<terrain>> m_terrain;
		background						  m_background;
		terrain_map                       m_map;
		uint32                            m_tick;
		status                            m_status = status::INVALID;
	};
}
