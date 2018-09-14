#include "histProduce/histProduce/interface/rooTHMainTkTk.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "math.h"


root_TreeHistoMain_TkTk::root_TreeHistoMain_TkTk( TFileDirectory* d ) :
    root_TreeHistoMain( d, "TkTk" ), formatTree_TkTk( totNumD, totNumI ),
    kaonMass ( 0.493667 ), protonMass ( 0.9382720813 ), pionMass ( 0.13957061 )
{
    setInputTreeName( "lbSpecificDecay/TkTk" );

    if ( !d ) return;
    RegTree();
    RegHisto();
}
void root_TreeHistoMain_TkTk::Process( unsigned int pIdx )
{
    try
    {
        Clear();
        readTree()->GetEntry( pIdx );

        // preselection {{{

        // preselection end }}}
        if ( readD[fake_Lam0Mass] > 1. && readD[fake_Lam0Mass] < 1.2 )
            fillHisto( "mass_fakeLam0", readD[fake_Lam0Mass] );

        thisTree()->Fill();
    } catch (...) {}
}

void root_TreeHistoMain_TkTk::RegTree()
{
    if ( NoOutput() ) return;

    TTree* t = thisTree();
    RegFormatTree(t);
    // t->Branch("newBranch", &dataD[totNumD+newBranch], "newBranch/D" );
    return;
}
void root_TreeHistoMain_TkTk::RegHisto()
{
    if ( NoOutput() ) return;
    createHisto( "mass_fakelam0", 50, 1., 1.2 );
}


void root_TreeHistoMain_TkTk::LoadSourceBranch()
{
    TTree* t = readTree();
    LoadFormatSourceBranch(t);
    return;
}
void root_TreeHistoMain_TkTk::SummaryCalc()
{
    return;
}
