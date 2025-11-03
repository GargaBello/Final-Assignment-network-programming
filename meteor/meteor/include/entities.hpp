//entities.hpp

#pragma once

#include "raylib.h"
#include "network.hpp"

namespace meteor {

	struct entity {

		void set_position(Vector2 position);
		void set_hit(bool hit);

		Vector2 m_position = { 0,0 };
		bool	m_hit = false;
	};

	struct player : entity {

		void set_cooldown(float cd);

		float m_cooldown = 0;
		uint32 m_id = 0;
	};

	struct bomb : entity {

		void set_size(Rectangle rec);
		void set_timer(float timer);

		uint32     m_id = 0;
		float	  m_explosion_timer = 0;
		Rectangle m_size_rec = { 0, 0, 0, 0 };
	};

	struct terrain : entity {

		int RECTANGLE_WIDTH = 10, RECTANGLE_HEIGHT = 10;

		void set_size(Rectangle rec);
		void set_hit(bool hit);

		bool m_hit = false;
		Vector2 m_origin = {0,0};
		Rectangle m_size_rec = { 0,0,0,0 };
	};

	std::vector<player>  m_all_players;
	std::vector<bomb>    m_all_bombs;
	std::vector<terrain> m_all_terrain;


	void create_bomb(uint8 id, Vector2 position);

	void create_player(uint32 id);
}
