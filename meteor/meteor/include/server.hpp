//server.hpp

#pragma once

#include "connection.hpp"
#include "protocol.hpp"
#include "game.hpp"

namespace meteor {
	struct server {
		struct listener {
			virtual void on_connect(uint32 id) = 0;
			virtual void on_disconnect(uint32 id, bool timeout) = 0;
			virtual void on_send(uint32 id, byte_stream_writer& writer) = 0;
			virtual void on_receive(uint32 id, byte_stream_reader& reader) = 0;
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
			}
		}

		void transmit() {

		}

		bool init(const ip_endpoint& endpoint);
		void shut();

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

			//m_listener->on_disconnect(conn->id(), false);

			//Remove connection
		}

		void handle_payload_packet(const ip_endpoint& endpoint, byte_stream_reader& reader) {
			if (!contains(endpoint)) {
				debug::error("Payload packet received from ip not in connections");
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

			//m_listener->on_receive(conn->id(), reader);
		}

		bool send_connect(connection& conn) {
			connect_packet packet;

			byte_stream stream;
			byte_stream_writer writer(stream);
			if (!packet.write(writer)) {
				debug::error("Connect failed to send");
				return false;
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

			return true;
		}

		bool send_disconnect(const ip_endpoint& endpoint,
			const disconnect_reason_type reason,
			std::string_view message);

		listener*				m_listener = nullptr;
		udp_socket				m_socket;
		ip_endpoint				m_endpoint;
		uint32					m_id_counter = 0;
		std::vector<connection> m_connections;
	};

	struct application : server::listener {
		application()
			:m_server(this) 
		{
		}

		void init() {
			m_game.init();
		}

		void update() {
			m_server.update();
			m_game.update();
		}

		void on_connect(uint32 id) {

		}

		void on_disconnect(uint32 id, bool timeout) {

		}

		void on_send(uint32 id, byte_stream_writer& writer) {

		}

		void on_receive(uint32 id, uint32 sequence, byte_stream_reader& reader) {

		}

		server m_server;
		game   m_game;
	};
}