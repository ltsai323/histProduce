#include "histProduce/CMSSWTreeMgr/interface/rooTHMainSampler.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "math.h"
#include "TCanvas.h"
#include "TPaveText.h"
#include "TFile.h"
#include "TH1D.h"
#include "TLine.h"


//root_TreeHistoMain_Sampler::root_TreeHistoMain_Sampler( TFile* d ) :
root_TreeHistoMain_Sampler::root_TreeHistoMain_Sampler( TFileDirectory* d ) :
    root_TreeHistoMain( d, "Sampler" )
{
    setInputTreeName( "lbSpecificDecay/LbTkGenInfo" );


    if ( !d ) return;
    // RegTree();
    //RegHisto();
    return;
}

void root_TreeHistoMain_Sampler::Process( unsigned int pIdx )
{
    // Clear();
    readTree()->GetEntry( pIdx );
    return;
}

void root_TreeHistoMain_Sampler::RegTree()
{
    if ( NoOutput() ) return;

    TTree* t = thisTree();
    //RegFormatTree(t);
    return;
}

void root_TreeHistoMain_Sampler::RegHisto()
{
    if ( NoOutput() ) return;
    createHisto( "mass_LbTk", 45, 5.5, 5.8 );
}


void root_TreeHistoMain_Sampler::LoadSourceBranch()
{
    TTree* t = readTree();
    //LoadFormatSourceBranch(t);

    return;
}

void root_TreeHistoMain_Sampler::SummaryCalc()
{
//    if ( NoOutput() ) return;
//    const std::string preName = getPreName();
//    char fName[48] ; sprintf( fName , "h_sum_%s.pdf" , preName.c_str() );
//    char fNameI[48]; sprintf( fNameI, "h_sum_%s.pdf[", preName.c_str() );
//    char fNameF[48]; sprintf( fNameF, "h_sum_%s.pdf]", preName.c_str() );
//
//
//    TCanvas* c1 = new TCanvas( "c1", "", 1000, 1000 );
//    c1->SaveAs( fNameI );
//
//    const std::map<std::string,TH1D*>& myMap = getHistos();
//
//    std::map<std::string,TH1D*>::const_iterator iter;
//    std::map<std::string,TH1D*>::const_iterator iend = myMap.cend  ();
//    for ( int i=0; i<20; ++i )
//    {
//        char hName[20];
//        sprintf( hName, "testCut%02d", i );
//        const std::string hname = hName;
//        iter = myMap.find( hname );
//        if ( iter == iend ) continue;
//
//        TH1D* histo = iter->second;
//        c1->Clear();
//        histo->Draw();
//        c1->Update();
//
//
//        std::pair<double, double> signalBand( 5.60, 5.64 );
//        usefulFuncs::SNRes sigNumRes = usefulFuncs::SignalNumberCalculator( histo, signalBand.first, signalBand.second );
//
//        // print summary text {{{
//        TPaveText* txt = new TPaveText( 0.6, 0.5, 0.89, 0.75, "NDC" );
//        char myText[26] = {'\0'};
//
//        sprintf( myText, "%s: %.5f", testingCut, cutInit + cutInterval * double(i) );
//        txt->AddText(myText);
//        sprintf( myText, "# of signal: %d +- %.3f", sigNumRes.Nsig, sigNumRes.ErrSig );
//        txt->AddText(myText);
//        sprintf( myText,"S/(sqrt(S+B) = %.2f", double(sigNumRes.Nsig) / sqrt(double(sigNumRes.N)) );
//        txt->AddText(myText);
//        txt->Draw("same");
//        // print summary text end }}}
//
//        // print signal band line {{{
//        TLine* lfirst = new TLine( signalBand.first , c1->GetUymin(), signalBand.first , c1->GetUymax() );
//        TLine* lsecond= new TLine( signalBand.second, c1->GetUymin(), signalBand.second, c1->GetUymax() );
//        lfirst ->SetLineWidth(1); lfirst ->SetLineColor(1); lfirst ->SetLineStyle(2);
//        lsecond->SetLineWidth(1); lsecond->SetLineColor(1); lsecond->SetLineStyle(2);
//        lfirst->Draw("same"); lsecond->Draw("same");
//        // print signal band line end }}}
//
//        c1->SaveAs( fName );
//        delete txt;
//        delete lfirst; delete lsecond;
//    }
//    c1->SaveAs( fNameF );

}
