# Decision log

Record decisions that refine or intentionally override `SPEC.md`.

## ADR-001 — Vertical slice matchup

**Status:** Accepted  
**Decision:** Prime Adult Dragon versus Veteran Prime Adult Wyvern in Basalt Caldera.  
**Reason:** This matchup exercises the largest contrast between stable ground grappling and aerial pursuit while sharing enough anatomy for a manageable first production slice.

## ADR-002 — Grounded-fantasy physics

**Status:** Accepted  
**Decision:** Use one explicit aether-organ premise to support giant flight and breath weapons. Preserve inertia, fatigue, heat, injury, and spatial contact.  
**Reason:** Strict Earth biology would remove the core fantasy; unrestricted magic would remove the desired physical realism.

## ADR-003 — AI architecture

**Status:** Accepted  
**Decision:** StateTree for high-level modes plus utility scoring for actions.  
**Reason:** Modes remain readable in editor while action choice can respond continuously to injury, energy, altitude, and species instincts.

## ADR-004 — Final asset policy

**Status:** Accepted  
**Decision:** Higgsfield output is concept/previs reference, not final production geometry, collision, rigging, or animation.  
**Reason:** Production assets need controlled topology, anatomy consistency, licensing review, and gameplay-specific iteration.

## ADR-005 — Three morphotypes, built in sequence

**Status:** Accepted
**Context:** The kit shipped six species definitions while the slice only ever needed two. The four unused files were the source of the anatomy-vocabulary drift.
**Decision:** Roster is drake, wyvern, and arch-serpent (longneck). Built in that order: drake vs wyvern for slice 1, longneck added in slice 2. Everything else moves to reserve archetypes in `DRAGON_TAXONOMY.md`.
**Consequences:** Fewer species, more depth per species. Adding a lineage becomes content work rather than a new body plan. Supersedes the six-family taxonomy in SPEC v0.1.

## ADR-006 — JSON authors anatomy, DataAsset consumes it

**Status:** Accepted
**Context:** `Data/*.json` and `UDragonSpeciesDataAsset` were two unreconciled sources of truth with incompatible part vocabularies and no importer. This was the largest structural defect in the kit.
**Decision:** JSON is the authoring format and the review surface. A commit-time importer generates the data assets. The C++ `EDragonBodyPart` enum and the schema's part-id enum are the same closed set and must be changed together. Runtime authority remains the imported asset.
**Consequences:** One place to author, one place to review, drift caught by the validator instead of by a bug six weeks later. Costs an importer (task M0.6) before any body-state work.

## ADR-007 — StateTree plus utility scoring; no Behaviour Trees

**Status:** Accepted
**Context:** The research recommends StateTree for macro phases plus Behaviour Trees and Blackboard for local tactics.
**Decision:** Keep StateTree plus a utility-scoring layer. Do not add Behaviour Trees.
**Reason:** The research's contribution is the decision inputs — assessment, resource value, persistence, terrain — and those are container-agnostic. Two AI authoring systems means two debugging surfaces and a standing question of which owns a decision.
**Consequences:** Utility scoring must stay readable in the editor. Revisit if tactical authoring becomes painful. Reaffirms ADR-003.

## ADR-008 — AI-vs-AI observer mode is a first-class deliverable

**Status:** Accepted
**Context:** Player skill masks system behaviour. If a morphotype only reads as itself when a human drives it, the identity lives in the player's head.
**Decision:** Both fighters AI-drivable, free observer camera, headless batch runs, telemetry per round. Lands before the third morphotype.
**Consequences:** The AI must be good enough to fight itself before the roster grows. This is the harness the falsification thresholds in `TELEMETRY.md` run on.

## ADR-009 — Difficulty scales judgement, never statistics

**Status:** Accepted, refined July 2026
**Decision:** Difficulty tiers change assessment quality, timing discipline, and energy management. No tier may alter health, damage, reaction speed, or grant access to hidden player input.
**Refinement:** The tiers are ordered self-assessment first, opponent-reading later — easy tracks its own cost badly, medium tracks it well, hard adds opponent-reading, very hard adds skill. This follows ADR-011 rather than the intuition that a smarter opponent primarily reads the player.
**Consequences:** Harder opponents disengage from bad fights earlier. If a tier cannot be made to feel harder this way, that is evidence the assessment model is underpowered, not licence to add a multiplier. The difficulty axis itself is a design translation — no literature frames contest skill as a tunable dial — and is labelled *reasoned*.

## ADR-011 — Cumulative assessment, not mutual assessment

