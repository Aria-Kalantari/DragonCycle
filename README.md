# DragonCycle

**Grounded-fantasy, fully 3D dragon-versus-dragon combat for PC — built in Unreal Engine 5.8.**

DragonCycle asks a question most dragon games skip: what if two giant animals actually fought like animals? Not a hero dodge-rolling under a boss's scripted attacks — two creatures that can each attack, defend, interrupt, grapple, and reposition at any moment; that take off and pursue each other through the air, tire, overheat, lose the use of a wing or a leg, read whether the fight is still worth it, and break off when it isn't.

## The thesis

The whole project exists to answer one design question, and it is deliberately measurable:

> Do three body plans produce combat that reads as distinct and plays as fun?

An observer with the HUD off should be able to name the creature from its behavior alone within a few rounds. If a drake only looks like a drake when a human plays it, the identity lives in the player's head instead of in the simulation — and that is a failure the telemetry is built to catch, not paper over.

## What makes it different

Each of these is a hard design constraint, not a feature wishlist:

- **Anatomy is destiny, not stats.** The roster is three morphotypes — **drake**, **wyvern**, **arch-serpent** — that differ by body plan and ecology, not by a stat slider or a VFX palette. A wyvern's wings *are* its forelimbs; an arch-serpent has no legs and a long segmented neck. Mythic lineages are content dressed over these three plans, never a fourth.
- **Body-part damage changes what you can do.** Wreck a wing and takeoff is denied and roll authority drops; damage the neck and reach and breath-arc shrink *before* raw damage does; hurt a leg and launch, sprint, and footing all suffer. The AI feels these losses and re-plans around them.
- **Stamina degrades performance before it hits zero.** You feel a tiring dragon slow down — not a bar that only matters once it is empty.
- **Flight is earned, not free.** Wing loading trades speed against agility (a heavier flyer is never also more nimble). Takeoff is gated by launch mode, limb integrity, clearance, and stamina — fail the gate and you stay grounded. A committed dive cannot be re-aimed mid-stoop.
- **The opponent reads like an animal.** It estimates its own capability and yours from what it can actually observe, tallies the accruing cost of the fight, and withdraws when the prize is no longer worth the risk. It never cheats by reading your inputs.
- **Difficulty scales judgement, never numbers.** Harder AI makes better decisions; it never gains health, damage, or superhuman reaction speed.

Every one of those is grounded in real biomechanics and animal-contest research — wing loading, launch energetics, aging curves, assessment theory — traced in `Docs/BIOMECHANICS.md`, `Docs/RESEARCH_BASIS.md`, and `Docs/RESEARCH_BIBLIOGRAPHY.md`.

## The vertical slice

Scope is deliberately one fight: a player-controlled **drake** versus an AI **wyvern** in the **Basalt Caldera**. It has to prove the thesis end to end — ground fighting, grappling, gated takeoff, aerial pursuit, body-part consequence, fatigue and heat, assessment, and tactical change — without collapsing into a dodge-roll game.

- **Slice 1** — drake vs wyvern, Basalt Caldera *(in progress)*
- **Slice 2** — the arch-serpent and the forest arena
- **Slice 3** — the legacy cycle: adaptation and inheritance across generations

## Tech

- **Unreal Engine 5.8**, C++ runtime module `DragonCycle` plus an editor-only `DragonCycleEditor` module.
- **Gameplay Ability System + Gameplay Tags** for combat actions and state gating; **StateTree** for AI; **Enhanced Input**, **Niagara**, **Motion Warping**, **Control Rig / IK Rig**.
- **Data-driven anatomy.** JSON in `Data/` is the authoring source; a commandlet importer generates the runtime `UDragonSpeciesDataAsset` (ADR-006), so anatomy has a single source of truth and the C++ enum and the schema are one closed set.
- **Telemetry-first.** A single `CombatEvent` feeds animation, VFX, audio, UI, AI, and analytics alike, and ten falsification thresholds (`Docs/TELEMETRY.md`) are written as failure conditions — able to prove the design broke rather than flatter it.
- **Unreal MCP** (experimental) lets Claude Code drive the editor directly; the client config lives in `.mcp.json`.

## Status

Milestone 0 (foundations) is nearly complete: the module compiles, plugins and Enhanced Input are in, the greybox caldera and game mode exist, and both the JSON→data-asset importer and the telemetry instrument are built and tested. Remaining in M0: a temporary rigged skeletal proxy (M0.5). Next is Milestone 1 — the custom `UDragonMovementComponent` and gated flight, which the rest of the combat depends on. `TASKS.md` is the live backlog.

## Building it

1. Install **Unreal Engine 5.8** (Epic Games Launcher) and **Visual Studio 2022** with the *Desktop development with C++* workload (MSVC v143 14.4x toolchain — not the banned 14.39).
2. Right-click `DragonCycle.uproject` → *Generate Visual Studio project files*, or simply open it and accept the module rebuild.
3. Data changes must pass `python Tools/validate_data.py` and `python -m pytest -q` before being committed.
4. To let Claude Code drive the editor: enable **Unreal MCP** and **All Toolsets** in *Edit → Plugins*, run `ModelContextProtocol.StartServer` in the console, then launch `claude` from the repository root so it picks up `.mcp.json` and `CLAUDE.md`.

Binary assets (`.uasset`, `.umap`, `.fbx`, audio, textures) are tracked with **Git LFS** — run `git lfs install` before your first commit.

## Repository map

- `SPEC.md` — authoritative product spec and acceptance criteria.
- `CLAUDE.md` — how Claude Code should work in this repository.
- `TASKS.md` — ordered implementation backlog (milestones M0–M7, then slices 2–3).
- `Data/` — authoring source for morphotypes and attacks, with JSON schemas.
- `Source/DragonCycle/` — runtime C++: types, attributes, body state, species data asset, telemetry.
- `Source/DragonCycleEditor/` — editor-only tooling, including the species importer commandlet.
- `Content/DragonCycle/` — greybox level, game mode, input assets, and generated data assets.
- `Docs/` — design, architecture, telemetry, research, and decision records.
- `Prompts/` — first-run instructions and the Claude Code prompt playbook.
- `Tools/` — `validate_data.py` and the idempotent editor-bootstrap scripts.
- `Tests/` — data-validator and telemetry-contract tests; bad data must report, never traceback.

## Documentation worth reading first

- `Docs/BIOMECHANICS.md` — the constraint contract that makes the morphotypes differ (partly machine-checked).
- `Docs/TELEMETRY.md` — the `CombatEvent` schema and the ten falsification thresholds.
- `Docs/DECISIONS.md` — architecture decision records (ADRs) for every approved deviation from the spec.
- `Docs/PROTOTYPE_PLAN.md` — phase order, honest staffing reality, and what to cut first. The research's 12-week figure assumes a staffed team; read this before planning against any schedule.
