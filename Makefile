.PHONY: validate test check tree

validate:
	python Tools/validate_data.py

test:
	python -m pytest -q

check: validate test

tree:
	find . -maxdepth 4 -type f | sort
