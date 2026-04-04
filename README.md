# 3D Game of Life

During a course about computability, we've been learning about 2D cellular automata. While my professor, Mr. Perrot, was talking about Turing machines and related problems, I asked myself if a 3D Game of Life existed.

I first started by coding my 2D automaton in the terminal, and then got the idea to turn it into a 3D Game of Life. My professor told me that it would be interesting to see, but he had no idea how to properly model it to get a clear view of the game inside.

## Compiling & Running

This project currently compiles on Linux. Some modifications might be required to make it work on your specific operating system. 

To build and launch the project, simply run the bash script:
```bash
./exec.sh
```

## Parameters

There are two configuration files that you can easily modify to tweak the simulation and the visual output:

sim_params.hpp : Contains the logic and rules for the cellular automaton.

render_params.hpp : Contains the graphical settings (grid size, spacing, etc.).

## The Optimization

My first representation was really time-consuming to compute, and we couldn't really see the depth difference because everything was in black and white. 
I kept working on this project, using shaders, modifying my algorithm, and tweaking colors...

Then I got my first real representation, but it was only rendering a 50 x 50 x 50 cube in real time. I got the idea to build an offline renderer to visualize way more cubes and generate a video. 

I wanted something much more performant, so I opted for **Compute Shaders**, which compute the grid of the new dead and alive cells directly on the GPU. Now, with my *NVIDIA GeForce GTX 1650* and *11th Gen Intel Core i5-11400H*, I can run a **100 x 100 x 100 grid** pretty easily!

Finally, I reworked my architecture to achieve better visibility and logic, adding parameters to provide a greater visual experience.

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
  <em>Observing some Moiré Phenomena</em>
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
**[Watch the Reddit Example](https://packaged-media.redd.it/yzk6huh3kztf1/pb/m2-res_1080p.mp4?m=DASHPlaylist.mpd&var=sgpssan&v=1&e=1775098800&s=aaa116a0d0430ed476d079bc5af4fb1603bbe1d1)**
