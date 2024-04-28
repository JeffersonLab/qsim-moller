/*
Reads the root files from qsim for each X and Y position of the electron beam hit
    and writes the mean PE yield in a csv file
The slurm job submit script is in slurm_job/submit_qsim_mami_fullScan.py 
    which has the X and Y positions of the electron beam hit.
*/

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "TROOT.h"
#include "TChain.h"
#include "TString.h"
#include "TFile.h"
#include "TH1F.h"
#include "TTree.h"
#include "TSystem.h"

using namespace std;

void write_csv_fullScan_hitn(){
    string config = "qsim_47";
    string particle = "e-";
    float beamEnergy = 855;
    string geometry = "showerMaxRetro_v2-7-2";

    // X and Y positions of the electron beam hit
    int xMinDet = 0;
    int xMaxDet = 160;
    int yMinDet = 0;
    int yMaxDet = 260; // used 260 instead of 265
    vector<float> posXvalueVector = {};
    vector<float> posYvalueVector = {};
    for (int i = xMinDet; i < xMaxDet+1; i+=5){ 
        posXvalueVector.push_back(i - (xMaxDet/2)); // forced to make an integer
    }
    for (int i = yMinDet; i < yMaxDet+1; i+=5){
        posYvalueVector.push_back(i - yMaxDet/2);
    }

    // Cout the X and Y positions of the electron beam hit
    cout << "X positions of the electron beam hit: ";
    for (int i = 0; i < posXvalueVector.size(); i++){
        cout << posXvalueVector[i] << " ";
    }
    cout << endl;
    for (int i = 0; i < posYvalueVector.size(); i++){
        cout << posYvalueVector[i] << " ";
    }
    cout << endl;

    //int fileSplit = 1;

    int hist_xmin = 0;
    int hist_xmax = 200;
    int nbins = 200;

    // Determine the dependent variables
    #if defined(__APPLE__)
        TString inRootfile_dir = Form("/Users/sudip/programs/qsim/qsim-showermax/rootfiles/%s/", config.c_str());
	#elif __linux
		TString inRootfile_dir = Form("/volatile/halla/moller12gev/sudip/qsim_rootfiles/%s/", config.c_str());
    #endif

    //string inFileDir = Form("~/programs/qsim/qsim-showermax/rootfiles/%s/", config.c_str());


    // Write the csv file
    gSystem->Exec(Form("mkdir -p ./output/%s", config.c_str()));
    ofstream outfile_mean, outfile_res;
    outfile_mean.open(Form("./output/%s/hitn_mean_fullScan_%s_1.csv",config.c_str(), geometry.c_str()));
    outfile_res.open(Form("./output/%s/hitn_res_fullScan_%s_1.csv",config.c_str(), geometry.c_str()));

    // Write the header of the csv file
    outfile_mean << "Mean PE yield for " << geometry << " at " << beamEnergy << " MeV\n" << Form("Date-time:%s-%s\n", __DATE__, __TIME__);
    outfile_res << "PE yield resolution for " << geometry << " at " << beamEnergy << " MeV\n" << Form("Date-time:%s-%s\n", __DATE__, __TIME__);

    outfile_mean << "X\\Y,";
    outfile_res << "X\\Y,";
    for (int i = 0; i < posYvalueVector.size(); i++){
        outfile_mean << posYvalueVector[i] << ",";
        outfile_res << posYvalueVector[i] << ",";
    }
    outfile_mean << endl;
    outfile_res << endl;

    // Read the root files from qsim for each X and Y position of the electron beam hit
    for (int i = 0; i < posXvalueVector.size(); i++){
        outfile_mean << posXvalueVector[i] << ",";
        outfile_res << posXvalueVector[i] << ",";
        for (int j = 0; j < posYvalueVector.size(); j++){
            TString inRootFileName = Form("qsim_out_%s_%.0fMeV_%0.0fx_%0.0fy_10k.root", geometry.c_str(), beamEnergy, posXvalueVector[i], posYvalueVector[j]);
            cout << "Reading " << inRootfile_dir + inRootFileName << endl;

            double mean_hist;
            double res_hist;
            TFile * inFile = new TFile(inRootfile_dir + inRootFileName, "read");
            // Continue if the file is not found or is a zombie
            if (!inFile || inFile->IsZombie()){
                cout << "File not found or is a zombie" << endl;
                mean_hist = 0;
                res_hist = 0;
                continue;
            }

            TTree * T = (TTree *)inFile->Get("T");
            TH1F* h_hitn;
            Int_t hitn;
            T->SetBranchAddress("hit.n", &hitn);

            T->Draw("hit.n>>hitn", "", "goff");
            h_hitn = (TH1F*)gDirectory->FindObject("hitn");
            mean_hist = h_hitn->GetMean();
            res_hist = h_hitn->GetRMS()/mean_hist;

            outfile_mean << mean_hist << ",";
            outfile_res << res_hist << ",";

            cout << "Mean: " << mean_hist << " Res: " << res_hist << endl;
            
            delete inFile;
        }
        outfile_mean << endl;
        outfile_res << endl;
    }
    outfile_mean.close();
    outfile_res.close();
}    