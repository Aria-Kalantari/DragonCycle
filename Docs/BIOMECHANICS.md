# Biomechanical constraint contract

This document converts the research findings into rules the prototype must
obey. It is the reason species differ. If a tuning change would violate a
rule here, it needs an ADR, not a nudged number.

`SPEC.md` §5 states the physics contract in prose. This document states it
in a form the validator and the tests can check.

Each constraint is tagged with its evidence status. **Sourced** means a paper
in `RESEARCH_BIBLIOGRAPHY.md` supports it. **Reasoned** means it is a design
inference that has not been traced to a source yet — still a good rule, but
argue with it on design grounds, not by citing evidence that is not there.

## Why this exists

The failure mode for creature combat is that every species becomes a
reskinned boss with different VFX. The defence against that is not
discipline — it is constraints that make reskinning impossible. Each rule
below removes a degree of freedom that would otherwise collapse the roster
back toward one animal.

## The eight constraints

### 1. Wing loading trades speed against agility — *sourced (19, 20, 22, 23)*

Wing loading is mass divided by lifting area. Heavier-loaded fliers are
faster and turn worse. This is monotonic across the roster and
`Tools/validate_data.py` enforces it: no morphotype may be both more
heavily loaded and more agile than another.

Current roster:

| Morphotype | Mass (kg) | Wing area (m²) | Loading (kg/m²) | Max turn (°/s) |
|---|---:|---:|---:|---:|
| Drake | 6000 | 110 | 54.5 | 45 |
| Longneck | 3400 | 78 | 43.6 | 62 |
| Wyvern | 2200 | 95 | 23.2 | 95 |

Consequence: the drake cannot solve the wyvern by out-turning it. It has to
deny the sky, control ground, or win the one exchange it gets.

### 2. Launch is a gated state, not a jump — *sourced (17, 18)*

Large fliers need a forceful launch, and launch mode differs by anatomy.
Quadrupedal launch loads the forelimbs and pectoral system; bipedal launch
loads the hind limbs; a serpentine body springs from a coil.

This means takeoff is **denied**, not slowed, when its preconditions fail:
insufficient run-up or clearance, impaired limbs for the relevant launch
mode, wing integrity below threshold, or stamina below the launch cost.
There is no universal escape-by-flight from a losing ground position. The
vulnerability window during launch is a design feature.

### 3. Bite force scales with size, so mass pays on the confirm — *sourced (24)*

Heavy morphotypes should be lethal on a clean secured bite rather than
through long strings of medium hits. Damage per contact scales with mass;
contacts per second does not.

### 4. A secured bite can convert to rotational tearing — *sourced (25, 29)*

Body rotation on a held bite generates shear that grows disproportionately
with body size. This is why the drake has `drake_secured_twist` and the
wyvern does not: the wyvern's bite is a snap, not a grip.

Implementation: a secured bite grants `State.BiteSecured`. Follow-ups gate
on that state and are the morphotype's damage ceiling **and** its longest
punish window. Never a guaranteed chain — the held party retains a costly
emergency response.

Long slender jaws trade structural resistance for other feeding performance
(29), which is the reason the wyvern and arch-serpent cannot convert: a
slender jaw will not take the torsion. Design intuition first, source second.

### 5. Neck length buys reach and angle, and costs inertia — *sourced (86–92)*

Long necks enable off-axis entry, feints around cover, and hook-and-pull
bites. They cost retraction time and expose a long committed line.

`neck_segments` drives threat-envelope radius, not damage. Neck impairment
shortens reach **before** it reduces bite damage — a wounded longneck loses
its identity before it loses its output, which is the point.

Cervical joints couple their rotations: in birds, axial rotation and
lateroflexion are strongly linked and joint mobility is predictable from
vertebral morphology (86). A long neck does not swing freely in any direction
— it moves along preferred paths, which is why off-axis entry has a
characteristic shape rather than being omnidirectional.

The inertia cost is measured, not assumed. Craniocervical modelling of a large
theropod sets neck-muscle accelerative capacity explicitly against the
rotational inertia of head and neck (88), and in very long necks most of the
length turns out to have limited mobility (89) — length buys reach at the cost
of controllable articulation.

**Neck morphology splits into two predation styles** (87), and this is the most
useful thing in the whole constraint. Theropod necks divide into rapid-strike
architectures and powerful puncture-and-pull architectures. The roster should
follow: the arch-serpent has a rapid-strike neck, the drake a puncture-and-pull
neck. Same body part, opposite tuning.

