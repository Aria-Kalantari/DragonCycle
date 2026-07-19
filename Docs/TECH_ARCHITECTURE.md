# Technical architecture

## Runtime layers

### Foundation

- `ADragonPawn`: physical combat avatar and component owner.
- `UDragonAbilitySystemComponent`: abilities, tags, effects.
- `UDragonAttributeSet`: health, stamina, heat, breath reserve, balance.
- `UDragonBodyStateComponent`: localized integrity and capability multipliers.
- Future `UDragonFlightMovementComponent`: grounded and aerial movement.
- Future `UDragonCombatComponent`: attack windows, hit deduplication, combat events.

### Data

- `UDragonSpeciesDataAsset`: anatomy, movement baseline, body profiles.
- `UDragonAttackDataAsset`: attack geometry and resource profile.
- JSON mirrors in `Data/` support review, generation, and validation. Unreal assets remain runtime authority after import.

### Presentation

- Animation Blueprint reads movement and impairment state.
- Gameplay Cues drive Niagara, audio, decals, and camera response.
- UI subscribes to attributes and body-state events.

### AI

- AI Controller owns perception and StateTree.
- Tactical utility functions are deterministic C++ or Blueprint function-library calls.
- StateTree chooses mode; utility layer chooses a valid action within the mode.

## Event contract

Use a single Blueprint-visible combat event struct containing:

- attacker and target;
- ability/attack tag;
- target body part;
- raw and applied damage;
- balance damage;
- contact point and normal;
- relative velocity;
- resulting impairment flags.

Avoid separate, inconsistent damage notifications for UI, AI, and VFX.

## Data ownership

- Species data: immutable definition asset.
- Current body integrity: body-state component.
- Global resources: GAS attribute set.
- Action state: GAS tags and active abilities.
- Movement state: movement component.
- Long-term progression: SaveGame object.

## Dependency direction

```text
Data Assets -> Pawn Components -> GAS/Movement/AI -> Presentation
```

Presentation must never be the sole source of gameplay state.
