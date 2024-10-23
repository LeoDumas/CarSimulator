#include <iostream>

// Global variables
float engine_torque = 0.0f;
float wheel_radius = 0.0f;
float transmission_efficiency = 0.0f;
float traction_force = 0.0f;

float car_mass = 0.0f;
float car_acceleration = 0.0f;
float gear_ratio = 0.0f;
float differential_ratio = 0.0f;

// Function to validate inputs
bool validate_input(const float value, const std::string& name, const float min_value = 0.0f, const float max_value = -1.0f) {
    if (value <= min_value || (max_value > 0 && value > max_value)) {
        std::cerr << "Error: " << name << " must be ";
        if (max_value > 0) {
            std::cerr << "between " << min_value << " and " << max_value << ".";
        } else {
            std::cerr << "greater than " << min_value << ".";
        }
        std::cerr << std::endl;
        return false;
    }
    return true;
}

// Function to input vehicle parameters
bool get_car_parameters() {
    std::cout << "Enter car mass (kg): ";
    std::cin >> car_mass;
    if (!validate_input(car_mass, "Car mass", 0)) return false;

    std::cout << "Enter engine torque (Nm): ";
    std::cin >> engine_torque;
    if (!validate_input(engine_torque, "Engine torque", 0)) return false;

    std::cout << "Enter wheel radius (m): ";
    std::cin >> wheel_radius;
    if (!validate_input(wheel_radius, "Wheel radius", 0)) return false;

    std::cout << "Enter transmission efficiency (0-1): ";
    std::cin >> transmission_efficiency;
    if (!validate_input(transmission_efficiency, "Transmission efficiency", 0, 1)) return false;

    std::cout << "Enter gear ratio: ";
    std::cin >> gear_ratio;
    if (!validate_input(gear_ratio, "Gear ratio", 0)) return false;

    std::cout << "Enter differential ratio: ";
    std::cin >> differential_ratio;
    if (!validate_input(differential_ratio, "Differential ratio", 0)) return false;

    return true;
}

// Function to calculate traction force
void calculate_traction_force() {
    // Traction force F = (T * efficiency * gear_ratio * differential_ratio) / wheel_radius
    traction_force = (engine_torque * transmission_efficiency * gear_ratio * differential_ratio) / wheel_radius;
}

// Function to calculate acceleration
void calculate_acceleration() {
    // Calculate acceleration from the traction force
    car_acceleration = traction_force / car_mass;
}

int main() {
    // Input vehicle parameters
    if (!get_car_parameters()) {
        return -1;  // Exit if an input error is detected
    }

    // Calculate traction force
    calculate_traction_force();
    std::cout << "Traction Force: " << traction_force << " N" << std::endl;

    // Calculate acceleration
    calculate_acceleration();
    std::cout << "Car Acceleration: " << car_acceleration << " m/s^2" << std::endl;

    return 0;
}
