#include "histProduce/CMSSWTreeMgr/interface/rooTHMainGGPhoton.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "math.h"
#include "TCanvas.h"
#include "TPaveText.h"
#include "TFile.h"
#include "TH1D.h"
#include "TLine.h"


//root_TreeHistoMain_GGPhoton::root_TreeHistoMain_GGPhoton( TFile* d ) :
root_TreeHistoMain_GGPhoton::root_TreeHistoMain_GGPhoton( TFileDirectory* d ) :
    root_TreeHistoMain( d, "GGPhoton" ),
    phoLoadF(), interData()
{
    setInputTreeName( "ggNtuplizer/EventTree" );


    if ( !d ) return;
    //RegTree();
    RegHisto();
    return;
}

root_TreeHistoMain_GGPhoton::~root_TreeHistoMain_GGPhoton()
{}

void root_TreeHistoMain_GGPhoton::Process( unsigned int evtIdx )
{
    // Clear();
    GetEntry( evtIdx );
    fillHisto("mass_LbTk", 5.619);
    fillHisto("nPho", phoLoadF.size());
    //std::cout << phoLoadF.size() << std::endl;

    //for ( int iCand = 0; iCand < phoLoadF.size(); ++iCand )
    //{
    //    phoLoadF.atCand(iCand);
        //phoLoadF.atCand(0);
        //fillHisto( "eta", phoLoadF.var(varF::phoEta) );
        if ( phoLoadF.size() )
        {
            phoLoadF.atCand(0);
            fillHisto( "eta", phoLoadF.var(varF::phoEta) );
        }
    //}

    return;
}
void root_TreeHistoMain_GGPhoton::GetEntry( unsigned int evtIdx )
{
    thisTree()->GetEntry(evtIdx);
    //interData.GetEntry(evtIdx);
    evtContentReader.GetEntry(evtIdx);
    phoLoadF.LoadEvtContent(evtContentReader);

    return;
}

void root_TreeHistoMain_GGPhoton::RegTree()
{
    if ( NoOutput() ) return;

    TTree* t = thisTree();
    phoLoadF.RegFormatTree(t);
    return;
}

void root_TreeHistoMain_GGPhoton::RegHisto()
{
    if ( NoOutput() ) return;
    createHisto( "mass_LbTk", 45, 5.5, 5.8 );
    createHisto( "nPho", 10,0.,10. );
    createHisto( "eta", 80, -4.,4. );
    createHisto( "eta2", 80, -4.,4. );
}


void root_TreeHistoMain_GGPhoton::LoadSourceBranch()
{
    TTree* t = readTree();
    phoLoadF.LoadFormatSourceBranch(t);
    interData.InitContent(t);

    return;
}

void root_TreeHistoMain_GGPhoton::SummaryCalc()
{
}
