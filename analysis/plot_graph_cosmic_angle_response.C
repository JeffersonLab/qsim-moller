// Plots graph of PE response / Resolution vs angle of cosmic muons

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

void plot_graph_cosmic_angle_response(){
    //gROOT->Reset();
    //gStyle->SetOptStat("neMR");
    //gStyle->SetTitleYOffset(1.3);
    //gStyle->SetPadGridX(1);
    //gStyle->SetPadGridY(1);
    gStyle->SetOptFit();
    //TGaxis::SetMaxDigits(3);
    
    string config = "qsim_44";
    string particle = "mu";
    string geometry = "smBenchmark4stackQsim";
    int color[] = {kBlack, kRed, kBlue, kGreen+2, kMagenta,};

    vector<float> muonAngle;
    vector<float> meanHits;
    vector<float> meanHitsError;
    vector<float> detRes;
    vector<string> inRootFileName;

 
    string inFileDir = Form("/volatile/halla/moller12gev/sudip/qsim_rootfiles/%s/",config.c_str());
    //string inFileDir = "/Users/sudip/test/qsim_rootfiles/qsim_32/";

    for (float iMuonAngle=0; iMuonAngle<=5; iMuonAngle+=0.5){
        muonAngle.push_back(iMuonAngle);
    }

    TH1F *h_hitn[muonAngle.size()];

    for (int i=0; i<muonAngle.size(); i++){
        inRootFileName.push_back(Form("qsim_out_%.1fdeg_%s_100k_1001.root", muonAngle[i], geometry.c_str()));
        string inRootFile = inFileDir + inRootFileName.at(i);

        TFile *inFile = new TFile(inRootFile.c_str(), "READ");
        TTree *T = (TTree*)inFile->Get("T");
        T->Draw(Form("hit.n>>hitn_y=%.0fmm", muonAngle.at(i)), "", "goff");

        h_hitn[i] = (TH1F*)gDirectory->FindObject(Form("hitn_y=%.0fmm", muonAngle.at(i)));
        
        meanHits.push_back(h_hitn[i]->GetMean());
        meanHitsError.push_back(h_hitn[i]->GetMeanError());

        float rms = h_hitn[i]->GetRMS();
        detRes.push_back(rms/meanHits[i]);

        //cout << meanHits[i] << " " << muonAngle[i] << endl;
        //cout << muonAngle[i] << " " << detRes[i] << endl;

    }
    // Draw angular range vs mean PE
    TCanvas* c1 = new TCanvas("c1", "Muons angle vs mean PE");
    TGraphErrors *grMean = new TGraphErrors(muonAngle.size(), &muonAngle[0], &meanHits[0], 0, &meanHitsError[0]);
    grMean->SetTitle("Muons angle range response ; Angular range of the muons [+- deg]; PE yield");
    grMean->SetMarkerStyle(20);
    grMean->SetDrawOption("AP");
    grMean->SetMarkerColor(kRed);
    //grMean->GetYaxis()->SetRangeUser(-50, 400);
    grMean->Draw("AP");
    gPad->SetGrid();

    gPad->Update();

    //Save canvas
    gSystem->Exec(Form("mkdir -p plots/%s/",config.c_str()));
    c1->SaveAs(Form("./plots/%s/angularResponse%s_%s1.pdf",config.c_str(), geometry.c_str(),particle.c_str()));

    // Draw angular range vs det resolution
    TCanvas* c2 = new TCanvas("c2", "Angle range vs det resolution");
    TGraph *grRes = new TGraph(muonAngle.size(), &muonAngle[0], &detRes[0]);
    grRes->SetTitle("Muons angle vs det res; Angular range of the muons [+- deg]; RMS/Mean");
    grRes->Draw();
 
    c2->SaveAs(Form("./plots/%s/angularResponseResolution_%s_%s1.pdf",config.c_str(), geometry.c_str(),particle.c_str()));

}
