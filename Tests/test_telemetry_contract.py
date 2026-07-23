"""The telemetry code and Docs/TELEMETRY.md are one contract (ADR-010).

Like test_closed_set.py, this catches drift without an engine in the loop. The
combat-event vocabulary and the ten falsification thresholds are the kind of
thing a later balance pass is tempted to quietly soften; these assertions make
that edit fail plain CI instead of passing silently.
"""

from __future__ import annotations

import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

TELEMETRY = ROOT / "Docs" / "TELEMETRY.md"
EVENT_HDR = ROOT / "Source" / "DragonCycle" / "DragonCombatTelemetry.h"
FALSIFY_HDR = ROOT / "Source" / "DragonCycle" / "DragonFalsification.h"
ATTACK_SCHEMA = ROOT / "Data" / "Schemas" / "attack.schema.json"


def normalize(name: str) -> str:
    """'wing disable', 'wing_disable', and 'WingDisable' agree after this."""
    return re.sub(r"[^a-z0-9]", "", name.lower())


def enum_values(header_text: str, enum_name: str) -> list[str]:
    match = re.search(rf"enum class {enum_name} : uint8\s*\{{(.*?)\}};", header_text, re.S)
    assert match, f"{enum_name} not found"
    body = re.sub(r"/\*.*?\*/", "", match.group(1), flags=re.S)
    body = re.sub(r"//[^\n]*", "", body)
    tokens = [t.strip().split("=")[0].strip() for t in body.split(",")]
    return [t for t in tokens if re.fullmatch(r"[A-Za-z0-9_]+", t or "")]


def code_constants() -> dict[str, float]:
    text = FALSIFY_HDR.read_text(encoding="utf-8")
    return {
        m.group(1): float(m.group(2))
        for m in re.finditer(r"constexpr double (\w+)\s*=\s*([0-9.]+);", text)
    }


# --- win_condition: exactly the five TELEMETRY.md values --------------------


def test_win_condition_matches_doc() -> None:
    flat = re.sub(r"\s+", " ", TELEMETRY.read_text(encoding="utf-8"))
    match = re.search(r"must be one of: (.+?)\.", flat)
    assert match, "win_condition sentence not found in TELEMETRY.md"
    doc_values = {normalize(p) for p in match.group(1).split(",")}
    assert len(doc_values) == 5, f"expected five win conditions, found {doc_values}"

    enum_values_norm = {normalize(v) for v in enum_values(EVENT_HDR.read_text(encoding="utf-8"), "EDragonWinCondition")}
    assert enum_values_norm == doc_values, (
        "EDragonWinCondition and the TELEMETRY.md win_condition list have drifted; "
        "they must stay exactly the five documented values"
    )


# --- commit_class: same closed set as the authoring schema ------------------


def test_commit_class_matches_attack_schema() -> None:
    schema = json.loads(ATTACK_SCHEMA.read_text(encoding="utf-8"))
    schema_values = {normalize(v) for v in schema["properties"]["commit_class"]["enum"]}
    enum_values_norm = {normalize(v) for v in enum_values(EVENT_HDR.read_text(encoding="utf-8"), "EDragonCommitClass")}
    assert enum_values_norm == schema_values, (
        "EDragonCommitClass and the attack schema's commit_class enum have drifted"
    )


# --- target_body_part uses the closed EDragonBodyPart set -------------------


def test_target_body_part_uses_body_part_enum() -> None:
    text = EVENT_HDR.read_text(encoding="utf-8")
    assert re.search(r"EDragonBodyPart\s+TargetBodyPart", text), (
        "FDragonCombatEvent.target_body_part must be typed EDragonBodyPart so it "
        "shares the closed 11-part vocabulary; no parallel part list is allowed"
    )


# --- F1-F10 thresholds match the document numbers verbatim ------------------

# Which code constant(s) carry each documented threshold, in the order the
# numbers appear in the doc row.
CODE_FOR_ROW = {
    "F1": ["F1_DrakeLethalWinFraction"],
    "F2": ["F2_DrakeContactFraction"],
    "F3": ["F3_ObserverCorrectFraction"],
    "F4": ["F4_HeatmapCorrelation"],
    "F7": ["F7_StaminaBandLow", "F7_StaminaBandHigh"],
    "F8": ["F8_WinConditionShare"],
    "F9": ["F9_AncientWinRate"],
    "F10": ["F10_MinSeconds", "F10_MaxSeconds"],
}


def doc_rows() -> dict[str, str]:
    """Falsification-table rows keyed by F-id -> the 'Measured by' cell text."""
    rows: dict[str, str] = {}
    for line in TELEMETRY.read_text(encoding="utf-8").splitlines():
        if not line.lstrip().startswith("| F"):
            continue
        cells = [c.strip() for c in line.strip().strip("|").split("|")]
        if len(cells) >= 3 and re.fullmatch(r"F\d+", cells[0]):
            rows[cells[0]] = cells[2]
    return rows


def doc_thresholds(measured: str) -> list[float]:
    """Numbers in a 'Measured by' cell, converted to the code's units."""
    numbers = [float(n) for n in re.findall(r"\d+(?:\.\d+)?", measured)]
    lower = measured.lower()
    if "minute" in lower:
        return [n * 60.0 for n in numbers]
    if "%" in measured:
        return [n / 100.0 for n in numbers]
    return numbers


def test_all_ten_thresholds_present() -> None:
    rows = doc_rows()
    assert sorted(rows, key=lambda f: int(f[1:])) == [f"F{i}" for i in range(1, 11)], (
        f"TELEMETRY.md must document exactly F1..F10; found {sorted(rows)}"
    )


def test_thresholds_match_document() -> None:
    rows = doc_rows()
    constants = code_constants()

    for f_id, const_names in CODE_FOR_ROW.items():
        expected = doc_thresholds(rows[f_id])
        assert len(expected) == len(const_names), (
            f"{f_id}: doc has {len(expected)} numbers but {len(const_names)} code constants mapped"
        )
        for value, const_name in zip(expected, const_names):
            assert const_name in constants, f"{const_name} missing from DragonFalsification.h"
            assert abs(constants[const_name] - value) < 1e-9, (
                f"{f_id}: {const_name}={constants[const_name]} but TELEMETRY.md says {value}. "
                "Thresholds must not be softened, rounded, or re-derived."
            )
