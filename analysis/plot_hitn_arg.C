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
#include "TF1.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TPaveStats.h"
#include "TSystem.h"
#include "TLatex.h"

using namespace std;
// Function prototype
pair<string, string> extractGeometryAndSource(const string& rootFileName);

void plot_hitn_arg(const string& rootFileName){
    //gROOT->Reset();
    gStyle->SetOptStat("eMR");
    //gStyle->SetTitleYOffset(1.3);
    //gStyle->SetPadGridX(1);
    //gStyle->SetPadGridY(1);
    //gStyle->SetStatFormat("0.2g");
    // gStyle->SetOptFit(1);
    TGaxis::SetMaxDigits(2);

    // Exit if file not found
    if (gSystem->AccessPathName(rootFileName.c_str())) {
        cout << "File " << rootFileName << " not found." << endl;
        return;
    }
    
    string config = "local_mac";
    string particle = "e";
    float beamEnergy = 855;
    int color[] = {kBlack, kRed, kBlue, kMagenta, kGreen+2};

    int hist_xmin = 0;
    int hist_xmax = 100;
    int nbins = 100;
    int xMaxRange = 100;

    // Read geometry and source from rootFileName
    pair<string, string> geometryAndSource = extractGeometryAndSource(rootFileName);
    string geometry = geometryAndSource.first;
    string source = geometryAndSource.second;
    cout << "Geometry: " << geometry << ", Source: " << source << endl;

    // Read rootFileName
    TFile *file = new TFile(rootFileName.c_str());
    TTree *T = (TTree*)file->Get("T");

    TH1F* h_hitn;
    TPaveStats* stat[1];
    TCanvas* c1 = new TCanvas("c1", "Photon distribution");

    Int_t hitn;
    T->SetBranchAddress("hit.n", &hitn);
    Int_t events = T->GetEntries();

    h_hitn = new TH1F(Form("h_hitn_%.0fMeV",beamEnergy), Form("PE hits distribution of %s with %s beam; PEs; Events/PE", geometry.c_str(), source.c_str()),
            nbins, hist_xmin, hist_xmax);
    h_hitn->SetLineColor(kBlack);
    h_hitn->Rebin(2);
    //h_hitn->GetXaxis()->SetRangeUser(-1, xMaxRange);
    //h_hitn[i]->SetLineWidth(1);
    
    for (int j=0; j<events; j++){
        T->GetEntry(j);
        h_hitn->Fill(hitn);
        if (hitn > xMaxRange) cout << "hitn = " << hitn << endl;
    
    }
    
    //gPad->SetLogy();
    h_hitn->Draw();

    // Fit the histogram with a Gaussian 
    //TF1 *fit = new TF1("fit", "gaus", h_hitn->GetMean()-1.5*h_hitn->GetRMS(), h_hitn->GetMean()+1*h_hitn->GetRMS());
    //h_hitn->Fit(fit, "R");
    //fit->SetLineColor(kRed);
    //fit->Draw("same");

    gPad->Update();
    float widthStat = 0.30; //width of a stat box
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
    gPad->Update();

    double resHist = h_hitn->GetRMS()/h_hitn->GetMean();
    //double resFit = fit->GetParameter(2)/fit->GetParameter(1);
    TString resHistText = Form("#frac{RMS}{Mean} = %.2f", resHist);
    //TString resFitText = Form("Fit:  #frac{RMS}{Mean} = %.2f", resFit);

    TLatex latex;
    latex.SetNDC(1);
    latex.SetTextSize(0.04);
    //latex.SetTextFont(42);
    latex.SetTextColor(kBlack);
    latex.DrawLatex(0.65, 0.6, resHistText);
    //latex.DrawLatex(0.66, 0.35, resFitText);

    //h_hitn->SaveAs("./test/test.root");

    gSystem->Exec(Form("mkdir -p output/plots/%s/",config.c_str()));
    c1->SaveAs(Form("./output/plots/%s/%s_%s.pdf",config.c_str(), geometry.c_str(),particle.c_str()));
}

// Function to get geometry from rootFileName
pair<string, string> extractGeometryAndSource(const string& rootFileName) {
    pair<string, string> result;
    size_t startPos = rootFileName.find("qsim_out_");
    if (startPos != string::npos) {
        startPos += 9; // Move startPos to end of "qsim_out_"
        size_t endPosGeometry = rootFileName.find("_", startPos);
        if (endPosGeometry != string::npos) {
            result.first = rootFileName.substr(startPos, endPosGeometry - startPos);
            size_t startPosSource = endPosGeometry + 1;
            size_t endPosSource = rootFileName.find("_", startPosSource);
            if (endPosSource != string::npos) {
                result.second = rootFileName.substr(startPosSource, endPosSource - startPosSource);
            }
        }
    }
    return result;
}