**Status:** Accepted
**Context:** The rebase introduced an assessment model in which each combatant estimated its own and its opponent's fighting ability, weighted roughly equally. Sourcing that model surfaced a problem: across species, self-assessment and cumulative assessment are better supported than full mutual assessment (79), and the standard method for inferring mutual assessment produces false positives (80).
**Decision:** Implement the cumulative assessment model (75). Withdrawal is driven by accumulated cost — self-inflicted effort plus opponent-inflicted damage — against a threshold set by subjective resource value and persistence (74, 78). Opponent-reading becomes a weighted modifier via `MutualAssessmentWeight`, capped by experience rank and enforced by the validator. Skill is modelled as a separate axis from raw capability (81), and winner/loser effects update self-assessment as a decaying tally (82).
**Consequences:** The disengage rule is now a cost comparison rather than a capability comparison, which is both better supported and cheaper to compute. A naive individual can no longer be authored with veteran-grade opponent-reading. Threat displays signal motivation rather than ability (85), so they move resource value and persistence, not self-capability.

## ADR-010 — Telemetry ships with the first combat loop

**Status:** Accepted
**Context:** Realism and fun are separable, and the usual failure is trading the second for the first without noticing.
**Decision:** The combat event schema, round summaries, and the ten falsification thresholds in `TELEMETRY.md` are implemented alongside the first attack, not in a polish phase.
**Consequences:** Slower first attack, far cheaper balance work. There is exactly one combat event source; a parallel analytics path is forbidden.

## ADR-012 — Evidence labelling is part of the spec

**Status:** Accepted
**Context:** Two sourcing passes closed most of the project's biomechanical claims and left three that cannot be closed the same way. Undifferentiated, a design document reads as uniformly authoritative, and a future contributor cannot tell which rules are findings and which are positions.
**Decision:** Every constraint in `BIOMECHANICS.md` and `COMBAT_MODEL.md` carries an evidence label — *sourced*, *reasoned*, or *unstudied* — with source numbers into `RESEARCH_BIBLIOGRAPHY.md`. Changing a rule labelled *sourced* requires engaging the evidence. Changing one labelled *reasoned* requires only a design argument.
**Consequences:** Three claims stay non-evidence: the no-re-aim rule inside interception, the cross-clade sensory comparison, and reptile performance senescence. The first two are inference from measured constraints. The third is a hole in the literature — no study measures burst performance against adult age in any reptile — and is marked *unstudied* rather than *reasoned*, because the distinction matters: one is our reasoning, the other is nobody's data.
**Anti-pattern recorded:** source 99 measures alligator bite force *rising* from hatchling to adult. It is ontogenetic, not senescent, and must never be cited in support of age-related decline. It is flagged in the bibliography for exactly this reason.

## ADR-013 — Two damage ceilings, incompatible preconditions

**Status:** Accepted
**Context:** Sourcing the neck constraint surfaced that theropod necks divide into rapid-strike and puncture-and-pull architectures, and that saber-tooth prey dispatch was neck- and forelimb-powered rather than jaw-powered, working only on restrained prey.
**Decision:** The drake and the arch-serpent reach comparable damage ceilings by opposite routes. The drake secures a bite and rotates, force coming from body mass. The arch-serpent anchors, restrains, then dispatches with cervical musculature — `longneck_neck_dispatch`, gated on both `bite_secured` and `anchored`.
**Consequences:** The arch-serpent's weak jaw becomes a design feature rather than a balance problem. Its ceiling is harder to reach — two preconditions instead of one — and correspondingly harder to escape once reached. Validated by the existing follow-up-state cross-check.

## ADR-014 — Two evidence gaps closed as decisions, not tasks

**Status:** Accepted
**Context:** Two claims survived both sourcing passes without evidence: the abort point inside a committed stoop, and age-related performance decline in reptiles. Neither can be closed by searching — the first study has not been run, the second is a hole in the literature. Left in `TASKS.md` they would sit open indefinitely, implying work someone could do, and inviting every future agent to propose researching them again. An open task nobody can complete decays into noise.

**Decision 1 — the stoop abort window is a playtest value.**
No published measurement locates the point at which a raptor can no longer re-aim. It would not help if it existed: such a figure would describe a one-kilogram bird at roughly 100 m/s, and the wyvern is 2,200 kg. The constant does not transfer across three orders of magnitude of mass. What transfers is the structure, and that is already sourced (47–50): proportional-navigation guidance, a load factor two to three times the target's, and commitment as a real constraint rather than a stylistic one. The abort window is tuned in the editor and recorded in `TELEMETRY.md`, not sourced.

