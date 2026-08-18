# Tinape (This is not a physics Engine)

**Author:** Robert Andreas Fritsch

## Numeric Core Types

I am implementing a layered set of numeric core types:

* **UnsignedBig**: A compile-time-growing big integer type that never overflows as long as it is not cast to a smaller
  type. Due to its compile-time-growing nature, it cannot be used in summation loops without explicit casting.
* **SignedBig**: An extension of `UnsignedBig` that carries an additional sign bit.
* **RestrictedBig**: An interval-typed integer parameterized by lower and upper bounds (`RestrictedBig<lb, ub>`).
  Signedness is derived directly from the bounds rather than an independent type axis. Bounds are pure compile-time
  functions of operand bounds, so accumulation loops require an explicit cast by design.
* **FixedBig**: Stored over a `RestrictedBig` mantissa with an explicit exponent (`FixedBig<bits, shift>`), so the
  exponent logic is the only concern managed by this layer while inheriting exact bound tracking.
* **Rational**: A wrapper for formulas used to enable division when formulating equations. Divisions are optimized away
  by transforming them using cross-multiplication rules. Rationals are generally used to formulate equations that
  ultimately result in comparisons.

### Purpose

These layers allow the final project to implement functions like overlap, intersection, and containment using
numerically stable math that never loses precision. While the resulting engine updates might not be 100% physically
accurate due to the inherent quantization of the stored values, a well-executed implementation guarantees absolute
determinism and glitch-free gameplay.

## Abstract

