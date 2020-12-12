//
// Created by aslan on 12.12.2020.
//

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define AR 4
#define AC 4
#define BC 4
#define MASTER 0

int main(int argc, char *argv[]) {
    int numtasks,
            taskid,
            numworkers,
            sourceTaskId,
            destTaskId,
            msgTag,
            rows,                  /* число строк для каждого воркера */
    rowsPerWorker, extraRows, offset,
            i, j, k, rc;
    double a[AR][AC],
            b[AC][BC],
            c[AR][BC];
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    numworkers = numtasks - 1;


    if (taskid == MASTER) {
        printf("started %d tasks.\n", numtasks);
        printf("Creating matrices...\n");
        for (i = 0; i < AR; i++)
            for (j = 0; j < AC; j++)
                a[i][j] = i + j * j + 4;
        for (i = 0; i < AC; i++)
            for (j = 0; j < BC; j++)
                b[i][j] = i * j + i + 2;

        printf("/////////////////// A Matrix ///////////////////\n");
        for (i = 0; i < AR; i++) {
            printf("\n");
            for (j = 0; j < AC; j++)
                printf("%6.2f   ", a[i][j]);
        }
        printf("\n///////////////////////////////////\n");

        printf("\n/////////////////// B Matrix ///////////////////\n");
        for (i = 0; i < AR; i++) {
            printf("\n");
            for (j = 0; j < BC; j++)
                printf("%6.2f   ", b[i][j]);
        }
        printf("\n//////////////////////////////////////\n");

        rowsPerWorker = AR / numworkers;
        extraRows = AR % numworkers;
        offset = 0;
        msgTag = 1;
        for (destTaskId = 1; destTaskId <= numworkers; destTaskId++) {
            /* распределяем висячие строки между воркерами */
            rows = (destTaskId <= extraRows) ? rowsPerWorker + 1 : rowsPerWorker;
            printf("%d rows sent to task %d offset=%f\n", rows, destTaskId, a[offset][0]);
            MPI_Send(&offset, 1, MPI_INT, destTaskId, msgTag, MPI_COMM_WORLD);
            MPI_Send(&rows, 1, MPI_INT, destTaskId, msgTag, MPI_COMM_WORLD);
            MPI_Send(&a[offset][0], rows * AC, MPI_DOUBLE, destTaskId, msgTag,
                     MPI_COMM_WORLD);
            MPI_Send(&b, AC * BC, MPI_DOUBLE, destTaskId, msgTag, MPI_COMM_WORLD);
            offset = offset + rows;
        }

        /* ответ */
        msgTag = 2;
        for (i = 1; i <= numworkers; i++) {
            sourceTaskId = i;
            MPI_Recv(&offset, 1, MPI_INT, sourceTaskId, msgTag, MPI_COMM_WORLD, &status);
            MPI_Recv(&rows, 1, MPI_INT, sourceTaskId, msgTag, MPI_COMM_WORLD, &status);
            MPI_Recv(&c[offset][0], rows * BC, MPI_DOUBLE, sourceTaskId, msgTag,
                     MPI_COMM_WORLD, &status);
            printf("Got results from task %d\n %6.2f\n", sourceTaskId, c[offset][0]);
        }

        /* Print results */
        printf("//////////////////////////////////////////////////////\n");
        printf("/////////////////// Result Matrix ///////////////////\n");
        for (i = 0; i < AR; i++) {
            printf("\n");
            for (j = 0; j < BC; j++)
                printf("%6.2f   ", c[i][j]);
        }
        printf("\n//////////////////////////////////////////////////////\n");
    }


    if (taskid != MASTER) {
        msgTag = 1;
        MPI_Recv(&offset, 1, MPI_INT, MASTER, msgTag, MPI_COMM_WORLD, &status);
        MPI_Recv(&rows, 1, MPI_INT, MASTER, msgTag, MPI_COMM_WORLD, &status);
        MPI_Recv(&a, rows * AC, MPI_DOUBLE, MASTER, msgTag, MPI_COMM_WORLD, &status);
        MPI_Recv(&b, AC * BC, MPI_DOUBLE, MASTER, msgTag, MPI_COMM_WORLD, &status);
        for (k = 0; k < BC; k++) {
            for (i = 0; i < rows; i++) {
                c[i][k] = 0.0;
                for (j = 0; j < AC; j++)
                    c[i][k] = c[i][k] + a[i][j] * b[j][k];
            }
        }
        msgTag = 2;
        MPI_Send(&offset, 1, MPI_INT, MASTER, msgTag, MPI_COMM_WORLD);
        MPI_Send(&rows, 1, MPI_INT, MASTER, msgTag, MPI_COMM_WORLD);
        MPI_Send(&c, rows * BC, MPI_DOUBLE, MASTER, msgTag, MPI_COMM_WORLD);
    }
    MPI_Finalize();
}
