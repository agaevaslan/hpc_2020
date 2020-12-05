//
// Created by aslan on 03.12.2020.
//

// C++ program to find rank of a matrix
#include <bits/stdc++.h>
#include <chrono>

using namespace std;
#define R 10
#define C 10
#define NUM_THREADS 1

void printMatrix(int matrix[R][C], int row, int col) {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++){
            printf("  %d", matrix[i][j]);
        }
        printf("\n");
    }
}

/* function for finding rank of matrix */
int getRankOfMatrix(int matrix[R][C], int numThreads) {
    const double EPS = 1E-5;

    int rank = C;
    vector<char> line_used (C);
#pragma omp parallel for num_threads(numThreads)
    for (int i=0; i<C; ++i) {
        int j;
        for (j=0; j<R; ++j)
            if (!line_used[j] && abs(matrix[j][i]) > EPS)
                break;
        if (j == R)
            --rank;
        else {
            line_used[j] = true;
            for (int p=i+1; p<C; ++p)
                matrix[j][p] /= matrix[j][i];
            for (int k=0; k<R; ++k)
                if (k != j && abs(matrix[k][i]) > EPS)
                    for (int p=i+1; p<C; ++p)
                        matrix[k][p] -= matrix[j][p] * matrix[k][i];
        }
    }
    return rank;
}

int main() {
//   заполняем матрицу случайными числами
    int matrix[C][R];
    int i, j;

    srand(time(NULL));
    for (i = 0; i < R; i++)
    {
        for (j = 0; j < C; j++)
        {
            matrix[i][j] = rand() % 10;
        }
    }
    printMatrix(matrix, R, C);

    int timesToRun = 1;
    for (int numThreads = 1; numThreads < 8; ++numThreads) {
        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
        for (int run = 0; run < timesToRun; ++run) {
            printf("Rank of the matrix is : %d\n", getRankOfMatrix(matrix,  numThreads));
            getRankOfMatrix(matrix,  numThreads);
        }
        long long time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count();
        printf("Time for %d threads: %lld\n", numThreads, time / timesToRun);
    }

    return 0;
}


// g++ -fopenmp lab7_2.cpp
// ./a.out
