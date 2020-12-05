//
// Created by aslan on 05.12.2020.
//
// Результаты (для 4 процессов, среднее время):
// MPI_Scatter:
    // 10 запусков              4640
    // 1000 запусков            396
    // 10000                    222
//
// Custom:
    // 10 запусков              12173
    // 1000 запусков            1047
    // 10000                    1015
// Цифры сильно разнятся от запуска к запуску.
// Вывод: MPI_Scatter быстрее. К тому же, сравнительно меньше разброс по времени для разных запусков.


#include <cstdio>
#include <chrono>
#include <mpi.h>
#include <random>

#define MODE  0 // 0 - mpi_scatter, 1 - custom
int timesToRun = 10;

int main(int argc, char **argv) {
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int *senBbuf = new int[size];
    for (int i = 0; i < size; i++) {
        senBbuf[i] = i;
    }
    int *recvBuf = new int[size];

    long long int time;

    if (MODE == 0) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < timesToRun; i++) {
            MPI_Scatter(senBbuf, 1, MPI_INT, recvBuf, 1, MPI_INT, 0, MPI_COMM_WORLD);
        }
        time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count();
        if (rank == 0) {
            printf("MPI_Scatter: %lld\n", time / timesToRun);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    } else {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < timesToRun; i++) {
            if (rank == 0) {
                for (int j = 0; j < size; j++) {
                    MPI_Request request;
                    MPI_Isend(&senBbuf[j], 1, MPI_INT, j, rank, MPI_COMM_WORLD, &request);
                }
            }

            MPI_Barrier(MPI_COMM_WORLD);
            MPI_Status status;
            int recInt;
            MPI_Recv(&recInt, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        }
        time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count();
        if (rank == 0) {
            printf("Custom: %lld\n", time / timesToRun);
        }
    }

    delete[] senBbuf;
    delete[] recvBuf;
    MPI_Finalize();
    return 0;
}

// mpic++ lab10.cpp
//mpirun -n 4 ./a.out