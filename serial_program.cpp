#include <mpi.h>
#include <iostream>
#include <vector>
#include <stdlib.h>     /* atoi */
#include "f.h"
using namespace std;
// using std::endl;

int IT_NUM = 10;

long long func(long long a, long long b, long long c, long long d) {
    return (a + b + c + d) % 100;
}

void run_serial(vector<vector<long long>>& A0, int n, long long (*f)(long long, long long, long long, long long), int if_print) {
    cout << "n is: " << n << endl;

    double start = MPI_Wtime();
    vector<vector<long long>> A(n, vector<long long> (n, 0));

    for (int it = 0; it < IT_NUM; it++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (i == 0 || j == 0 || i == n-1 || j == n-1) A[i][j] = A0[i][j];
                else A[i][j] = f(A0[i][j], A0[i + 1][j], A0[i][j + 1], A0[i + 1][j + 1]);
            }
        }

        if (if_print) {
            // print
            cout << "\nA for it = " << it << endl;
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) cout << A[i][j] << " ";
                cout << "\n";
            }
            A0 = A;
        }



    }
    
    long long out_sum = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) out_sum += A[i][j];
    }

    double final = MPI_Wtime();
    cout << "Sum is:  " << out_sum << endl;
    cout << "Wall time:  " << final - start << endl;
}


int main(int argc, char** argv) {
// Initialize the MPI environment.
    MPI_Init(&argc, &argv);
    int n = atoi(argv[1]);

    // initilize
    vector<vector<long long>> A0(n, vector<long long> (n, 0));
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) A0[i][j] = (long long)(i + j * n);
    // print

    int if_print = atoi(argv[2]);
    if (if_print) {
        cout << "A0:\n";
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) cout << A0[i][j] << " ";
            cout << "\n";
        }
        cout << "\n";
    }
    

    MPI_Barrier(MPI_COMM_WORLD);
    run_serial(A0, n, &f, if_print);

    // Finalize MPI.
    MPI_Finalize();
    return 0;
}