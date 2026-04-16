import random

with open("chr1_raw.txt", "r") as f:
    genome = f.read().strip()

genome = list(genome)

mutations = [
    ("CAG" * 50, 50000000),
    ("CAG" * 60, 100000000),
    ("CGG" * 55, 150000000),
    ("CTG" * 45, 200000000),
    ("GAA" * 50, 30000000),
    ("CAG" * 40, 80000000),
    ("CGG" * 70, 180000000),
    ("CAA" * 50, 220000000),
]

for pattern, pos in mutations:
    genome[pos:pos+len(pattern)] = list(pattern)

with open("chr1_diseased.txt", "w") as f:
    f.write("".join(genome))

print("done, diseased genome written to chr1_diseased.txt")
cat > inject_mutations.py << 'EOF'
import random

with open("chr1_raw.txt", "r") as f:
    genome = f.read().strip()

genome = list(genome)

mutations = [
    ("CAG" * 50, 50000000),
    ("CAG" * 60, 100000000),
    ("CGG" * 55, 150000000),
    ("CTG" * 45, 200000000),
    ("GAA" * 50, 30000000),
    ("CAG" * 40, 80000000),
    ("CGG" * 70, 180000000),
    ("CAA" * 50, 220000000),
]

for pattern, pos in mutations:
    genome[pos:pos+len(pattern)] = list(pattern)

with open("chr1_diseased.txt", "w") as f:
    f.write("".join(genome))

print("done, diseased genome written to chr1_diseased.txt")

python3 inject_mutations.py

