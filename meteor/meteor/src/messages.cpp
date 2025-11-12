// messages.cpp

#include "messages.hpp"

namespace meteor
{
   /*entity_state_message::entity_state_message(int32 id, Vector2 position, Color color, entity_type ent_type)
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
   }*/

   /*template <typename T>
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
   }*/


    template <typename T_from, typename T_to>
    bool serialize(T_from& type, byte_stream_writer& stream)
    {
        bool success = true;
        success &= stream.serialize((T_to)type);
        return success;
    }

    template <typename T_from, typename T_to>
    bool serialize(T_from& type, byte_stream_reader& stream)
    {
        bool success = true;
        T_to v = 0;
        success &= stream.serialize(v);
        type = (T_from)v;
        return success;
    }


    input_action_message::input_action_message(uint8 move_req, uint32 tick)
       : m_type((uint8)message_type::INPUT_ACTION)
       , m_movement_request(move_req)
       , m_tick(tick)
   {
   }

   template <typename T>
   bool serialize(input_action_message&message, T& stream)
   {
       bool success = true;
       success &= stream.serialize(message.m_type);
       success &= stream.serialize(message.m_movement_request);
       return success;
   }

   bool input_action_message::read(byte_stream_reader& reader)
   {
       return serialize(*this, reader);
   }

   bool input_action_message::write(byte_stream_writer& writer)
   {
       return serialize(*this, writer);
   }

   snapshot_message::snapshot_message(snapshot shot, uint32 tick)
       : m_type((uint8)message_type::SNAPSHOT)
       , m_shot(shot)
       , m_tick(tick)
   {
   }

   //Taken directly from Theos genius

   template <typename T>
   bool serialize(player& player, T& stream) {
       bool success = true;

       success &= stream.serialize(player.m_id);
       success &= stream.serialize(player.m_hit);
       success &= stream.serialize(player.m_position.x);
       success &= stream.serialize(player.m_position.y);
       success &= stream.serialize(player.m_cooldown);

       return success;
   }

   template <typename T>
   bool serialize(bomb& bomb, T& stream) {
       bool success = true;

       success &= stream.serialize(bomb.m_id);
       success &= stream.serialize(bomb.m_explosion_tick);
       success &= stream.serialize(bomb.m_position.x);
       success &= stream.serialize(bomb.m_position.y);

       return success;
   }

   template <typename T>
   bool serialize(terrain& terrain, T& stream) {
       bool success = true;

       success &= stream.serialize(terrain.m_center_of_pos);
       success &= stream.serialize(terrain.m_hit);
       success &= stream.serialize(terrain.m_origin);
       success &= stream.serialize(terrain.m_position);
       success &= stream.serialize(terrain.m_size_rec);
       success &= stream.serialize(terrain.m_terrain_map_pos);
       success &= stream.serialize(terrain.RECTANGLE_SIDE_LENGTH);

   }

   template <typename T>
   bool serialize(snapshot& shot, T& stream) {
       bool success = true;

       for (int i = 0; i < MAX_PLAYERS; i++) {
           success &= serialize(shot.m_bombs[i], stream);
           success &= serialize(shot.m_players[i], stream);
       }

       

       /*for (uint8 tile : m_all_terrain) {
           success &= stream.serialize(tile);
       }*/

       return success;
   }

   template <typename T>
   bool serialize(snapshot_message& message, T& stream)
   {
       bool success = true;

       //success &= serialize<message_type, uint8>(message.m_type, stream);
       success &= stream.serialize(message.m_type);
       success &= stream.serialize(message.m_tick);
       success &= serialize(message.m_shot, stream);

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
