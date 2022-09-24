// This script plots the hit.n distribution from qsim. 
// Several long pass filter are applied using TCuts and plotted in the same canvas

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

void plot_photonEnergyDistn(){
    //gROOT->Reset();
    gStyle->SetOptStat(0);
    //gStyle->SetTitleYOffset(1.3);
    //gStyle->SetPadGridX(1);
    //gStyle->SetPadGridY(1);
    //TGaxis::SetMaxDigits(3);
    
    string config = "qsim_07";
    string particle = "e";
    int lpValue[3] = {0, 350, 400};     //long pass filter wavelength
    string geometry = "smRetroQsim";
    string energy = "855MeV";
    int color[] = {kBlack, kRed, kGreen+2, kBlue, kMagenta};

    int xmin = 150;
    int xmax =700;
    int nbins = 500; 

    //string inFileDir = Form("/volatile/halla/moller12gev/sudip/qsim_rootfiles/%s/",config.c_str());
    string inFileDir = "~/programs/qsim/qsim-showermax/rootfiles/";
    string inRootFileName = Form("qsim_out_%s_%s_%s_10k.root", energy.c_str(), particle.c_str(), geometry.c_str());
    //string inRootFileName = "qsim_out_855MeV_e_smRetroQsim_10k.root";
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
    //std::vector<Double_t> *hit_e;
    //Double_t hit_wl;
    //T->SetBranchAddress("hit.e", &hit_e);
    //int events = T->GetEntries();

    for (int i=0; i<3; i++){
        h_hite[i] = new TH1F(Form("h_lp%.0fnm", wlCut[i]), Form("Photon spectrum;Photon wavelength [in nm]; Counts/bin"),
                nbins, xmin, xmax);
        h_hite[i]->SetLineColor(color[i+1]);
        h_hite[i]->SetFillColor(color[i+1]);
        h_hite[i]->SetTitle("Photon spectrum with diffferent long pass fiilters");

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

        //cout << "Processing hist" << i << endl; 
        //for (int iEvent=0; iEvent<events; iEvent++){
            //T->GetEntry(iEvent);
            //cout << hit_e->size() << endl;
            //for (int iHit=0; iHit<hit_e->size(); iHit++){
                //hit_wl = 1240/(hit_e->at(iHit)*1e9);
                //h_hitn[i]->Fill(hit_wl);
                //cout << hit_e->at(iHit) << endl;
            //}
            //if (iEvent%1000==0) cout << "\tfilled " << iEvent << " events." << endl;
        //}
    /*
    gPad->Update();
    float widthStat = 0.15; //width of a stat box
    float heightStat = 0.15;
    int nColStat = 2;   //number of columns of the stat boxes
    for (int i=0; i<3; i++){
        stat[i] = (TPaveStats*)h_hite[i]->FindObject("stats");
        stat[i]->SetTextColor(color[i]);
        stat[i]->SetX1NDC(0.9-widthStat-(i%nColStat)*widthStat);
        stat[i]->SetX2NDC(0.9-(i%nColStat)*widthStat);
        stat[i]->SetY1NDC(0.9-(i/nColStat)*heightStat);
        stat[i]->SetY2NDC(0.9-heightStat-(i/nColStat)*heightStat);
        stat[i]->Draw();
    }
    */
    
    //gSystem->Exec(Form("mkdir -p plots/%s/",config.c_str()));
    //c1->SaveAs(Form("./plots/%s/%s_%s.pdf",config.c_str(), geometry.c_str(),particle.c_str()));
}
