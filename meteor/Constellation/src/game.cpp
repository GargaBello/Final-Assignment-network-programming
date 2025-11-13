//game.cpp

#include "game.hpp"

namespace meteor {

	const player& game_state::get_player(const int index) const
	{
		assert(index < MAX_PLAYERS && index >= 0);
		if (index >= MAX_PLAYERS) return m_players[MAX_PLAYERS - 1];
		else if (index < 0)		  return m_players[0];
		else					  return m_players[index];
	}

	const bomb& game_state::get_bomb(const int index) const
	{
		assert(index < MAX_PLAYERS && index >= 0);
		if (index >= MAX_PLAYERS) return m_bombs[MAX_PLAYERS - 1];
		else if (index < 0)		  return m_bombs[0];
		else					  return m_bombs[index];
	}

	const terrain_map& game_state::get_map() const
	{
		
		return m_map;
	}

	const bool game_state::is_default() const
	{
		return m_players[0].m_prev_action == player::action::INVALID;
	}

	//bool tilemap::is_tile_active(const uint8 x, const uint8 y) const
	//{
	//	assert(valid_tile(x, y));

	//	uint8 byte = *(m_tiles + ((x + y * WIDTH) / 8));
	//	uint8 bitmask = (uint8)1 << ((x + y * WIDTH) % 8);

	//	return (byte & bitmask) != 0;
	//}

	//bool tilemap::is_tile_active(const uint32 index) const
	//{
	//	assert(index < tilemap::COUNT);

	//	uint8 byte = *(m_tiles + (index / 8));
	//	uint8 bitmask = (uint8)1 << (index % 8);

	//	return (byte & bitmask) != 0;			// & is the bitwise "and" operator, so if the result is higher than 0, that bit is active.
	//}

	//void tilemap::set_tile(const uint8 x, const uint8 y, bool value)
	//{
	//	assert(valid_tile(x, y));
	//	uint8& byte = *(m_tiles + ((x + y * WIDTH) / 8));
	//	uint8 bitmask = (uint8)1 << ((x + y * WIDTH) % 8);

	bool terrain_map::tile_active(int x, int y, terrain_map map)
	{
		bool active = true;

		if (!out_of_bounds(x, y)) {
			active = true;
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

	//	if (value) { byte = byte | bitmask; }			// | is bitwise "or", resulting in all 1s being kept from both
	//	else { byte = byte & (~bitmask); }				// ~ is bitwise complement operator, flipping all bits 1->0 and 0->1
	//}
}