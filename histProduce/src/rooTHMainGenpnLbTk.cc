#include "histProduce/histProduce/interface/rooTHMainGenpnLbTk.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "math.h"
#include "TCanvas.h"
#include "TPaveText.h"
#include "TFile.h"
#include "TH1D.h"
#include "TLine.h"

namespace
{
    //const unsigned NUM_TESTHIST = 20;
    const int NUM_TESTHIST = 20;
    const char* testingCut = "FDSig>";
    const double cutInit = 100;
    const double cutInterval = 50;
}

root_TreeHistoMain_GenInfo_plusminus_LbTk::root_TreeHistoMain_GenInfo_plusminus_LbTk( TFileDirectory* d ) :
    root_TreeHistoMain( d, "pnLbTkGenInfo" ), formatTree_plusminus_LbTk( totNumD, totNumI ),
    kaonMass ( 0.493667 ), protonMass ( 0.9382720813 ), pionMass ( 0.13957061 )
{
    setInputTreeName( "lbSpecificDecay/pnLbTkGenInfo" );
    // gaRes = { 0.00, -0.69, 11.50, 12.47, 6.50, 5.67, 4.48, 4.51, 2.00, 2.11, 2.47, 18.95, -0.21, 15.05, -12.17, 11.88, -16.30, 19.57 };
    //gaRes = { 3.87,0.15,17.43,0.04,4.33,0.14,2.77,0.00,1.29,-0.13,0.14,0.08,-0.02,0.09,0.02,0.34,0.01,0.00 };
    gaRes = { 0.00,0.00, 0.00,0.00,0.00,0.00,0.00,0.00,0.00, 0.00,0.00,0.00, 0.00,0.00,0.00,0.00,0.00,0.00 };

    if ( !d ) return;
    RegTree();
    //RegHisto();
    return;
}

void root_TreeHistoMain_GenInfo_plusminus_LbTk::Process( unsigned int pIdx )
{
    // use rthMain_LbTk, needed to be modified!
    try
    {
        Clear();
        readTree()->GetEntry( pIdx );

        // preselection {{{
        if ( readD[ plbtkMass ] < 5.0 ) return;
        if ( readD[ plbtkMass ] > 6.0 ) return;
        // remove Bd
        if ( (readD[pfake_BdMass]>5.22&&readD[pfake_BdMass]<5.32
           && readD[pfake_KstarMass]>0.85&&readD[pfake_KstarMass]<0.95) ) return;
        // remove Bs
        if ( (readD[pfake_BsMass]>5.32&&readD[pfake_BsMass]<5.38
           && readD[pfake_PhiMass]>1.01&&readD[pfake_PhiMass]<1.03) ) return;

        // preselection end }}}

        {

            for ( int i=0; i<totNumD; ++i )
                dataD[i] = readD[i];
            for ( int i=0; i<totNumI; ++i )
                dataI[i] = readI[i];
            thisTree()->Fill();
        }

        // for histogram part
        // test for GA result
        if ( readD[plbtkFlightDistanceSig  ] < gaRes[ 0] ) return;
        if ( readD[plbtkVtxprob            ] < gaRes[ 1] ) return;
        if ( readD[plbtkPt                 ] < gaRes[ 2] ) return;
        //if ( readD[plbtkMom                ] < gaRes[ 3] ) return;
        if ( readD[ptktkPt                 ] < gaRes[ 4] ) return;
        //if ( readD[ptktkMom                ] < gaRes[ 5] ) return;
        if ( readD[pptonPt                  ] < gaRes[ 6] ) return;
        //if ( readD[pptonMom                 ] < gaRes[ 7] ) return;
        if ( readD[pkaonPt                  ] < gaRes[ 8] ) return;


    } catch (...) {}
}

void root_TreeHistoMain_GenInfo_plusminus_LbTk::RegTree()
{
    if ( NoOutput() ) return;

    TTree* t = thisTree();
    RegFormatTree(t);

    t->Branch( "pptonPID", &dataI[pptonPID], "pptonPID/I" );
    t->Branch( "pkaonPID", &dataI[pkaonPID], "pkaonPID/I" );
    t->Branch( "pptonMomPID", &dataI[pptonMomPID], "pptonMomPID/I" );
    t->Branch( "pkaonMomPID", &dataI[pkaonMomPID], "pkaonMomPID/I" );
    t->Branch( "nptonPID", &dataI[nptonPID], "nptonPID/I" );
    t->Branch( "nkaonPID", &dataI[nkaonPID], "nkaonPID/I" );
    t->Branch( "nptonMomPID", &dataI[nptonMomPID], "nptonMomPID/I" );
    t->Branch( "nkaonMomPID", &dataI[nkaonMomPID], "nkaonMomPID/I" );
    return;
}

