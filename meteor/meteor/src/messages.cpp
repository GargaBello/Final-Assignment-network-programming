// messages.cpp

#include "messages.hpp"

namespace meteor
{
   entity_state_message::entity_state_message(int32 id, Vector2 position, Color color, entity_type ent_type)
      : m_type((uint8)message_type::ENTITY_STATE)
      , m_id(id)
      , m_position(position)
      , m_color(color)
      , m_entity_type((uint8)ent_type)
   {
   }

   template <typename T>
   bool serialize(entity_state_message &message, T& stream)
   {
      bool success = true;
      success &= stream.serialize(message.m_type);
      success &= stream.serialize(message.m_entity_type);
      success &= stream.serialize(message.m_id);
      success &= stream.serialize(message.m_position.x);
      success &= stream.serialize(message.m_position.y);
      success &= stream.serialize(message.m_color.r);
      success &= stream.serialize(message.m_color.g);
      success &= stream.serialize(message.m_color.b);
      success &= stream.serialize(message.m_color.a);
      return success;
   }

   bool entity_state_message::write(byte_stream_writer& writer)
   {
      return serialize(*this, writer);
   }

   bool entity_state_message::read(byte_stream_reader& reader)
   {
      return serialize(*this, reader);
   }

   latency_message::latency_message(double time)
      : m_type((uint8)message_type::LATENCY)
      , m_time(time)
   {
   }

   template <typename T>
   bool serialize(latency_message &message, T& stream)
   {
      bool success = true;
      success &= stream.serialize(message.m_type);
      success &= stream.serialize(message.m_time);
      return success;
   }

   bool latency_message::write(byte_stream_writer& writer)
   {
      return serialize(*this, writer);
   }

   bool latency_message::read(byte_stream_reader& reader)
   {
      return serialize(*this, reader);
   }

   mouse_position_message::mouse_position_message(float x, float y)
      : m_type((uint8)message_type::POSITION)
      , m_x(x)
      , m_y(y)
   {
   }

   template <typename T>
   bool serialize(mouse_position_message &message, T& stream)
   {
      bool success = true;
      success &= stream.serialize(message.m_type);
      success &= stream.serialize(message.m_x);
      success &= stream.serialize(message.m_y);
      return success;
   }

   bool mouse_position_message::write(byte_stream_writer& writer)
   {
      return serialize(*this, writer);
   }

   bool mouse_position_message::read(byte_stream_reader& reader)
   {
      return serialize(*this, reader);
   }

   movement_request_message::movement_request_message(uint8 move_req)
       : m_type((uint8)message_type::MOVEMENT_REQUEST)
       , m_movement_request(move_req)
   {
   }

   template <typename T>
   bool serialize(movement_request_message &message, T& stream) 
   {
       bool success = true;
       success &= stream.serialize(message.m_type);
       success &= stream.serialize(message.m_movement_request);
       return success;
   }

   bool movement_request_message::read(byte_stream_reader& reader) 
   {
       return serialize(*this, reader);
   }

   bool movement_request_message::write(byte_stream_writer& writer) 
   {
       return serialize(*this, writer);
   }

   snapshot_message::snapshot_message(snapshot shot)
       : m_type((uint8)message_type::SNAPSHOT)
       , m_shot(shot)
   {
   }

   template <typename T>
   bool serialize(snapshot_message& message, T& stream) 
   {
       bool success = true;
       success &= stream.serialize(message.m_type);
       success &= stream.serialize(message.m_shot.m_sequence);
       return success;
   }

   bool snapshot_message::read(byte_stream_reader& reader)
   {
       return serialize(*this, reader);
   }

   bool snapshot_message::write(byte_stream_writer& writer)
   {
       return serialize(*this, writer);
   }

} // !meteor
