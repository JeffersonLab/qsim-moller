#!/bin/bash

#SBATCH --account=halla
#SBATCH --partition=production
#SBATCH --job-name=qsimv2
#SBATCH --mail-user=sudip@jlab.org
#SBATCH --mail-type=END,FAIL
#SBATCH --time=8:59:59
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --mem=500M
#SBATCH --output=/w/halla-scshelf2102/moller12gev/sudip/qsim/qsim-showermax/slurm_job/job_log/qsim_singleRun/qsim_retrov2-2-0_1.out
#SBATCH --error=/w/halla-scshelf2102/moller12gev/sudip/qsim/qsim-showermax/slurm_job/job_log/qsim_singleRun/qsim_retrov2-2-0_1.err
cd /w/halla-scshelf2102/moller12gev/sudip/qsim/qsim-showermax/
./build/qsim geometry/showerMaxRetro_v2-2-0.gdml /w/halla-scshelf2102/moller12gev/sudip/qsim/qsim-showermax/macros/runexample_mami.mac
