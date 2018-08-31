#include <iostream>
#include <vector>
#include "histProduce/histProduce/interface/rooTHMainLbTk.h"

#include "TTree.h"
#include "TFile.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TGraph.h"
#include "TGaxis.h"

#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooGaussian.h"
#include "RooPolynomial.h"
#include "RooPlot.h"
#include "RooAddPdf.h"
#include "RooArgList.h"
#include "RooGlobalFunc.h"
#include "RooChebychev.h"
#include "RooExtendPdf.h"
#include "RooWorkspace.h"
#include "RooRandom.h"

typedef root_TreeHistoMain_LbTk readData;

int main()
{
    std::vector<double> gaRes;
    //gaRes = { 0.63,-1.10,19.24,0.00,3.06,1.06,3.64,0.71,2.61,0.92,0.00,0.06,0.13,-0.47,-0.80,3.42,-2.84,1.94 };
    //gaRes = { -0.88,-1.10,16.91,0.00,4.68,-1.04,4.68,-0.06,1.85,-0.33,0.12,-1.38,-0.56,-0.26,-0.64,3.42,-2.71,5.69 };
    //gaRes = { 1.23,0.16,15.18,0.00,4.39,0.74,3.92,-0.12,1.98,-0.02,0.00,0.00,0.00,-1.09,-0.76,2.23,-2.41,3.01 };
    //gaRes = { -1.94,0.16,13.21,0.00,4.85,0.25,4.14,-1.04,2.32,-0.42,-0.26,0.66,0.00,1.51,-1.03,3.47,-2.89,3.81 };
    //gaRes = { 1.58,0.31,13.44,0.00,4.12,0.00,4.56,0.00,2.29,0.00,0.00,0.00,0.00,0.00,-0.53,0.75,-0.32,0.45 };
    //gaRes = {-0.08,-1.10,13.23,0.00,4.72,-0.63,5.59,0.00,2.34,0.37,0.00,0.45,0.37,0.49,-1.41,2.49,-1.11,2.30 };
    //gaRes = { -0.53,-0.32,13.31,0.16,4.23,-1.22,5.24,0.00,3.58,-0.51,-0.36,-0.59,0.22,-0.25,-2.41,3.48,-2.65,3.03 };
    //gaRes = { -0.24,-0.54,13.31,-0.98,1.96,2.00,5.42,0.30,3.48,0.00,-0.73,0.75,0.54,-0.25,-6.00,6.06,-5.47,5.30 }; 
    //gaRes = {1.21,0.37,18.81,0.00,3.47,0.00,3.66,0.00,2.89,0.18,0.00,0.00,0.00,0.00,-0.47,0.00,-0.65,0.00 };
    //gaRes = { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0, 0., 0., 0., 0., 0. };
    //gaRes = { 3.87,0.15,17.43,0.04,4.33,0.14,2.77,0.00,1.29,-0.13,0.14,0.08,-0.02,0.09,0.02,0.34,0.01,0.00 };
    gaRes = { 4.46,-0.25,15.87,-0.15,2.39,0.10,1.87,-0.17,1.02,-0.21,-0.15,-0.18,-0.04,-0.08,-0.03,-0.04,0.21,0.00 };
 
    std::vector< std::pair<std::string,std::string> > fileNameLabels;
    fileNameLabels.push_back( std::make_pair("tmpSpace/tree_forGA_removeBsBdOnly/tree_forGA_.2016RunC_noReduced_removeBsBd.root", "2016RunC") );
    fileNameLabels.push_back( std::make_pair("tmpSpace/tree_forGA_removeBsBdOnly/tree_forGA_.2016RunD_noReduced_removeBsBd.root", "2016RunD") );
    fileNameLabels.push_back( std::make_pair("tmpSpace/tree_forGA_removeBsBdOnly/tree_forGA_.2016RunE_noReduced_removeBsBd.root", "2016RunE") );
    fileNameLabels.push_back( std::make_pair("tmpSpace/tree_forGA_removeBsBdOnly/tree_forGA_.2016RunF_noReduced_removeBsBd.root", "2016RunF") );
    fileNameLabels.push_back( std::make_pair("tmpSpace/tree_forGA_removeBsBdOnly/tree_forGA_.2016RunG_noReduced_removeBsBd.root", "2016RunG") );
    fileNameLabels.push_back( std::make_pair("tmpSpace/tree_forGA_removeBsBdOnly/tree_forGA_2016RunH_07Aug2017ReReco_noReduced_removeBsBd.root", "2016RunH") );

    TCanvas* c1 = new TCanvas("c1", "", 1600, 1000);
    c1->SaveAs("storefig/sepRunsHistos.pdf[");
    // chg 1
    TH1D* h[fileNameLabels.size()];
    for ( unsigned i=0; i<fileNameLabels.size(); ++i )
        h[i] = new TH1D( fileNameLabels[i].second.c_str(), fileNameLabels[i].second.c_str(),  50, 5.4, 5.9 );
    TH1D* hVar = new TH1D("hvar", "", 100, 0., 1.);

    for ( unsigned idx = 0; idx < fileNameLabels.size(); ++idx )
    {
        const std::string& fName = fileNameLabels[idx].first;
        const std::string& label = fileNameLabels[idx].second;
        TFile* dataFile = TFile::Open( fName.c_str() );
        readData data(nullptr);
        data.SetInputFile( dataFile );
        data.LoadSourceBranch();
        TTree* dataTree = data.readTree();

        unsigned i = 0;
        unsigned N = dataTree->GetEntries();
        while ( i != N )
        {
            dataTree->GetEntry(i++);
            if ( data.readD[readData::lbtkMass             ] > 5.90 ) continue; 
            if ( data.readD[readData::lbtkMass             ] < 5.4  ) continue; 
            hVar->Fill( data.readD[readData::lbtkVtxprob] );

            if ( data.readD[readData::lbtkFlightDistanceSig] < gaRes[ 0] ) continue;
            if ( data.readD[readData::lbtkVtxprob          ] < gaRes[ 1] ) continue;
            if ( data.readD[readData::lbtkPt               ] < gaRes[ 2] ) continue;
            if ( data.readD[readData::tktkPt               ] < gaRes[ 4] ) continue;
            if ( data.readD[readData::ptkPt                ] < gaRes[ 6] ) continue;
            if ( data.readD[readData::ntkPt                ] < gaRes[ 8] ) continue;
            h[idx]->Fill(data.readD[readData::lbtkMass]);
        }

        h[idx]->Draw();
        char outHistName[128];
        sprintf( outHistName, "storefig/h_GARes_%s.eps", label.c_str() );
        c1->SaveAs( outHistName );
        c1->SaveAs( "storefig/sepRunsHistos.pdf" );
        h[idx]->SetDirectory(0);
        dataFile->Close();
    }
    c1->Clear();
    c1->Divide(3,2);
    for ( unsigned idx = 0; idx < fileNameLabels.size(); ++idx )
    {
        c1->cd(idx+1);
        h[idx]->Draw();
    }
    c1->SaveAs( "storefig/sepRunsHistos.eps" );
    c1->SaveAs( "storefig/sepRunsHistos.pdf" );
    c1->SaveAs( "storefig/sepRunsHistos.pdf]" );
    
    //RooRealVar




    delete c1;
    for ( unsigned idx = 0; idx < fileNameLabels.size(); ++idx )
        delete h[idx];
}
