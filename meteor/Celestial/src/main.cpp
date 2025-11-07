#include <thread>
#include <chrono>
#include <cstdio>

#include "raylib.h"
#include "timer.hpp"
#include "network_layer.hpp"
#include "server.hpp"


#define _CLIENT

int main(int argc, char** argv)
{
    using namespace meteor;
    network::startup boot;
    //application* app;

    application app;

    constexpr uint16 PORT = 54321;
    const ip_endpoint SERVER_ENDPOINT(ip_address(192, 168, 1, 72), PORT);
    connection my_connection;


    const int WINDOW_WIDTH = 1200,
        WINDOW_HEIGHT = 700;

    int x = 500, y = 500;

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Meteor engine");

    SetExitKey(0);

    network_layer network_layer;
    if (!network_layer.open()) {
        return 0;
    }

    double next_tick_time = GetTime();

    app.init();

    bool running = true;
    while (running) {
        const float dt = GetFrameTime();
        //running &= WindowShouldClose();

        double time = GetTime();

        if (time >= next_tick_time) {
            next_tick_time += TICK_TIME;

            app.update();

            BeginDrawing();

            ClearBackground(GREEN);

            DrawText("Hello There", x, y, 40, BLACK);

            EndDrawing();
        }

        // note: save the forest!
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    network_layer.close();

    return 0;
}


