#pragma once

#include <raylib.h>
#include <deque>
#include <string>
#include <vector>

#include <Physics.hpp>

struct EnergyPlot {
    // Draw area
    Rectangle rect;
    std::deque<float> values;
    size_t max_size;
    Color color;

    EnergyPlot(Rectangle r, size_t max_points, Color c)
        : rect(r), values{}, max_size(max_points), color(c) {}

    void update(float newValue);
    void draw() const;
    float calculateEnergy(const std::vector<Point>& points, const std::vector<Spring>& springs) const;
};