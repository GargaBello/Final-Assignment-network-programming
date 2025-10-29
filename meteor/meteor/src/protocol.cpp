// protocol.cpp

#include "protocol.hpp"

namespace meteor
{
	connect_packet::connect_packet(uint32 magic, uint32 version)
		: m_type((uint8) protocol_packet_type::CONNECT)
		, m_magic(PROTOCOL_MAGIC)
		, m_version(PROTOCOL_VERSION)
	{
	}

	template <typename T>
	bool serialize(connect_packet& packet, T& stream) {
		bool success = true;
		success &= stream.serialize(packet.m_type);
		success &= stream.serialize(packet.m_magic);
		success &= stream.serialize(packet.m_version);
		return success;
	}

	bool connect_packet::write(byte_stream_writer& writer) {
		return serialize(*this, writer);
	}

	bool connect_packet::read(byte_stream_reader& reader) {
		return serialize(*this, reader);
	}

	disconnect_packet::disconnect_packet(uint32 sequence, uint8 reason) 
		: m_type((uint8) protocol_packet_type::DISCONNECT)
		, m_sequence(sequence)
		, m_reason(reason)
	{
	}

	template <typename T>
	bool serialize(disconnect_packet& packet, T& stream) {
		bool success = true;
		success &= stream.serialize(packet.m_type);
		success &= stream.serialize(packet.m_sequence);
		success &= stream.serialize(packet.m_reason);
		//success &= stream.serialize(packet.m_message);
		return success;
	}

	bool disconnect_packet::write(byte_stream_writer& writer) {
		return serialize(*this, writer);
	}

	bool disconnect_packet::read(byte_stream_reader& reader) {
		return serialize(*this, reader);
	}

	payload_packet::payload_packet(uint32 sequence, uint32 acknowledge)
		: m_type((uint8)protocol_packet_type::PAYLOAD)
		, m_sequence(sequence)
		, m_acknowledge(acknowledge)
	{
	}

	template <typename T>
	bool serialize(payload_packet& packet, T& stream) {
		bool success = true;
		success &= stream.serialize(packet.m_type);
		success &= stream.serialize(packet.m_sequence);
		success &= stream.serialize(packet.m_acknowledge);
		return success;
	}

	bool payload_packet::read(byte_stream_reader& reader) {
		return serialize(*this, reader);
	}

	bool payload_packet::write(byte_stream_writer& writer) {
		return serialize(*this, writer);
	}


} // !meteor
