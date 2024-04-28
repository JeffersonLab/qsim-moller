// This script plots the hit.n distribution from qsim rootfiles

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

void plot_hitn(){
    gROOT->SetBatch(kTRUE);
    //gROOT->Reset();
    gStyle->SetOptStat("eMR");
    //gStyle->SetTitleYOffset(1.3);
    //gStyle->SetPadGridX(1);
    //gStyle->SetPadGridY(1);
    gStyle->SetStatFormat("0.2g");
    gStyle->SetOptFit(1);
    TGaxis::SetMaxDigits(3);
    
    string config = "qsim_43";
    string particle = "mu-";
    float beamEnergy = 855;
    string geometry = "showerMaxDetector_v3-1-0";
    int color[] = {kBlack, kRed, kBlue, kMagenta, kGreen+2};

    int hist_xmin = 0;
    int hist_xmax = 1000;
    int nbins = 1000;
    int fileSplit = 1;
    int binWidth = (hist_xmax-hist_xmin)/nbins;

    TString inFileDir = Form("/volatile/halla/moller12gev/sudip/qsim_rootfiles/%s/",config.c_str());
    //TString inFileDir = Form("~/programs/qsim/qsim-showermax/rootfiles/%s/", config.c_str());
    TString inRootFileName[fileSplit];

    TChain *T = new TChain("T");
    for (int i=1; i<fileSplit+1; i++){
        inRootFileName[i] = Form("qsim_out_%s_100k_%d.root",geometry.c_str(), 1000+i);
        T->Add(inFileDir + inRootFileName[i]);
    }

    TH1F* h_hitn;
    TPaveStats* stat[fileSplit];
    TCanvas* c1 = new TCanvas("c1", "Photon distribution");

    Int_t hitn;
    T->SetBranchAddress("hit.n", &hitn);
    int events = T->GetEntries();

    h_hitn = new TH1F(Form("h_hitn_%.0fMeV",beamEnergy), Form("PE hits distribution of %s with %s beam; PEs; Events/%dPEs", geometry.c_str(), particle.c_str(), binWidth),
            nbins, hist_xmin, hist_xmax);
    h_hitn->SetLineColor(kBlack);
    h_hitn->SetAxisRange(0, 50);
    //h_hitn[i]->SetLineWidth(1);
    
    for (int j=0; j<events; j++){
        T->GetEntry(j);
        h_hitn->Fill(hitn);
        if (j%(events/10)==0) cout << "\tfilled " << j << " events." << endl;
    }
    
    //gPad->SetLogy();
    h_hitn->Draw();

    gPad->Update();
    float widthStat = 0.25; //width of a stat box
    float heightStat = 0.20;
    int nColStat = 1;   //number of columns of the stat boxes
    for (int i=0; i<1; i++){
        stat[i] = (TPaveStats*)h_hitn->FindObject("stats");
        stat[i]->SetTextColor(kBlack);
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
    latex.SetTextColor(kBlack);
    latex.DrawLatex(0.66, 0.65, resText);

    //h_hitn->SaveAs("./test/test.root");

    gSystem->Exec(Form("mkdir -p plots/%s/",config.c_str()));
    c1->SaveAs(Form("./plots/%s/%s_%s.pdf",config.c_str(), geometry.c_str(),particle.c_str()));

    // Sace the histogram to a root file
    gSystem->Exec(Form("mkdir -p output_files/%s/",config.c_str()));
    TFile * outFile = new TFile(Form("./output_files/%s/h_hitn_855MeV.root",config.c_str()), "RECREATE");
    h_hitn->Write();
    outFile->Close();
}
