"""M0.6 follow-up: point BP_Drake_Player.SpeciesData at the imported drake asset.

Run AFTER the DragonSpeciesImport commandlet has generated the data assets:

    UnrealEditor-Cmd.exe DragonCycle.uproject -run=pythonscript \
        -script="Tools/editor_bootstrap_m06.py"

Kept out of the C++ importer on purpose: the importer is generic over the
schema, while this is project assembly - one named Blueprint gets one named
asset. Idempotent; reassigning the same reference is a no-op.

Uses -run=pythonscript (no level editor needed for CDO edits), which folds
logged errors into the process exit code, so failure here is visible to the
caller without parsing the log.
"""

import unreal

BP_PATH = "/Game/DragonCycle/Blueprints/BP_Drake_Player"
DA_PATH = "/Game/DragonCycle/Data/DA_Drake_PrimeAdult"
LOG = "[M06-wire] "

eal = unreal.EditorAssetLibrary
failed = False


def fail(msg):
    global failed
    failed = True
    unreal.log_error(LOG + msg)


species = eal.load_asset(DA_PATH)
blueprint = eal.load_asset(BP_PATH)
if species is None:
    fail("missing " + DA_PATH + " - run the DragonSpeciesImport commandlet first")
if blueprint is None:
    fail("missing " + BP_PATH + " - run Tools/editor_bootstrap_m03.py first")

if not failed:
    generated_class = unreal.load_object(None, BP_PATH + ".BP_Drake_Player_C")
    if generated_class is None:
        fail("could not load BP_Drake_Player generated class")
    else:
        cdo = unreal.get_default_object(generated_class)
        cdo.set_editor_property("species_data", species)
        if not eal.save_loaded_asset(blueprint):
            fail("could not save " + BP_PATH)

if not failed:
    verify = unreal.get_default_object(unreal.load_object(None, BP_PATH + ".BP_Drake_Player_C"))
    assigned = verify.get_editor_property("species_data")
    if assigned is None or assigned.get_path_name().split(".")[0] != DA_PATH:
        fail("SpeciesData did not stick; found %s" % assigned)
    else:
        unreal.log(LOG + "BP_Drake_Player.SpeciesData = %s (species id %s)"
                   % (DA_PATH, assigned.get_editor_property("species_id")))

if failed:
    unreal.log_error(LOG + "M06 WIRING FAILED")
else:
    unreal.log(LOG + "M06 WIRING OK")
