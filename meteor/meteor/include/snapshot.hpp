//snapshot.hpp

#pragma once

#include "client_controller.hpp"
#include "raylib.h"
#include "entities.hpp"
#include "game.hpp"

namespace meteor {

	const uint8 MAX_SNAPSHOTS = 5;

	struct snapshot {

		snapshot() = default;
		snapshot(uint32 sequence, 
			std::vector<client> clients, 
			std::vector<player> players, 
			std::vector<terrain> terrain, 
			std::vector<bomb> bombs);

		uint32				 m_sequence = 0;
		
		std::vector<client>  m_clients;
		std::vector<player>	 m_players;
		std::vector<terrain> m_terrain;
		std::vector<bomb>    m_bombs;

	};

	struct snapshot_queue {
		snapshot_queue() = default;

		std::vector<snapshot>  m_snapshots;

		void create_snapshot() {
			snapshot shot;

			m_snapshots.emplace(m_snapshots.begin(), shot);
		}
	};
}