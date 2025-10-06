# Boolean Function Minimization (Project 2)

This program implements Boolean function minimization using the Quine-McCluskey algorithm and Petrick's method.

## Project Overview

- **Input**: Single-output Boolean function in PLA format (with don't cares)
- **Algorithm**: 
  1. Quine-McCluskey algorithm → find all Prime Implicants (PIs)
  2. Petrick's Algorithm → find minimal SOP cover(s)
- **Output**: Minimized PLA format with statistics

## Quick Start

### Build
```bash
make
```

### Run
```bash
./minimize <input.pla> <output.pla>
```

### Example
```bash
make
./minimize pla_files/test1.pla output.pla
```

### Clean
```bash
make clean
```

## PLA Format

Supported PLA format:
- `.i N` - number of inputs
- `.o M` - number of outputs (must be 1)
- `.ilb v1 v2 ...` - input variable names
- `.ob out1 ...` - output names
- `.p K` - number of product terms
- Product terms: `<input_cube> <output>`
  - `0` / `1` / `-` (don't care) in input cube
  - `1` / `-` in output (where `-` means don't care output)
- `.e` - end marker

### Example
```
.i 4
.o 1
.ilb a b c d
.ob f
.p 5
0000 1
0001 1
0010 -
0011 1
1111 1
.e
```

## Algorithm Details

### Quine-McCluskey
1. Group minterms by number of 1s
2. Iteratively combine adjacent terms (differ by 1 bit)
3. Mark prime implicants (cannot be combined further)

### Petrick's Method
1. Build Prime Implicant chart
2. Find essential PIs
3. Solve covering problem using Boolean algebra
4. Select minimal cost solution(s)

## Output Format

Output PLA contains:
- Minimized product terms
- Comment with statistics:
  - Number of product terms
  - Total number of literals

## Project Structure
```
DSD_Project_02/
├── src/
│   ├── main.cpp              # Entry point
│   ├── PlaParser.cpp/h       # PLA file parser
│   ├── QuineMcCluskey.cpp/h  # QM algorithm (TODO)
│   └── Petrick.cpp/h         # Petrick's algorithm (TODO)
├── pla_files/                # Test PLA files
│   ├── test1.pla
│   ├── test2.pla
│   └── input.pla
├── Makefile
└── README.md
```

## Implementation Status

- [x] PLA parser
- [x] Project structure
- [x] Makefile
- [ ] Quine-McCluskey algorithm
- [ ] Petrick's algorithm
- [ ] PLA output writer

## Notes

This is Project 2 for Digital System Design course, focusing on Boolean function minimization.
