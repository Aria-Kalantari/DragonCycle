# Morphotypes and combat identity

This document expands the roster in `SPEC.md` into content-design rules.
Read `BIOMECHANICS.md` first — it defines the constraints these identities
are built inside.

## Naming discipline

Fantasy species names carry two separate loads, and mixing them causes
trouble. Names such as *wyvern*, *lindworm*, and *amphiptere* come from
heraldic and antiquarian description; they are body-plan tags, not stable
pan-European biological categories. Beings such as the East Asian long,
nāga, Yamata no Orochi, mušḫuššu, Quetzalcoatl, taniwha, and the Rainbow
Serpent are embedded in living religious and cultural systems and were never
species sheets at all.

So the project separates them:

- **Morphotype** is the engineering unit. Three of them. Everything the
  simulation touches — anatomy, launch mode, flight envelope, attack set,
  body parts — hangs off the morphotype.
- **Mythic identity** is the content layer. A morphotype can be dressed as
  many lineages without changing a line of simulation code.

This buys accuracy and safety at once: it avoids flattening living
traditions into stat blocks, and it stops the roster inflating every time
somebody wants a new visual identity.

The long-neck body plan has no canonical mythic source. It is a derived
morphotype assembled from serpentine length, winged-serpent forms, and
long-bodied sky creatures. Call it the **arch-serpent** in production
documents. Do not name it after a modern fictional dragon.

## Shared design rule

A morphotype is not a skin. It must differ in at least four of:

- locomotion topology;
- launch mode and takeoff requirements;
- attack reach and active surfaces;
- grapple anchors and leverage;
- defence posture;
- resource economy;
- preferred terrain;
- assessment profile and disengage threshold;
- body-part consequences;
- silhouette and animation rhythm.

The three below differ in all ten. That is the bar.

## The roster

| | Drake | Wyvern | Arch-serpent (longneck) |
|---|---|---|---|
| Body plan | 4 legs + 2 wings | 2 legs, wings are forelimbs | legless, 2 forelimbs, reduced wings |
| Launch | quadrupedal | bipedal | coil spring |
| Wing loading | highest | lowest | middle |
| Neck segments | 3 | 3 | 9 |
| Ground identity | stable grappler | unstable, evasive | anchored range control |
| Air identity | committed generalist | interception predator | glide duelist |
| Wins by | secured bite into rotation | altitude, angle, attrition | reach, displacement, terrain |
| Loses to | sustained aerial harassment | being trapped on the ground | jammed neck base, square collision |
| Primary failure | wing area and heat | missed stoop recovery | lateral instability |
| Home terrain | caldera | caldera ridges | forest |

## Move ecology

Identity comes from move ecology, not stat spreads.

**Drake** — short committed flight, crushing bite, secured-bite rotation,
shoulder ram with a real entry-speed floor, tail sweep, cone breath, brace.
Low feint frequency, high commitment damage, best grapple leverage in the
roster.

**Wyvern** — rapid launch, stoop with a locked line, wing buffet for balance
pressure rather than damage, snap bite that grants no grip, disengage climb.
High initiative, high miss-recovery vulnerability, no clinch game.

**Arch-serpent** — off-centreline bite, neck feint that costs almost nothing
and threatens a commit, terrain anchor into coil drag, around-cover strike,
tail-anchor pivot, neck-powered dispatch on a restrained target. Strong
positional control, poor crush resistance in a square body collision.

The two heavy morphotypes reach their damage ceiling by opposite routes, and
this is the roster's sharpest contrast. Theropod necks divide into rapid-strike
and puncture-and-pull architectures (87), so the drake gets the second and the
arch-serpent the first. The drake secures a bite and rotates: force from body
mass. The arch-serpent restrains, then dispatches with cervical musculature —
the saber-tooth model, where a jaw at roughly a third of the expected force
still kills because the neck supplies the rest and the prey cannot struggle
(90, 91, 92). Same ceiling, incompatible preconditions.

## Reserve archetypes

Not in the prototype. Kept here so the roster can grow without redesign.
Each maps to an existing morphotype plus a variation, not a new body plan.

| Archetype | Maps to | Variation |
|---|---|---|
| Treasure-guarding wyrm | drake | den-fighter, venom, reduced flight |
| Multi-headed marsh serpent | longneck | multiple neck chains, area denial, boss-scale |
| Feathered serpent | longneck | precision aerial zoning, lower mass |
| Water-linked river dragon | longneck | swim locomotion, surface-disturbance sensing |
| Composite guardian | drake | prestige elite, unusual gait |
| Burrowing hook-limbed drake | longneck | burrow locomotion, forelimb anchoring |

Adding one is a content task, not an engineering task. If it needs a new
body plan, it needs an ADR first.

## Adaptation examples

Adaptations must carry visible and mechanical cost.

- Reinforced wing finger: higher dive tolerance; more wing inertia.
- Air sacs: better climb efficiency; larger vulnerable torso volume.
- Heat pits: stronger close-range sensing; sensory overload near fire.
- Extended neck chain: more reach and angle; slower retraction, longer exposed line.
- Anchor claws: stronger terrain holds; reduced walking efficiency.
- Clotting lattice: reduced bleed; high heat and food demand.

## Forbidden shortcuts

- One skeleton and animation set for every morphotype.
- Identity expressed only through elemental damage.
- Stat-only differences such as +10% speed.
- A legless morphotype hovering through magic.
- A wyvern that can convert a bite into a grapple.
- New mythic lineage implemented as a fourth engineering body plan.
