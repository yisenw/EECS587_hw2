#!/bin/bash
# (See https://arc-ts.umich.edu/greatlakes/user-guide/ for command details)
# Set up batch job settings
#SBATCH --job-name=mpi_hello_world
#SBATCH --mail-type=BEGIN, END
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=4
#SBATCH --mem-per-cpu=1g
#SBATCH --time=00:05:00
#SBATCH --account=eecs587f21_class
#SBATCH --partition=standard

# Run your program
# (">" redirects the print output of your pfogram,
# in this case to "output.txt")

mpic++ serial_program.cpp -o serial.out -O3
mpirun -np 1 --oversubscribe serial.out 4 1
