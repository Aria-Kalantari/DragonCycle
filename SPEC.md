# DragonCycle — Product and Technical Specification

**Status:** Authoritative v0.2 — rebased on the dragon species research, July 2026  
**Target:** PC, single-player, Unreal Engine 5.8  
**Primary tools:** Unreal Engine, Claude Code, Higgsfield  
**Prototype codename:** DragonCycle

---

## 1. Product statement

DragonCycle is a fully 3D, grounded-fantasy action game about a legendary dragon hunter who survives by absorbing biological adaptations from defeated dragon lineages. With every victory the hunter becomes less human and more draconic. The cycle ends with a confrontation against a legendary dragon that is the transformed previous hero.

The central fantasy is not "fight a boss that happens to look like a dragon." It is **be one giant animal fighting another giant animal** across ground, cliff, and sky.

## 2. Non-negotiable pillars

1. **Dragon-vs-dragon physicality.** Combat includes collisions, weight transfer, biting, clawing, tail strikes, grappling, forced landings, takeoff denial, pursuit, and terrain interaction.
2. **Mutual offense and defense.** Both fighters can initiate, interrupt, counter, brace, disengage, or exploit damage. The player does not wait for scripted openings.
3. **No dodge-roll combat language.** There are no universal invulnerability rolls. Avoidance comes from distance, altitude, banking, body orientation, terrain, bracing, wing folding, and real displacement.
4. **Localized consequences.** Wings, legs, jaw, neck, torso, tail, and sensory organs can be impaired. Damage changes capability, animation, AI choices, and available attacks.
5. **Morphotypes, not a bestiary.** Three body plans — drake, wyvern, arch-serpent — differ in anatomy, launch mode, flight envelope, ecological role, and fighting style. Mythic lineages are a content layer dressed over these three, never a fourth engineering body plan.
6. **Fights are decisions, not scripts.** Both fighters assess the exchange and can quit it. A dragon asks whether the energy, wound risk, and position surrendered are worth the outcome. Difficulty raises the quality of that judgement, never the statistics.
7. **Age, condition, and experience are three axes.** Life stage changes physiology on a curve, not a ladder. Condition degrades independently. Experience changes tactics. An ancient can be slow and still strategically dangerous, and must be more punishable in a long chase.
8. **Grounded fantasy, not strict zoology.** One explicit impossible premise supports giant flight and breath weapons; everything else respects inertia, leverage, fatigue, heat, anatomy, and resource limits.

## 3. Explicit exclusions

- Not a Souls-like lock-on-and-roll boss game.
- Not a 2D or lane-based fighting game.
- Not an MMO, live service, PvP title, or open-world game for the first production milestone.
- Not a human sword-combat prototype disguised as a dragon game.
- No unrestricted spell list detached from anatomy.
- No endless aerial hovering by heavy species.
- No animation-only hits that ignore contact, body orientation, or reach.
- No difficulty tier that adds health, damage, or reaction speed instead of judgement.
- No invisibility stealth. Concealment is line-breaking and silhouette management.
- No continuous mid-stoop retargeting by aerial predators.
- No final production art generated directly from AI video.

## 4. Audience and experience target

- Players who want cinematic creature combat, mastery, ecology, and replayable encounters.
- Session target for the vertical slice: 5–10 minutes per fight.
- Difficulty should come from reading anatomy, momentum, terrain, and opponent intent—not memorizing arbitrary delayed attack timings.
- Camera should preserve scale while maintaining combat readability.

## 5. Grounded-fantasy physics contract

### 5.1 The one impossible premise

Dragons possess an **aether organ** that temporarily reduces effective weight during powered flight and stores high-density metabolic energy for breath attacks. It does not cancel inertia, remove fatigue, instantly heal tissue, or permit unlimited hovering.

This premise exists only to bridge the largest biological impossibilities. It must not become a general-purpose magic excuse.

### 5.2 Simulation rules

