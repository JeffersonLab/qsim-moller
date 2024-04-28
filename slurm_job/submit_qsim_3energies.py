# Description: This script creates the run macro file and job submit file for qsim and submits the job to ifarm
#              It submits job for multiple gdml geomerties and different energies
# Change: config, beamEnergy, geometry, eventsNum and sourceDir as needed

import os
import sys
import random
import numpy as np

# Define variables
config = "qsim_41"
nJobs = 1
beamParticle = "e-"
beamEnergy = [2.0, 5.0, 8.0]
#for i in np.arange(0.5, 100.0, 0.5):
#    beamEnergy.append(i)
energyUnit = "GeV"
geometry = ["showerMaxDetector_v3-1-0"]
eventsNum = 10000
seed = []
for i in range(3):
    seed.append(random.randrange(10000))
#sourceDir = "/home/sudip/programs/qsim/qsim-showermax/"
sourceDir = "/w/halla-scshelf2102/moller12gev/sudip/qsim/qsim-showermax/"

logDir = sourceDir + "slurm_job/job_log/" + config + "/"
macroDir = sourceDir + "slurm_job/macros/" + config + "/"
jobDir = sourceDir + "slurm_job/sbatch_scripts/" + config + "/"
#outRootFileDir = sourceDir + "rootfiles/" + config + "/"
outRootFileDir = "/lustre19/expphy/volatile/halla/moller12gev/sudip/qsim_rootfiles/" + config + "/"

# Make directories if required
if not os.path.exists(logDir):
    os.makedirs(logDir)
if not os.path.exists(macroDir):
    os.makedirs(macroDir)
if not os.path.exists(jobDir):
    os.makedirs(jobDir)
if not os.path.exists(outRootFileDir):
    os.makedirs(outRootFileDir)

# Define functions
def writeQsimRunMacro(macroName:str, outFileName="qsim_out.root", beamEnergy=2, events=100, seed = 50):
    '''Creates a macro file to run in qsim'''
    file = open(macroDir + macroName, "w")
    file.write("/qsim/fSourceMode 1\n")
    file.write("/run/initialize\n")
    file.write("/process/optical/boundary/setInvokeSD true\n")
    file.write("/qsim/filename " + outRootFileDir + outFileName + ".root\n")
    file.write("/qsim/seed {}\n".format(seed))
    file.write("/qsim/emin {} {}\n".format(beamEnergy, energyUnit))
    file.write("/qsim/emax {} {}\n".format(beamEnergy, energyUnit))
    #file.write("/qsim/emax " + str(beamEnergy) + energyUnit + "\n")
    file.write("/gun/particle " + beamParticle + "\n")
    file.write("/run/beamOn " + str(events) + "\n")
    file.close
    print("\nRun macro " + macroFileName + " created with seed " + str(seed))

def writeJobSubmitScript(scriptName:str, jobOutErrName:str, geometryGDML:str, macro:str, jobName="Qsim"):
    '''Creates a job submission script to run qsim in the Jlab ifarm'''
    file = open(jobDir + scriptName, "w")
    file.write("#!/bin/bash\n\n")
    file.write("#SBATCH --account=halla\n")
    file.write("#SBATCH --partition=production\n")
    file.write("#SBATCH --job-name={}\n".format(jobName))
    file.write("#SBATCH --mail-user=sudip@jlab.org\n")
    file.write("#SBATCH --mail-type=FAIL,END\n")
    file.write("#SBATCH --time=1-23:59:59\n")
    file.write("#SBATCH --nodes=1\n")
    file.write("#SBATCH --ntasks=1\n")
    file.write("#SBATCH --cpus-per-task=2\n")
    file.write("#SBATCH --mem=6G\n")
    file.write("#SBATCH --output=" + logDir + jobOutErrName + ".out\n")
    file.write("#SBATCH --error=" + logDir + jobOutErrName + ".err\n")
    #file.write("source /site/12gev_phys/softenv.sh 2.4\n\n")
    file.write("cd " + sourceDir + "\n")
    file.write("./build/qsim geometry/" +geometryGDML + " " + macroDir + macroFileName)
    file.close
    print("Ifarm job submission script " + scriptName + " created.")

# Use loop to create different macros and job submission file and sbatch them
for iBeam in range(len(beamEnergy)):
    for iGeomtry in range(0,len(geometry)):
        for iJob in range(nJobs):
            macroFileName = "runbatch_{}{}_{}_{}k_{}.mac".format(beamEnergy[iBeam], energyUnit, geometry[iGeomtry], eventsNum//1000, 1001+iJob)
            outRootFile = "qsim_out_{}{}_{}_{}k_{}".format(beamEnergy[iBeam],energyUnit,geometry[iGeomtry],eventsNum//1000, 1001+iJob)
            writeQsimRunMacro(macroFileName,outRootFile,beamEnergy[iBeam],eventsNum, seed[iJob])

            jobSubmitFileName = "jobsubmit_{}{}_{}_{}k_{}.sh".format(beamEnergy[iBeam], energyUnit, geometry[iGeomtry], eventsNum//1000, 1001+iJob)
            jobOutErrName = "qsim_{}{}_{}".format(beamEnergy[iBeam],energyUnit,geometry[iGeomtry])
            geometryFile = geometry[iGeomtry]+".gdml"
            jobName = "{}{}-{}-qsim".format(beamParticle,iBeam,iGeomtry)
            writeJobSubmitScript(jobSubmitFileName,jobOutErrName, geometryFile, macroFileName, jobName)

            os.system("sbatch " + jobDir + jobSubmitFileName)     
