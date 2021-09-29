#include <mpi.h>
#include <iostream>
#include <vector>
#include <stdlib.h>     /* atoi */
using std::cout;
using std::endl;

int func(int a, int b, int c, int d) {
    return a - b - c - d;
}

void run_serial(vector<vector<int>> A0, int n, int (*f)(int, int, int, int)) {
    int A0[n][n];
    cout << "n is: " << n << endl;

    double start = MPI_Wtime();
    int A[n][n];
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == 0 || j == 0 || i == n-1 || j == n-1) A[i][j] = A0[i][j];
            else A[i][j] = f(A0[i][j], A0[i + 1][j], A0[i][j + 1], A0[i + 1][j + 1]);
        }
    }

    // print
    cout << "A0:\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << A0[i][j] << " ";
        }
        cout << "\n";
    }
    cout << "\nA:\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << A[i][j] << " ";
        }
        cout << "\n";
    }

    double final = MPI_Wtime();
    cout << "Wall time:  " << final - start << endl;

}


int main(int argc, char** argv) {
// Initialize the MPI environment.
    MPI_Init(&argc, &argv);
    int n = atoi(argv[1]);

    // initilize
    vector<vector<int>> A0(n, vector<int> (n, 0));

    MPI_Barrier(MPI_COMM_WORLD);
    run_serial(A0, n, &func);

    // Finalize MPI.
    MPI_Finalize();
    return 0;
}