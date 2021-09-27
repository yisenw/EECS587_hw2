#include <mpi.h>
#include <iostream>
using std::cout;
using std::endl;
int main(int argc, char** argv) {
// Initialize the MPI environment.
    MPI_Init(&argc, &argv);
    // Obtain my id and the world size.
    MPI_BARRIER(COMM);
    double start = MPI_WTIME();

    int n = 4;
    int A[n][n];

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; i++) {
            A = i + n * j;
        }
    }

    double final = MPI_WTIME();
    cout << "Wall time:  " << final - start << endl;

    // Finalize MPI.
    MPI_Finalize();
    return 0;
}