# DragonCycle implementation backlog

Status legend: `[ ]` not started, `[-]` in progress, `[x]` done, `[!]` blocked.

Rebased on the dragon species research, July 2026. Slice 1 is **drake vs
wyvern in the Basalt Caldera**. The arch-serpent and the forest arena are
slice 2. See `Docs/PROTOTYPE_PLAN.md` for phase order and what to cut first.

## Milestone 0 — Repository, data spine, and editor bootstrap

- [x] M0.1 Generate project files and compile the empty `DragonCycle` module in Unreal Engine 5.8.
- [x] M0.2 Enable GameplayAbilities, GameplayTags, GameplayTasks, EnhancedInput, Niagara, StateTree, GameplayStateTree, MotionWarping, ControlRig, IK Rig, and EQS plugins.
- [x] M0.3 Create `L_BasaltCaldera_Greybox`, `GM_DragonCycle`, and `BP_Drake_Player` (via `Tools/editor_bootstrap_m03.py`). `SpeciesData` was wired to the generated `DA_Drake_PrimeAdult` once M0.6 landed.
- [x] M0.4 Create Enhanced Input assets listed in `Docs/UNREAL_EDITOR_CHECKLIST.md` (via `Tools/editor_bootstrap_m04.py`). Triggers (hold vs press) left default; M1 ability wiring decides them.
- [ ] M0.5 Import a temporary skeletal proxy with wing, jaw, neck, tail, forelimb, and hind-limb bones.
- [ ] M0.5a **Establish the canonical drake and wyvern skeletons and the retarget hub (ADR-015).** Fixed bone-naming convention realising the `EDragonBodyPart` closed set plus the segmented neck; an IK-Retargeter workflow that maps any purchased/generated mesh onto them. Hit zones (M2.2), Control Rig, AnimBP, montages, sockets, and physics bind to the canonical skeleton, never to a vendor asset. The M0.5 proxy — and the placeholder Protofactor meshes — are the first meshes retargeted onto it, so art stays swappable.
- [x] M0.6 **Write the JSON to `UDragonSpeciesDataAsset` importer (ADR-006).** `UDragonSpeciesImportCommandlet` in the `DragonCycleEditor` module: `-run=DragonSpeciesImport`. Generic over the schema, fails non-zero on any error, skips rewriting unchanged assets so reruns produce no LFS churn. Closed-set drift is covered by `Tests/test_closed_set.py`.
- [x] M0.7 Implement the `CombatEvent` struct and round-summary writer from `Docs/TELEMETRY.md`. Lands before the first attack, not after. `FDragonCombatEvent` (full schema), `FDragonRoundSummary` + `FDragonRoundSummaryWriter` (JSONL), the single-source `UDragonTelemetrySubsystem`, and the F1-F10 report (`FDragonFalsificationReport`) all in the runtime module. Aggregation math covered by `DragonTelemetryTests.cpp` (automation) and the doc/threshold contract by `Tests/test_telemetry_contract.py`. No editor assets. No emission wiring yet — that arrives with the M2 damage pipeline.

## Milestone 1 — Heavy locomotion and gated flight

- [ ] M1.1 Create `UDragonMovementComponent` (custom `UPawnMovementComponent`). Everything below depends on it.
- [ ] M1.2 Implement grounded acceleration, braking, turn inertia, and slope handling.
- [ ] M1.3 Implement launch gating by mode: run-up and clearance, limb integrity for the relevant launch mode, wing integrity, stamina. Failure denies flight rather than slowing it.
- [ ] M1.4 Implement assisted flight intent: lift, drag, thrust, banking, stall margin, driven by wing loading from the data asset.
- [ ] M1.5 Implement glide, dive-energy conversion, climb cost, and landing.
- [ ] M1.6 Apply left/right wing impairment to lift and roll authority.
- [ ] M1.7 Add flight-vector, wing-loading, and stall-margin debug drawing.
- [ ] M1.8 Tune dynamic camera distance and FOV.

