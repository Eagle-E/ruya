Below is a **self‑contained, updated version** of `design.md` (ECS & Scene Design Reference) in one piece, with all earlier changes integrated.

***

# ECS & Scene Design Reference

## 1. Vocabulary

### Primitive Types

| Name      | Type      | Meaning                                                   |
|----------|-----------|-----------------------------------------------------------|
| `EntityId` | `uint32_t` | Integer name-tag identifying an entity. Not an object, just a number. `0` = invalid. |
| `MeshId` | `uint32_t` | Index into `Vault::meshes` (CPU geometry only).          |
| `ImageId` | `uint32_t` | Index into `Vault::images` (CPU image pixels only).      |

### Asset Types

| Name      | Side | Meaning                                                                                  |
|-----------|------|------------------------------------------------------------------------------------------|
| `Mesh`    | CPU  | Raw geometry: vertices, normals, indices. Used for upload, collision, bounds, LOD, picking, etc.  |
| `Image`   | CPU  | Raw pixel data: width, height, format, pixel buffer. Used for upload and CPU image processing.  |
| `MeshGPU` | GPU  | OpenGL mesh buffers: VAO, VBO, EBO, index count. Used only for rendering. Owned by `render::GPUVault`.  |
| `Texture` | GPU  | OpenGL texture object: GL handle, wrap/filter/mip settings. Used only for rendering. Owned by `render::GPUVault`.  |

A `MeshId` indexes into `Vault::meshes[id]` (CPU data). An `ImageId` indexes into `Vault::images[id]` (CPU data). GPU representations (`MeshGPU`, `Texture`) are stored separately in `render::GPUVault` and are created on demand from these CPU assets. 

### ECS Types

| Name      | Meaning |
|-----------|---------|
| `Storage<T>` | Sparse-set container for one component type `T`. Owns all `T` instances, maps `EntityId → T`. Supports dense iteration and O(1) add/get/remove/has. |
| `Registry`   | ECS core. Owns all `Storage<T>` instances (type-indexed). Manages `EntityId` generation. API: `create()`, `destroy()`, `add<T>()`, `get<T>()`, `has<T>()`, `storage<T>()`.  |

### Components (attached to entities via `Registry`)

| Name        | Meaning |
|------------|---------|
| `Transform` | Local and world `glm::mat4` for an entity. |
| `Model`     | A visible object: flat list of `Element`s (at least one, enforced by assert). |
| `Element`   | One renderable piece of a `Model`: `MeshId` + `mat::Phong` material (including color parameters and optional texture maps) + local `Transform`.  |
| `PointLight` | Positional light: `color (vec3)`, `strength (float)`. |
| `SpotLight`  | Cone light: `color`, `strength`, `direction (vec3)`, `angle (float)`. |
| `Ambient`    | Global light: `color (vec3)`, `strength (float)`. |
| `Attractor`  | Gravitational pull: `position (vec3)`, `strength (float)`. |

Each entity can have **at most one** component of each type. For one-to-many (e.g. multiple meshes per entity), use a wrapper component that owns a `std::vector` (like `Model` owning `std::vector<Element>`). 

### Asset Stores and Scene

| Name     | Meaning |
|----------|---------|
| `Vault`  | CPU-side asset store. Owns all `Mesh` and `Image` instances and caches them by file path to avoid duplicate loads. It does not know about GPU objects.  |
| `GPUVault` | GPU-side cache in `ruya::render`. Owns all `MeshGPU` and `Texture` instances, indexed by `MeshId` / `ImageId`. Lazily uploads CPU assets from `Vault` on first use.  |
| `Scene`  | Top-level domain object. Owns a `Registry` and a `Vault`. Later: camera, skybox, environment settings.  |

***

## 2. Software Architecture

