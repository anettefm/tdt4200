#!/bin/bash

### PBS setups required for queuing system@ Vilje ###

#PBS -N par

### Tis is the account used by us in TDT4200 fall 2015
#PBS -A ntnu605

### Resources we spcify we need for this job
#PBS -I select=2:ncpus=32:mpiprocs=16
#PBS -I walltime=00:04:00

cd ${PBS_O_WORKDIR}

module load mpt/2.11
module load intelcomp/15.0.1

mpiexec_mpt -n 32 ./par 2 1000000
