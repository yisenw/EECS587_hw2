#!/bin/bash
# (See https://arc-ts.umich.edu/greatlakes/user-guide/ for command details)
# Set up batch job settings

#SBATCH --job-name=submit
#SBATCH --mail-type=BEGIN,END
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=36
#SBATCH --mem-per-cpu=1g
#SBATCH --time=00:05:00
#SBATCH --account=yisenw
#SBATCH --partition=standard

# Run your program
# (">" redirects the print output of your pfogram,
# in this case to "output.txt")

mpirun -np 36 --oversubscribe pal.out 4000 0 > out.txt
