// This script reads the hit.n from several qsim rootfiles and writes them in the output txt file

#include <string>
#include <iostream>
#include <fstream>
#include "TROOT.h"
#include "TChain.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"
#include "TPaveStats.h"
#include "TSystem.h"
#include "TLatex.h"
#include "TH1D.h"

using namespace std;

void read_hitn(){
    //gROOT->Reset();
    gStyle->SetOptStat("eMR");
    //gStyle->SetTitleYOffset(1.3);
    //gStyle->SetPadGridX(1);
    //gStyle->SetPadGridY(1);
    gStyle->SetStatFormat("0.2g");
    gStyle->SetOptFit(1);
    
    string config = "qsim_30";
    string sector[] = {"open", "closed", "trans"};
    int nSector = 3;
    int nFile = 8;
    string geometry = "smRetroQsim";

    //string inFileDir = Form("/volatile/halla/moller12gev/sudip/qsim_rootfiles/%s/",config.c_str());
    TString inFileDir = Form("/run/user/1000/gvfs/sftp:host=sudips-mbp.local/Users/sudip/test/qsim_rootfiles/%s/", config.c_str());
    //TString inRootFileName;

    ofstream outfile;
    outfile.open("./output_files/hitn_3sector.csv");
    outfile << "open,closed,trans" << endl;

    for (int iFile=1001; iFile<1001+nFile; iFile++){
        for (int iSector=0; iSector<nSector; iSector++){
            TString inRootFileName = Form("qsim_out_%s_%i.root", sector[iSector].c_str(), iFile);
            TFile * inFile = new TFile(inFileDir + inRootFileName, "read");
            TTree * T = (TTree *)inFile->Get("T");
            Int_t hitn;
            T->SetBranchAddress("hit.n", &hitn);

            T->Draw("hit.n>>hitn", "", "goff");
            TH1D * h_hitn = (TH1D*)gDirectory->FindObject("hitn");
            double mean = h_hitn->GetMean();
            //cout << mean << endl;
            if (iSector!=2) outfile << mean << ",";
            else outfile << mean;

            delete inFile;
            //delete T;
            //delete h_hitn;


        }
        outfile << endl;
        if (iFile%2==0) {cout << "Completed " << iFile << " files." << endl;}
    }
    outfile.close();

}
