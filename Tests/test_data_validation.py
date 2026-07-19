"""Validator contract tests.

The validator is a quality gate that CLAUDE.md mandates on every change, so
its failure behaviour is part of the contract: bad data must produce a
reported error and exit 1, never a traceback.
"""

from __future__ import annotations

import json
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[1]


def run_validator(root: Path) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        [sys.executable, str(root / "Tools" / "validate_data.py")],
        cwd=root, capture_output=True, text=True, check=False,
    )


@pytest.fixture
def sandbox():
    """A throwaway copy of the repo data so mutations cannot corrupt the real files."""
    with tempfile.TemporaryDirectory() as tmp:
        root = Path(tmp) / "kit"
        root.mkdir()
        shutil.copytree(ROOT / "Data", root / "Data")
        (root / "Tools").mkdir()
        shutil.copy(ROOT / "Tools" / "validate_data.py", root / "Tools" / "validate_data.py")
        yield root


def mutate_species(root: Path, name: str, fn) -> None:
    path = root / "Data" / "Dragons" / name
    data = json.loads(path.read_text())
    fn(data)
    path.write_text(json.dumps(data))


def test_shipped_data_validates() -> None:
    result = run_validator(ROOT)
    assert result.returncode == 0, result.stderr


def test_unknown_morphotype_reports_instead_of_crashing(sandbox) -> None:
    mutate_species(sandbox, "drake_prime_adult.json", lambda d: d.update(morphotype="basilisk"))
    result = run_validator(sandbox)
    assert result.returncode == 1
    assert "Traceback" not in result.stderr
    assert "basilisk" in result.stderr


def test_malformed_json_reports_line_and_column(sandbox) -> None:
    (sandbox / "Data" / "Dragons" / "broken.json").write_text('{"id": "broken",')
    result = run_validator(sandbox)
    assert result.returncode == 1
    assert "Traceback" not in result.stderr
    assert "invalid JSON" in result.stderr


def test_attack_requiring_absent_part_is_rejected(sandbox) -> None:
    path = sandbox / "Data" / "attacks.json"
    attacks = json.loads(path.read_text())
    attacks[0]["morphotype"] = "wyvern"
    attacks[0]["required_parts"] = ["forelimb_left"]
    path.write_text(json.dumps(attacks))
    result = run_validator(sandbox)
    assert result.returncode == 1
    assert "which no wyvern declares" in result.stderr


def test_orphaned_follow_up_state_is_rejected(sandbox) -> None:
    path = sandbox / "Data" / "attacks.json"
    attacks = [a for a in json.loads(path.read_text()) if a["id"] != "drake_bite"]
    path.write_text(json.dumps(attacks))
    result = run_validator(sandbox)
    assert result.returncode == 1
    assert "bite_secured" in result.stderr


def test_wing_loading_ordering_is_enforced(sandbox) -> None:
    """A heavier-loaded morphotype must not also be the more agile one."""
    mutate_species(
        sandbox, "drake_prime_adult.json",
        lambda d: d["flight_envelope"].update(max_turn_rate_deg_s=140),
    )
    result = run_validator(sandbox)
    assert result.returncode == 1
    assert "wing loading" in result.stderr


def test_wyvern_may_not_declare_separate_forelimbs(sandbox) -> None:
    def add_forelimbs(d):
        d["body_parts"].extend([
            {"id": "forelimb_left", "max_integrity": 100, "armor": 0.2, "break_threshold": 0.3},
            {"id": "forelimb_right", "max_integrity": 100, "armor": 0.2, "break_threshold": 0.3},
        ])
    mutate_species(sandbox, "wyvern_prime_adult.json", add_forelimbs)
    result = run_validator(sandbox)
    assert result.returncode == 1
    assert "must not declare separate forelimb parts" in result.stderr


def test_neck_is_mandatory(sandbox) -> None:
    mutate_species(
        sandbox, "longneck_prime_adult.json",
        lambda d: d.update(body_parts=[p for p in d["body_parts"] if p["id"] != "neck"]),
    )
    result = run_validator(sandbox)
    assert result.returncode == 1
    assert "neck is a required damageable part" in result.stderr


def test_mutual_assessment_may_not_outrun_experience(sandbox) -> None:
    """Opponent-reading is learned; a naive individual cannot be authored with it."""
    def naive_but_perceptive(d):
        d["experience"] = "naive"
        d["ai_weights"]["mutual_assessment_weight"] = 0.9
    mutate_species(sandbox, "wyvern_prime_adult.json", naive_but_perceptive)
    result = run_validator(sandbox)
    assert result.returncode == 1
    assert "naive ceiling" in result.stderr
