// This script plots the hit.n distribution from qsim

#include <string>
#include <iostream>
#include <map>
#include "TROOT.h"
#include "TStyle.h"
#include "TGaxis.h"
#include "TFile.h"
#include "TH1I.h"
#include "TH2.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TPaveStats.h"
#include "TSystem.h"

using namespace std;

void plot_eVsHitn(){
    //gROOT->Reset();
    gStyle->SetOptStat(0);
    gStyle->SetPalette(55);
    //gStyle->SetTitleYOffset(1.3);
    //gStyle->SetPadGridX(1);
    //gStyle->SetPadGridY(1);
    //TGaxis::SetMaxDigits(3);
    
    string config = "qsim_07";
    string particle = "e";
    float mass_e = 0.511; //in GeV
    string geometry = "showermaxQsim";
    int color[] = {kBlack, kRed, kBlue, kMagenta, kGreen+2};

    int xmin = 0;   //in GeV
    int xmax = 9;
    int ymin = 0;
    int ymax = 1500;
    int nbins = 100; 

    //string inFileDir = "/run/user/1000/gvfs/dav+sd:host=Spice%2520client%2520folder._webdav._tcp.local/qsim_rootfiles/qsim_02/";
    //string inFileDir = "/run/user/1000/gvfs/sftp:host=sudips-mbp.local/Users/sudip/utm-ubuntu-shared/qsim_rootfiles/qsim_03/";
    //string inFileDir = Form("/volatile/halla/moller12gev/sudip/qsim_rootfiles/%s/",config.c_str());
    string inFileDir = "~/programs/qsim/qsim-showermax/rootfiles/";
    //string inRootFileName = Form("qsim_out_8GeV_%s_10k.root", geometry.c_str());
    string inRootFileName = "qsim_out_e6.root";
    string inRootFile;
    TH2F* h_eVsHitn;
    TCanvas* c1 = new TCanvas("c1", "Energy vs PE hit distribution");

    inRootFile = inFileDir + inRootFileName;
    TFile* inFile = new TFile(inRootFile.c_str(), "READ");
    TTree* T = (TTree*)inFile->Get("T");
    Int_t hitn;
    Double_t p;
    Double_t energy;
    T->SetBranchAddress("hit.n", &hitn);
    T->SetBranchAddress("ev.p", &p);
    int events = T->GetEntries();

    h_eVsHitn = new TH2F(Form("h_eVsHitn"), Form("Energy vs hits distribution of %s with %s beam;Energy of events [in GeV]; Number of Hits; HitCounts/bin ", geometry.c_str(), particle.c_str()),
            nbins, xmin, xmax, nbins, ymin, ymax);
    
    for (int j=0; j<events; j++){
        T->GetEntry(j);
        energy = sqrt(p*p-mass_e*mass_e);
        h_eVsHitn->Fill(energy,hitn);
        if (j%1000==0) cout << "Processed " << j << " events." << endl;
    }

    h_eVsHitn->Draw("COLZ");

    //System->Exec(Form("mkdir -p plots/%s/",config.c_str()));
    c1->SaveAs(Form("./plots/eVsHitn_%s_%s.pdf", geometry.c_str(),particle.c_str()));
}
