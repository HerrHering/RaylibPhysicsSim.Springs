#pragma once

#include <Globals.hpp>

#include <raylib.h>
#include <raymath.h>

#include <deque>
#include <vector>
#include <math.h>

struct Point;

namespace helper
{
    using namespace PhysicsConstants;

    /// @brief Origin at center, right-up
    /// @param world_pos 
    /// @return 
    inline Vector2 worldToScreen(Vector2 world_pos) {
        world_pos -= camera_offset;
        return Vector2{sim_screen_width * 0.5f + world_pos.x * PIXELS_PER_METER, sim_screen_height * 0.5f - world_pos.y * PIXELS_PER_METER};
    }
    /// @brief Origin at top left, right-down
    /// @param screen_pos
    /// @return 
    inline Vector2 screenToWorld(Vector2 screen_pos) {
        return Vector2{(screen_pos.x - sim_screen_width * 0.5f) / PIXELS_PER_METER, (sim_screen_height * 0.5f - screen_pos.y) / PIXELS_PER_METER} + camera_offset;
    }

    void DrawArrow(Vector2 start, Vector2 end, float thickness, Color color);

    /// @brief Draws velocity vectors and force vectors
    /// @param point 
    /// @param vel_scale 
    /// @param force_scale 
    void drawPointVectors(const Point& point, float vel_scale, float force_scale);

    float calc_dampening(float m1, float m2, float k, float damp_per_cycle);

    struct PointTracer {
        const Point& point;
        size_t max_history_len;
        bool permanent;
        std::deque<Vector2> history;
        Color color;

        PointTracer(const Point& target, float len_seconds, Color color = PURPLE)
            : point(target), max_history_len(static_cast<size_t>(len_seconds / dt)), permanent(false), history{}, color(color) {
        }

        PointTracer(const Point& target, Color color = PURPLE)
            : point(target), max_history_len(0), permanent(true), history{}, color(color) {
        }
        
        void reset();
        void update();
        void draw();
    };

    struct Tracermanager {
        std::vector<PointTracer*> tracers;
        void update() {
            for (auto tracer : tracers) {
                tracer->update();
            }
        }
        void reset() {
            for (auto tracer : tracers) {
                tracer->reset();
            }
        }
        void draw() {
            for (auto tracer : tracers) {
                tracer->draw();
            }
        }
    };
} // namespace helper


struct Point {
    Vector2 position;
    float mass;
    bool is_locked;
    Vector2 old_position;
    Vector2 total_force;
    
    /// @brief Initializes the point with zero initial velocity
    /// @param position The (starting) position of the Point
    Point(Vector2 position, float mass = 1.0f, bool is_locked = false)
    : position(position), mass(mass), is_locked(is_locked), old_position(position), total_force({0.0f, 0.0f}) {}
    /// @brief Initializes the point with zero initial velocity
    /// @param position The (starting) position of the Point
    /// @param is_locked Special parameter for making a point STATIC 
    Point(Vector2 position, bool is_locked = false)
    : position(position), mass(1.0f), is_locked(is_locked), old_position(position), total_force({0.0f, 0.0f}) {}
    
    Vector2 getVel() const;

    /// @brief Applies force to the point
    void applyForce(Vector2 force);
    /// @brief Updates the position of the Point, given the accumulated forces
    void update();
    /// @brief Draws a Point on the screen
    void draw() const;
};

/// @brief Creates a Spring constraint between two Points
struct Spring {
    Point& p1;
    Point& p2;
    float rest_length;
    float spring_constant;
    float dampening;

    /// @brief 
    /// @param p1 
    /// @param p2 
    /// @param rest_len 
    /// @param spring_k 
    /// @param dampening_per_cycle Decides what percentage of energy should be lost per cycle (between 0-1)
    Spring(Point& p1, Point& p2, float rest_len, float spring_k, float dampening_per_cycle = 0.1f)
        : p1(p1), p2(p2), rest_length(rest_len), spring_constant(spring_k), dampening(0.0f) {
            dampening = helper::calc_dampening(p1.mass, p2.mass, spring_constant, dampening_per_cycle);
        }

    /// @brief Applies the spring forces to the points
    void applyConstraint() const;
    /// @brief Draws a Point on the screen
    void draw() const {
        DrawLineV(helper::worldToScreen(p1.position), helper::worldToScreen(p2.position), GRAY);
    }
};