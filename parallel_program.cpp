#include <mpi.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <stdlib.h>     /* atoi */
using namespace std;
// using std::endl;

int IT_NUM = 10;

int func(int a, int b, int c, int d) {
    return (a + b + c + d) % 100;
}

void run_parallel(vector<vector<int>>& A0, int n, int (*f)(int, int, int, int), int if_print, int P, int ID) {
    cout << "Start process " << ID << " of " << P << endl;
    int sub_n = ceiling(n / sqrt(P));
    cout << "sub_n = " << sub_n << endl;
    cout << "Contents:" << endl;
    for (int i = sub_n * ID; i < min(sub_n * (ID + 1), n); i++) {
        for (int j = sub_n * ID; j < min(sub_n * (ID + 1), n); j++) cout << A[i][j] << " ";
        cout << "\n";
    }
    cout << "\n";
}


int main(int argc, char** argv) {
// Initialize the MPI environment.
    MPI_Init(&argc, &argv);
    int n = atoi(argv[1]);
    int if_print = atoi(argv[2]);
    // initilize
    vector<vector<int>> A0(n, vector<int> (n, 0));
    
    // print
    int P;
    int ID;
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    MPI_Comm_rank(MPI_COMM_WORLD, &ID);
    if (ID == 0) {
        cout << "Root processor " << ID << " is initializing." << endl;
        for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) A0[i][j] = i + j * n;
        if (if_print) {
            cout << "n is: " << n << endl;
            cout << "A0:\n";
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) cout << A0[i][j] << " ";
                cout << "\n";
            }
            cout << "\n";
        } 
    }
    

    

    MPI_Barrier(MPI_COMM_WORLD);
    run_parallel(A0, n, &func, if_print, P, ID);

    // Finalize MPI.
    MPI_Finalize();
    return 0;
}