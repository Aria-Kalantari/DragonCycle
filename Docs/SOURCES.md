# Official implementation references

These are implementation references, not design authorities. `SPEC.md` remains the project source of truth.

## Unreal Engine

- Gameplay Systems: https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-systems-in-unreal-engine
- Gameplay Framework: https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-framework-in-unreal-engine
- Gameplay Ability System: https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-ability-system-for-unreal-engine
- Using Gameplay Abilities: https://dev.epicgames.com/documentation/en-us/unreal-engine/using-gameplay-abilities-in-unreal-engine
- StateTree Quick Start: https://dev.epicgames.com/documentation/en-us/unreal-engine/statetree-quick-start-guide
- Motion Matching: https://dev.epicgames.com/documentation/en-us/unreal-engine/motion-matching-in-unreal-engine
- Motion Warping: https://dev.epicgames.com/documentation/en-us/unreal-engine/motion-warping-in-unreal-engine
- Control Rig: https://dev.epicgames.com/documentation/en-us/unreal-engine/control-rig-in-unreal-engine
- Chaos Destruction: https://dev.epicgames.com/documentation/en-us/unreal-engine/chaos-destruction-in-unreal-engine

Behavior Trees are intentionally not used. See ADR-007.

- Unreal MCP in Unreal Editor: https://dev.epicgames.com/documentation/unreal-engine/unreal-mcp-in-unreal-editor

Unreal MCP is a first-party Experimental plugin in UE 5.8 (identifier
`ModelContextProtocol`; requires All Toolsets). Local-only, unauthenticated,
HTTP and SSE, and it serialises tool calls on the game thread. Enabled in
`DragonCycle.uproject`; setup in `UNREAL_EDITOR_CHECKLIST.md`. Good for
agent-assisted editor iteration, not a production runtime dependency.

## Claude Code

- Documentation: https://code.claude.com/docs/en/overview
- CLI reference: https://code.claude.com/docs/en/cli-reference
- Best practices and CLAUDE.md: https://www.anthropic.com/engineering/claude-code-best-practices

The previous `docs.anthropic.com/en/docs/claude-code/...` paths are stale.

## Higgsfield

- CLI and agent integration: https://higgsfield.ai/cli

Verified July 2026: `npm install -g @higgsfield/cli`, `higgsfield auth login`,
`npx skills add higgsfield-ai/skills`.

## Design research

- `Docs/RESEARCH_BIBLIOGRAPHY.md` — 46 sources, mapped constraint by constraint, with evidence status.
- `Docs/RESEARCH_BASIS.md` — what the July 2026 rebase adopted, rejected, and why.
