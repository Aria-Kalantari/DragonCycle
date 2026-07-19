#!/usr/bin/env python3
"""Validate DragonCycle JSON data and project-specific invariants.

Failure mode contract: this script never raises on malformed data. Every
problem is reported as a line on stderr and the process exits 1. If it
tracebacks, that is a bug in the validator, not in the data.
"""

from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any

try:
    import jsonschema
except ImportError as exc:  # pragma: no cover
    raise SystemExit("Install dev requirements: python -m pip install -r requirements-dev.txt") from exc

ROOT = Path(__file__).resolve().parents[1]
SCHEMAS = ROOT / "Data" / "Schemas"
DRAGONS = ROOT / "Data" / "Dragons"

# Canonical plans keyed by morphotype: (legs, independent_wings, wing_arms).
CANONICAL_PLAN = {
    "drake": (4, 2, 0),
    "wyvern": (2, 0, 2),
    "longneck": (0, 2, 0),
}

# Opponent-reading is learned. A naive individual that reads its opponent well
# is a difficulty cheat wearing a data file, so the ceiling rises with rank.
MUTUAL_ASSESSMENT_CEILING = {
    "naive": 0.20,
    "seasoned": 0.45,
    "veteran": 0.65,
    "master": 0.85,
    "legendary": 1.00,
}

PAIRED_PARTS = {
    "wings": ("wing_left", "wing_right"),
    "forelimbs": ("forelimb_left", "forelimb_right"),
    "hindlimbs": ("hindlimb_left", "hindlimb_right"),
}


def load_json(path: Path) -> tuple[Any, list[str]]:
    """Return (data, errors). Never raises for malformed or missing files."""
    try:
        with path.open("r", encoding="utf-8") as handle:
            return json.load(handle), []
    except FileNotFoundError:
        return None, [f"{path.name}: file not found"]
    except json.JSONDecodeError as exc:
        return None, [f"{path.name}: invalid JSON at line {exc.lineno}, column {exc.colno}: {exc.msg}"]


def validate_schema(instance: Any, schema: Any, label: str) -> list[str]:
    validator = jsonschema.Draft202012Validator(schema)
    errors = []
    for error in sorted(validator.iter_errors(instance), key=lambda item: list(item.path)):
        location = ".".join(str(part) for part in error.path) or "<root>"
        errors.append(f"{label}:{location}: {error.message}")
    return errors


def validate_species_invariants(dragon: dict[str, Any], label: str) -> list[str]:
    """Only called on species that already passed schema validation."""
    errors: list[str] = []
    morphotype = dragon["morphotype"]
    plan = dragon["body_plan"]
    locomotion = set(dragon["locomotion"])
    part_ids = [part["id"] for part in dragon["body_parts"]]
    parts = set(part_ids)

    actual = (plan["legs"], plan["independent_wings"], plan["wing_arms"])
    if actual != CANONICAL_PLAN[morphotype]:
        errors.append(f"{label}: body plan {actual} does not match canonical {morphotype} plan {CANONICAL_PLAN[morphotype]}")

    if len(part_ids) != len(parts):
        errors.append(f"{label}: duplicate body part ids")

    # Anatomy and the part list must describe the same animal.
    can_fly = plan["independent_wings"] + plan["wing_arms"] > 0
    if can_fly != bool({"powered_flight", "glide"} & locomotion):
        errors.append(f"{label}: wing anatomy and flight locomotion disagree")
    if can_fly != (set(PAIRED_PARTS["wings"]) <= parts):
        errors.append(f"{label}: wing anatomy and wing body parts disagree")

    # A wyvern's wings are its forelimbs. It must not also carry forelimb parts.
    if plan["wing_arms"] > 0 and set(PAIRED_PARTS["forelimbs"]) & parts:
        errors.append(f"{label}: wing_arms morphotype must not declare separate forelimb parts")
    if plan["legs"] == 0 and set(PAIRED_PARTS["hindlimbs"]) & parts:
        errors.append(f"{label}: legless morphotype must not declare hindlimb parts")
    if plan["legs"] >= 2 and not set(PAIRED_PARTS["hindlimbs"]) <= parts:
        errors.append(f"{label}: legged morphotype must declare both hindlimb parts")

    for group, (left, right) in PAIRED_PARTS.items():
        if (left in parts) != (right in parts):
            errors.append(f"{label}: {group} must be declared as a symmetric pair")

    # Neck is load-bearing for reach and breath arc, so it is never optional.
    if "neck" not in parts:
        errors.append(f"{label}: neck is a required damageable part for every morphotype")

    if plan["serpentine"] and plan["neck_segments"] < 5:
        errors.append(f"{label}: serpentine morphotype needs neck_segments >= 5 to justify off-axis reach")

    weights = dragon["ai_weights"]
    ceiling = MUTUAL_ASSESSMENT_CEILING[dragon["experience"]]
    if weights["mutual_assessment_weight"] > ceiling:
        errors.append(
            f"{label}: mutual_assessment_weight {weights['mutual_assessment_weight']} "
            f"exceeds the {dragon['experience']} ceiling of {ceiling}"
        )

    envelope = dragon["flight_envelope"]
    if can_fly and envelope["wing_area_m2"] <= 0:
        errors.append(f"{label}: flying morphotype needs positive wing_area_m2")
    if can_fly and envelope["launch_mode"] == "none":
        errors.append(f"{label}: flying morphotype needs a launch_mode")

    return errors


