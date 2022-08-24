# This script submits the job to ifarm

from asyncio import events
import os

beamEnergy = [2]#, 5.5, 8}
geometry = ["smBenchmark1quartzQsim.gdml"]
            #"smBenchmark1stackQsim.gdml",
            #"smBenchmark2stackQsim.gdml",
            #"smBenchmark3stackQsim.gdml",
            #"smBenchmark4stackQsim.gdml",
            #"showermaxQsim.gdml"}
eventsNum = 10
sourceDir = "/home/sudip/programs/qsim/qsim-showermax/"
#sourceDir = "/w/halla-scshelf2102/moller12gev/sudip/qsim/qsim-showermax"
jobDir = sourceDir + "slurm_job/"
logDir = sourceDir + "slurm_job/job_log/"
macroDir = sourceDir + "slurm_job/macros/"

if not os.path.exists(logDir):
    os.mkdir(logDir)
if not os.path.exists(macroDir):
    os.mkdir(macroDir)

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
    #file.write("source /site/12gev_phys/softenv.sh 2.4\n\n")
    file.write("cd " + sourceDir + "\n")
    file.write("./build/qsim geometry/" +geometryGDML + " slurm_job/macros/" + macro)
    file.close
    print("Ifarm job submission script " + scriptName + " created.")

#writeQsimRunMacro("runexample.mac", events=eventsNum)
#writeJobSubmitScript("testJob", "job1", "sm.gdml", "testMacro.mac")

for iBeam in range(len(beamEnergy)):
    for iGeomtry in range(len(geometry)):
        macroFileName = "runbatch_{}GeV_{}k.mac".format(beamEnergy[iBeam],eventsNum//1000)
        outRootFile = "qsim_out_{}GeV_{}_{}k".format(beamEnergy[iBeam],geometry[iGeomtry],eventsNum//1000)
        writeQsimRunMacro(macroFileName,outRootFile,beamEnergy[iBeam],eventsNum)

        jobSubmitFileName = "jobsubmit_{}GeV_{}k.sh".format(beamEnergy[iBeam],eventsNum//1000)
        jobOutErrName = "qsim_{}GeV_{}".format(beamEnergy[iBeam], geometry[iGeomtry])
        writeJobSubmitScript(jobSubmitFileName,jobOutErrName, geometry[iGeomtry], macroFileName)

        os.system("sh " + jobDir + jobSubmitFileName)     
