//client_controller.hpp

#pragma once
#include "entities.hpp"
#include "connection.hpp"

namespace meteor {

	constexpr int MAX_CLIENTS = 4;

	struct client {
		client() = default;

		player	              m_player;
		connection            m_connection;

	};

	struct client_controller {

		client_controller() = default;



		client m_client;
	};

	void add_client(ip_endpoint endpoint);

	uint32 generate_id();

	//std::vector<Client> m_clients;

	extern client			  m_clients[MAX_CLIENTS];
	extern client_controller  m_client_controllers[MAX_CLIENTS];
}