def validate_wing_loading_order(species: list[tuple[str, dict[str, Any]]]) -> list[str]:
    """Wing loading buys speed and costs agility. Enforce that across the roster.

    This is the one biomechanical claim the design rests on, so it is checked
    rather than trusted: no morphotype may be both heavier-loaded and more
    agile than another.
    """
    errors: list[str] = []
    loaded = []
    for label, dragon in species:
        env = dragon["flight_envelope"]
        area = env["wing_area_m2"]
        if area <= 0:
            continue
        loaded.append((label, env["mass_kg"] / area, env["max_turn_rate_deg_s"]))

    for index, (label_a, load_a, turn_a) in enumerate(loaded):
        for label_b, load_b, turn_b in loaded[index + 1:]:
            heavier = (label_a, load_a, turn_a) if load_a > load_b else (label_b, load_b, turn_b)
            lighter = (label_b, load_b, turn_b) if load_a > load_b else (label_a, load_a, turn_a)
            if heavier[1] > lighter[1] and heavier[2] > lighter[2]:
                errors.append(
                    f"{heavier[0]}: wing loading {heavier[1]:.1f} kg/m2 exceeds {lighter[0]} "
                    f"({lighter[1]:.1f}) but also turns faster ({heavier[2]} vs {lighter[2]} deg/s)"
                )
    return errors


def validate_attack_cross_references(
    attacks: list[dict[str, Any]], parts_by_morphotype: dict[str, set[str]]
) -> list[str]:
    """The check the previous validator advertised and did not perform."""
    errors: list[str] = []
    granted: dict[str, set[str]] = {}

    for index, attack in enumerate(attacks):
        morphotype = attack.get("morphotype")
        if morphotype not in parts_by_morphotype:
            errors.append(f"attacks.json[{index}] ({attack.get('id')}): no species defines morphotype {morphotype!r}")
            continue
        granted.setdefault(morphotype, set()).update(attack.get("grants_state", []))

        available = parts_by_morphotype[morphotype]
        missing = [part for part in attack.get("required_parts", []) if part not in available]
        if missing:
            errors.append(
                f"attacks.json[{index}] ({attack.get('id')}): requires {missing} "
                f"which no {morphotype} declares"
            )

    # A follow-up whose entry state nothing grants is dead content.
    for index, attack in enumerate(attacks):
        morphotype = attack.get("morphotype")
        if morphotype not in parts_by_morphotype:
            continue
        for state in attack.get("requires_state", []):
            if state in {"bite_secured", "anchored"} and state not in granted.get(morphotype, set()):
                errors.append(
                    f"attacks.json[{index}] ({attack.get('id')}): requires state {state!r} "
                    f"that no {morphotype} attack grants"
                )

    for morphotype in sorted(parts_by_morphotype):
        if not any(attack.get("morphotype") == morphotype for attack in attacks):
            errors.append(f"attacks.json: morphotype {morphotype!r} has no attacks defined")

    return errors


def main() -> int:
    errors: list[str] = []

    species_schema, schema_errors = load_json(SCHEMAS / "dragon_species.schema.json")
    errors.extend(schema_errors)
    attack_schema, attack_schema_errors = load_json(SCHEMAS / "attack.schema.json")
    errors.extend(attack_schema_errors)
    if species_schema is None or attack_schema is None:
        print("Validation failed:", file=sys.stderr)
        for error in errors:
            print(f"- {error}", file=sys.stderr)
        return 1

    seen_ids: set[str] = set()
    parts_by_morphotype: dict[str, set[str]] = {}
    valid_species: list[tuple[str, dict[str, Any]]] = []

    for path in sorted(DRAGONS.glob("*.json")):
        dragon, load_errors = load_json(path)
        if load_errors:
            errors.extend(load_errors)
            continue

        schema_problems = validate_schema(dragon, species_schema, path.name)
        errors.extend(schema_problems)
        if schema_problems:
            # Structure is unknown past this point; invariant checks would be guesswork.
            continue

        if dragon["id"] in seen_ids:
            errors.append(f"{path.name}: duplicate species id {dragon['id']}")
        seen_ids.add(dragon["id"])

        errors.extend(validate_species_invariants(dragon, path.name))
        valid_species.append((path.name, dragon))
        parts_by_morphotype.setdefault(dragon["morphotype"], set()).update(
            part["id"] for part in dragon["body_parts"]
        )

    errors.extend(validate_wing_loading_order(valid_species))

    attacks, load_errors = load_json(ROOT / "Data" / "attacks.json")
    errors.extend(load_errors)
    attack_count = 0
    if attacks is not None:
        if not isinstance(attacks, list):
            errors.append("attacks.json: root must be an array")
        else:
            attack_count = len(attacks)
            attack_ids: set[str] = set()
            well_formed: list[dict[str, Any]] = []
            for index, attack in enumerate(attacks):
                label = f"attacks.json[{index}]"
                schema_problems = validate_schema(attack, attack_schema, label)
                errors.extend(schema_problems)
                if schema_problems:
                    continue
                if attack["id"] in attack_ids:
                    errors.append(f"{label}: duplicate attack id {attack['id']}")
                attack_ids.add(attack["id"])
                well_formed.append(attack)
            errors.extend(validate_attack_cross_references(well_formed, parts_by_morphotype))

    if errors:
        print("Validation failed:", file=sys.stderr)
        for error in errors:
            print(f"- {error}", file=sys.stderr)
        return 1

    print(f"Validated {len(seen_ids)} morphotypes and {attack_count} attacks.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
