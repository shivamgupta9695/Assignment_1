"""
================================================================================
Q3 - Characters of Ice and Fire
================================================================================
Tasks:
    a. Get all characters from API
    b. Find how many seasons the character was part of
    c. Sort according to number of TV season appearances
    d. Add all available sorted data into an Excel file

API URL : https://anapioficeandfire.com/api/characters
Author  : Calsoft Internship Assignment 2026

Dependencies:
    pip install requests openpyxl
================================================================================
"""

import requests
import logging
from datetime import datetime

try:
    import openpyxl
    from openpyxl.styles import (
        Font, PatternFill, Alignment, Border, Side
    )
    from openpyxl.utils import get_column_letter
except ImportError:
    raise SystemExit(
        "Missing dependency: run  pip install openpyxl  then re-run this script."
    )

# ── Logging Setup ─────────────────────────────────────────────────────────────
logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s  [%(levelname)s]  %(message)s",
    datefmt="%H:%M:%S"
)
log = logging.getLogger(__name__)

# ── Constants ─────────────────────────────────────────────────────────────────
BASE_URL   = "https://anapioficeandfire.com/api/characters"
PAGE_SIZE  = 50
OUT_EXCEL  = "Q3_characters_output.xlsx"


# ══════════════════════════════════════════════════════════════════════════════
#  a. Fetch all characters from paginated API
# ══════════════════════════════════════════════════════════════════════════════

def fetch_all_characters() -> list[dict]:
    """
    Fetches all character records from the Ice and Fire API.
    The API has ~2,100+ characters spread across many pages.

    Returns:
        List of raw character dicts.
    """
    characters = []
    page       = 1

    log.info("Fetching characters from API (this may take a minute)...")

    while True:
        response = requests.get(
            BASE_URL,
            params={"page": page, "pageSize": PAGE_SIZE},
            timeout=20
        )
        response.raise_for_status()

        batch = response.json()
        if not batch:
            break

        characters.extend(batch)

        if page % 10 == 0 or page == 1:
            log.info(f"  Page {page:>3}  |  Total characters so far: {len(characters)}")

        if 'rel="next"' not in response.headers.get("Link", ""):
            break
        page += 1

    log.info(f"Fetch complete. Total characters: {len(characters)}")
    return characters


# ══════════════════════════════════════════════════════════════════════════════
#  b. Count seasons + build structured records
# ══════════════════════════════════════════════════════════════════════════════

def build_character_records(characters: list[dict]) -> list[dict]:
    """
    Processes raw character data.

    For each character:
      - Extracts all available fields
      - Counts unique TV seasons (tvSeries list) → season_count
      - Lists all aliases, titles, books appeared in

    Args:
        characters: Raw API character list.

    Returns:
        List of structured character dicts, ready for sorting and export.
    """
    records = []

    for char in characters:
        # b. Season count: tvSeries is a list like ["Season 1", "Season 2", ...]
        tv_series    = [s.strip() for s in char.get("tvSeries", []) if s.strip()]
        season_count = len(tv_series)

        # All available fields
        record = {
            "name"          : char.get("name")    or "(Unnamed)",
            "gender"        : char.get("gender")  or "Unknown",
            "culture"       : char.get("culture") or "Unknown",
            "born"          : char.get("born")    or "Unknown",
            "died"          : char.get("died")    or "Unknown",
            "titles"        : ", ".join(char.get("titles", []))   or "None",
            "aliases"       : ", ".join(char.get("aliases", []))  or "None",
            "father"        : char.get("father")  or "",
            "mother"        : char.get("mother")  or "",
            "spouse"        : char.get("spouse")  or "",
            "tv_seasons"    : ", ".join(tv_series) if tv_series else "None",
            "season_count"  : season_count,
            "playedBy"      : ", ".join(char.get("playedBy", []))  or "None",
            "books_count"   : len(char.get("books", [])),
            "povBooks_count": len(char.get("povBooks", [])),
        }
        records.append(record)

    return records


# ══════════════════════════════════════════════════════════════════════════════
#  c. Sort by number of season appearances (descending)
# ══════════════════════════════════════════════════════════════════════════════

