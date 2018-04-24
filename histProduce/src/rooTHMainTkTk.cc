#include "histProduce/histProduce/interface/rooTHMainTkTk.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "math.h"


root_TreeHistoMain_TkTk::root_TreeHistoMain_TkTk( TFileDirectory* d ) :
    root_TreeHistoMain( d, "TkTk" ),
    kaonMass ( 0.493667 ), protonMass ( 0.9382720813 ), pionMass ( 0.13957061 )
{
    setInputTreeName( "lbSpecificDecay/TkTk" );
    RegTree();
    RegHisto();
}
void root_TreeHistoMain_TkTk::Process( unsigned int i )
{
    try
    {
        // preselect events {{{
        Clear();
        readTree()->GetEntry( i );

        // preselection {{{

        // preselection end }}}
        if ( readD[fake_Lam0Mass] > 1. && readD[fake_Lam0Mass] < 1.2 )
            fillHisto( "mass_fakeLam0", readD[fake_Lam0Mass] );

        thisTree()->Fill();
    } catch (...) {}
}

void root_TreeHistoMain_TkTk::Clear()
{
    memset( dataD, 0x00, totNumD*sizeof( double ) );
    memset( dataI, 0x00, totNumI*sizeof( int ) );
}
void root_TreeHistoMain_TkTk::RegTree()
{
    // don't reg tree.
    return;
    if ( NoOutput() ) return;

    TTree* t = thisTree();
    t->Branch( "tktkMass", &dataD[tktkMass], "tktkMass/D" );
    t->Branch( "tktkFD2d", &dataD[tktkFlightDistance2d], "tktkFD2d/D" );
    t->Branch( "tktkVtxprob", &dataD[tktkVtxprob], "tktkVtxprob/D" );
    t->Branch( "tktkPt", &dataD[tktkPt], "tktkPt/D" );
    t->Branch( "fake_Lam0Mass", &dataD[fake_Lam0Mass], "fake_Lam0Mass/D" );
    t->Branch( "fake_KstarMass", &dataD[fake_KstarMass], "fake_KstarMass/D" );
    t->Branch( "fake_KshortMass", &dataD[fake_KshortMass], "fake_KshortMass/D" );
    t->Branch( "fake_PhiMass", &dataD[fake_PhiMass], "fake_PhiMass/D" );
    t->Branch( "ptkMom", &dataD[ptkMom], "ptkMom/D" );
    t->Branch( "ptkDEDX.Harmonic", &dataD[ptkDEDX_Harmonic], "ptkDEDX.Harmonic/D" );
    t->Branch( "ptkDEDX.pixelHrm", &dataD[ptkDEDX_pixelHrm], "ptkDEDX.pixelHrm/D" );
    t->Branch( "ptkIPt", &dataD[ptkIPt], "ptkIPt/D" );
    t->Branch( "ptkIPtErr", &dataD[ptkIPtErr], "ptkIPtErr/D" );
    t->Branch( "ntkMom", &dataD[ntkMom], "ntkMom/D" );
    t->Branch( "ntkDEDX.Harmonic", &dataD[ntkDEDX_Harmonic], "ntkDEDX.Harmonic/D" );
    t->Branch( "ntkDEDX.pixelHrm", &dataD[ntkDEDX_pixelHrm], "ntkDEDX.pixelHrm/D" );
    t->Branch( "ntkIPt", &dataD[ntkIPt], "ntkIPt/D" );
    t->Branch( "ntkIPtErr", &dataD[ntkIPtErr], "ntkIPtErr/D" );

    // t->Branch("newBranch", &dataD[totNumD+newBranch], "newBranch/D" );
}
void root_TreeHistoMain_TkTk::RegHisto()
{
    if ( NoOutput() ) return;
    createHisto( "mass_fakelam0", 50, 1., 1.2 );
}


void root_TreeHistoMain_TkTk::LoadSourceBranch()
{
    TTree* t = readTree();
    t->SetBranchAddress( "tktkMass", &readD[tktkMass] );
    t->SetBranchAddress( "tktkFD2d", &readD[tktkFlightDistance2d] );
    t->SetBranchAddress( "tktkVtxprob", &readD[tktkVtxprob] );
    t->SetBranchAddress( "tktkPt", &readD[tktkPt] );
    t->SetBranchAddress( "fake_Lam0Mass", &readD[fake_Lam0Mass] );
    t->SetBranchAddress( "fake_KstarMass", &readD[fake_KstarMass] );
    t->SetBranchAddress( "fake_KshortMass", &readD[fake_KshortMass] );
    t->SetBranchAddress( "fake_PhiMass", &readD[fake_PhiMass] );
    t->SetBranchAddress( "ptkMom", &readD[ptkMom] );
    t->SetBranchAddress( "ptkDEDX.Harmonic", &readD[ptkDEDX_Harmonic] );
    t->SetBranchAddress( "ptkDEDX.pixelHrm", &readD[ptkDEDX_pixelHrm] );
    t->SetBranchAddress( "ptkIPt", &readD[ptkIPt] );
    t->SetBranchAddress( "ptkIPtErr", &readD[ptkIPtErr] );
    t->SetBranchAddress( "ntkMom", &readD[ntkMom] );
    t->SetBranchAddress( "ntkDEDX.Harmonic", &readD[ntkDEDX_Harmonic] );
    t->SetBranchAddress( "ntkDEDX.pixelHrm", &readD[ntkDEDX_pixelHrm] );
    t->SetBranchAddress( "ntkIPt", &readD[ntkIPt] );
    t->SetBranchAddress( "ntkIPtErr", &readD[ntkIPtErr] );
}
void root_TreeHistoMain_TkTk::SummaryCalc()
{
    return;
}
