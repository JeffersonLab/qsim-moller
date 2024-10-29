/*This root script reads the output of the simulation and plots the cathode current estimate 
*/

#include <iostream>
#include <fstream>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TString.h>
#include <TPaveStats.h>
#include <TSystem.h>

using namespace std;

void plot_cathCurrent_phsyGen() {
    // Set the style
    gStyle->SetOptStat("neMR");

    // Define variables
    TString config = "qsim_60";
    TString generator[] = {"moller", "elastic", "inelastic"};
    TString sector[] = {"open", "closed", "trans"};
    TString beamParticle[] = {"electron", "gamma"};
    TString rootFileDir = "./rootfiles/" + config + "/";
    
    // Write output to a csv file
    TString csvFileName = Form("./output/files/%s/rate_peRate.csv", config.Data());
    gSystem->Exec(Form("mkdir -p ./output/files/%s", config.Data()));
    ofstream csvFile(csvFileName);
    if (!csvFile.is_open()) {
        std::cerr << "Error opening file: " << csvFileName << std::endl;
        return;
    }

    // Write the header
    csvFile << "generator,sector,beamParticle,rate,pe,ratePE" << std::endl;

    // Loop over all three generator, sector and paticle type
    for_each(generator, generator + 3, [&](TString gen) {
        for_each(sector, sector + 3, [&](TString sec) {
            for_each(beamParticle, beamParticle + 2, [&](TString part) {
                // Plot the histogram
                TString rootFileName = Form("qsim_out_%s_%s_%s.root", gen.Data(), sec.Data(), part.Data());
                TString rootFile = rootFileDir + rootFileName;

                // Open the ROOT file
                TFile *file = TFile::Open(rootFile);
                if (!file || file->IsZombie()) {
                    std::cerr << "Error opening file: " << rootFile << std::endl;
                    return;
                }

                //Read tree
                TTree *T = (TTree*)file->Get("T");
                cout << "Entries in tree: " << T->GetEntries() << endl;

                // Retrieve the histogram
                T->Draw("hit.n*ev.rate>>hist_peRate", "", "goff");
                T->Draw("hit.n>>hist_pe", "", "goff");
                T->Draw("hit.n>>hist_rate", "ev.rate", "goff");
                TH1F *hist_peRate = (TH1F*)gDirectory->Get("hist_peRate");
                TH1F *hist_pe = (TH1F*)gDirectory->Get("hist_pe");
                TH1F *hist_rate = (TH1F*)gDirectory->Get("hist_rate");
                if (!hist_peRate || !hist_pe || !hist_rate) {
                    std::cerr << "Error retrieving histogram: " << hist_peRate->GetName() << std::endl;
                    file->Close();
                    return;
                }

                Double_t rate = hist_rate->Integral();
                Double_t pe = hist_pe->GetMean();
                Double_t ratePE = hist_peRate->GetMean()*hist_peRate->GetEntries();

                // Write the output to the csv file
                csvFile << gen << "," << sec << "," << part << "," << rate << "," << pe << "," << ratePE << std::endl;

                // Clean up
                file->Close();
                
            });
        });
    });


    // //Draw histogram
    // TCanvas *c = new TCanvas("c", "c", 800, 600);
    // c->SetGrid();
    // hist->SetTitle("Pulse height distribution for MollerOpenElectron");
    // hist->Draw("hist");

    

    // Clean up
    //file->Close();
}
