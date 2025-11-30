//game.hpp

#pragma once

#include "network.hpp"
#include "raylib.h"
#include "raymath.h"
#include "helper_functions.hpp"
#include "client_controller.hpp"
#include <string>

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
		BOMB_COOLDOWN_TICKS = (uint32)(1 * (double)TICK_RATE);

	constexpr int
		MAX_PLAYERS = 4;

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
			bool terrain_hits[6][6],
			uint8 status);

		uint32				 m_tick = 0;
		player               m_players[MAX_PLAYERS];
		bomb                 m_bombs[MAX_PLAYERS];
		bool				 m_terrain_hits[6][6] = {};
		uint8				 m_status = 0;


	};

	struct snapshot_queue {
		const uint8 MAX_SNAPSHOTS = 30;

		snapshot_queue() = default;

		std::vector<snapshot>  m_snapshots;

	};



	struct game {

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
				m_players[i].m_hit = true;
				m_bombs[i] = default_bomb;
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
						m_players[i].m_hit = false;
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
				double current_time = GetTime();


				#ifdef _CLIENT

				for (int p = 0; p < MAX_PLAYERS; p++) {
					if (m_players[p].is_player_character == true) {

						//m_players[p].m_prev_position = m_players[p].m_position;

						if (IsKeyPressed(KEY_W)) {
							m_players[p].m_action = player::action::MOVE_UP;
							m_players[p].m_predict_action = player::action::MOVE_UP;
						}
						else if (IsKeyPressed(KEY_S)) {
							m_players[p].m_action = player::action::MOVE_DOWN;
							m_players[p].m_predict_action = player::action::MOVE_DOWN;
						}
						else if (IsKeyPressed(KEY_D)) {
							m_players[p].m_action = player::action::MOVE_RIGHT;
							m_players[p].m_predict_action = player::action::MOVE_RIGHT;
						}
						else if (IsKeyPressed(KEY_A)) {
							m_players[p].m_action = player::action::MOVE_LEFT;
							m_players[p].m_predict_action = player::action::MOVE_LEFT;
						}
						else if (IsKeyPressed(KEY_V)) {
							m_players[p].m_action = player::action::PLACE_BOMB;
							m_players[p].m_predict_action = player::action::PLACE_BOMB;
						}
						else if (IsKeyPressed(KEY_M)) {
							m_timeout_check = true;
						}
					}
				}
				#endif // _CLIENT

				for (int b = 0; b < MAX_PLAYERS; b++) {
					if (m_bombs[b].m_hit == false) {
						m_bombs[b].m_explosion_tick--;
					}

					if (m_bombs[b].m_cooldown >= 0) {
						m_bombs[b].m_cooldown--;
					}
				}

				for (int i = 0; i < MAX_PLAYERS; i++) {
					player::action action = m_players[i].m_action;

					bool player_moved = false;  

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
							m_players[i].m_position = m_map.m_terrain_map
								[(int)m_players[i].m_terrain_map_pos.x]
								[(int)m_players[i].m_terrain_map_pos.y].m_center_of_pos;
							player_moved = true;
							m_players[i].m_action = player::action::STAND_STILL;
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
							m_players[i].m_position = m_map.m_terrain_map
								[(int)m_players[i].m_terrain_map_pos.x]
								[(int)m_players[i].m_terrain_map_pos.y].m_center_of_pos;
							player_moved = true;
							m_players[i].m_action = player::action::STAND_STILL;
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
							m_players[i].m_position = m_map.m_terrain_map
								[(int)m_players[i].m_terrain_map_pos.x]
								[(int)m_players[i].m_terrain_map_pos.y].m_center_of_pos;
							player_moved = true;
							m_players[i].m_action = player::action::STAND_STILL;
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
							m_players[i].m_position = m_map.m_terrain_map
								[(int)m_players[i].m_terrain_map_pos.x]
								[(int)m_players[i].m_terrain_map_pos.y].m_center_of_pos;
							player_moved = true;
							m_players[i].m_action = player::action::STAND_STILL;

						}
						break;
					}
					case player::action::PLACE_BOMB: {
						if (can_place_bomb(m_players[i])) {
							for (int j = 0; j < MAX_PLAYERS; j++) {
								if (m_bombs[j].m_hit || m_bombs[j].m_id == 0) {
									m_bombs[j].m_id = m_players[i].m_id;
									m_bombs[j].m_position = m_players[i].m_position;
									m_bombs[j].m_terrain_map_pos = m_players[i].m_terrain_map_pos;
									m_bombs[j].m_hit = false;
									m_bombs[j].m_explosion_tick = m_bombs[j].FUSE_TICKS;
									m_players[i].m_action = player::action::STAND_STILL;
									break; 
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


				for (int i = 0; i < MAX_PLAYERS; i++) {
					if ((int)m_bombs[i].m_explosion_tick <= 0) {
						if (!m_bombs[i].m_hit) {
							bomb_explodes((int)m_bombs[i].m_terrain_map_pos.x, (int)m_bombs[i].m_terrain_map_pos.y);
							m_bombs[i].m_cooldown = m_bombs[i].COOLDOWN_TICKS;
							m_bombs[i].m_hit = true;
						}
						m_bombs[i].m_explosion_tick = m_bombs[i].FUSE_TICKS;
					}
				}


				snapshot shot;

				bool terrain_hits[6][6];
				for (int x = 0; x < m_map.ARRAY_WIDTH; x++) {
					for (int y = 0; y < m_map.ARRAY_HEIGHT; y++) {
						terrain_hits[x][y] = m_map.m_terrain_map[x][y].m_hit;
						shot.m_terrain_hits[x][y] = terrain_hits[x][y];
					}
				}


				for (int i = 0; i < MAX_PLAYERS; i++) {

					shot.m_players[i] = m_players[i];
					shot.m_bombs[i] = m_bombs[i];
				}


				shot.m_tick = m_tick;
				shot.m_status = (uint8)m_status;
				m_snapshot = shot;

				m_queue.m_snapshots.insert(m_queue.m_snapshots.begin(), shot);

				if (m_queue.m_snapshots.size() > m_queue.MAX_SNAPSHOTS) {
					m_queue.m_snapshots.pop_back();
				}
				
			}
			else if (m_status == game::status::POST_GAME) {

				for (int i = 0; i < MAX_PLAYERS; i++) {


					m_players[i].m_hit = true;
					m_bombs[i].m_hit = true;

					
				}

				for (int i = 0; i < m_map.ARRAY_WIDTH; i++) {
					for (int j = 0; j < m_map.ARRAY_HEIGHT; j++) {
						m_map.m_terrain_map[i][j].m_hit = true;
					}
				}
			}

			draw();
		}

		void draw() {
			m_background.draw();

			for (int i = 0; i < MAX_PLAYERS; i++) {
				if (m_bombs[i].m_hit == false && m_bombs[i].m_id != 0) {
					DrawRectangle((int)m_bombs[i].m_position.x, (int)m_bombs[i].m_position.y, (int)m_bombs[i].RECTANGLE_SIDE_LENGTH, (int)m_bombs[i].RECTANGLE_SIDE_LENGTH, BLACK);
				}
			}

			for (int i = 0; i < MAX_PLAYERS; i++) {
				if (m_players[i].m_hit == false) {
					double current_time = GetTime();
					double time_since_update = current_time - m_time_since_last_update;

					double interpolation_period = 10 / (double)TICK_RATE;

					double lerp_fraction = (float)time_since_update / interpolation_period;
					lerp_fraction = Clamp((float)lerp_fraction, 0.0f, 1.0f);

					if (m_players[i].is_player_character) {
						m_players[i].m_prev_position = m_players[i].m_position;
					}

					
					Vector2 render_pos = Vector2Lerp(
						m_players[i].m_prev_position,  
						m_players[i].m_position,        
						(float)lerp_fraction            
					);

					if (lerp_fraction >= 1.0f && !m_players[i].is_player_character) {
						m_players[i].m_prev_position = m_players[i].m_position;
					}
					
					DrawRectangle(
						(int)render_pos.x,
						(int)render_pos.y,
						(int)m_players[i].RECTANGLE_SIDE_LENGTH,
						(int)m_players[i].RECTANGLE_SIDE_LENGTH,
						RED
					);
				}
			}

			for (int i = 0; i < m_map.ARRAY_WIDTH; i++) {
				for (int j = 0; j < m_map.ARRAY_HEIGHT; j++) {
					if (!m_map.m_terrain_map[i][j].m_hit) {
						DrawRectangleRec(m_map.m_terrain_map[i][j].m_size_rec, DARKBROWN);
					}
				}
			}

			std::string text = "RTT: " + sec_to_ms_str_pretty(m_rtt_time);
			DrawText(text.c_str(), m_background.BACKGROUND_WIDTH - 200, m_background.BACKGROUND_HEIGHT - 575, 30, BLACK);

			if (m_disconnected) {
				int good_spot_for_text = 300,
					half_of_window_height = 350;

				DrawText(m_disconnect_text.c_str(), good_spot_for_text, half_of_window_height, 50, BLACK);
			}

		}

		static std::string sec_to_ms_str_pretty(const double sec) {
			std::string r = "";
			r += std::to_string(sec * 1000);
			if (r.find('.') + 2 < r.size()) {
				r.erase(r.begin() + r.find('.') + 2, r.end());
			}
			r += "ms";
			return r;
		}

		bool can_place_bomb(player player) {
			if (player.m_hit) return false;

			
			for (int i = 0; i < MAX_PLAYERS; i++) {
				if (m_bombs[i].m_id == player.m_id && !m_bombs[i].m_hit) {
					return false; 
				}
			}

			return true;
		}

		bool bomb_in_way(int x, int y) const {
			bool bomb_in_way = false;
			for (int i = 0; i < MAX_PLAYERS; i++) {
				if ((int)m_bombs[i].m_hit == false) {
					if (x == (int)m_bombs[i].m_terrain_map_pos.x && y == (int)m_bombs[i].m_terrain_map_pos.y) {
						bomb_in_way = true;
					}
				}
			}
			return bomb_in_way;
		}

		void bomb_explodes(int x, int y) {
			for (int i = x; i < m_map.ARRAY_WIDTH; i++) {
				for (int k = 0; k < MAX_PLAYERS; k++) {
					if ((int)m_players[k].m_terrain_map_pos.x == i && (int)m_players[k].m_terrain_map_pos.y == y) {
						m_players[k].m_hit = true;
					}
				}

				if (m_map.tile_active(i, y, m_map)) {
					m_map.m_terrain_map[i][y].m_hit = true;
					break;
				}
			}

			for (int i = x; i > 0; i--) {
				for (int k = 0; k < MAX_PLAYERS; k++) {
					if ((int)m_players[k].m_terrain_map_pos.x == i && (int)m_players[k].m_terrain_map_pos.y == y) {
						m_players[k].m_hit = true;
					}
				}

				if (m_map.tile_active(i, y, m_map)) {
					m_map.m_terrain_map[i][y].m_hit = true;
					break;
				}
			}

			for (int j = y; j < m_map.ARRAY_HEIGHT; j++) {
				for (int o = 0; o < MAX_PLAYERS; o++) {
					if ((int)m_players[o].m_terrain_map_pos.x == x && (int)m_players[o].m_terrain_map_pos.y == j) {
						m_players[o].m_hit = true;
					}
				}

				if (m_map.tile_active(x, j, m_map)) {
					m_map.m_terrain_map[x][j].m_hit = true;
					break;
				}
			}

			for (int j = y; j > 0; j--) {
				for (int o = 0; o < MAX_PLAYERS; o++) {
					if ((int)m_players[o].m_terrain_map_pos.x == x && (int)m_players[o].m_terrain_map_pos.y == j) {
						m_players[o].m_hit = true;

					}
				}

				if (m_map.tile_active(x, j, m_map)) {
					m_map.m_terrain_map[x][j].m_hit = true;
					break;
				}
			}
		}

		bool go_to_post_game() {
			int dead_player_counter = 0;
			for (int i = 0; i < MAX_PLAYERS; i++) {

				if (m_players[i].m_hit) dead_player_counter++;
			}

			if (dead_player_counter >= 3) {
				m_status = status::POST_GAME;
			}

		}

		void reconciliation(player& Clientplayer, player& Serverplayer) {
			int distance_threshold = 1;

			Vector2 sum = Clientplayer.m_terrain_map_pos - Serverplayer.m_terrain_map_pos;

			if (abs(sum.x) > distance_threshold || abs(sum.y) > distance_threshold) {
				Clientplayer.m_terrain_map_pos = Serverplayer.m_terrain_map_pos;
				Clientplayer.m_position = Serverplayer.m_position;
				Clientplayer.m_prev_action = Serverplayer.m_prev_action;
			}
		}

		player							  m_players[MAX_PLAYERS] = {};
		bomb							  m_bombs[MAX_PLAYERS] = {};
		background						  m_background;
		terrain_map						  m_map;
		snapshot						  m_snapshot;
		snapshot_queue					  m_queue;

		uint32                            m_tick = 0;
		uint32						      m_thirty_tick_seconds = 1800;
		status                            m_status = status::INVALID;

		double                            m_rtt_time = 0;
		double                            m_last_update_time = 0.0;
		double                            m_time_since_last_update = 0.0;

		std::string                       m_disconnect_text = "";
		bool                              m_disconnected = false;
		bool                              m_timeout_check = false;
	};
}
