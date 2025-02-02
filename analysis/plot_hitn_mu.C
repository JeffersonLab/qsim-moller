// This script plots the hit.n distribution from qsim

#include <string>
#include <iostream>
#include <map>
#include "TROOT.h"
#include "TChain.h"
#include "TStyle.h"
#include "TGaxis.h"
#include "TFile.h"
#include "TH1I.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TPaveStats.h"
#include "TSystem.h"
#include "TLatex.h"

using namespace std;

void plot_hitn_mu(){
    //gROOT->Reset();
    gStyle->SetOptStat("eMR");
    //gStyle->SetTitleYOffset(1.3);
    //gStyle->SetPadGridX(1);
    //gStyle->SetPadGridY(1);
    //TGaxis::SetMaxDigits(3);
    
    string config = "localMac";
    string particle = "mu";
    float beamEnergy = 1;
    string geometry = "smRetroQsim";
    int color[] = {kBlack, kRed, kBlue, kMagenta, kGreen+2};

    int hist_xmin = 0;
    int hist_xmax = 60;
    int nbins = 60;
    int fileSplit = 1;

    //TString inFileDir = Form("/volatile/halla/moller12gev/sudip/qsim_rootfiles/%s/",config.c_str());
    TString inFileDir = Form("~/programs/qsim/qsim-showermax/rootfiles/");
    TString inRootFileName[fileSplit];

    TChain *T = new TChain("T");
    for (int i=1; i<fileSplit+1; i++){
        //inRootFileName[i] = Form("qsim_out_1GeV_mu_%s_10k_%d.root",geometry.c_str(), 1000+i);
        inRootFileName[i] = Form("qsim_out_1GeV_mu_smRetro.root");
        T->Add(inFileDir + inRootFileName[i]);
    }

    TH1F* h_hitn;
    TPaveStats* stat[fileSplit];
    TCanvas* c1 = new TCanvas("c1", "Muon distribution");

    Int_t hitn;
    T->SetBranchAddress("hit.n", &hitn);
    int events = T->GetEntries();

    //h_hitn = new TH1F(Form("h_hitn_%.1fGeV",beamEnergy), Form("PE hits distribution of %s with %s beam;PE per event; Counts/bin", geometry.c_str(), particle.c_str()),
    //        nbins, hist_xmin, hist_xmax);
    h_hitn = new TH1F(Form("h_hitn_%.1fGeV",beamEnergy), Form("Shower-max muon PE response;PEs; Events/PE"),
            nbins, hist_xmin, hist_xmax);
    h_hitn->SetLineColor(kBlue);
    h_hitn->SetAxisRange(0, 500);
    //h_hitn[i]->SetLineWidth(1);
    
    for (int j=0; j<events; j++){
        T->GetEntry(j);
        h_hitn->Fill(hitn);
        if (j%1000==0) cout << "\tfilled " << j << " events." << endl;
    }
    
    h_hitn->Draw();
     

    gPad->Update();
    float widthStat = 0.25; //width of a stat box
    float heightStat = 0.20;
    int nColStat = 1;   //number of columns of the stat boxes
    for (int i=0; i<1; i++){
        stat[i] = (TPaveStats*)h_hitn->FindObject("stats");
        stat[i]->SetTextColor(kBlue);
        stat[i]->SetX1NDC(0.9-widthStat-(i%nColStat)*widthStat);
        stat[i]->SetX2NDC(0.9-(i%nColStat)*widthStat);
        stat[i]->SetY1NDC(0.9-(i/nColStat)*heightStat);
        stat[i]->SetY2NDC(0.9-heightStat-(i/nColStat)*heightStat);
        stat[i]->Draw();
    }

    double rms = h_hitn->GetRMS();
    double mean = h_hitn->GetMean();
    double res = rms/mean;
    TString resText = Form("#frac{RMS}{Mean} = %.1f", rms/mean);

    TLatex latex;
    latex.SetNDC(1);
    latex.SetTextSize(0.03);
    latex.SetTextFont(42);
    latex.SetTextColor(kBlue);
    latex.DrawLatex(0.66, 0.63, resText);

    gSystem->Exec(Form("mkdir -p plots/%s/",config.c_str()));
    c1->SaveAs(Form("./plots/%s/%s_%s.pdf",config.c_str(), geometry.c_str(),particle.c_str()));
}
