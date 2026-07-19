# DragonCycle Starter Kit

A handoff-ready design and engineering starter repository for a grounded-fantasy, fully 3D dragon combat game built with **Unreal Engine 5.8** (including its Experimental Unreal MCP plugin), **Claude Code**, and **Higgsfield**.

Rebased on the dragon species research in July 2026. See `Docs/RESEARCH_BASIS.md` for what changed and what was deliberately not adopted.

The repository is intentionally scoped around one vertical slice: a player-controlled **Drake** versus an AI **Wyvern** in a basalt caldera. The slice must prove that two giant animals can fight on the ground, grapple, take off, pursue each other in the air, damage specific body parts, tire, overheat, assess whether the fight is worth continuing, and change tactics — without becoming a dodge-roll game.

The roster is three morphotypes, not a bestiary: **drake**, **wyvern**, and **arch-serpent**. The arch-serpent and the forest arena are slice 2.

## Start here

1. Read `SPEC.md`. It is the authoritative product specification.
2. Read `Docs/BIOMECHANICS.md`. It is the constraint contract that makes the morphotypes different, and parts of it are machine-checked.
3. Read `CLAUDE.md`. It defines how Claude should work in this repository.
4. Open `TASKS.md` and take the first unblocked task.
5. Create a C++ Unreal project from `DragonCycle.uproject` using Unreal Engine 5.8.
6. Run `python Tools/validate_data.py` and `python -m pytest -q` before committing data changes.

Before planning against any schedule, read `Docs/PROTOTYPE_PLAN.md`. The
research's 12-week figure assumes a staffed team.

## Recommended workflow

```bash
# From the repository root
claude

# In Claude Code
/plan-slice
```

For Higgsfield setup, see `Docs/ART_PIPELINE_HIGGSFIELD.md`.

## Repository map

- `SPEC.md` — source of truth for product scope and acceptance criteria.
- `CLAUDE.md` — concise instructions automatically loaded by Claude Code.
- `TASKS.md` — ordered implementation backlog.
- `Docs/BIOMECHANICS.md` — the eight constraints the roster is built inside.
- `Docs/TELEMETRY.md` — event schema and ten falsification thresholds.
- `Docs/RESEARCH_BASIS.md` — research traceability, including what was rejected.
- `Docs/RESEARCH_BIBLIOGRAPHY.md` — 46 sources mapped to constraints, with evidence status per claim.
- `Docs/PROTOTYPE_PLAN.md` — phase order, staffing reality, and what to cut first.
- `Docs/` — design, architecture, editor, art-pipeline, and decision records.
- `Data/` — authoring source for morphotypes and attacks, with JSON schemas.
- `Source/DragonCycle/` — compile-oriented Unreal C++ foundation.
- `Prompts/` — first-run instructions and production prompt packs.
- `Tools/validate_data.py` — schema, anatomy invariants, attack cross-references, and wing-loading ordering.
- `Tests/` — validator contract tests; bad data must report, never traceback.
- `Generated/` — ignored staging area for Higgsfield concepts and previs.

## Scope boundary

This starter kit does **not** include a movement component, final meshes, animations, sounds, levels, or an Unreal-generated `.sln`. `UDragonMovementComponent` is task M1.1 and everything in Milestone 1 depends on it. Those must be created in Unreal or a DCC tool. Generated AI imagery is reference material, not production-ready geometry.
