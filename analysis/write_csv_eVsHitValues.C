// Plots PE response for electron beam
// the graph of energy of e- beam vs mean PE hit distribution from qsim

#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include "TROOT.h"
#include "TStyle.h"
#include "TGaxis.h"
#include "TFile.h"
#include "TH1I.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TPaveStats.h"
#include "TSystem.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TF1.h"
#include "TLegend.h"

using namespace std;

void write_csv_eVsHitValues(){
    //gROOT->Reset();
    //gStyle->SetOptStat("neMR");
    //gStyle->SetTitleYOffset(1.3);
    //gStyle->SetPadGridX(1);
    //gStyle->SetPadGridY(1);
    gStyle->SetOptFit();
    //TGaxis::SetMaxDigits(3);
    
    string config = "qsim_60";
    string particle = "e";
    string geometry = "showerMaxDetector_v3-1-2";
    int color[] = {kBlack, kRed, kBlue, kGreen+2, kMagenta,};

    vector<float> beamEnergy;
    vector<float> meanHits;
    vector<float> meanHitsError;
    vector<float> peakHit;
    vector<float> rms;
    vector<float> rmsError;
    vector<float> detRes;
    vector<string> inRootFileName;

    string inFileDir = Form("/volatile/halla/moller12gev/sudip/qsim_rootfiles/%s/",config.c_str());
    //string inFileDir = "~/programs/qsim/qsim-moller/rootfiles/qsim_08/";

    for (float iBeamEnergy=1.0; iBeamEnergy<5.0; iBeamEnergy+=1.0){
        beamEnergy.push_back(iBeamEnergy);
    }
    for (float iBeamEnergy=5.0; iBeamEnergy<100.0; iBeamEnergy+=5.0){
        beamEnergy.push_back(iBeamEnergy);
    }
    for (float iBeamEnergy=100.0; iBeamEnergy<1000.0; iBeamEnergy+=100.0){
        beamEnergy.push_back(iBeamEnergy);
    }
    for (float iBeamEnergy=1000.0; iBeamEnergy<8000.0; iBeamEnergy+=500.0){
        beamEnergy.push_back(iBeamEnergy);
    }

    TH1F *h_hitn[beamEnergy.size()];

    for (int i=0; i<beamEnergy.size(); i++){
        inRootFileName.push_back(Form("qsim_out_%.1fGeV_%s_10k_1001.root", beamEnergy.at(i), geometry.c_str()));
        string inRootFile = inFileDir + inRootFileName.at(i);

        TFile *inFile = new TFile(inRootFile.c_str(), "READ");
        TTree *T = (TTree*)inFile->Get("T");
        T->Draw(Form("hit.n>>hitn_%.1fGeV", beamEnergy.at(i)), "", "goff");

        h_hitn[i] = (TH1F*)gDirectory->FindObject(Form("hitn_%.1fGeV", beamEnergy.at(i)));
        
        meanHits.push_back(h_hitn[i]->GetMean());
        meanHitsError.push_back(h_hitn[i]->GetMeanError());

        peakHit.push_back(h_hitn[i]->GetMaximum());

        rms.push_back(h_hitn[i]->GetRMS());
        rmsError.push_back(h_hitn[i]->GetRMSError());
        
        float rms = h_hitn[i]->GetRMS();
        detRes.push_back(rms/meanHits[i]);

        cout << "Beam energy: " << beamEnergy[i] << " GeV  " << "Mean PE: " << meanHits[i] << endl; 
        //cout << meanHits[i] << " " << peakHit[i] << endl;
        //cout << beamEnergy[i] << " " << detRes[i] << endl;
    }

    // Write to a file
    gSystem->Exec(Form("mkdir -p ./output/files/%s",config.c_str()));
    ofstream outFile;
    outFile.open(Form("./output/files/%s/eVsHitResults_%s_%s.csv",config.c_str(), geometry.c_str(),particle.c_str()));

    outFile << "Beam energy [MeV], Mean PE, mean PE error, Peak PE, rms, rms error, det resolution\n";
    for (int i=0; i<beamEnergy.size(); i++){
        outFile << beamEnergy[i] << ", " << meanHits[i] << ", " << meanHitsError[i] << ", " << peakHit[i] << ", " << rms[i] << ", " << rmsError[i] << ", " << detRes[i] << "\n";
    }
    outFile.close();

}