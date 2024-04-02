// This script plots the modifiied hit.n distribution from qsim
// For each events, the hits that has the photons wavelength greater than the specified limit 
// are considered and counted in modified hit.n

#include <string>
#include <iostream>
#include <map>
#include "TROOT.h"
#include "TChain.h"
#include "TStyle.h"
#include "TGaxis.h"
#include "TFile.h"
#include "TH1I.h"
#include "THStack.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TPaveStats.h"
#include "TSystem.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TRandom.h"

using namespace std;

void plot_hitn_lpFilter(){
    //gROOT->Reset();
    gStyle->SetOptStat("eMR");
    //gStyle->SetTitleYOffset(1.3);
    //gStyle->SetPadGridX(1);
    //gStyle->SetPadGridY(1);
    //TGaxis::SetMaxDigits(3);
    
    string config = "localMac";
    string particle = "e-";
    float beamEnergy = 1;
    int lpFilterWL[] = {0, 300, 350, 400, 450}; // in nm
    int nLpFilterWL = sizeof(lpFilterWL)/sizeof(lpFilterWL[0]);
    string geometry = "showerMaxDetector_v3-1-0";
    int color[] = {kBlack, kRed, kBlue, kMagenta, kGreen+2, kCyan, kOrange-3, kViolet, kAzure, kTeal};

    int hist_xmin = 0;
    int hist_xmax = 100;
    int nbins = 100;
    int fileSplit = 2;

    //TString inFileDir = Form("/volatile/halla/moller12gev/sudip/qsim_rootfiles/%s/",config.c_str());
    TString inFileDir = Form("~/programs/qsim/qsim-showermax/rootfiles/");
    TString inRootFileName[fileSplit];

    TChain *T = new TChain("T");
    for (int i=1; i<fileSplit; i++){
        //inRootFileName[i] = Form("qsim_out_1GeV_mu_%s_10k_%d.root",geometry.c_str(), 1000+i);
        inRootFileName[i] = Form("qsim_out_mami_retro-v2_cath70mm_%d.root", i+1);
        T->Add(inFileDir + inRootFileName[i]);
    }

    TH1F* h_hitn[nLpFilterWL];
    TPaveStats* stat[fileSplit];
    TCanvas* c1 = new TCanvas("c1", "PE distribution");

    Int_t hitn;
    Double_t hitE[200];
    T->SetBranchAddress("hit.n", &hitn);
    T->SetBranchAddress("hit.e", hitE);
    int events = T->GetEntries();

    // Create histograms
    for (int i=0; i<nLpFilterWL; i++){
        h_hitn[i] = new TH1F(Form("h_hitn_%d_%d",lpFilterWL[i],lpFilterWL[i+1]), 
                Form("PE hits distribution of %s with %s beam [LP%d nm];PE per event; Counts/bin", geometry.c_str(), particle.c_str(), lpFilterWL[i]),
                nbins, hist_xmin, hist_xmax);
        h_hitn[i]->SetLineColor(color[i]);
        h_hitn[i]->SetLineWidth(2);
    }
    
    // Fill histograms
    for (int iFilter=0; iFilter<nLpFilterWL; iFilter++){
        cout << "LP filter " << lpFilterWL[iFilter] << " nm" << endl;
        for (int iEvent=0; iEvent<events; iEvent++){
            int hitCount = 0; 
            T->GetEntry(iEvent);

            for (int iHit=0; iHit<hitn; iHit++){
                if (hitE[iHit]*1e9*lpFilterWL[iFilter]<1239.8) hitCount++;
            }
           
            h_hitn[iFilter]->Fill(hitCount);

        }
    }
    
    // Draw histograms
    THStack *hs = new THStack("hs","PE hits distribution for different LP filters");
    for (int i=0; i<nLpFilterWL; i++){
        hs->Add(h_hitn[i]);
    }
    hs->Draw("nostack");
    hs->GetXaxis()->SetTitle("PE per event");
    hs->GetYaxis()->SetTitle("Counts/bin");

    //Draw legend
    TLegend *legend = new TLegend(0.6,0.5,0.9,0.9);
    legend->SetHeader("LP wl: Mean, RMS/Mean");
    for (int i=0; i<nLpFilterWL; i++){
        legend->AddEntry(h_hitn[i], Form("%d nm: \t%0.1f, \t%0.2f", lpFilterWL[i], h_hitn[i]->GetMean(), h_hitn[i]->GetRMS()/h_hitn[i]->GetMean()), "l");
    }
    legend->Draw();

/*
    gPad->Update();
    float widthStat = 0.25; //width of a stat box
    float heightStat = 0.20;
    int nColStat = 3;   //number of columns of the stat boxes
    for (int i=0; i<nLpFilterWL; i++){
        stat[i] = (TPaveStats*)h_hitn[i]->FindObject("stats");
        stat[i]->SetTextColor(kBlue);
        stat[i]->SetX1NDC(0.9-widthStat-(i%nColStat)*widthStat);
        stat[i]->SetX2NDC(0.9-(i%nColStat)*widthStat);
        stat[i]->SetY1NDC(0.9-(i/nColStat)*heightStat);
        stat[i]->SetY2NDC(0.9-heightStat-(i/nColStat)*heightStat);
        stat[i]->Draw();
    } 

    double rms = h_hitn[i]->GetRMS();
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
    */
}