- Mass and speed produce meaningful momentum.
- Wing loading — mass over lifting area — trades speed against agility, monotonically across the roster. A heavier-loaded morphotype is never also more agile. Enforced by `Tools/validate_data.py`.
- Takeoff is a gated state with an anatomy-specific launch mode, not a jump. Failing its preconditions denies flight outright rather than slowing it.
- High-speed interception is set up rather than steered. A committed stoop cannot be re-aimed; the overshoot is the defender's opening.
- A secured bite can convert to rotational leverage for morphotypes with the mass and limb topology to apply it. Shear scales with body size.
- Neck length buys reach and off-axis angle, and costs retraction time and exposure. Neck impairment shortens reach before it reduces damage.
- Turning radius increases with speed and mass.
- Lift requires forward airflow, wing motion, or thermal assistance.
- Takeoff requires space, leg impulse, wing integrity, and sufficient stamina.
- Heavy species gain efficiency from gliding, diving, ridge lift, and thermals.
- Breath weapons consume stored reagent, heat capacity, oxygen, or tissue tolerance.
- Low stamina reduces acceleration, attack recovery, sustained grip, climb rate, and maximum turn authority.
- Severe local damage can disable a move without reducing global health to zero.
- Grapples resolve from contact geometry, leverage, relative mass, grip quality, stamina, and terrain.

## 6. Morphotypes

Three engineering body plans. See `Docs/DRAGON_TAXONOMY.md` for the naming
discipline that separates morphotype from mythic identity, and for the
reserve archetypes that map onto these three without adding a fourth.

### 6.1 Drake

- **Body plan:** four legs plus two wings; quadrupedal launch.
- **Role:** heavy apex grappler.
- **Strengths:** ground stability, best grapple leverage in the roster, secured-bite rotation, broad attack set.
- **Weaknesses:** highest wing loading, worst turn rate, large vulnerable wing area, high metabolic cost.
- **Signature behaviour:** plants, denies ground, converts one clean bite into a committed rotation.

### 6.2 Wyvern

- **Body plan:** two hind legs; wings are modified forelimbs; bipedal launch.
- **Role:** aerial interception predator.
- **Strengths:** lowest wing loading, fastest turn, efficient powered flight, fast launch, narrow profile.
- **Weaknesses:** no clinch game, cannot convert a bite to a grip, catastrophic miss-recovery, poor when grounded.
- **Signature behaviour:** refuses symmetric ground fights, buys kills with altitude and angle, targets wings.

### 6.3 Arch-serpent (longneck)

- **Body plan:** legless serpentine torso, two forelimbs, reduced wings, nine neck segments; coil-spring launch.
- **Role:** range-control duelist and terrain exploiter.
- **Strengths:** off-axis reach, feints around cover, terrain anchoring into coil drag, displacement over damage.
- **Weaknesses:** poor crush resistance in square body collisions, punishable when the neck base is jammed, low lateral stability.
- **Signature behaviour:** anchors to terrain, threatens around cover without exposing the torso, drags heavier opponents into ground they cannot fight in.

## 7. Life stage and experience

Age, condition, and experience vary independently. See `Docs/BIOMECHANICS.md` §8 for the full curve.

### 7.1 Life stages

| Stage | Physiology | Gameplay tendency |
|---|---|---|
| Hatchling | fragile, high recovery, incomplete abilities | tutorial/non-combat ecology |
| Juvenile | agile, inefficient, impulsive | fast attacks, poor endurance |
| Prime Adult | maximum power-to-weight and fertility | strongest general combat profile |
| Elder | reduced burst, stronger control and conservation | fewer wasted actions, better terrain use |
| Ancient | physically degraded or specialized, exceptional knowledge | traps, prediction, intimidation, deliberate tempo |

### 7.2 Experience ranks

Naive, Seasoned, Veteran, Master, Legendary.

Experience modifies threat assessment, feint use, memory of player patterns, retreat thresholds, target selection, terrain exploitation, and resource conservation. It must never be inferred only from age.

### 7.3 Condition

Condition is the third axis, in the range 0–1, degrading independently of life stage through injury, exhaustion, and scarcity. An injured prime adult should lose to a healthy elder. Condition is the persistence hook for permanent injury across a legacy cycle.

Aging is a curve, not a buff. Mass, reach, and armour rise through juvenile into prime adult. Burst acceleration, recovery speed, and sustained output decline from prime adult onward. Tactical discipline and intimidation presence keep rising. An ancient must be able to beat a prime adult through range control, feints, terrain, and energy discipline — and must become more punishable in long chases and repeated high-g turns. If ancients simply win, the curve is wrong.

