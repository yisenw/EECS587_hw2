#include <mpi.h>
#include <iostream>
#include <vector>
#include <stdlib.h>     /* atoi */
using namespace std;
// using std::endl;

double IT_NUM = 10;

double func(double a, double b, double c, double d) {
    return a / (a + b + c + d);
}

void run_serial(vector<vector<double>>& A0, double n, double (*f)(double, double, double, double)) {
    cout << "n is: " << n << endl;

    double start = MPI_Wtime();
    vector<vector<double>> A(n, vector<double> (n, 0));

    for (int it = 0; it < IT_NUM; it++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (i == 0 || j == 0 || i == n-1 || j == n-1) A[i][j] = A0[i][j];
                else A[i][j] = f(A0[i][j], A0[i + 1][j], A0[i][j + 1], A0[i + 1][j + 1]);
            }
        }

        

        // print
        cout << "\nA for it = " << it << endl;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) cout << A[i][j] << " ";
            cout << "\n";
        }
        A0 = A;

    }

    double final = MPI_Wtime();
    cout << "Wall time:  " << final - start << endl;
}


int main(int argc, char** argv) {
// Initialize the MPI environment.
    MPI_Init(&argc, &argv);
    int n = atoi(argv[1]);

    // initilize
    vector<vector<double>> A0(n, vector<double> (n, 0));
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) A0[i][j] = i + j * n;
    // print
    cout << "A0:\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) cout << A0[i][j] << " ";
        cout << "\n";
    }
    cout << "\n";

    MPI_Barrier(MPI_COMM_WORLD);
    run_serial(A0, n, &func);

    // Finalize MPI.
    MPI_Finalize();
    return 0;
}