```text
Scene
├── Registry                          ← ECS core
│   ├── Storage<Transform>
│   ├── Storage<Model>
│   ├── Storage<PointLight>
│   ├── Storage<SpotLight>
│   ├── Storage<Ambient>
│   └── Storage<Attractor>
│
└── Vault                             ← CPU asset store
    ├── meshes:  Vec<Mesh>           ← CPU geometry
    ├── images:  Vec<Image>          ← CPU pixels
    ├── mesh_cache:  Map<path, MeshId>
    └── image_cache: Map<path, ImageId>
```

GPU-side (in `ruya::render`):

```text
GPUVault                             ← GPU asset cache
├── meshes:   Vec<optional<MeshGPU>>  (indexed by MeshId)
└── textures: Vec<optional<Texture>>  (indexed by ImageId)
```

**Systems** are free functions that take `Scene&` (and for rendering also `GPUVault&`). They query `Registry` storages for component data and resolve asset IDs via `Vault` / `GPUVault`. 

***

## 3. Key Data Structures

### `Storage<T>` (sparse-set)

```cpp
template<typename T>
class Storage : public IStorage {
    std::unordered_map<EntityId, uint32_t> sparse_; // entity → dense index
    std::vector<T>        dense_;   // packed component data
    std::vector<EntityId> id_of_;   // dense_[i] belongs to id_of_[i]
public:
    T&   add(EntityId, T);
    T&   get(EntityId);
    bool has(EntityId) const;
    void remove(EntityId);           // swap-and-pop
    std::span<T>        all();       // dense iteration
    std::span<EntityId> entities();  // parallel to all()
};
```

### `Registry`

```cpp
class Registry {
    EntityId next_id_ = 1;
    std::vector<std::unique_ptr<IStorage>> storages_; // indexed by ComponentTypeId

    template<typename T> Storage<T>& assure(); // get-or-create storage for T
public:
    EntityId create();
    void     destroy(EntityId);

    template<typename T> T&          add(EntityId, T);
    template<typename T> T&          get(EntityId);
    template<typename T> bool        has(EntityId);
    template<typename T> Storage<T>& storage();
};
```

### Materials (`ruya::scene::mat`)

```cpp
namespace ruya::scene::mat {

using ImageId = uint32_t; // alias for Vault::images index

struct Phong {
    glm::vec3 diffuse;     // base color / albedo
    glm::vec3 specular;    // specular color
    float     shininess;   // reflection sharpness

    ImageId   diffuse_map  = 0; // 0 = fallback 1×1 white
    ImageId   specular_map = 0; // 0 = no specular map
};
} // namespace ruya::scene::mat
```

### `Model` and `Element`

```cpp
struct Element {
    MeshId        mesh;
    mat::Phong    material;    // Phong-only for now
    Transform     local_xform;
};

struct Model {
    std::vector<Element> elements;
    // Invariant: elements.size() >= 1
    // Enforced via assert in constructor (or factory function later)
};
```

Currently, `Element` is explicitly Phong-based. Future material models (e.g. PBR) will use separate components and/or render passes rather than runtime polymorphism. 

### `Vault` (CPU assets)

```cpp
struct Vault {
    std::vector<Mesh>  meshes;  // CPU-only geometry
    std::vector<Image> images;  // CPU-only pixels

    std::unordered_map<std::string, MeshId>  mesh_cache;   // path → MeshId
    std::unordered_map<std::string, ImageId> image_cache;  // path → ImageId

    MeshId  load_mesh(const std::string& path);   // cache-aware CPU load
    ImageId load_image(const std::string& path);  // cache-aware CPU load
};
```

`Vault` does not own any GPU objects. It is a passive CPU asset store. 

### `GPUVault` (GPU assets, in `ruya::render`)

```cpp
namespace ruya::render {

struct MeshGPU {
    // VAO, VBO, EBO, index count, etc.
};

struct Texture {
    // GL handle, wrap/filter/mip settings.
};

struct GPUVault {
    // Indexed by MeshId / ImageId
    std::vector<std::optional<MeshGPU>> meshes;
    std::vector<std::optional<Texture>> textures;
};

// Upload-on-demand helpers
MeshGPU& get_or_upload_mesh(GPUVault&, const ruya::scene::Vault&, MeshId);
Texture& get_or_upload_texture(GPUVault&, const ruya::scene::Vault&, ImageId);

} // namespace ruya::render
```

