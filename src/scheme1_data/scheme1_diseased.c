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

    if (rank == 0)
    {
        FILE *genome_file = fopen("../data/chr1_diseased.txt", "r");
        fseek(genome_file, 0, SEEK_END);
        genome_len = ftell(genome_file);
        fseek(genome_file, 0, SEEK_SET);
        genome = malloc(genome_len + 1);
        fread(genome, 1, genome_len, genome_file);
        genome[genome_len] = '\0';
        fclose(genome_file);

        FILE *pat_file = fopen("../data/patterns.txt", "r");
        while (fgets(patterns[pat_count], 256, pat_file))
        {
            patterns[pat_count][strcspn(patterns[pat_count], "\n")] = '\0';
            if (strlen(patterns[pat_count]) > 0)
                pat_count++;
        }
        fclose(pat_file);
    }

    MPI_Bcast(&genome_len, 1, MPI_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&pat_count, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(patterns, 100 * 256, MPI_CHAR, 0, MPI_COMM_WORLD);

    int max_plen = 0;
    for (int p = 0; p < pat_count; p++)
    {
        int l = strlen(patterns[p]);
        if (l > max_plen) max_plen = l;
    }

    long chunk = genome_len / size;
    long start_idx = rank * chunk;
    long end_idx = (rank == size - 1) ? genome_len : start_idx + chunk;
    long overlap = max_plen - 1;
    long recv_start = start_idx;
    long recv_end = (end_idx + overlap < genome_len) ? end_idx + overlap : genome_len;
    long recv_len = recv_end - recv_start;

    char *local = malloc(recv_len + 1);

    if (rank == 0)
    {
        memcpy(local, genome, recv_len);
        local[recv_len] = '\0';
        for (int r = 1; r < size; r++)
        {
            long rs = r * chunk;
            long re = (r == size - 1) ? genome_len : rs + chunk;
            long re_ov = (re + overlap < genome_len) ? re + overlap : genome_len;
            long rl = re_ov - rs;
            MPI_Send(&genome[rs], rl, MPI_CHAR, r, 0, MPI_COMM_WORLD);
        }
        free(genome);
    }
    else
    {
        MPI_Recv(local, recv_len, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        local[recv_len] = '\0';
    }

    double t_start = MPI_Wtime();

    long local_matches = 0;
    long search_len = end_idx - start_idx;
    for (int p = 0; p < pat_count; p++)
    {
        int plen = strlen(patterns[p]);
        for (long i = 0; i <= recv_len - plen; i++)
        {
            if (i + plen > search_len + overlap) break;
            if (memcmp(&local[i], patterns[p], plen) == 0)
                local_matches++;
        }
    }

    double t_end = MPI_Wtime();

    long total_matches = 0;
    MPI_Reduce(&local_matches, &total_matches, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    double elapsed = t_end - t_start;
    double max_elapsed;
    MPI_Reduce(&elapsed, &max_elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("processes: %d\n", size);
        printf("total matches: %ld\n", total_matches);
        printf("time: %.4f seconds\n", max_elapsed);
    }

    free(local);
    MPI_Finalize();
    return 0;
}
