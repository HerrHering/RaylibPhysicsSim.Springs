#include <iostream>

#include <Physics.hpp>
#include <vector>

#include <raymath.h>
#include <iostream>

int main() {
    InitWindow(PhysicsConstants::screen_width, PhysicsConstants::screen_height, "2D Spring Simulation");
    const int targetFPS = static_cast<int>(std::round(1.0f / PhysicsConstants::dt));
    SetTargetFPS(targetFPS);

    // Physics setup
    float accumulator = 0.0f;
    float time_scale = 1.0f;

    std::vector<Point> points;
    points.emplace_back(Vector2{ 0, 5 }, true);
    points.emplace_back(Vector2{ 0, 0 }, 1.0f);
    points.emplace_back(Vector2{ 0, -5 }, 1.0f);


    std::vector<Spring> springs;
    springs.emplace_back(points[0], points[1], 3.0f, 50.0f);
    springs.emplace_back(points[1], points[2], 3.0f, 50.0f);

    // Attach Tracker
    helper::PointTracer tracer1{points[1], PURPLE};
    helper::PointTracer tracer2{points[2], BLUE};
    helper::Tracermanager tracers;
    //tracers.tracers.push_back(&tracer1);
    tracers.tracers.push_back(&tracer2);

    helper::UserInputManager input_manager{time_scale};

    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        const float delta_time = GetFrameTime();
        accumulator += delta_time * time_scale;

        while (accumulator >= PhysicsConstants::dt) {
            // Update physics with a fixed time step
            for (auto& spring : springs) {
                spring.applyConstraint();
            }

            for (auto& point : points) {
                // Add gravity
                point.applyForce(Vector2Scale(PhysicsConstants::gravity, point.mass));
                helper::drawPointVectors(point, 0.2f, 0.2f);
                point.update();
            }
            accumulator -= PhysicsConstants::dt;

            // Update tracer(s)
            tracers.update();
        }

        // Handle user input
        input_manager.update();

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            points[1].position = helper::screenToWorld(GetMousePosition());
            points[1].update();

            // Clear tracer(s)
            tracers.reset();
        }
        else if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
            points[2].position = helper::screenToWorld(GetMousePosition());
            points[2].update();

            // Clear tracer(s)
            tracers.reset();
        }


        // Draw
        BeginDrawing();
        ClearBackground(BLACK);

        // Quick FPS indicator
        {
            int currentFPS = GetFPS();
            char fpsText[32];
            sprintf(fpsText, "%d / %d FPS", currentFPS, targetFPS);
            // Draw the FPS text at the top-left corner.
            DrawText(fpsText, 10, 30, 20, LIME);

            if (input_manager.paused) {
                char speedText[32];
                sprintf(speedText, "SPEED: x%.1f", input_manager.paused_time);
                DrawText(speedText, 10, 50, 20, RED);
            }
            else {
                char speedText[32];
                sprintf(speedText, "SPEED: x%.1f", input_manager.time_scale);
                DrawText(speedText, 10, 50, 20, LIME);
            }
        }

        for (const auto& spring : springs) {
            spring.draw();
        }

        for (const auto& point : points) {
            point.draw();
        }

        // Draw (accumulated) tracer
        tracers.draw();

        DrawText("Click and drag the blue point", 10, 10, 20, LIGHTGRAY);

        EndDrawing();
    }

    // De-Initialization
    CloseWindow();

    return 0;
}