## Milestone 2 — Contact combat

- [ ] M2.1 Initialize body-part states from the imported `UDragonSpeciesDataAsset`.
- [ ] M2.2 Create hit-zone components bound to proxy bones, including neck.
- [ ] M2.3 Add attack-window interface driven by montage notifies, with `commit_class` recovery.
- [ ] M2.4 Implement the drake kit: bite, foreclaw, tail sweep, shoulder ram, breath.
- [ ] M2.5 Enforce `min_entry_speed_ms`: deny the ram below the floor rather than scaling it down.
- [ ] M2.6 Implement brace and balance damage.
- [ ] M2.7 Emit `CombatEvent` from the damage pipeline; add hit log and body-zone overlay.
- [ ] M2.8 Confirm low stamina reduces movement and recovery well before zero.

## Milestone 3 — Consequence model

- [ ] M3.1 Wing damage causes asymmetry, takeoff denial, and forced landing.
- [ ] M3.2 Leg damage affects launch impulse, sprint, and grounded stability.
- [ ] M3.3 Jaw and throat damage affects bite and breath.
- [ ] M3.4 Neck damage shortens reach and breath arc **before** it reduces damage.
- [ ] M3.5 Tail damage affects balance and aerial yaw.
- [ ] M3.6 Add visible animation and VFX placeholders for each impairment.

## Milestone 4 — Grappling and secured-bite conversion

- [ ] M4.1 Detect valid grapple contact and anatomical anchors.
- [ ] M4.2 Compute leverage from mass, orientation, grip, support, and stamina.
- [ ] M4.3 Grant `State.BiteSecured` on a clean bite against a valid anchor.
- [ ] M4.4 Implement `drake_secured_twist`: shear scaling with mass and rotation rate, longest punish window in the kit.
- [ ] M4.5 Implement outcomes: pin/throw, disengage, cliff or aerial fall.
- [ ] M4.6 Support interruption by damage, balance loss, wing deployment, and terrain impact.
- [ ] M4.7 Add Motion Warping and Control Rig alignment after contact validation.

## Milestone 5 — Wyvern AI and assessment

- [ ] M5.1 Create `ST_Wyvern_Combat` high-level states.
- [ ] M5.2 Implement utility scoring with morphotype, injury, energy, altitude, and terrain inputs.
- [ ] M5.3 Implement `FDragonAssessment` updates from observable evidence only.
- [ ] M5.4 Implement the cumulative assessment withdrawal rule (ADR-011): accrued cost against a threshold from resource value and persistence, with opponent-reading as a weighted modifier.
- [ ] M5.4a Implement winner/loser effects as a decaying outcome tally feeding self-capability.
- [ ] M5.4b Implement skill as an axis distinct from capability: accuracy, tactic selection, decision latency.
- [ ] M5.5 Implement difficulty tiers as judgement quality (ADR-009). No stat multipliers.
- [ ] M5.6 Implement aerial attack runs with locked stoop lines and no mid-dive retargeting.
- [ ] M5.7 Implement wyvern kit, wing targeting, and injury protection.
- [ ] M5.8 Add Veteran pattern memory without reading hidden input.
- [ ] M5.9 Add AI debug overlay showing current assessment and disengage margin.

## Milestone 6 — Test harness

- [ ] M6.1 Make the player drake AI-drivable.
- [ ] M6.2 Add the free observer camera and AI-vs-AI encounter mode.
- [ ] M6.3 Add headless batch runs with per-round telemetry output.
- [ ] M6.4 Implement all ten falsification thresholds from `Docs/TELEMETRY.md` as reportable metrics.
- [ ] M6.5 Run the blind observer protocol; record results in `Docs/DECISIONS.md`.

## Milestone 7 — Arena and presentation

