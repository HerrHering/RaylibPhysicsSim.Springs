#include <iostream>

#include <Physics.hpp>
#include <Plot.hpp>
#include <vector>
#include <clothe.hpp>

#include <raymath.h>
#include <iostream>

int main() {
    const int graph_height = 150;
    const int total_screen_height = PhysicsConstants::sim_screen_height + graph_height;
    InitWindow(PhysicsConstants::sim_screen_width, total_screen_height, "2D Spring Simulation");
    const int targetFPS = static_cast<int>(std::round(1.0f / PhysicsConstants::dt));
    SetTargetFPS(targetFPS);

    // Graph drawer
    EnergyPlot plot{Rectangle{.x = 0, .y = PhysicsConstants::sim_screen_height, .width = PhysicsConstants::sim_screen_width, .height = graph_height}, 1000, RED};

    // Clothe sim
    Clothe clothe = Clothe::Create(10, 1.0f, 100.0f);
    (void)clothe;

    // Physics setup
    float accumulator = 0.0f;
    float time_scale = 1.0f;

    // Attach Tracker
    helper::Tracermanager tracers;

    helper::UserInputManager input_manager{time_scale};

    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        const float delta_time = GetFrameTime();
        accumulator += delta_time * time_scale;

        while (accumulator >= PhysicsConstants::dt) {
            // Update physics with a fixed time step
            
            clothe.update();

            accumulator -= PhysicsConstants::dt;

            // Update tracer(s)
            tracers.update();
        }

        // Handle user input
        input_manager.update();

        // if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        //     points[1].position = helper::screenToWorld(GetMousePosition());
        //     points[1].update();

        //     // Clear tracer(s)
        //     tracers.reset();
        // }
        // else if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
        //     points[2].position = helper::screenToWorld(GetMousePosition());
        //     points[2].update();

        //     // Clear tracer(s)
        //     tracers.reset();
        // }


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

        // We want to put a graph below the simulation window, and we dont want to overdraw it
        BeginScissorMode(0, 0, PhysicsConstants::sim_screen_width, PhysicsConstants::sim_screen_height);
        
        clothe.draw();

        // Draw (accumulated) tracer
        tracers.draw();
        EndScissorMode(); // End simulation window

        DrawText("Click and drag the blue point", 10, 10, 20, LIGHTGRAY);

        // Draw graph
        if (!input_manager.paused) {
            plot.update(plot.calculateEnergy(clothe.points, clothe.springs));
            plot.draw();
        }

        EndDrawing();
    }

    // De-Initialization
    CloseWindow();

    return 0;
}