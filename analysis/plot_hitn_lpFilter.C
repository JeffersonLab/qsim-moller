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

//Function prototype
double getWL(double energy); // Return the wavelength of the photon with the given energy in GeV

void plot_hitn_lpFilter(){
    //gROOT->Reset();
    gStyle->SetOptStat("eMR");
    //gStyle->SetTitleYOffset(1.3);
    //gStyle->SetPadGridX(1);
    //gStyle->SetPadGridY(1);
    TGaxis::SetMaxDigits(3);
    
    string config = "qsim_48";
    string particle = "e-";
    float beamEnergy = 1;
    int lpFilterWL[] = {0, 300, 350, 400, 425, 450}; // in nm
    int nLpFilterWL = sizeof(lpFilterWL)/sizeof(lpFilterWL[0]);
    string geometry = "smRetro-v2-5-2";
    int color[] = {kBlack, kRed, kBlue, kMagenta, kGreen+2, kCyan, kOrange-3, kViolet, kAzure, kTeal};

    int hist_xmin = 0;
    int hist_xmax = 100;
    int nbins = 100;
    int fileSplit = 1;

    //TString inFileDir = Form("/volatile/halla/moller12gev/sudip/qsim_rootfiles/%s/",config.c_str());
    TString inFileDir = Form("~/programs/qsim/qsim-moller/rootfiles/");
    //TString inFileDir = Form("~/programs/qsim/qsim-moller/rootfiles/ifarm_workdir/");
    TString inRootFileName[fileSplit];

    TChain *T = new TChain("T");
    for (int i=1; i<fileSplit+1; i++){
        //inRootFileName[i] = Form("qsim_out_1GeV_mu_%s_10k_%d.root",geometry.c_str(), 1000+i);
        inRootFileName[i] = Form("qsim_out_%s_mami_%d.root",geometry.c_str(), i);
        //inRootFileName[i] = Form("qsim_out_855MeV_%s_hadd.root",geometry.c_str());
        // inRootFileName[i] = Form("qsim_out_%s_855MeV_*.root",geometry.c_str());
        cout << "File added: " << inRootFileName[i] << endl;
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
                if (getWL(hitE[iHit])>lpFilterWL[iFilter]) hitCount++;
            }
           
            h_hitn[iFilter]->Fill(hitCount);

        }
    }
    
    // Draw histograms
    THStack *hs = new THStack("hs", "PE hits distribution for different LP filters");
    for (int i=0; i<nLpFilterWL; i++){
        hs->Add(h_hitn[i]);
    }
    hs->Draw("nostack");
    hs->GetXaxis()->SetTitle("PE per event");
    hs->GetYaxis()->SetTitle("Counts/bin");

    //Draw legend
    TLegend *legend = new TLegend(0.55,0.45,0.9,0.9);
    legend->SetHeader("   \tLP wl:\tMean,\tRes");
    for (int i=0; i<nLpFilterWL; i++){
        legend->AddEntry(h_hitn[i], Form("%d nm: \t%0.1f,  \t%0.2f", lpFilterWL[i], h_hitn[i]->GetMean(), h_hitn[i]->GetRMS()/h_hitn[i]->GetMean()), "l");
    }
    legend->Draw();

    // Save plots
    gSystem->Exec(Form("mkdir -p plots/%s/",config.c_str()));
    c1->SaveAs(Form("./plots/%s/lpResponse_%s_%s.pdf",config.c_str(), geometry.c_str(),particle.c_str()));

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

double getWL(double energy){
    // double h = 6.62607015e-34; // Planck constant in J.s
    // double c = 299792458; // Speed of light in m/s
    // double eV = 1.602176634e-19; // 1 eV in J
    // double lambda = h*c/(energy*eV*1e9);
    //return lambda*1e9; // in nm
    return 1240/(energy*1e9); // in nm
}
