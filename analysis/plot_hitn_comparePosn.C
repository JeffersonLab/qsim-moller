// This script plots the hit.n distribution from qsim

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
#include "TLatex.h"

using namespace std;

void plot_hitn_comparePosn(){
    //gROOT->Reset();
    gStyle->SetOptStat("neMR");
    //gStyle->SetTitleYOffset(1.3);
    //gStyle->SetPadGridX(1);
    //gStyle->SetPadGridY(1);
    //TGaxis::SetMaxDigits(3);
    
    string config = "localMac";
    string particle = "cosmic-muon";
    string beamPosn[] = {"center", "near", "far"}; // from the PMT
    string geometry = "smBenchmark";
    int color[] = {kBlack, kRed, kBlue, kMagenta, kGreen+2};

    // Map geometry and upper bound of the histogram
    map<string, int> geomXmax;
    geomXmax.insert(pair<string,int>("smBenchmark1quartzQsim", 200));
    geomXmax.insert(pair<string,int>("smBenchmark1stackQsim", 4000));
    geomXmax.insert(pair<string,int>("smBenchmark2stackQsim", 7000));
    geomXmax.insert(pair<string,int>("smBenchmark3stackQsim", 9000));
    geomXmax.insert(pair<string,int>("smBenchmark4stackQsim", 10000));
    geomXmax.insert(pair<string,int>("smBenchmark", 500));
    geomXmax.insert(pair<string,int>("showermaxQsim", 2000));
    geomXmax.insert(pair<string,int>("smretro", 500));

    int hist_xmin = 100;
    int hist_xmax = geomXmax[geometry];
    int nbins = 100; 
    const int nFiles = 3;

    //string inFileDir = "/run/user/1000/gvfs/dav+sd:host=Spice%2520client%2520folder._webdav._tcp.local/qsim_rootfiles/qsim_02/";
    //string inFileDir = "/run/user/1000/gvfs/sftp:host=sudips-mbp.local/Users/sudip/utm-ubuntu-shared/qsim_rootfiles/qsim_03/";
    //string inFileDir = Form("/volatile/halla/moller12gev/sudip/qsim_rootfiles/%s/",config.c_str());
    string inFileDir = "~/programs/qsim/qsim-showermax/rootfiles/";
    string inRootFileName[nFiles] = {Form("qsim_out_cosmic_%s_1.root", geometry.c_str()),
                                    Form("qsim_out_cosmic_%s_2.root", geometry.c_str()),
                                    Form("qsim_out_cosmic_%s_3.root", geometry.c_str())};
    string inRootFile[nFiles];
    TH1F* h_hitn[nFiles];
    TPaveStats* stat[nFiles];
    TCanvas* c1 = new TCanvas("c1", "Photon distribution");

    for (int i=0; i<nFiles; i++){
        inRootFile[i] = inFileDir + inRootFileName[i];
        TFile* inFile = new TFile(inRootFile[i].c_str(), "READ");
        TTree* T = (TTree*)inFile->Get("T");
        Int_t hitn;
        T->SetBranchAddress("hit.n", &hitn);
        int events = T->GetEntries();

        h_hitn[i] = new TH1F(Form("h_hitn_%s",beamPosn[i].c_str()), Form("PE hits distribution of %s with %s beam;PE per event; Counts/bin", geometry.c_str(), particle.c_str()),
                nbins, hist_xmin, hist_xmax);
        h_hitn[i]->SetLineColor(color[i]);
        //h_hitn[i]->SetLineWidth(1);
        
        cout << "Processing hist" << i << endl; 
        for (int j=0; j<events; j++){
            T->GetEntry(j);
            h_hitn[i]->Fill(hitn);
            if (j%1000==0) cout << "\tfilled " << j << " events." << endl;
        }
    }
    
    for (int i=0; i<nFiles; i++){
        //gPad->SetLogy();
        if (i==0) h_hitn[i]->Draw();
        else h_hitn[i]->Draw("sames"); //notice the option "sames" not "same"
    }


    gPad->Update();
    float widthStat = 0.25; //width of a stat box
    float heightStat = 0.20;
    int nColStat = 1;   //number of columns of the stat boxes
    for (int i=0; i<nFiles; i++){
        stat[i] = (TPaveStats*)h_hitn[i]->FindObject("stats");
        stat[i]->SetTextColor(color[i]);
        stat[i]->SetX1NDC(0.9-widthStat-(i%nColStat)*widthStat);
        stat[i]->SetX2NDC(0.9-(i%nColStat)*widthStat);
        stat[i]->SetY1NDC(0.9-(i/nColStat)*heightStat);
        stat[i]->SetY2NDC(0.9-heightStat-(i/nColStat)*heightStat);
    
        // Retrieve the stats box
        TPaveStats *ps = (TPaveStats*)c1->GetPrimitive("stats");
        ps->SetName("myStats");
        TList *listOfLines = ps->GetListOfLines();
        
        // Add a new line in the stat box
        TLatex *t = new TLatex(0,0,Form("RMS/mean = %.2f",h_hitn[i]->GetRMS()/h_hitn[i]->GetMean()));
        t->SetTextFont(62);
        t->SetTextSize(0.03);
        t->SetTextColor(color[i]);
        listOfLines->Add(t);     

        h_hitn[i]->SetStats(0);
        c1->Modified();

        //stat[i]->Draw();
    }
    gSystem->Exec(Form("mkdir -p plots/%s/",config.c_str()));
    c1->SaveAs(Form("./plots/%s/%s_%s.pdf",config.c_str(), geometry.c_str(),particle.c_str()));
}
