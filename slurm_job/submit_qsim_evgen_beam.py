# Description: This script creates the run macro file and job submit file for qsim and submits the job to ifarm
#              It submits job for multiple gdml geomerties and different energies
# Change: config, beamEnergy, geometry, eventsNum and sourceDir as needed

import os
import sys

# Define variables
config = "qsim_30"
generator = "beam"
sector = ["open", "closed", "trans"]
geometry = "smFullscaleQsim"
eventsNum = 100
jobCount = 2

sourceDir = "/home/sudip/programs/qsim/qsim-moller/"
#sourceDir = "/w/halla-scshelf2102/moller12gev/sudip/qsim/qsim-moller/"

logDir = sourceDir + "slurm_job/job_log/" + config + "/"
macroDir = sourceDir + "slurm_job/macros/" + config + "/"
jobDir = sourceDir + "slurm_job/sbatch_scripts/" + config + "/"
#outRootFileDir = "/lustre19/expphy/volatile/halla/moller12gev/sudip/qsim_rootfiles/" + config + "/"
outRootFileDir = sourceDir + "rootfiles/" + config + "/"

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
def writeQsimRunMacro(macroName:str, evGenFileName:str, outFileName="qsim_out.root", events=100):
    '''Creates a macro file to run in qsim'''
    file = open(macroDir + macroName, "w")
    file.write("/qsim/fSourceMode 2\n")
    file.write("/run/initialize\n")
    file.write("/qsim/filename " + outRootFileDir + outFileName + ".root\n")
    file.write("/qsim/seed 50\n")
    file.write("/qsim/evgenfilename {}\n".format(evGenFileName))
    file.write("/run/beamOn " + str(events) + "\n")
    file.close
    print("\nRun macro " + macroFileName + " created.")

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
for sec in sector:
    for iJob in range(jobCount):
        macroFileName = "runbatch_beam_{}_{}.mac".format(sec, iJob)
        outRootFile = "qsim_out_{}_{}".format(sec, iJob)
        #evGenFile = "InputEventDistribution/smEvGen_beam_{}_{}.root".format(sec, iJob)
        evGenFile = "/run/user/1000/gvfs/sftp:host=sudips-mbp.local/Users/sudip/academics/Research/Scripts/moller-analysis/rootfiles/sm_qsim_v08/smEvGen_beam_{}_{}.root".format(sec, iJob)
        writeQsimRunMacro(macroFileName, evGenFile, outRootFile, eventsNum)

        jobSubmitFileName = "jobsubmit_beam_{}_{}.sh".format(sec, iJob)
        jobOutErrName = "qsim_beam_{}_{}".format(sec, iJob)
        geometryFile = geometry + ".gdml"
        jobName = "{}{}qsim".format(sec,iJob)
        writeJobSubmitScript(jobSubmitFileName,jobOutErrName, geometryFile, macroFileName, jobName)

        #os.system("sbatch " + jobDir + jobSubmitFileName)     
        os.system("sh " + jobDir + jobSubmitFileName)     
