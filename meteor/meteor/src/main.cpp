// main.cpp

#include <thread>
#include <chrono>
#include <cstdio>

#include "raylib.h"
#include "timer.hpp"
#include "network_layer.hpp"
#include "server.hpp"

int main(int argc, char **argv)
{
   using namespace meteor;
   network::startup boot;
   //application* app;

   application app;

   const int WINDOW_WIDTH = 1200,
       WINDOW_HEIGHT = 1200;

   InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Meteor engine");
   
   network_layer network_layer;
   if (!network_layer.open()) {
      return 0;
   }

   app.init();

   bool running = true;
   while (running) {

       app.update();

       

      //std::vector<Client> &clientsapp->update();
      // note: save the forest!
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
   }

   network_layer.close();

   return 0;
}
