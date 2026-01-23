#pragma once

#include <raylib.h>

namespace PhysicsConstants
{
    inline float dt = 1.0f / 60.0f;
    inline Vector2 gravity{0.0f, -9.8f};
    inline const int sim_screen_width = 800;
    inline const int sim_screen_height = 600;
    inline float PIXELS_PER_METER = 50.0f;
} // namespace constants