## 8. Core game loop

1. Track a lineage through ecological evidence.
2. Prepare an assimilation loadout and choose a hunting approach.
3. Enter a bounded but spatially rich habitat.
4. Fight a dragon using ground, air, and grapple phases that emerge from physics and damage.
5. Defeat, spare, or drive off the target.
6. Absorb one constrained adaptation from the lineage.
7. Carry visible physiological and mechanical consequences into the next hunt.
8. Eventually face the previous cycle's hero as a bespoke legendary dragon assembled from their adaptations and combat history.

### 8.1 Assimilation rules

- Adaptations are anatomical or metabolic, not free-floating spells.
- The player has limited compatibility slots.
- Every adaptation has a tradeoff in mass, heat, stamina, maneuverability, vulnerability, or behavior.
- Major adaptations visibly alter silhouette or tissue.
- The final hero-dragon is generated from prior build choices and recorded combat tendencies, but must remain authored enough to be readable and fair.

## 9. Combat model

### 9.1 Combat states

- Grounded
- Launching
- Airborne powered flight
- Gliding/diving
- Grappled
- Staggered
- Forced landing
- Downed but conscious
- Dead/defeated

State changes are driven by conditions, not cinematic phase triggers. A damaged wing may force a landing; a grapple near a cliff may become an aerial fall; exhaustion may prevent relaunch.

### 9.2 Player controls — PC prototype

| Input | Ground | Air |
|---|---|---|
| Mouse | camera plus head/attack aim | camera plus flight/look intent |
| W/A/S/D | locomotion and strafe/turn intent | pitch/yaw intent with assisted coordination |
| Left Mouse | context primary: bite/claw/body strike | bite/claw/ram when valid |
| Right Mouse | brace/guard; hold to orient protected side | wing tuck/defensive posture; reduces control |
| Shift | sprint/commit power | powered wingbeat/afterburn effort |
| Space | jump/takeoff when valid | climb |
| Ctrl | crouch/lower center | descend/dive intent |
| Q/E | shoulder turn or sidestep | bank/roll intent |
| F | breath weapon | breath weapon |
| R | initiate/contest grapple | aerial grapple when contact permits |
| Tab | soft target awareness, never hard animation lock | soft target awareness |

Input must remain rebindable through Enhanced Input.

### 9.3 Resources

- **Health:** global survival state, not the sole determinant of function.
- **Stamina:** muscular and cardiovascular effort. Low stamina slows locomotion and recovery.
- **Heat:** thermal load from breath use, exertion, environment, and injury.
- **Breath reserve:** reagent or charge specific to lineage.
- **Balance:** short-horizon resistance to stagger, knockdown, and uncontrolled rotation.
- **Grip:** contextual value derived from limb integrity, contact, angle, and stamina.
- **Assessment:** accumulated cost measured against a threshold set by the subjective value of what is contested and willingness to continue. Opponent-reading modifies the threshold, weighted by experience, but does not drive it. Drives disengagement. See `Docs/COMBAT_MODEL.md` and ADR-011.

### 9.4 Damage locations and effects

| Body part | Example impairments |
|---|---|
| Head/senses | aim error, perception loss, slower reactions |
| Jaw/throat | weaker bite, interrupted breath, reagent leak |
| Neck | reduced head tracking, breath arc limits |
| Torso | global health loss, stamina and oxygen penalties |
| Left/right wing | lift loss, roll asymmetry, failed takeoff, forced landing |
| Forelimbs | weaker grapple, reduced brace and climb |
| Hind limbs | reduced sprint, launch impulse, grounded stability |
| Tail | reduced balance, yaw control, tail-attack authority |

Damage uses tissue layers: hide/scales, muscle, structural tissue, organ. Penetration and blunt trauma are distinct. Severing is rare and reserved for extreme authored outcomes.

### 9.5 Hit resolution

A hit is valid only when attack geometry overlaps a hittable body zone during an authored active window. Damage is calculated from:

- attack base profile;
- relative velocity and mass contribution;
- contact angle;
- body-part armor and vulnerability;
- attacker's damaged anatomy;
- target brace state;
- difficulty tuning clamps.

Animation notifies open and close attack windows. Root motion and physics impulses must agree; do not teleport attackers into range.

