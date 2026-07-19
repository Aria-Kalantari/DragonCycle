# Telemetry and falsification

Realism and fun are different properties, and the usual failure is trading
the second for the first without noticing. Telemetry is how this project
notices. It is not a polish-phase feature — it lands with the first combat
loop, because without it the slice cannot answer its own design question.

## The design question

> Do three body plans produce combat that reads as distinct and plays as fun?

An observer should identify each morphotype from behaviour alone within a
few rounds, with the HUD off.

## AI-vs-AI observer mode

The single most valuable test harness here, and it is cheap: the AI already
has to drive one dragon. Drive both, add a free camera, run the matchup
headless at speed, and log.

This exists because player skill masks system behaviour. If the drake only
looks like a drake when a human plays it, the species identity lives in the
player's head rather than in the simulation. Run every matchup pairing,
every difficulty, both maps, hundreds of rounds.

## Event schema

One combat event feeds animation, VFX, audio, UI, AI, and telemetry. Do not
add a parallel analytics path — a second event source will drift.

```text
CombatEvent
  timestamp, encounter_id, round_id
  attacker_id, target_id, morphotype pair
  ability_tag, commit_class
  target_body_part, damage layers, applied damage, balance damage
  contact point, contact normal, relative velocity
  attacker stamina/heat/breath at contact
  resulting impairment flags
  attacker state tags, target state tags
  terrain cell, altitude AGL
```

Per-round aggregates:

```text
RoundSummary
  duration, winner, win_condition
  time airborne / grounded / grappled per fighter
  stamina exhaustion events, disengage events, re-engage events
  dive attempts and conversion rate
  grapple attempts and outcome distribution
  terrain occupancy heatmap
  body-part damage distribution
  assessment trace: capability estimates and disengage decisions
```

`win_condition` must be one of: wing disable, stamina attrition, lethal
damage, terrain kill, opponent disengage. A slice where one condition
dominates has a balance problem the aggregate numbers will hide.

## Falsification thresholds

These are pass/fail, evaluated from AI-vs-AI runs. They are written as
failure conditions on purpose — it is easier to be honest about a threshold
you can breach than a goal you can approach.

| # | The system has failed if | Measured by |
|---|---|---|
| F1 | The heaviest morphotype wins mostly through durability | win_condition distribution; lethal-damage wins > 60% for drake |
| F2 | The lightest morphotype wins mostly through kiting | drake time-in-contact < 25% of round duration in drake-vs-wyvern |
| F3 | Observers cannot name the morphotype from behaviour | blind observer test, < 80% correct within three rounds |
| F4 | Terrain does not change behaviour | terrain occupancy heatmaps correlate > 0.85 across the two maps |
| F5 | Difficulty raises stats rather than judgement | hard-tier win rate rises while disengage timing quality is flat |
| F6 | Body damage does not change tactics | post-impairment ability distribution ≈ pre-impairment |
| F7 | Stamina only matters at zero | no measurable behaviour change in the 30–60% band |
| F8 | One win condition dominates | any single win_condition > 55% across all matchups |
| F9 | Ancients simply beat prime adults | ancient win rate > 60% at equal experience |
| F10 | Realism cost the fun | median round outside the 5–10 minute target, or playtest engagement drops as realism rises |

F10 is the one that matters most and is the hardest to measure. Do not let
the nine countable thresholds crowd it out.

## Instrumentation order

1. CombatEvent emission and a round summary writer — with the first attack.
2. Terrain occupancy sampling — with the first map.
3. Assessment trace — with the first disengage logic.
4. AI-vs-AI runner and observer camera — before the second morphotype.
5. Blind observer protocol — before any balance tuning is treated as final.

Step 4 lands before the second morphotype on purpose. The first time two
species exist is the first time species identity can be wrong, and finding
that out by hand is far more expensive than building the harness.
