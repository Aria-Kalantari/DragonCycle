"""Verify imported species assets against the authoring JSON (ADR-006).

Independent of the importer's own logging: loads each generated asset, reads
the properties back out, and compares them to Data/Dragons/*.json. Confirms
the 11-part closed set round-trips intact rather than trusting the log line.

    UnrealEditor-Cmd.exe DragonCycle.uproject -run=pythonscript \
        -script="Tools/verify_species_import.py"
"""

import json
import os

import unreal

LOG = "[M06-verify] "
DATA_DIR = "/Game/DragonCycle/Data"
JSON_DIR = os.path.join(unreal.Paths.project_dir(), "Data", "Dragons")

# Built by normalization, not hard-coded: Python exposes HindLimbLeft as
# HIND_LIMB_LEFT, and spelling that out per part is the drift ADR-006 forbids.
PART_ENUM = {}
for _name in dir(unreal.DragonBodyPart):
    if _name.startswith("_"):
        continue
    _value = getattr(unreal.DragonBodyPart, _name)
    if isinstance(_value, unreal.DragonBodyPart):
        PART_ENUM[_name.replace("_", "").lower()] = _value

failures = []


def check(condition, message):
    if not condition:
        failures.append(message)
        unreal.log_error(LOG + message)


def close(a, b):
    return abs(float(a) - float(b)) < 0.01


for file_name in sorted(os.listdir(JSON_DIR)):
    if not file_name.endswith(".json"):
        continue
    with open(os.path.join(JSON_DIR, file_name), "r", encoding="utf-8") as handle:
        src = json.load(handle)

    morph = "".join(p.capitalize() for p in src["morphotype"].split("_"))
    stage = "".join(p.capitalize() for p in src["life_stage"].split("_"))
    path = "%s/DA_%s_%s" % (DATA_DIR, morph, stage)

    asset = unreal.EditorAssetLibrary.load_asset(path)
    if asset is None:
        check(False, "%s: missing generated asset %s" % (file_name, path))
        continue

    check(str(asset.get_editor_property("species_id")) == src["id"],
          "%s: species_id mismatch" % file_name)
    check(close(asset.get_editor_property("mass_kg"), src["flight_envelope"]["mass_kg"]),
          "%s: mass_kg mismatch" % file_name)
    check(close(asset.get_editor_property("wing_area_m2"), src["flight_envelope"]["wing_area_m2"]),
          "%s: wing_area_m2 mismatch" % file_name)
    check(close(asset.get_editor_property("stall_speed_meters_per_second"),
                src["flight_envelope"]["stall_speed_ms"]),
          "%s: stall_speed mismatch" % file_name)
    check(asset.get_editor_property("neck_segments") == src["body_plan"]["neck_segments"],
          "%s: neck_segments mismatch" % file_name)
    check(close(asset.get_editor_property("condition"), src.get("condition", 1.0)),
          "%s: condition mismatch" % file_name)

    assessment = asset.get_editor_property("base_assessment")
    weights = src["ai_weights"]
    for prop, key in [("self_bias", "rhp_self_bias"), ("resource_value", "resource_value"),
                      ("persistence", "persistence"), ("skill", "skill"),
                      ("mutual_assessment_weight", "mutual_assessment_weight")]:
        check(close(assessment.get_editor_property(prop), weights[key]),
              "%s: assessment.%s mismatch" % (file_name, prop))

    # The closed set: same parts, same count, same order-independent values.
    imported = {}
    for profile in asset.get_editor_property("body_parts"):
        imported[profile.get_editor_property("part")] = profile
    check(len(imported) == len(src["body_parts"]),
          "%s: part count %d != %d" % (file_name, len(imported), len(src["body_parts"])))
    for part in src["body_parts"]:
        enum_value = PART_ENUM.get(part["id"].replace("_", "").lower())
        if enum_value is None:
            check(False, "%s: part id %s has no EDragonBodyPart entry" % (file_name, part["id"]))
            continue
        profile = imported.get(enum_value)
        if profile is None:
            check(False, "%s: part %s missing from asset" % (file_name, part["id"]))
            continue
        check(close(profile.get_editor_property("max_integrity"), part["max_integrity"]),
              "%s/%s: max_integrity mismatch" % (file_name, part["id"]))
        check(close(profile.get_editor_property("armor"), part["armor"]),
              "%s/%s: armor mismatch" % (file_name, part["id"]))
        check(close(profile.get_editor_property("impairment_threshold"), part["break_threshold"]),
              "%s/%s: break_threshold mismatch" % (file_name, part["id"]))

    unreal.log(LOG + "%s round-trips: %d parts, %.1f kg/m2 wing loading"
               % (path.split("/")[-1], len(imported), asset.get_wing_loading()))

if failures:
    unreal.log_error(LOG + "VERIFY FAILED: %d mismatch(es)" % len(failures))
else:
    unreal.log(LOG + "VERIFY OK")
