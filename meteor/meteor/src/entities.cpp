//entities.cpp

#include "entities.hpp"

namespace meteor {

	std::vector<player>  m_all_players;
	std::vector<bomb>    m_all_bombs;
    std::vector<terrain> m_all_terrain;

	void create_player(uint32 id)
	{
		player player;
		player.m_id = id;
		player.m_hit = false;
		player.m_cooldown = 0;
		player.m_position = { 0,0 };

		m_all_players.push_back(player);
	}

	void create_bomb(uint8 id, Vector2 position)
	{
		bomb bomb;
		bomb.m_id = id;
		bomb.m_position = position;
		bomb.m_explosion_timer = 0;
		bomb.m_hit = false;

		m_all_bombs.push_back(bomb);
	}
}