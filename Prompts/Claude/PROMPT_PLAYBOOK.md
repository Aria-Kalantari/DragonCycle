# Claude Code prompt playbook — DragonCycle

How to drive Claude Code through this repo without it drifting off-spec,
faking editor work, or breaking the quality gates. Prompts below are ordered
by dependency, not by milestone number. Paste them one at a time from the
project root, where `CLAUDE.md` and the Unreal MCP client config are picked
up together.

`FIRST_RUN.md` is still the correct first prompt (it targets M0.1). Everything
here assumes M0.1 already compiled.

---

## 1. Why prompting this repo is different

This is a spec-governed codebase with hard guardrails already written down.
The prompts do not need to *contain* the design — they need to *point at it*
and refuse the shortcuts Claude Code otherwise takes. Three failure modes to
design every prompt against:

- **Scope creep.** It will happily implement M1.4 flight while you asked for
  M1.1 movement. Name one task and forbid the rest.
- **Phantom editor work.** It cannot create `.uasset` files or set editor
  settings from source, but it will narrate them as done. Force it to declare
  Unreal MCP connected/not-connected before it claims any editor result.
- **Retuning around the falsification thresholds.** When a number in
  `Docs/TELEMETRY.md` is inconvenient it will quietly soften it. Tell it to
  surface the breach instead.

Everything below is built to close those three doors.

## 2. The anatomy of a good prompt here

A reliable prompt for this repo has six parts, in this order:

1. **Context to read** — the exact spec/doc/source files for this task, not
   "read the docs."
2. **One task** — the single `TASKS.md` id, stated as the only deliverable.
3. **Boundaries** — what *not* to touch (adjacent tasks, editor claims,
   schema/enum edits, generated folders).
4. **Definition of done** — the concrete artifacts and the gate the task
   must pass (compile, `validate_data.py`, `pytest`, telemetry single-source).
5. **Verification to run** — the exact commands, and "stop and report if red."
6. **Bookkeeping** — update `TASKS.md` only if genuinely complete; list
   changed files, tests, risks, and any manual editor steps left.

If a prompt is missing part 3 or part 4, expect drift.

## 3. Reusable template

Copy this and fill the five slots. This is the backbone for any task not
pre-written below.

```text
Read SPEC.md, CLAUDE.md, TASKS.md, and <DOCS/SOURCE FILES FOR THIS TASK>.
Restate the task and the systems it touches before writing code.

Implement ONLY <TASK ID + one-line description>. Do not start any other
TASKS.md item. Do not add features beyond the smallest vertical slice that
makes this task real and testable.

Respect CLAUDE.md: C++ for <math/data/schema>, data assets over hard-coded
morphotype branches, telemetry only from the single CombatEvent source,
never edit Binaries/DerivedDataCache/Intermediate/Saved, never claim editor
work you did not perform through a connected integration.

Done when: <concrete artifacts> exist and <gate> passes. Add a test for any
non-trivial deterministic math.

Run: python Tools/validate_data.py and python -m pytest -q. If anything is
red, stop and report — do not retune thresholds or comment out checks.

Then update TASKS.md only if genuinely complete, and summarize changed
files, tests, risks, and remaining manual editor steps.
```

## 4. Ready-to-paste prompts (dependency order)

The critical path out of Milestone 0 is: **M0.6 importer → M0.7 telemetry →
editor bootstrap (M0.3–M0.5) → M1.1 movement.** M0.6 and M1.1 are the two
hard blockers — the spec says so in `TASKS.md`. Do them in this order.

### P1 — M0.6: JSON → `UDragonSpeciesDataAsset` importer (unblocks all body-state work)

```text
Read SPEC.md, CLAUDE.md, Docs/DECISIONS.md (ADR-006), Data/Schemas/
dragon_species.schema.json, Data/Dragons/*.json, Source/DragonCycle/
DragonSpeciesDataAsset.h, and DragonTypes.h.

Implement ONLY TASKS.md M0.6: the importer that turns the authoring JSON in
Data/ into UDragonSpeciesDataAsset instances, per ADR-006 where JSON is the
authoring source and the data asset holds runtime authority. Do not start
M0.7 or any Milestone 1 task.

Requirements:
- The schema's part-id enum and EDragonBodyPart must stay one closed set. If
  the JSON and EDragonBodyPart disagree, fail the import with a clear error;
  do not silently drop or invent parts, and do not edit the enum to match bad
  data without flagging it.
- Populate every field on the data asset that the JSON defines, including
  BodyParts, wing area/mass, neck segments, launch mode, and base assessment.
- Reuse UDragonSpeciesDataAsset::ValidateDefinition; the importer must reject
  anything that fails validation.
- No hard-coded per-species branches — the importer is generic over the schema.

Done when: importing drake_prime_adult.json, wyvern_prime_adult.json, and
longneck_prime_adult.json produces valid assets, round-trips the closed
part-id set, and the module compiles.

Run python Tools/validate_data.py and python -m pytest -q; add a test for the
schema<->enum closed-set check. If red, stop and report.

Update TASKS.md M0.6 only if complete. Report which steps (if any) must run
inside Unreal Editor vs. which are pure source, and whether Unreal MCP is
connected.
```

### P2 — M0.7: `CombatEvent` + round-summary writer (must land before the first attack)

