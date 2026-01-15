#pragma once

#include <raylib.h>

/// @brief Physics constant
inline const float& dt() {
    static const float dt = 0.01f;
    return dt;
}

inline const Vector2& gravity() {
    static const Vector2 g{0.0f, -9.8f};
    return g;
}

inline const int& screen_width() {
    static const int screen_width = 800;
    return screen_width;
}
inline const int& screen_height() {
    static const int screen_height = 600;
    return screen_height;
}

/// @brief Physics constant 
inline const float& PIXELS_PER_METER() {
    static const float ratio = 50.0f;
    return ratio;
}

namespace helper
{
    /// @brief Origin at center, right-up
    /// @param world_pos 
    /// @return 
    inline Vector2 worldToScreen(Vector2 world_pos) {
        return Vector2{screen_width() * 0.5f + world_pos.x * PIXELS_PER_METER(), screen_height() * 0.5f - world_pos.y * PIXELS_PER_METER()};
    }
} // namespace helper


struct Point {
    Vector2 position;
    float mass;
    bool is_locked;
    Vector2 old_position;
    Vector2 total_force;
    
    /// @brief Initializes the point with zero initial velocity
    /// @param position The (starting) position of the Point
    Point(Vector2 position, float mass = 1.0f)
    : position(position), mass(mass), is_locked(false), old_position(position), total_force({0.0f, 0.0f}) {}
    /// @brief Initializes the point with zero initial velocity
    /// @param position The (starting) position of the Point
    /// @param is_locked Special parameter for making a point STATIC 
    Point(Vector2 position, bool is_locked = false)
    : position(position), mass(1.0f), is_locked(is_locked), old_position(position), total_force({0.0f, 0.0f}) {}
    
    /// @brief Applies force to the point
    void applyForce(Vector2 force);
    /// @brief Updates the position of the Point, given the accumulated forces
    void update();
    /// @brief Draws a Point on the screen
    void draw() const {
        DrawCircleV(helper::worldToScreen(position), 10, is_locked ? RED : BLUE);
    }
};

/// @brief Creates a Spring constraint between two Points
struct Spring {
    Point& p1;
    Point& p2;
    float rest_length;
    float spring_constant;

    Spring(Point& p1, Point& p2, float rest_len, float spring_k)
        : p1(p1), p2(p2), rest_length(rest_len), spring_constant(spring_k) {}

    /// @brief Applies the spring forces to the points
    void applyConstraint();
    /// @brief Draws a Point on the screen
    void draw() const {
        DrawLineV(helper::worldToScreen(p1.position), helper::worldToScreen(p2.position), GRAY);
    }
};