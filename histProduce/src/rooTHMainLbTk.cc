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
    const int NUM_TESTHIST = 0;
    const char* testingCut = "FDSig>";
    const double cutInit = 100;
    const double cutInterval = 50;
}

root_TreeHistoMain_LbTk::root_TreeHistoMain_LbTk( TFileDirectory* d ) :
    root_TreeHistoMain( d, "LbTk" ),formatTree_LbTk(totNumD, totNumI),
    kaonMass ( 0.493667 ), protonMass ( 0.9382720813 ), pionMass ( 0.13957061 )
{
    setInputTreeName( "lbSpecificDecay/LbTk" );
    //setInputTreeName( "treeCreatingSpecificDecay/SDanalyzer/LbTk" );
    
    // GA result for 2016RunH
    //gaRes = { 3.87,0.15,17.43,0.04,4.33,0.14,2.77,0.00,1.29,-0.13,0.14,0.08,-0.02,0.09,0.02,0.34,0.01,0.00 };
    // no preselection
    gaRes = { 0.00,0.00, 0.00,0.00,0.00,0.00,0.00,0.00,0.00, 0.00,0.00,0.00, 0.00,0.00,0.00,0.00,0.00,0.00 };

    if ( !d ) return;
    RegTree();
    RegHisto();
    return;
}

void root_TreeHistoMain_LbTk::Process( unsigned int pIdx )
{
    try
    {
        Clear();
        readTree()->GetEntry( pIdx );

        // preselection {{{
        if ( readD[ lbtkMass ] < 5.2 ) return;
        if ( readD[ lbtkMass ] > 5.95) return;
        // remove Bd
        if ( (readD[fake_BdMass]>5.22&&readD[fake_BdMass]<5.32
           && readD[fake_KstarMass]>0.85&&readD[fake_KstarMass]<0.95) ) return;
        // remove Bs
        if ( (readD[fake_BsMass]>5.32&&readD[fake_BsMass]<5.38
           && readD[fake_PhiMass]>1.01&&readD[fake_PhiMass]<1.03) ) return;

        // preselection end }}}

        // for smaller tree for GA use.
//        if ( readD[lbtkMass] < 5.70 )
//            if ( readD[lbtkMass] > 5.55 )
//                if ( readD[ptkPt] > 2.00 )
//                    if ( readD[ntkPt] > 1.00 )
//                        if ( readD[lbtkVtxprob] > 0.13 )
        {

            for ( int i=0; i<totNumD; ++i )
                dataD[i] = readD[i];
            // save reduced tree
            thisTree()->Fill();
        }

        // for histogram part
        // test for GA result
    //    if ( readD[lbtkFlightDistanceSig  ] < gaRes[ 0] ) return;
    //    if ( readD[lbtkVtxprob            ] < gaRes[ 1] ) return;
    //    if ( readD[lbtkPt                 ] < gaRes[ 2] ) return;
    //    //if ( readD[lbtkMom                ] < gaRes[ 3] ) return;
    //    if ( readD[tktkPt                 ] < gaRes[ 4] ) return;
        //if ( readD[tktkMom                ] < gaRes[ 5] ) return;
        if ( readD[ptkPt                  ] < gaRes[ 6] ) return;
        //if ( readD[ptkMom                 ] < gaRes[ 7] ) return;
        if ( readD[ntkPt                  ] < gaRes[ 8] ) return;
        fillHisto( "mass_LbToJPsipK", readD[lbtkMass] );
        fillHisto( "mass_target_JPsiP_total", readD[targetJpsiP_mass] );
        const double sig_mean = 5.61529;
        const double sig_width= 0.00857351;

        if ( readD[lbtkMass] > sig_mean - 2.5*sig_width && readD[lbtkMass] < sig_mean + 2.5*sig_width )
            fillHisto( "mass_target_JPsiP_inSignalRegion"    , readD[targetJpsiP_mass] );
        else
        if ( readD[lbtkMass] > sig_mean - 5.5*sig_width && readD[lbtkMass] < sig_mean - 3. *sig_width )
            fillHisto( "mass_target_JPsiP_inSideband", readD[targetJpsiP_mass] );
        else
        if ( readD[lbtkMass] > sig_mean + 3. *sig_width && readD[lbtkMass] < sig_mean + 5.5*sig_width )
            fillHisto( "mass_target_JPsiP_inSideband", readD[targetJpsiP_mass] );
        

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

void root_TreeHistoMain_LbTk::RegTree()
{
    if ( NoOutput() ) return;

    TTree* t = thisTree();
    RegFormatTree(t);
    return;
}

void root_TreeHistoMain_LbTk::RegHisto()
{
    if ( NoOutput() ) return;
    createHisto( "mass_LbToJPsipK", 50, 5.4, 5.9 );
    createHisto( "mass_target_JPsiP_total", 60, 4.0, 5.2 );
    createHisto( "mass_target_JPsiP_inSignalRegion", 60, 4.0, 5.2 );
    createHisto( "mass_target_JPsiP_inSideband", 60, 4.0, 5.2 );
    // createHisto( "testCut00", 45, 5.5, 5.8 );
    // createHisto( "testCut01", 45, 5.5, 5.8 );
    // createHisto( "testCut02", 45, 5.5, 5.8 );
    // createHisto( "testCut03", 45, 5.5, 5.8 );
    // createHisto( "testCut04", 45, 5.5, 5.8 );
    // createHisto( "testCut05", 45, 5.5, 5.8 );
    // createHisto( "testCut06", 45, 5.5, 5.8 );
    // createHisto( "testCut07", 45, 5.5, 5.8 );
    // createHisto( "testCut08", 45, 5.5, 5.8 );
    // createHisto( "testCut09", 45, 5.5, 5.8 );
    // createHisto( "testCut10", 45, 5.5, 5.8 );
    // createHisto( "testCut11", 45, 5.5, 5.8 );
    // createHisto( "testCut12", 45, 5.5, 5.8 );
    // createHisto( "testCut13", 45, 5.5, 5.8 );
    // createHisto( "testCut14", 45, 5.5, 5.8 );
    // createHisto( "testCut15", 45, 5.5, 5.8 );
    // createHisto( "testCut16", 45, 5.5, 5.8 );
    // createHisto( "testCut17", 45, 5.5, 5.8 );
    // createHisto( "testCut18", 45, 5.5, 5.8 );
    // createHisto( "testCut19", 45, 5.5, 5.8 );
}


void root_TreeHistoMain_LbTk::LoadSourceBranch()
{
    TTree* t = readTree();
    LoadFormatSourceBranch(t);
    return;
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
