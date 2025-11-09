//server.hpp

#pragma once

#include "connection.hpp"
#include "protocol.hpp"
#include "game.hpp"
#include "client_controller.hpp"
#include "helper_functions.hpp"
#include "network.hpp"
#include "messages.hpp"

namespace meteor {
	struct server {
		struct listener {
			virtual void on_connect(uint32 id) = 0;
			virtual void on_disconnect(uint32 id, bool timeout) = 0;
			virtual void on_send(uint32 id, byte_stream_writer& writer) = 0;
			virtual void on_receive(uint32 id, uint32 sequence, byte_stream_reader& reader) = 0;
		};

		server(listener* listener)
			: m_listener(listener)
		{
		}

		void update() {
			receive();
			transmit();
		}

		void receive() {
			int32 counter = 10;
			while (m_socket.has_data() && counter >= 0) {
				byte_stream stream;
				ip_endpoint endpoint;
				if (!m_socket.receive_from(endpoint, stream)) {
					debug::error("Unable to receive stream");
					break;
				}

				byte_stream_reader reader(stream);
				protocol_packet_type packet_type = (protocol_packet_type)reader.peek();
				switch (packet_type)
				{
				case meteor::protocol_packet_type::CONNECT:
					handle_connect_packet(endpoint, reader);
					break;
				case meteor::protocol_packet_type::DISCONNECT:
					handle_disconnect_packet(endpoint, reader);
					break;
				case meteor::protocol_packet_type::PAYLOAD:
					handle_payload_packet(endpoint, reader);
					break;
				default:
					debug::error("Invalid packet received");
					break;
				}

				counter--;
			}
		}

		void transmit() {
			// send all the packages at a fixed rate

			if (timer_check(m_prev_time)) {
				#ifdef _SERVER

				m_server_sequence++;

				for (int i = 0; i < MAX_CLIENTS; i++) {
					if (m_clients[i].m_connection.m_status == connection::status::CONNECTED) {
						m_clients[i].m_connection.m_sequence = m_server_sequence;

						send_payload(m_clients[i].m_connection);
					}
				}
				#endif // _SERVER

				#ifdef _CLIENT

				if (m_my_connection.m_status == connection::status::CONNECTING || m_my_connection.m_status == connection::status::CONNECTED) {
					m_my_connection.m_sequence++;

					send_payload(m_my_connection);
					
				}

				#endif // _CLIENT

			}
		}

		//bool init(const ip_endpoint& endpoint) 
		bool init() {


			network::query_local_addresses(m_local_addresses);
			m_local_address = m_local_addresses[0];

#ifdef _SERVER
			m_local_endpoint = { m_local_address, 54321 };
#endif // _SERVER


#ifdef _CLIENT
			m_local_endpoint = { m_local_address, 54322 };
#endif // _CLIENT


			if (!m_socket.open_and_bind(m_local_endpoint)) {
				debug::info("Could not bind socket");
				return false;
			}

			


			#ifdef _CLIENT

			m_my_connection.m_endpoint = SERVER_ENDPOINT;
			m_my_connection.m_id = 0;

			send_connect(m_my_connection, m_my_connection.m_id);

			#endif // _CLIENT

			return true;

			/*network::query_local_addresses(m_local_addresses);
			m_local_address = m_local_addresses[0];
			m_local_endpoint = { m_local_address, 54321 };

			if (!m_socket.open_and_bind(m_local_endpoint)) {
				debug::info("Could not bind socket");
				return false;
			}

			return true;*/
		}


		void shut() {
			m_socket.close();
		}

		void perform_timeout_check(connection& conn) {
			double current_time = GetTime();
			float timer = 5.0f;

			if ((current_time - conn.m_last_receive_time) >= timer) {
				conn.m_status = connection::status::DISCONNECTED;
				send_disconnect(conn.m_endpoint, disconnect_reason_type::TIMED_OUT, "You have timed out"); 
				m_listener->on_disconnect(conn.m_id, true);
			}
		}