- [ ] M7.1 Greybox basin, shelves, pillars, thermal, and cliff risk.
- [ ] M7.2 Implement line-break concealment: silhouette reduction, cover, and morphotype-specific detection. Aerial detection is distance-vision based; ground and serpentine detection is close-range. Do not give the arch-serpent infrared — pit organs exist only in specific lineages.
- [ ] M7.3 Implement threat displays with an intimidation radius. Displays signal motivation, not ability — they move the opponent's resource value and persistence, not its estimate of your capability.
- [ ] M7.4 Add representative drake and wyvern proxy animation set.
- [ ] M7.5 Implement combustion breath Niagara placeholder and heat feedback.
- [ ] M7.6 Add body-status silhouette and resource HUD.
- [ ] M7.7 Add accessibility and flight-assist settings.
- [ ] M7.8 Run performance and five-player usability test pass.

## Slice 2 — Arch-serpent and forest

- [ ] S2.1 Author `longneck_prime_adult` rig with a nine-segment neck chain.
- [ ] S2.2 Implement neck reach as a function of segment integrity.
- [ ] S2.3 Implement terrain anchoring and coil drag as a feedback-modulated contest: effort tracks the target's resistance rather than running a fixed authored duration.
- [ ] S2.3a Implement `longneck_neck_dispatch` (ADR-013): neck-and-forelimb powered kill gated on both `bite_secured` and `anchored`. The arch-serpent's damage ceiling, reached by restraint rather than rotation.
- [ ] S2.3b Tune the two neck architectures apart — arch-serpent rapid-strike, drake puncture-and-pull — so the same body part reads differently in animation and in threat envelope.
- [ ] S2.6 Implement head-up posture cost: sustained raised-neck guard and threat display draw stamina and have a sustain limit, from heart-to-head distance and blood pooling in elongated bodies.
- [ ] S2.4 Greybox the forest arena with occlusion lines and stealth routes.
- [ ] S2.5 Validate that terrain occupancy differs measurably between arenas (threshold F4).

## Slice 3 — Legacy cycle

- [ ] S3.1 Create adaptation data model and compatibility slots.
- [ ] S3.2 Record combat tendencies from encounters.
- [ ] S3.3 Persist `condition` and permanent injury across cycles.
- [ ] S3.4 Build a deterministic previous-hero profile from saved choices.
- [ ] S3.5 Prototype one visible adaptation and one tactical inheritance.

## Research tasks that may run in parallel

- [x] R1 Consolidate the research bibliography and map it to the constraints (`Docs/RESEARCH_BIBLIOGRAPHY.md`).
- [x] R1a Source the interception rule — proportional navigation guidance, sources 47–50.
- [x] R1b Source the morphotype sensing rule — sources 51–59.
- [x] R1c Source the aging decline profile — sources 60–67.
- [x] R1d Source the assessment model — contest theory, sources 68–85. Drove ADR-011.
- [x] R1e Verify three flagged citations. Enquist & Leimar confirmed 127(2): 187–205; Leitch & Catania confirmed; Briffa corrected to Briffa & Sneddon 2007, issue 4.
- [x] R1f Close the remaining reasoned claims. Neck leverage and skill-as-axis now sourced; sensing sourced within squamates.
- [x] R1g/R1h Closed as decisions in ADR-014, not as open research. The stoop abort window is a playtest value; the aging reference class is large birds and mammals. Do not reopen these as research tasks.
- [ ] R1i Verify Huey et al. 1990 (pre-DOI, JSTOR) and the Baeckens et al. 2017 epub/print split before the bibliography is treated as final.
- [ ] R2 Validate scale, wing loading, and launch assumptions against a documented reference range.
- [ ] R3 Produce Higgsfield anatomy and movement prompt outputs for drake and wyvern.
- [ ] R4 Decide the wyvern tail weapon after silhouette and animation tests.
- [ ] R5 Define target PC hardware tiers.
- [ ] R6 Enable Unreal MCP + All Toolsets, generate the Claude Code client config, and record scope limits and review policy in `DECISIONS.md`.
