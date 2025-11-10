//game.hpp

#pragma once

#include "network.hpp"
#include "raylib.h"
#include "helper_functions.hpp"
#include "client_controller.hpp"

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


	/*static bool valid_tile(const uint8 x, const uint8 y) {
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
	}*/

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

		void const draw() {
			DrawRectangleRec(m_background_map, DARKGREEN);
			DrawRectangleLinesEx(m_background_map, (float)LINE_THICKNESS, BLACK);
		}
	};

	struct terrain_map : background {

		terrain_map() = default;
		static const int ARRAY_WIDTH  = 6,
				         ARRAY_HEIGHT = 6;

		Vector2 
			PLAYER_ONE_START_INDEX = { 0, 0 },
			PLAYER_TWO_START_INDEX = { ARRAY_WIDTH - 1, 0 },
			PLAYER_THREE_START_INDEX = {0, ARRAY_HEIGHT - 1},
			PLAYER_FOUR_START_INDEX = { ARRAY_WIDTH - 1, ARRAY_HEIGHT - 1};


		const float LINE_OFFSET = 10,
			        TERRAIN_OFFSET = 5;

		bool m_terrain_map_bools[ARRAY_WIDTH][ARRAY_WIDTH] =
		{{false, false, true, true, false, false}
		,{false, true,  true, true, true,  false}
		,{true,  true,  true, true, true,  true	}
		,{true,  true,  true, true, true,  true	}
		,{false, true,  true, true, true,  false}
		,{false, false, true, true, false, false}};

		terrain m_terrain_map[ARRAY_WIDTH][ARRAY_WIDTH] = {};

		void create_terrain_map() {
			for (int x = 0; x < ARRAY_WIDTH; x++) {
				for (int y = 0; y < ARRAY_HEIGHT; y++) {
					if (m_terrain_map_bools[x][y]) {
						terrain terrain;
						terrain.m_hit = false;
						terrain.m_origin = { ((MAP_POS.x) + ((float)x) * terrain.RECTANGLE_SIDE_LENGTH), ( (MAP_POS.y) + ((float)y) * terrain.RECTANGLE_SIDE_LENGTH)};
						terrain.m_center_of_pos = { terrain.m_origin.x + LINE_OFFSET, terrain.m_origin.y + LINE_OFFSET };
						terrain.m_size_rec = { terrain.m_origin.x, terrain.m_origin.y, terrain.RECTANGLE_SIDE_LENGTH - TERRAIN_OFFSET, terrain.RECTANGLE_SIDE_LENGTH - TERRAIN_OFFSET };
						m_terrain_map[x][y] = terrain;
					}
					else if (!m_terrain_map_bools[x][y]) {
						terrain terrain;
						terrain.m_hit = true;
						terrain.m_origin = { ((MAP_POS.x) + ((float)x) * terrain.RECTANGLE_SIDE_LENGTH), ((MAP_POS.y) + ((float)y) * terrain.RECTANGLE_SIDE_LENGTH) };
						terrain.m_center_of_pos = { terrain.m_origin.x + LINE_OFFSET, terrain.m_origin.y + LINE_OFFSET };
						terrain.m_size_rec = { terrain.m_origin.x, terrain.m_origin.y, terrain.RECTANGLE_SIDE_LENGTH - TERRAIN_OFFSET, terrain.RECTANGLE_SIDE_LENGTH - TERRAIN_OFFSET };
						m_terrain_map[x][y] = terrain;
					}
				}
			}
		}

		bool tile_active(int x, int y, terrain_map map);
	};

	struct snapshot {

		snapshot() = default;
		snapshot(uint32 tick,
			player players[MAX_PLAYERS],
			bomb bombs[MAX_PLAYERS],
			terrain_map map,
			uint8 status);

		uint32				 m_tick = 0;
		player               m_players[MAX_PLAYERS];
		bomb                 m_bombs[MAX_PLAYERS];
		terrain_map          m_map = {};
		uint8				 m_status = 0;


	};

	struct snapshot_queue {
		const uint8 MAX_SNAPSHOTS = 30;

		snapshot_queue() = default;

		std::vector<snapshot>  m_snapshots;

		void create_snapshot(uint32 tick);

		void update();
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
		terrain_map   m_map = {};

		const player& get_player(const int index) const;
		const bomb& get_bomb(const int index) const;

		const terrain_map& get_map() const;

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

		void init() {
			m_map.create_terrain_map();

			player default_player;
			bomb   default_bomb;

			default_player.m_hit = true;
			default_bomb.m_hit = true;
			default_bomb.m_explosion_tick = default_bomb.FUSE_TICKS;
			for (int i = 0; i < MAX_PLAYERS; i++) {
				m_players[i] = default_player;
				m_players[i].m_hit = false;
				m_bombs[i] = default_bomb;

				switch (i)
				{
				case 0:
					m_players[i].m_position = m_map.m_terrain_map[(int)m_map.PLAYER_ONE_START_INDEX.x][(int)m_map.PLAYER_ONE_START_INDEX.y].m_center_of_pos;
					break;
				case 1:
					m_players[i].m_position = m_map.m_terrain_map[(int)m_map.PLAYER_TWO_START_INDEX.x][(int)m_map.PLAYER_TWO_START_INDEX.y].m_center_of_pos;
					break;
				case 2:
					m_players[i].m_position = m_map.m_terrain_map[(int)m_map.PLAYER_THREE_START_INDEX.x][(int)m_map.PLAYER_THREE_START_INDEX.y].m_center_of_pos;
					break;
				case 3:
					m_players[i].m_position = m_map.m_terrain_map[(int)m_map.PLAYER_FOUR_START_INDEX.x][(int)m_map.PLAYER_FOUR_START_INDEX.y].m_center_of_pos;
					break;
				default:
					break;
				}
			}

			m_status = game::status::PRE_GAME;
		}

		void update() {

			if (m_status == game::status::PRE_GAME) {
				m_tick++;


				#ifdef _SERVER

				for (int i = 0; i < MAX_CLIENTS; i++) {
					if (m_clients[i].m_connection.m_id > 0) {
						m_players[i].m_id = m_clients[i].m_connection.m_id;
					}
				}

				int counter = 0;
				for (const player& player : m_players) {
					if (player.m_id > 0) {
						counter++;
					}
				}

				if (counter > 2) {
					(int)m_thirty_tick_seconds--;
				}

				if (m_thirty_tick_seconds <= 0) {
					m_status = game::status::IN_GAME;
					m_tick = 0;
				}

				if (IsKeyPressed(KEY_L)) {
					m_status = game::status::IN_GAME;
					m_tick = 0;
				}


				debug::info("Tick check");

				#endif // _SERVER
			}
			else if (m_status == game::status::IN_GAME) {
				m_tick++;


				#ifdef _CLIENT

				for (int p = 0; p < MAX_PLAYERS; p++) {
					if (m_players[p].is_player_character == true) {
						if (IsKeyDown(KEY_W)) {
							m_players[p].m_action = player::action::MOVE_UP;
							m_players[p].m_predict_action = player::action::MOVE_UP;
						}
						else if (IsKeyDown(KEY_S)) {
							m_players[p].m_action = player::action::MOVE_DOWN;
							m_players[p].m_predict_action = player::action::MOVE_DOWN;
						}
						else if (IsKeyDown(KEY_A)) {
							m_players[p].m_action = player::action::MOVE_RIGHT;
							m_players[p].m_predict_action = player::action::MOVE_RIGHT;
						}
						else if (IsKeyDown(KEY_D)) {
							m_players[p].m_action = player::action::MOVE_LEFT;
							m_players[p].m_predict_action = player::action::MOVE_LEFT;
						}
						else if (IsKeyPressed(KEY_V)) {
							m_players[p].m_action = player::action::PLACE_BOMB;
							m_players[p].m_predict_action = player::action::PLACE_BOMB;
						}
					}
				}
				#endif // _CLIENT

				for (int b = 0; b < MAX_PLAYERS; b++) {
					if (m_bombs[b].m_hit == false) {
						m_bombs[b].m_explosion_tick--;

						if (m_bombs[b].m_cooldown >= 0) {
							m_bombs[b].m_cooldown--;
						}
					}
				}

				for (int i = 0; i < MAX_PLAYERS; i++) {
					player::action action = m_players[i].m_action;

					switch (action)
					{
					case player::action::MOVE_UP: {
						if (m_map.tile_active((int)m_players[i].m_terrain_map_pos.x, (int)(m_players[i].m_terrain_map_pos.y - 1), m_map)) {
							m_players[i].m_action = player::action::STAND_STILL;
						}
						else if (bomb_in_way((int)(m_players[i].m_terrain_map_pos.x), (int)(m_players[i].m_terrain_map_pos.y - 1))) {
							m_players[i].m_action = player::action::STAND_STILL;
						}
						else {
							m_players[i].m_terrain_map_pos.y -= 1;
						}
						break;
					}
					case player::action::MOVE_DOWN: {
						if (m_map.tile_active((int)m_players[i].m_terrain_map_pos.x, (int)(m_players[i].m_terrain_map_pos.y + 1), m_map)) {
							m_players[i].m_action = player::action::STAND_STILL;
						}
						else if (bomb_in_way((int)(m_players[i].m_terrain_map_pos.x), (int)(m_players[i].m_terrain_map_pos.y + 1))) {
							m_players[i].m_action = player::action::STAND_STILL;
						}
						else {
							m_players[i].m_terrain_map_pos.y += 1;
						}
						break;
					}
					case player::action::MOVE_RIGHT: {
						if (m_map.tile_active((int)(m_players[i].m_terrain_map_pos.x + 1), (int)m_players[i].m_terrain_map_pos.y, m_map)) {
							m_players[i].m_action = player::action::STAND_STILL;
						}
						else if (bomb_in_way((int)(m_players[i].m_terrain_map_pos.x + 1), (int)m_players[i].m_terrain_map_pos.y)) {
							m_players[i].m_action = player::action::STAND_STILL;
						}
						else {
							m_players[i].m_terrain_map_pos.x += 1;
						}
						break;
					}
					case player::action::MOVE_LEFT: {
						if (m_map.tile_active((int)(m_players[i].m_terrain_map_pos.x - 1), (int)m_players[i].m_terrain_map_pos.y, m_map)) {
							m_players[i].m_action = player::action::STAND_STILL;
						}
						else if (bomb_in_way((int)(m_players[i].m_terrain_map_pos.x - 1), (int)m_players[i].m_terrain_map_pos.y)) {
							m_players[i].m_action = player::action::STAND_STILL;
						}
						else {
							m_players[i].m_terrain_map_pos.x -= 1;
						}
						break;
					}
					case player::action::PLACE_BOMB: {
						if (can_place_bomb(m_players[i])) {
							for (int j = 0; j < MAX_PLAYERS; j++) {
								if (m_players[i].m_id == m_bombs[j].m_id) {
									m_bombs[j].m_position = m_players[i].m_position;
									m_bombs[j].m_hit = false;
								}
							}
						}
						break;
					}
					case player::action::STAND_STILL: {

						break;
					}
					default:
					{

						break;
					}
					}
				}

				#ifdef _SERVER



				for (int i = 0; i < MAX_PLAYERS; i++) {

					m_players[i].m_position = m_map.m_terrain_map[(int)m_players[i].m_terrain_map_pos.x][(int)m_players[i].m_terrain_map_pos.y].m_center_of_pos;

					if ((int)m_bombs[i].m_explosion_tick <= 0) {
						//bomb will explode
						if (!m_bombs[i].m_hit) {
							bomb_explodes((int)m_bombs[i].m_position.x, (int)m_bombs[i].m_position.y);
							m_bombs[i].m_cooldown = m_bombs[i].COOLDOWN_TICKS;
							m_bombs[i].m_hit = true;
						}
						m_bombs[i].m_explosion_tick = m_bombs[i].FUSE_TICKS;
					}
				}

				snapshot shot(m_tick, m_players, m_bombs, m_map);

				#endif // _SERVER

				/*	todo make function that checks the rows a bomb was on when exploding and apply damage to whatever the explosion hit
				*	make client
				*   client send receive
				*	client prediction
				*   client reconciliation
				*	client interpolation
				*
				*   lobby
				*	make game start 60 seconds after first 2 clients join
				*	check if packets can send and shit
				*
				*	overhaul snapshots
				*/


				draw();
			}
		}

		void draw() {
			m_background.draw();

			for (int i = 0; i < MAX_PLAYERS; i++) {
				if (m_bombs[i].m_explosion_tick > 0 && !m_bombs[i].m_hit) {
					DrawRectangle((int)m_bombs[i].m_position.x, (int)m_bombs[i].m_position.y, (int)m_bombs[i].RECTANGLE_SIDE_LENGTH, (int)m_bombs[i].RECTANGLE_SIDE_LENGTH, BLACK);
				}
			}

			for (int i = 0; i < MAX_PLAYERS; i++) {
				if (m_players[i].m_hit == false) {
					DrawRectangle((int)m_players[i].m_position.x, (int)m_players[i].m_position.y, (int)m_players[i].RECTANGLE_SIDE_LENGTH, (int)m_players[i].RECTANGLE_SIDE_LENGTH, RED);
				}
			}

			for (int i = 0; i < m_map.ARRAY_WIDTH; i++) {
				for (int j = 0; j < m_map.ARRAY_HEIGHT; j++) {
					if (!m_map.m_terrain_map[i][j].m_hit) {
						DrawRectangleRec(m_map.m_terrain_map[i][j].m_size_rec, DARKBROWN);
					}
				}
			}

		}

		bool can_place_bomb(player player) {
			bool r = true;

			r &= !player.m_hit;

			for (int i = 0; i < MAX_PLAYERS; i++) {
				if (player.m_id == m_bombs[i].m_id) {
					if (!m_bombs[i].m_hit) {
						m_bombs[i].m_position = player.m_position;
						m_bombs[i].m_hit = false;
						m_bombs[i].m_explosion_tick = m_bombs[i].FUSE_TICKS;
					}
				}
			}

			

			return r;
		}

		bool bomb_in_way(int x, int y) const {
			bool bomb_in_way = true;
			for (int i = 0; i < MAX_PLAYERS; i++) {
				if (x == (int)m_bombs[i].m_position.x && y == (int)m_bombs[i].m_position.y) {
					bomb_in_way = true;
				}
				else {
					bomb_in_way = false;
				}
			}
			return bomb_in_way;
		}

		void bomb_explodes(int x, int y) {
			for (int i = x; i < m_map.ARRAY_WIDTH; i++) {
				if (m_map.tile_active(i, y, m_map)) {
					m_map.m_terrain_map[i][y].m_hit = true;

					for (int k = 0; k < MAX_PLAYERS; k++) {
						if ((int)m_players[k].m_position.x == i && (int)m_players[k].m_position.y == y) {
							m_players[k].m_hit = true;
							break;
						}
					}
				}
			}

			for (int j = y; j < m_map.ARRAY_HEIGHT; j++) {
				if (m_map.tile_active(x, j, m_map)) {
					m_map.m_terrain_map[x][j].m_hit = true;

					for (int o = 0; o < MAX_PLAYERS; o++) {
						if ((int)m_players[o].m_position.x == x && (int)m_players[o].m_position.y == j) {
							m_players[o].m_hit = true;
							break;
						}
					}
				}
			}
		}

		player        m_players[MAX_PLAYERS] = {};
		bomb		  m_bombs[MAX_PLAYERS] = {};
		background	  m_background;
		terrain_map   m_map;
		snapshot      m_snapshot;

		uint32                            m_tick = 0;
		uint32						      m_thirty_tick_seconds = 1800;
		status                            m_status = status::INVALID;
	};
}
