



# Q1
Question related to how entities are created and represented

Representation: Say we have a car in our scene, with the current ecs design we could define it in some way (e.g. yaml) and load it with our ecs: create an entity ID, load the meshes and textures into the vault, create Models and assign them to the car entity id. But we would want to have custom behaviour that is specifically for cars: e.g. we would give it more attributes like engine power, fuel level, tire quality and when simulating it we would take its position in the world look at the car state and depending on whether the tires are making contact with the floor we update its velocity and position. How do we implement this in a data-driven way, is there a separate concept for this or is it just simply making use of the ECS? E.g. new component types {car, tire, engine, fuel} and in the definition we could define it like so:

```
car_standard: // standard car has 4 wheels
    wheels: // custom attribute for car
    - wheel_front_right: // is this an entity?
        tire: // this is an entity then? I'm not sure
            name: "tire_front_right"
            transform: ... // front right
            model:
            parts:
                - mesh: "tire.mesh"
                diffuse: "tire_diffuse.png"
                // no transform -> Identity matrix assigned for this part (means this takes the tires transform?)
        rim: // this is also an entity?
            name: "rim_front_right"
            transform: ... /front right
            model:
            parts:
                - mesh: "rim.mesh"
                diffuse: "rim_diffuse.png"
                // no transform -> Identity matrix assign for this part (means this takes the rims transform?)
    - wheel_front_left: ...
    - wheel_back_right: ...
    - wheel_back_left: ...
    engine:
    name: "engine"
    horse_power: 200 // custom attribute for engine
    transform: ... // optional, could be hidden in the car, so we may now show it if we want?
    model: ... //optional, because it's hidden in the car so we may not show it?
```

## **Answer**
See [this file](./example_entity_composition_car.md)


# Q2
**A followup question to Q1:** when defining a scene (e.g. in yaml) it's useful to have hierarchies (e.g. car has 4 wheels, wheel has tire and rim), but what about within the entitiy component system or in a data-oriented design? We could keep the flat arrays and have a hierarchy by adding child or parent IDs to component structs that need hierarchy, but this means that the systems processing these would walk the hierarchy graphs. Is this okey or should we try to avoid hierarchies like these?

## **Answer**
Short answer: hierarchies are fine and often necessary; keep storage flat, represent hierarchy with `EntityId` links, and only walk as deep as you truly need. [ppl-ai-file-upload.s3.amazonaws](https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/collection_c5469497-c908-436d-8de6-86c82805e2da/7f8211f3-9d41-487f-9be3-76212e9dc2e4/design.md)

### What YAML hierarchy means

YAML hierarchy is **authoring structure**, not storage layout:

- Scene file: `car → wheels → tire/rim` makes it easy for humans to express relationships.
- Loader: flattens this into entities in `Registry` plus cross-entity `EntityId` references (parent, children, or both).
- ECS storage remains flat sparse-sets (`Storage<T>`). 

So it is normal that the data in memory is flat and you reconstruct relationships via IDs when needed.

### Ways to store hierarchy

Pick one of these simple patterns (they all keep arrays flat):

1. **Parent-only link (most common, simple)**  
   - Each child has `parent: EntityId` in some component.
   - Example:

     ```cpp
     struct Parent {
         EntityId parent; // 0 = no parent
     };
     ```

   - To process a car and its wheels, start at car entity, query all entities whose `Parent.parent == car_id` (via a small index, not a full scan once it matters).

2. **Children list on parent**  
   - Parent has `std::vector<EntityId> children;`.
   - Example (car-specific):

     ```cpp
     struct CarState {
         std::array<EntityId, 4> wheel_entities;
         EntityId                engine_entity;
     };
     ```

   - This is already what you sketched for the car: you know the 4 wheels without traversing a general graph.

3. **Both parent and children (when you need flexibility)**  
   - Parent component on child, plus a `children` vector on parent.
   - Use only when you really need two-way traversal.

In your current use case (cars, wheels, visual parts), **option 2 plus a few specific “links” components is enough**.

### Is walking the hierarchy okay?

Yes, with a few constraints:

- Keep **per-frame work predictable**:
  - For common cases (cars have 4 wheels), store direct `EntityId`s in components (`CarState`, `Wheel`) so systems don’t need to do arbitrary graph traversals.
- For deep or variable hierarchies (e.g. scene graph for generic transforms):
  - Have one dedicated **Transform hierarchy system** that:
    - Walks parent→children once per frame.
    - Writes `Transform.world` for all entities. [ppl-ai-file-upload.s3.amazonaws](https://ppl-ai-file-upload.s3.amazonaws.com/web/direct-files/attachments/19276299/9636aef5-fd1c-4f88-b222-e72fa5475f65/design.md)
  - Other systems then just read `Transform.world` in a flat loop, no graph walking.

Example: simple transform graph system:

```cpp
struct Parent { EntityId parent; };        // 0 = root

void update_transforms(Scene& s) {
    auto& xforms = s.registry.storage<Transform>();
    auto& parents = s.registry.storage<Parent>();

    // Simplest version: process in an order where parents come before children
    auto xs   = xforms.all();
    auto ents = xforms.entities();

    for (size_t i = 0; i < xs.size(); ++i) {
        EntityId e = ents[i];
        auto& xf   = xs[i];
        if (!s.registry.has<Parent>(e)) {
            xf.world = xf.local;
        } else {
            EntityId p = s.registry.get<Parent>(e).parent;
            auto& px   = s.registry.get<Transform>(p);
            xf.world   = px.world * xf.local;
        }
    }
}
```

Here, **only one system walks parent links**. Everyone else just reads `world`.

### When to avoid hierarchy

Avoid general-purpose deep hierarchies for everything. Instead:

- Use **specific, shallow links** for hot gameplay data:
  - `CarState` holds 4 wheels.
  - `Wheel` holds 2 visual entities.
- Use a generic transform graph only when you need arbitrary nesting (e.g. attaching a camera to a bone, placing props on moving platforms).

For your engine’s current scope and learning goals, I would:

- Implement a **small transform hierarchy** (Parent + Transform update system).
- For game-specific relationships (car → wheels, wheel → visual parts), use **explicit `EntityId` fields** in components rather than a generic “children vector on every entity”.



# Scene validity
  - Q2 - Validity: some combinations of components on an entity do not make sense, e.g. we could have an entity for a gravitational point that pulls stuff towards itself, so there could be a component called "Gravitation". But it does not make sense to add a "Gravitation" component to a car, how could I specify constraints or rules so it is not possible to make weird errors like this. However I would like to specify the constraints myself, I don't want to have a strict system that always requires some kind of set of constraints on all entities. The idea of a car that gravitates stuff towards itself sound funny, so you never know, maybe I'll make a game using this render engine and have cars with gravitation :)