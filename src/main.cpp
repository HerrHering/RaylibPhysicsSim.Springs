#include <iostream>

#include <Physics.hpp>
#include <vector>

#include <raymath.h>

int main() {
    InitWindow(screen_width(), screen_height(), "2D Spring Simulation");
    const int targetFPS = static_cast<int>(std::round(1.0f / dt()));
    SetTargetFPS(targetFPS);

    // Physics setup
    float accumulator = 0.0f;
    float time_scale = 1.0f;

    std::vector<Point> points;
    points.emplace_back(Vector2{ 0, 5 }, true);
    points.emplace_back(Vector2{ 0, 0 }, true);

    std::vector<Spring> springs;
    springs.emplace_back(points[0], points[1], 4.0f, 5.0f);

    // Attach Tracker
    helper::PointTracer tracer{points[1]};

    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        accumulator += GetFrameTime() * time_scale;

        while (accumulator >= dt()) {
            // Update physics with a fixed time step
            for (auto& spring : springs) {
                spring.applyConstraint();
            }

            for (auto& point : points) {
                // Add gravity
                point.applyForce(Vector2Scale(gravity(), point.mass));
                helper::drawPointVectors(point, 0.2f, 0.2f);
                point.update();
            }
            accumulator -= dt();
        }

        // Handle user input
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            points[1].is_locked = false;
        } else {
            points[1].is_locked = true;
        }
        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
            points[1].position = helper::screenToWorld(GetMousePosition());
            points[1].update();

            // Clear tracer for point 1
            tracer.reset();
        }
        // Time scale
        if (IsKeyDown(KEY_ONE)) time_scale = 1.0f;
        else if (IsKeyDown(KEY_TWO)) time_scale = 2.0f;
        else if (IsKeyDown(KEY_THREE)) time_scale = 3.0f;
        else if (IsKeyDown(KEY_FOUR)) time_scale = 4.0f;
        else if (IsKeyDown(KEY_FIVE)) time_scale = 5.0f;


        // Draw
        BeginDrawing();
        ClearBackground(BLACK);

        // Quick FPS indicator
        {
            int currentFPS = GetFPS();
            char fpsText[32];
            sprintf(fpsText, "%d / %d FPS x%d", currentFPS, targetFPS, static_cast<int>(time_scale));
            // Draw the FPS text at the top-left corner.
            DrawText(fpsText, 10, 30, 20, LIME);
        }

        for (const auto& spring : springs) {
            spring.draw();
        }

        for (const auto& point : points) {
            point.draw();
        }

        // Draw tracer
        tracer.draw();

        DrawText("Click and drag the blue point", 10, 10, 20, LIGHTGRAY);

        EndDrawing();
    }

    // De-Initialization
    CloseWindow();

    return 0;
}