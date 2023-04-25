// Plots PE response vs longitudinal scan position
// the graph of y hit position of e- beam vs mean PE hit distribution from qsim

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
#include "TGraph.h"

using namespace std;

void plot_graph_longScan(){
    //gROOT->Reset();
    //gStyle->SetOptStat("neMR");
    //gStyle->SetTitleYOffset(1.3);
    //gStyle->SetPadGridX(1);
    //gStyle->SetPadGridY(1);
    gStyle->SetOptFit();
    //TGaxis::SetMaxDigits(3);
    
    string config = "qsim_32";
    string particle = "e";
    string geometry = "smRetroQsim";
    int color[] = {kBlack, kRed, kBlue, kGreen+2, kMagenta,};

    vector<float> azimuthalPosition;
    vector<float> meanHits;
    vector<float> meanHitsError;
    vector<float> detRes;
    vector<string> inRootFileName;

 
    //string inFileDir = Form("/volatile/halla/moller12gev/sudip/qsim_rootfiles/%s/",config.c_str());
    string inFileDir = "/Users/sudip/test/qsim_rootfiles/qsim_32/";

    for (float iAzPos=-120.0; iAzPos<=120; iAzPos+=10.0){
        azimuthalPosition.push_back(iAzPos);
    }

    TH1F *h_hitn[azimuthalPosition.size()];

    for (int i=0; i<azimuthalPosition.size(); i++){
        inRootFileName.push_back(Form("qsim_out_855MeV_%s_10k_%d.root", geometry.c_str(), 1003+i));
        string inRootFile = inFileDir + inRootFileName.at(i);

        TFile *inFile = new TFile(inRootFile.c_str(), "READ");
        TTree *T = (TTree*)inFile->Get("T");
        T->Draw(Form("hit.n>>hitn_y=%.0fmm", azimuthalPosition.at(i)), "", "goff");

        h_hitn[i] = (TH1F*)gDirectory->FindObject(Form("hitn_y=%.0fmm", azimuthalPosition.at(i)));
        
        meanHits.push_back(h_hitn[i]->GetMean());
        meanHitsError.push_back(h_hitn[i]->GetMeanError());

        float rms = h_hitn[i]->GetRMS();
        detRes.push_back(rms/meanHits[i]);

        //cout << meanHits[i] << " " << azimuthalPosition[i] << endl;
        //cout << azimuthalPosition[i] << " " << detRes[i] << endl;

    }
    // Draw y-pos energy vs mean PE
    TCanvas* c1 = new TCanvas("c1", "Azimuthal pos vs mean PE");
    TGraphErrors *grMean = new TGraphErrors(azimuthalPosition.size(), &azimuthalPosition[0], &meanHits[0], 0, &meanHitsError[0]);
    grMean->SetTitle("Azimuthal scan ; Azimuthal pos of e beam hit [in mm]; PE yield");
    grMean->SetMarkerStyle(20);
    grMean->SetDrawOption("AP");
    grMean->SetMarkerColor(kRed);
    //grMean->GetYaxis()->SetRangeUser(-50, 400);
    grMean->Draw("AP");
    gPad->SetGrid();

    gPad->Update();

    //Save canvas
    //gSystem->Exec(Form("mkdir -p plots/%s/",config.c_str()));
    //c1->SaveAs(Form("./plots/%s/eVsMeanHit_%s_%s.pdf",config.c_str(), geometry.c_str(),particle.c_str()));

    //// Draw beam energy vs det resolution
    //TCanvas* c3 = new TCanvas("c3", "Energy vs det resolution");
    //TGraph *grRes = new TGraph(azimuthalPosition.size(), &azimuthalPosition[0], &detRes[0]);
    //grRes->SetTitle("Electron beam energy vs det resolution; Beam energy [in GeV]; RMS/Mean");
    //grRes->Draw();

    // Save position vs mean PE to text file
    ofstream outTextFile;
    gSystem->Exec(Form("mkdir -p output/%s/",config.c_str()));
    outTextFile.open(Form("./output/%s/meanHitVsAzPos_%s_%s.txt",config.c_str(), geometry.c_str(),particle.c_str()));
    for (int i=0; i<azimuthalPosition.size(); i++){
        outTextFile << azimuthalPosition[i] << " " << meanHits[i] << " " << meanHitsError[i] << endl;
    }
    outTextFile.close();


}