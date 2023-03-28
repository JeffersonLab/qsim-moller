// This script plots the hit.n distribution from qsim for different mu beam energy

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

using namespace std;

void plot_hitn_compareBeam_mu(){
    //gROOT->Reset();
    gStyle->SetOptStat("neMR");
    //gStyle->SetTitleYOffset(1.3);
    //gStyle->SetPadGridX(1);
    //gStyle->SetPadGridY(1);
    //TGaxis::SetMaxDigits(3);
    
    string config = "qsim_05";
    string particle = "mu";
    string beamEnergy[] = {"200MeV", "300MeV", "400MeV", "500MeV", "600MeV", "700MeV", "800MeV", "900MeV", "1GeV", "50GeV", "100GeV"}; 
    int nBeamEnergy = sizeof(beamEnergy)/sizeof(string);
    string geometry = "showermaxQsim";
    int color[] = {kBlack, kRed, kBlue, kMagenta, kGreen+2, kCyan, kOrange+7, kViolet-5};
    
    // Map geometry and upper bound of the histogram
    map<string, int> geomXmax;
    geomXmax.insert(pair<string,int>("smBenchmark1quartzQsim", 200));
    geomXmax.insert(pair<string,int>("smBenchmark1stackQsim", 4000));
    geomXmax.insert(pair<string,int>("smBenchmark2stackQsim", 7000));
    geomXmax.insert(pair<string,int>("smBenchmark3stackQsim", 9000));
    geomXmax.insert(pair<string,int>("smBenchmark4stackQsim", 10000));
    geomXmax.insert(pair<string,int>("showermaxQsim", 50));

    int hist_xmin = 0;
    int hist_xmax = geomXmax[geometry];
    int nbins = 50; 

    //string inFileDir = "/run/user/1000/gvfs/dav+sd:host=Spice%2520client%2520folder._webdav._tcp.local/qsim_rootfiles/qsim_02/";
    //string inFileDir = "/run/user/1000/gvfs/sftp:host=sudips-mbp.local/Users/sudip/utm-ubuntu-shared/qsim_rootfiles/qsim_03/";
    string inFileDir = Form("/volatile/halla/moller12gev/sudip/qsim_rootfiles/%s/",config.c_str());
    //string inFileDir = "~/programs/qsim/qsim-showermax/rootfiles/";
    //string inRootFileName[3] = {Form("qsim_out_2GeV_%s_10k.root", geometry.c_str()),
    //                          Form("qsim_out_5GeV_%s_10k.root", geometry.c_str()),
    //                         Form("qsim_out_8GeV_%s_10k.root", geometry.c_str())};
    string inRootFileName[nBeamEnergy];
    for (int iEnergy=0; iEnergy<nBeamEnergy; iEnergy++){
        inRootFileName[iEnergy] = Form("qsim_out_%s_%s_10k.root", beamEnergy[iEnergy].c_str(), geometry.c_str());            
    }

    string inRootFile[nBeamEnergy];
    TH1F* h_hitn[nBeamEnergy];
    TPaveStats* stat[nBeamEnergy];
    TCanvas* c1 = new TCanvas("c1", "Photon distribution");

    for (int i=0; i<nBeamEnergy; i++){
        inRootFile[i] = inFileDir + inRootFileName[i];
        TFile* inFile = new TFile(inRootFile[i].c_str(), "READ");
        TTree* T = (TTree*)inFile->Get("T");
        Int_t hitn;
        T->SetBranchAddress("hit.n", &hitn);
        int events = T->GetEntries();

        h_hitn[i] = new TH1F(Form("h_hitn_%s",beamEnergy[i].c_str()), Form("PE hits distribution of %s with %s beam;PE per event; Counts/bin", geometry.c_str(), particle.c_str()),
                nbins, hist_xmin, hist_xmax);
        h_hitn[i]->SetLineColor(color[i]);
        //h_hitn[i]->SetLineWidth(1);
        
        cout << "Processing hist" << i << endl; 
        for (int j=0; j<events; j++){
            T->GetEntry(j);
            h_hitn[i]->Fill(hitn);
            if (j%1000==0) cout << "\tDone " << j << " events." << endl;
        }
    }
    for (int i=0; i<nBeamEnergy; i++){
        //gPad->SetLogx();
        if (i==0) h_hitn[i]->Draw();
        else h_hitn[i]->Draw("sames");
    }

    gPad->Update();
    float widthStat = 0.15; //width of a stat box
    float heightStat = 0.15;
    int nColStat = 3;   //number of columns of the stat boxes
    for (int i=0; i<nBeamEnergy; i++){
            stat[i] = (TPaveStats*)h_hitn[i]->FindObject("stats");
            stat[i]->SetTextColor(i+1);
            stat[i]->SetX1NDC(0.9-widthStat-(i%nColStat)*widthStat);
            stat[i]->SetX2NDC(0.9-(i%nColStat)*widthStat);
            stat[i]->SetY1NDC(0.9-(i/nColStat)*heightStat);
            stat[i]->SetY2NDC(0.9-heightStat-(i/nColStat)*heightStat);
            stat[i]->Draw();
    }
    gSystem->Exec(Form("mkdir -p plots/%s/",config.c_str()));
    c1->SaveAs(Form("./plots/%s/%s_%s.pdf",config.c_str(), geometry.c_str(),particle.c_str()));
}
