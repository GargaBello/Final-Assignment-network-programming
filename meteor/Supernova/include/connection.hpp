//connection.hpp

#pragma once

#include "network.hpp"

namespace meteor {
	struct connection {
		enum class status {
			DISCONNECTED,
			CONNECTING,
			CONNECTED,
			DISCONNECTING
		};

		void id();
		void set_status();
		void is_connecting();

		connection() = default;
		connection(uint32 id, ip_endpoint endpoint, status status, double time, uint32 sequence);

		uint32		m_id = 0;
		ip_endpoint m_endpoint;
		status		m_status = {};
		double		m_last_receive_time = 0;
		uint32		m_sequence = 0;
		uint32		m_acknowledge = 0;
	};
}
