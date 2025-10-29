//client_controller.cpp

#include "client_controller.hpp"
#include "connection.hpp"

namespace meteor {
	void add_client(ip_endpoint endpoint, std::vector<Client>& clients) {
		Client client;
		client.m_connection.m_endpoint = endpoint;
		client.m_connection.m_id = generate_id(clients);
		client.m_connection.m_last_receive_time = GetTime();
		client.m_connection.m_sequence = 0;
		client.m_connection.m_status = connection::status::CONNECTING;
		clients.push_back(client);
		debug::info("Client created");
	}

	uint32 generate_id(std::vector<Client> &clients) {
		uint32 id = 0;
		uint32 temp_id = 0;

		for (int i = 0; i < clients.size(); i++) {
			if (temp_id < clients[i].m_connection.m_id) {
				temp_id = clients[i].m_connection.m_id;
			}
		}

		id = (temp_id += 1);

		return id;
	}
}
