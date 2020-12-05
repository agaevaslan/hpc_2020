#include <omp.h>
#include <stdio.h>

//int N = 238942;
int N = 10;
int A[100]; //последовательность чисел a0…a99


//void main(int argc, char *argv[])
//{
//    int i;
//
//    if (omp_get_max_threads() < 4)
//    {
//        printf("мало нитей");
//        omp_set_num_threads(omp_get_max_threads());
//    }
//    else
//        omp_set_num_threads(4);
//
//    double x[N], y[N];
//    x[0] = 0;
//    #pragma omp parallel
//    for (i = 1; i < N; i++)
//    {
//        x[i] = x[i - 1] * x[i - 1];
//        y[i] = x[i] / y[i - 1];
//    }
//    y[0] = x[N - 1];
//}


void main(int argc, char *argv[]) {


    if (omp_get_max_threads() < 4) {
        printf("мало нитей");
        omp_set_num_threads(omp_get_max_threads());
    } else
        omp_set_num_threads(4);

    double x[N], y[N];
    x[0] = 1.01;
    int i;
#pragma omp parallel shared(x) private(i)
    {
        for (i = 0; i < N; i++) {
            x[i + 1] = x[i] * x[i];
        }
    }

    y[0] = x[N - 1];

    int j;
#pragma omp parallel shared(y) private(j)
    {
        for (j = 1; j < N; j++) {
            y[j] = x[j] / y[j - 1];
        }
    }


    for (int i = 0; i < N; i++) {
        printf("i: %i, x: %f, y: %f \n", i, x[i], y[i]);
    }
}


// тут ведь все 0??
//x[0] = 0;0
//x[1] = 0*0 = 0;
//x[2] = 0*0 = 0;
//тогда у тоже 0
//у задается после цикла
//
//чтобы придать какой-то смысл нужно сделтаь совсем другую программу
// х[0] = что-то кроме 0 и 1
//разделить циклы
//
// gcc -fopenmp lab7.c
// ./a.out