### 9.6 Defense and counters

- Brace a durable body region into the attack.
- Deflect with horn, wing edge, forelimb, or tail when anatomy allows.
- Break line of attack by changing altitude, bank, or terrain cover.
- Counter-grapple by attacking grip points or reversing leverage.
- Interrupt attacks with sufficient momentum, local trauma, or balance damage.
- A well-timed defense creates a short flow advantage, but never freezes the opponent into a guaranteed combo.

### 9.7 Grappling

Grapples are contact-based contests, not canned paired animations alone.

Minimum prototype variables:

- attacker and defender mass;
- number and integrity of engaged limbs/jaws;
- contact anchors;
- relative orientation;
- terrain support;
- stamina and grip;
- ongoing local damage.

The prototype may use authored pose-matching animations after contact is validated, but exits must support interruption, wall impact, cliff fall, wing deployment, and third-axis rotation.

## 10. Flight model

### 10.1 Design target

Flight should feel heavy, readable, and learnable rather than aeronautically exact. The player controls intent while the movement component coordinates pitch, bank, lift, and animation.

### 10.2 Required behaviors

- Ground run or leap into takeoff.
- Stall risk at low speed/high angle.
- Gliding with gradual energy loss.
- Diving converts altitude into speed.
- Climbing consumes speed and stamina.
- Banking is the primary high-speed turn mechanism.
- Wing damage creates asymmetric lift and control loss.
- Thermals and ridge lift can reduce stamina cost in selected arenas.
- Collision with terrain produces damage based on speed, angle, and body part.

### 10.3 Camera

Use a spring-arm third-person camera with dynamic distance and FOV. Pull back during high-speed flight and large grapples. Move closer during grounded exchanges. Avoid constant screen shake. Camera collision must not clip through wings or terrain.

## 11. AI model

Use a hierarchical **StateTree** for high-level state and a utility-scoring layer for tactical choice.

High-level states:

- Observe
- Threat display
- Ground engage
- Launch/reposition
- Air pursuit
- Attack run
- Grapple
- Recover
- Protect injury
- Retreat
- Desperation

Utility considerations:

- species instinct weights;
- age and experience;
- current body impairment;
- relative energy and heat;
- altitude and terrain;
- recent player actions;
- repeated-pattern confidence;
- escape route quality.

Assessment inputs, updated from observable evidence only:

- landed and missed attacks on both sides;
- visible impairment;
- altitude, closing speed, and terrain held;
- accrued cost in stamina, integrity, and surrendered position.

AI must not read hidden player input. Higher experience means better inference from the same observations — faster convergence and less self-assessment bias — never privileged information.

Difficulty scales judgement, not statistics. Easy over-pursues and disengages far too late; hard reads relative advantage and disengages after partial success; very hard shapes the space before committing. No tier may alter health, damage, or reaction speed.

## 12. Vertical slice

### 12.1 Encounter

**Arena:** Basalt Caldera  
**Player:** Prime Adult Drake, Seasoned experience  
**Opponent:** Prime Adult Wyvern, Veteran experience  
**Target duration:** 5–10 minutes

The arch-serpent and the forest arena are slice 2. Building three body plans
before flight and grapple are proven on two is how a prototype turns into a
content pipeline with no combat in it.

An **AI-vs-AI observer mode** is part of slice 1, not a later addition: both
fighters AI-driven, free camera, headless batch runs with telemetry. Player
skill masks system behaviour, and if the drake only reads as a drake when a
human drives it, the identity is in the player's head rather than the
simulation.

### 12.2 Arena features

- central open basin for ground combat;
- two elevated launch shelves;
- basalt pillars that break line of breath;
- one thermal column;
- cliff edge creating fall and forced-landing risk;
- no open-world streaming requirement.

### 12.3 Drake kit

- bite, granting a secured grip on a valid anchor;
- secured-bite rotational follow-up;
- left/right foreclaw;
- tail sweep;
- shoulder ram, denied below its entry-speed floor;
- short cone combustion breath;
- brace;
- grapple;
- quadrupedal takeoff, powered flight, glide, dive, forced landing.

### 12.4 Wyvern kit

