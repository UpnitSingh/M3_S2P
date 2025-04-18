#include <mpi.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
    int rank, size;
    char message[100];

    MPI_Init(&argc, &argv); // Initialize MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get process rank
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Get total number of processes

    // Part 1: Using MPI_Send and MPI_Recv
    if (rank == 0) {
        // Master process sends "Hello World!" to all other processes
        strcpy(message, "Hello World!");
        for (int i = 1; i < size; i++) {
            MPI_Send(message, strlen(message) + 1, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }
    } else {
        // Worker processes receive and print the message
        MPI_Recv(message, 100, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d received message (Send/Recv): %s\n", rank, message);
    }

    MPI_Barrier(MPI_COMM_WORLD); // Ensure all processes sync before next part

    // Part 2: Using MPI_Bcast
    if (rank == 0) {
        strcpy(message, "Hello again from MPI_Bcast!");
    }

    MPI_Bcast(message, 100, MPI_CHAR, 0, MPI_COMM_WORLD);
    printf("Process %d received message (Bcast): %s\n", rank, message);

    MPI_Finalize(); // Clean up MPI
    return 0;
}
