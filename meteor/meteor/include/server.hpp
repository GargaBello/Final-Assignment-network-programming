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
				m_server_sequence++;

				for (int i = 0; i < MAX_CLIENTS; i++) {
					if (m_clients[i].m_connection.m_status == connection::status::CONNECTED) {
						m_clients[i].m_connection.m_sequence = m_server_sequence;

						send_payload(m_clients[i].m_connection);
					}
				}
			}
		}

		//bool init(const ip_endpoint& endpoint) 
		bool init() {
			network::query_local_addresses(m_local_addresses);
			m_local_address = m_local_addresses[0];
			m_local_endpoint = { m_local_address, 54321 };

			if (!m_socket.open_and_bind(m_local_endpoint)) {
				debug::info("Could not bind socket");
				return false;
			}

			return true;
		}


		void shut();

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
			if (contains(endpoint)) {
				return;
			}

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
					send_connect(m_clients[i].m_connection, m_clients[i].m_player.m_id);
				}
			}

		}

		void handle_disconnect_packet(const ip_endpoint& endpoint, byte_stream_reader& reader) {
			if (!contains(endpoint)) {
				debug::info("Disconnect packet tried to send to ip not in connections");
				return;
			}

			disconnect_packet packet;
			if (!packet.read(reader)) {
				debug::error("Unable to read disconnect packet");
				return;
			}

			send_disconnect(endpoint, disconnect_reason_type::DISCONNECTING, "You are disconnecting");

			for (int i = 0; i < MAX_CLIENTS; i++) {
				if (m_clients[i].m_connection.m_endpoint == endpoint) {
					m_clients[i].m_connection.m_status = connection::status::DISCONNECTING;
					remove_disconnected_connections(m_clients[i].m_connection);
					m_listener->on_disconnect(m_clients[i].m_connection.m_id, false);
				}
			}

			//Remove connection
		}

		void handle_payload_packet(const ip_endpoint& endpoint, byte_stream_reader& reader) {
			if (!contains(endpoint)) {
				debug::error("Payload packet received from ip not in connections");
				return;
			}

			/*if (conn->is_connecting()) {
				conn->set_status(connection::status::CONNECTED);
			}*/

			//conn->update_last_received_time();

			payload_packet packet;
			if (!packet.read(reader)) {
				debug::error("Unable to read payload packet");
				return;
			}
			

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
		}

		bool send_connect(connection& conn, uint32 id) {
			connect_packet packet;
			packet.m_id = id;

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

			//send data, increment sequence after data sent


			if (!m_socket.send_to(conn.m_endpoint, stream)) {
				debug::error("Could not send payload to: %d.%d.%.d.%d",
					conn.m_endpoint.address().a(),
					conn.m_endpoint.address().b(),
					conn.m_endpoint.address().c(),
					conn.m_endpoint.address().d()
				);
			}

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
		snapshot                m_snapshot;
		snapshot_queue          m_snap_queue;
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
			m_queue.create_snapshot(m_game.m_tick);
			m_server.transmit();
		}

		void on_connect(uint32 id) {

		}

		void on_disconnect(uint32 id, bool timeout) {

		}

		void on_send(uint32 id, byte_stream_writer& writer) {
			for (int i = 0; i < MAX_CLIENTS; i++) {
				if (m_clients[i].m_connection.m_id == id) {
					// create message with clients data and write it to the writer

					byte_stream stream;
					payload_packet packet(m_server.m_server_sequence, m_clients->m_connection.m_acknowledge);

					if (!packet.write(writer)) {
						debug::error("Could not write payload packet");
						return;
					}

					snapshot_message message(
						m_queue.m_snapshots.front(),
						m_queue.m_snapshots.front().m_tick
					);

					if (!message.write(writer)) {
						debug::error("Could not write snapshot_message");
					}
				}
			}
		}

		void on_receive(uint32 id, uint32 sequence, byte_stream_reader& reader) {
			for (int i = 0; i < MAX_CLIENTS; i++) {
				if (m_clients[i].m_connection.m_id == id) {
					if (m_clients[i].m_connection.m_sequence < sequence) {
						auto messageType = (message_type)reader.peek();

						switch (messageType)
						{
							case message_type::SNAPSHOT: {
								snapshot_message message;

								if (!message.read(reader)) {
									debug::error("Could not read snapshot message");
									return;
								}

								
								break;
							}
							case message_type::INPUT_ACTION: {
								input_action_message message;

								if (!message.read(reader)) {
									debug::error("Could not read input message");
									return;
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
		}

		server         m_server;
		game		   m_game;
		snapshot_queue m_queue;
	};
}