- dive rake, requiring minimum entry speed and a locked line;
- wing buffet, trading damage for balance pressure;
- snap bite that grants no grip;
- tail stinger or blade — choose one during art validation;
- aerial disengage;
- ground panic defence;
- targeted wing attacks.

### 12.5 Slice success criteria

The slice is successful only when all are true:

1. The player can transition ground → air → ground without a cutscene.
2. Wing damage visibly and mechanically changes flight.
3. Low stamina slows the player and reduces attack recovery.
4. At least one grapple can begin from live collision and end in three different outcomes.
5. The Wyvern prefers aerial asymmetry and behaves poorly when trapped on the ground.
6. The player can win through at least three strategies: wing disable, stamina pressure, or direct lethal damage.
7. No required defense depends on invulnerability frames.
8. Combat remains understandable with debug overlays disabled.
9. A complete encounter maintains the target frame rate on the agreed test machine.
10. A non-developer can complete the fight after a short controls explanation.
11. The Wyvern disengages from an exchange it is losing, and re-engages when its estimate improves.
12. A blind observer watching AI-vs-AI names each morphotype from behaviour alone within three rounds.
13. Every falsification threshold in `Docs/TELEMETRY.md` is measurable and none is breached.

## 13. Unreal architecture

### 13.1 Implementation split

- **C++:** movement math, damage/body state, GAS foundations, performance-critical combat queries, save schema, deterministic utility functions.
- **Blueprints:** content assembly, animation graphs, montages, VFX/SFX binding, encounter scripting, tuning, UI.
- **Data Assets/JSON:** species, attacks, body profiles, life-stage modifiers, AI personality weights.

### 13.2 Unreal systems

- Gameplay Framework for GameMode, PlayerController, Pawn, and persistent state.
- Enhanced Input for all player actions.
- Gameplay Ability System for actions, resource costs, cooldowns, status tags, and effects.
- Gameplay Tags for state and capability gating.
- StateTree for hierarchical AI state.
- AI Perception and EQS for sensing and spatial choices where useful.
- Chaos collision/physics for impacts and environmental interaction.
- Niagara for breath, heat, dust, impact, and wingtip effects.
- Control Rig, IK Rig, and Motion Warping for grounded contact and grapple alignment.
- Primary Data Assets for authored definitions.

### 13.3 Key gameplay tags

```text
State.Grounded
State.Airborne
State.Grappled
State.Staggered
State.ForcedLanding
State.Exhausted
State.Overheated
State.Dead
State.BiteSecured
State.Anchored
State.Concealed
State.Displaying
Capability.Flight
Capability.Breath
Capability.Grapple
Damage.Wing.Left
Damage.Wing.Right
Damage.Jaw
Damage.Neck
Damage.Leg
Damage.Tail
Ability.Attack.Bite
Ability.Attack.Claw
Ability.Attack.Tail
Ability.Attack.Breath
Ability.Attack.Ram
Ability.Attack.Feint
Ability.Attack.SecuredTwist
Ability.Attack.DiveRake
Ability.Attack.WingBuffet
Ability.Attack.CoilDrag
Ability.Attack.NeckDispatch
Ability.Movement.Takeoff
Ability.Movement.Dive
Ability.Movement.AnchorPivot
Ability.Movement.Disengage
Ability.Defense.Brace
```

Every tag above is declared in `Config/DefaultEngine.ini`. `WarnOnInvalidTags`
is on, so the spec list and the config list must be changed together.

### 13.4 Save data

Persist:

- cycle number;
- defeated/spared lineages;
- equipped adaptations;
- life stage and experience;
- permanent injuries/scars where designed;
- combat tendency summary used for the next legendary dragon;
- accessibility and control settings.

Do not persist transient GAS effects or raw Actor references.

## 14. Art and Higgsfield pipeline

Higgsfield is used for concept exploration, mood, shot language, behavioral reference, and cinematic previs. It is not the final asset source.

Required concept deliverables per lineage:

1. neutral side/front/top anatomy sheet;
2. silhouette comparison at shared scale;
3. grounded locomotion key poses;
4. takeoff and landing sequence;
5. signature attack sequence;
6. injury-state references;
7. habitat and nesting behavior;
8. 5–10 second combat-previs clips with explicit camera notes.

