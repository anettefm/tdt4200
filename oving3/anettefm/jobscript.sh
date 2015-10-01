#!/bin/bash

### PBS setups required for queuing system@ Vilje ###

#PBS -N par

### Tis is the account used by us in TDT4200 fall 2015
#PBS -A ntnu605

### Resources we spcify we need for this job
#PBS -l select=1:ncpus=1
#PBS -l walltime=00:04:00


cd ${PBS_O_WORKDIR}

module load mpt/2.11
module load intelcomp/15.0.1

time ./newImageIdea 1
