# Example: Car, Wheels, Tires, Rims in ECS

## 1. Terminology

- **Entity**: just an `EntityId` (a `uint32_t`), e.g. `car_entity = 42`. [ppl-ai-file-upload.s3.amazonaws](https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/collection_c5469497-c908-436d-8de6-86c82805e2da/7f8211f3-9d41-487f-9be3-76212e9dc2e4/design.md)
- **Component**: plain data attached to an entity, e.g. `Transform`, `Model`, `Tire`. [ppl-ai-file-upload.s3.amazonaws](https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/attachments/19276299/9636aef5-fd1c-4f88-b222-e72fa5475f65/design.md)
- **System**: free function operating on storages, e.g. `simulate_cars(Scene&)`. [ppl-ai-file-upload.s3.amazonaws](https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/collection_c5469497-c908-436d-8de6-86c82805e2da/7f8211f3-9d41-487f-9be3-76212e9dc2e4/design.md)

We keep three concepts separate:

- Game objects (car, wheel, tire, rim) → **entities**.
- Visual representation (meshes, textures) → **Model** components and `Element`s. [ppl-ai-file-upload.s3.amazonaws](https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/attachments/19276299/9636aef5-fd1c-4f88-b222-e72fa5475f65/design.md)
- Simulation data (engine power, tire grip) → domain-specific **physics components**.

***

## 2. Components

```cpp
// Shared components (from design doc)
struct Transform {
    glm::mat4 local;
    glm::mat4 world;
};

struct Element {
    MeshId    mesh;
    TexId     diffuse_map;
    Transform local_xform;
};

struct Model {
    std::vector<Element> elements; // elements.size() >= 1
};

// Car domain components
struct CarState {
    std::array<EntityId, 4> wheel_entities;   // FL, FR, BL, BR
    EntityId                engine_entity;
    float                   velocity;         // m/s
};

struct Engine {
    float horse_power;
    float rpm;
};

struct Fuel {
    float capacity;   // litres
    float level;      // litres
};

struct Tire {         // physics properties of *the wheel’s tire*
    float grip;       // 0–1
    float wear;       // 0–1
    bool  grounded;   // updated by physics from contact tests
};

struct Wheel {
    EntityId tire_visual_entity; // 0 = none
    EntityId rim_visual_entity;  // 0 = none
};
```

Key decisions:

- `Tire` is a **physics component**. It lives on the **wheel entity**, not on the visual tire mesh entity.
- `Wheel` is a **link/component** that tells us which visual entities represent this wheel (tire mesh, rim mesh).
- Visual entities only need `Transform` + `Model`. They don’t carry physics data.

***

## 3. Entity Layouts

### 3.1 Car with simple wheels (no customization)

For a basic game with no rim/tire swapping, you can keep it very simple:

```text
car_entity        → Transform, CarState, Fuel, Model (car body)

wheel_FL_entity   → Transform, Tire, Model (elements: tire + rim)
wheel_FR_entity   → Transform, Tire, Model
wheel_BL_entity   → Transform, Tire, Model
wheel_BR_entity   → Transform, Tire, Model
```

- Each **wheel entity** has:
  - `Transform` for its position relative to the car.
  - `Tire` for wheel physics.
  - `Model` whose `elements` vector contains two `Element`s: one for the tire mesh, one for the rim mesh.
- Swapping visuals at runtime means mutating that wheel’s `Model.elements` (e.g. changing `mesh`/`diffuse_map`), no new entities required.

This is the **simplest** representation and should be your default until you actually need dynamic part swapping.

***

### 3.2 Car with swappable rims and tires

When you want full customization (player can swap rims/tires independently), you separate visuals into their own entities:

```text
car_entity        → Transform, CarState, Fuel, Model (car body)

wheel_FL_entity   → Transform, Tire, Wheel      // logic + links
tire_FL_entity    → Transform, Model            // tire mesh only
rim_FL_entity     → Transform, Model            // rim mesh only

... similarly for FR, BL, BR ...
engine_entity     → Transform, Engine, (optional Model)
```

- `wheel_FL_entity` is the **logical wheel** (used in physics and gameplay).
  - Holds `Transform` (wheel pose).
  - Holds `Tire` (physics data).
  - Holds `Wheel` (links to current visual tire and rim entities).
- `tire_FL_entity` and `rim_FL_entity` are **pure visual entities**.
  - Each has `Transform` (offset relative to the wheel) and `Model` (mesh/texture).

