#include "connection.hpp"

namespace meteor {
	connection::connection(uint32 id, ip_endpoint endpoint, status status, double time, uint32 sequence)
		: m_id(id)
		, m_endpoint(endpoint)
		, m_status(status)
		, m_last_receive_time(time)
		, m_sequence(sequence)
	{
	}
}