**Tinape** is a highly specialized, deterministic 2D Continuous Collision Detection (CCD) engine designed specifically
for pseudo-3D real-time MMORPGs. It abandons traditional physics simulations—such as momentum, mass, restitution
(bouncing), and 3D depth—in favor of strict, mathematically provable boundary logic. By treating collisions as permanent
velocity space restrictions rather than physical impulses, Tinape guarantees absolute determinism, $O (1)$ iteration
limits, and native immunity to infinite loops (Zeno's Paradox), all while supporting massive entity densities.

## 1. Core Design Philosophy

Tinape is built on the principle that MMO server architecture does not need physical realism; it needs fast, unbreakable
rules. The engine evaluates whether an entity is permitted to pass a boundary or not. If a boundary is hit, the entity
slides. There are no complex polygon meshes, no physics materials, and no floating-point approximations.

### Entity Interaction Matrix

To minimize unnecessary calculations, collision checks are strictly filtered by entity type:

* **Players:** Collide with Players, Monsters, and Static Objects.
* **Monsters:** Collide with Players and other Monsters. They pass freely through Static Objects.
* **Static Objects:** Guaranteed to never move, removing them from all dynamic spatial updates.

## 2. Primitives and One-Sided Barriers

The engine utilizes only two geometric primitives, both engineered to act as one-sided barriers.

* **Static Environment (Line Segments):**
  Environment borders are modeled as simple closed polygonal chains, broken down into singular line segments during
  preprocessing.
* *One-Sided Rule:* A line segment allows an entity to cross from the right side of the describing vector to the left
  side. Crossing from left to right is strictly blocked.


* **Dynamic Entities (Circles):**
  All players and monsters are represented as circles.
* *Positive Radius:* Acts as a repulsor. Moving objects are allowed to escape its bounds but cannot enter.
* *Negative Radius:* Acts as a container. Moving objects are allowed to enter its bounds but cannot escape.

## 3. Mathematical Foundation

Tinape relies entirely on **Exact Fixed-Point Arithmetic**.

* **No Floating-Point Errors:** By avoiding 32-bit or 64-bit floats, the engine prevents positional jitter and
  non-deterministic rounding errors.
* **Exact Comparisons:** The results of fixed-point operations (like dot products or cross products) are stored in data
  types guaranteed to hold the exact result without truncation. At the end of a collision function, the boolean result
  (pass or block) is mathematically perfect.
* **Trigonometry-Free:** Angles and rotations are never calculated using expensive trigonometric functions (e.g., sine,
  cosine). All sliding and boundary logic is resolved using pure vector mathematics (Dot Products and Cross Products).

## 4. Spatial Partitioning (The 4-Chunk Guarantee)

To solve the $O (N^2)$ complexity of spatial queries, Tinape uses a uniform grid system with hard constraints built into
the game design.

* **The Constraint:** The engine enforces strict maximums on both circle radius and entity movement speed per frame.
* **The Guarantee:** Because the maximum possible swept area (size + speed) is known, the grid size is tuned so that a
  moving entity can intersect a maximum of **four chunks** in a single frame.
* **The Result:** Spatial lookups are $O (1)$ integer divisions. The broad phase simply gathers the entities located in
  the up to four chunks touched by the swept bounding box of the entity's maximum theoretical movement.

## 5. The Execution Pipeline

Every server tick, Tinape executes a highly optimized Continuous Collision Detection (CCD) pipeline utilizing Simulation
Islands.

1. **Broad Phase (Pair Generation):**
   Using the 4-chunk grid, the engine generates a list of potential collision pairs based on the swept AABB of each
   entity's original, unhindered movement vector. Because sliding only subtracts velocity, this bounding box is
   absolute.
2. **Island Generation:**
   A Weakly Connected Components algorithm groups overlapping collision pairs into isolated "Simulation Islands." Each
   island is mathematically independent and can be processed in parallel across multiple CPU threads.
3. **Global Time-Stepping (TOI Loop):**
   For each island, the engine iterates through time ($t=0.0$ to $t=1.0$):

* Calculate the exact Time of Impact (TOI) for all pairs.
* Find the earliest TOI.
* Advance all positions in the island to that exact time.
* Apply the collision resolution (sliding) to the colliding entities.
* Update velocities and recalculate TOIs for the affected entities.
* Repeat until the earliest TOI is greater than the remaining frame time.

## 6. Velocity Space Restriction (Sliding Resolution)

When an entity hits a barrier, Tinape does not calculate a "bounce." Instead, it permanently restricts the entity's
valid movement cone for the remainder of the frame.

* **The Global Half-Plane Restriction:** When a circle hits a line (or another circle), a normal vector is calculated.
  The entity's velocity is projected along the tangent (sliding).
* **The Dot Product Lock:** The normal of that collision is stored. For the rest of the tick, any subsequent slide
  vector is checked against all previously hit normals using a Dot Product. If the dot product is negative (meaning the
  new slide attempts to push back into a previously restricted half-plane), the velocity is instantly forced to zero.
* **Visual Result:** Entities glide smoothly along walls, slide off one another, and elegantly stop perfectly wedged in
  corners without jittering.

## 7. Architectural Guarantees

Because of the mathematical rules enforced by Robert Andreas Fritsch's design, Tinape offers three ironclad guarantees
rare in standard physics engines:

1. **Finite Iteration Bound:** A collision permanently eliminates that pair from colliding again in the same frame. For
   an island of $M$ entities, the absolute maximum number of collision resolutions is exactly $\frac{M (M-1)}{2}$. The
   loop can never run infinitely.
2. **Immunity to Zeno’s Paradox:** Because relative velocity projected onto the collision normal can mathematically only
   be positive or zero ($V_{rel} \cdot N \geq 0$), entities physically cannot "chatter" or trigger infinite
   micro-collisions. No artificial time-step minimum ($\epsilon$) is required to break loops.
3. **Absolute Determinism:** The combination of fixed-point math, integer-based spatial grids, and strict algebraic
   velocity restrictions guarantees that identical inputs will yield byte-for-byte identical outputs across all server
   architecture and client hardware.

## Layout

- `include/tinape.hpp` — implement `solve()` reading `std::cin`, writing `std::cout` (rebound to fast in-memory streams
  by the runner; output is compared byte for byte, no trimming).
- `test/*.in` / `test/*.out` — input/output pairs, baked into one ctest binary each.
- `test/local/` — extra pairs that run locally only (skipped in CI).

## Build & test

```sh
cmake --preset debug          # clang, ASan+UBSan, max diagnostics
cmake --build --preset debug -j
ctest --preset debug

cmake --preset release        # clang, -O3 -march=native -flto
cmake --build --preset release -j
ctest --preset release
```

## Lint

```sh
cmake --build --preset debug --target format-check  # clang-format-20, report only
cmake --build --preset debug --target tidy-check    # clang-tidy-20, report only
```

## Hooks

```sh
python3 hook.py   # installs pre-push hook: format-check + tidy-check must pass
```
