#include <clothe.hpp>

#include <stdexcept>
#include <format>
#include <raymath.h>

Clothe Clothe::Create(int size, float spacing, float stiffness)
{
    if (size <= 0)
        throw std::runtime_error(std::format("Clothe size must be > 0. Current value: {}", size));
    if (spacing <= 0.0f)
        throw std::runtime_error(std::format("Clothe spaving should be positive. Spacing: {}", spacing));

    // Fill grid & connect with springs
    float half_extent = (float)size * 0.5f * spacing;
    Vector2 bottom_left{-half_extent, -half_extent};
    std::vector<Point> points;
    std::vector<Spring> springs;
    points.reserve(static_cast<size_t>(size * size));
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            // Top row is static
            bool is_locked = y == size - 1;

            points.emplace_back(bottom_left + Vector2{x * spacing, y * spacing}, 1.0f, is_locked);

            // Link springs left-down
            float damp = 0.8f;
            if (x > 0) {
                // Left
                springs.emplace_back(
                    points[clothe_helper::Grid2Index(x - 1, y, size)], 
                    points[clothe_helper::Grid2Index(x, y, size)], 
                    spacing, stiffness, damp
                );
            }
            if (y > 0) {
                // Down
                springs.emplace_back(
                    points[clothe_helper::Grid2Index(x, y - 1, size)], 
                    points[clothe_helper::Grid2Index(x, y, size)], 
                    spacing, stiffness, damp
                );
            }
        }
    }

    return Clothe{
        std::move(points),
        std::move(springs)
    };
}

void Clothe::update() {
    for (const Spring& s : springs) {
        s.applyConstraint();
    }

    for (Point& p : points) {
        p.applyForce(PhysicsConstants::gravity * p.mass);
        p.update();
    }
}

void Clothe::draw() const {
    for (const Spring& s : springs) {
        s.draw();
    }

    for (const Point& p : points) {
        p.draw();
    }
}
