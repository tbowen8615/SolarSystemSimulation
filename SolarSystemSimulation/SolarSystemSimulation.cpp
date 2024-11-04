// SolarSystemSimulation.cpp : Defines the entry point for the application.
//

#include "SolarSystemSimulation.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath> // For trigonometry functions

// Function to draw a circle (Sun) at the center of the screen
void drawCircle(float x, float y, float radius, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= 100; i++) {
        float angle = 2.0f * 3.14159f * i / 100;
        float dx = cosf(angle) * radius;
        float dy = sinf(angle) * radius;
        glVertex2f(x + dx, y + dy);
    }
    glEnd();
}

int main()
{
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(800, 600, "Solar System Simulation", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Set up the orthographic projection to draw in 2D
    glOrtho(-1.5, 1.5, -1.0, 1.0, -1.0, 1.0);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Clear the screen with a black color
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw the Sun at the center (yellow color)
        drawCircle(0.0f, 0.0f, 0.1f, 1.0f, 1.0f, 0.0f);

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    // Clean up and exit
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
