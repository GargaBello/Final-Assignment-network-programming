// messages.hpp

#pragma once

#include "raylib.h"
#include "network.hpp"
#include "snapshot.hpp"

namespace meteor
{
   enum class message_type : uint8 {
      /*LATENCY,
      POSITION,
      ENTITY_STATE,*/
      SNAPSHOT,
      INPUT_ACTION
   };

   enum class movement_request : uint8 {
       RIGHT,
       LEFT,
       UP,
       DOWN
   };

   enum class entity_type : uint8 {
       PLAYER,
       BOMB,
       TERRAIN
   };

   /*struct entity_state_message {
      entity_state_message() = default;
      entity_state_message(int32 id, Vector2 position, Color color, entity_type ent_type);

      bool write(byte_stream_writer& writer);
      bool read(byte_stream_reader& reader);
      
      uint8   m_type = 0;
      uint8   m_entity_type = 0;
      int32   m_id = 0;
      Vector2 m_position = {};
      Color   m_color = {};
      uint8   m_move_request = 0;
   };

   struct latency_message {
      latency_message() = default;
      latency_message(double time);

      bool write(byte_stream_writer& writer);
      bool read(byte_stream_reader& reader);

      uint8  m_type = 0;
      double m_time = 0;
   };

   struct mouse_position_message {
      mouse_position_message() = default;
      mouse_position_message(float x, float y);

      bool write(byte_stream_writer& writer);
      bool read(byte_stream_reader& reader);

      uint8 m_type = 0;
      float m_x = 0;
      float m_y = 0;
   };*/

   struct input_action_message {
       input_action_message() = default;
       input_action_message(uint8 move_req, uint32 tick);

       bool write(byte_stream_writer& writer);
       bool read(byte_stream_reader& reader);

       uint8  m_type = 0;
       uint32 m_tick = 0;
       uint8  m_movement_request = 0;
   };

   struct snapshot_message {
       snapshot_message() = default;
       snapshot_message(snapshot shot, uint32 tick);

       bool write(byte_stream_writer& writer);
       bool read(byte_stream_reader& reader);

       uint8    m_type = 0;
       uint32   m_tick = 0;
       snapshot m_shot = {};
   };
} // !meteor
