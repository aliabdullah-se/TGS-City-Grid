# TGS CityGrid

**CityGrid Operations Center — Data Structures Assignment 4, Phase 1**

A command-driven C++17 emergency incident management system that maintains city junctions, registers incidents, and processes emergency requests by priority. Built with four manually implemented data structures—**AVL Tree, Binary Search Tree, Hash Table, and Binary Max-Heap**—without STL containers.

![Language](https://img.shields.io/badge/Language-C%2B%2B17-blue) ![Project](https://img.shields.io/badge/Project-DSA%20Phase%201-374151) ![Implementation](https://img.shields.io/badge/Data%20Structures-Manual-2f855a)

| | |
|---|---|
| **Student** | Ali Abdullah |
| **Roll number** | 25L-3022 |
| **University** | FAST National University of Computer and Emerging Sciences, Lahore |
| **Program** | BS Software Engineering |
| **Course / term** | Data Structures · Fall 2026 |
| **Assignment** | Assignment 4 — TGS CityGrid, Phase 1 |

## Overview

CityGrid simulates an emergency reporting desk. Operators can register city junctions, report incidents, search active records, request priority handling, inspect the next emergency, serve incidents, and close records. The system reads two data files and then a script of commands; it produces a strictly formatted console transcript. This phase does **not** implement roads, routing, graph traversal, or vehicle dispatch.

## Key capabilities

- **Junction directory:** add, find, list in sorted order, and inspect AVL preorder (including LL, RR, LR, RL rotations).
- **Incident registry:** report, find, list, and permanently close incidents using a plain BST with all three deletion cases.
- **Fast lookups:** manually chained incident hash table with deterministic hashing and automatic prime-capacity resizing.
- **Priority handling:** manually implemented max-heap with exact severity → timestamp → ID tie-breaking.
- **State synchronization:** incident objects are shared by pointer across BST, hash index, and heap without copying the records.
- **Robust input processing:** exact validation order, per-line diagnostics, continued execution after bad commands, and Windows CRLF support.

## Repository structure

```text
TGS-City-Grid/
├── core/
│   ├── Junction.h               # Junction record
│   └── Incident.h               # Incident record + comparator
├── index/
│   ├── AVL.h                    # Self-balancing junction directory
│   ├── BST.h                    # Incident registry and ownership
│   ├── HashMap.h                # Separate-chaining incident index
│   └── MaxHeap.h                # Emergency priority queue
├── engine/
│   ├── CityGrid.h               # CityGrid class interface
│   ├── CityGrid.cpp             # File loading and validation
│   ├── CityGridCommands.cpp     # Script commands and coordination
│   └── main.cpp                 # Program entry point
├── tests/
│   └── testNN_topic/           # Four .txt files in each test folder
└── README.md
```

All data structures are templates implemented in their header files. Parsing, validation and output are confined to `engine/`.

## Build and run

**Requirements:** a C++17 compiler (`g++` / MinGW on Windows), a terminal, and no external libraries.

From the repository root, use the **exact assignment build command**:

```bash
g++ -std=c++17 -Wall -Icore -Iindex -Iengine engine/*.cpp -o citygrid
```

Run with exactly three file arguments:

```text
citygrid <junctions file> <incidents file> <script file>
```

On Windows PowerShell, for example:

```powershell
.\citygrid.exe tests\test10_worked_example\junctions.txt tests\test10_worked_example\incidents.txt tests\test10_worked_example\script.txt
```

All output goes to standard output; there is no keyboard input. The program returns exit code `1` for invalid argument count or input-file opening failure, and `0` after normal execution.

## Input files

**Junctions (`junctions.txt`):** `junctionID|name|type`. Types: `HOSPITAL`, `FIRE`, `POLICE`, `DEPOT`, `PLAIN`. Example:

```text
1|CentralSquare|PLAIN
2|CityHospital|HOSPITAL
3|NorthFireHall|FIRE
```

**Incidents (`incidents.txt`):** `incidentID|junctionID|category|severity|timestamp`. Categories: `MEDICAL`, `FIRE`, `CRIME`; severity ranges from `1` to `5`; timestamp ranges from `0` to `999999999`. Example:

```text
101|1|MEDICAL|5|1000
102|1|FIRE|3|1005
103|2|MEDICAL|5|900
```

**Commands (`script.txt`):** one command per nonblank line. Leading `#` comments are ignored, and keywords are uppercase and case-sensitive.

```text
CITY_FIND 2
INC_FIND 101
REQUEST 101
PRIORITY_NEXT
PRIORITY_COUNT
```

IDs are decimal integers in the range `1` to `999999999`. Numeric tokens may use leading zeros but may not include a sign, internal spaces or more than nine digits. Validation checks happen in the assignment's prescribed order; the first failure determines the diagnostic.

## Command reference

| Command | Behavior |
|---|---|
| `CITY_ADD <id> <name> <type>` | Insert junction into AVL. |
| `CITY_FIND <id>` | Search junction by ID. |
| `CITY_LIST` | List junctions in ascending ID order. |
| `CITY_PREORDER` | Show AVL preorder for rotation verification. |
| `INC_REPORT <id> <junction> <category> <severity> <time>` | Register a new `REPORTED` incident. |
| `INC_FIND <id>` | Find an incident using the hash table. |
| `INC_LIST` | List incidents in ascending ID order. |
| `INC_PREORDER` | Show BST preorder to verify deletion. |
| `INC_CLOSE <id>` | Permanently delete an incident from all structures. |
| `REQUEST <id>` | Set status to `PENDING` and insert into heap. |
| `PRIORITY_NEXT` | View highest-priority pending incident. |
| `PRIORITY_SERVE` | Extract highest priority and permanently close it. |
| `PRIORITY_COUNT` | Count pending incidents. |
| `HASH_STATS` | Report hash table capacity and active entries. |

There are exactly two stored statuses: `REPORTED` and `PENDING`. Closing is permanent deletion, not a third stored status. The script echo format is `> <command>`, while invalid commands produce `ERROR line <n>: <reason>` without halting later commands.

## Design decisions

### 1. AVL junction directory

An AVL keeps junction operations logarithmic by maintaining node heights and applying LL/RR/LR/RL rotations as needed. In-order traversal lists IDs in ascending order; preorder exposes tree structure for correctness checks. Junctions are stored by value and never removed in this phase.

### 2. Plain BST incident registry

Incidents are stored by ID without balancing, as explicitly required. Deletion handles leaf, single-child, and two-child nodes. Two-child deletion detaches the in-order successor and **moves its `Incident*`**, never copying the incident object. Sorted insertion can create a chain and degrade performance to `O(n)`.

### 3. Separate-chaining hash table

The required hash function is `incidentID % capacity`. Collisions use manually linked nodes per bucket, allowing deletion without tombstones. The table starts at capacity **11**, then expands to the smallest prime at least twice the old capacity when the active load factor exceeds **0.75** after insertion: `11 → 23 → 47 → 97 → ...`. Resizing rehashes active entries; the table never shrinks.

### 4. Array-based binary max-heap

A zero-indexed dynamic array starts at capacity **16** and doubles when full. The parent of index `i` is `(i - 1) / 2`; its children are `2i + 1` and `2i + 2`. Insertion uses sift-up, extraction uses sift-down, and removal by ID uses a linear search followed by heap repair.

Priority is determined in this order: **higher severity**, then **earlier timestamp**, then **smaller incident ID**. The unique incident ID ensures that no two active records tie completely.

### Record ownership and safety

Each incident is allocated **once** with `new`. The **BST owns and deletes** the `Incident` object. The HashMap and MaxHeap store borrowed pointers; neither deletes the incident. For `INC_CLOSE`, the engine removes the heap reference (if pending), then the hash reference, and **finally deletes via BST**. For `PRIORITY_SERVE`, the heap extracts first, the hash removes next, and the BST deletes last. Structure destructors free their own remaining allocations before exit.

## Required time complexity

Let `n` denote the number of items in the relevant structure, and `h` the BST height.

| Operation | Complexity |
|---|---|
| `CITY_ADD`, `CITY_FIND` | `O(log n)` |
| `CITY_LIST`, `CITY_PREORDER` | `O(n)` |
| `INC_REPORT` | `O(h + log n) + O(1)` average hash operations |
| `INC_FIND`, `REQUEST` lookup | `O(1)` average hash lookup |
| `INC_LIST`, `INC_PREORDER` | `O(n)` |
| `INC_CLOSE` | `O(n)` heap search + `O(log n)` repair + `O(h)` BST + `O(1)` average hash |
| `REQUEST` heap insert | `O(log n)` |
| `PRIORITY_NEXT`, `PRIORITY_COUNT` | `O(1)` |
| `PRIORITY_SERVE` | `O(log n)` heap + `O(h)` BST + `O(1)` average hash |
| `HASH_STATS` | `O(1)` |
| Hash rehash | `O(n)` only when triggered |

A plain BST can reach height `h = n` on sorted incident IDs. Hash lookup is average constant-time, but a pathological collision chain can be linear. Resizing is amortized across hash insertions.

## Tests and verification

Every test folder contains `junctions.txt`, `incidents.txt`, `script.txt`, and `expected_output.txt`. Output is compared line by line with trailing spaces ignored by the assignment evaluator.

| Test | Coverage |
|---|---|
| `test01_avl_rotations` | LL rotation, ordered listing, duplicate/missing junctions. |
| `test02_avl_balancing` | Empty tree, LR/RL/RR rebalancing, AVL preorders. |
| `test03_bst_deletion` | Leaf, one-child, two-children and root deletion. |
| `test04_bst_empty_root` | Empty BST, root and last-node deletion, re-reporting. |
| `test05_hash_collision` | Colliding IDs, removal and re-reporting. |
| `test06_hash_resize` | Growth `11 → 23`, duplicates and lookup after deletion. |
| `test07_heap_priority` | Severity/time/ID ties, duplicate request, pending cancellation. |
| `test08_heap_edge_cases` | Empty heap, tie-breaking and non-top cancellation. |
| `test09_loader_validation` | Invalid fields, duplicates, physical line numbers, blank lines and Windows CRLF. |
| `test10_worked_example` | Official assignment example and cross-structure integration. |
| `test11_command_errors` | Unknown commands, argument/field errors, continued execution. |

To run a single fixture in PowerShell, use its three input files and compare the output with `expected_output.txt`. The project has at least two targeted tests for each of Modules 1–4.

## Submission requirements

The assignment expects a single ZIP named **`25L-3022_DS_Assignment4.zip`** containing `core/`, `index/`, `engine/`, `tests/`, and this `README.md` at its root. **Do not include** compiled executables, `.o` / `.obj` files, `.git/`, or other build artifacts.

## Author

**Ali Abdullah** · BS Software Engineering · FAST-NUCES Lahore · Roll No. **25L-3022**

*Academic implementation for Data Structures Assignment 4 (Fall 2026).*
