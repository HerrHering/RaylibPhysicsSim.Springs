#include <clothe.hpp>

#include <stdexcept>
#include <format>
#include <raymath.h>
#include <Globals.hpp>

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

    const float point_mass = 1.0f;
    // Note: The spring will be stretched in an equilibrium
    // Vector2 eq_spacing_offset = PhysicsConstants::gravity * (0.5f * point_mass / stiffness);

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            // Top row is static
            bool is_locked = y == size - 1;
            
            points.emplace_back(bottom_left + Vector2{x * spacing, y * spacing}, point_mass, is_locked);
            
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

void Clothe::applyForces() {
    for (const Spring& s : springs) {
        s.applyConstraint();
    }

    for (Point& p : points) {
        p.applyForce(PhysicsConstants::gravity * p.mass);
    }
}

void Clothe::update() {
    for (Point& p : points) {
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

void Clothe::stabilize() {
    if (springs.size() == 0)
        return;

    // Original values
    float mass = springs[0].p1.mass;
    float original_damp = springs[0].dampening;
    float strong_damp = helper::calc_dampening(mass, mass, springs[0].spring_constant, 0.99f);

    // Set dampening to really high
    for (Spring& s : springs) {
        s.dampening = strong_damp;
    }

    // Run simulation, until forces settle
    float thsreshold = mass * Vector2Length(PhysicsConstants::gravity) * 1e-3f;
    Vector2 sum_forces;
    do {
        sum_forces = Vector2{0.0f, 0.0f};
        // Apply forces
        applyForces();
        // Sum forces
        for (const Point& p : points) {
            sum_forces += p.total_force;
        }
        // Go next frame
        update();
    } while(Vector2Length(sum_forces) > thsreshold);

    // Reset dampening to original value
    for (Spring& s : springs) {
        s.dampening = original_damp;
    }
}

void helper::UserInputManager::update() {
    // Zoom
    if (IsKeyPressed(KEY_M)) {
        PIXELS_PER_METER *= 2.0f;
    }
    else if (IsKeyPressed(KEY_N)) {
        PIXELS_PER_METER *= 0.5f;
    }

    // Speed control
    if (!paused) {
        if (IsKeyPressed(KEY_ONE)) time_scale = 1.0f;
        else if (IsKeyPressed(KEY_TWO)) time_scale += 1.0f;
        else if (IsKeyPressed(KEY_THREE)) {
            time_scale -= 1.0f;
            time_scale = std::max(time_scale, 1.0f);
        }
    }
    else {
        if (IsKeyPressed(KEY_ONE)) paused_time = 1.0f;
        else if (IsKeyPressed(KEY_TWO)) paused_time += 1.0f;
        else if (IsKeyPressed(KEY_THREE)) {
            paused_time -= 1.0f;
            paused_time = std::max(paused_time, 1.0f);
        }
    }

    if (IsKeyPressed(KEY_SPACE)) {
        // Pause
        if (!paused) {
            paused_time = time_scale;
            time_scale = 0.0f;
            paused = true;
        }
        else {
            // Unpause
            time_scale = paused_time;
            paused = false;
        }
    }

    // Mouse drag feature
    // target point
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        // Find closest point to mouse
        float min = std::numeric_limits<float>::infinity();
        Point* closest = nullptr;
        for (Point& p : clothe.points) {
            if (closest == nullptr || Vector2DistanceSqr(helper::screenToWorld(GetMousePosition()), p.position) < min) {
                min = Vector2DistanceSqr(helper::screenToWorld(GetMousePosition()), p.position);
                closest = &p;
            }
        }
        closest_to_mouse = closest;

        // If we have a point, lock it
        if (closest_to_mouse != nullptr) {
            closest_to_mouse->is_locked = true;
        }
    }
    // drag
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        if (closest_to_mouse != nullptr) {
            closest_to_mouse->position = helper::screenToWorld(GetMousePosition());
        }
    }
    // release point
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        // If we have a point, unlock it, if it isnt top row
        if (closest_to_mouse != nullptr) {
            closest_to_mouse->is_locked = false;
        }
        closest_to_mouse = nullptr;
    }
    // Stabilize clothe net
    if (IsKeyPressed(KEY_S)) {
        clothe.stabilize();
    }
}