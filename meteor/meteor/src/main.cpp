// main.cpp

#include <thread>
#include <chrono>
#include <cstdio>

#include "raylib.h"
#include "timer.hpp"
#include "network_layer.hpp"

int main(int argc, char **argv)
{
   using namespace meteor;
   network::startup boot;
   
   network_layer network_layer;
   if (!network_layer.open()) {
      return 0;
   }

   bool running = true;
   while (running) {

      // note: save the forest!
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
   }

   network_layer.close();

   return 0;
}
