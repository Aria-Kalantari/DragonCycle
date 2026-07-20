"""M0.3 editor bootstrap: L_BasaltCaldera_Greybox, GM_DragonCycle, BP_Drake_Player.

Run headless while the interactive editor may be open in parallel:

    UnrealEditor-Cmd.exe DragonCycle.uproject -run=pythonscript \
        -script="Tools/editor_bootstrap_m03.py"

Idempotent: skips assets that already exist rather than overwriting them,
so a partial earlier run can be resumed. All units are centimeters.

Greybox contents follow Docs/UNREAL_EDITOR_CHECKLIST.md "First map":
basin floor, two launch shelves, five pillars, one thermal trigger volume,
one cliff drop, scale references at 2 m / 10 m / 30 m, simple lighting,
simple collision (engine basic shapes only). The tuned arena pass is M7.1.
"""

import unreal

LOG = "[M03] "
BP_DIR = "/Game/DragonCycle/Blueprints"
GAME_DIR = "/Game/DragonCycle/Game"
MAPS_DIR = "/Game/DragonCycle/Maps"
PAWN_BP_PATH = BP_DIR + "/BP_Drake_Player"
GM_BP_PATH = GAME_DIR + "/GM_DragonCycle"
LEVEL_PATH = MAPS_DIR + "/L_BasaltCaldera_Greybox"

CUBE = "/Engine/BasicShapes/Cube"          # 100 cm base size
CYLINDER = "/Engine/BasicShapes/Cylinder"  # 100 cm diameter, 100 cm tall

asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
eal = unreal.EditorAssetLibrary
errors = []


def log(msg):
    unreal.log(LOG + msg)


def fail(msg):
    errors.append(msg)
    unreal.log_error(LOG + msg)


def create_blueprint(asset_name, package_dir, parent_class):
    """Create a Blueprint child of a C++ class, or return the existing one."""
    asset_path = package_dir + "/" + asset_name
    if eal.does_asset_exist(asset_path):
        log("exists, skipping: " + asset_path)
        return eal.load_asset(asset_path)
    factory = unreal.BlueprintFactory()
    factory.set_editor_property("parent_class", parent_class)
    bp = asset_tools.create_asset(asset_name, package_dir, None, factory)
    if bp is None:
        fail("could not create blueprint " + asset_path)
        return None
    eal.save_loaded_asset(bp)
    log("created " + asset_path)
    return bp


def generated_class(bp_path):
    cls = unreal.load_object(None, bp_path + "." + bp_path.rsplit("/", 1)[-1] + "_C")
    if cls is None:
        fail("could not load generated class for " + bp_path)
    return cls


# ---------------------------------------------------------------- blueprints
pawn_bp = create_blueprint("BP_Drake_Player", BP_DIR, unreal.DragonPawn)
gm_bp = create_blueprint("GM_DragonCycle", GAME_DIR, unreal.DragonCycleGameModeBase)
pawn_class = generated_class(PAWN_BP_PATH)
gm_class = generated_class(GM_BP_PATH)

if gm_class and pawn_class:
    gm_cdo = unreal.get_default_object(gm_class)
    gm_cdo.set_editor_property("default_pawn_class", pawn_class)
    if gm_bp:
        eal.save_loaded_asset(gm_bp)
    log("GM_DragonCycle.DefaultPawnClass = BP_Drake_Player")

# --------------------------------------------------------------------- level
# LevelEditorSubsystem needs the level-editor UI and crashes under
# -run=pythonscript; EditorLoadingAndSavingUtils is engine-level and safe.
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
lsu = unreal.EditorLoadingAndSavingUtils

world = None
populate = True
if eal.does_asset_exist(LEVEL_PATH):
    log("level exists, loading it: " + LEVEL_PATH)
    world = lsu.load_map(LEVEL_PATH)
    if world:
        # A crashed earlier run can leave an empty map behind; only skip
        # population when the greybox is actually there.
        labels = [a.get_actor_label() for a in eas.get_all_level_actors()]
        populate = "BasinFloor" not in labels
        if not populate:
            log("greybox already populated, skipping geometry")
else:
    world = lsu.new_blank_map(False)
if world is None:
    fail("could not create or load level " + LEVEL_PATH)