def sort_by_seasons(records: list[dict]) -> list[dict]:
    """
    Sorts characters by season_count descending (most appearances first).
    Secondary sort: alphabetical by name.

    Args:
        records: List of character dicts.

    Returns:
        Sorted list.
    """
    return sorted(
        records,
        key=lambda c: (-c["season_count"], c["name"].lower())
    )


# ══════════════════════════════════════════════════════════════════════════════
#  d. Write to Excel with full formatting
# ══════════════════════════════════════════════════════════════════════════════

def write_to_excel(records: list[dict], filepath: str) -> None:
    """
    Writes sorted character data into a formatted Excel (.xlsx) file.

    Features:
      - Frozen header row
      - Alternating row colours
      - Auto-fitted column widths
      - Bold, coloured header row
      - A summary sheet with statistics

    Args:
        records : Sorted list of character dicts.
        filepath: Output .xlsx file path.
    """
    wb = openpyxl.Workbook()

    # ── Sheet 1: Characters Data ─────────────────────────────────────────────
    ws = wb.active
    ws.title = "Characters"

    # Styles
    header_font    = Font(bold=True, color="FFFFFF", size=11)
    header_fill    = PatternFill("solid", fgColor="1F3864")   # dark navy
    alt_fill       = PatternFill("solid", fgColor="DCE6F1")   # light blue
    center_align   = Alignment(horizontal="center", vertical="center", wrap_text=True)
    left_align     = Alignment(horizontal="left",   vertical="center", wrap_text=True)
    thin           = Side(style="thin", color="AAAAAA")
    border         = Border(left=thin, right=thin, top=thin, bottom=thin)

    # Column definitions: (header_label, dict_key, width)
    columns = [
        ("Rank",              None,             6),
        ("Character Name",    "name",           35),
        ("Season Count",      "season_count",   14),
        ("Seasons Appeared",  "tv_seasons",     35),
        ("Played By",         "playedBy",       25),
        ("Gender",            "gender",         10),
        ("Culture",           "culture",        15),
        ("Born",              "born",           18),
        ("Died",              "died",           18),
        ("Titles",            "titles",         30),
        ("Aliases",           "aliases",        30),
        ("Books Count",       "books_count",    13),
        ("POV Books Count",   "povBooks_count", 16),
    ]

    # Write header row
    for col_idx, (header, _, width) in enumerate(columns, start=1):
        cell = ws.cell(row=1, column=col_idx, value=header)
        cell.font      = header_font
        cell.fill      = header_fill
        cell.alignment = center_align
        cell.border    = border
        ws.column_dimensions[get_column_letter(col_idx)].width = width

    ws.row_dimensions[1].height = 30
    ws.freeze_panes = "A2"   # freeze header

    # Write data rows
    for row_idx, char in enumerate(records, start=2):
        is_alt = (row_idx % 2 == 0)
        row_fill = alt_fill if is_alt else None

        rank_cell = ws.cell(row=row_idx, column=1, value=row_idx - 1)
        rank_cell.alignment = center_align
        rank_cell.border    = border
        if row_fill: rank_cell.fill = row_fill

        for col_idx, (_, key, _) in enumerate(columns[1:], start=2):
            value = char.get(key, "")
            cell  = ws.cell(row=row_idx, column=col_idx, value=value)
            cell.alignment = center_align if col_idx in (3, 6, 12, 13) else left_align
            cell.border    = border
            if row_fill: cell.fill = row_fill

        ws.row_dimensions[row_idx].height = 20

    # Auto-filter on header row
    ws.auto_filter.ref = f"A1:{get_column_letter(len(columns))}1"

    # ── Sheet 2: Summary Statistics ──────────────────────────────────────────
    ws2 = wb.create_sheet("Summary")

    title_font   = Font(bold=True, size=14, color="1F3864")
    label_font   = Font(bold=True, size=11)
    summary_fill = PatternFill("solid", fgColor="F2F2F2")

    ws2.column_dimensions["A"].width = 35
    ws2.column_dimensions["B"].width = 25

    ws2["A1"] = "Q3 – Characters of Ice and Fire"
    ws2["A1"].font = title_font
    ws2.merge_cells("A1:B1")

    ws2["A2"] = f"Generated: {datetime.now().strftime('%Y-%m-%d  %H:%M:%S')}"
    ws2["A2"].font = Font(italic=True, color="666666")
    ws2.merge_cells("A2:B2")

    stats = [
        ("Total Characters",          len(records)),
        ("Characters in ≥1 Season",   sum(1 for c in records if c["season_count"] > 0)),
        ("Max Seasons by One Char",    max((c["season_count"] for c in records), default=0)),
        ("Characters in All 6 Seasons",sum(1 for c in records if c["season_count"] == 6)),
        ("Characters (No TV Season)",  sum(1 for c in records if c["season_count"] == 0)),
        ("Named Characters",           sum(1 for c in records if c["name"] != "(Unnamed)")),
        ("Unique Cultures",            len({c["culture"] for c in records if c["culture"] != "Unknown"})),
    ]

    for i, (label, value) in enumerate(stats, start=4):
        ws2.cell(row=i, column=1, value=label).font  = label_font
        ws2.cell(row=i, column=2, value=value).font  = Font(size=11)
        ws2.cell(row=i, column=1).fill = summary_fill
        ws2.cell(row=i, column=2).fill = summary_fill

    # Season distribution table
    ws2["A12"] = "Season Appearance Distribution"
    ws2["A12"].font = Font(bold=True, size=12, color="1F3864")

    ws2["A13"] = "Seasons"
    ws2["B13"] = "Characters Count"
    ws2["A13"].font = ws2["B13"].font = Font(bold=True)

    dist = {}
    for c in records:
        dist[c["season_count"]] = dist.get(c["season_count"], 0) + 1

    for row_i, seasons in enumerate(sorted(dist.keys()), start=14):
        ws2.cell(row=row_i, column=1, value=seasons)
        ws2.cell(row=row_i, column=2, value=dist[seasons])

    wb.save(filepath)
    log.info(f"Excel file saved: {filepath}")


