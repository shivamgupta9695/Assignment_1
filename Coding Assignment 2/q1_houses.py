"""
================================================================================
Q1 - Houses of Ice and Fire
================================================================================
Tasks:
    a. Create a list of all houses and regions from API
    b. Write this list in a text file
    c. Order all houses alphabetically

API URL : https://anapioficeandfire.com/api/houses
Author  : Calsoft Internship Assignment 2026
================================================================================
"""

import requests
import logging
from datetime import datetime

# ── Logging Setup ─────────────────────────────────────────────────────────────
logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s  [%(levelname)s]  %(message)s",
    datefmt="%H:%M:%S"
)
log = logging.getLogger(__name__)

# ── Constants ─────────────────────────────────────────────────────────────────
BASE_URL  = "https://anapioficeandfire.com/api/houses"
PAGE_SIZE = 50
OUT_FILE  = "Q1_houses_output.txt"


# ══════════════════════════════════════════════════════════════════════════════
#  a. Fetch all houses from paginated API
# ══════════════════════════════════════════════════════════════════════════════

def fetch_all_houses() -> list[dict]:
    """
    Fetches every house object from the Ice and Fire API.
    Handles pagination automatically using the Link header.

    Returns:
        List of house dicts with keys: name, region, coatOfArms, words, etc.
    """
    houses = []
    page   = 1

    log.info("Starting to fetch houses from API...")

    while True:
        params   = {"page": page, "pageSize": PAGE_SIZE}
        response = requests.get(BASE_URL, params=params, timeout=15)
        response.raise_for_status()

        batch = response.json()
        if not batch:                   # empty page → we're done
            break

        houses.extend(batch)
        log.info(f"  Page {page:>3} fetched  |  Running total: {len(houses)} houses")

        # Follow Link header for next page (standard RFC 5988 pagination)
        link_header = response.headers.get("Link", "")
        if 'rel="next"' not in link_header:
            break
        page += 1

    log.info(f"Fetch complete. Total houses retrieved: {len(houses)}")
    return houses


# ══════════════════════════════════════════════════════════════════════════════
#  b. Build (name, region) list  +  write to text file
# ══════════════════════════════════════════════════════════════════════════════

def build_house_list(houses: list[dict]) -> list[tuple[str, str]]:
    """
    Extracts (house_name, region) tuples from the raw API data.

    Args:
        houses: Raw list of house dicts from the API.

    Returns:
        List of (name, region) tuples.
    """
    return [
        (
            h.get("name")   or "(Unknown House)",
            h.get("region") or "(No Region)"
        )
        for h in houses
    ]


def write_to_text_file(house_list: list[tuple[str, str]], filepath: str) -> None:
    """
    Writes the sorted house list to a neatly formatted plain-text file.

    Args:
        house_list: Sorted list of (name, region) tuples.
        filepath  : Destination file path.
    """
    width_name   = max(len(n) for n, _ in house_list) + 2
    width_region = max(len(r) for _, r in house_list) + 2
    separator    = "─" * (width_name + width_region + 9)

    with open(filepath, "w", encoding="utf-8") as f:
        f.write("=" * (width_name + width_region + 9) + "\n")
        f.write("         HOUSES OF ICE AND FIRE  (Alphabetical Order)\n")
        f.write("=" * (width_name + width_region + 9) + "\n")
        f.write(f"  Generated : {datetime.now().strftime('%Y-%m-%d  %H:%M:%S')}\n")
        f.write(f"  Total     : {len(house_list)} houses\n")
        f.write("=" * (width_name + width_region + 9) + "\n\n")

        # Column headers
        f.write(f"  {'No.':<5} {'House Name':<{width_name}} {'Region':<{width_region}}\n")
        f.write(f"  {separator}\n")

        for idx, (name, region) in enumerate(house_list, start=1):
            f.write(f"  {idx:<5} {name:<{width_name}} {region:<{width_region}}\n")

        f.write(f"\n  {separator}\n")
        f.write(f"  END OF LIST  |  Total: {len(house_list)} houses\n")
        f.write("=" * (width_name + width_region + 9) + "\n")

    log.info(f"Text file written: {filepath}")


# ══════════════════════════════════════════════════════════════════════════════
#  c. Sort alphabetically
# ══════════════════════════════════════════════════════════════════════════════

def sort_houses(house_list: list[tuple[str, str]]) -> list[tuple[str, str]]:
    """
    Sorts houses alphabetically by name (case-insensitive).

    Args:
        house_list: List of (name, region) tuples.

    Returns:
        Sorted list.
    """
    return sorted(house_list, key=lambda h: h[0].lower())


# ══════════════════════════════════════════════════════════════════════════════
#  Main
# ══════════════════════════════════════════════════════════════════════════════

def main():
    print("\n" + "═" * 60)
    print("   Q1 – HOUSES OF ICE AND FIRE")
    print("═" * 60)

    # a. Fetch
    raw_houses = fetch_all_houses()

    # Build (name, region) pairs
    house_list = build_house_list(raw_houses)
    log.info(f"House list built: {len(house_list)} entries")

    # c. Sort
    sorted_houses = sort_houses(house_list)
    log.info("Houses sorted alphabetically.")

    # b. Write file
    write_to_text_file(sorted_houses, OUT_FILE)

    # Console preview
    print("\n  ┌─────────────────────────────────────────────────────────┐")
    print("  │  PREVIEW – First 10 Houses (sorted)                     │")
    print("  ├──────┬──────────────────────────────────────┬────────────┤")
    print(f"  │ {'No':<5}│ {'House Name':<37}│ {'Region':<11}│")
    print("  ├──────┼──────────────────────────────────────┼────────────┤")
    for i, (name, region) in enumerate(sorted_houses[:10], 1):
        print(f"  │ {i:<5}│ {name[:36]:<37}│ {region[:10]:<11}│")
    print("  └──────┴──────────────────────────────────────┴────────────┘")

    print(f"\n  ✔  Done!  {len(sorted_houses)} houses saved to → {OUT_FILE}\n")


if __name__ == "__main__":
    main()
