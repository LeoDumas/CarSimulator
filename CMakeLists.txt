cmake_minimum_required(VERSION 3.8)
project(carSim2)

find_package(glad CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

add_executable(carSimulator "carSim2.cpp" "UseImGui.cpp" "UseImGui.h")

target_link_libraries(carSimulator PRIVATE glad::glad imgui::imgui glfw)
