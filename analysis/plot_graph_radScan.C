// Plots PE response vs longitudinal scan position
// the graph of x hit position of e- beam vs mean PE hit distribution from qsim

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

void plot_graph_radScan(){
    //gROOT->Reset();
    //gStyle->SetOptStat("neMR");
    //gStyle->SetTitleYOffset(1.3);
    //gStyle->SetPadGridX(1);
    //gStyle->SetPadGridY(1);
    gStyle->SetOptFit();
    //TGaxis::SetMaxDigits(3);
    
    string config = "qsim_40";
    string particle = "e";
    string geometry = "smRetroQsim";
    int color[] = {kBlack, kRed, kBlue, kGreen+2, kMagenta,};

    vector<float> radialPosition;
    vector<float> meanHits;
    vector<float> meanHitsError;
    vector<float> detRes;
    vector<string> inRootFileName;

 
    string inFileDir = Form("/volatile/halla/moller12gev/sudip/qsim_rootfiles/%s/",config.c_str());
    //string inFileDir = "/Users/sudip/test/qsim_rootfiles/qsim_32/";

    for (float iRadPos=-50.0; iRadPos<=50; iRadPos+=10.0){
        radialPosition.push_back(iRadPos);
    }

    TH1F *h_hitn[radialPosition.size()];

    for (int i=0; i<radialPosition.size(); i++){
        inRootFileName.push_back(Form("qsim_out_855MeV_%s_10k_%d.root", geometry.c_str(), 1001+i));
        string inRootFile = inFileDir + inRootFileName.at(i);

        TFile *inFile = new TFile(inRootFile.c_str(), "READ");
        TTree *T = (TTree*)inFile->Get("T");
        T->Draw(Form("hit.n>>hitn_y=%.0fmm", radialPosition.at(i)), "", "goff");

        h_hitn[i] = (TH1F*)gDirectory->FindObject(Form("hitn_y=%.0fmm", radialPosition.at(i)));
        
        meanHits.push_back(h_hitn[i]->GetMean());
        meanHitsError.push_back(h_hitn[i]->GetMeanError());

        float rms = h_hitn[i]->GetRMS();
        detRes.push_back(rms/meanHits[i]);

        //cout << meanHits[i] << " " << radialPosition[i] << endl;
        //cout << radialPosition[i] << " " << detRes[i] << endl;

    }
    // Draw y-pos energy vs mean PE
    TCanvas* c1 = new TCanvas("c1", "radial pos vs mean PE");
    TGraphErrors *grMean = new TGraphErrors(radialPosition.size(), &radialPosition[0], &meanHits[0], 0, &meanHitsError[0]);
    grMean->SetTitle("radial scan ; radial pos of e beam hit [in mm]; PE yield");
    grMean->SetMarkerStyle(20);
    grMean->SetDrawOption("AP");
    grMean->SetMarkerColor(kRed);
    //grMean->GetYaxis()->SetRangeUser(-50, 400);
    grMean->Draw("AP");
    gPad->SetGrid();

    gPad->Update();

    //Save canvas
    gSystem->Exec(Form("mkdir -p plots/%s/",config.c_str()));
    c1->SaveAs(Form("./plots/%s/eVsMeanHit_%s_%s.pdf",config.c_str(), geometry.c_str(),particle.c_str()));

    //// Draw beam energy vs det resolution
    //TCanvas* c3 = new TCanvas("c3", "Energy vs det resolution");
    //TGraph *grRes = new TGraph(radialPosition.size(), &radialPosition[0], &detRes[0]);
    //grRes->SetTitle("Electron beam energy vs det resolution; Beam energy [in GeV]; RMS/Mean");
    //grRes->Draw();

    // Save position vs mean PE to text file
    ofstream outTextFile;
    gSystem->Exec(Form("mkdir -p output_files/%s/",config.c_str()));
    outTextFile.open(Form("./output_files/%s/meanHitVsRadPos_%s_%s.txt",config.c_str(), geometry.c_str(),particle.c_str()));
    for (int i=0; i<radialPosition.size(); i++){
        outTextFile << radialPosition[i] << " " << meanHits[i] << " " << meanHitsError[i] << endl;
    }
    outTextFile.close();


}
