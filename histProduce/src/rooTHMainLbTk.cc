#include "histProduce/histProduce/interface/rooTHMainLbTk.h"
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
    const int NUM_TESTHIST = 1;
    const char* testingCut = "FDSig>";
    const double cutInit = 100;
    const double cutInterval = 50;
}

root_TreeHistoMain_LbTk::root_TreeHistoMain_LbTk( TFileDirectory* d ) :
    root_TreeHistoMain( d, "LbTk" ),
    kaonMass ( 0.493667 ), protonMass ( 0.9382720813 ), pionMass ( 0.13957061 )
{
    setInputTreeName( "lbSpecificDecay/LbTk" );
    // gaRes = { 0.57,0.38,18.33,-0.01,4.94,-0.43,4.07,0.32,2.12,0.63,0.00,-0.87,-0.49,0.00,-0.88,0.52,-0.28,0.62 };
    gaRes = { 0.63,-1.10,19.24,0.00,3.06,1.06,3.64,0.71,2.61,0.92,0.00,0.06,0.13,-0.47,-0.80,3.42,-2.84,1.94 };

    if ( !d ) return;
    RegTree();
    RegHisto();
    return;
}

void root_TreeHistoMain_LbTk::Process( unsigned int i )
{
    try
    {
        Clear();
        readTree()->GetEntry( i );

        // preselection {{{
        if ( readD[ lbtkMass ] < 5.5 ) return;
        if ( readD[ lbtkMass ] > 5.8 ) return;
        // remove Bd
        if ( (readD[fake_BdMass]>5.22&&readD[fake_BdMass]<5.32
           && readD[fake_KstarMass]>0.85&&readD[fake_KstarMass]<0.95) ) return;
        // remove Bs
        if ( (readD[fake_BsMass]>5.32&&readD[fake_BsMass]<5.38
           && readD[fake_PhiMass]>1.01&&readD[fake_PhiMass]<1.03) ) return;

        // preselection end }}}

        // for smaller tree for GA use.
        if ( readD[lbtkMass] < 5.70 )
            if ( readD[lbtkMass] > 5.55 )
//                if ( readD[ptkPt] > 2.00 )
//                    if ( readD[ntkPt] > 1.00 )
//                        if ( readD[lbtkVtxprob] > 0.13 )
        {

            for ( int i=0; i<totNumD; ++i )
                dataD[i] = readD[i];
            // save reduced tree
            // thisTree()->Fill();
        }

        // for histogram part
        // test for GA result
        if ( readD[lbtkFlightDistanceSig  ] < gaRes[ 0] ) return;
        if ( readD[lbtkVtxprob            ] < gaRes[ 1] ) return;
        if ( readD[lbtkPt                 ] < gaRes[ 2] ) return;
        //if ( readD[lbtkMom                ] < gaRes[ 3] ) return;
        if ( readD[tktkPt                 ] < gaRes[ 4] ) return;
        //if ( readD[tktkMom                ] < gaRes[ 5] ) return;
        if ( readD[ptkPt                  ] < gaRes[ 6] ) return;
        //if ( readD[ptkMom                 ] < gaRes[ 7] ) return;
        if ( readD[ntkPt                  ] < gaRes[ 8] ) return;
        //if ( readD[ntkMom                 ] < gaRes[ 9] ) return;
        //if ( readD[ptkDEDX_Mom_ratio] < gaRes[10] ) return;
        //if ( readD[ptkDEDX_Mom_ratio] > gaRes[11] ) return;
        //if ( readD[ntkDEDX_Mom_ratio] < gaRes[12] ) return;
        //if ( readD[ntkDEDX_Mom_ratio] > gaRes[13] ) return;
        //if ( readD[ptkIPt]/readD[ptkIPtErr] < gaRes[14] ) return;
        //if ( readD[ptkIPt]/readD[ptkIPtErr] > gaRes[15] ) return;
        //if ( readD[ntkIPt]/readD[ntkIPtErr] < gaRes[16] ) return;
        //if ( readD[ntkIPt]/readD[ntkIPtErr] > gaRes[17] ) return;
        
        // cut for drawing histogram
        /*
        if ( readD[ptkPt] < 2.60 ) return;
        if ( readD[ntkPt] < 1.30 ) return;
        if ( readD[lbtkFlightDistanceSig] < 0.111 ) return;
        if ( readD[lbtkVtxprob] < 0.15 ) return;
        if ( readD[lbtkPt] < 15. ) return;
        if ( readD[tktkPt] < 4.3  ) return;
        */
        fillHisto( "mass_LbTk", readD[lbtkMass] );

        for ( int i=0; i<NUM_TESTHIST; ++i )
        {
            // small sig && large pt
            //if ( readD[lbtkPt] > 30. ) continue;
            //if ( readD[lbtkFlightDistanceSig] > 0.15 ) continue;
            //double cutVal = cutInit + double(i)*cutInterval;

            // test cut put
            //if ( readD[lbtkFlightDistanceSig] < cutVal ) continue;
            //if ( readD[lbtkPt] > cutVal ) continue;
            char testhist[20];
            sprintf( testhist, "testCut%02d", i );
            std::string sname = testhist;

            fillHisto( sname, readD[lbtkMass] );
            break;
        }

    } catch (...) {}
}

