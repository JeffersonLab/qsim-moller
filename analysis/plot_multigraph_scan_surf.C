/*
Reads the txt files from the scan analysis and plot them in a multigraph
*/

#include <string>
#include <iostream>
#include <fstream>

#include "TMultiGraph.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TLegend.h"

using namespace std;

void plot_multigraph_scan_surf(){
    int color[] = {kBlack, kRed, kBlue, kGreen+2, kMagenta, kOrange, kCyan, kViolet, kAzure, kTeal, kSpring};

    TMultiGraph* mg = new TMultiGraph("mg", "Comparison of scan");

    TCanvas* c1 = new TCanvas("c1", "c1", 800, 600);

    TLegend* leg = new TLegend(0.35,0.65,0.6,0.9);
    leg->SetHeader("Reflective surface");
    leg->SetTextFont(42);
    leg->SetTextSize(0.03);
    const int nFile = 3;
    
    string file[nFile] = {"./output/qsim_34/meanHitVsAzPos_smRetroQsim-v2.1_e.txt", 
                    "./output/qsim_35/meanHitVsAzPos_smRetroQsim-v2.7_e.txt",
                    "./output/qsim_36/meanHitVsAzPos_smRetroQsim-v2.8_e.txt"};

    // Legend entries
    string legEnt[nFile] = {"Miro Silver", "Ground Al Surface", "Black Surface"};

    for (int i = 0; i < nFile; i++) {
        TGraph* gr = new TGraph(file[i].c_str());
        gr->SetTitle(legEnt[i].c_str());
        //gr->SetTitle(Form("%f", upperLGAngle[i]));
        gr->SetLineColor(color[i]);
        gr->SetMarkerStyle(20);
        gr->SetMarkerSize(0.5);
        gr->SetMarkerColor(color[i]);
        leg->AddEntry(gr, legEnt[i].c_str(), "lp");

        mg->Add(gr);
    }

   
    mg->Draw("APL");
    mg->SetTitle("Comparison of scan for different LG top surface");
    mg->GetXaxis()->SetTitle("Azimuthal position [in mm]");
    mg->GetYaxis()->SetTitle("Mean PE yield");

    gPad->Modified();
    // mg->GetYaxis()->SetLimits(0, 50);
    mg->SetMinimum(30.);
    mg->SetMaximum(44.);
    leg->Draw();
    
    c1->SaveAs("./plots/qsim_36/meanHitVsAzPos_smRetroQsim-v2.pdf");
}
