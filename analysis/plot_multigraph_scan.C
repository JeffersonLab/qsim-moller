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

void plot_multigraph_scan(){
    int color[] = {kBlack, kRed, kBlue, kGreen+2, kMagenta, kOrange, kCyan, kViolet, kAzure, kTeal, kSpring};

    TMultiGraph* mg = new TMultiGraph("mg", "Comparison of scan");

    TCanvas* c1 = new TCanvas("c1", "c1", 800, 600);

    TLegend* leg = new TLegend(0.7,0.1,0.9,0.4);
    leg->SetHeader("Upper LG angle [deg]");
    leg->SetTextFont(42);
    leg->SetTextSize(0.03);
    

    double upperLGAngle[6] = {0, 0, 0, 0, 0, 0};
    double upperLGHeight = 7.207; // (inches) height of upper LG from the mid bend
    double upperLGBoxWidth = 10.43; // (inches) width of upper LG box design
    double upperLGSlantWidth = 2.71; // (inches) width of upper LG slant design
    double upperLGDesignWidthMargin = (upperLGBoxWidth - upperLGSlantWidth)/2;

    for (int i = 0; i < 6; i++) {
        double upperLGPerp = i*upperLGDesignWidthMargin/5;
        double UpperLGAngle = TMath::ATan(upperLGPerp/upperLGHeight)*180/3.14159265359;
        upperLGAngle[i] = UpperLGAngle;
    }

    for (int i = 0; i < 6; i++) {
        string file = "./output/qsim_32/meanHitVsAzPos_smRetroQsim-v2." + to_string(i+1) + "_e.txt";
        TGraph* gr = new TGraph(file.c_str());
        gr->SetTitle(Form("%f", upperLGAngle[i]));
        gr->SetLineColor(color[i]);
        gr->SetMarkerStyle(20);
        gr->SetMarkerSize(0.5);
        gr->SetMarkerColor(color[i]);
        leg->AddEntry(gr, Form("%.1f", upperLGAngle[i]), "lp");

        mg->Add(gr);
    }

    mg->Draw("APL");
    mg->SetTitle("Comparison of scan for different upper LG angles");
    mg->GetXaxis()->SetTitle("Azimuthal position [in mm]");
    mg->GetYaxis()->SetTitle("Mean PE yield");

    leg->Draw();

    c1->SaveAs("./plots/qsim_34/meanHitVsAzPos_smRetroQsim-v2.pdf");

    
}