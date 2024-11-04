#include "UseImGui.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include <cmath>
#include <limits>
#include <string>
#include <algorithm>
#include <thread>
#include <mutex>

// Define constants
constexpr double PI = 3.14159265358979323846;
constexpr float GRAVITY = 9.81f;
constexpr float MAX_FLOAT = std::numeric_limits<float>::max();

class CustomImGui : public UseImGui {
public:
    void Update() override {
        // Variables for calculations
        static float engine_torque = 250.0f;          // Nm
        static float engine_power = 100.0f;           // kW
        static float vehicle_mass = 1500.0f;          // kg
        static float wheel_radius = 0.3f;             // m
        static float transmission_efficiency = 0.85f; // 0-1
        static float gear_ratio = 0.0f;
        static float final_drive_ratio = 3.7f;
        static float Cd = 0.3f;                       // Drag coefficient
        static float frontal_area = 2.2f;             // m²
        static float Crr = 0.015f;                    // Rolling resistance
        static float air_density = 1.225f;            // kg/m³ at sea level, 15°C

        // New variables
        static float altitude = 0.0f;                 // m above sea level
        static float temperature = 15.0f;             // °C
        static float road_gradient = 0.0f;            // degrees
        static float tire_pressure = 2.2f;            // bar
        static int gear_count = 6;                    // number of gears
        static float gear_ratios[10] = { 3.5f, 2.5f, 1.8f, 1.3f, 1.0f, 0.8f }; // Up to 10 gears

        // Results
        static float traction_force = 0.0f;
        static float acceleration = 0.0f;
        static float max_speed = 0.0f;
        static float zero_to_hundred = 0.0f;
        static float fuel_consumption = 0.0f;
        static bool show_result = false;
        static std::string error_message;
        static std::mutex result_mutex;

        ImGui::Begin("Enhanced Vehicle Performance Calculator");

        // Basic Parameters
        if (ImGui::CollapsingHeader("Basic Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::InputFloat("Engine Torque (Nm)", &engine_torque);
            ImGui::InputFloat("Engine Power (kW)", &engine_power);
            ImGui::InputFloat("Vehicle Mass (kg)", &vehicle_mass);
            ImGui::InputFloat("Wheel Radius (m)", &wheel_radius);
        }

        // Transmission Parameters
        if (ImGui::CollapsingHeader("Transmission", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::InputFloat("Transmission Efficiency (0-1)", &transmission_efficiency);
            ImGui::InputInt("Number of Gears", &gear_count);
            gear_count = std::min(std::max(gear_count, 1), 10);

            for (int i = 0; i < gear_count; i++) {
                std::string gear_label = "Gear " + std::to_string(i + 1) + " Ratio";
                ImGui::InputFloat(gear_label.c_str(), &gear_ratios[i]);
            }
            ImGui::InputFloat("Final Drive Ratio", &final_drive_ratio);
        }

        // Aerodynamics and Rolling Resistance
        if (ImGui::CollapsingHeader("Aerodynamics & Resistance", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::InputFloat("Drag Coefficient (Cd)", &Cd);
            ImGui::InputFloat("Frontal Area (m^2)", &frontal_area);
            ImGui::InputFloat("Rolling Resistance Coefficient", &Crr);
            ImGui::InputFloat("Tire Pressure (bar)", &tire_pressure);
        }

        // Environmental Conditions
        if (ImGui::CollapsingHeader("Environmental Conditions")) {
            ImGui::InputFloat("Altitude (m)", &altitude);
            ImGui::InputFloat("Temperature (°C)", &temperature);
            ImGui::InputFloat("Road Gradient (degrees)", &road_gradient);

            // Calculate air density based on altitude and temperature
            float temperature_K = temperature + 273.15f;
            float pressure = 101325.0f * std::pow(1.0f - 2.25577e-5f * altitude, 5.25588f);
            air_density = pressure / (287.05f * temperature_K);

            ImGui::Text("Calculated Air Density: %.3f kg/m^3", air_density);
        }

        if (ImGui::Button("Calculate Performance")) {
            error_message.clear();
            show_result = false;

            // Validate inputs
            if (!ValidateInputs(engine_torque, engine_power, vehicle_mass, wheel_radius,
                transmission_efficiency, gear_ratios, gear_count, final_drive_ratio,
                Cd, frontal_area, Crr, tire_pressure, error_message)) {
                show_result = false;
            }
            else {
                // Start calculation in a new thread
                std::thread calc_thread([&]() {
                    CalculatePerformance(
                        engine_torque, engine_power, vehicle_mass, wheel_radius,
                        transmission_efficiency, gear_ratios, gear_count, final_drive_ratio,
                        Cd, frontal_area, Crr, air_density, road_gradient, tire_pressure,
                        traction_force, acceleration, max_speed, zero_to_hundred, fuel_consumption,
                        error_message
                    );
                    std::lock_guard<std::mutex> lock(result_mutex);
                    show_result = error_message.empty();
                    });
                calc_thread.detach();
            }
        }

        // Display error message if any
        if (!error_message.empty()) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", error_message.c_str());
        }

        // Results display
        if (show_result) {
            ImGui::Separator();
            ImGui::Text("Performance Results:");

            // Maximum performance
            ImGui::Text("Maximum Tractive Force: %.2f N", traction_force);
            ImGui::Text("Peak Acceleration: %.2f m/s^2 (%.2f g)", acceleration, acceleration / GRAVITY);
            ImGui::Text("Maximum Speed: %.2f km/h", max_speed * 3.6f);
            ImGui::Text("0-100 km/h: %.1f seconds", zero_to_hundred);

            // Efficiency metrics
            ImGui::Text("Estimated Fuel Consumption at 100 km/h: %.1f L/100km", fuel_consumption);

            // Power loss breakdown
            float v = 100.0f / 3.6f; // 100 km/h in m/s
            float aero_power = 0.5f * air_density * Cd * frontal_area * std::pow(v, 3);
            float rolling_power = Crr * vehicle_mass * GRAVITY * v;
            float gradient_power = vehicle_mass * GRAVITY * std::sin(static_cast<float>(road_gradient * PI / 180.0f)) * v;

            ImGui::Text("\nPower Loss Breakdown at 100 km/h:");
            ImGui::Text("Aerodynamic Resistance: %.2f kW", aero_power / 1000.0f);
            ImGui::Text("Rolling Resistance: %.2f kW", rolling_power / 1000.0f);
            ImGui::Text("Gradient Resistance: %.2f kW", gradient_power / 1000.0f);
        }

        ImGui::End();
    }

private:
    bool ValidateInputs(float engine_torque, float engine_power, float vehicle_mass, float wheel_radius,
        float transmission_efficiency, float* gear_ratios, int gear_count,
        float final_drive_ratio, float Cd, float frontal_area, float Crr,
        float tire_pressure, std::string& error_message) {
        if (engine_torque <= 0.0f) return SetError(error_message, "Engine torque must be positive");
        if (engine_power <= 0.0f) return SetError(error_message, "Engine power must be positive");
        if (vehicle_mass <= 0.0f) return SetError(error_message, "Vehicle mass must be positive");
        if (wheel_radius <= 0.0f) return SetError(error_message, "Wheel radius must be positive");
        if (transmission_efficiency <= 0.0f || transmission_efficiency > 1.0f)
            return SetError(error_message, "Transmission efficiency must be between 0 and 1");
        if (final_drive_ratio <= 0.0f) return SetError(error_message, "Final drive ratio must be positive");
        if (Cd < 0.1f || Cd > 2.0f) return SetError(error_message, "Invalid drag coefficient (typical range: 0.1-2.0)");
        if (frontal_area <= 0.0f) return SetError(error_message, "Frontal area must be positive");
        if (Crr <= 0.0f) return SetError(error_message, "Rolling resistance coefficient must be positive");
        if (tire_pressure <= 0.0f) return SetError(error_message, "Tire pressure must be positive");

        for (int i = 0; i < gear_count; i++) {
            if (gear_ratios[i] <= 0.0f)
                return SetError(error_message, "All gear ratios must be positive");
        }

        return true;
    }

    bool SetError(std::string& error_message, const char* message) {
        error_message = message;
        return false;
    }

    void CalculatePerformance(
        float engine_torque, float engine_power, float vehicle_mass, float wheel_radius,
        float transmission_efficiency, float* gear_ratios, int gear_count, float final_drive_ratio,
        float Cd, float frontal_area, float Crr, float air_density, float road_gradient,
        float tire_pressure, float& traction_force, float& acceleration, float& max_speed,
        float& zero_to_hundred, float& fuel_consumption, std::string& error_message) {

        std::lock_guard<std::mutex> lock(result_mutex);

        // Calculate maximum tractive force in first gear
        float max_gear_ratio = gear_ratios[0];
        traction_force = (engine_torque * max_gear_ratio * final_drive_ratio * transmission_efficiency) / wheel_radius;

        // Calculate peak acceleration (in first gear)
        float gradient_resistance = vehicle_mass * GRAVITY * std::sin(static_cast<float>(road_gradient * PI / 180.0f));
        float rolling_resistance = Crr * vehicle_mass * GRAVITY * std::cos(static_cast<float>(road_gradient * PI / 180.0f));
        // Adjust Crr based on tire pressure (simplified model)
        rolling_resistance *= std::sqrt(2.2f / tire_pressure);

        acceleration = (traction_force - rolling_resistance - gradient_resistance) / vehicle_mass;

        // Prevent negative or zero acceleration
        if (acceleration <= 0.0f) {
            SetError(error_message, "Vehicle cannot accelerate with the given parameters.");
            return;
        }

        // Calculate maximum speed through iterative method
        float power_available = engine_power * 1000.0f; // Convert kW to W
        float v_min = 0.0f;
        float v_max = 200.0f; // m/s

        // Binary search for maximum speed
        for (int i = 0; i < 100; ++i) {
            float v = (v_min + v_max) / 2.0f;
            float aero_resistance = 0.5f * air_density * Cd * frontal_area * v * v;
            float total_resistance = aero_resistance + rolling_resistance + gradient_resistance;
            float power_required = total_resistance * v;

            if (power_required > power_available) {
                v_max = v;
            }
            else {
                v_min = v;
            }
        }
        max_speed = v_min;

        // Calculate 0-100 km/h time through numerical integration
        float dt = 0.01f;
        float v = 0.0f;
        float t = 0.0f;
        int current_gear = 0;
        const int max_iterations = 100000;
        int iteration = 0;

        while (v < 27.78f && iteration < max_iterations) { // 100 km/h = 27.78 m/s
            // Determine optimal gear
            current_gear = DetermineOptimalGear(v, gear_ratios, gear_count, engine_power, wheel_radius, final_drive_ratio);

            float wheel_torque = engine_torque * gear_ratios[current_gear] * final_drive_ratio * transmission_efficiency;
            float current_traction = wheel_torque / wheel_radius;

            float aero_resistance = 0.5f * air_density * Cd * frontal_area * v * v;
            float total_resistance = aero_resistance + rolling_resistance + gradient_resistance;

            float adjusted_velocity = std::max(v, 1.0f); // Avoid division by zero
            float max_force = power_available / adjusted_velocity;
            float net_force = std::min(current_traction, max_force) - total_resistance;
            float a = net_force / vehicle_mass;

            if (a <= 0.0f) {
                SetError(error_message, "Calculation aborted: Non-positive acceleration encountered.");
                return;
            }

            v += a * dt;
            t += dt;
            iteration++;
        }

        if (iteration >= max_iterations) {
            SetError(error_message, "Calculation aborted: Maximum iterations reached.");
            return;
        }

        zero_to_hundred = t;

        // Estimate fuel consumption at 100 km/h
        float v_cruise = 27.78f; // 100 km/h in m/s
        float power_at_cruise = (0.5f * air_density * Cd * frontal_area * std::pow(v_cruise, 3) +
            rolling_resistance * v_cruise + gradient_resistance * v_cruise);

        // Assume a typical engine efficiency curve
        float engine_efficiency = 0.25f + 0.10f * (power_at_cruise / power_available); // Simplified efficiency model
        fuel_consumption = (power_at_cruise / (engine_efficiency * 42000000.0f)) * 3600.0f * 100.0f; // L/100km
    }

    // Updated function signature to include final_drive_ratio
    int DetermineOptimalGear(float velocity, float* gear_ratios, int gear_count, float engine_power, float wheel_radius, float final_drive_ratio) {
        float optimal_engine_speed = 5000.0f; // RPM where peak power typically occurs
        int optimal_gear = 0;
        float min_diff = MAX_FLOAT;

        for (int i = 0; i < gear_count; i++) {
            float engine_speed = (velocity * gear_ratios[i] * final_drive_ratio) / (2.0f * PI * wheel_radius) * 60.0f;
            float diff = std::abs(engine_speed - optimal_engine_speed);
            if (diff < min_diff) {
                min_diff = diff;
                optimal_gear = i;
            }
        }

        return optimal_gear;
    }

    std::mutex result_mutex;
};

int main()
{
    // Setup window
    if (!glfwInit())
        return 1;

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Vehicle Performance Calculator", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw("Unable to initialize OpenGL context");

    int screen_width, screen_height;
    glfwGetFramebufferSize(window, &screen_width, &screen_height);
    glViewport(0, 0, screen_width, screen_height);

    CustomImGui myimgui;
    myimgui.Init(window, glsl_version);

    // Set initial clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.00f);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);
        myimgui.NewFrame();
        myimgui.Update();
        myimgui.Render();
        glfwSwapBuffers(window);
    }
    myimgui.Shutdown();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