# ══════════════════════════════════════════════════════════════════════════════
#  Main
# ══════════════════════════════════════════════════════════════════════════════

def main():
    print("\n" + "═" * 60)
    print("   Q3 – CHARACTERS OF ICE AND FIRE")
    print("═" * 60)

    # a. Fetch all characters
    raw_characters = fetch_all_characters()

    # b. Build structured records with season count
    records = build_character_records(raw_characters)
    log.info(f"Records built: {len(records)}")

    # c. Sort by season appearances (descending)
    sorted_records = sort_by_seasons(records)
    log.info("Characters sorted by number of season appearances.")

    # d. Write to Excel
    write_to_excel(sorted_records, OUT_EXCEL)

    # Console preview – top 10 by season count
    tv_chars = [c for c in sorted_records if c["season_count"] > 0]

    print("\n  ┌─────────────────────────────────────────────────────────────────┐")
    print("  │  TOP 10 CHARACTERS  by TV Season Appearances                    │")
    print("  ├──────┬────────────────────────────────┬─────────┬───────────────┤")
    print(f"  │ {'Rank':<5}│ {'Name':<31}│ {'Seasons':<8}│ {'Played By':<14}│")
    print("  ├──────┼────────────────────────────────┼─────────┼───────────────┤")
    for i, char in enumerate(tv_chars[:10], 1):
        print(
            f"  │ {i:<5}│ {char['name'][:30]:<31}"
            f"│ {char['season_count']:<8}│ {char['playedBy'][:13]:<14}│"
        )
    print("  └──────┴────────────────────────────────┴─────────┴───────────────┘")

    # Summary stats
    print("\n  SUMMARY STATISTICS")
    print("  " + "─" * 40)
    print(f"  Total characters      : {len(sorted_records)}")
    print(f"  In at least 1 season  : {sum(1 for c in sorted_records if c['season_count'] > 0)}")
    print(f"  In all 6 seasons      : {sum(1 for c in sorted_records if c['season_count'] == 6)}")
    print(f"  No TV appearance      : {sum(1 for c in sorted_records if c['season_count'] == 0)}")
    print()
    print(f"  ✔  Done!  Data saved to → {OUT_EXCEL}\n")


if __name__ == "__main__":
    main()
