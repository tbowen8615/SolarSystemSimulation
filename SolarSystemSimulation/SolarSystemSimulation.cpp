// SolarSystemSimulation.cpp : Defines the entry point for the application.
//
#include "SolarSystemSimulation.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <cstdlib>    // For random number generation
#include <ctime>      // For seeding the random generator

// Constants for scaling and window size
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;
const float SCALE = 1.5f;  // Scaling factor for orbit radii
const float G = 0.0001f;   // Gravitational constant (scaled for simplicity)
const float PI = 3.14159f;
const float TWO_PI = 2 * PI;
const float TOLERANCE = 1e-6f;  // Tolerance for eccentric anomaly approximation
const float TIME_SCALE = 0.1f; // Scale factor to slow down orbits

// Variables for camera control
float zoomLevel = 1.0f; // Starting zoom level
float xOffset = 0.0f;   // X-axis pan offset
float yOffset = 0.0f;   // Y-axis pan offset

// Structure to hold planet data
struct Planet {
    float semiMajorAxis; // Semi-major axis of the orbit
    float eccentricity;  // Eccentricity of the orbit
    float orbitalPeriod; // Orbital period (speed modifier)
    float size;          // Visual size of the planet
    float r, g, b;       // Color of the planet
    float angle;         // Current angle in orbit (true anomaly)
    float meanAnomaly;   // Mean anomaly
    const char* name;    // Name of the planet

    // Constructor to initialize all members
    Planet(float semiMajorAxis, float eccentricity, float orbitalPeriod, float size, float r, float g, float b, const char* name)
        : semiMajorAxis(semiMajorAxis), eccentricity(eccentricity), orbitalPeriod(orbitalPeriod), size(size),
        r(r), g(g), b(b), angle(0.0f), meanAnomaly(0.0f), name(name) {}
};

// Create planets with semi-major axis, eccentricity, orbital period, size, color, and name
std::vector<Planet> planets = {
    Planet(0.4f * SCALE, 0.205f, 0.24f, 0.015f, 1.0f, 0.0f, 0.0f, "Mercury"),
    Planet(0.7f * SCALE, 0.007f, 0.62f, 0.02f, 1.0f, 1.0f, 1.0f, "Venus"),
    Planet(1.0f * SCALE, 0.017f, 1.0f, 0.025f, 0.0f, 0.0f, 1.0f, "Earth"),
    Planet(1.5f * SCALE, 0.093f, 1.88f, 0.02f, 1.0f, 0.0f, 0.0f, "Mars"),
    Planet(2.8f * SCALE, 0.048f, 11.86f, 0.04f, 1.0f, 0.5f, 0.0f, "Jupiter"),
    Planet(3.5f * SCALE, 0.056f, 29.45f, 0.035f, 1.0f, 1.0f, 0.5f, "Saturn"),
    Planet(4.0f * SCALE, 0.046f, 84.02f, 0.03f, 0.0f, 0.5f, 1.0f, "Uranus"),
    Planet(4.5f * SCALE, 0.010f, 164.79f, 0.03f, 0.0f, 0.0f, 1.0f, "Neptune"),
    Planet(5.9f * SCALE, 0.249f, 248.0f, 0.02f, 0.8f, 0.8f, 0.8f, "Pluto")  // Adding Pluto with its parameters
};

// Function to initialize planets with random mean anomaly to avoid straight-line alignment
void initializePlanets() {
    srand(static_cast<unsigned int>(time(0))); // Seed the random generator

    for (Planet& planet : planets) {
        planet.meanAnomaly = static_cast<float>(rand()) / RAND_MAX * TWO_PI; // Randomize mean anomaly
    }
}

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

// Function to draw the orbit of a planet as an ellipse
void drawOrbit(const Planet& planet) {
    glColor3f(0.5f, 0.5f, 0.5f); // Gray color for orbits
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 100; i++) {
        float angle = TWO_PI * i / 100;
        float radius = planet.semiMajorAxis * (1 - planet.eccentricity * planet.eccentricity) /
            (1 + planet.eccentricity * cosf(angle));
        float x = radius * cosf(angle);
        float y = radius * sinf(angle);
        glVertex2f(x, y);
    }
    glEnd();
}

// Function to update planet positions based on elliptical orbits
void updatePlanets(float deltaTime) {
    deltaTime *= TIME_SCALE;

    for (Planet& planet : planets) {
        // Update mean anomaly based on orbital period
        planet.meanAnomaly += (TWO_PI / planet.orbitalPeriod) * deltaTime;
        if (planet.meanAnomaly > TWO_PI) {
            planet.meanAnomaly -= TWO_PI;
        }

        // Calculate eccentric anomaly using Newton's method with tolerance-based exit
        float eccentricAnomaly = planet.meanAnomaly;
        for (int i = 0; i < 10; i++) {
            float delta = (eccentricAnomaly - planet.eccentricity * sinf(eccentricAnomaly) - planet.meanAnomaly) /
                (1 - planet.eccentricity * cosf(eccentricAnomaly));
            eccentricAnomaly -= delta;
            if (fabs(delta) < TOLERANCE) break;
        }

        // Convert eccentric anomaly to true anomaly
        float trueAnomaly = 2 * atanf(sqrtf((1 + planet.eccentricity) / (1 - planet.eccentricity)) * tanf(eccentricAnomaly / 2));
        planet.angle = trueAnomaly;

        // Calculate the radius based on the true anomaly
        float distance = planet.semiMajorAxis * (1 - planet.eccentricity * planet.eccentricity) /
            (1 + planet.eccentricity * cosf(trueAnomaly));

        // Convert polar coordinates to Cartesian coordinates
        float x = distance * cosf(planet.angle);
        float y = distance * sinf(planet.angle);

        // Draw the planet
        drawCircle(x, y, planet.size, planet.r, planet.g, planet.b);
    }
}

// Handle scroll input for zooming
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    zoomLevel *= (1.0f + static_cast<float>(yoffset) * 0.1f); // Zoom in or out based on scroll direction
    if (zoomLevel < 0.1f) zoomLevel = 0.1f; // Prevent excessive zoom out
}

// Handle keyboard input for panning
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    const float panSpeed = 0.1f / zoomLevel; // Adjust pan speed based on zoom level
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_LEFT) xOffset -= panSpeed;
        if (key == GLFW_KEY_RIGHT) xOffset += panSpeed;
        if (key == GLFW_KEY_UP) yOffset += panSpeed;
        if (key == GLFW_KEY_DOWN) yOffset -= panSpeed;
    }
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Solar System Simulation", primaryMonitor, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetKeyCallback(window, keyCallback);

    // Set up orthographic projection within the OpenGL context
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    const float FULL_SCALE = 10.0f;
    float aspectRatio = static_cast<float>(mode->width) / static_cast<float>(mode->height);
    glOrtho(-FULL_SCALE * aspectRatio, FULL_SCALE * aspectRatio, -FULL_SCALE, FULL_SCALE, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    initializePlanets();

    float previousTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        glClear(GL_COLOR_BUFFER_BIT);
        glPushMatrix();

        // Apply zoom and pan transformations
        glScalef(zoomLevel, zoomLevel, 1.0f);
        glTranslatef(xOffset, yOffset, 0.0f);

        // Draw the Sun at the center
        drawCircle(0.0f, 0.0f, 0.1f, 1.0f, 1.0f, 0.0f);

        // Draw orbits and update planets
        for (const Planet& planet : planets) drawOrbit(planet);
        updatePlanets(deltaTime);

        glPopMatrix();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
