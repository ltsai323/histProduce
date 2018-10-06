#include "histProduce/histProduce/interface/formatTreepnLbTk.h"
#include "histProduce/histProduce/interface/formatTreeLbTk.h"
#include <math.h>

#include "TFile.h"
#include "TTree.h"
#include "TNtupleD.h"
#include "TH1D.h"
#include "TPaveText.h"

// target: use algorithm to pick up candidate or anti candidate.
// Algorithm now: check lb and anti lb mass.
// if lb & anti lb are in signal region, give up this event.
// choose the one is closed to signal region.

typedef formatTree_LbTk uniqueData;
typedef formatTree_plusminus_LbTk pnData;

bool inSignalRegion( double m );
double massFromSig( double m );
bool isBs( double mBs, double mPhi );
bool isBd( double mBd, double mKx );
int main()
{
    // get old tree
    TFile* fOld = TFile::Open("storeroot/tReduced/tree_forGA_removeBsBdOnly/data_2016RunBCDEFGH.root");
    TTree* oldTree = (TTree*) fOld->Get("lbSpecificDecay/pnLbTk");
    pnData oldData(pnData::comNumD, pnData::comNumI);
    oldData.LoadFormatSourceBranch(oldTree);

    // reg new tree
    TFile* fNew = new TFile("newFile.root", "RECREATE");
    TTree* newTree = new TTree("LbTk", "simplified Tree");
    newTree->SetDirectory(fNew);
    uniqueData newData(uniqueData::comNumD,uniqueData::comNumI);
    newData.RegFormatTree(newTree);

    TH1D* hSummary = new TH1D("hCheck", "histogram to check this event is p or n or nothing", 5, -2., 2.);
    hSummary->SetDirectory(fNew);

    const int nOffset = pnData::pkaonIPtErr + 1 ;
    unsigned i = 0;
    unsigned N = oldTree->GetEntries();
    while ( i != N )
    {
        oldTree->GetEntry(i++);
        newData.Clear();

        if (oldData.readD[pnData::plbtkMass] > 5.95 ) continue;
        if (oldData.readD[pnData::nlbtkMass] > 5.95 ) continue;
        if (oldData.readD[pnData::plbtkMass] < 5.20 ) continue;
        if (oldData.readD[pnData::nlbtkMass] < 5.20 ) continue;
        
        bool unableToDistinguish = false;
        if ( inSignalRegion(oldData.readD[pnData::plbtkMass]) && inSignalRegion(oldData.readD[pnData::nlbtkMass]) ) unableToDistinguish = true;
        if ( unableToDistinguish ) continue;
        
        int pnSwitch = -1;
        if ( massFromSig(oldData.readD[pnData::plbtkMass]) > massFromSig(oldData.readD[pnData::nlbtkMass]) ) pnSwitch = 1;
        else if ( massFromSig(oldData.readD[pnData::plbtkMass]) < massFromSig(oldData.readD[pnData::nlbtkMass]) ) pnSwitch = 0;
        hSummary->Fill(pnSwitch);
        if ( isBs(oldData.readD[pnData::pfake_BsMass], oldData.readD[pnData::pfake_PhiMass]) ) continue;
        if ( isBd(oldData.readD[pnData::pfake_BdMass], oldData.readD[pnData::pfake_KstarMass]) ) continue;
        if ( isBs(oldData.readD[pnData::nfake_BsMass], oldData.readD[pnData::nfake_PhiMass]) ) continue;
        if ( isBd(oldData.readD[pnData::nfake_BdMass], oldData.readD[pnData::nfake_KstarMass]) ) continue;
        //pnSwitch = 1;
        if ( pnSwitch == -1 ) continue;


        const int dataOffset = pnSwitch * nOffset;
        for ( int J=0;J < uniqueData::comNumD; ++J )
            newData.dataD[J] = oldData.readD[dataOffset+J];
        newTree->Fill();
    }
    std::cout << "print out lbtkMass = " << uniqueData::lbtkMass << ", plbtkMass = " << pnData::plbtkMass << ", nlbtkMass = " << pnData::nlbtkMass << ". And offset number is " << uniqueData::comNumD << ", nOffset = " << nOffset << std::endl;
    fNew->Write();
    fNew->Close();
    fOld->Close();
}
bool inSignalRegion( double m )
{
    if ( m < 5.619 - 0.02 ) return false;
    if ( m > 5.619 + 0.02 ) return false;
    return true;
}

double massFromSig( double m )
{ return fabs(m-5.637); }
bool isBd( double mBd, double mKx )
{ if ( mBd > 5.22 && mBd < 5.32 && mKx > 0.85 && mKx < 0.95 ) return true; return false; }
bool isBs( double mBs, double mPhi )
{ if ( mBs > 5.32 && mBs < 5.38 && mPhi > 1.01 && mPhi < 1.03 ) return true; return false; }
