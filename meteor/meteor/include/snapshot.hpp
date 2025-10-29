//snapshot.hpp

#pragma once

#include "network.hpp"
#include "client_controller.hpp"
#include "raylib.h"
#include "entities.hpp"

namespace meteor {
	struct snapshot {
		
		std::vector<Client>  m_clients;
		std::vector<player>	 m_players;
		std::vector<terrain> m_terrain;
		std::vector<bomb>    m_bombs;
	};
}