Swapping parts now means:

```cpp
// Change the rim for a wheel
auto& wheel = registry.get<Wheel>(wheel_entity);
wheel.rim_visual_entity = new_rim_entity;   // created from a prefab

// Or swap the tire visual
wheel.tire_visual_entity = new_tire_entity;
```

The **physics** still read the `Tire` component on the `wheel_entity`. Visual tire entities are just render data.

***

## 4. YAML Sketches

### 4.1 Simple wheel (no part swapping)

```yaml
wheel_front_left:
  components:
    Transform: { ... }
    Tire: { grip: 0.9, wear: 0.0 }
    Model:
      parts:
        - mesh: "tire.mesh"
          diffuse: "tire_diffuse.png"
          transform: ... # tire offset relative to wheel
        - mesh: "rim.mesh"
          diffuse: "rim_diffuse.png"
          transform: ... # rim offset relative to wheel
```

Loader pseudo-code:

```cpp
EntityId w = registry.create();
registry.add<Transform>(w, wheel_xform);
registry.add<Tire>(w, Tire{ .grip = 0.9f, .wear = 0.0f });

Model m;
m.elements.push_back(build_element(vault, "tire.mesh", "tire_diffuse.png", tire_local));
m.elements.push_back(build_element(vault, "rim.mesh",  "rim_diffuse.png",  rim_local));
registry.add<Model>(w, std::move(m));
```

### 4.2 Swappable parts

```yaml
wheel_front_left:
  components:
    Transform: { ... }
    Tire: { grip: 0.9, wear: 0.0 }
    Wheel:
      tire_visual: "tire_front_left_visual"
      rim_visual:  "rim_front_left_visual"

tire_front_left_visual:
  components:
    Transform: { ... # relative to wheel }
    Model:
      parts:
        - mesh: "tire.mesh"
          diffuse: "tire_diffuse.png"

rim_front_left_visual:
  components:
    Transform: { ... # relative to wheel }
    Model:
      parts:
        - mesh: "rim.mesh"
          diffuse: "rim_diffuse.png"
```

Loader:

```cpp
EntityId tire_vis = load_entity_from_yaml("tire_front_left_visual");
EntityId rim_vis  = load_entity_from_yaml("rim_front_left_visual");

EntityId w = registry.create();
registry.add<Transform>(w, wheel_xform);
registry.add<Tire>(w, Tire{ .grip = 0.9f, .wear = 0.0f });
registry.add<Wheel>(w, Wheel{ tire_vis, rim_vis });
```

***

## 5. Physics System

The physics system only cares about **wheel entities**, not the visual parts:

```cpp
void simulate_wheels(Scene& s, float dt) {
    auto& tire_store  = s.registry.storage<Tire>();
    auto& xform_store = s.registry.storage<Transform>();

    auto tires   = tire_store.all();
    auto entities = tire_store.entities();

    for (size_t i = 0; i < tires.size(); ++i) {
        EntityId e   = entities[i];
        Tire&    t   = tires[i];
        auto&    xf  = xform_store.get(e); // wheel transform

        // Example: raycast down from wheel center to detect ground contact
        t.grounded = raycast_ground(xf.world);

        // Use t.grip, t.wear, t.grounded to compute forces, update car velocity, etc.
    }
}
```

Visual tire/rim entities are only touched in the **render system**, which iterates over `Model` components and uses `Element.mesh`/`Element.diffuse_map` with the `Vault`. [ppl-ai-file-upload.s3.amazonaws](https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/attachments/19276299/9636aef5-fd1c-4f88-b222-e72fa5475f65/design.md)

***

## 6. Why physics data on the wheel, not on the visual tire entity?

- The logical **wheel** is what participates in physics (contact patch, torque, suspension), not the render mesh.
- You want one source of truth for “this wheel’s tire grip/wear”, independent of which specific mesh is currently used.
- Visual variants are interchangeable; the physics still refer to the same `Tire` component on the wheel entity.
- This keeps systems simple: physics iterate over `Tire` components, rendering iterates over `Model` components — no cross-contamination of concerns. [ppl-ai-file-upload.s3.amazonaws](https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/collection_c5469497-c908-436d-8de6-86c82805e2da/7f8211f3-9d41-487f-9be3-76212e9dc2e4/design.md)

Would you like a second small example file that shows the same pattern but for a character with interchangeable helmets/armour, to solidify the pattern?