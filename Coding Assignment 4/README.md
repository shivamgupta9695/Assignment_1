# Assignment 4

This repository contains solutions for Assignment 4 based on Python programming and Project Euler problems.

---

# Q1 — Project Euler Problem 68 (Magic 5-gon Ring)

## Problem Statement
Construct a magic 5-gon ring using the numbers 1 to 10 such that:

- Each line contains 3 numbers
- The sum of every line is equal
- The arrangement forms a valid magic 5-gon ring
- The final answer is formed by concatenating the groups clockwise
- The sequence must start from the group with the smallest external node
- Find the maximum 16-digit string

---

## Approach
- Used permutations to generate possible inner ring arrangements
- Computed outer ring values mathematically
- Eliminated invalid combinations early
- Generated valid 16-digit strings
- Selected the maximum valid string

---

## Concepts Used
- Permutations
- Mathematical Optimization
- String Manipulation
- Search Optimization

---

## Output
```text
6531031914842725
```

---

# Q2 — Project Euler Problem 318 (2011 Nines)

## Problem Statement
For expressions of the form:

```text
(sqrt(p) + sqrt(q))^(2n)
```

determine the minimum value of `n` such that the fractional part starts with at least 2011 consecutive nines.

Compute:

```text
Σ N(p, q) for p + q ≤ 2011
```

---

## Approach
- Applied mathematical transformation and logarithmic analysis
- Used the identity:

```text
(sqrt(p)+sqrt(q))^(2n) + (sqrt(q)-sqrt(p))^(2n)
```

which is always an integer

- Derived the formula using logarithms
- Iterated efficiently through all valid `(p, q)` pairs

---

## Concepts Used
- Mathematics
- Logarithms
- Number Theory
- Optimization

---

## Output
```text
709313889
```

---

# Q3 — Python Command Executor

## Problem Statement
Develop a Python utility that:

- Executes multiple shell commands
- Removes duplicate commands
- Captures command output and errors
- Handles invalid commands
- Handles timeout scenarios
- Displays structured JSON output

---

## Features
- Duplicate command removal
- Timeout handling
- Exception handling
- Structured JSON response
- Modular and reusable code
- Clean and maintainable implementation

---

## Technologies Used
- Python
- subprocess module
- json module

---

## Sample Commands
```python
commands = [
    "dir",
    "cd",
    "whoami",
    "hostname",
    "invalid_command",
    "echo Hello World"
]
```

---

## Sample Output Structure
```json
[
    {
        "dir": {
            "output": "...",
            "error": "",
            "status": "success"
        }
    }
]
```

---

# Folder Structure

```text
Coding Assignment 4/
│
├── Magic_5_gon_RingQ_68.py
├── 2011_NinesQ_318.py
├── Q1_python_execute_Commands.py
├── outputs/
│   ├── Q1_Output.png
│   ├── Q2_Output.png
│   └── Q3_Output.png
└── README.md
```

---

# How to Run

## Run Q1
```bash
python Magic_5_gon_RingQ_68.py
```

## Run Q2
```bash
python 2011_NinesQ_318.py
```

## Run Q3
```bash
python Q1_python_execute_Commands.py
```

---

# Requirements

- Python 3.x
- VS Code / Terminal

---

# Author

Shivam Gupta