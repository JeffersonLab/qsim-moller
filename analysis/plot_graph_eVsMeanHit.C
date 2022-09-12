// This script plots the graph of energy vs mean/Peak hit distribution from qsim

#include <string>
#include <iostream>
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

using namespace std;

void plot_graph_eVsMeanHit(){
    //gROOT->Reset();
    //gStyle->SetOptStat("neMR");
    //gStyle->SetTitleYOffset(1.3);
    //gStyle->SetPadGridX(1);
    //gStyle->SetPadGridY(1);
    //TGaxis::SetMaxDigits(3);
    
    string config = "qsim_08";
    string particle = "e";
    string geometry = "smBenchmark1stackQsim";
    int color[] = {kBlack, kRed, kBlue, kGreen+2, kMagenta,};

    vector<float> beamEnergy;
    vector<float> meanHits;
    vector<float> meanHitsError;
    vector<float> peakHit;
    vector<float> detRes;
    vector<string> inRootFileName;

    //string inFileDir = Form("/volatile/halla/moller12gev/sudip/qsim_rootfiles/%s/",config.c_str());
    string inFileDir = "~/programs/qsim/qsim-showermax/rootfiles/";

    for (float iBeamEnergy=2; iBeamEnergy<=8; iBeamEnergy+=3){
        beamEnergy.push_back(iBeamEnergy);
    }

    TH1F *h_hitn[beamEnergy.size()];

    for (int i=0; i<beamEnergy.size(); i++){
        inRootFileName.push_back(Form("qsim_out_%.fGeV_%s_10k.root", beamEnergy.at(i), geometry.c_str()));
        string inRootFile = inFileDir + inRootFileName.at(i);

        TFile *inFile = new TFile(inRootFile.c_str(), "READ");
        TTree *T = (TTree*)inFile->Get("T");
        T->Draw(Form("hit.n>>hitn_%.0fGeV", beamEnergy.at(i)), "hit.pid==0", "goff");

        h_hitn[i] = (TH1F*)gDirectory->FindObject(Form("hitn_%.0fGeV", beamEnergy.at(i)));
        
        meanHits.push_back(h_hitn[i]->GetMean());
        meanHitsError.push_back(h_hitn[i]->GetMeanError());

        peakHit.push_back(h_hitn[i]->GetMaximum());
        
        float rms = h_hitn[i]->GetRMS();
        detRes.push_back(meanHits[i]/rms);

        //cout << meanHits[i] << " " << peakHit[i] << endl;
        cout << beamEnergy[i] << " " << detRes[i] << endl;

    }

    // Draw beam energy vs mean PE
    TCanvas* c1 = new TCanvas("c1", "Energy vs mean PE");
    TGraphErrors *grMean = new TGraphErrors(beamEnergy.size(), &beamEnergy[0], &meanHits[0], 0, &meanHitsError[0]);
    grMean->SetTitle("Mean PE yield; Energy of e beam [GeV]; PE yield");
    grMean->SetMarkerStyle(20);
    grMean->SetTitle("Mean PE");
    grMean->SetDrawOption("AP");
    grMean->SetMarkerColor(kRed);
    grMean->Draw("AP");

    // Draw beam energy vs peak PE
    //TCanvas* c2 = new TCanvas("c2", "Energy vs peak PE");
    TGraph *grPeak = new TGraph(beamEnergy.size(), &beamEnergy[0], &peakHit[0]);
    grPeak->SetMarkerStyle(30);
    grPeak->SetTitle("Peak PE");
    grPeak->SetDrawOption("AP");
    grPeak->SetMarkerColor(kBlue);
    //grPeak->Draw();
    
    // Draw beam energy vs det resolution
    TCanvas* c3 = new TCanvas("c3", "Energy vs det resolution");
    TGraph *grRes = new TGraph(beamEnergy.size(), &beamEnergy[0], &detRes[0]);
    grRes->SetTitle("Electron beam energy vs det resolution; Beam energy [in GeV]; RMS/Mean");
    grRes->Draw();

    //// Draw multigraph of beam energy vs mean and peak PE
    //TCanvas* c4 = new TCanvas("c4", "energy vs PE");
    //TMultiGraph *mg = new TMultiGraph("mg", "mg");
    //mg->SetTitle("Energy vs PE; ELectron beam energy [GeV]; PE count");
    //mg->Add(grMean, "lp");
    //mg->Add(grPeak, "cp");
    //mg->Draw("ap");

}