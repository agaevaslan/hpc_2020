#include <omp.h>
#include <stdio.h>

int n = 522222;

int isPrime(int n_)
{
    int i;
    for (i=2; i<=(n_/2); i++)
        if ((n_ % i) == 0) return 0;
    return 1;
}

void main(int argc, char *argv[])
{
    int i;
    // #ifdef NUM   
    //     printf("%d", NUM);
    //     omp_set_num_threads(NUM);
    // #else
    //     omp_set_num_threads(4);
    // #endif
    if (omp_get_max_threads() < 4)
    {
        printf("мало нитей");
        omp_set_num_threads(omp_get_max_threads());
        // return (0);
    }
    else
        omp_set_num_threads(4);

    int primes[n];
    int primesNum = 0;
    #pragma omp parallel for schedule(dynamic) shared(primesNum)
    for (i = 0; i < n; i++)
    {
        if (isPrime(i)) {
            // printf("%d: %d \n",i, isPrime(i));
//            printf("%d\n",i);
            primes[primesNum] = i;
            #pragma omp atomic
            primesNum += 1;
        }
    }
    for (int j = 0; j < primesNum; j++) {
//        if (!isPrime(primes[j])) {
//            printf("%d, %d\n", primes[j], isPrime(primes[j]));
//        }
        printf("%d\n", primes[j]);
    }
    // return (0);
}

// main()


// USAGE
// gcc -fopenmp lab6.c
// ./a.out
