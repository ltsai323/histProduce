#include <iostream>
#include <vector>

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

#include "histProduce/histProduce/interface/rooTHMainLbTk.h"
typedef root_TreeHistoMain_LbTk readData;

TH1D* CalcBkgPullDist( const char* hName, RooWorkspace& wspace, RooRealVar& var, RooExtendPdf& expdf, RooRealVar& nsig );
int main()
{
    TFile* inFile = TFile::Open("storeroot/reducedTHOutput.root");
    readData data(nullptr);
    data.SetInputFile( inFile );
    data.LoadSourceBranch();
    TTree* dataTree = data.readTree();

    TH1D* h_Data = new TH1D("h_Data", "#Lambda^{0}_{b} mass with GA cut", 300, 5.4, 5.95);
    for ( int i=0; i<dataTree->GetEntries(); ++i )
    { 
        dataTree->GetEntry(i);
        if ( data.readD[readData::lbtkMass]>5.4&&data.readD[readData::lbtkMass]<5.95 )
            h_Data->Fill(data.readD[readData::lbtkMass]);
    }

    RooRealVar varMass( "mass", "", 5.6, 5.4, 5.95 );
    RooDataHist binnedData("histData", "data histogram", RooArgList(varMass), h_Data);
    // start to fit Data
    TH1D* hPullRes[10];
    int hIdx = 0;
// 
//     // 4th chebychev pulldistribution {{{
//     {
//     char histName[] = "pullDist_4thChebshev";
//     //RooRealVar par_c1("c1", "parameter to polynomial : 1st order", -0.02, -100., 100. );
//     //RooRealVar par_c2("c2", "parameter to polynomial : 2nd order", -0.02, -100., 100. );
//     //RooPolynomial pdf_poly("poly", "PDF : polynomial", varMass, RooArgSet(par_c1, par_c2) );
//     RooRealVar par_ch1("ch1", "parameter to chebychev : 1st order",  0.5, -10., 10. );
//     RooRealVar par_ch2("ch2", "parameter to chebychev : 2nd order", -0.2, -10., 10. );
//     RooRealVar par_ch3("ch3", "parameter to chebychev : 3rd order", -0.2, -10., 10. );
//     RooRealVar par_ch4("ch4", "parameter to chebychev : 4th order", -0.2, -10., 10. );
//     //RooChebychev pdf_cheb("cheb", "PDF : chebychev", varMass, RooArgSet(par_ch1, par_ch2) );
//     RooChebychev pdf_cheb("cheb", "PDF : chebychev", varMass, RooArgSet(par_ch1, par_ch2, par_ch3, par_ch4) );
//     RooRealVar N_cheb("N_cheb", "extend number : chebchev", (double)h_Data->GetEntries(), 0., 2.*(double)h_Data->GetEntries());
//     RooExtendPdf extend_pdf_cheb("ext_cheb", "PDF: chebychev", pdf_cheb, N_cheb);
//     extend_pdf_cheb.fitTo(binnedData, RooFit::Minos(kTRUE));
// 
//     // calculate pull distribution
//     RooArgSet* params = (RooArgSet*) extend_pdf_cheb.getParameters(varMass);
//     RooWorkspace* tmpWorkspace = new RooWorkspace("tmpWorkspace");
//     tmpWorkspace->import(extend_pdf_cheb);
//     tmpWorkspace->defineSet("parameters", *params);
//     tmpWorkspace->defineSet("observable", varMass);
//     tmpWorkspace->saveSnapshot("reference_fit", *params, kTRUE);
//     hPullRes[hIdx++] = CalcBkgPullDist( histName, *tmpWorkspace, varMass, extend_pdf_cheb, N_cheb );
//     delete tmpWorkspace;
//     }
//     // end of 4th chebychev distribution }}}
//     // 3rd chebychev pulldistribution {{{
//     {
//     char histName[] = "pullDist_3rdChebshev";
//     //RooRealVar par_c1("c1", "parameter to polynomial : 1st order", -0.02, -100., 100. );
//     //RooRealVar par_c2("c2", "parameter to polynomial : 2nd order", -0.02, -100., 100. );
//     //RooPolynomial pdf_poly("poly", "PDF : polynomial", varMass, RooArgSet(par_c1, par_c2) );
//     RooRealVar par_ch1("ch1", "parameter to chebychev : 1st order",  0.5, -10., 10. );
//     RooRealVar par_ch2("ch2", "parameter to chebychev : 2nd order", -0.2, -10., 10. );
//     RooRealVar par_ch3("ch3", "parameter to chebychev : 3rd order", -0.2, -10., 10. );
//     RooChebychev pdf_cheb("cheb", "PDF : chebychev", varMass, RooArgSet(par_ch1, par_ch2, par_ch3) );
//     RooRealVar N_cheb("N_cheb", "extend number : chebchev", (double)h_Data->GetEntries(), 0., 2.*(double)h_Data->GetEntries());
//     RooExtendPdf extend_pdf_cheb("ext_cheb", "PDF: chebychev", pdf_cheb, N_cheb);
//     extend_pdf_cheb.fitTo(binnedData, RooFit::Minos(kTRUE));
// 
//     // calculate pull distribution
//     RooArgSet* params = (RooArgSet*) extend_pdf_cheb.getParameters(varMass);
//     RooWorkspace* tmpWorkspace = new RooWorkspace("tmpWorkspace");
//     tmpWorkspace->import(extend_pdf_cheb);
//     tmpWorkspace->defineSet("parameters", *params);
//     tmpWorkspace->defineSet("observable", varMass);
//     tmpWorkspace->saveSnapshot("reference_fit", *params, kTRUE);
//     hPullRes[hIdx++] = CalcBkgPullDist( histName, *tmpWorkspace, varMass, extend_pdf_cheb, N_cheb );
//     delete tmpWorkspace;
//     }
//     // end of 3rd chebychev distribution }}}
    // 2nd chebychev pulldistribution {{{
    {
    char histName[] = "pullDist_2ndChebshev";
    RooRealVar par_ch1("ch1", "parameter to chebychev : 1st order",  0.5, -10., 10. );
    RooRealVar par_ch2("ch2", "parameter to chebychev : 2nd order", -0.2, -10., 10. );
    RooChebychev pdf_cheb("cheb", "PDF : chebychev", varMass, RooArgSet(par_ch1, par_ch2) );
    RooRealVar N_cheb("N_cheb", "extend number : chebchev", (double)h_Data->GetEntries(), 0., 2.*(double)h_Data->GetEntries());
    RooExtendPdf extend_pdf_cheb("ext_cheb", "PDF: chebychev", pdf_cheb, N_cheb);
    extend_pdf_cheb.fitTo(binnedData, RooFit::Minos(kTRUE));

    // calculate pull distribution
    RooArgSet* params = (RooArgSet*) extend_pdf_cheb.getParameters(varMass);
    RooWorkspace* tmpWorkspace = new RooWorkspace("tmpWorkspace");
    tmpWorkspace->import(extend_pdf_cheb);
    tmpWorkspace->defineSet("parameters", *params);
    tmpWorkspace->defineSet("observable", varMass);
    tmpWorkspace->saveSnapshot("reference_fit", *params, kTRUE);
    hPullRes[hIdx++] = CalcBkgPullDist( histName, *tmpWorkspace, varMass, extend_pdf_cheb, N_cheb );
    delete tmpWorkspace;
    }
    // end of 2nd chebychev distribution }}}
//     // 2nd pulynomial pulldistribution {{{
//     {
//     char histName[] = "pullDist_4thChebshev";
//     RooRealVar par_c1("c1", "parameter to polynomial : 1st order", -0.02, -100., 100. );
//     RooRealVar par_c2("c2", "parameter to polynomial : 2nd order", -0.02, -100., 100. );
//     RooPolynomial pdf_poly("poly", "PDF : polynomial", varMass, RooArgSet(par_c1, par_c2) );
//     RooRealVar N_poly("N_poly", "extend number : chebchev", (double)h_Data->GetEntries(), 0., 2.*(double)h_Data->GetEntries());
//     RooExtendPdf extend_pdf_poly("ext_poly", "PDF: polynomial", pdf_poly, N_poly);
//     extend_pdf_poly.fitTo(binnedData, RooFit::Minos(kTRUE));
// 
//     // calculate pull distribution
//     RooArgSet* params = (RooArgSet*) extend_pdf_poly.getParameters(varMass);
//     RooWorkspace* tmpWorkspace = new RooWorkspace("tmpWorkspace");
//     tmpWorkspace->import(extend_pdf_poly);
//     tmpWorkspace->defineSet("parameters", *params);
//     tmpWorkspace->defineSet("observable", varMass);
//     tmpWorkspace->saveSnapshot("reference_fit", *params, kTRUE);
//     hPullRes[hIdx++] = CalcBkgPullDist( histName, *tmpWorkspace, varMass, extend_pdf_poly, N_poly );
//     delete tmpWorkspace;
//     }
//     // end of 2nd polynomial pulldistribution }}}
// 
    
    // fit the pull distribution
    RooRealVar var_pull("var_pull", "variable to pull distribution", 0., -10., 10.);
    RooDataHist pullDist("pullDist", "result to pull distribution", RooArgSet(var_pull), hPullRes[0]);
    RooRealVar par_mean_pullDist( "par_mean_pullDist", "parameter to gaussian in pull distribution: mean", 0.0001, -2., 2. );
    RooRealVar par_width_pullDist("par_width_pullDist","parameter to gaussian in pull distribution: width",1.0000,  0., 5. );
    RooGaussian pdf_gaus_pullDist("gaus_pullDist", "PDF in pull distribution: gaus", var_pull, par_mean_pullDist, par_width_pullDist);
    pdf_gaus_pullDist.fitTo(pullDist, RooFit::Minos(kTRUE));


    TCanvas* c1 = new TCanvas("c1", "", 1600, 1000);
    RooPlot* pullFrame = var_pull.frame();
    pullDist.plotOn(pullFrame);
    pdf_gaus_pullDist.plotOn(pullFrame);
    pullFrame->Draw();
    c1->SaveAs("storefig/h_pullDistributionToBkgFitting.Data.eps");
    c1->SaveAs("storefig/resultTopullDistributionToBkgFitting.Data.pdf");

    // record histograms into TFile {{{
    TFile* histRecordFile = new TFile("storeroot/resultToPullDistributions_diffPars.root", "RECREATE");
    histRecordFile->cd();
    for ( int i=0;i<hIdx;++i )
    {
        const char* _histName = hPullRes[i]->GetTitle();
        hPullRes[i]->Write( _histName );
    }
    histRecordFile->Close();
    delete histRecordFile;
    // record histograms into TFile end }}}

    for ( int i=0; i<hIdx;++i )
        delete hPullRes[i];

    delete c1;
}