Every prompt must include anatomy locks, scale reference, movement intent, environment, lens/camera, lighting, and negative constraints. See `Prompts/Higgsfield/`.

## 15. Audio direction

- Wingbeats emphasize displaced air and membrane tension, not generic explosions.
- Vocalization must reflect body size and respiratory load.
- Breath weapons have intake, ignition, sustained flow, and recovery phases.
- Localized injuries change breathing, footfalls, wing rhythm, and vocal timbre.
- Music supports phase intensity but must not conceal attack telegraphs.

## 16. UI and accessibility

Minimal HUD:

- health and stamina;
- heat/breath reserve;
- small body-status silhouette;
- target awareness indicator;
- optional altitude and airspeed indicators.

Accessibility:

- full input rebinding;
- hold/toggle options;
- camera shake slider;
- FOV control;
- color-independent damage states;
- subtitle and audio cue options;
- optional stronger attack-intent indicators;
- assisted flight mode without changing enemy damage.

## 17. Performance targets

Initial PC target:

- 60 FPS at 1080p on the agreed mid-range test machine in the vertical-slice arena;
- no more than two hero-quality dragons active;
- scalable Niagara and shadow settings;
- collision complexity separated from visual mesh;
- animation budget and LOD strategy defined before adding crowds or distant fauna.

Exact hardware tiers are a production decision and must be recorded in `Docs/DECISIONS.md`.

## 18. Testing strategy

### 18.0 Telemetry

Telemetry ships with the first combat loop, not in a polish phase. One combat
event feeds animation, VFX, audio, UI, AI, and analytics — a parallel
analytics path is forbidden because it will drift. Round summaries, terrain
occupancy, assessment traces, and the ten falsification thresholds are
specified in `Docs/TELEMETRY.md`.

### 18.1 Automated

- JSON schema validation, anatomy invariants, attack-to-morphotype cross-references, and wing-loading ordering.
- Unit tests for damage calculation, impairment thresholds, resource clamping, and utility scoring.
- Unreal automation tests for takeoff conditions, wing-disable effects, and ability-tag gating.

### 18.2 Functional

- repeatable encounter seeds;
- debug body-zone display;
- debug flight vectors: velocity, lift intent, stall margin;
- AI utility and current-state overlay;
- combat event log with attack, body part, damage layers, and impulse.

### 18.3 Playtest questions

- Did the fight feel like two animals contesting space and leverage?
- Did damage create visible tactical change?
- Could the player explain why they were hit?
- Did flight preserve weight and momentum?
- Did the Wyvern behave like a Wyvern rather than a reskinned Dragon?
- Did low stamina affect decisions before the bar reached zero?

## 19. Production gates

### Gate A — Greybox locomotion

Ground movement, takeoff, glide, powered flight, landing, basic camera, debug vectors.

### Gate B — Contact combat

Body zones, one attack per major anatomy group, brace, hit validation, stamina effects.

### Gate C — Damage consequences

Wing asymmetry, leg impairment, jaw/breath impairment, forced landing.

### Gate D — Live grappling

Contact validation, at least three outcomes, terrain interaction, interruption.

### Gate E — Species AI

Wyvern aerial preference, injury protection, retreat/re-engage, experience modifiers.

### Gate F — Presentation

Representative animation, VFX, audio, UI, accessibility, performance pass.

No gate may be skipped by replacing mechanics with a cinematic.

## 20. Definition of done for any task

A task is done when:

- implementation matches this specification or an approved decision record;
- code compiles without warnings introduced by the change;
- relevant automated tests pass;
- data validates;
- Blueprint-facing APIs have tooltips and sensible categories;
- debug visualization exists for non-obvious systems;
- no placeholder silently becomes an architectural dependency;
- documentation and `TASKS.md` are updated.

## 21. Open decisions

1. Final title and narrative setting.
2. Whether the hunter begins human in the shipped game or the transformation is backstory.
3. Exact aether-organ visual and narrative treatment.
4. Wyvern tail weapon: stinger versus cutting blade.
5. Flight assistance presets and default difficulty.
6. Target hardware tiers.
7. Whether the first release is encounter-based, roguelite, or a short authored campaign.
8. How much of the previous hero's behavior is procedural versus authored.

Until resolved, these decisions must not block the vertical slice.
