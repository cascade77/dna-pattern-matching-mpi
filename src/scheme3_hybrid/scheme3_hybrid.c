#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    char patterns[100][256];
    int pat_count = 0;
    long genome_len = 0;
    char *genome = NULL;

    // --- THIS IS WHERE WE USE YOUR FILE ---
    const char* genome_path = "/hdd2/sines/mdl/saleha.sines/pdc_project/data/chr1_diseased.txt";
    const char* pattern_path = "/hdd2/sines/mdl/saleha.sines/pdc_project/data/patterns.txt";

    if (rank == 0) {
        // Open the file you mentioned: chr1_diseased.txt
        FILE *genome_file = fopen(genome_path, "r");
        if (!genome_file) { printf("Error: File not found!\n"); MPI_Abort(MPI_COMM_WORLD, 1); }
        
        // Find how big the file is
        fseek(genome_file, 0, SEEK_END);
        genome_len = ftell(genome_file);
        fseek(genome_file, 0, SEEK_SET);
        
        // Load the data into memory
        genome = malloc(genome_len + 1);
        fread(genome, 1, genome_len, genome_file);
        genome[genome_len] = '\0';
        fclose(genome_file);

        // Load the 12 patterns
        FILE *pat_file = fopen(pattern_path, "r");
        while (fgets(patterns[pat_count], 256, pat_file)) {
            patterns[pat_count][strcspn(patterns[pat_count], "\n")] = '\0';
            if (strlen(patterns[pat_count]) > 0) pat_count++;
        }
        fclose(pat_file);
    }

    // Share the metadata with all other processes
    MPI_Bcast(&genome_len, 1, MPI_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&pat_count, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(patterns, 100 * 256, MPI_CHAR, 0, MPI_COMM_WORLD);

    // HYBRID LOGIC: Every process gets a slice of DNA
    long chunk = genome_len / size;
    long start_idx = rank * chunk;
    long end_idx = (rank == size - 1) ? genome_len : start_idx + chunk;
    long overlap = 255; 
    long recv_len = (end_idx + overlap < genome_len) ? (end_idx + overlap) - start_idx : genome_len - start_idx;

    char *local_dna = malloc(recv_len + 1);
    if (rank != 0) genome = malloc(genome_len + 1);
    
    MPI_Bcast(genome, genome_len, MPI_CHAR, 0, MPI_COMM_WORLD);
    memcpy(local_dna, &genome[start_idx], recv_len);
    local_dna[recv_len] = '\0';

    double t_start = MPI_Wtime();
    long local_matches = 0;

    // Search all 12 patterns in your specific slice of the diseased genome
    for (int p = 0; p < pat_count; p++) {
        int plen = strlen(patterns[p]);
        for (long i = 0; i <= (end_idx - start_idx); i++) {
            if (memcmp(&local_dna[i], patterns[p], plen) == 0) {
                local_matches++;
            }
        }
    }

    double t_end = MPI_Wtime();
    long total_matches = 0;
    MPI_Reduce(&local_matches, &total_matches, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    
    double elapsed = t_end - t_start;
    double max_elapsed;
    MPI_Reduce(&elapsed, &max_elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("\n--- Scheme 3 (Hybrid) Results ---\n");
        printf("Processes: %d\n", size);
        printf("Total Matches Found: %ld (Expected: 791)\n", total_matches);
        printf("Time Taken: %.4f seconds\n", max_elapsed);
    }

    free(local_dna);
    free(genome);
    MPI_Finalize();
    return 0;
}