**A neck can be the dispatch mechanism, not just the delivery arm.** *Smilodon*
had roughly one third the bite force of a comparable lion — about 1,100 N
against 3,400 N — and a skull poorly optimised to resist struggling prey, but
was well optimised for bites on *restrained* prey where cervical musculature
augments the jaw (90). Follow-up work extends this to neck and forelimb
musculature driving the kill (91, 92).

That is the arch-serpent exactly: weak jaw, strong neck, and a kill that only
works once the target is held. It gives the morphotype its own damage ceiling
analogous to the drake's rotation, reached by a completely different route —
restrain first, then dispatch. See `longneck_neck_dispatch` in `attacks.json`.

A second, physiological cost applies specifically to holding the neck up:
heart-to-head distance and blood pooling constrain elongated animals in
head-up posture (31, 32). A sustained raised-neck guard should drain and have
a hard limit. Scheduled as S2.6.

### 6. High-speed interception is set up, not steered — *sourced (47–50); the no-re-aim rule is reasoned*

Fast aerial predators buy kills with altitude, angle, and prediction. They
cannot re-aim continuously through a committed stoop.

Peregrines steer terminal attacks by **proportional navigation** — the same
feedback law used by guided missiles (48). That law is tuned for high-speed
open-air interception and performs poorly against highly manoeuvrable targets;
hawks pursuing agile prey through clutter use a different, mixed law coupling
proportional navigation with pursuit (49). Simulation shows high-speed stoops
beat low-altitude attacks against erratic prey *only* given tightly tuned
guidance and high visual and control precision (47).

So the wyvern's speed is not free. It buys closing rate and pays in the
ability to follow a target that changes its mind.

`wyvern_dive_rake` has a minimum entry speed, requires the `diving` state, and
cannot be re-aimed once the window opens. A missed stoop is the wyvern's most
punishable state. Endless mid-air retargeting is banned.

The no-re-aim rule is inference, not measurement: no study locates a point of
no return inside a stoop or times the recovery after a miss. What is measured
is the control precision the manoeuvre demands. If the wyvern later needs a
small mid-dive correction to feel good, that is a legitimate design change and
not a violation of evidence — say so in an ADR rather than pretending the
literature forbade it.

### 7. Sensing tracks foraging mode — *sourced within squamates (95–97); cross-clade is reasoned*

Aerial predators are built around distance vision. Diurnal raptors have
rod-free, double-cone-free foveae with high cone and ganglion-cell densities
and high temporal resolution (51), and the deep central fovea gives peak
acuity in the *lateral* visual field — configured for spotting prey a long
way off (52). Visual fields and eye-movement range differ between pursuit and
scavenging raptors (53), which is why line-of-sight breaking works at all.

Ground and serpentine predators are built around close range. Crocodilian
integumentary sensory organs — thousands of them — carry rapidly-adapting
units tuned to water-surface vibration around 20–35 Hz, with mechanical
sensitivity finer than a primate fingertip (55, 56). Snakes lost effective
pressure hearing but kept acute substrate-vibration sensitivity around
80–160 Hz through jaw-quadrate coupling to the inner ear (57), and use
tongue-flick chemoreception to trail prey (58, 59).

Two corrections the design must respect, or it will overstate its own basis:

- **Infrared is not a serpentine trait.** Pit organs and their TRPA1-based
  transduction exist in vipers, pythons and boas (54) — not snakes generally.
  The arch-serpent does not get heat sensing unless a specific lineage is
  authored with it, and that becomes a content decision with a stated source.
- **Crocodilian ISOs are multimodal** — mechanical, thermal and chemical (55).
  Calling them vibration receptors is a simplification.

The organising principle is now sourced, though at a narrower scope than the
design uses it. Across 94 lizard and snake species, foraging mode rather than
phylogeny drives chemosensory investment (97); ambush foragers lean on vision
while active foragers add vomerolfaction, with matching morphology in tongue
form and vomeronasal receptor density (95, 96). Sensory investment tracks *how
an animal hunts*, not what clade it belongs to — which is precisely the rule
the roster needs.

What does not exist is a single analysis scoring sensory investment across
body plans as distant as raptor, serpent and crocodilian. Within squamates the
principle is established; the raptor-versus-serpent contrast is extrapolation
from it. State it that way rather than implying a cross-clade study.

This is what makes concealment credible without invisibility — see
`COMBAT_MODEL.md` on line-break concealment.

