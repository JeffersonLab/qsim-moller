#!/bin/bash

#SBATCH --account=halla
#SBATCH --partition=production
#SBATCH --job-name=readhitn
#SBATCH --mail-user=sudip@jlab.org
#SBATCH --mail-type=END,FAIL
#SBATCH --time=5:59:59
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=2
#SBATCH --mem=50G
#SBATCH --output=/w/halla-scshelf2102/moller12gev/sudip/qsim/qsim-showermax/slurm_job/job_log/general/read_hitn.out
#SBATCH --error=/w/halla-scshelf2102/moller12gev/sudip/qsim/qsim-showermax/slurm_job/job_log/general/read_hitn.err
cd /w/halla-scshelf2102/moller12gev/sudip/qsim/qsim-showermax/
root -q analysis/read_hitn.C
