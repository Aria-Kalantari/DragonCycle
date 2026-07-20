"""The schema's part-id enum and EDragonBodyPart are one closed set (ADR-006).

The two lists live in different languages and are edited by different kinds of
change, which is exactly how they drift apart. The importer enforces the
bijection at import time; this test enforces it without an engine in the loop,
so plain CI catches the drift before anyone boots the editor.
"""

from __future__ import annotations

import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def normalize(name: str) -> str:
    """'wing_left' and 'WingLeft' agree after lowercasing and dropping underscores."""
    return name.lower().replace("_", "")


def schema_part_ids() -> list[str]:
    schema = json.loads((ROOT / "Data" / "Schemas" / "dragon_species.schema.json").read_text())
    return schema["properties"]["body_parts"]["items"]["properties"]["id"]["enum"]


def enum_part_names() -> list[str]:
    header = (ROOT / "Source" / "DragonCycle" / "DragonTypes.h").read_text()
    match = re.search(r"enum class EDragonBodyPart : uint8\s*\{(.*?)\};", header, re.S)
    assert match, "EDragonBodyPart not found in DragonTypes.h"
    body = re.sub(r"/\*.*?\*/", "", match.group(1), flags=re.S)
    body = re.sub(r"//[^\n]*", "", body)
    body = re.sub(r"UMETA\s*\([^)]*\)", "", body)
    tokens = [token.strip().split("=")[0].strip() for token in body.split(",")]
    return [token for token in tokens if re.fullmatch(r"[A-Za-z0-9_]+", token or "")]


def test_closed_set_is_bijective() -> None:
    schema_ids = schema_part_ids()
    enum_names = enum_part_names()
    assert len(schema_ids) == len(set(schema_ids)), "duplicate ids in schema part enum"
    assert len(enum_names) == len(set(enum_names)), "duplicate entries in EDragonBodyPart"
    assert sorted(normalize(i) for i in schema_ids) == sorted(normalize(n) for n in enum_names), (
        "schema part-id enum and EDragonBodyPart have drifted apart; "
        "ADR-006 requires changing them together"
    )


def test_closed_set_has_eleven_parts() -> None:
    # Deliberate tripwire, not redundancy: growing or shrinking the closed set
    # is a design decision that must update this number consciously.
    assert len(schema_part_ids()) == 11
