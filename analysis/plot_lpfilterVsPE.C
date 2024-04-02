// This script plots the graph of wavelength of the long pass filter vs mean PE yeild
// due to application of that filter

#include <string>
#include <iostream>
#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TCut.h"
#include "TLegend.h"
#include "TGraph.h"
#include "TMultiGraph.h"

using namespace std;

void plot_lpfilterVsPE(){
    //gROOT->Reset();
    gStyle->SetOptStat(0);
    //gStyle->SetTitleYOffset(1.3);
    //gStyle->SetPadGridX(1);
    //gStyle->SetPadGridY(1);
    //TGaxis::SetMaxDigits(3);
    
    TString config = "qsim_20";
    const Int_t nGen = 3;
    const Int_t nSector = 3;
    const Int_t nParticle = 2;
    const Int_t nFilter = 15;
    TString generator[nGen] = {"moller", "elastic", "inelastic"};
    TString sector[nSector] = {"open", "closed", "trans"};
    TString particle[nParticle] = {"electron", "gamma"};
    Int_t nEvents = 10000;
    Double_t lpValue[nFilter] = {0, 50, 100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 600, 650, 700};     //long pass filter wavelengths
    Int_t color[] = {kBlack, kRed, kGreen+2, kBlue, kMagenta};

    //TString inFileDir = Form("/volatile/halla/moller12gev/sudip/qsim_rootfiles/%s/",config.c_str());
    TString inFileDir = Form("/Users/sudip/test/qsim_rootfiles/%s/", config.Data());

    TCut photonOnly = "hit.pid==0";
    TCut lpCut[nFilter];
    for (int iFilter=0; iFilter<nFilter; iFilter++) {
        lpCut[iFilter] = Form("(1240/(hit.e*1e9))>%.0f", lpValue[iFilter]);
    }
    
    TCanvas* canvas[nGen][nParticle];
    TMultiGraph *mg[nGen][nParticle];
    //TGraph* gr[nGen][nParticle][nSector];

    for (int iGen = 0; iGen < nGen; iGen++){
        for (int iParticle = 0; iParticle < nParticle; iParticle++){
            canvas[iGen][iParticle] = new TCanvas(Form("c_%s_%s",generator[iGen].Data(), particle[iParticle].Data()), 
                    Form("LP_response_%s_%s", generator[iGen].Data(), particle[iParticle].Data()), 600, 500);
            mg[iGen][iParticle] = new TMultiGraph("mg", "multigraph");
            
            TGraph* gr[nSector];

            for (int iSector = 0; iSector < nSector; iSector++) {
                TString inRootFile = Form("%sqsim_out_%s_%s_%s.root",inFileDir.Data(), generator[iGen].Data(), sector[iSector].Data(), particle[iParticle].Data());
                
                TFile* inFile = new TFile(inRootFile.Data(), "READ");
                TTree* T = (TTree*)inFile->Get("T");
                Double_t PE[nFilter];

                for (int iFilter = 0; iFilter < nFilter; iFilter++) {
                    T->Draw(Form("hit.n>>hitn"), photonOnly&&lpCut[iFilter], "goff");
                    auto h_hitn = (TH1F*)gDirectory->FindObject("hitn");
                    Double_t meanPE = h_hitn->GetEntries()/nEvents; 
                    PE[iFilter] = meanPE;

                    cout << meanPE << endl;
                }

                //Implement graph
                gr[iSector] = new TGraph(nFilter, lpValue, PE);
                gr[iSector]->SetTitle(sector[iSector].Data());
                gr[iSector]->SetLineColor(color[iSector]);
                gr[iSector]->SetLineWidth(2);
                gr[iSector]->SetMarkerStyle(iSector+2);
                gr[iSector]->SetLineColor(color[iSector]);
            }

            //Add graphs to multigraph
            for (int iSector = 0; iSector < nSector; iSector++) {
                mg[iGen][iParticle]->Add(gr[iSector]);
                mg[iGen][iParticle]->SetTitle(sector[iSector].Data());
            }
            
            //Draw multigraphs
            mg[iGen][iParticle]->SetTitle(Form("Longpass filter response for %s events , %s; #lambda [nm]; Mean PE",generator[iGen].Data(), particle[iParticle].Data()));
            mg[iGen][iParticle]->Draw("apl");

            //Draw Legend    
            canvas[iGen][iParticle]->BuildLegend(0.7,0.65, 0.9, 0.9, "Sectors");
            
            gSystem->Exec(Form("mkdir -p ./plots/%s/", config.Data()));
            canvas[iGen][iParticle]->SaveAs(Form("./plots/%s/lpResponse_%s_%s.pdf", config.Data(), generator[iGen].Data(), particle[iParticle].Data()));
        }
    }
}
