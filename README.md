# 3D Game of Life

During a course about computability, we've been learning about 2D cellular automata. While my professor, [Mr. Perrot](https://pageperso.lis-lab.fr/kevin.perrot/), was talking about Turing machines and related problems, I asked myself if a 3D Game of Life existed.

I first started by coding my 2D automaton in the terminal, and then got the idea to turn it into a 3D Game of Life. My professor told me that it would be interesting to see, but he had no idea how to properly model it to get a clear view of the game inside.

## The Optimization

My first representation was really time-consuming to compute, and we couldn't really see the depth difference because everything was in black and white. 
I kept working on this project, using shaders, modifying my algorithm, and tweaking colors...

Then I got my first real representation, but it was only rendering a 50 x 50 x 50 cube in real time. I got the idea to build an offline renderer to visualize way more cubes and generate a video. 

I wanted something much more performant, so I opted for **Compute Shaders**, which compute the grid of the new dead and alive cells directly on the GPU. Now, with my *NVIDIA GeForce GTX 1650* and *11th Gen Intel Core i5-11400H*, I can run a **100 x 100 x 100 grid** pretty easily!

Finally, I reworked my architecture to achieve better visibility and logic, adding parameters to provide a greater visual experience.

---

## Dependencies

* **C++ Compiler** with **C++17** support (e.g., GCC 7+ or Clang 5+)
* **CMake** (version 3.10 or higher)
* **Make**
* **OpenGL 4.3+** (Strictly required for Compute Shaders)

**System Libraries:**
* **GLFW 3** (`libglfw3-dev` on Linux) for window and context management.
* **libGL** and **libdl** (Standard Linux graphics and dynamic linking libraries).

*(Note: GLAD, GLM, and stb_image_write are included directly in the `libs/` folder of this repository, so you don't need to install them )*

---

## Compiling & Running

This project currently compiles on Linux. Some modifications might be required to make it work on your specific operating system. 

To build and launch the project, simply run the bash script:
```bash
./exec.sh arg
```
the **arg** value may be 1 or 2

1 = Real time rendering<br>
2 = Offline rendering

You can also have the information of this command with :
```bash
./exec.sh --help


Usage: ./exec.sh [MODE]

Modes:
  1             Run in real-time mode (Default).
  2             Run in offline rendering mode (Exports frames to 'render/').
  -h, --help    Show this help message and exit.

Examples:
  ./exec.sh       # Compiles and runs in default real-time mode
  ./exec.sh 2     # Compiles and runs the offline renderer

```

## Parameters

There are two configuration files that you can easily modify to tweak the simulation and the visual output:


sim_params.hpp : Contains the logic and rules for the cellular automaton.

```cpp
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
```
render_params.hpp : Contains the graphical settings of the offline rendering

```cpp
struct RenderParams{
    //PARAMETERS
    const int FPS = 60;
    const int TIME_SECONDS = 60;
    





    //DO not modify
    const int MAX_FRAMES = FPS * TIME_SECONDS;
    int frameCounter = 0;
};
```

---
## Offline Rendering

When offline mode rendering is defined, all the frames are placed in the render directory. You can execute this command in the root of the project :

first install ffmpeg : 

```bash
sudo apt update
sudo apt install ffmpeg
```

when it is installed :

```bash
ffmpeg -framerate 60 -i render/frame_%04d.bmp -c:v libx264 -pix_fmt yuv420p simulation_GameOfLife3D.mp4
````

for more information about ffmpeg : https://ffmpeg.org/ffmpeg.html

---

## Gallery

<p align="center">
  <img src="https://github.com/user-attachments/assets/32bdd4bf-02cc-4295-82e6-feebe8661461" alt="Simulation close" width="80%">
  <br>
  <em>Simulation when cubes are pretty close</em>
</p>

<p align="center">
  <img src="https://github.com/user-attachments/assets/e68311b6-7a01-412d-a52e-acef8aad8ca8" alt="Simulation distant" width="80%">
  <br>
  <em>Simulation when the cubes are more distant</em>
</p>

<p align="center">
  <img src="https://github.com/user-attachments/assets/8dfce2ea-c51c-4349-ae3a-d3b95fa2d655" alt="Moire Phenomena" width="80%">
  <br>
  <em>Observing some funny phenomena</em>
</p>

---

## Next Steps

There are still many exciting features and optimizations to explore for this project:

* **Algorithm Upgrades:** Exploring new optimization techniques and increasing the radius for neighbor detection to create more complex patterns.
* **Holographic Rendering:** Replacing the standard cubes with a texture-based rasterization approach, calculating positions to recreate a sort of hologram effect.
* **Physics & Cosmic Simulations:** Adding physical constraints and new movement rules to simulate massive structures, perhaps even recreating a galaxy or something similar!

---

## Sources and Further Reading

**https://fr.wikipedia.org/wiki/Jeu_de_la_vie**
<br>
**https://www.j3ea.org/articles/j3ea/pdf/2022/02/j3ea222014.pdf**
<br>
**https://content.wolfram.com/sites/13/2018/02/01-3-1.pdf**
<br>
**[Reddit example of the phenomena](https://packaged-media.redd.it/yzk6huh3kztf1/pb/m2-res_1080p.mp4?m=DASHPlaylist.mpd&var=sgpssan&v=1&e=1775098800&s=aaa116a0d0430ed476d079bc5af4fb1603bbe1d1)**
