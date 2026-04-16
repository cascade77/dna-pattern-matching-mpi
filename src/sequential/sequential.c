#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(void)
{
    FILE *genome_file = fopen("../data/chr1_raw.txt", "r");
    if (!genome_file)
    {
        printf("error opening genome file\n");
        return 1;
    }

    fseek(genome_file, 0, SEEK_END);
    long genome_len = ftell(genome_file);
    fseek(genome_file, 0, SEEK_SET);

    char *genome = malloc(genome_len + 1);
    fread(genome, 1, genome_len, genome_file);
    genome[genome_len] = '\0';
    fclose(genome_file);

    FILE *pat_file = fopen("../data/patterns.txt", "r");
    if (!pat_file)
    {
        printf("error opening patterns file\n");
        return 1;
    }

    char patterns[100][256];
    int pat_count = 0;
    while (fgets(patterns[pat_count], 256, pat_file))
    {
        patterns[pat_count][strcspn(patterns[pat_count], "\n")] = '\0';
        if (strlen(patterns[pat_count]) > 0)
            pat_count++;
    }
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(void)
{
    FILE *genome_file = fopen("../data/chr1_raw.txt", "r");
    if (!genome_file)
    {
        printf("error opening genome file\n");
        return 1;
    }

    fseek(genome_file, 0, SEEK_END);
    long genome_len = ftell(genome_file);
    fseek(genome_file, 0, SEEK_SET);

    char *genome = malloc(genome_len + 1);
    fread(genome, 1, genome_len, genome_file);
    genome[genome_len] = '\0';
    fclose(genome_file);

    FILE *pat_file = fopen("../data/patterns.txt", "r");
    if (!pat_file)
    {
        printf("error opening patterns file\n");
        return 1;
    }

    char patterns[100][256];
    int pat_count = 0;
    while (fgets(patterns[pat_count], 256, pat_file))
    {
        patterns[pat_count][strcspn(patterns[pat_count], "\n")] = '\0';
        if (strlen(patterns[pat_count]) > 0)
            pat_count++;
    }
    fclose(pat_file);

    printf("genome length: %ld\n", genome_len);
    printf("patterns loaded: %d\n", pat_count);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    long total_matches = 0;
    for (int p = 0; p < pat_count; p++)
    {
        int plen = strlen(patterns[p]);
        for (long i = 0; i <= genome_len - plen; i++)
        {
            if (memcmp(&genome[i], patterns[p], plen) == 0)
                total_matches++;
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("total matches: %ld\n", total_matches);
    printf("time: %.4f seconds\n", elapsed);

    free(genome);
    return 0;
}
