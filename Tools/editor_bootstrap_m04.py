"""M0.4 editor bootstrap: Enhanced Input assets from Docs/UNREAL_EDITOR_CHECKLIST.md.

Run headless:

    UnrealEditor-Cmd.exe DragonCycle.uproject \
        -ExecutePythonScript="Tools/editor_bootstrap_m04.py" -nullrhi

Idempotent: existing input actions are updated in place, and IMC_Dragon's
mapping list is rebuilt from scratch on every run so reruns converge.

Bindings follow SPEC 9.2. Triggers (hold vs press) are deliberately left
default; M1 ability wiring decides those semantics per action.
"""

import unreal

LOG = "[M04] "
INPUT_DIR = "/Game/DragonCycle/Input"

asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
eal = unreal.EditorAssetLibrary
errors = []


def log(msg):
    unreal.log(LOG + msg)


def fail(msg):
    errors.append(msg)
    unreal.log_error(LOG + msg)


AXIS2D = unreal.InputActionValueType.AXIS2D
BOOL = unreal.InputActionValueType.BOOLEAN

# name, value type, tooltip (SPEC 9.2 ground / air meaning)
ACTIONS = [
    ("IA_Move", AXIS2D, "Locomotion and strafe/turn intent. Air: pitch/yaw intent with assisted coordination."),
    ("IA_Look", AXIS2D, "Camera plus head/attack aim. Air: camera plus flight/look intent."),
    ("IA_PrimaryAttack", BOOL, "Context primary: bite/claw/body strike. Air: bite/claw/ram when valid."),
    ("IA_Brace", BOOL, "Brace/guard; hold to orient protected side. Air: wing tuck, reduces control."),
    ("IA_SprintWingbeat", BOOL, "Sprint/commit power. Air: powered wingbeat/afterburn effort."),
    ("IA_AscendTakeoff", BOOL, "Jump/takeoff when valid. Air: climb."),
    ("IA_DescendDive", BOOL, "Crouch/lower center. Air: descend/dive intent."),
    ("IA_BankLeft", BOOL, "Shoulder turn or sidestep left. Air: bank/roll left intent."),
    ("IA_BankRight", BOOL, "Shoulder turn or sidestep right. Air: bank/roll right intent."),
    ("IA_Breath", BOOL, "Breath weapon."),
    ("IA_Grapple", BOOL, "Initiate/contest grapple. Air: aerial grapple when contact permits."),
    ("IA_TargetAwareness", BOOL, "Soft target awareness. Never a hard animation lock."),
]


def make_input_action(name, value_type, description):
    path = INPUT_DIR + "/" + name
    if eal.does_asset_exist(path):
        ia = eal.load_asset(path)
        log("exists, updating: " + path)
    else:
        # InputActionFactory is not exposed to Python in 5.8; input assets
        # are DataAssets, so the generic factory works.
        factory = unreal.DataAssetFactory()
        factory.set_editor_property("data_asset_class", unreal.InputAction)
        ia = asset_tools.create_asset(name, INPUT_DIR, unreal.InputAction, factory)
        if ia is None:
            fail("could not create " + path)
            return None
        log("created " + path)
    ia.set_editor_property("value_type", value_type)
    try:
        ia.set_editor_property("action_description", unreal.Text(description))
    except Exception as exc:
        log("tooltip skipped for %s: %s" % (name, exc))
    eal.save_loaded_asset(ia)
    return ia


actions = {}
for action_name, vtype, desc in ACTIONS:
    ia = make_input_action(action_name, vtype, desc)
    if ia:
        actions[action_name] = ia

# ------------------------------------------------------------------ context
IMC_PATH = INPUT_DIR + "/IMC_Dragon"
if eal.does_asset_exist(IMC_PATH):
    imc = eal.load_asset(IMC_PATH)
    log("exists, rebuilding mappings: " + IMC_PATH)
else:
    imc_factory = unreal.DataAssetFactory()
    imc_factory.set_editor_property("data_asset_class", unreal.InputMappingContext)
    imc = asset_tools.create_asset("IMC_Dragon", INPUT_DIR,
                                   unreal.InputMappingContext, imc_factory)
    if imc is None:
        fail("could not create " + IMC_PATH)
    else:
        log("created " + IMC_PATH)


