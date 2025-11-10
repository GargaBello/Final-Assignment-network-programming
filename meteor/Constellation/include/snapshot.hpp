//snapshot.hpp

#pragma once

#include "raylib.h"
#include "entities.hpp"
#include "game.hpp"

namespace meteor {

	//struct snapshot {

	//	snapshot() = default;

	//	/*snapshot(uint32 tick, 
	//		std::vector<player> players, 
	//		std::vector<terrain> terrain, 
	//		std::vector<bomb> bombs
	//		);*/

	//	snapshot(uint32 tick, 
	//		player players[MAX_PLAYERS], 
	//		bomb bombs[MAX_PLAYERS], 
	//		terrain_map map);

	//	uint32				 m_tick = 0;
	//	player               m_players[MAX_PLAYERS];
	//	bomb                 m_bombs[MAX_PLAYERS];
	//	terrain_map          m_map = {};

	//	/*std::vector<client>  m_clients;
	//	std::vector<player>	 m_players;
	//	std::vector<terrain> m_terrain;
	//	std::vector<bomb>    m_bombs;*/

	//};

	//struct snapshot_queue {
	//	const uint8 MAX_SNAPSHOTS = 30;

	//	snapshot_queue() = default;

	//	std::vector<snapshot>  m_snapshots;

	//	void create_snapshot(uint32 tick);

	//	void update();
	//};
}