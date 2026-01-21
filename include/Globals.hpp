#pragma once

#include <raylib.h>

namespace PhysicsConstants
{
    inline float dt = 1.0f / 60.0f;
    inline Vector2 gravity{0.0f, -9.8f};
    inline int screen_width = 800;
    inline int screen_height = 600;
    inline float ratio = 50.0f;
    inline float PIXELS_PER_METER = 50.0f;
} // namespace constants