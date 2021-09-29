#include <mpi.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <stdlib.h>     /* atoi */
using namespace std;
// using std:: "\n";

int IT_NUM = 10;

int func(int a, int b, int c, int d) {
    return (a + b + c + d) % 100;
}

void run_parallel(int n, int (*f)(int, int, int, int), int if_print, int P, int ID) {
    int sub_n = ceil(n / sqrt(P));
    int row = floor(ID / sqrt(P));
    int col = ID - row * sqrt(P);
    cout << "Start process " << ID << " of " << P <<  ", sub_n is " << sub_n << ", i_start = " 
        << sub_n * row << ", i_end = " <<min(sub_n * (row + 1), n) << ", j_start = " 
        << sub_n * col << ", j_end = " <<min(sub_n * (col + 1), n) << " ";
    int num_row = min(sub_n * (row + 1), n) - sub_n * row;
    int num_col = min(sub_n * (col + 1), n) - sub_n * col;
    vector<vector<int>> A0(num_row, vector<int> (num_col, 0));

    if (if_print) {
        cout << "sub_n = " << sub_n << ", ";
        cout << "Contents of " << ID <<  ": ";
        for (int i = 0; i < num_row; i++) {
            for (int j = 0; j < num_col; j++) {
                A0[i][j] = (row*sub_n + i) + (col*sub_n + j) * n;
                cout << A0[i][j] << " ";
            }
            // cout << "\n";
        }
        cout << "\n";
    }
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
        cout << "Root processor " << ID << " is initializing." <<  "\n";
        for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) A0[i][j] = i + j * n;
        if (if_print) {
            cout << "n is: " << n <<  "\n";
            cout << "A0:\n";
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) cout << A0[i][j] << " ";
                cout << "\n";
            }
            cout << "\n";
        } 
    }
    

    

    MPI_Barrier(MPI_COMM_WORLD);
    run_parallel(n, &func, if_print, P, ID);

    // Finalize MPI.
    MPI_Finalize();
    return 0;
}