**Decision 2 — the aging reference class is large birds and mammals, not reptiles.**
Reptile performance senescence is unstudied, and the instinct to wait for reptile data is wrong here. Reinke (30) is precisely the paper showing ectotherm aging is unusually *diverse*, with some turtles approaching negligible senescence — which is the one condition under which extrapolating from reptiles would be unsafe. But negligible senescence in turtles is plausibly downstream of low metabolic rate and indeterminate growth, and this design commits to the opposite: high metabolic cost, powered flight, and stamina-limited everything. For a six-tonne powered flier the appropriate reference class is large birds and mammals, where the decline curve is documented (62, 63), supported by ectotherm performance senescence where it has been measured (100, 101). SPEC §2 says grounded fantasy, not strict zoology; insisting on reptile sourcing here would make the model less accurate, not more.

**Consequences:** R1g and R1h are removed from the backlog. Both claims keep their evidence labels — the no-re-aim rule is *reasoned*, the decline curve is *sourced against birds and mammals* — so nothing is laundered into looking settled. If either study is ever published, this ADR is superseded rather than reopened.

**Watch item:** the no-re-aim rule is the likeliest thing in this document to feel bad in play, because it removes control at the highest-adrenaline moment. When that surfaces, the temptation will be to hunt for evidence permitting a small mid-dive correction. The honest position is that no evidence ever forbade one. Changing it is a design decision requiring an ADR, not a research question.

## ADR-015 — Canonical morphotype skeletons; art is swappable, systems bind to the skeleton

**Status:** Accepted
**Context:** Slice-1 fighters use purchased placeholder meshes that are not final art (Protofactor Mountain Dragon as the drake, Protofactor Wyvern as the wyvern — a coherent, same-studio, same-rig pair chosen over a higher-fidelity but mismatched cinematic asset). The player drake in particular will likely be replaced later by a bespoke or higher-fidelity model, and no marketplace carries a hero-tier quadruped drake to match a cinematic wyvern, so a final art pass means commissioned assets regardless of what is bought now. Without a rule, every system that touches the character — hit zones (M2.2), impairment consequences (M3.x), Control Rig, the animation blueprint, GAS montages, sockets, physics assets — risks binding to a specific vendor's skeleton. That would turn a later art swap into a rebuild instead of a re-skin, and would quietly re-introduce the morphotype-specific coupling ADR-005 and the CLAUDE.md engineering rules forbid.

**Decision:** Systems bind to a canonical per-morphotype skeleton, never to a vendor asset. Define one canonical drake skeleton and one canonical wyvern skeleton (arch-serpent later) whose bone hierarchy realises the closed `EDragonBodyPart` set — head, jaw, neck (a segmented chain sized from `NeckSegments`), torso, wing_left/right, forelimb_left/right, hindlimb_left/right, tail — under a fixed, documented bone-naming convention. Every purchased or generated mesh is retargeted onto the canonical skeleton with Unreal's IK Retargeter. Hit-zone components, Control Rig, animation blueprint, montages, sockets, and physics assets are authored once against the canonical skeleton. Placeholder art (Protofactor now) and any future cinematic or bespoke asset are interchangeable skins over the same skeleton. This is the Mannequin retarget-hub pattern, applied per morphotype.

**Consequences:** Upgrading art later costs a bounded tech-art pass — re-skin, retarget the animation set, re-hook rig/physics for the new mesh — not a re-authoring of movement, combat, damage, AI, or telemetry, none of which ever referenced the mesh. The wyvern's four limbs and the drake's six are expressed as skeleton differences, not code branches. Buying the cheap coherent Protofactor pair now is therefore the economical choice: the money to spend once is on final assets built to the canonical skeleton, not on an interim cinematic asset that would itself be replaced. The one real cost is the neck — a mesh whose neck bone count differs from the morphotype's `NeckSegments` must have its chain reconciled to the canonical skeleton before retargeting.

**Watch item:** the arch-serpent's nine-segment neck (S2.1) is the stress test for this rule. Its canonical skeleton must exist before its consequence systems (S2.2 neck reach) are built, or those systems will accrete against whatever placeholder arrived first — exactly the coupling this ADR exists to prevent.

## Pending template

### ADR-XXX — Title

**Status:** Proposed / Accepted / Rejected / Superseded  
**Context:**  
**Decision:**  
**Consequences:**  