void root_TreeHistoMain_LbTk::Clear()
{
    memset( dataD, 0x00, totNumD*sizeof( double ) );
    memset( dataI, 0x00, totNumI*sizeof( int ) );
}
void root_TreeHistoMain_LbTk::RegTree()
{
    // don't reg tree.
    // return;
    if ( NoOutput() ) return;

    TTree* t = thisTree();
    t->Branch( "lbtkMass", &dataD[lbtkMass], "lbtkMass/D" );
    t->Branch( "lbtkFD2d", &dataD[lbtkFlightDistance2d], "lbtkFD2d/D" );
    t->Branch( "lbtkFDSig", &dataD[lbtkFlightDistanceSig], "lbtkFDSig/D" );
    t->Branch( "lbtkVtxprob", &dataD[lbtkVtxprob], "lbtkVtxprob/D" );

    t->Branch( "lbtkMom", &dataD[lbtkMom], "lbtkMom/D" );
    t->Branch( "lbtkPt", &dataD[lbtkPt], "lbtkPt/D" );
    t->Branch( "tktkPt", &dataD[tktkPt], "tktkPt/D" );
    t->Branch( "tktkMom", &dataD[tktkMom], "tktkMom/D" );

    t->Branch( "fake_Lam0Mass", &dataD[fake_Lam0Mass], "fake_Lam0Mass/D" );
    t->Branch( "fake_LbL0Mass", &dataD[fake_LbL0Mass], "fake_LbL0Mass/D" );
    t->Branch( "fake_KstarMass", &dataD[fake_KstarMass], "fake_KstarMass/D" );
    t->Branch( "fake_BdMass", &dataD[fake_BdMass], "fake_BdMass/D" );
    t->Branch( "fake_PhiMass", &dataD[fake_PhiMass], "fake_PhiMass/D" );
    t->Branch( "fake_BsMass", &dataD[fake_BsMass], "fake_BsMass/D" );
    t->Branch( "fake_KshortMass", &dataD[fake_KshortMass], "fake_KshortMass/D" );
    t->Branch( "fake_mumupipiMass", &dataD[fake_mumupipiMass], "fake_mumupipiMass/D" );

    t->Branch( "ptkPt", &dataD[ptkPt], "ptkPt/D" );
    t->Branch( "ptkMom", &dataD[ptkMom], "ptkMom/D" );
    t->Branch( "ptkDEDX.Harmonic", &dataD[ptkDEDX_Harmonic], "ptkDEDX.Harmonic/D" );
    t->Branch( "ptkDEDX.pixelHrm", &dataD[ptkDEDX_pixelHrm], "ptkDEDX.pixelHrm/D" );
    t->Branch( "ptkIPt", &dataD[ptkIPt], "ptkIPt/D" );
    t->Branch( "ptkIPtErr", &dataD[ptkIPtErr], "ptkIPtErr/D" );

    t->Branch( "ntkPt", &dataD[ntkPt], "ntkPt/D" );
    t->Branch( "ntkMom", &dataD[ntkMom], "ntkMom/D" );
    t->Branch( "ntkDEDX.Harmonic", &dataD[ntkDEDX_Harmonic], "ntkDEDX.Harmonic/D" );
    t->Branch( "ntkDEDX.pixelHrm", &dataD[ntkDEDX_pixelHrm], "ntkDEDX.pixelHrm/D" );
    t->Branch( "ntkIPt", &dataD[ntkIPt], "ntkIPt/D" );
    t->Branch( "ntkIPtErr", &dataD[ntkIPtErr], "ntkIPtErr/D" );
    return;
}

