//client_controller.hpp

#pragma once
#include "entities.hpp"
#include "connection.hpp"

namespace meteor {
	struct Client {
		Client() = default;

		player	   m_player;
		connection m_connection;
	};

	struct client_controller {

		void   add_client(ip_endpoint endpoint, std::vector<Client>& clients);
		uint32 generate_id(std::vector<Client> &clients);

		std::vector<Client> m_clients;
	};
}
