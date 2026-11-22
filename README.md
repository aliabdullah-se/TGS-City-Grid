# TGS CityGrid - Assignment 4, Phase 1

**Student:** TY_FIND 999
**Roll number:** name, and type record. |
**Course:** Data Structures (Fall 2026)
**Language:** C++17

## Build

Run this exact build command from the project root:

```bash
g++ -std=c++17 -Wall -Icore -Iindex -Iengine engine/*.cpp -o citygrid
```

On Windows with MinGW, this produces `citygrid.exe`.

## Run

```text
citygrid <junctions file> <incidents file> <script file>
```

Example on Windows: `citygrid.exe junctions.txt incidents.txt script.txt`.
The program prints to standard output and does not read keyboard input.

## Class responsibilities

| Component | Responsibility |
|---|---|
| `core/Junction.h` | Junction ID,| `core/Incident.h` | Incident record, status, and priority comparator. |
| `index/AVL.h` | Balanced junction dictionary with insertion, search, in-order and pre-order traversals; LL/RR/LR/RL rotations. |
| `index/BST.h` | Unbalanced, ID-ordered incident registry, all three deletion cases, and memory ownership. |
| `index/HashMap.h` | Fast incident-ID lookup with separate chaining and prime-capacity resizing. |
| `index/MaxHeap.h` | Array-based emergency priority queue, peek, extract-max, remove-by-ID, and count. |
| `engine/CityGrid.h/.cpp` | File loading, parsing, validation, coordination, exact output and diagnostics. |
| `engine/CityGridCommands.cpp` | Interpret and execute all 14 commands while synchronizing structures. |
| `engine/main.cpp` | Check three command-line filenames, execute, return error/success code. |

## Hash collision strategy and resizing

I use **separate chaining** with my own singly linked nodes per bucket. The required hash function is `incidentID % capacity`. Each chain node holds a key and `Incident*`. Insertion checks duplicates. Removal unlinks and deletes only the chain node, never the Incident. The table begins with capacity **11** and immediately resizes after insertion when `size / capacity > 0.75`. The replacement capacity is the smallest prime at least twice the old capacity (11, 23, 47, 97, ...). Every live entry is rehashed. The table never shrinks. Separate chaining does not require tombstones.

## Emergency priority comparator

The highest **severity** is served first (5 beats 4). For equal severities, the **earlier timestamp** wins (smaller number). If both match, the **smaller incident ID** wins. This total ordering is used by the max-heap.

## Incident ownership and safe deletion

`INC_REPORT` or incident-file loading allocates exactly one `Incident` with `new`. The BST stores and **owns** that `Incident*`; the HashMap and MaxHeap only borrow the same pointer. For `INC_CLOSE`, the engine removes the pending heap entry, if any, then removes the hash entry, then asks the BST to delete the record and its object. `PRIORITY_SERVE` first extracts from the heap, then removes the hash entry, then deletes through the BST. This order prevents dangling references. The BST destructor deletes remaining incidents; other structure destructors free their own nodes/arrays without deleting incidents.

## Required time complexities

Let `n` be the size of the relevant structure, and `h` the actual BST height.

| Command | Required time |
|---|---|
| `CITY_ADD` | O(log n) AVL insertion |
| `CITY_FIND` | O(log n) AVL lookup |
| `CITY_LIST` | O(n) AVL in-order |
| `CITY_PREORDER` | O(n) AVL pre-order |
| `INC_REPORT` | O(h + log n) + O(1) average for BST, AVL, and hash |
| `INC_FIND` | O(1) average hash lookup |
| `INC_LIST` | O(n) BST in-order |
| `INC_PREORDER` | O(n) BST pre-order |
| `INC_CLOSE` | O(n) heap search + O(log n) heap repair + O(h) BST + O(1) average hash |
| `REQUEST` | O(1) average hash lookup + O(log n) heap insertion |
| `PRIORITY_NEXT` | O(1) heap peek |
| `PRIORITY_SERVE` | O(log n) heap extraction + O(h) BST + O(1) average hash |
| `PRIORITY_COUNT` | O(1) |
| `HASH_STATS` | O(1) |

Hash resizing takes O(n) when triggered. The BST is **not balanced**; inserting sorted incident IDs can produce a chain with `h = n`, so BST insertion, search and deletion can become O(n). The AVL guarantees O(log n) height. A collision chain may also be O(n) in a pathological hash-table case; average lookup is O(1).

## Test coverage

Each test folder contains `junctions.txt`, `incidents.txt`, `script.txt`, and `expected_output.txt`.

| Test folder | Covered behavior |
|---|---|
| `test01_avl_rotations` | LL rotation, duplicate/missing junctions, sorted list. |
| `test02_avl_balancing` | Empty junction file/tree, LR/RL and RR rebalancing, lists/preorders. |
| `test03_bst_deletion` | Leaf, one-child, two-child and root deletion, successor pointer. |
| `test04_bst_empty_root` | Empty incident file/tree, last record, root deletion, re-reporting. |
| `test05_hash_collision` | Same-bucket IDs, lookup after collision deletion, re-reporting. |
| `test06_hash_resize` | 11-to-23 resize on ninth insertion, duplicates and deletion. |
| `test07_heap_priority` | Severity/timestamp/ID tie-breaks, duplicate requests, pending cancellation. |
| `test08_heap_edge_cases` | Empty heap, equal-priority ties, non-top cancellation, draining. |
| `test09_loader_validation` | All file rejection reasons, blank lines, Windows CRLF, physical line numbers. |
| `test10_worked_example` | Exact official assignment example; synchronization and unknown command. |
| `test11_command_errors` | Unknown/wrong-argument commands, all invalid-field diagnostics, state unchanged on failure, continuing after errors. |

## Scope

Phase 1 covers junctions, incidents, hash lookup and emergency priority. Road graph edges, routing, dispatch and network analysis are outside this assignment phase.
