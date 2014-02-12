/* http://shootout.alioth.debian.org/u32q/benchmark.php?test=fannkuch&lang=all */
/* adapted from C# Mono sample */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int n = 7;

int
fannkuch(int n)
{
    int check = 0;
    int *perm;
    int *perm1;
    int *count;
    int *maxPerm;
    int maxFlipsCount = 0;
    int m = n - 1;
    int i, j, r;
    int flipsCount = 0;
    int k, k2;
    int perm0;
    int temp;

    perm = malloc(n * sizeof(int));
    memset(perm, 0, n * sizeof(int));

    perm1 = malloc(n * sizeof(int));
    memset(perm1, 0, n * sizeof(int));

    count = malloc(n * sizeof(int));
    memset(count, 0, n * sizeof(int));

    maxPerm = malloc(n * sizeof(int));
    memset(maxPerm, 0, n * sizeof(int));

    for (i = 0; i < n; ++i)
	perm1[i] = i;
    r = n;

    while (1) {
	if (check < 30) {
	    for(j = 0; j < n; j++)
		printf("%d", perm1[j] + 1);
	    printf("\n");
	    ++check;
	}

	while (r != 1) {
	    count[r - 1] = r;
	    --r;
	}
	if (!(perm1[0] == 0 || perm1[m] == m)) {
	    for(j = 0; j < n; j++)
		perm[j] = perm1[j];
	    flipsCount = 0;

            while (!((k = perm[0]) == 0)) {
		k2 = (k + 1) >> 1;
		for(j = 0; j < k2; j++) {
		    temp = perm[j];
		    perm[j] = perm[k - j];
		    perm[k - j] = temp;
		}
		++flipsCount;
	    }

	    if (flipsCount > maxFlipsCount) {
		maxFlipsCount = flipsCount;
		for(j = 0; j < n; j++)
		    maxPerm[j] = perm1[j];
	    }
	}

	// Use incremental change to generate another permutation
	while (1) {
	    if (r == n)
		return maxFlipsCount;
	    perm0 = perm1[0];
	    i = 0;
	    while (i < r) {
		j = i + 1;
		perm1[i] = perm1[j];
		i = j;
	    }
	    perm1[r] = perm0;

	    count[r] = count[r] - 1;
	    if (count[r] > 0)
		break;
	    ++r;
	}
    }
}

int
main(int argc, char *argv[])
{
    printf("Pfannkuchen(%d) = %d\n", n, fannkuch(n));

    return (0);
}