def spawn_mesh(mesh_path, label, folder, loc, scale, rot=None):
    mesh = eal.load_asset(mesh_path)
    actor = eas.spawn_actor_from_object(mesh, unreal.Vector(*loc),
                                        rot or unreal.Rotator(0, 0, 0))
    if actor is None:
        fail("could not spawn " + label)
        return None
    actor.set_actor_scale3d(unreal.Vector(*scale))
    actor.set_actor_label(label)
    actor.set_folder_path(folder)
    return actor


def spawn_class(cls, label, folder, loc, rot=None, scale=None):
    actor = eas.spawn_actor_from_class(cls, unreal.Vector(*loc),
                                       rot or unreal.Rotator(0, 0, 0))
    if actor is None:
        fail("could not spawn " + label)
        return None
    if scale:
        actor.set_actor_scale3d(unreal.Vector(*scale))
    actor.set_actor_label(label)
    actor.set_folder_path(folder)
    return actor


if not errors and populate:
    # Basin floor: 200 m x 200 m slab, top surface at Z = 0.
    spawn_mesh(CUBE, "BasinFloor", "Greybox", (0, 0, -50), (200, 200, 1))

    # Cliff: the floor's +X rim. A lower slab 50 m down gives the fall risk.
    spawn_mesh(CUBE, "CliffBase", "Greybox", (17500, 0, -5050), (150, 200, 1))

    # Two elevated launch shelves (SPEC 12.2).
    spawn_mesh(CUBE, "LaunchShelf_West", "Greybox", (-7000, -6000, 750), (30, 30, 15))
    spawn_mesh(CUBE, "LaunchShelf_North", "Greybox", (6500, 7000, 1000), (30, 30, 20))

    # Five basalt pillars: 6 m diameter, 30 m tall, breaking line of breath.
    for i, (px, py) in enumerate(
            [(2500, -2000), (-3500, 1500), (1000, 4500), (-1500, -4500), (5000, 3000)],
            start=1):
        spawn_mesh(CYLINDER, "BasaltPillar_%d" % i, "Greybox/Pillars",
                   (px, py, 1500), (6, 6, 30))

    # Thermal column placeholder: trigger volume only, no logic until M1.4.
    spawn_class(unreal.TriggerBox, "ThermalColumn_Trigger", "Greybox",
                (-5000, 5000, 3000), scale=(12.5, 12.5, 75))

    # Scale references (checklist: 2 m, 10 m, 30 m) with labels.
    for size_m, loc in [(2, (1200, -900)), (10, (2500, -3500)), (30, (5500, -6500))]:
        h = size_m * 100
        spawn_mesh(CUBE, "ScaleRef_%dm" % size_m, "Greybox/ScaleRefs",
                   (loc[0], loc[1], h / 2), (size_m, size_m, size_m))
        text = spawn_class(unreal.TextRenderActor, "ScaleLabel_%dm" % size_m,
                           "Greybox/ScaleRefs",
                           (loc[0], loc[1] - h / 2 - 150, h + 100),
                           rot=unreal.Rotator(0, 0, 90))
        if text:
            comp = text.get_component_by_class(unreal.TextRenderComponent)
            comp.set_text("%d m" % size_m)
            comp.set_world_size(max(150, size_m * 25))

    spawn_class(unreal.PlayerStart, "PlayerStart", "Gameplay", (0, 0, 300))

    spawn_class(unreal.DirectionalLight, "Sun", "Lighting", (0, 0, 10000),
                rot=unreal.Rotator(0, -50, 30))
    spawn_class(unreal.SkyLight, "SkyLight", "Lighting", (0, 0, 10000))
    spawn_class(unreal.SkyAtmosphere, "SkyAtmosphere", "Lighting", (0, 0, 0))

if not errors and world:
    # Per-map game mode override keeps the DefaultEngine.ini delta minimal.
    try:
        ws = world.get_world_settings()
        ws.set_editor_property("default_game_mode", gm_class)
        log("WorldSettings.DefaultGameMode = GM_DragonCycle")
    except Exception as exc:  # fall back recorded for the caller to handle in ini
        fail("world settings override failed, use GlobalDefaultGameMode: %s" % exc)

    if not lsu.save_map(world, LEVEL_PATH):
        fail("could not save level " + LEVEL_PATH)

# ------------------------------------------------------------------- summary
for path in (PAWN_BP_PATH, GM_BP_PATH, LEVEL_PATH):
    if not eal.does_asset_exist(path):
        fail("missing after run: " + path)

if errors:
    unreal.log_error(LOG + "M03 BOOTSTRAP FAILED: %d error(s)" % len(errors))
else:
    log("M03 BOOTSTRAP OK")
