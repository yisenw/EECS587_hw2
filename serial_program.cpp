#include <mpi.h>
#include <iostream>
#include <stdlib.h>     /* atoi */
using std::cout;
using std::endl;

int func(int a, int b, int c, int d) {
    return a - b - c - d;
}

void run_serial(int n, int (*f)(int, int, int, int)) {
    int A0[n][n];
    cout << "n is: " << n << endl;
    // initilize
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            A0[i][j] = i + n * j;
            // cout << A0[i][j] << endl;
        }
    }

    int A[n][n];
    // for (int i = 0; i < n; i++) {
    //     for (int j = 0; j < n; j++) {
    //         if (i == 0 || j == 0 || i == n-1 || j == n-1) A[i][j] = A0[i][j];
    //         else A[i][j] = f(A0[i][j], A0[i + 1][j], A0[i][j + 1], A0[i + 1][j + 1]);
    //     }
    // }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; i++) {
            cout << A0[i][j] << " ";
        }
        cout << "\n";
    }
    // for (int i = 0; i < n; i++) {
    //     for (int j = 0; j < n; i++) {
    //         cout << A[i][j] << " ";
    //     }
    //     cout << "\n";
    // }

}


int main(int argc, char** argv) {
// Initialize the MPI environment.
    MPI_Init(&argc, &argv);
    // Obtain my id and the world size.
    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();
    int n = atoi(argv[1]);
    run_serial(n, &func);

    double final = MPI_Wtime();
    cout << "Wall time:  " << final - start << endl;
    // Finalize MPI.
    MPI_Finalize();
    return 0;
}