TH1D* CalcBkgPullDist( const char* hName, RooWorkspace& wspace, RooRealVar& var, RooExtendPdf& expdf, RooRealVar& nsig )
{
    // need to delete output RooDataHist!
    double num_target = nsig.getVal();
    TH1D* biasRes = new TH1D(hName, hName,120, -10., 10.);
    
    TCanvas* cc = new TCanvas("cc", "", 1600, 1000);
    cc->SaveAs("storefig/pullDist_interRes.pdf[");
    unsigned i = 0;
    unsigned N = 2000;
    while ( i++ != N )
    {
        wspace.loadSnapshot("reference_fit");

        int num_genEvent = RooRandom::randomGenerator()->Poisson(num_target);
        RooDataSet* toyData = expdf.generate( var, num_genEvent );

        expdf.fitTo(*toyData, RooFit::Minos(true));
        RooPlot* frame = var.frame();
        toyData->plotOn(frame);
        expdf.plotOn(frame);
        frame->Draw();
        cc->SaveAs("storefig/pullDist_interRes.pdf");

        double num_toyMCfitRes = nsig.getVal();
        double err_toyMCfitRes = nsig.getError();
        double result = (num_toyMCfitRes - num_target)/err_toyMCfitRes;
        biasRes->Fill(result);
    }
    
    // recover to original state
    wspace.loadSnapshot("reference_fit");
    cc->SaveAs("storefig/pullDist_interRes.pdf]");
    delete cc;
    return biasRes;
}
