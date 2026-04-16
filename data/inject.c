#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void)
{
    FILE *f = fopen("chr1_raw.txt", "r");
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *genome = malloc(len + 1);
    fread(genome, 1, len, f);
    genome[len] = '\0';
    fclose(f);

    char *patterns[] = {"CAG", "CAG", "CGG", "CTG", "GAA", "CAG", "CGG", "CAA"};
    int   repeats[]  = {50, 60, 55, 45, 50, 40, 70, 50};
    long  positions[] = {50000000, 100000000, 150000000, 200000000, 30000000, 80000000, 180000000, 220000000};
    int n = 8;

    for (int i = 0; i < n; i++)
    {
        int plen = strlen(patterns[i]);
        for (int r = 0; r < repeats[i]; r++)
            memcpy(&genome[positions[i] + r * plen], patterns[i], plen);
    }

    FILE *out = fopen("chr1_diseased.txt", "w");
    fwrite(genome, 1, len, out);
    fclose(out);
    free(genome);
    printf("done\n");
    return 0;
}
