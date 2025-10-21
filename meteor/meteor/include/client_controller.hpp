//client_controller.hpp

#pragma once
#include "entities.hpp"
#include "connection.hpp"

namespace meteor {
	struct Client {
		player	   m_player;
		connection m_connection;
	};
}
