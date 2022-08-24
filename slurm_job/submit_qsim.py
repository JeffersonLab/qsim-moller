# Description: This script creates the run macro file and job submit file for qsim and submits the job to ifarm
#              It submits job for multiple gdml geomerties and different energies
# Change: config, beamEnergy

import os

# Define variables
config = "stackStudy_cathRefl0125"
beamEnergy = [2, 5, 8]
geometry = ["smBenchmark1quartzQsim",
            "smBenchmark1stackQsim",
            "smBenchmark2stackQsim",
            "smBenchmark3stackQsim",
            "smBenchmark4stackQsim",
            "showermaxQsim"]
eventsNum = 10000
sourceDir = "/home/sudip/programs/qsim/qsim-showermax/"
#sourceDir = "/w/halla-scshelf2102/moller12gev/sudip/qsim/qsim-showermax"
logDir = sourceDir + "slurm_job/job_log/" + config + "/"
macroDir = sourceDir + "slurm_job/macros/" + config + "/"
jobDir = sourceDir + "slurm_job/sbatch_scripts/" + config + "/"
print (jobDir)

# Make directories if required
if not os.path.exists(logDir):
    os.makedirs(logDir)
if not os.path.exists(macroDir):
    os.makedirs(macroDir)
if not os.path.exists(jobDir):
    os.makedirs(jobDir)

# Define functions
def writeQsimRunMacro(macroName:str, outFileName="qsim_out.root", beamEnergy=2, events=100):
    '''Creates a macro file to run in qsim'''
    file = open(macroDir + macroName, "w")
    file.write("/qsim/fSourceMode 1\n")
    file.write("/run/initialize\n")
    file.write("/qsim/filename rootfiles/" + outFileName + ".root\n")
    file.write("/qsim/seed 50\n")
    file.write("/qsim/emin " + str(beamEnergy) + " GeV\n")
    file.write("/qsim/emax " + str(beamEnergy) + " GeV\n")
    file.write("/gun/particle e-\n")
    file.write("/run/beamOn " + str(events) + "\n")
    file.close
    print("\nRun macro " + macroFileName + " created.")

def writeJobSubmitScript(scriptName:str, jobOutErrName:str, geometryGDML:str, macro:str):
    '''Creates a job submission script to run qsim in the Jlab ifarm'''
    file = open(jobDir + scriptName, "w")
    file.write("#!/bin/bash\n\n")
    file.write("#SBATCH --account=halla\n")
    file.write("#SBATCH --partition=production\n")
    file.write("#SBATCH --job-name=QSIM\n")
    file.write("#SBATCH --mail-user=sudip@jlab.org\n")
    file.write("#SBATCH --mail-type=FAIL,END\n")
    file.write("#SBATCH --time=1-23:59:59\n")
    file.write("#SBATCH --nodes=1\n")
    file.write("#SBATCH --ntasks=1\n")
    file.write("#SBATCH --cpus-per-task=2\n")
    file.write("#SBATCH --mem=6G\n")
    file.write("#SBATCH --output= " + logDir + jobOutErrName + ".out\n")
    file.write("#SBATCH --error= " + logDir + jobOutErrName + ".err\n")
    file.write("source /site/12gev_phys/softenv.sh 2.4\n\n")
    file.write("cd " + sourceDir + "\n")
    file.write("./build/qsim geometry/" +geometryGDML + " slurm_job/macros/" + macro)
    file.close
    print("Ifarm job submission script " + scriptName + " created.")

# Use loop to create different macros and job submission file and sbatch them
for iBeam in range(len(beamEnergy)):
    for iGeomtry in range(len(geometry)):
        macroFileName = "runbatch_{}GeV_{}_{}k.mac".format(beamEnergy[iBeam], geometry[iGeomtry], eventsNum//1000)
        outRootFile = "qsim_out_{}GeV_{}_{}k".format(beamEnergy[iBeam],geometry[iGeomtry],eventsNum//1000)
        writeQsimRunMacro(macroFileName,outRootFile,beamEnergy[iBeam],eventsNum)

        jobSubmitFileName = "jobsubmit_{}GeV_{}_{}k.sh".format(beamEnergy[iBeam],geometry[iGeomtry], eventsNum//1000)
        jobOutErrName = "qsim_{}GeV_{}".format(beamEnergy[iBeam], geometry[iGeomtry])
        geometryFile = geometry[iGeomtry]+".gdml"
        writeJobSubmitScript(jobSubmitFileName,jobOutErrName, geometryFile, macroFileName)

        #os.system("sh " + jobDir + jobSubmitFileName)     
