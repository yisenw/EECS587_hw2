#include <mpi.h>
#include <iostream>
using std::cout;
using std::endl;
int main(int argc, char** argv) {
// Initialize the MPI environment.
    MPI_Init(&argc, &argv);
    // Obtain my id and the world size.
    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    int n = 4;
    int A[n][n];

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; i++) {
            A[i][j] = i + n * j;
        }
    }

    double final = MPI_Wtime();
    cout << "Wall time:  " << final - start << endl;

    // Finalize MPI.
    MPI_Finalize();
    return 0;
}