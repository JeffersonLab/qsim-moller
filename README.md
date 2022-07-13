# Quartz simulation (Qsim) of Shower-max for MOLLER Experiment at Jefferson Lab

This version of qsim is dedicated to showermax detector. It has the latest geometries, materials, and optical properties.

Original developer:
Seamus Riordan
sriordan@physics.umass.edu
September 26, 2013(Updated June 26, 2015)

GDML version developer:
Carlos Bula Villareal

Sudip Bhattarai
sudipbhattarai@isu.edu
July 12, 2022

## qsim installation and running instructions
Tested and ran on Ubuntu 22 inside UTM in macOS 12

Requirements
* Geant4 >= 4.10
* cmake > 2.6
* root

### Download the qsim-showermax
To download the code, use git clone:
```
git clone https://github.com/sudipbhattarai/qsim-showermax
```
### Compilation

To build, create the directory you would like to build in, say `build`:
```
mkdir build
cd build
cmake <path to qsim>
make
```

### Running simulation
There are sets of command inside the file macros/runexample.mac which is useful to run in batch or visual mode.

To run in batch mode, execute with a macro such as:
```
./qsim gdml_file.gdml runexample.mac
```
Ensure that all macros include a /run/initialization command or else they will
not work. 

Visualization:

Visualization macros are found in vis/

To run, execute
```
./qsim gdml_file.gdml
```
which should bring up a graphical command interface and it should load the gdml file.

To see the particle visualization:
```
/control/execute macros/runexample.mac
```
Make sure the number of beam (/run/beamOn) in the runexample.mac file is not large (just 1 is good for showermax)

The output root file will have several variables that is described in [readme file](README.variables.md).

CLI User Commands

Using the Geant4 CLI it is possible to pass commands to modify behavior
and utilize the vis.mac macro from the command line.
These are all visible from the menu on the left.

## Operational Mode Switches

There are CLI User Commands that allow the user to change the stand design and
incident particle characteristics to test different configurations and 
experimental expectations. These commands must be passed before a visualization
macro is used or the initialization command have been passed.

With the implemetation of gdml files the only switches avalilable are the source switches.

## Source mode

Set by: /qsim/fSourceMode <0, 1, 2>

0 = cosmic muons
Generates primary particles following cosmic muon angular distribution and energy spectrum.  
Energy spectrum obtained from fit to PDG data for muons with 0 deg incidence (good to 25% out to 36 GeV).  
Note that this does not automatically change the primary particle type to muons; this must be set by "/gun/particle mu-"

1 = beam
Generates perfectly straight, monoenergetic beam.
Current implementation generates particles at pinpoint, but beam spot size can be changed in qsimPrimaryGeneratorAction.cc 
Energy of beam can be changed in qsimPrimaryGeneratorAction.cc

2 = remoll (not implemented yet)
Generates 2-8 GeV particles following position, angular, energy and momentum distribution obtained from remoll simulation. In remoll simulation, three sensitive detectors are placed right infront of the open(beam right), closed(beam left) and transition(beam up) SM modules and the output rootfiles is skimmed to make Event distribution root file.
The z position of primary vertex can be changed in qsimPrimaryGeneratorAction.cc, to effectively move detector closer/farther from VDC.
The distributions are stored in file RemollPrimaryDistribution.root (copied to build directory when qsim is made), which has <not yet decided> events.
 
## Stand mode

Set by: /qsim/fStandMode <0, 1>

0 = beam/PREX
Detector only.

1 = cosmic
Detector, top/bottom scintillators, and lead.
Scintillator size/separation and lead size can be adjusted in qsimDetectorConstruction.cc


## GDML GEOMETRY DESCRIPTION 
showermaxQsim.gdml contains the geometry description with their material and optical surface properties. This GDML is created using the python wrapper:
https://github.com/sudipbhattarai/remoll-showermax-generator (qsimSM branch). In the GDML,only the tungsten-quartz stack, light guides, filter, window and PMT cathode are included.

## OPTICAL PROPERTIES

Index of refraction (quartz): 
Specification sheet for Heraeus Spectrosil 2000 provides >25 data points for n(E).
Fit with polynomial.

Absorption length (quartz):
Specification sheet for Heraeus Spectrosil 2000 provides only 2 data points for L(E).
Current functional form of L(E) in qsim is of unknown origin and is inconsistent with Heraeus data points.

Transmission coefficient (quartz):

Reflectivity (loghtguide mirror):
Currently defined as a function of photon energy only (and possibly incorrect).
Needs to be defined as a function of photon energy AND photon angle (supposedly possible in Geant 4.10 but not yet implemented in qsim).

Reflectivity (Photo cathode):

Quantum Efficiency (Photo Cathode):

Surface roughness (Quartz): Glisur model




