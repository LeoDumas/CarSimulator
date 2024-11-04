# Vehicle Performance Calculator or CarSim (idk)

## Not all calculations are perfect, I’m working on them as I learn the mechanics behind cars. Mistakes may have been made.

![Example](https://github.com/user-attachments/assets/1014cda0-6302-479b-a648-392cd923cccf)
C++ application for calculating and visualizing vehicle performance metrics. Built with ImGui and OpenGL.

## Features

- **Performance Calculations**
  - Maximum tractive force
  - Peak acceleration (in m/s² and g-force)
  - Maximum speed
  - 0-100 km/h acceleration time
  - Estimated fuel consumption

- **Parameter Input**
  - **Basic Parameters**
    - Engine torque and power
    - Vehicle mass
    - Wheel radius
  
  - **Transmission Settings**
    - Transmission efficiency
    - Multiple gear ratios (up to 10 gears)
    - Final drive ratio
  
  - **Aerodynamics & Resistance**
    - Drag coefficient (Cd)
    - Frontal area
    - Rolling resistance coefficient
    - Tire pressure
  
  - **Environmental Conditions**
    - Altitude
    - Temperature
    - Road gradient
    - Automatic air density calculation

## Dependencies

- Dear ImGui
- GLFW
- GLAD
- OpenGL 3.0+

## Performance Metrics

Units for each parameter: (USI soon for units)
- Maximum tractive force (N)
- Peak acceleration (m/s² and g)
- Maximum speed (km/h)
- 0-100 km/h time (seconds)
- Estimated fuel consumption (L/100km)
