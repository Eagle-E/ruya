# Project Structure

## Main Components

| Namespace | Role | Description |
|---|---|---|
| `ruya::ecs` | **Machinery** | Generic ECS infrastructure. The backbone that powers the scene. |
| `ruya::scene` | **Data** | Defines the world state. The "what." |
| `ruya::render` | **System** | Transforms scene data into pixels. The "how." |
| `ruya::ui` | **Tool** | Visualizes and mutates the scene data at runtime. |
| `ruya::io` | **Translation** | Converts external formats into internal data structures. |

***

## Folder & Namespace Descriptions

### `src/core/`
- **Namespace**: `ruya::ecs` (for ECS types); no namespace for math/utility primitives
- **Role**: Foundational infrastructure.
- **Description**: The "standard library" of the engine. Contains the ECS machinery (`Storage<T>`, `Registry`, `EntityId`) and foundational utilities (math types, common primitives) that every other module depends on.
- **Idea**: This code is **universal**. It knows nothing about rendering, scenes, or UI. It provides the vocabulary and tools for the rest of the engine.
- **Contents**:
  - `ecs/entity.hpp` — `EntityId`, `NULL_ENTITY`
  - `ecs/storage.hpp` — `Storage<T>`, `IStorage`
  - `ecs/registry.hpp` — `Registry`
  - `math/` — vector/matrix math (glm wrappers or utilities)

***

### `src/scene/`
- **Namespace**: `ruya::scene`
- **Role**: Data / State.
- **Description**: Defines the world. Contains plain data structs representing entities, components, and assets.
- **Idea**: The scene is a **passive database**. It has no logic to render itself, update itself, or save itself. It is purely a container of data. This allows multiple systems (renderer, UI, physics) to read and write the same data without coupling to each other.
- **Contents**:
  - `scene.hpp` — `Scene` struct (owns `Registry` + `Vault`)
  - `components.hpp` — `Transform`, `Model`, `Element`, `PointLight`, `SpotLight`, `Ambient`, `Attractor`
  - `vault.hpp` — `Vault` struct + asset loading free functions (`load_mesh`, `load_texture`)
  - `materials.hpp` — `mat::Phong`, and future material data structs

***

### `src/render/`
- **Namespace**: `ruya::render`
- **Role**: System / Logic.
- **Description**: Contains the active logic that talks to the GPU. Owns the OpenGL context, manages VRAM, and executes draw calls.
- **Idea**: The renderer is conceptually a **pure function** — it takes a `const Scene&` as input and produces an image. It borrows scene data to render it but does not own it. All OpenGL-specific work (shaders, buffers, uniforms) is contained here and nowhere else.
- **Contents**:
  - `renderer.hpp` — main `draw_frame(const Scene&)` function
  - `context.hpp` — window and GL context creation
  - `material_system.hpp` — logic to bind material data to shaders
  - `backend/` — wrappers for GL buffers, VAOs, textures
  - `shaders/` — GLSL source files and C++ shader wrapper types

***

### `src/ui/`
- **Namespace**: `ruya::ui`
- **Role**: Tool / View.
- **Description**: The editor interface built on ImGui. Allows the user to inspect and modify scene state at runtime.
- **Idea**: The UI is a **mutation layer**. Unlike the renderer (which only reads the scene), the UI has mutable access (`Scene&`) to change values at runtime (e.g. drag a slider to change light intensity). It sits above the engine, orchestrating interaction between the user and the data.
- **Contents**:
  - `editor.hpp` — top-level UI draw function
  - `panels/` — individual ImGui panel components (scene hierarchy, inspector, etc.)

***

### `src/io/`
- **Namespace**: `ruya::io`
- **Role**: IO / Serialization.
- **Description**: Handles import and export of assets and scenes between disk and memory.
- **Idea**: IO is a **translation layer**. It converts external formats (PNG, OBJ, YAML, TOML) into internal `scene` data structures. Keeping this isolated means complex parsing logic and third-party parser headers never leak into the core engine.
- **Contents**:
  - `mesh_loader.hpp` — load OBJ/other formats into `Mesh` (CPU geometry)
  - `image_loader.hpp` — load PNG/JPG into `Image` (CPU pixels)
  - `scene_loader.hpp` — parse scene definition files into a fully populated `Scene`

***

## Dependency Rules

Dependencies flow **downward only** — never circular:

```
ruya::ecs    ←  no dependencies (pure machinery)
ruya::scene  ←  ruya::ecs
ruya::render ←  ruya::scene, ruya::ecs
ruya::ui     ←  ruya::scene, ruya::ecs
ruya::io     ←  ruya::scene, ruya::ecs
```

No module in `render`, `ui`, or `io` may include headers from each other.

***

## Key Data Types at a Glance

| Name | Namespace | Side | Meaning |
|---|---|---|---|
| `EntityId` | `ruya::ecs` | — | Integer entity identifier. `0` = invalid. |
| `MeshId` | `ruya::scene` | — | Index into `Vault` mesh arrays (CPU + GPU) |
| `TexId` | `ruya::scene` | — | Index into `Vault` texture arrays (CPU + GPU) |
| `Mesh` | `ruya::scene` | CPU | Raw geometry: vertices, normals, indices |
| `MeshGPU` | `ruya::scene` | GPU | OpenGL mesh buffers: VAO, VBO, EBO |
| `Image` | `ruya::scene` | CPU | Raw pixel data: width, height, pixel buffer |
| `Texture` | `ruya::scene` | GPU | OpenGL texture object and sampling state |
| `Storage<T>` | `ruya::ecs` | — | Sparse-set component container for type `T` |
| `Registry` | `ruya::ecs` | — | Owns all `Storage<T>`, manages entity IDs |
| `Vault` | `ruya::scene` | — | Owns all CPU+GPU assets, caches by file path |
| `Scene` | `ruya::scene` | — | Top-level world object: owns `Registry` + `Vault` |
| `Transform` | `ruya::scene` | — | Local and world matrices for an entity |
| `Model` | `ruya::scene` | — | Renderable component: flat list of `Element`s |
| `Element` | `ruya::scene` | — | One piece of a `Model`: `MeshId` + `TexId` + local `Transform` |