		void remove_disconnected_connections(connection& conn) {
			for (int i = 0; i < MAX_CLIENTS; i++) {
				if (m_clients[i].m_connection.m_status == connection::status::DISCONNECTING || m_clients[i].m_connection.m_status == connection::status::DISCONNECTED) {
					connection default_connection;

					m_clients[i].m_connection = default_connection;
				}
			}
		}

		bool contains(const ip_endpoint& endpoint) const {
			for (const auto& conn : m_connections) {
				if (conn.m_endpoint == endpoint) {
					return true;
				}
			}
			return false;
		}

		void handle_connect_packet(const ip_endpoint& endpoint, byte_stream_reader& reader) {

			connect_packet packet;
			if (!packet.read(reader)) {
				debug::error("Unable to read connect packet");
				return;
			}


			if (~packet.m_magic == PROTOCOL_MAGIC) {
				//send disconnect packet with reason as wrong magic
				std::string_view message = "You have the wrong magic";

				send_disconnect(endpoint, disconnect_reason_type::WRONG_MAGIC, message);
			} else if (~packet.m_version == PROTOCOL_VERSION) {
				//Send disconnect packet with reason as wrong version
				std::string_view message = "You have the wrong version";

				send_disconnect(endpoint, disconnect_reason_type::WRONG_VERSION, message);
			}
			#ifdef _SERVER
			else {
				bool client_present = false;

				for (int i = 0; i < MAX_CLIENTS; i++) {
					if (m_clients[i].m_connection.m_endpoint == endpoint) {
						client_present = true;
					}
				}

				if (!client_present) {
					add_client(endpoint);
				}
			}
			

			for (int i = 0; i < MAX_CLIENTS; i++) {
				if (m_clients[i].m_connection.m_endpoint == endpoint) {
					send_connect(m_clients[i].m_connection, m_clients[i].m_connection.m_id);
					m_listener->on_connect(m_clients[i].m_player.m_id);
				}
			}

			#endif // _SERVER

			#ifdef _CLIENT

			m_my_connection.m_status = connection::status::CONNECTING;
			m_my_connection.m_id = packet.m_id;


			m_listener->on_connect(m_my_connection.m_id);
			#endif // _CLIENT
		}

		void handle_disconnect_packet(const ip_endpoint& endpoint, byte_stream_reader& reader) {

			disconnect_packet packet;
			if (!packet.read(reader)) {
				debug::error("Unable to read disconnect packet");
				return;
			}

			#ifdef _SERVER
			send_disconnect(endpoint, disconnect_reason_type::DISCONNECTING, "You are disconnecting");

			for (int i = 0; i < MAX_CLIENTS; i++) {
				if (m_clients[i].m_connection.m_endpoint == endpoint) {
					m_clients[i].m_connection.m_status = connection::status::DISCONNECTING;
					remove_disconnected_connections(m_clients[i].m_connection);
					m_listener->on_disconnect(m_clients[i].m_connection.m_id, false);
				}
			}

			#endif // _SERVER

			#ifdef _CLIENT

			send_disconnect(endpoint, disconnect_reason_type::DISCONNECTING, "I am disconnecting");

			m_listener->on_disconnect(m_my_connection.m_id, false);

			#endif // _CLIENT

		}

