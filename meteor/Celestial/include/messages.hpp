// messages.hpp

#pragma once

#include "raylib.h"
#include "network.hpp"
#include "game.hpp"

namespace meteor
{
   enum class message_type : uint8 {
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