`GPUVault` is mutated only by render code; it is not part of the `Scene`. 

### `Scene`

```cpp
struct Scene {
    ecs::Registry registry;
    Vault         assets;
    // later: Camera, Skybox, ...
};
```

***

## 4. Data Flow

### Scene Loading

```text
Parse scene file (YAML / JSON / TOML / hardcoded C++)

  For each entity:
    EntityId e = scene.registry.create()
    scene.registry.add<Transform>(e, ...)

  For Model entities:
    For each element's mesh path:
      MeshId mid = scene.assets.load_mesh(path)
        → if mesh_cache has path: return cached MeshId
        → else: load Mesh (CPU), store it, cache path→id

    For each element's image path (for diffuse/specular maps):
      ImageId img = scene.assets.load_image(path)
        → same cache logic

    Build mat::Phong material:
      mat::Phong m;
      m.diffuse      = ...;
      m.specular     = ...;
      m.shininess    = ...;
      m.diffuse_map  = diffuse_img_id;
      m.specular_map = specular_img_id;

    Build Model{ elements: [ Element{ mid, m, xform }, ... ] }
    scene.registry.add<Model>(e, std::move(model))

  For Light entities:
    scene.registry.add<PointLight>(e, { color, strength })
    ...
```

GPU upload is deferred to the render step; no GL calls happen during scene loading.

### Render Loop

```text
render_scene(const Scene& s, render::GPUVault& gpu):

  for each entity e with a Model:
    world_xform = s.registry.get<Transform>(e)

    for each Element& el in model:

      // Ensure GPU mesh exists
      MeshGPU& mesh_gpu =
        get_or_upload_mesh(gpu, s.assets, el.mesh)

      // Ensure GPU textures exist (if maps are non-zero)
      Texture& diff_tex =
        get_or_upload_texture(gpu, s.assets, el.material.diffuse_map)
      Texture& spec_tex =
        get_or_upload_texture(gpu, s.assets, el.material.specular_map)

      bind VAO (mesh_gpu)
      bind Phong shader

      upload matrices:
        world = world_xform * el.local_xform

      upload material uniforms:
        diffuse, specular, shininess

      bind diff_tex and spec_tex to texture units
      draw

  collect all PointLights → upload to uniform buffer
  collect all SpotLights  → upload to uniform buffer
  collect Ambient         → upload to uniform buffer
```

All OpenGL-specific work (buffers, textures, shader binding) lives in `ruya::render` and uses `render::GPUVault`. `Scene` and `Vault` stay GPU-agnostic and contain only CPU-side data. 

***

## 5. Design Principles Applied

- **Entities are IDs** — no base class, no vtable, just `uint32_t`. 
- **Components are plain data** — structs with no behaviour, no inheritance. 
- **Systems are free functions** — take `Scene&` (and `GPUVault&` for rendering), operate on storages, no coupling to entity types. 
- **Assets are shared and indexed** — `MeshId` / `ImageId` prevent duplication; multiple entities reference the same CPU asset, and the renderer mirrors them in `GPUVault` as needed. 
- **Scene is GPU-agnostic** — `Vault` only stores CPU `Mesh` / `Image`. `MeshGPU` / `Texture` live exclusively in `ruya::render::GPUVault`. 
- **Cross-entity references use `EntityId`** — never raw pointers into ECS storages (dangling on swap-and-pop). 
- **AoS inside components** — `std::vector<Element>` in `Model` keeps related data together (mesh + material + transform always accessed as a unit). 
- **SoA at the registry level** — `Storage<Transform>`, `Storage<Model>` are separate dense arrays, enabling cache-friendly per-system iteration. 
- **Materials are model-specific PODs** — `mat::Phong` is a simple POD with color and texture map IDs, easy to extend later (e.g. add PBR materials) using new structs and overloads, without runtime polymorphism. 

Would you like a separate updated version of `structure.md` as well so both docs stay in sync?