		void handle_payload_packet(const ip_endpoint& endpoint, byte_stream_reader& reader) {
			payload_packet packet;
			if (!packet.read(reader)) {
				debug::error("Unable to read payload packet");
				return;
			}
			
			#ifdef _SERVER

			for (int i = 0; i < MAX_CLIENTS; i++) {
				if (m_clients[i].m_connection.m_endpoint == endpoint) {
					if (m_clients[i].m_connection.m_status == connection::status::CONNECTING || m_clients[i].m_connection.m_status == connection::status::CONNECTED) {
						m_clients[i].m_connection.m_status = connection::status::CONNECTED;
						if (packet.m_sequence > m_clients[i].m_connection.m_sequence) {
							m_clients[i].m_connection.m_last_receive_time = GetTime();
							m_clients[i].m_connection.m_acknowledge = packet.m_sequence;
							m_listener->on_receive(m_clients[i].m_connection.m_id, packet.m_sequence, reader);
						}
					}
				}
			}


			#endif // _SERVER

			#ifdef _CLIENT

			if (m_my_connection.m_sequence < packet.m_sequence) {
				if (m_my_connection.m_status == connection::status::CONNECTING || m_my_connection.m_status == connection::status::CONNECTED) {
					m_my_connection.m_status = connection::status::CONNECTED;
					m_my_connection.m_sequence = packet.m_sequence;
					m_my_connection.m_acknowledge = packet.m_acknowledge;
					m_my_connection.m_last_receive_time = GetTime();

					m_listener->on_receive(m_my_connection.m_id, packet.m_sequence, reader);
				}
			}
			
			#endif // _CLIENT

		}

		bool send_connect(connection& conn, uint32 id) {
			connect_packet packet((uint8)id);

			byte_stream stream;
			byte_stream_writer writer(stream);
			if (!packet.write(writer)) {
				debug::error("Connect failed to send");
				return false;
			}

			if (!m_socket.send_to(conn.m_endpoint, stream)) {
				debug::error("Failed to send connect packet back to: %d.%d.%d.%d",
					conn.m_endpoint.address().a(),
					conn.m_endpoint.address().b(),
					conn.m_endpoint.address().c(),
					conn.m_endpoint.address().d()
				);
			}

			debug::info("Sent connect");

			return true;
		}

		bool send_payload(connection& conn) {
			payload_packet packet;
			packet.m_sequence = conn.m_sequence;

			byte_stream stream;
			byte_stream_writer writer(stream);

			if (!packet.write(writer)) {
				debug::error("Payload failed to send");
				return false;
			}

			m_listener->on_send(conn.m_id, writer);


			if (!m_socket.send_to(conn.m_endpoint, stream)) {
				debug::error("Could not send payload to: %d.%d.%.d.%d",
					conn.m_endpoint.address().a(),
					conn.m_endpoint.address().b(),
					conn.m_endpoint.address().c(),
					conn.m_endpoint.address().d()
				);
			}

			debug::info("Sent Payload");

			return true;
		}

		bool send_disconnect(const ip_endpoint& endpoint,
			const disconnect_reason_type reason,
			std::string_view message) {
			disconnect_packet packet;
			packet.m_reason = (uint8)reason;

			byte_stream stream;
			byte_stream_writer writer(stream);

			if (!packet.write(writer)) {
				debug::error("Failed to write disconnect packet");
				return false;
			}

			if (!m_socket.send_to(endpoint, stream)) {
				debug::error("Failed to send disconnect packet");
				return false;
			}

			debug::info("Sent disconnect");

			return true;
		}

		listener*				m_listener = nullptr;
		udp_socket				m_socket;
		ip_endpoint				m_endpoint;
		uint32					m_id_counter = 0;
		uint32                  m_server_sequence = 0;
		std::vector<connection> m_connections;
		double                  m_prev_time = 0;
		ip_endpoint             m_local_endpoint;
		ip_address              m_local_address;
		std::vector<ip_address> m_local_addresses;

#ifdef _CLIENT

		const uint16 PORT = 54321;
		const ip_endpoint SERVER_ENDPOINT = {ip_address(192, 168, 1, 53), PORT};
		connection m_my_connection;

#endif // _CLIENT

	};

	struct application : server::listener {
		application()
			:m_server(this) 
		{
		}

		void init() {
			m_game.init();
			m_server.init();
		}

		void update() {
			m_server.receive();
			m_game.update();
			m_server.transmit();
		}

		void close() {
			m_server.shut();
		}

