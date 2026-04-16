# DNA Pattern Matching for Neurological Disease Detection

Parallel string search across a synthetic human chromosome (chr1) to detect
disease-associated mutation patterns. Three MPI-based parallelization schemes
are implemented, benchmarked, and compared on an HPC cluster.

This was a group lab mid-exam project for the Parallel and Distributed Computing (PDC) course.

---

## What This Does

We take a 238MB raw genome file and 12 known disease-related DNA patterns, then
search for all occurrences of each pattern across the genome. The sequential
baseline does this in ~10 seconds on a single process. The goal is to make it
faster through parallelism and see how well each strategy scales.

---

## Parallelization Schemes

**Scheme 1 — Data Decomposition**
The genome is split across all MPI processes. Each process searches its chunk
for all 12 patterns. Rank 0 reads and broadcasts the patterns, then scatters
the genome.

**Scheme 2 — Task Decomposition**
The 12 patterns are distributed across processes. Every process holds the full
genome but only searches for its assigned patterns. More of a task-parallel approach.

**Scheme 3 — Hybrid**
Combines both strategies. Genome is split AND patterns are distributed, so each
process works on a subset of both. Best overall performance.

---

## Benchmark Results

All times measured on the university HPC cluster using OpenMPI.

| Processes | Scheme 1 (s) | Scheme 2 (s) | Scheme 3 (s) |
|-----------|-------------|-------------|-------------|
| 1         | 9.93        | —           | —           |
| 2         | 5.44        | 7.04        | 6.23        |
| 4         | 3.05        | 3.84        | 2.72        |
| 8         | 2.03        | 2.55        | 1.79        |
| 16        | 1.20        | 1.75        | 0.90        |

Scheme 3 wins at every process count. At 16 processes it hits **0.90s** vs the
sequential baseline of **9.93s** — roughly 11x speedup.

---

## Repo Structure

```
dna-pattern-matching-mpi/
├── README.md
├── .gitignore
├── report.pdf
├── data/
│   ├── patterns.txt
│   ├── inject.c
│   └── inject_mutations.py
├── src/
│   ├── scheme1_data.c
│   ├── scheme1_diseased.c
│   ├── scheme2_task.c
│   └── scheme3_hybrid.c
└── sequential/
    ├── sequential.c
    └── sequential_diseased.c
```

---

## Data

The genome files (`chr1.fa`, `chr1_raw.txt`, `chr1_diseased.txt`) are not
included because they're ~238MB each. You can get the raw FASTA from NCBI
(chromosome 1, hg38), strip headers to get `chr1_raw.txt`, then generate the
diseased version by running:

```bash
gcc -o inject data/inject.c
./inject chr1_raw.txt chr1_diseased.txt data/patterns.txt
```

This injects synthetic mutations at known positions, producing 791 total pattern matches.

---

## How to Compile and Run

Requires OpenMPI installed.

```bash
# Scheme 1 example
mpicc -o scheme1 src/scheme1_data/scheme1_data.c
mpirun -np 4 ./scheme1

# Scheme 3 example
mpicc -o scheme3 src/scheme3_hybrid/scheme3_hybrid.c
mpirun -np 8 ./scheme3
```

File paths inside the `.c` files point to HPC scratch paths — update them
to your local paths before compiling.

---

## Environment

- HPC Cluster: university cluster (`master` node)
- MPI: OpenMPI
- Language: C (C99)
