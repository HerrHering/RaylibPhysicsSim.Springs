#include <Physics.hpp>

#include <raymath.h>

void Point::applyForce(Vector2 force) {
    if (is_locked)
        return;

    total_force = Vector2Add(force, total_force);
}

void Point::update() {
    // Static objects dont move
    if (is_locked) {
        old_position = position;
        total_force = {0.0f, 0.0f};
        return;
    }

    // Use Verlet to find new position
    // p_next = 2*p_current - p_old + a_current*dt^2
    Vector2 acc = Vector2Scale(total_force, 1.0f / mass);
    Vector2 p_next = Vector2Scale(position, 2.0f) - old_position + Vector2Scale(acc, dt() * dt());

    // Update positions
    old_position = position;
    position = p_next;

    // Reset for next frame
    total_force = {0.0f, 0.0f};
}

void Spring::applyConstraint() {
    // p2 <- p1
    Vector2 delta = Vector2Subtract(p2.position, p1.position);
    float distance = Vector2Length(delta);
    float stretch = distance - rest_length;
    // Force for p1, for p2 it will be its counter force
    Vector2 force_dir = Vector2Scale(delta, 1.0f / distance);
    Vector2 force = Vector2Scale(force_dir, stretch * spring_constant);

    p1.applyForce(force);
    p2.applyForce(Vector2Negate(force));
}
