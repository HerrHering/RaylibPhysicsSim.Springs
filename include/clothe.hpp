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
    void update();
    void draw() const;

private:
    Clothe(std::vector<Point>&& points, std::vector<Spring>&& springs)
        : points(std::move(points)), springs(std::move(springs)) {}
};