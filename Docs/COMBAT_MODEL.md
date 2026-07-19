# Combat model notes

## Core proposition

The combat system evaluates **capability**, **contact**, **commitment**, and **consequence**.

- Capability: can this anatomy currently perform the action?
- Contact: did an active attack surface touch a valid target zone?
- Commitment: how much momentum, stamina, and recovery does the action require?
- Consequence: what local and global state changes result?

## Suggested damage pipeline

1. Validate active attack window and attacker capability tags.
2. Find first meaningful contact and reject duplicate contacts for the same window.
3. Compute relative contact velocity.
4. Resolve blunt, cut, puncture, heat, and chemical components.
5. Apply armor/tissue response for the target body part.
6. Apply health, local integrity, balance, heat, and status effects.
7. Emit one combat event used by animation, VFX, audio, UI, AI, telemetry, and tests.

## Suggested normalized prototype equation

```text
EffectiveDamage = BaseDamage
                * VelocityFactor
                * AngleFactor
                * AttackerIntegrityFactor
                * TargetVulnerability
                * BraceFactor
```

Clamp every factor to authored ranges. The equation is a tuning model, not a claim of biological accuracy.

## Stamina performance curve

Do not wait until stamina reaches zero. Suggested normalized multipliers:

| Stamina | Acceleration | Attack recovery | Grip | Climb thrust |
|---:|---:|---:|---:|---:|
| 100% | 1.00 | 1.00 | 1.00 | 1.00 |
| 60% | 0.95 | 0.95 | 0.95 | 0.92 |
| 30% | 0.78 | 0.75 | 0.72 | 0.65 |
| 10% | 0.55 | 0.50 | 0.45 | 0.30 |
| 0% | 0.35 | 0.35 | 0.25 | 0.00 |

Interpolate with a curve asset, not hard-coded tiers.

## Flow advantage

A successful brace, deflection, or position escape may grant a short advantage in recovery and aim authority. It must not stun-lock the opponent or create an automatic combo. The opponent remains able to choose a costly emergency response.

## Grapple prototype simplification

For the first slice, support a fixed set of anatomical anchors:

- jaw to neck/wing root;
- foreclaw to shoulder/torso;
- hind claw to flank;
- body pin against ground or wall.

Use continuous contest values, then select an authored pose family that matches the validated anchor pair and relative orientation.

## Assessment and disengagement

*Sourced (68–85).* This rests on a fifty-year theoretical and empirical
literature, and it is now the best-supported part of the design.

Contest theory is consistent on one point: outcomes depend on assessment,
motivation and accumulated cost, not solely on raw fighting ability. Animals
assess and withdraw when persistence is no longer worth what is being
contested (70, 69). A dragon should not ask "can I kill this?" but "is this
exchange worth the energy, the wound risk, and the position I give up?"

### The withdrawal rule: cumulative assessment

The design implements the **cumulative assessment model** (75): withdrawal is
driven by accumulated cost — both self-inflicted through effort and
opponent-inflicted through damage — measured against a threshold set by the
subjective value of what is contested (74, 78).

This choice is deliberate and evidence-led. A meta-analysis across species
found stronger support for **self**-assessment and cumulative assessment than
for full mutual assessment (79), and the standard method for inferring mutual
assessment — regressing contest duration on winner and loser ability —
generates false positives (80). So opponent-reading is a *modifier* here, not
the engine. An earlier draft of this design had it the other way around.

The competing models are named so the implementation can say which one it is:
sequential assessment, where each side estimates relative strength and the
weaker quits (73); the energetic war of attrition, pure self-assessment with
no opponent-reading at all (76); and the asymmetric war of attrition (72).

### State

Every combatant carries an `FDragonAssessment`:

- **AccruedCost** — the primary driver. Stamina spent, integrity lost, position surrendered.
- **CostThreshold** — derived from `ResourceValue` and `Persistence`. Cross it and disengage.
- **SelfCapability** — estimate of own fighting ability, from observable state.
- **SelfBias** — systematic error in that estimate. Naive individuals skew positive.
- **OpponentCapability** — estimate of the opponent's ability, from observed behaviour.
- **MutualAssessmentWeight** — how much `OpponentCapability` is allowed to move the threshold. Low by default, rising with experience.
- **Skill** — accuracy, tactic selection and decision speed. Distinct from raw ability (81).

