# Description: This script creates the run macro file and job submit file for qsim and submits the job to ifarm
#              It submits job for multiple gdml geomerties and different energies
# Change: config, beamEnergy, geometry, eventsNum and sourceDir as needed

import os
import platform
import ROOT

# Define variables
config = "qsim_60"
generator = ["moller", "elastic", "inelastic"]
sector = ["open", "closed", "trans"]
beamParticle = ["electron", "gamma"]
geometry = "showerMaxDetector_v3-1-2"

# Check device and set source directory
if platform.uname().node=='Sudips-MBP.local':
    sourceDir = "/Users/sudip/programs/qsim/qsim-moller/"
    evFileDir = "/Users/sudip/Documents/Academics/Research/Scripts/moller-analysis/rootfiles/sm_qsim_v09-ifarm/"
    outRootFileDir = sourceDir + "rootfiles/" + config + "/"
else:
    sourceDir = "/w/halla-scshelf2102/moller12gev/sudip/qsim/qsim-moller/"
    evFileDir = "/w/halla-scshelf2102/moller12gev/sudip/moller_sim/moller-analysis/rootfiles/sm_qsim_v09/"
    outRootFileDir = "/lustre19/expphy/volatile/halla/moller12gev/sudip/qsim_rootfiles/" + config + "/"

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
def getNumEvents(evGenFileName:str)->int:
    '''Get the number of events in the event generation file'''
    file = ROOT.TFile(evGenFileName)
    tree = file.Get("T")
    return tree.GetEntries()

def writeQsimRunMacro(macroName:str, evGenFileName:str, beamParticle:str, outFileName="qsim_out.root", events=100):
    '''Creates a macro file to run in qsim'''
    file = open(macroDir + macroName, "w")
    file.write("/qsim/fSourceMode 2\n")
    file.write("/run/initialize\n")
    file.write("/process/optical/boundary/setInvokeSD true\n")
    file.write("/qsim/filename " + outRootFileDir + outFileName + ".root\n")
    file.write("#/qsim/seed 50\n")
    file.write("/qsim/zmin -41.0 mm\n")
    file.write("/qsim/zmax -41.0 mm\n")
    if (beamParticle == "electron"):
        beamParticle = "e-"
    file.write("/gun/particle " + beamParticle + "\n")
    file.write("/qsim/evgenfilename {}\n".format(evGenFileName))
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
    file.write("#SBATCH --cpus-per-task=1\n")
    file.write("#SBATCH --mem=1G\n")
    file.write("#SBATCH --output=" + logDir + jobOutErrName + ".out\n")
    file.write("#SBATCH --error=" + logDir + jobOutErrName + ".err\n\n")
    file.write("source /.login\n")
    file.write("cd " + sourceDir + "\n")
    file.write("./build/qsim geometry/" +geometryGDML + " " + macroDir + macroFileName)
    file.close
    print("Ifarm job submission script " + scriptName + " created.")

# Use loop to create different macros and job submission file and sbatch them
for gen in generator:
    for sec in sector:
        for particle in beamParticle:
            eventsNum = getNumEvents(evFileDir + "smEvGen_{}_{}_{}.root".format(gen, sec, particle))
            # print("Number of events in the file: ", eventsNum)
            macroFileName = "runbatch_{}_{}_{}.mac".format(gen, sec, particle)
            outRootFile = "qsim_out_{}_{}_{}".format(gen, sec, particle)
            evGenFile = evFileDir + "smEvGen_{}_{}_{}.root".format(gen, sec, particle)
            writeQsimRunMacro(macroFileName, evGenFile, particle, outRootFile, eventsNum)

            jobSubmitFileName = "jobsubmit_{}_{}_{}.sh".format(gen, sec, particle)
            jobOutErrName = "qsim_{}_{}_{}".format(gen, sec, particle)
            geometryFile = geometry + ".gdml"
            jobName = "{}{}{}-qsim".format(gen[0],sec[0],particle[0])
            writeJobSubmitScript(jobSubmitFileName,jobOutErrName, geometryFile, macroFileName, jobName)

            #os.system("sbatch " + jobDir + jobSubmitFileName)     