void root_TreeHistoMain_GenInfo_plusminus_LbTk::RegHisto()
{
    if ( NoOutput() ) return;
    createHisto( "mass_LbTk", 45, 5.5, 5.8 );
    createHisto( "mass_LbTk_testcut", 45, 5.5, 5.8 );
    createHisto( "mass_Lbtk_lowPtHighFDSig", 45, 5.5, 5.8 );
    createHisto( "mass_LbTk_highPtLowFDSig", 45, 5.5, 5.8 );
    createHisto( "par_FDSig_highPt", 100, 0., 20. );
    createHisto( "par_FDSig_lowPt", 100, 0., 20. );
    createHisto( "par_FDSig_midPt", 100, 0., 20. );
    createHisto( "testCut00", 45, 5.5, 5.8 );
    createHisto( "testCut01", 45, 5.5, 5.8 );
    createHisto( "testCut02", 45, 5.5, 5.8 );
    createHisto( "testCut03", 45, 5.5, 5.8 );
    createHisto( "testCut04", 45, 5.5, 5.8 );
    createHisto( "testCut05", 45, 5.5, 5.8 );
    createHisto( "testCut06", 45, 5.5, 5.8 );
    createHisto( "testCut07", 45, 5.5, 5.8 );
    createHisto( "testCut08", 45, 5.5, 5.8 );
    createHisto( "testCut09", 45, 5.5, 5.8 );
    createHisto( "testCut10", 45, 5.5, 5.8 );
    createHisto( "testCut11", 45, 5.5, 5.8 );
    createHisto( "testCut12", 45, 5.5, 5.8 );
    createHisto( "testCut13", 45, 5.5, 5.8 );
    createHisto( "testCut14", 45, 5.5, 5.8 );
    createHisto( "testCut15", 45, 5.5, 5.8 );
    createHisto( "testCut16", 45, 5.5, 5.8 );
    createHisto( "testCut17", 45, 5.5, 5.8 );
    createHisto( "testCut18", 45, 5.5, 5.8 );
    createHisto( "testCut19", 45, 5.5, 5.8 );
}


void root_TreeHistoMain_GenInfo_plusminus_LbTk::LoadSourceBranch()
{
    TTree* t = readTree();
    LoadFormatSourceBranch(t);

    t->SetBranchAddress( "pptonPID",    &readI[pptonPID] );
    t->SetBranchAddress( "pkaonPID",    &readI[pkaonPID] );
    t->SetBranchAddress( "pptonMomPID", &readI[pptonMomPID] );
    t->SetBranchAddress( "pkaonMomPID", &readI[pkaonMomPID] );
    t->SetBranchAddress( "nptonPID",    &readI[nptonPID] );
    t->SetBranchAddress( "nkaonPID",    &readI[nkaonPID] );
    t->SetBranchAddress( "nptonMomPID", &readI[nptonMomPID] );
    t->SetBranchAddress( "nkaonMomPID", &readI[nkaonMomPID] );
    return;
}

void root_TreeHistoMain_GenInfo_plusminus_LbTk::SummaryCalc()
{
    if ( NoOutput() ) return;
    const std::string preName = getPreName();
    char fName[48] ; sprintf( fName , "h_sum_%s.pdf" , preName.c_str() );
    char fNameI[48]; sprintf( fNameI, "h_sum_%s.pdf[", preName.c_str() );
    char fNameF[48]; sprintf( fNameF, "h_sum_%s.pdf]", preName.c_str() );


    TCanvas* c1 = new TCanvas( "c1", "", 1000, 1000 );
    c1->SaveAs( fNameI );

    const std::map<std::string,TH1D*>& myMap = getHistos();

    std::map<std::string,TH1D*>::const_iterator iter;
    std::map<std::string,TH1D*>::const_iterator iend = myMap.cend  ();
    for ( int i=0; i<20; ++i )
    {
        char hName[20];
        sprintf( hName, "testCut%02d", i );
        const std::string hname = hName;
        iter = myMap.find( hname );
        if ( iter == iend ) continue;

        TH1D* histo = iter->second;
        c1->Clear();
        histo->Draw();
        c1->Update();


        std::pair<double, double> signalBand( 5.60, 5.64 );
        usefulFuncs::SNRes sigNumRes = usefulFuncs::SignalNumberCalculator( histo, signalBand.first, signalBand.second );

        // print summary text {{{
        TPaveText* txt = new TPaveText( 0.6, 0.5, 0.89, 0.75, "NDC" );
        char myText[26] = {'\0'};

        sprintf( myText, "%s: %.5f", testingCut, cutInit + cutInterval * double(i) );
        txt->AddText(myText);
        sprintf( myText, "# of signal: %d +- %.3f", sigNumRes.Nsig, sigNumRes.ErrSig );
        txt->AddText(myText);
        sprintf( myText,"S/(sqrt(S+B) = %.2f", double(sigNumRes.Nsig) / sqrt(double(sigNumRes.N)) );
        txt->AddText(myText);
        txt->Draw("same");
        // print summary text end }}}

        // print signal band line {{{
        TLine* lfirst = new TLine( signalBand.first , c1->GetUymin(), signalBand.first , c1->GetUymax() );
        TLine* lsecond= new TLine( signalBand.second, c1->GetUymin(), signalBand.second, c1->GetUymax() );
        lfirst ->SetLineWidth(1); lfirst ->SetLineColor(1); lfirst ->SetLineStyle(2);
        lsecond->SetLineWidth(1); lsecond->SetLineColor(1); lsecond->SetLineStyle(2);
        lfirst->Draw("same"); lsecond->Draw("same");
        // print signal band line end }}}

        c1->SaveAs( fName );
        delete txt;
        delete lfirst; delete lsecond;
    }
    c1->SaveAs( fNameF );

}