```text
Read Docs/TELEMETRY.md in full, SPEC.md (telemetry + tags sections), and
CLAUDE.md.

Implement ONLY TASKS.md M0.7: the CombatEvent struct and the RoundSummary
writer, matching the event schema and per-round aggregates in
Docs/TELEMETRY.md. Do not implement any attack, ability, or AI logic — this
task is the instrument, not the thing it measures.

Hard rules:
- ONE event source. CombatEvent is the single origin for animation, VFX,
  audio, UI, AI, and telemetry. Do not add a parallel analytics path.
- win_condition is exactly the five values in TELEMETRY.md (wing disable,
  stamina attrition, lethal damage, terrain kill, opponent disengage).
- Build the ten falsification thresholds in as reportable fields/metrics now,
  even though nothing emits them yet. Do not soften or re-derive any threshold.

Done when: a CombatEvent can be constructed and logged, a RoundSummary can be
opened/appended/flushed to disk, the field set matches TELEMETRY.md, and the
module compiles. Add tests for deterministic aggregation math (durations,
distributions, occupancy accumulation).

Run python Tools/validate_data.py and python -m pytest -q. If red, stop and
report rather than adjusting a threshold.

Update TASKS.md M0.7 only if complete; summarize files, tests, and risks.
```

### P3 — M0.3–M0.5: Editor bootstrap (MCP-aware, do NOT fake it)

```text
Read Docs/UNREAL_EDITOR_CHECKLIST.md, CLAUDE.md, and TASKS.md M0.2–M0.5.

FIRST: tell me whether Unreal MCP (ModelContextProtocol) is connected right
now. Check — do not assume. State "connected" or "not connected" explicitly
before doing anything else.

IF CONNECTED: perform M0.3–M0.5 through Unreal MCP — create
L_BasaltCaldera_Greybox, GM_DragonCycle, PC_DragonCycle, BP_Drake_Player from
ADragonPawn, the Enhanced Input assets and IMC_Dragon listed in the checklist,
the collision channels, and the temporary skeletal proxy with wing/jaw/neck/
tail/forelimb/hind-limb bones. Issue MCP tool calls ONE AT A TIME — they run
serially on the game thread. Verify each asset exists after creating it.
Assign the species data asset from the M0.6 importer to BP_Drake_Player; do
not hand-author it.

IF NOT CONNECTED: do not create or claim any .uasset. Instead emit a precise,
ordered manual checklist I can follow in the editor, and mark these tasks as
editor-blocked in your summary.

Either way: verify the SPEC §13.3 gameplay tags already exist in
Config/DefaultEngine.ini rather than recreating them. Update TASKS.md only for
steps genuinely completed and verified.
```

### P4 — M1.1: `UDragonMovementComponent` (unblocks all of Milestone 1)

```text
Read SPEC.md (locomotion + flight sections), Docs/BIOMECHANICS.md,
Docs/COMBAT_MODEL.md, Docs/TECH_ARCHITECTURE.md, CLAUDE.md, and
Source/DragonCycle/DragonPawn.h.

Implement ONLY TASKS.md M1.1: create UDragonMovementComponent as a custom
UPawnMovementComponent and wire it onto ADragonPawn. Do NOT implement M1.2
locomotion tuning, launch gating, or flight yet — this task is the component
and its integration seam only, with a compiling stub for each movement mode.

Constraints from CLAUDE.md and the design:
- C++ for all movement math. Wing loading (MassKg / WingAreaM2 from the data
  asset) drives the speed/agility trade — read it from the species data asset,
  never hard-code per-morphotype values.
- No per-frame heap allocation, no repeated full-world searches.
- Do not put physics-critical behavior in an animation Blueprint.
- Leave clearly-marked extension points for M1.2–M1.6 rather than stubbing
  them with fake numbers.

Done when: the component compiles, attaches to ADragonPawn, consumes the
species data asset, and exposes the debug-drawable state M1.7 will need
(current mode, velocity, wing-loading). Add a unit test for any deterministic
helper (e.g. GetWingLoading-derived values).

Run python Tools/validate_data.py and python -m pytest -q. If red, stop and
report. Update TASKS.md M1.1 only if complete; list follow-up tasks unblocked.
```

## 5. Prompt patterns worth reusing

**The "read-then-restate" gate.** Every prompt starts by naming files to read
and asks Claude to restate the task before coding. This catches
misunderstanding while it is still cheap. Keep it.

**The single-task lock.** "Implement ONLY <id>. Do not start <adjacent id>."
The explicit forbid matters more than the instruction — name the neighbor it
is most likely to wander into (here, always the next task in the same
milestone).

**The editor honesty clause.** "State connected or not connected before doing
anything." This is the single most valuable sentence in this repo's prompts,
because faked editor work is invisible until you open the editor and find
nothing there.

**The threshold lock.** "If red, stop and report — do not retune thresholds or
comment out checks." Without it, a failing gate becomes a softened gate.

**The bookkeeping tail.** "Update TASKS.md only if genuinely complete; list
changed files, tests, risks, remaining editor steps." This is your audit trail
between sessions.

## 6. Anti-patterns — do not send these

- **"Build the movement system."** No task id, no boundary → it builds three
  milestones badly. Always scope to one `TASKS.md` id.
- **"Read the docs and do the next thing."** It picks the wrong next thing.
  Name the id and the files.
- **"Set up the input actions and the map."** Mixed source + editor work in one
  prompt lets it blur what it actually did. Split editor tasks into their own
  MCP-aware prompt (P3).
- **"Make the numbers pass."** Invites retuning around falsification
  thresholds — the one thing `Docs/TELEMETRY.md` exists to prevent.
- **"Also, while you're in there…"** Every "while you're in there" is scope
  creep wearing a friendly hat. New concern → new prompt.

## 7. Between-session hygiene

- Start each session from the project root so `CLAUDE.md` + MCP config load.
- Have Claude Code run `git status` and read `TASKS.md` first so it works the
  real next unblocked item.
- After a task lands, skim the diff yourself before the next prompt — the
  cheapest place to catch a wrong turn is before it compounds.
- Keep one task per prompt per commit. It keeps the git history bisectable and
  the telemetry attributable.
