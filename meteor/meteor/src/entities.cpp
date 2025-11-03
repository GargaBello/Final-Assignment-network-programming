//entities.cpp

#include "entities.hpp"

namespace meteor {
	void entity::set_position(Vector2 position)
	{

	}

	void entity::set_hit(bool hit)
	{

	}

	void player::set_cooldown(float cd)
	{

	}

	void create_player(uint32 id)
	{
		player player;
		player.m_id = id;
		player.m_hit = false;
		player.m_cooldown = 0;
		player.m_position = { 0,0 };

		m_all_players.push_back(player);
	}

	void bomb::set_size(Rectangle rec)
	{

	}

	void bomb::set_timer(float timer) {

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

	void terrain::set_size(Rectangle rec) {
		m_size_rec = rec;
	}

	void terrain::set_hit(bool hit) {

	}
}