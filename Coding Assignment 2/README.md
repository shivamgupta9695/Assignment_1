# ❄️ Ice and Fire API Assignment

This repository contains solutions for the **Calsoft Internship Coding Assignment 2026** using Python and REST APIs.

---

# 📌 Questions Covered

## Q1 – Houses of Ice and Fire
- Fetch all houses from API
- Extract house names and regions
- Sort houses alphabetically
- Save data into a text file

Output:
`Q1_houses_output.txt`

---

## Q2 – Books of Ice and Fire
- Fetch all books from API
- Create dictionary:
```python
{
    book_name: [pages, release_date, ISBN, publisher]
}
```
- Export data into CSV file

Output:
`Q2_books_output.csv`

---

## Q3 – Characters of Ice and Fire
- Fetch all characters from API
- Calculate TV season appearances
- Sort by season appearances
- Export data into Excel file

Output:
`Q3_characters_output.xlsx`

---

# 🛠️ Technologies Used

- Python 3
- Requests
- OpenPyXL
- CSV Module

---

# 🌐 API Used

https://anapioficeandfire.com/

Endpoints:
- `/api/houses`
- `/api/books`
- `/api/characters`

---

# 📂 Project Structure

```Coding Assignment 2
├── q1_houses.py
├── q2_books.py
├── q3_characters.py
├── Q1_houses_output.txt
├── Q2_books_output.csv
├── Q3_characters_output.xlsx
├── screenshots_of_output/
└── README.md
```

---

# ▶️ Run Commands

```bash
pip install requests openpyxl
```

Run files:

```bash
python q1_houses.py
python q2_books.py
python q3_characters.py
```

---

# 📷 Screenshots_of_output

Program output screenshots are available inside:

```text
screenshots_of_output/
```

---

# 👨‍💻 Author

**Shivam Gupta**