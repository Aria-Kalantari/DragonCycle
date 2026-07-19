# First prompt for Claude Code

Read `SPEC.md`, `CLAUDE.md`, `TASKS.md`, and all files under `Docs/`, starting with `RESEARCH_BASIS.md` and `BIOMECHANICS.md`. Then:

1. Explain the intended vertical slice in no more than ten bullets.
2. Inspect the Unreal C++ scaffold for likely Unreal Engine 5.8 compile issues.
3. Do not add features.
4. Implement only task M0.1. Note that M0.6 (the JSON to DataAsset importer) blocks all body-state work, and M1.1 (the movement component) blocks all of Milestone 1.
5. Run every locally available validation step: `python Tools/validate_data.py` and `python -m pytest -q`.
6. Report exact Unreal Editor steps I must perform manually.
7. Update `TASKS.md` only when the task is genuinely complete.

Do not claim that `.uasset` files or editor settings exist unless you created them in Unreal Editor through an available integration and verified them.

Unreal MCP (UE 5.8, Experimental) is that integration when the plugin is
enabled and connected. If it is available, say so and use it for M0.3-M0.5
rather than emitting a manual checklist. If it is not, say that too — do not
narrate editor steps as though they were performed.
