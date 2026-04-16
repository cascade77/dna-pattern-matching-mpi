#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    char patterns[100][256];
    int pat_count = 0;
    long genome_len = 0;
    char *genome = NULL;

    // ---------------- RANK 0 READS FILES ----------------
    if (rank == 0)
    {
        FILE *genome_file = fopen("/hdd2/sines/mdl/saleha.sines/pdc_project/data/chr1_diseased.txt", "r");
        if (!genome_file)
        {
            printf("error opening genome file\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        fseek(genome_file, 0, SEEK_END);
        genome_len = ftell(genome_file);
        fseek(genome_file, 0, SEEK_SET);

        genome = malloc(genome_len + 1);
        fread(genome, 1, genome_len, genome_file);
        genome[genome_len] = '\0';
        fclose(genome_file);

        FILE *pat_file = fopen("/hdd2/sines/mdl/saleha.sines/pdc_project/data/patterns.txt", "r");
        if (!pat_file)
        {
            printf("error opening patterns file\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        while (fgets(patterns[pat_count], 256, pat_file))
        {
            patterns[pat_count][strcspn(patterns[pat_count], "\n")] = '\0';
            if (strlen(patterns[pat_count]) > 0)
                pat_count++;
        }
        fclose(pat_file);
    }

    // ---------------- BROADCAST METADATA ----------------
    MPI_Bcast(&genome_len, 1, MPI_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&pat_count, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // ---------------- ALLOCATE GENOME ON ALL PROCESSES ----------------
    if (rank != 0)
        genome = malloc(genome_len + 1);

    // ---------------- BROADCAST DATA ----------------
    MPI_Bcast(genome, genome_len, MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Bcast(patterns, 100 * 256, MPI_CHAR, 0, MPI_COMM_WORLD);

    // ---------------- BALANCED PATTERN DISTRIBUTION ----------------
    int base = pat_count / size;
    int remainder = pat_count % size;

    int start, end;

    if (rank < remainder)
    {
        start = rank * (base + 1);
        end = start + (base + 1);
    }
    else
    {
        start = rank * base + remainder;
        end = start + base;
    }

    // ---------------- TIMING START ----------------
    double t_start = MPI_Wtime();

    long local_matches = 0;

    // ---------------- NAIVE MATCHING ----------------
    for (int p = start; p < end; p++)
    {
        int plen = strlen(patterns[p]);

        for (long i = 0; i <= genome_len - plen; i++)
        {
            if (memcmp(&genome[i], patterns[p], plen) == 0)
                local_matches++;
        }
    }

    double t_end = MPI_Wtime();

    // ---------------- REDUCE RESULTS ----------------
    long total_matches = 0;
    MPI_Reduce(&local_matches, &total_matches, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    double elapsed = t_end - t_start;
    double max_elapsed;

    MPI_Reduce(&elapsed, &max_elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    // ---------------- OUTPUT ----------------
    if (rank == 0)
    {
        printf("processes: %d\n", size);
        printf("total matches: %ld\n", total_matches);
        printf("time: %.4f seconds\n", max_elapsed);
    }

    free(genome);
    MPI_Finalize();
    return 0;
}
