//entities.hpp

#pragma once

#include "raylib.h"

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
	};

	struct bomb : entity {

		void set_size(Rectangle rec);
		void set_timer(float timer);

		float	  m_explosion_timer = 0;
		Rectangle m_size_rec = { 0, 0, 0, 0 };
	};

	struct terrain : entity {

		void set_size(Rectangle rec);
		void set_hit(bool hit);

		bool m_hit = false;
		Rectangle m_size_rec = { 0,0,0,0 }
	};
}
