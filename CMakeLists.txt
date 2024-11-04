# CMakeLists.txt : Top-level CMake project file for global configuration
cmake_minimum_required(VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Project name
project("SolarSystemSimulation")

# Define the executable target
add_executable(${PROJECT_NAME} SolarSystemSimulation/SolarSystemSimulation.cpp)

# Include directories for external libraries
include_directories(${PROJECT_SOURCE_DIR}/external/glm)
include_directories(${PROJECT_SOURCE_DIR}/external/glfw/include)

# Define GLFW_STATIC if using the static version of GLFW
add_definitions(-DGLFW_STATIC)

# Add GLFW as a subdirectory for building within the project
add_subdirectory(external/glfw)

# Link GLFW and OpenGL to the executable
target_link_libraries(${PROJECT_NAME} glfw opengl32)