		void on_connect(uint32 id) {

#ifdef _CLIENT

			for (int i = 0; i < MAX_PLAYERS; i++) {
				if (m_game.m_players[i].m_id == id) {
					m_game.m_players[i].is_player_character = true;
				}
			}

#endif // _CLIENT
		}

		void on_disconnect(uint32 id, bool timeout) {

		}

		void on_send(uint32 id, byte_stream_writer& writer) {
			#ifdef _SERVER
			for (int i = 0; i < MAX_CLIENTS; i++) {
				if (m_clients[i].m_connection.m_id == id) {

					byte_stream stream;
					payload_packet packet(m_server.m_server_sequence, m_clients->m_connection.m_acknowledge);

					if (!packet.write(writer)) {
						debug::error("Could not write payload packet");
						return;
					}

					snapshot_message message(
						m_game.m_snapshot,
						m_game.m_tick
					);

					/*snapshot_message message(
						m_queue.m_snapshots.front(),
						m_queue.m_snapshots.front().m_tick
					);*/

					if (!message.write(writer)) {
						debug::error("Could not write snapshot_message");
					}
				}
			}
			#endif // _SERVER

			#ifdef _CLIENT

			byte_stream stream;
			payload_packet packet(m_server.m_my_connection.m_sequence, m_server.m_my_connection.m_acknowledge);

			if (!packet.write(writer)) {
				debug::error("Could not write cclient payload packet");
				return;
			}

			input_action_message message;
			message.m_type = (uint8)message_type::INPUT_ACTION;
			message.m_tick = m_game.m_tick;

			for (const player& player : m_game.m_players) {
				if (m_server.m_my_connection.m_id == player.m_id) {
					message.m_movement_request = (uint8)player.m_predict_action;
				}
			}

			if (!message.write(writer)) {
				debug::error("Could not write input action message");
			}
					
			#endif // _CLIENT


		}


		void on_receive(uint32 id, uint32 sequence, byte_stream_reader& reader) {
			#ifdef _SERVER


			for (int i = 0; i < MAX_CLIENTS; i++) {
				if (m_clients[i].m_connection.m_id == id) {
					if (m_clients[i].m_connection.m_sequence < sequence) {
						auto messageType = (message_type)reader.peek();

						switch (messageType)
						{
							case message_type::INPUT_ACTION: {
								input_action_message message;

								if (!message.read(reader)) {
									debug::error("Could not read input message");
									return;
								}

								for (int j = 0; j < MAX_PLAYERS; j++) {
									if (m_game.m_players[j].m_id == id) {
										m_game.m_players[j].m_action = (player::action)message.m_movement_request;
									}
								}

								break;
							}
							default:
							{
								debug::error("Invalid message received");
								break;
							}
						}
					}
				}
			}
			#endif // _SERVER

			#ifdef _CLIENT

			auto messageType = (message_type)reader.peek();

			switch (messageType)
			{
				case meteor::message_type::SNAPSHOT: {
					snapshot_message message;

					if (!message.read(reader)) {
						debug::error("Could not read snapshot message");
						return;
					}

					for (const player& player : message.m_shot.m_players) {
						for (int i = 0; i < MAX_PLAYERS; i++) {
							if (m_game.m_players[i].m_id == player.m_id) {
								m_game.m_players[i] = player;
							}
						}
					}
					
					for (const bomb& bomb : message.m_shot.m_bombs) {
						for (int i = 0; i < MAX_PLAYERS; i++) {
							if (m_game.m_bombs[i].m_id == bomb.m_id) {
								m_game.m_bombs[i] = bomb;
							}
						}
					}

					for (int x = 0; x < m_game.m_map.ARRAY_WIDTH; x++) {
						for (int y = 0; y < m_game.m_map.ARRAY_HEIGHT; y++) {
							m_game.m_map.m_terrain_map[x][y] = message.m_shot.m_map.m_terrain_map[x][y];
						}
					}

					break;
				}
				default: {
					break;
				}
			}

			#endif // _CLIENT

		}

		server         m_server;
		game		   m_game;
	};
}