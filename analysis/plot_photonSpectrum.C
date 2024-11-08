// This script plots the photon spectrum with several long pass filter cut applied

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
#include "TCut.h"
#include "TLegend.h"

using namespace std;

void plot_photonSpectrum(){
    //gROOT->Reset();
    gStyle->SetOptStat(0);
    //gStyle->SetTitleYOffset(1.3);
    //gStyle->SetPadGridX(1);
    //gStyle->SetPadGridY(1);
    //TGaxis::SetMaxDigits(3);
    
    string config = "qsim_07";
    string particle = "e";
    int lpValue[3] = {0, 350, 400};     //long pass filter wavelength
    string geometry = "smBenchmark1stackQsim";
    int color[] = {kBlack, kRed, kGreen+2, kBlue, kMagenta};

    int xmin = 150;
    int xmax =700;
    int nbins = 500; 

    //string inFileDir = Form("/volatile/halla/moller12gev/sudip/qsim_rootfiles/%s/",config.c_str());
    string inFileDir = "~/programs/qsim/qsim-moller/rootfiles/";
    //string inRootFileName = Form("qsim_out_8GeV_%s_10k.root", geometry.c_str());
    string inRootFileName = "qsim_out_e6.root";
    string inRootFile;
    TH1F* h_hite[3];
    TPaveStats* stat[3];
    TCut photonOnly = "hit.pid==0";
    TCut lp350nm = "(1240/(hit.e*1e9))>350";
    TCut lp400nm = "(1240/(hit.e*1e9))>400";
    Double_t wlCut[3] = {0, 350, 400};

    TCut cut[3] = {photonOnly, lp350nm, lp400nm};
    TCanvas* c1 = new TCanvas("c1", "Photon distribution");
    inRootFile = inFileDir + inRootFileName;
    TFile* inFile = new TFile(inRootFile.c_str(), "READ");
    TTree* T = (TTree*)inFile->Get("T");

    for (int i=0; i<3; i++){
        h_hite[i] = new TH1F(Form("h_lp%.0fnm", wlCut[i]), Form("Photon spectrum;Photon wavelength [in nm]; Counts/bin"),
                nbins, xmin, xmax);
        h_hite[i]->SetLineColor(color[i+1]);
        h_hite[i]->SetFillColor(color[i+1]);
    } 

    T->Draw("1240/(hit.e*1e9)>>h_lp0nm", photonOnly, "goff");
    T->Draw("1240/(hit.e*1e9)>>h_lp350nm", photonOnly&&lp350nm, "goff");
    T->Draw("1240/(hit.e*1e9)>>h_lp400nm", photonOnly&&lp400nm, "goff");

    for (int i=0; i<3; i++){
        h_hite[i]->Draw("same");
    }

    TLegend *legend = new TLegend(0.45, 0.65, 0.9, 0.9);
    legend->SetHeader("Total events: 10000");
    legend->AddEntry(h_hite[0], Form("Without filter, Entries=%.0f ", h_hite[0]->GetEntries()));
    legend->AddEntry(h_hite[1], Form("With 350 nm lp filter, Entries=%.0f" , h_hite[1]->GetEntries()));
    legend->AddEntry(h_hite[2], Form("With 400 nm lp filter, Entries=%.0f" , h_hite[2]->GetEntries()));
    legend->Draw();

    gSystem->Exec(Form("mkdir -p plots/%s/",config.c_str()));
    c1->SaveAs(Form("./plots/%s/%s_%s.pdf",config.c_str(), geometry.c_str(),particle.c_str()));
}