void root_TreeHistoMain_LbTk::RegHisto()
{
    if ( NoOutput() ) return;
    createHisto( "mass_LbTk", 30, 5.5, 5.7 );
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


void root_TreeHistoMain_LbTk::LoadSourceBranch()
{
    TTree* t = readTree();
    t->SetBranchAddress( "lbtkMass", &readD[lbtkMass] );
    t->SetBranchAddress( "lbtkFD2d", &readD[lbtkFlightDistance2d] );
    t->SetBranchAddress( "lbtkFDSig", &readD[lbtkFlightDistanceSig] );
    t->SetBranchAddress( "lbtkVtxprob", &readD[lbtkVtxprob] );

    t->SetBranchAddress( "lbtkMom", &readD[lbtkMom] );
    t->SetBranchAddress( "lbtkPt", &readD[lbtkPt] );
    t->SetBranchAddress( "tktkPt", &readD[tktkPt] );
    t->SetBranchAddress( "tktkMom", &readD[tktkMom] );

    t->SetBranchAddress( "fake_Lam0Mass", &readD[fake_Lam0Mass] );
    t->SetBranchAddress( "fake_LbL0Mass", &readD[fake_LbL0Mass] );
    t->SetBranchAddress( "fake_KstarMass", &readD[fake_KstarMass] );
    t->SetBranchAddress( "fake_BdMass", &readD[fake_BdMass] );
    t->SetBranchAddress( "fake_PhiMass", &readD[fake_PhiMass] );
    t->SetBranchAddress( "fake_BsMass", &readD[fake_BsMass] );
    t->SetBranchAddress( "fake_KshortMass", &readD[fake_KshortMass] );
    t->SetBranchAddress( "fake_mumupipiMass", &readD[fake_mumupipiMass] );

    t->SetBranchAddress( "ptkPt", &readD[ptkPt] );
    t->SetBranchAddress( "ptkMom", &readD[ptkMom] );
    t->SetBranchAddress( "ptkDEDX.Harmonic", &readD[ptkDEDX_Harmonic] );
    t->SetBranchAddress( "ptkDEDX.pixelHrm", &readD[ptkDEDX_pixelHrm] );
    t->SetBranchAddress( "ptkIPt", &readD[ptkIPt] );
    t->SetBranchAddress( "ptkIPtErr", &readD[ptkIPtErr] );

    t->SetBranchAddress( "ntkPt", &readD[ntkPt] );
    t->SetBranchAddress( "ntkMom", &readD[ntkMom] );
    t->SetBranchAddress( "ntkDEDX.Harmonic", &readD[ntkDEDX_Harmonic] );
    t->SetBranchAddress( "ntkDEDX.pixelHrm", &readD[ntkDEDX_pixelHrm] );
    t->SetBranchAddress( "ntkIPt", &readD[ntkIPt] );
    t->SetBranchAddress( "ntkIPtErr", &readD[ntkIPtErr] );
}

void root_TreeHistoMain_LbTk::SummaryCalc()
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
