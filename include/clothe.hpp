#pragma once

#include <vector>
#include "Physics.hpp"

namespace clothe_helper {
    inline int Grid2Index(int x, int y, int size) {
        return x + y * size;
    }
};

struct Clothe {
    std::vector<Point> points;
    std::vector<Spring> springs;

    static Clothe Create(int size, float spacing, float stiffness);
    void applyForces();
    void update();
    void draw() const;
    /// @brief Runs the simulation until all points settle
    void stabilize();

private:
    Clothe(std::vector<Point>&& points, std::vector<Spring>&& springs)
        : points(std::move(points)), springs(std::move(springs)) {}
};

namespace helper
{
    struct UserInputManager {
        float& time_scale;
        Clothe& clothe;
        float paused_time;
        bool paused;
        Point* closest_to_mouse;
        UserInputManager(float& time_scale, Clothe& clothe) : time_scale(time_scale), clothe(clothe), paused_time(time_scale), paused(true), closest_to_mouse(nullptr) {
            time_scale = 0.0f;
        }
        void update();
    };
} // namespace helper
