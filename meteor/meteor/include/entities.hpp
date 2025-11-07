//entities.hpp

#pragma once

#include "raylib.h"
#include "network.hpp"
#include "helper_functions.hpp"

namespace meteor {

	struct entity {

		Vector2 m_position = { 0,0 };
		Vector2 m_terrain_map_pos = { 0, 0 };
		bool	m_hit = false;
	};

	struct player : entity {

		float RECTANGLE_SIDE_LENGTH = 80;

		player() = default;
		player(Vector2 position);

		enum class action : uint8 {
			INVALID,
			STAND_STILL,
			MOVE_RIGHT,
			MOVE_LEFT,
			MOVE_UP,
			MOVE_DOWN,
			PLACE_BOMB
		};

		float m_cooldown = 0;
		uint32 m_id = 0;
		uint32 m_prev_action_tick = 0;
		action m_prev_action = action::INVALID;
		action m_action = action::INVALID;
		action m_predict_action = action::INVALID;

		Vector2 m_origin = { 0,0 };

#ifdef _CLIENT

		bool is_player_character = false;

#endif // _CLIENT


		int pos_index[1][1] = {};
	};

	struct bomb : entity {

		float RECTANGLE_SIDE_LENGTH = 80;

		static constexpr double FUSE_TIME = 3.0;
		static constexpr uint32 FUSE_TICKS = (uint32)(FUSE_TIME * (double)TICK_RATE);
		static constexpr uint32 COOLDOWN_TICKS = (uint32)(1.0 * (double)TICK_RATE);

		bomb() = default;
		bomb(uint8 x, uint8 y, uint32 explosion_tick);

		uint8 
			m_x = 0, 
			m_y = 0;
		uint32    m_id = 0;
		uint32    m_explosion_tick = 0;
		uint32    m_cooldown = 0;

		int pos_index[1][1] = {};
	};

	struct terrain : entity {

		float RECTANGLE_SIDE_LENGTH = 100;

		bool m_hit = false;
		Vector2 m_origin = {0,0};
		Rectangle m_size_rec = { 0,0,0,0 };

		Vector2 m_center_of_pos = { m_origin.x + 5, m_origin.y + 5 };
	};

	void create_bomb(uint8 id, Vector2 position);

	void create_player(uint32 id);
}