def negate(x=True, y=True, z=True):
    mod = unreal.new_object(unreal.InputModifierNegate, outer=imc)
    mod.set_editor_property("x", x)
    mod.set_editor_property("y", y)
    mod.set_editor_property("z", z)
    return mod


def swizzle_yxz():
    mod = unreal.new_object(unreal.InputModifierSwizzleAxis, outer=imc)
    mod.set_editor_property("order", unreal.InputAxisSwizzle.YXZ)
    return mod


# action name, key name, modifier factory list.
# WASD composite follows the engine third-person template convention:
# W/S swizzle onto Y, S/A negate. Mouse Y negated for mouse-up = look-up.
BINDINGS = [
    ("IA_Move", "W", [swizzle_yxz]),
    ("IA_Move", "S", [negate, swizzle_yxz]),
    ("IA_Move", "A", [negate]),
    ("IA_Move", "D", []),
    ("IA_Look", "Mouse2D", [lambda: negate(x=False, y=True, z=False)]),
    ("IA_PrimaryAttack", "LeftMouseButton", []),
    ("IA_Brace", "RightMouseButton", []),
    ("IA_SprintWingbeat", "LeftShift", []),
    ("IA_AscendTakeoff", "SpaceBar", []),
    ("IA_DescendDive", "LeftControl", []),
    ("IA_BankLeft", "Q", []),
    ("IA_BankRight", "E", []),
    ("IA_Breath", "F", []),
    ("IA_Grapple", "R", []),
    ("IA_TargetAwareness", "Tab", []),
]

def make_key(key_name):
    """FKey construction varies across UE python versions; try each route."""
    try:
        key = unreal.Key()
        key.set_editor_property("key_name", key_name)
        return key
    except Exception as exc:
        log("Key.set_editor_property route failed: %s" % exc)
    try:
        return unreal.Key(key_name=key_name)
    except Exception as exc:
        log("Key kwargs route failed: %s" % exc)
    # diagnostics for the next attempt
    log("dir(unreal.Key): " + ", ".join(sorted(dir(unreal.Key))))
    log("input libs: " + ", ".join(sorted(
        n for n in dir(unreal)
        if "Input" in n and ("Library" in n or "Editor" in n or "Subsystem" in n))))
    return None


if imc and not errors:
    mappings = []
    for action_name, key_name, modifier_factories in BINDINGS:
        if action_name not in actions:
            fail("no action for binding " + action_name)
            continue
        key = make_key(key_name)
        if key is None:
            fail("could not construct FKey for " + key_name)
            break
        mapping = unreal.EnhancedActionKeyMapping()
        mapping.set_editor_property("action", actions[action_name])
        mapping.set_editor_property("key", key)
        mapping.set_editor_property("modifiers",
                                    [factory() for factory in modifier_factories])
        mappings.append(mapping)
    imc.set_editor_property("mappings", mappings)
    eal.save_loaded_asset(imc)
    log("IMC_Dragon rebuilt with %d mappings" % len(mappings))

# ------------------------------------------------------------------- verify
if imc:
    reloaded = eal.load_asset(IMC_PATH)
    count = len(reloaded.get_editor_property("mappings"))
    if count != len(BINDINGS):
        fail("mapping count mismatch: %d saved, %d expected" % (count, len(BINDINGS)))
    for m in reloaded.get_editor_property("mappings"):
        action = m.get_editor_property("action")
        key = m.get_editor_property("key")
        key_text = str(key)
        log("mapping: %s <- %s" % (action.get_name() if action else "None", key_text))
        if "None" in key_text:
            fail("mapping saved without a key: " + (action.get_name() if action else "?"))

missing = [n for n, _, _ in ACTIONS if not eal.does_asset_exist(INPUT_DIR + "/" + n)]
if missing:
    fail("missing after run: " + ", ".join(missing))

if errors:
    unreal.log_error(LOG + "M04 BOOTSTRAP FAILED: %d error(s)" % len(errors))
else:
    log("M04 BOOTSTRAP OK")
