# Research basis and traceability

The kit was rebased on the dragon species research in July 2026. This
document records what changed, what was deliberately not adopted, and what
still needs work before the research can serve as a citable project
reference.

## Adopted

| Research finding | Change |
|---|---|
| Prototype three morphotypes, not a bestiary | Roster cut from six families to drake / wyvern / longneck. `Data/Dragons/` reduced from six files to three. |
| Wyvern, lindworm, amphiptere are heraldic descriptors, not biological categories | "Family" replaced with "morphotype" throughout. `DRAGON_TAXONOMY.md` rewritten to treat names as body-plan tags with a mythic-identity layer kept separate. |
| Long-neck is a derived game morphotype, not a canonical myth species | Named "arch-serpent" in all production docs. `neck_segments` added as a real driver of reach and inertia. |
| Wing loading trades speed for agility | `flight_envelope` block added to the schema; ordering enforced by the validator. |
| Large fliers need forceful, anatomy-specific launch | `launch_mode` added; takeoff is a gated state with a vulnerability window. |
| Bite force scales with size | Heavy morphotypes get damage on the confirm, not through combo length. |
| Rotational tearing on a secured bite | New `State.BiteSecured` and `drake_secured_twist`. Wyvern explicitly cannot convert a bite. |
| Neck leverage buys reach and costs inertia | Neck is a required damageable part for every morphotype; longneck's neck has high integrity and low armour. |
| High-speed interception is set up, not steered | `wyvern_dive_rake` has a minimum entry speed, requires `diving`, and cannot re-aim mid-stoop. |
| Age needs a curve, and condition is a third axis | `condition` field added. Aging table in `BIOMECHANICS.md` §8. |
| Contest theory: RHP, resource value, willingness to continue | `FDragonAssessment` in C++; five new `ai_weights` fields. Difficulty now scales judgement, not stats. |
| Terrain belongs in the decision model | `terrain_affinity` per morphotype; forest map added for slice 2. |
| Metrics layer is day-one, not polish | `TELEMETRY.md`, with ten falsification thresholds. |
| AI-vs-AI observer mode | Added as milestone M5.7, before the third morphotype. |
| Higgsfield is previz and concept only | Already ADR-004. Unchanged, now corroborated. |

## Not adopted

**Behaviour Trees alongside StateTree.** The research recommends StateTree
for macro phases plus Behaviour Trees and Blackboard for local tactics. The
kit keeps StateTree plus a utility-scoring layer (ADR-003, reaffirmed as
ADR-007).

Reasoning: the research's real contribution here is the *decision inputs* —
assessment, resource value, persistence, terrain — and those are
container-agnostic. Running two AI authoring systems means two debugging
surfaces, two places tactical state can live, and a standing question of
which one owns a given decision. One container with good inputs beats two
containers with the same inputs. Revisit if utility scoring produces
tactical behaviour that is hard to author or read in the editor.

**Two maps in the first slice.** The forest map is real added scope. It is
scheduled with the longneck in slice 2, because forest is the longneck's
home terrain and a second map with nothing to exploit it proves nothing.
Slice 1 stays in the caldera.

**Three species simultaneously.** The research recommends three; the kit
builds them in sequence. Drake vs wyvern first (ADR-001 stands), longneck
third. Sequencing is not disagreement — it is the same roster with a
dependency order, so that flight and grapple are proven on two bodies before
a third body plan stresses them.

**The 12-week timeline as stated.** See `PROTOTYPE_PLAN.md`. The phase
*order* is adopted. The duration assumes a staffed team and is not
achievable solo.

## Citations

Closed over two passes. `RESEARCH_BIBLIOGRAPHY.md` holds 104 sources mapped
constraint by constraint, every rule carries an evidence label, and three
flagged records were verified against CrossRef and publisher pages — one of
which turned out to be missing a co-author.

Going to the sources changed the design twice, which is the argument for
doing it at all:

**ADR-011.** The assessment model had opponent-reading and self-assessment
weighted about equally. The literature says self and cumulative assessment are
better supported across species, and that the standard method for inferring
mutual assessment produces false positives. The withdrawal rule became a cost
comparison, and opponent-reading became experience-gated.

**ADR-013.** Sourcing the neck constraint surfaced that necks divide into
rapid-strike and puncture-and-pull architectures, and that saber-tooth dispatch
was neck-powered and only worked on restrained prey. The arch-serpent's weak
jaw stopped being a balance problem and became its identity: it restrains, then
dispatches, reaching the same ceiling as the drake by an incompatible route.

Three claims are not evidence and are labelled as such. Two are inference from
measured constraints. The third — reptile performance senescence — is a hole
in the literature rather than a weakness in the design, and is marked
*unstudied* to keep that distinction visible.

Nothing in this rebase depends on a claim that looked doubtful. The
constraints adopted above are the ones that were both well-supported and
load-bearing.
