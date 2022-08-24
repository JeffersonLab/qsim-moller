#!/bin/bash

#SBATCH --account=halla
#SBATCH --partition=production
#SBATCH --job-name=QSIM
#SBATCH --mail-user=sudip@jlab.org
#SBATCH --mail-type=FAIL,END
#SBATCH --time=1-23:59:59
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=2
#SBATCH --mem=6G
#SBATCH --output= /home/sudip/programs/qsim/qsim-showermax/slurm_job/job_log/qsim_2GeV_smBenchmark1quartzQsim.gdml.out
#SBATCH --error= /home/sudip/programs/qsim/qsim-showermax/slurm_job/job_log/qsim_2GeV_smBenchmark1quartzQsim.gdml.err
cd /home/sudip/programs/qsim/qsim-showermax/
./build/qsim geometry/smBenchmark1quartzQsim.gdml slurm_job/macros/runbatch_2GeV_0k.mac