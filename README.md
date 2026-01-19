# UE5 Procedural City & Weather Architecture

![Unreal Engine](https://img.shields.io/badge/Unreal_Engine-5.3+-black?style=for-the-badge&logo=unrealengine)
![Language](https://img.shields.io/badge/Language-C++_%7C_Blueprints-blue?style=for-the-badge)
![Focus](https://img.shields.io/badge/Focus-PCG_%7C_Tech_Art_%7C_Optimization-orange?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Technical_Showcase-green?style=for-the-badge)

![Hero Showcase](docs/hero_rain_night.gif)

> **View the full 4K High-Fidelity Showcase:** [YouTube Link Here](https://www.youtube.com/watch?v=PF5UTIv-ig0) | [Bilibili Link Here](https://www.bilibili.com/video/BV1LErDBBEio/)

## 📖 Abstract

This project serves as a **technical demonstration** of a scalable, procedurally generated city pipeline and a dynamic weather system within Unreal Engine 5. 

Leveraging **10+ years of Graphics Programming experience (OpenGL/Vulkan)**, this repository focuses on the *engineering challenges* of rendering large-scale environments. It bridges the gap between raw C++ optimization and high-level Technical Art workflows, featuring:

* **Procedural Content Generation (PCG):** Deterministic city layout generation with collision handling and logic-driven prop placement.
* **Dynamic Weather System:** A unified Master Material workflow for real-time wetness accumulation, coupled with Niagara-based volumetric rain.
* **Performance Optimization:** Custom C++ render state management (Skylight/Cloud interaction) and World Partition HLOD strategies for stable framerates.

> **⚠️ Repository Note:** > Due to copyright restrictions and file size limits of high-fidelity assets (Quixel Megascans / City Sample), this repository hosts the **Core Logic, Source Code, Material Graphs, and Blueprints**. It is intended to demonstrate *system architecture* and *implementation logic*.

### 1. Advanced PCG & Graph Algorithms
The city layout utilizes a **hierarchical, deterministic PCG framework**. It encompass from "Single Building Generation" to "Region Buildings Generation", creating a modular system that functions as an interactive level design tool.

* **Layer 1: Spline-Based Road Network (Topology Solver)**
    * **Graph Logic:** Procedurally generates static mesh roads along user-defined splines.
    * **Intersection Solving:** Implemented a topological check to detect spline branching. The system automatically identifies node valency (3-way vs. 4-way) and spawns the correct crossroad meshes with aligned tangents.
    * **Scatter Logic:** Automatically populates roadside assets (street lamps, trash cans) based on spline length and curvature density.

* **Layer 2: Parametric Building Modules (Subgraph Architecture)**
    * **Modular Design:** Developed a library of **12 distinct architectural styles**, each implemented as an independent PCG Subgraph.
    * **Procedural Assembly:** Buildings are not static meshes but **dynamic assemblies**. I exposed public parameters (`Width`, `Length`, `Height`) for each style, allowing the system to generate variations ranging from small shops to skyscrapers using the same logic graph.
    * **Material Variation:** Integrated logic to randomize window emissive textures and wall materials per instance.

* **Layer 3: Region-Based City Layout (The Orchestrator)**
    * **Constraint Solving:** A master PCG graph that samples arbitrary closed spline regions to scatter the building modules mentioned above.
    * **Self-Intersection Check:** The graph acts as a manager, running an iterative **bounds-check loop** to ensure generated buildings do not overlap.

* **Layer 4: Context-Aware Background Proxy:**
    * **Data Interaction:** The Volume-based generator for distant architecture samples the **Road Spline Data** to create a dynamic "Exclusion Zone."
    * **Result:** Simple cubes are automatically culled if they interfere with the playable road network, demonstrating **inter-graph communication** between the road and building systems.

| **Spline Road Generation (Layer 1)** | **Region City Layout (Layer 3)** |
| :---: | :---: |
| ![Road Gen](docs/SplineRoad.gif) | ![City Gen](docs/SplineBuilding.gif) |
| *Real-time intersection solving* | *Constraint-based building scattering* |

<details>
<summary><strong>🔍 Click to view PCG Graph Logic (Under the Hood)</strong></summary>

> **Intersection Solver Logic:**
> This snippet demonstrates the math used to calculate spline tangents and determine node valency (3-way vs 4-way) to spawn the correct mesh.
>
> ![PCG Graph Screenshot](docs/RoadPCG.png)
> ![PCG Graph Screenshot](docs/BuildingsPCG.png)

</details>