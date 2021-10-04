#include <mpi.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <stdlib.h>     /* atoi */
#include "f.h"
#include <cassert>
using namespace std;
// using std:: "\n";

int IT_NUM = 10;

long long func(long long a, long long b, long long c, long long d) {
    return (a + b + c + d) % 100;
}

void get_first_col(vector<long long>& msg, const vector<vector<long long>>& A0) {
    for (vector<long long> vec: A0) msg.push_back(vec[0]);
}

void append_col(vector<vector<long long>>& A0) {
    for (vector<long long>& vec: A0) vec.push_back(0);
}

void run_parallel(int n, long long (*f)(long long, long long, long long, long long), int if_print, int P, int ID) {
    int sub_n = ceil(n / sqrt(P));
    int row = floor(ID / sqrt(P));
    int col = ID - row * sqrt(P);
    // if (if_print)  {
    //     cout << "Start process " << ID << " of " << P <<  ", sub_n is " << sub_n << ", i_start = " 
    //     << sub_n * row << ", i_end = " <<min(sub_n * (row + 1), n) << ", j_start = " 
    //     << sub_n * col << ", j_end = " <<min(sub_n * (col + 1), n) << " ";
    // }
    int num_row = min(sub_n * (row + 1), n) - sub_n * row;
    int num_col = min(sub_n * (col + 1), n) - sub_n * col;

    int n_of_P = sqrt(P);
    vector<vector<long long>> A0(num_row, vector<long long> (num_col, 0));

    // if (row != n_of_P - 1) { // not the lowest
    //     A0.push_back(vector<long long> (num_col, 0));
    // }
    // if (col != n_of_P - 1) { // not the rightest
    //     append_col(A0);
    // }


    for (int i = 0; i < num_row; i++) {
        for (int j = 0; j < num_col; j++) {
            A0[i][j] = (long long)((row*sub_n + i) + (col*sub_n + j) * n);
        }
    }

    // if (if_print) {
    //     cout << "sub_n = " << sub_n << ", ";
    //     cout << "Contents of " << ID <<  ": ";
    //     for (int i = 0; i < num_row; i++) {
    //         for (int j = 0; j < num_col; j++) {
    //             cout << A0[i][j] << " ";
    //         }
    //         // cout << "\n";
    //     }
    //     cout << "\n";
    // }

    // start sending 
    // right send to left; lower send to upper; lower right send to upper left.
    for (int it = 0; it < IT_NUM; it++) {

        if (col != 0) { // right send to left
            vector<long long> msg;
            get_first_col(msg, A0);
            // if (if_print) {
            //     cout << "Sending msg from right " << ID << " of " << P << " to " << ID - 1 << ": ";
            //     for (long long elt: msg) cout << elt << " ";
            //     cout << endl;
            // }
            // long long* a = &msg[0];
            MPI_Send(&msg[0], msg.size(), MPI_LONG_LONG, ID - 1, 0, MPI_COMM_WORLD);
        }
        if (row != 0) { // lower send to upper
            vector<long long> msg = A0[0];
            // if (if_print) {
            //     cout << "Sending msg from lower " << ID << " of " << P << " to " << ID - n_of_P << ": ";
            //     for (long long elt: msg) cout << elt << " ";
            //     cout << endl;
            // }
            MPI_Send(&msg[0], msg.size(), MPI_LONG_LONG, ID - n_of_P, 0, MPI_COMM_WORLD);
        }
        if (col != 0 && row != 0) { // lower l_r to u_l
            long long msg = A0[0][0];
            // if (if_print) {
            //     cout << "Sending msg from l_r " << ID << " of " << P << " to " << ID - 1 - n_of_P << ": ";
            //     cout << msg;
            //     cout << endl;
            // }
            MPI_Send(&msg, 1, MPI_LONG_LONG, ID - 1 - n_of_P, 0, MPI_COMM_WORLD);
        }

        

        long long last_row[num_col];
        long long last_col[num_row];
        long long l_r = -1;
        // start receiving
        if (col != n_of_P - 1) { // not the rightest, receive from right
            MPI_Recv(&last_col, num_row, MPI_LONG_LONG, ID + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        if (row != n_of_P - 1) { // not the lowest
            MPI_Recv(&last_row, num_col, MPI_LONG_LONG, ID + n_of_P, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        if (row != n_of_P - 1 && col != n_of_P - 1) { // not the lowest or rightest
            MPI_Recv(&l_r, 1, MPI_LONG_LONG, ID + n_of_P + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        if (if_print && it == 9) {
            if (col != n_of_P - 1) {
                cout << "Last col of " << ID << ", ";
                for (long long elt: last_col) cout << elt << " ";
            }
            if (row != n_of_P - 1) {
                cout << "Last row of " << ID << ", ";
                for (long long  elt: last_row) cout << elt << " ";
            }
            cout << "LR of " << ID << ", ";
            cout << l_r << "\n";
        }

        

        // begin calculation.
        vector<vector<long long>> A(num_row, vector<long long> (num_col, 0));
        int start_row = 0;
        int start_col = 0;
        if (row == 0) {
            start_row = 1;
            for (int j = 0; j < num_col; j++) A[0][j] = A0[0][j];
        }
        if (col == 0) {
            start_col = 1;
            for (int i = 0; i < num_row; i++) A[i][0] = A0[i][0];
        } // upperest and leftest are handled

        for (int i = start_row; i < num_row - 1; i++) { // handle mid elements
            for (int j = start_col; j < num_col - 1; j++) {
                A[i][j] = f(A0[i][j], A0[i + 1][j], A0[i][j + 1], A0[i + 1][j + 1]);
            }
        }
        if (col != n_of_P - 1) { // not the rightest
            for (int i = start_row; i < num_row - 1; i++) { // last col
                A[i][num_row-1] = f(A0[i][num_col-1], A0[i + 1][num_col-1], last_col[i], last_col[i + 1]);
            }
        }
        if (row != n_of_P - 1) { // not the lowerst
            for (int j = start_col; j < num_col - 1; j++) { // last row
                A[num_col-1][j] = f(A0[num_row-1][j], last_row[j], A0[num_row-1][j + 1], last_row[j + 1]);
            }
        }
        if (row != n_of_P - 1 && col != n_of_P - 1) { // not the lowest or rightest
            assert(l_r != -1);
            A[num_col-1][num_row-1] = f(A0[num_col-1][num_row-1], last_row[num_col-1], last_col[num_row-1], l_r);
        }


        

        // handle rightest and lowerest
        if (col == n_of_P - 1) for (int i = 0; i < num_row; i++) A[i][num_col-1] = A0[i][num_col-1];
        if (row == n_of_P - 1) for (int j = 0; j < num_col; j++) A[num_row-1][j] = A0[num_row-1][j];
        if (row == n_of_P - 1 && col == n_of_P - 1) A[num_row-1][num_col-1] = A0[num_row-1][num_col-1];

        
        

        if (if_print && it == 9) {
            cout << "Contents of A of " << ID <<  ": ";
            for (int i = 0; i < num_row; i++) {
                for (int j = 0; j < num_col; j++) {
                    cout << A[i][j] << " ";
                }
                // cout << "\n";
            }
            cout << "\n";
        }


        A0 = A; // important!
        // cout << ID << " mei shi" << endl;
        MPI_Barrier(MPI_COMM_WORLD);
    
    }

    // find sum
    long long local_sum = 0;
    for (int i = 0; i < num_row; i++) {
        for (int j = 0; j < num_col; j++) {
            local_sum += A0[i][j];
            // if (target == A[i][j]) total++;
        }
    }
    if (ID != 0) MPI_Send(&local_sum, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
    else { // ID == 0!
        long long total_sum = local_sum;
        for (int i = 1; i < P; i++) {
            long long i_sum = -1;
            MPI_Recv(&i_sum, 1, MPI_LONG_LONG, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_sum += i_sum;
        }
        cout << "Sum is:  " << total_sum << endl;
    }



    return;//sb

}


int main(int argc, char** argv) {
// Initialize the MPI environment.
    MPI_Init(&argc, &argv);
    int n = atoi(argv[1]);
    // initilize
    vector<vector<long long>> A0(n, vector<long long> (n, 0));
    
    // print
    int P;
    int ID;
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    MPI_Comm_rank(MPI_COMM_WORLD, &ID);
    int if_print = atoi(argv[2]);
    // if_print = if_print && (ID == 0);
    if (ID == 0) {
        cout << "Root processor " << ID << " is initializing." <<  "\n";
        for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) A0[i][j] = (long long)(i + j * n);
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
    run_parallel(n, &f, if_print, P, ID);

    // Finalize MPI.
    MPI_Finalize();
    return 0;
}