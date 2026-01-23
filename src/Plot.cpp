#include <Plot.hpp>

#include <iostream>
#include <algorithm>
#include <raymath.h>
#include <Globals.hpp>

void EnergyPlot::update(float newValue) {
    values.push_back(newValue);
    if (values.size() > max_size)
        values.pop_front();
}

void EnergyPlot::draw() const {
    DrawRectangleRec(rect, GRAY);
    DrawRectangleLinesEx(rect, 1.0f, LIGHTGRAY);
    DrawText("Sim Plot", (int)rect.x + 15, (int)rect.y + 5, 20, WHITE);

    if (values.size() < 2) {
        std::cout << "Not enough data for plot! (num " << values.size() << ")\n";
        return;
    }

    // Find min and max values to scale the graph vertically
    auto [min_it, max_it] = std::minmax_element(values.begin(), values.end());
    float min_val = *min_it;
    float max_val = *max_it;
    float mid_val = 0.5f * (min_val + max_val);
    float range = std::max(1.0f, max_val - min_val);
    // Readjust min max value if range is too small
    min_val = mid_val - 0.5f * range;
    max_val = mid_val + 0.5f * range;


    //auto get_x = [&](int i) { return rect.x + (float)i / (max_size - 1) * rect.width; };

    // Draw the line plot
    for (int i = 0; i < (int)values.size() - 1; ++i) {
        // Map the value and index to screen coordinates within the rectangle
        Vector2 start = {
            rect.x + (float)i / (max_size - 1) * rect.width,
            rect.y + rect.height - ((values[i] - min_val) / range * rect.height)
        };
        Vector2 end = {
            rect.x + (float)(i + 1) / (max_size - 1) * rect.width,
            rect.y + rect.height - ((values[i+1] - min_val) / range * rect.height)
        };
        DrawLineV(start, end, color);
    }

    // Draw min/max value labels
    DrawText(TextFormat("%.2f", max_val), (int)rect.x + 5, (int)rect.y + 20, 10, LIGHTGRAY);
    DrawText(TextFormat("%.2f", min_val), (int)rect.x + 5, (int)rect.y + (int)rect.height - 15, 10, LIGHTGRAY);
}

float EnergyPlot::calculateEnergy(const std::vector<Point> &points, const std::vector<Spring> &springs) const
{
    const Vector2 gravitational_pot_reference{0.0f, 0.0f};

    float total = 0.0f;
    // Average Potential Energy over (t, t+dt)
    float pot = 0.0f;
    // Spring: 1/2 * k * dx^2
    for (const auto& spring : springs) {
        // Calculate the average position of each endpoint over the last time step
        Vector2 avg_p1_pos = Vector2Scale(Vector2Add(spring.p1.position, spring.p1.old_position), 0.5f);
        Vector2 avg_p2_pos = Vector2Scale(Vector2Add(spring.p2.position, spring.p2.old_position), 0.5f);

        float stretch = Vector2Distance(avg_p1_pos, avg_p2_pos) - spring.rest_length;
        pot += 0.5f * spring.spring_constant * stretch * stretch;
    }
    // Gravitational
    for (const auto& p : points) {
        if (p.is_locked)
        continue;
        
        Vector2 avg_pos = Vector2Scale(Vector2Add(p.position, p.old_position), 0.5f);
        // Height in the direction of gravity
        
        float height_times_g = Vector2DotProduct(Vector2Subtract(gravitational_pot_reference, avg_pos), PhysicsConstants::gravity);
        pot += p.mass * height_times_g;
    }
    total += pot;
    
    // Average Kinetic Energy over (t, t+dt)
    float kin = 0.0f;
    for (const auto& p : points) {
        if (p.is_locked)
            continue;

        float speed = Vector2Distance(p.position, p.old_position) / PhysicsConstants::dt;
        kin += 0.5f * p.mass * speed * speed;
    }
    total += kin;

    return total;
}
