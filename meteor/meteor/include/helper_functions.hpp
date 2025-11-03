//helper_functions.hpp

#pragma once

#include "raylib.h"
#include "raymath.h"

namespace meteor {


    static Color
    get_random_color()
    {
        Color color = WHITE;
        color.r = GetRandomValue(0, 255) & 0xff;
        color.g = GetRandomValue(0, 255) & 0xff;
        color.b = GetRandomValue(0, 255) & 0xff;
        color.a = 0xff;
        return color;
    }

    static bool timer_check(double& prev_time) {
        double current_time = GetTime();
        float timer = 2.0f;

        if ((current_time - prev_time) >= timer) {
            prev_time = current_time;
            return true;
        }
        else {
            return false;
        }
    }
}
