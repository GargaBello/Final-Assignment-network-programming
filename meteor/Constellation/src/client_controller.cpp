//client_controller.cpp

#include "client_controller.hpp"

namespace meteor {

	client			  m_clients[MAX_CLIENTS];
	client_controller m_client_controllers[MAX_CLIENTS];

	void add_client(ip_endpoint endpoint) {
		int available_slot = -1;
		for (int i = 0; i < MAX_CLIENTS; i++) {
			if (m_clients[i].m_connection.m_status == connection::status::DISCONNECTED ||
				m_clients[i].m_connection.m_id == 0) {
				available_slot = i;
				break;
			}
		}

		// Check if we found a slot
		if (available_slot == -1) {
			debug::info("Server full - cannot add client");
			return;
		}

		client client;
		client.m_connection.m_endpoint = endpoint;
		client.m_connection.m_id = generate_id();
		client.m_connection.m_last_receive_time = GetTime();
		client.m_connection.m_sequence = 0;
		client.m_connection.m_status = connection::status::CONNECTING;

		

		client_controller controller;
		controller.m_client = client;
		
		m_clients[available_slot] = client;
		m_client_controllers[available_slot] = controller;
		
		debug::info("Client created");
	}

	uint32 generate_id() {
		uint32 temp_id = 0;

		for (int i = 0; i < MAX_CLIENTS; i++) {
			if (temp_id < m_clients[i].m_connection.m_id) {
				temp_id = m_clients[i].m_connection.m_id;
			}
		}

		return temp_id + 1;
	}


	void client_controller::update_clients_player(player player)
	{

	}
}
