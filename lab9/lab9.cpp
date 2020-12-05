#include <cstdio>
#include <cstdlib>
#include <mpi.h>
#include <cmath>

#define ALPGHABET_SIZE 26
#define MODULES_SIZE 3
std::string alphabet[ALPGHABET_SIZE + 1][MODULES_SIZE] = {{"z", "y", "b",},//"a"
                                                          {"y", "x", "c",},//"b"
                                                          {"x", "w", "d",},//"c"
                                                          {"w", "v", "e",},//"d"
                                                          {"v", "u", "f",},//"e"
                                                          {"u", "t", "g",},//"f"
                                                          {"t", "s", "h",},//"g"
                                                          {"s", "r", "i",},//"h"
                                                          {"r", "q", "j",},//"i"
                                                          {"q", "p", "k",},//"j"
                                                          {"p", "o", "l",},//"k"
                                                          {"o", "n", "m",},//"l"
                                                          {"n", "m", "n",},//"m"
                                                          {"m", "l", "o",},//"n"
                                                          {"l", "k", "p",},//"o"
                                                          {"k", "j", "q",},//"p"
                                                          {"j", "i", "r",},//"q"
                                                          {"i", "h", "s",},//"r"
                                                          {"h", "g", "t",},//"s"
                                                          {"g", "f", "u",},//"t"
                                                          {"f", "e", "v",},//"u"
                                                          {"e", "d", "w",},//"v"
                                                          {"d", "c", "a",},//"w"
                                                          {"c", "b", "y",},//"x"
                                                          {"b", "a", "z",},//"y"
                                                          {"a", "z", "x",},//"z"
                                                          {"0", "1", "2",}};

int main(int argc, char **argv) {
    std::string message = "zywvfg";
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    size_t batchSize = std::ceil(message.length() / (double) size);
    int alphabetIndex;
    char *batch = new char[batchSize];
    int startIndex = rank * batchSize;

//    назначаем, какой модуль какой алфавит использует
    for (int i = 0; i < MODULES_SIZE; i++) {
        if (std::stoi(alphabet[ALPGHABET_SIZE][i]) == rank) {
            alphabetIndex = i;
            break;
        }
    }

    for (int i = startIndex; i < startIndex + batchSize; i++) {
        if (i == message.length())
            break;
        for (int j = 0; j < ALPGHABET_SIZE; j++) {
            if (alphabet[j][alphabetIndex][0] == message[i]) {
                printf("alphabetIndex %d, rank %d, alphabet[j][alphabetIndex][0] %d \n", alphabetIndex, rank, alphabet[j][alphabetIndex][0]);
                batch[i - startIndex] = (97 + j);
            }
        }
    }
    if (rank != 0) {
        MPI_Send(batch, batchSize, MPI_CHAR, 0, rank, MPI_COMM_WORLD);
    }
    // ждем пока все пришлют результат
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) {
        std::string decodedMessage;
        decodedMessage.append(batch, batchSize);
        for (int i = 1; i < size; i++) {
            MPI_Status status;
            MPI_Recv(batch, batchSize, MPI_CHAR, i, i, MPI_COMM_WORLD, &status);
            decodedMessage.append(batch, batchSize);
        }
        printf("Message: %s\n", decodedMessage.c_str());
    }
    delete[] batch;

    MPI_Finalize();
    return 0;
}

// mpic++ lab9.cpp
// mpirun -n 3 ./a.out