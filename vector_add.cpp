#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <chrono>

using namespace std;
using namespace std::chrono;

void randomVector(int* vector, int size) {
    for (int i = 0; i < size; i++) {
        vector[i] = rand() % 100;
    }
}

int main(int argc, char* argv[]) {
    int rank, size;
    unsigned long total_size = 100000000; // total elements
    int *v1 = nullptr, *v2 = nullptr, *v3 = nullptr;
    int *local_v1, *local_v2, *local_v3;
    int local_size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // process rank
    MPI_Comm_size(MPI_COMM_WORLD, &size); // total number of processes

    local_size = total_size / size;

    // Allocate memory for local chunks
    local_v1 = new int[local_size];
    local_v2 = new int[local_size];
    local_v3 = new int[local_size];

    auto start = high_resolution_clock::now();

    // Only the master process allocates and initializes the full vectors
    if (rank == 0) {
        v1 = new int[total_size];
        v2 = new int[total_size];
        v3 = new int[total_size];
        srand(time(0));
        randomVector(v1, total_size);
        randomVector(v2, total_size);
    }

    // Scatter v1 and v2 to all processes
    MPI_Scatter(v1, local_size, MPI_INT, local_v1, local_size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(v2, local_size, MPI_INT, local_v2, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Perform local addition
    for (int i = 0; i < local_size; i++) {
        local_v3[i] = local_v1[i] + local_v2[i];
    }

    // Gather results back to the master process
    MPI_Gather(local_v3, local_size, MPI_INT, v3, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Reduce to find the total sum of v3 across all processes
    int local_sum = 0;
    for (int i = 0; i < local_size; i++) {
        local_sum += local_v3[i];
    }

    int total_sum = 0;
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    auto stop = high_resolution_clock::now();

    if (rank == 0) {
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "Time taken by MPI program: " << duration.count() << " microseconds" << endl;
        cout << "Total sum of v3: " << total_sum << endl;
    }

    // Cleanup
    delete[] local_v1;
    delete[] local_v2;
    delete[] local_v3;
    if (rank == 0) {
        delete[] v1;
        delete[] v2;
        delete[] v3;
    }

    MPI_Finalize();
    return 0;
}
