![Unreal Engine](https://img.shields.io/badge/Unreal_Engine-5.3+-black?style=for-the-badge&logo=unrealengine)
![Language](https://img.shields.io/badge/Language-C++_%7C_Blueprints-blue?style=for-the-badge)
![Focus](https://img.shields.io/badge/Focus-PCG_%7C_Tech_Art_%7C_Optimization-orange?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Technical_Showcase-green?style=for-the-badge)

![Hero Showcase](docs/hero_rain_night.gif)

> **View the full Showcase:** [YouTube Link Here](https://www.youtube.com/watch?v=PF5UTIv-ig0) | [Bilibili Link Here](https://www.bilibili.com/video/BV1LErDBBEio/)

## 📖 Abstract

This project is a **technical demo** of a procedural city pipeline and a dynamic weather system in Unreal Engine 5.

I have **10+ years of experience in Graphics Programming (OpenGL/Vulkan)**. This repository focuses on the *engineering challenges* of rendering large environments. It combines C++ optimization with Technical Art workflows, featuring:

* **PCG & Graph Algorithms:** A hierarchical city generation system that solves layout constraints in the editor.
* **Rendering & Shader Pipeline:** Solutions for common rendering artifacts (like tiling) and a physically consistent water/weather system.
* **System Architecture:** A hybrid design using **Blueprints for logic** and **C++ for performance optimization**.

> **⚠️ Repository Note:** Due to copyright and file size limits of assets (Quixel / City Sample), this repository only contains the **Core Logic, Source Code, Material Graphs, and Blueprints**. The purpose is to show my *system architecture* and *implementation logic*.

## 🛠️ Technical Architecture & Key Features

### 1. PCG & Graph Algorithms
The city layout uses a **hierarchical and deterministic PCG system**. It covers everything from generating a single building to a whole region. This system works as an interactive tool for level design.

* **Layer 1: Spline-Based Road Network (Topology Solver)**
    * **Graph Logic:** Automatically generates static mesh roads along user-defined splines.
    * **Intersection Solving:** Implemented a check to find spline branches. The system detects if a node is a 3-way or 4-way intersection and spawns the correct crossroad mesh.
    * **Scatter Logic:** Automatically places roadside assets (street lamps, trash cans) based on the road length and curve density.

* **Layer 2: Parametric Building Modules (Subgraph System)**
    * **Modular Design:** Created a library of **12 architectural styles**. Each style is an independent PCG Subgraph.
    * **Procedural Assembly:** Buildings are dynamic. I exposed parameters like `Width`, `Length`, and `Height` for each style. The system can generate variations from small shops to skyscrapers using the same logic.
    * **Material Variation:** Added logic to randomize window lights and wall materials for each building.

* **Layer 3: Region-Based City Layout (Region Manager)**
    * **Constraint Solving:** A master PCG graph that fills closed spline regions with the building modules mentioned above.
    * **Self-Intersection Check:** The graph acts as a manager. It runs a **bounds-check loop** to make sure generated buildings do not overlap.

* **Layer 4: Background Proxy with Collision Check**
    * **Data Interaction:** The generator for distant buildings reads the **Road Spline Data** to create an "Exclusion Zone."
    * **Result:** Simple cubes are automatically removed if they overlap with the roads. This shows **data communication** between the road system and the building system.

| **Spline Road Generation (Layer 1)** | **Region City Layout (Layer 3)** |
| :---: | :---: |
| ![Road Gen](docs/SplineRoad.gif) | ![City Gen](docs/SplineBuilding.gif) |
| *Real-time intersection solving* | *Constraint-based building scattering* |

<details>
<summary><strong>🔍 Click to view PCG Graph Logic (Under the Hood)</strong></summary>

> **Intersection Solver Logic:**
> This image shows the math to calculate spline tangents and check node types (3-way vs 4-way) to spawn the correct mesh.
> ![PCG Graph Screenshot](docs/RoadPCG.png)
>
> **Attribute-Driven Building Dispatcher:**
> Scatters buildings within a Spline Region, randomizing **Dimensions** and using a **Self-Intersection Check** to prevent overlap.
> ![PCG Graph Screenshot](docs/BuildingsPCG.png)

</details>