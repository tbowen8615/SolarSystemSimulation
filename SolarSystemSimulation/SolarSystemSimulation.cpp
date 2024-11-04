// SolarSystemSimulation.cpp : Defines the entry point for the application.
//

#include "SolarSystemSimulation.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <vector>

// Constants for scaling and window size
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;
const float SCALE = 1.5f;  // Scaling factor for orbit radii
const float G = 0.0001f;   // Gravitational constant (scaled for simplicity)
const float PI = 3.14159f;
const float TWO_PI = 2 * PI;
const float TOLERANCE = 1e-6f;  // Tolerance for eccentric anomaly approximation
const float TIME_SCALE = 0.1f; // Scale factor to slow down orbits

// Structure to hold planet data
struct Planet {
    float semiMajorAxis; // Semi-major axis of the orbit
    float eccentricity;  // Eccentricity of the orbit
    float orbitalPeriod; // Orbital period (speed modifier)
    float size;          // Visual size of the planet
    float r, g, b;       // Color of the planet
    float angle;         // Current angle in orbit (true anomaly)
    float meanAnomaly;   // Mean anomaly
};

// Create planets with semi-major axis, eccentricity, orbital period, size, color
std::vector<Planet> planets = {
    {0.4f * SCALE, 0.205f, 0.24f, 0.015f, 1.0f, 0.0f, 0.0f}, // Mercury (Red)
    {0.7f * SCALE, 0.007f, 0.62f, 0.02f, 1.0f, 1.0f, 1.0f},  // Venus (White)
    {1.0f * SCALE, 0.017f, 1.0f, 0.025f, 0.0f, 0.0f, 1.0f},  // Earth (Blue)
    {1.5f * SCALE, 0.093f, 1.88f, 0.02f, 1.0f, 0.0f, 0.0f},  // Mars (Red)
    {2.8f * SCALE, 0.048f, 11.86f, 0.04f, 1.0f, 0.5f, 0.0f}, // Jupiter (Orange)
    {3.5f * SCALE, 0.056f, 29.45f, 0.035f, 1.0f, 1.0f, 0.5f}, // Saturn (Yellow)
    {4.0f * SCALE, 0.046f, 84.02f, 0.03f, 0.0f, 0.5f, 1.0f}, // Uranus (Light Blue)
    {4.5f * SCALE, 0.010f, 164.79f, 0.03f, 0.0f, 0.0f, 1.0f}  // Neptune (Blue)
};

// Function to draw a circle (Sun or Planet)
void drawCircle(float x, float y, float radius, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= 100; i++) {
        float angle = TWO_PI * i / 100;
        float dx = cosf(angle) * radius;
        float dy = sinf(angle) * radius;
        glVertex2f(x + dx, y + dy);
    }
    glEnd();
}

// Function to update planet positions based on elliptical orbits
void updatePlanets(float deltaTime) {
    // Apply time scaling to slow down the orbits
    deltaTime *= TIME_SCALE;

    for (Planet& planet : planets) {
        // Update mean anomaly based on orbital period
        planet.meanAnomaly += (TWO_PI / planet.orbitalPeriod) * deltaTime;
        if (planet.meanAnomaly > TWO_PI) {
            planet.meanAnomaly -= TWO_PI;
        }

        // Calculate eccentric anomaly using Newton's method with tolerance-based exit
        float eccentricAnomaly = planet.meanAnomaly;
        for (int i = 0; i < 10; i++) { // Maximum 10 iterations for approximation
            float delta = (eccentricAnomaly - planet.eccentricity * sinf(eccentricAnomaly) - planet.meanAnomaly) /
                (1 - planet.eccentricity * cosf(eccentricAnomaly));
            eccentricAnomaly -= delta;
            if (fabs(delta) < TOLERANCE) break; // Exit loop if within tolerance
        }

        // Convert eccentric anomaly to true anomaly
        float trueAnomaly = 2 * atanf(sqrtf((1 + planet.eccentricity) / (1 - planet.eccentricity)) * tanf(eccentricAnomaly / 2));
        planet.angle = trueAnomaly;

        // Calculate the radius based on the true anomaly (distance to the Sun)
        float distance = planet.semiMajorAxis * (1 - planet.eccentricity * planet.eccentricity) /
            (1 + planet.eccentricity * cosf(trueAnomaly));

        // Convert polar coordinates to Cartesian coordinates
        float x = distance * cosf(planet.angle);
        float y = distance * sinf(planet.angle);

        // Draw the planet
        drawCircle(x, y, planet.size, planet.r, planet.g, planet.b);
    }
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Get the primary monitor and its video mode for full screen
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

    // Create a fullscreen window
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Solar System Simulation", primaryMonitor, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Update the SCALE factor to encompass outer planets
    const float FULL_SCALE = 10.0f; // Increase the scale for larger view area

    // Set up orthographic projection within the OpenGL context
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-FULL_SCALE, FULL_SCALE, -FULL_SCALE * (float)mode->height / mode->width, FULL_SCALE * (float)mode->height / mode->width, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Set up timing for dynamic deltaTime
    float previousTime = glfwGetTime();

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Calculate deltaTime based on elapsed time
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        // Clear the screen with a black color
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw the Sun at the center (yellow color)
        drawCircle(0.0f, 0.0f, 0.1f, 1.0f, 1.0f, 0.0f);

        // Update planet positions with dynamic delta time for frame-rate independence
        updatePlanets(deltaTime);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup and exit
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
