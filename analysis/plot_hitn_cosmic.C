// This script plots the hit.n distribution from qsim rootfiles generated with cosmic stand implementation
// The qsim uses the comic muons shot from the upper scintillator height (z posn) with different possible angles 
// but this script selects only those muon events that hits the lower scintillator
// See logbook page 77 for detailed calculation

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
#include "TMath.h"

using namespace std;

void plot_hitn_cosmic(){
    //gROOT->Reset();
    gStyle->SetOptStat("eMR");
    //gStyle->SetTitleYOffset(1.3);
    //gStyle->SetPadGridX(1);
    //gStyle->SetPadGridY(1);
    gStyle->SetStatFormat("0.2g");
    gStyle->SetOptFit(1);
    TGaxis::SetMaxDigits(3);
    
    string config = "qsim_45";
    string particle = "mu";
    float beamEnergy = 855;
    string geometry = "showerMaxDetector_v3-1-0";
    int color[] = {kBlack, kRed, kBlue, kMagenta, kGreen+2};

    int hist_xmin = 0;
    int hist_xmax = 50;
    int nbins = 50;
    int fileSplit = 1;

    //string inFileDir = Form("/volatile/halla/moller12gev/sudip/qsim_rootfiles/%s/",config.c_str());
    //TString inFileDir = Form("~/test/qsim_rootfiles/%s/", config.c_str());
    TString inFileDir = Form("~/programs/qsim/qsim-showermax/rootfiles/");
    TString inRootFileName[fileSplit];

    TChain *T = new TChain("T");
    for (int i=0; i<fileSplit; i++){
        //inRootFileName[i] = Form("qsim_out_%s_100k_%d.root",geometry.c_str(), 1000+i);
        inRootFileName[i] = Form("qsim_out_smFull-v3_cosmic_test3.root");
        T->Add(inFileDir + inRootFileName[i]);
    }
    //T->Add("~/programs/qsim/qsim-showermax/rootfiles/qsim_out_cosmic_showermax.root");

    TH1F* h_hitn;
    TH1F* h_hitn_all;
    TH1F* h_hitn_gr0;
    TH1F* h_evTh;
    TH1F* h_evPh;
    //TPaveStats* stat[fileSplit];

    // Defin al possible variable
    Double_t evPx, evPy, evPz; // Momentum of the events
    Double_t evTh, evPh;        // Theta and phi angles of the event generated
    Double_t evX, evY, evZ;     // Position coordinates of the event generated
    Double_t scintX, scintY, scintZ;    // Position coordinate of the bottom sciintillator. To check if the muons hit the scintillator
    Int_t hitn;
    Double_t scintDist = 1800.0;        // Distance (mm) between top and bottom scintillator
    Double_t scintSize = 300.0;              // Size (length and width in mm) of the scintillators

    T->SetBranchAddress("hit.n", &hitn);
    T->SetBranchAddress("ev.px", &evPx);
    T->SetBranchAddress("ev.py", &evPy);
    T->SetBranchAddress("ev.pz", &evPz);
    T->SetBranchAddress("ev.vx", &evX);
    T->SetBranchAddress("ev.vy", &evY);
    T->SetBranchAddress("ev.vz", &evZ);

    int nEvents = T->GetEntries();

    h_hitn = new TH1F(Form("h_hitn"), Form("PE hits distribution of %s with %s beam; PEs; Events/5PEs", geometry.c_str(), particle.c_str()),
            nbins, hist_xmin, hist_xmax);
    h_hitn_all = new TH1F(Form("h_hitn_all"), Form("PE hits_all distribution of %s with %s beam; PEs; Events/5PEs", geometry.c_str(), particle.c_str()),
            nbins, hist_xmin, hist_xmax); 
    h_hitn_gr0 = new TH1F(Form("h_hitn_gr0"), Form("PE hits distribution of %s with %s beam; PEs; Events/5PEs", geometry.c_str(), particle.c_str()),
            nbins, hist_xmin, hist_xmax);
    

    h_evTh = new TH1F("h_evTh", "event Theta angle; Theta [rad]; count", 100, -20, 20);
    h_evPh = new TH1F("h_evPh", "event Phi angle; Phi [rad]; count", 100, 0, 1);

    
    for (int j=0; j<nEvents; j++){
        T->GetEntry(j);

        // Make a cut such that the muon events that hits the upper and lower scintillators are counted as events
        evTh = TMath::ACos(evPz/TMath::Sqrt(evPx*evPx+evPy*evPy+evPz*evPz));
        evPh = TMath::ATan(evPy/evPx);

        double scintR = scintDist * TMath::Tan(evTh); 
        scintX = evX + scintR * TMath::Cos(evPh);
        scintY = evY + scintR * TMath::Sin(evPh);
        //cout << scintX << " , " << scintY << endl;

        h_evTh->Fill(evTh*180/TMath::Pi());
        // h_evPh->Fill(evPh);
        h_hitn_all->Fill(hitn);
        if (scintX>-scintSize/2 && scintX<scintSize/2 && scintY>-scintSize/2 && scintY<scintSize/2 ) {
            h_hitn->Fill(hitn);
        }
        if (hitn>0) h_hitn_gr0->Fill(hitn);    
        if (j%(nEvents/10)==0) cout << "\tfilled " << j << " events." << endl;
    }
    
    // Draw in canvas
    TCanvas* c1 = new TCanvas("c1", "Photon distribution", 800, 600);
    c1->Divide(2,2);
    //gPad->SetLogy();
    c1->cd(1);
    h_hitn->Draw();
    c1->cd(2);
    h_hitn_all->Draw();
    c1->cd(3);
    h_hitn_gr0->Draw();
    c1->cd(4);
    h_evTh->Draw();
    
    //TCanvas* c2 = new TCanvas("c2", "Theta distribution");
    //h_evTh->Draw();
    //h_hitn_all->Draw();

    //TCanvas* c3 = new TCanvas("c3", "Phi distribution");
    // h_evPh->Draw();

/*
    // gPad->Update();
    // float widthStat = 0.25; //width of a stat box
    // float heightStat = 0.20;
    // int nColStat = 1;   //number of columns of the stat boxes
    // for (int i=0; i<1; i++){
    //     stat[i] = (TPaveStats*)h_hitn->FindObject("stats");
    //     stat[i]->SetTextColor(kBlack);
    //     stat[i]->SetX1NDC(0.9-widthStat-(i%nColStat)*widthStat);
    //     stat[i]->SetX2NDC(0.9-(i%nColStat)*widthStat);
    //     stat[i]->SetY1NDC(0.9-(i/nColStat)*heightStat);
    //     stat[i]->SetY2NDC(0.9-heightStat-(i/nColStat)*heightStat);
    //     stat[i]->Draw();
    // }

    // double rms = h_hitn->GetRMS();
    // double mean = h_hitn->GetMean();
    // double res = rms/mean;
    // TString resText = Form("#frac{RMS}{Mean} = %.1f", rms/mean);

    // TLatex latex;
    // latex.SetNDC(1);
    // latex.SetTextSize(0.03);
    // latex.SetTextFont(42);
    // latex.SetTextColor(kBlack);
    // latex.DrawLatex(0.66, 0.65, resText);

    //h_hitn->SaveAs("./test/test.root");

    //gSystem->Exec(Form("mkdir -p plots/%s/",config.c_str()));
    //c1->SaveAs(Form("./plots/%s/%s_%s.pdf",config.c_str(), geometry.c_str(),particle.c_str()));
*/
}