Physiological cost thresholds are real: attacking hermit crabs quit when
lactate is high, and defenders' decisions track available energy reserves
(83). Accrued cost is not an abstraction bolted onto the fight — it is the
fight.

### Experience and the skill axis

Skill is a component of fighting ability distinct from RHP — accuracy,
choosing the right tactic, deciding fast — and it is acquired through
play-fighting and real contests (81). That is the literature basis for the
project's central AI claim: a better opponent fights *better*, not bigger.

Prior outcomes feed back. Winner and loser effects modulate future contest
decisions by updating self-assessment, accumulating like a leaky integrator
(82). A wyvern that has just been thrown should estimate itself lower for a
while, and recover that estimate over time rather than instantly.

That the judgement axis is genuinely separable from physical ability is not an
assumption. An artificial-selection experiment changed loser-effect duration
while leaving actual fighting success unchanged (104) — the experience axis
moved and the ability axis did not. And contest skill is measurable as graded
individual variation: accuracy and precision of strikes predict who wins hermit
crab contests (103). A difficulty dial that turns judgement without touching
statistics is doing something the literature says is real.

Estimates update from **observable** evidence only: landed and missed attacks,
visible impairment, altitude, closing speed, terrain held. The AI never reads
hidden player input. Higher experience means better inference from the same
observations — never privileged information.

### Difficulty scales judgement, not statistics

| Tier | Behaviour |
|---|---|
| Easy | poor cost tracking, over-pursues, wastes stamina, disengages far too late |
| Medium | tracks its own cost accurately, understands favourable spacing and terrain |
| Hard | adds opponent-reading, uses displays to manipulate distance, disengages after partial success |
| Very hard | high skill: shapes the space before committing at all |

Note the ordering. Self-assessment and cost tracking come *first*, and
opponent-reading is what higher tiers add — which follows the evidence (79)
rather than the intuition that a smart opponent primarily reads you.

A harder opponent quits bad fights **earlier**. It does not gain health,
damage, or reaction speed. If a difficulty tier needs a stat multiplier to
feel harder, the assessment model is not doing its job.

This mapping is a design translation and is labelled *reasoned*: no study
frames contest skill as a tunable difficulty axis. It is well grounded, but
the axis is ours.

## Displays

*Sourced (84, 85).* Threat display is a mechanic, not an animation. Agonistic
displays let opponents assess each other and settle contests without escalating
to costly fighting (84).

One nuance worth getting right: displays are better understood as honest
signals of **motivation** than of fighting ability (85). A display should
therefore say "I am willing to pay for this," not "I am strong" — which maps
onto `ResourceValue` and `Persistence` rather than onto `SelfCapability`. A
weaker animal that badly wants the ground can honestly signal that, and
sometimes win by it.

A display costs stamina, projects an intimidation radius scaled by size,
condition, and experience, and raises the opponent's estimate of what the
fight will cost. Ancients rely on it because their burst is gone.

A display that never changes an opponent's decision is decoration. If
telemetry shows displays have no effect on disengage timing, cut them.

## Line-break concealment

Dragons are enormous, so stealth cannot mean invisibility. It means
managing what the opponent can currently resolve:

- breaking line of sight behind pillars, ridges, and canopy;
- reducing silhouette by folding wings and lowering the body;
- moving during the opponent's committed recovery;
- withdrawing along terrain that denies a clean approach line.

Detection differs by morphotype — aerial predators track moving silhouettes
well, ground and serpentine predators read close-range disturbance better.
The longneck's around-cover strike exists because of this: it threatens
without exposing its whole body.

Concealment must never grant damage immunity or a guaranteed opener. It buys
position and forces a worse assessment on the opponent.

## Secured-bite conversion

A clean bite on a valid anchor grants `State.BiteSecured` rather than only
subtracting health. From there:

- the attacker may commit to a rotational follow-up, which is its damage
  ceiling and its longest punish window;
- the defender retains a costly emergency response — brace, wing deployment,
  local trauma to the attacker's jaw or neck, or terrain leverage;
- the grip breaks on sufficient local damage, balance loss, or terrain impact.

Effort is modulated, not scripted. Constricting snakes adjust their squeeze in
response to the prey's heartbeat rather than holding a fixed interval (28), and
the same principle applies here: a hold should track the defender's remaining
resistance and ease off or intensify accordingly. A fixed-duration authored
grapple is the thing this design exists to avoid.

Only morphotypes with the mass and limb topology to apply leverage may
convert. The wyvern bites and releases; that is the point of the wyvern.
