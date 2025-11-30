//game.cpp

#include "game.hpp"

namespace meteor {

	bool terrain_map::tile_active(int x, int y, terrain_map map)
	{
		bool active = true;

		if (out_of_bounds(x, y)) {
			active = true;
			return active;
		}
		else if (map.m_terrain_map[x][y].m_hit) {
			active = false;
		}
		
		return active;
	}

	snapshot::snapshot(uint32 tick, player players[MAX_PLAYERS], bomb bombs[MAX_PLAYERS], bool terrain_hits[6][6], uint8 status)
		: m_tick(tick)
		, m_players(players[MAX_PLAYERS])
		, m_bombs(bombs[MAX_PLAYERS])
		, m_terrain_hits(terrain_hits)
		, m_status(status)
	{
	}
}