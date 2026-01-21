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

void helper::DrawArrow(Vector2 start, Vector2 end, float thickness, Color color) {
    start = worldToScreen(start);
    end = worldToScreen(end);

    DrawLineEx(start, end, thickness, color);

    // Don't draw arrowhead if the vector is zero length
    if (Vector2Equals(start, end)) return;

    Vector2 direction = Vector2Normalize(Vector2Subtract(end, start));
    // Arrowhead perpendiculars
    Vector2 p1 = Vector2Add(end, Vector2Rotate(Vector2Scale(direction, -10.0f), 45 * DEG2RAD));
    Vector2 p2 = Vector2Add(end, Vector2Rotate(Vector2Scale(direction, -10.0f), -45 * DEG2RAD));

    DrawLineV(end, p1, color);
    DrawLineV(end, p2, color);
}

void helper::drawPointVectors(const Point &point, float vel_scale, float force_scale) {
    if (point.is_locked) return;

    // 1. Draw Velocity vector (derived from the change in position)
    Vector2 velocity = Vector2Subtract(point.position, point.old_position);
    Vector2 velocity_end_pos = Vector2Add(point.position, Vector2Scale(velocity, vel_scale / dt()));
    DrawArrow(point.position, velocity_end_pos, 2.0f, GREEN);

    // 2. Draw Acceleration vector
    Vector2 force_end_pos = Vector2Add(point.position, Vector2Scale(point.total_force, force_scale));
    DrawArrow(point.position, force_end_pos, 2.0f, YELLOW);
}

void helper::PointTracer::reset() {
    history.clear();
}

void helper::PointTracer::update() {
    // Cycle points
    history.push_back(point.position);
    if (!permanent && history.size() > max_history_len) {
        history.pop_front();
    }
}

void helper::PointTracer::draw()
{
    // Draw trail
    for (int i = 0; i < (int)history.size() - 1; ++i) {
        // Fade the line's color based on its age to create a "tail" effect
        float alpha = permanent ? 1.0f : (float)i / (float)history.size();
        DrawLineV(helper::worldToScreen(history[i]), helper::worldToScreen(history[i + 1]), Fade(PURPLE, alpha));
    }
}
