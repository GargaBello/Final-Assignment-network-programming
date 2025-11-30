// messages.cpp

#include "messages.hpp"

namespace meteor
{
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

   //Modified version of theos implementation

   template <typename T>
   bool serialize(player& player, T& stream) {
       bool success = true;

       success &= stream.serialize(player.m_id);
       success &= stream.serialize(player.m_hit);
       success &= stream.serialize(player.m_position.x);
       success &= stream.serialize(player.m_position.y);
       success &= stream.serialize(player.m_cooldown);
       success &= stream.serialize(player.m_terrain_map_pos.x);
       success &= stream.serialize(player.m_terrain_map_pos.y);

       return success;
   }

   template <typename T>
   bool serialize(bomb& bomb, T& stream) {
       bool success = true;

       success &= stream.serialize(bomb.m_id);
       success &= stream.serialize(bomb.m_explosion_tick);
       success &= stream.serialize(bomb.m_position.x);
       success &= stream.serialize(bomb.m_position.y);
       success &= stream.serialize(bomb.m_hit);

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

       const int TERRAIN_MAP_ARRAY_SIZE = 6;

       for (int x = 0; x < TERRAIN_MAP_ARRAY_SIZE; x++) {
           for (int y = 0; y < TERRAIN_MAP_ARRAY_SIZE; y++) {
               stream.serialize(shot.m_terrain_hits[x][y]);
           }
       }

       success &= stream.serialize(shot.m_status);

       return success;
   }

   template <typename T>
   bool serialize(snapshot_message& message, T& stream)
   {
       bool success = true;

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
