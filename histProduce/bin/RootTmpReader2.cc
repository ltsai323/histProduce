#include <iostream>
#include <vector>
//#include "histProduce/histProduce/interface/rooTHMainLbTk.h"
#include "histProduce/histProduce/interface/rooTHMainpnLbTk.h"

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

typedef root_TreeHistoMain_plusminus_LbTk readData;

bool inSignalRegion( double mass );
int main()
{
    std::vector<double> gaRes;
    gaRes = { 4.46,-0.25,15.87,-0.15,2.39,0.10,1.87,-0.17,1.02,-0.21,-0.15,-0.18,-0.04,-0.08,-0.03,-0.04,0.21,0.00 };
 
    std::vector< std::pair<std::string,std::string> > fileNameLabels;
    fileNameLabels.push_back( std::make_pair("storeroot/tReduced/tree_forGA_removeBsBdOnly/data_2016RunCDEFGH.root", "2016Data") );
    fileNameLabels.push_back( std::make_pair("storeroot/tReduced/tree_forGA_removeBsBdOnly/data_2016RunBCDEF.root" , "2017Data") );

    TCanvas* c1 = new TCanvas("c1", "", 1600, 1000);
    c1->SaveAs("storefig/tmp2.sepRunsHistos.pdf[");
    // chg 1
    unsigned fileNum = fileNameLabels.size();
    TH1D* h[fileNum];
    TH1D* hBlind[fileNum];
    TH1D* hPt[fileNum];
    for ( unsigned i=0; i<fileNum; ++i )
    {
        h[i]        = new TH1D( fileNameLabels[i].second.c_str(), fileNameLabels[i].second.c_str(),  50, 5.4, 5.9 );
        hBlind[i]   = new TH1D( fileNameLabels[i].second.c_str(), fileNameLabels[i].second.c_str(),  50, 5.4, 5.9 );
        hPt[i]      = new TH1D( (fileNameLabels[i].second+"Pt").c_str(), (fileNameLabels[i].second+" pt distribution").c_str(), 100, 0., 100. );
    }

    for ( unsigned idx = 0; idx < fileNum; ++idx )
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
            if ( data.readD[readData::plbtkMass             ] > 5.90 ) continue; 
            if ( data.readD[readData::plbtkMass             ] < 5.4  ) continue; 

            if ( data.readD[readData::plbtkFlightDistanceSig] < gaRes[ 0] ) continue;
            if ( data.readD[readData::plbtkVtxprob          ] < gaRes[ 1] ) continue;
            if ( data.readD[readData::plbtkPt               ] < gaRes[ 2] ) continue;
            if ( data.readD[readData::ptktkPt               ] < gaRes[ 4] ) continue;
            if ( data.readD[readData::pptonPt              ] < gaRes[ 6] ) continue;
            if ( data.readD[readData::pkaonPt              ] < gaRes[ 8] ) continue;
            h[idx]->Fill( data.readD[readData::plbtkMass] );
            if ( inSignalRegion(data.readD[readData::plbtkMass]) )
                hPt[idx]->Fill( data.readD[readData::plbtkPt] );
            else
                hBlind[idx]->Fill( data.readD[readData::plbtkMass] );

        }

        h[idx]->Draw();
        char outHistName[128];
        sprintf( outHistName, "storefig/tmp2.h_GARes_%s.eps", label.c_str() );
        c1->SaveAs( outHistName );
        c1->SaveAs( "storefig/tmp2.sepRunsHistos.pdf" );
        hPt[idx]->Draw();
        sprintf( outHistName, "storefig/tmp2.h_GARes_%s_ptDistribution.eps", label.c_str() );
        c1->SaveAs( outHistName );
        c1->SaveAs( "storefig/tmp2.sepRunsHistos.pdf" );
        h[idx]->SetDirectory(0);
        dataFile->Close();
    }
    c1->Clear();
    c1->Divide(2,1);
    for ( unsigned idx = 0; idx < fileNum; ++idx )
    {
        c1->cd(idx+1);
        h[idx]->Draw();
    }
    c1->SaveAs( "storefig/tmp2.sepRunsHistos.eps" );
    c1->SaveAs( "storefig/tmp2.sepRunsHistos.pdf" );
    c1->SaveAs( "storefig/tmp2.sepRunsHistos.pdf]");
    
    //RooRealVar




    delete c1;
    for ( unsigned idx = 0; idx < fileNum; ++idx )
    {
        delete h[idx];
        delete hPt[idx];
    }
}

// not yet complete!!!
bool inSignalRegion( double mass )
{
    if ( mass < 5.6196 - 1.00 ) return false;
    if ( mass > 5.6196 + 1.00 ) return false;
    return true;
}
