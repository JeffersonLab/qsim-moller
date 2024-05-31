# Description: This script creates the run macro file and job submit file for qsim and submits the job to ifarm
#              It submits job for smRetroQsim geometry and different y positions to mimic the MAMI longitudinal scan 
# Change: config, beamEnergy, geometry, eventsNum and sourceDir as needed

import os
import random
import time
#import numpy as np

# Define variables
config = "qsim_50"
nJobs = 0
beamParticle = "e-"
beamEnergy = 2
energyUnit = "GeV"
geometry = "showerMaxDetector_v3-3-2"
xMinDet = 0
xMaxDet = 160
yMinDet = 0
yMaxDet = 265
step = 20       # distance between two positions
eventsNum = 10000
posXvalueList = []
posYvalueList = []
fileSplit = 1 # Number of rootfiles for each position
# Create list of x and y positions to scan with xy coordinates from qsim (x up, y left)
for i in range(xMinDet, xMaxDet+1, step):     # Scan 160 mm in 5 mm steps
    posXvalueList.append(int(i- xMaxDet/2))
for i in range(yMinDet, yMaxDet, step):   # Scan 265 mm in 5 mm steps
    posYvalueList.append(int(i- yMaxDet/2+2.5))

# Print the list of x and y positions
print("X positions: ", posXvalueList)
print("Y positions: ", posYvalueList)

# Define source directory for mac or ifarm (machine dependent)
sourceDir = ""
outRootFileDir = ""

if os.uname() == 'Darwin':
    sourceDir = "/Users/sudip/programs/qsim/qsim-showermax/"
    outRootFileDir = sourceDir + "rootfiles/" + config + "/"
else:
    sourceDir = "/w/halla-scshelf2102/moller12gev/sudip/qsim/qsimTest/qsim-showermax/"
    outRootFileDir = "/lustre19/expphy/volatile/halla/moller12gev/sudip/qsim_rootfiles/" + config + "/"

print("Source directory: ", sourceDir)
logDir = sourceDir + "slurm_job/job_log/" + config + "/"
macroDir = sourceDir + "slurm_job/macros/" + config + "/"
jobDir = sourceDir + "slurm_job/sbatch_scripts/" + config + "/"

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
def writeQsimRunMacro(macroName:str, xValue:float, yValue:float, outFileName="qsim_out.root", beamEnergy=855, events=100, seed = 50):
    '''Creates a macro file to run in qsim'''
    file = open(macroDir + macroName, "w")
    file.write("/qsim/fSourceMode 1\n")
    file.write("/run/initialize\n")
    file.write("/process/optical/boundary/setInvokeSD true\n")
    file.write("/qsim/filename " + outRootFileDir + outFileName + ".root\n")
    file.write("#/qsim/seed {}\n".format(seed))
    file.write("/qsim/xmin {} mm\n".format(xValue))
    file.write("/qsim/xmax {} mm\n".format(xValue))
    file.write("/qsim/ymin {} mm\n".format(yValue))
    file.write("/qsim/ymax {} mm\n".format(yValue))
    file.write("/qsim/zmin -100 mm\n")
    file.write("/qsim/zmax -100 mm\n")
    file.write("/qsim/emin {} {}\n".format(beamEnergy, energyUnit))
    file.write("/qsim/emax {} {}\n".format(beamEnergy, energyUnit))
    file.write("/gun/particle " + beamParticle + "\n")
    file.write("/run/beamOn " + str(events) + "\n")
    file.close
    print("\nRun macro " + macroName + " created.")

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
    file.write("#SBATCH --mem=300M\n")
    file.write("#SBATCH --output=" + logDir + jobOutErrName + ".out\n")
    file.write("#SBATCH --error=" + logDir + jobOutErrName + ".err\n")
    #file.write("source /site/12gev_phys/softenv.sh 2.4\n\n")
    file.write("cd " + sourceDir + "\n")
    file.write("./build/qsim geometry/" +geometryGDML + " " + macroDir + macro)
    file.close
    print("Ifarm job submission script " + scriptName + " created.")

# Use loop to create different macros and job submission file and sbatch them
jobCount = 0
for iPosX in range(len(posXvalueList)):
    for iPosY in range(len(posYvalueList)):
        for iFile in range(1,fileSplit+1):
            macroFileName = "runbatch_{}_{}{}_{}x_{}y_{}k_{}.mac".format(geometry, beamEnergy, energyUnit, posXvalueList[iPosX], posYvalueList[iPosY], eventsNum//1000, 1000+iFile)
            outRootFile = "qsim_out_{}_{}{}_{}x_{}y_{}k_{}".format(geometry, beamEnergy, energyUnit, posXvalueList[iPosX], posYvalueList[iPosY], eventsNum//1000, 1000+iFile)
            writeQsimRunMacro(macroFileName, posXvalueList[iPosX], posYvalueList[iPosY], outRootFile, beamEnergy, eventsNum)
            
            jobSubmitFileName = "jobsubmit_{}_{}{}_{}x_{}y_{}k_{}.sh".format(geometry, beamEnergy, energyUnit, posXvalueList[iPosX], posYvalueList[iPosY], eventsNum//1000, 1000+iFile)
            jobOutErrName = "job_{}_{}{}_{}x_{}y_{}k_{}".format(geometry, beamEnergy, energyUnit, posXvalueList[iPosX], posYvalueList[iPosY], eventsNum//1000, 1000+iFile)
            geometryFile = geometry+".gdml"
            jobName = "{}x{}y{}".format(posXvalueList[iPosX], posYvalueList[iPosY],iFile) # Displays only 8 chars by default in squeue
            writeJobSubmitScript(jobSubmitFileName,jobOutErrName, geometryFile, macroFileName, jobName)
            
            os.system("sbatch " + jobDir + jobSubmitFileName)
            jobCount += 1
            print("Job submitted: ", jobCount)
