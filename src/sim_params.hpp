#pragma once
#include "../libs/glm/glm.hpp"

struct SimParams {
    // --- 1. Core Simulation ---
    const int sizeGrid = 100;                 // 3D grid dimensions (X, Y, Z)
    const int density = 7;                    // Initial living cells percentage
    const float intervalleSimulation = 0.05f; // Update tick rate (in seconds)

    // --- 2. Geometry ---
    const float cube_size = 1.0f;             // Individual cell size
    const float spacing = 7.0f;               // Distance between cells

    // --- 3. Camera & Motion ---
    const float near_plane = 0.1f;            // Minimum render distance
    const float far_plane = 2000.0f;          // Maximum render distance
    const float camera_distance_multiplier = 0.5f; // Camera zoom level
    const float rotation_speed_x = 0.2f;      // Pitch speed (Up/Down)
    const float rotation_speed_y = 0.05f;     // Yaw speed (Left/Right)

    // --- 4. Aesthetics ---
    const glm::vec3 color_bottom = glm::vec3(0.0f, 0.8f, 1.0f); // Cyan
    const glm::vec3 color_middle = glm::vec3(1.0f, 0.2f, 0.7f); // Magenta
    const glm::vec3 color_top    = glm::vec3(1.0f, 0.8f, 0.1f); // Gold
};