### 8. Age is a curve with three axes, not a level — *sourced against large birds and mammals (60–67, 100, 101); see ADR-014*

Age, condition, and experience are independent. Aging raises tactical
discipline and threat presence while lowering burst acceleration, recovery
speed, and sustained output.

| Stage | Mass / reach / armour | Burst & recovery | Tactical discipline |
|---|---|---|---|
| Hatchling | lowest | high relative recovery | none |
| Juvenile | rising | high burst, poor endurance | poor, impulsive |
| Prime adult | peak integrated | peak | moderate |
| Elder | slight decline | reduced burst | high |
| Ancient | specialised or degraded | lowest burst, slow recovery | highest |

Senescence in the wild is real, not a captivity artifact: evidence spans 175
species across 340 studies (60), with within-individual decline measurable
longitudinally (61). Locomotor performance follows the same rise-then-decline
curve across species as distant as a nematode, a mouse, a dog, a horse and a
human (62), and muscle power declines roughly linearly from around age 30 in
the best-documented case, driven by motor-unit loss (63).

Compensation is documented too, and in a reptile: collared lizards with lower
maximal sprint capacity use a *greater percentage* of it when foraging and
escaping (64), and realised escape speed predicts survival better than maximal
capacity does (65). That is the mechanical basis for an ancient that wins on
judgement — it is running closer to its ceiling, more of the time, in better
chosen situations.

`condition` is the third axis: an injured or underfed prime adult should lose
to a healthy elder. It degrades independently of life stage and is the hook
for persistent injury across a legacy cycle.

**On reference class.** No study measures sprint speed, bite force, endurance,
or recovery against advancing adult age in any reptile, and ADR-014 decides
not to wait for one. Reinke (30) shows ectotherm aging is unusually diverse,
with some turtles near negligible senescence — but that is plausibly downstream
of low metabolic rate and indeterminate growth, and this animal is the
opposite: high metabolic cost, powered flight, stamina-limited throughout. The
reference class is large birds and mammals, where the curve is documented.

The reptile literature is worth understanding anyway, because it is where the
citation traps are. The
closest attempt compares one-year against two-to-three-year adult lizards and
finds essentially no decline (98) — an age range far too narrow to test
senescence. The reptile performance literature is almost entirely ontogenetic:
alligator bite force *rises* from hatchling to adult (99), which is the
opposite of what this constraint claims and must never be cited in its support.
A review from inside the field concedes reptiles are an untapped aging model
with few performance-trait studies (102).

Performance senescence in ectotherms is real where it has been looked for —
bite and jump both decline with age in crickets, at different rates (100), and
both critical swimming speed and burst escape speed decline in guppies (101).
Those are the honest citations. The reptile version is not unsupported so much
as unstudied, and the difference matters: this is a gap in the literature, not
a weakness in the design.

The composite "physique down, experience up" curve also stitches together two
literatures never measured in the same combat system. Both halves are sourced;
the join is design.

An ancient must be able to beat a prime adult through range control, feints,
terrain, and energy discipline — and must become **more** punishable in long
chases and repeated high-g turns. If ancients simply win, the curve is wrong.

## What is checked automatically

| Rule | Enforced by |
|---|---|
| Wing loading vs turn rate ordering | `validate_wing_loading_order` |
| Flying morphotype needs area and launch mode | `validate_species_invariants` |
| Attacks only require parts the morphotype has | `validate_attack_cross_references` |
| Follow-ups require a state something grants | `validate_attack_cross_references` |
| Anatomy matches the declared body plan | `validate_species_invariants` |

Rules 3, 5, 6, 7, and 8 are not statically checkable. They are covered by
the telemetry thresholds in `TELEMETRY.md`, which is the only way to find out
whether they survived contact with tuning.

## Evidence gaps

Closed July 2026 across two passes. `RESEARCH_BIBLIOGRAPHY.md` holds 104
sources mapped constraint by constraint.

Three items are not evidence and are labelled so wherever they appear:

- **The no-re-aim rule (§6)** — inference from measured guidance and control
  limits. Closed as a decision in ADR-014: the abort window is tuned, not
  sourced.
- **The cross-clade sensory comparison (§7)** — established within squamates,
  extrapolated beyond them.

Reptile performance senescence was the third. ADR-014 resolves it by changing
the reference class rather than waiting for a study nobody has run.

Neither remaining item is an open research task. Both are labelled positions,
and changing either takes a design argument and an ADR — not a literature
search.
