//entity_controllers

#pragma once

#include "entities.hpp"

namespace meteor {

	struct entity_controller {
		void set_position(Vector2 position);
		void set_hit(bool hít);
		void set_size(Rectangle rec);
	};

	struct player_controller : entity_controller {
		void set_cooldown(float cd);
	};

	struct terrain_controller : entity_controller {
	};

	struct bomb_controller : entity_controller {

	};
}