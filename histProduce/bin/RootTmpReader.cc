#include <iostream>
#include <vector>
#include "histProduce/histProduce/interface/rooTHMainLbTk.h"
#include "histProduce/histProduce/interface/rooTHMainpnLbTk.h"
#include "histProduce/histProduce/interface/rooTHMainGenLbTk.h"
#include "histProduce/histProduce/interface/rooTHMainGenpnLbTk.h"

#include "TTree.h"
#include "TFile.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TGraph.h"
#include "TGaxis.h"
#include "TPaveText.h"

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

typedef root_TreeHistoMain_GenInfo_plusminus_LbTk readMC;
//typedef root_TreeHistoMain_LbTk readData;
typedef root_TreeHistoMain_plusminus_LbTk readData;

TH1D* CalcBkgPullDist( RooWorkspace& wspace, RooRealVar& var, RooExtendPdf& expdf, RooRealVar& nsig );
int main()
{
    std::vector<double> gaRes;
    //gaRes = { 0.63,-1.10,19.24,0.00,3.06,1.06,3.64,0.71,2.61,0.92,0.00,0.06,0.13,-0.47,-0.80,3.42,-2.84,1.94 };
    //gaRes = { -0.88,-1.10,16.91,0.00,4.68,-1.04,4.68,-0.06,1.85,-0.33,0.12,-1.38,-0.56,-0.26,-0.64,3.42,-2.71,5.69 };
    //gaRes = { 1.23,0.16,15.18,0.00,4.39,0.74,3.92,-0.12,1.98,-0.02,0.00,0.00,0.00,-1.09,-0.76,2.23,-2.41,3.01 };
    //gaRes = { -1.94,0.16,13.21,0.00,4.85,0.25,4.14,-1.04,2.32,-0.42,-0.26,0.66,0.00,1.51,-1.03,3.47,-2.89,3.81 };
    //gaRes = { 1.58,0.31,13.44,0.00,4.12,0.00,4.56,0.00,2.29,0.00,0.00,0.00,0.00,0.00,-0.53,0.75,-0.32,0.45 };
    //gaRes = {-0.08,-1.10,13.23,0.00,4.72,-0.63,5.59,0.00,2.34,0.37,0.00,0.45,0.37,0.49,-1.41,2.49,-1.11,2.30 };
    //gaRes = { -0.53,-0.32,13.31,0.16,4.23,-1.22,5.24,0.00,3.58,-0.51,-0.36,-0.59,0.22,-0.25,-2.41,3.48,-2.65,3.03 };
    //gaRes = { -0.24,-0.54,13.31,-0.98,1.96,2.00,5.42,0.30,3.48,0.00,-0.73,0.75,0.54,-0.25,-6.00,6.06,-5.47,5.30 }; 
    //gaRes = {1.21,0.37,18.81,0.00,3.47,0.00,3.66,0.00,2.89,0.18,0.00,0.00,0.00,0.00,-0.47,0.00,-0.65,0.00 };
    //gaRes = { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0, 0., 0., 0., 0., 0. };
    //gaRes = { 3.87,0.15,17.43,0.04,4.33,0.14,2.77,0.00,1.29,-0.13,0.14,0.08,-0.02,0.09,0.02,0.34,0.01,0.00 };
    gaRes = { 4.46,-0.25,15.87,-0.15,2.39,0.10,1.87,-0.17,1.02,-0.21,-0.15,-0.18,-0.04,-0.08,-0.03,-0.04,0.21,0.00 };
 

    TFile* mcFile = TFile::Open("storeroot/tReduced/tree_forGA_removeBsBdOnly/mc_LbToJPsipK_13TeV_noPU.root");
    readMC mc(nullptr);
    mc.SetInputFile( mcFile );

    TTree* mcTree = mc.readTree();

    // TH1D* h_MC   = new TH1D("h_signal", "#Lambda^{0}_{b} signal MC", 30, 5.55, 5.7);
    // TH1D* h_Data = new TH1D("h_data"  , "#Lambda^{0}_{b} data"     , 30, 5.55, 5.7);
    TH1D* h_MC   = new TH1D("h_signal", "#Lambda^{0}_{b} signal MC", 200, 5.4, 5.90);
    TH1D* h_Data = new TH1D("h_data"  , "#Lambda^{0}_{b} data"     , 200, 5.4, 5.90);
    TH1D* h_dFDSig= new TH1D("h_dFDSig" , "", 70, 3., 10.);
    TH1D* h_mFDSig= new TH1D("h_mFDSig" , "", 70, 3., 10.);

    TGaxis::SetMaxDigits(3);
    unsigned i = 0;
    unsigned N = mcTree->GetEntries();
    while ( i != N )
    {
        mcTree->GetEntry(i++);
        if ( mc.readD[readMC::plbtkMass             ] > 5.90 ) continue; 
        if ( mc.readD[readMC::plbtkMass             ] < 5.4  ) continue; 
        if ( mc.readD[readMC::plbtkFlightDistanceSig] < gaRes[ 0] ) continue;
        if ( mc.readD[readMC::plbtkVtxprob          ] < gaRes[ 1] ) continue;
        if ( mc.readD[readMC::plbtkPt               ] < gaRes[ 2] ) continue;
        if ( mc.readD[readMC::ptktkPt               ] < gaRes[ 4] ) continue;
        if ( mc.readD[readMC::pptonPt               ] < gaRes[ 6] ) continue;
        if ( mc.readD[readMC::pkaonPt               ] < gaRes[ 8] ) continue;
        h_MC->Fill(mc.readD[readMC::plbtkMass]);

        if ( mc.readD[readMC::plbtkFlightDistanceSig] < 3.0  ) continue;
        h_mFDSig->Fill(mc.readD[readMC::plbtkFlightDistanceSig]);
    }   
        

    TFile* dataFile = TFile::Open("storeroot/tReduced/tree_forGA_removeBsBdOnly/data_2016RunBCDEFGH.root");
    //TFile* dataFile = TFile::Open("storeroot/tReduced/tree_forGA_removeBsBdOnly/data_2017RunBCDEF.root");
    readData data(nullptr);
    data.SetInputFile( dataFile );
    TTree* dataTree = data.readTree();

    i = 0;
    N = dataTree->GetEntries();
    while ( i != N )
    {
        dataTree->GetEntry(i++);
        //if ( data.readD[readData::lbtkMass             ] > 5.90 ) continue; 
        //if ( data.readD[readData::lbtkMass             ] < 5.4  ) continue; 
          
        //if ( data.readD[readData::lbtkFlightDistanceSig] < gaRes[ 0] ) continue;
        //if ( data.readD[readData::lbtkVtxprob          ] < gaRes[ 1] ) continue;
        //if ( data.readD[readData::lbtkPt               ] < gaRes[ 2] ) continue;
        //if ( data.readD[readData::tktkPt               ] < gaRes[ 4] ) continue;
        //if ( data.readD[readData::ptkPt                ] < gaRes[ 6] ) continue;
        //if ( data.readD[readData::ntkPt                ] < gaRes[ 8] ) continue;
        //h_Data->Fill(data.readD[readData::lbtkMass]);
        //if ( data.readD[readData::lbtkFlightDistanceSig] < 3.0  ) continue;
        //h_dFDSig->Fill(data.readD[readData::lbtkFlightDistanceSig]);

        if ( data.readD[readData::plbtkMass             ] > 5.90 ) continue; 
        if ( data.readD[readData::plbtkMass             ] < 5.4  ) continue; 

        if ( data.readD[readData::plbtkFlightDistanceSig] < gaRes[ 0] ) continue;
        if ( data.readD[readData::plbtkVtxprob          ] < gaRes[ 1] ) continue;
        if ( data.readD[readData::plbtkPt               ] < gaRes[ 2] ) continue;
        if ( data.readD[readData::ptktkPt               ] < gaRes[ 4] ) continue;
        if ( data.readD[readData::pptonPt               ] < gaRes[ 6] ) continue;
        if ( data.readD[readData::pkaonPt               ] < gaRes[ 8] ) continue;
        h_Data->Fill(data.readD[readData::plbtkMass]);
        if ( data.readD[readData::plbtkFlightDistanceSig] < 3.0  ) continue;
        h_dFDSig->Fill(data.readD[readData::plbtkFlightDistanceSig]);
    }

    TCanvas* c1 = new TCanvas("c1","",1600,1000);
    c1->SaveAs("storefig/resultHisto.pdf[");
    h_mFDSig->Draw();
    c1->SaveAs("storefig/resultHisto.pdf");
    h_dFDSig->Draw();
    c1->SaveAs("storefig/resultHisto.pdf");

    RooRealVar varMass( "mass", "", 5.6, 5.4, 5.90 );
    RooDataHist binnedData("histData", "data histogram", RooArgList(varMass), h_Data);
    RooDataHist binnedMC("histMC", "MC histogram", RooArgList(varMass), h_MC);

    // start to fit MC
    RooRealVar par_mean( "mean", "parameter to gaussian: mean", 5.637, 5.55, 5.7 );
    RooRealVar par_width("width","parameter to gaussian: width",0.01, 0.000001, 10. );
    RooGaussian pdf_gaus("gaus", "PDF : gaussian", varMass, par_mean, par_width );
    RooRealVar par_mean2( "mean2", "parameter to gaussian: mean", 5.6142, 5.55, 5.7 );
    RooRealVar par_width2("width2","parameter to gaussian: width", 0.0342, 0.000001, 1. );
    RooGaussian pdf_gaus2("gaus2", "PDF : gaussian", varMass, par_mean2, par_width2 );
    RooRealVar frac( "frac", "fraction to gaussian & polynomial", 0.7, 0.001, 1.00 );
    RooAddPdf mcModel( "mcModel", "model to MC", RooArgList(pdf_gaus, pdf_gaus2), RooArgList(frac) );
    mcModel.fitTo(binnedMC);

    RooPlot* mcFrame = varMass.frame(RooFit::Title("signal MC : #Lambda^{0}_{b} #rightarrow J/#Psi p K"));
    mcFrame->GetXaxis()->SetTitle("GeV");
    binnedMC.plotOn(mcFrame,RooFit::Name("mHist"));
    mcModel.plotOn(mcFrame, RooFit::Name("mTot")  );
    mcModel.plotOn(mcFrame, RooFit::Name("mGaus1"), RooFit::Components(pdf_gaus ), RooFit::LineStyle(7), RooFit::LineColor(30));
    mcModel.plotOn(mcFrame, RooFit::Name("mGaus2"), RooFit::Components(pdf_gaus2), RooFit::LineStyle(7), RooFit::LineColor(40));
    double mcChi2 = mcFrame->chiSquare("mGaus1", "mHist");

    TGraph* tg_mHist  = (TGraph*)mcFrame->findObject("mHist");
    TGraph* tg_mTot   = (TGraph*)mcFrame->findObject("mTot");
    TGraph* tg_mGaus1 = (TGraph*)mcFrame->findObject("mGaus1");
    TGraph* tg_mGaus2 = (TGraph*)mcFrame->findObject("mGaus2");

    TLegend* legMC = new TLegend( 0.6, 0.6, 0.85, 0.85 );
    legMC->SetLineColor(0);
    legMC->SetFillColor(4000);
    legMC->SetFillStyle(4000);
    legMC->AddEntry(tg_mHist, "virtual data", "P");
    legMC->AddEntry(tg_mTot,  "total PDF", "LP");
    legMC->AddEntry(tg_mGaus1,"PDF: gaus1","LP");
    legMC->AddEntry(tg_mGaus2,"PDF: gaus2","LP");
    mcFrame->Draw();
    legMC->Draw("same");
    c1->SaveAs("storefig/h_fittingResult.MC.eps");
    c1->SaveAs("storefig/resultHisto.pdf");

    // keep the MC fitting result.
    par_mean.setConstant(true);
    par_width.setConstant(true);
    par_mean2.setConstant(true);
    par_width2.setConstant(true);
    frac.setConstant(true);

    // start to fit Data
    //RooRealVar par_c1("c1", "parameter to polynomial : 1st order", -0.02, -100., 100. );
    //RooRealVar par_c2("c2", "parameter to polynomial : 2nd order", -0.02, -100., 100. );
    //RooPolynomial pdf_poly("poly", "PDF : polynomial", varMass, RooArgSet(par_c1, par_c2) );
    RooRealVar par_ch1("ch1", "parameter to chebychev : 1st order",  0.5, -10., 10. );
    RooRealVar par_ch2("ch2", "parameter to chebychev : 2nd order", -0.2, -10., 10. );
    RooRealVar par_ch3("ch3", "parameter to chebychev : 3rd order", -0.2, -10., 10. );
    //RooChebychev pdf_cheb("cheb", "PDF : chebychev", varMass, RooArgSet(par_ch1, par_ch2) );
    RooChebychev pdf_cheb("cheb", "PDF : chebychev", varMass, RooArgSet(par_ch1, par_ch2, par_ch3) );
    RooRealVar N_cheb("N_cheb", "extend number : chebchev", (double)h_Data->GetEntries(), 0., 2.*(double)h_Data->GetEntries());
    RooExtendPdf extend_pdf_cheb("ext_cheb", "PDF: chebychev", pdf_cheb, N_cheb);

    double constr_mean  = par_mean .getVal();
    double constr_width = par_width.getVal();
    varMass.setRange("sR1", 5.4, constr_mean - 5.*constr_width);
    varMass.setRange("sR2", constr_mean + 5.*constr_width, 5.90);
    varMass.setRange("tot", 5.4, 5.90);
    varMass.setRange("signalRegion", constr_mean - 3.*constr_width, constr_mean + 3.*constr_width);
    //pdf_cheb.fitTo(binnedData);
    //pdf_cheb.fitTo(binnedData, RooFit::Range("sR1,sR2"), RooFit::Minos(kTRUE));
    extend_pdf_cheb.fitTo(binnedData, RooFit::Minos(kTRUE));
    RooPlot* tmpFrame = varMass.frame(RooFit::Title("d"));
    binnedData.plotOn(tmpFrame, RooFit::Name("a"));
    pdf_cheb.plotOn( tmpFrame , RooFit::Name("b"));
    double tmpChi2 = tmpFrame->chiSquare("b", "a");
    tmpFrame->Draw();
    c1->SaveAs("storefig/resultHisto.pdf");
    c1->SaveAs("storefig/h_fittingResult.intermediate.eps");


    double maxevent = (double)h_Data->GetEntries();
    RooRealVar ns( "nSig", "number of signal", maxevent/1000., 0., maxevent );
    RooRealVar nb( "nBkg", "number of background", maxevent, 0., maxevent);
    RooAddPdf totModel( "totModel", "PDF : data + MC", RooArgList(mcModel, pdf_cheb), RooArgList(ns,nb));
    totModel.fitTo(binnedData);

    RooPlot* dataFrame = varMass.frame(RooFit::Title("data : #Lambda^{0}_{b} #rightarrow J/#Psi p K"));
    dataFrame->GetXaxis()->SetTitle("GeV");

    binnedData.plotOn(dataFrame,RooFit::Name("dHist"));
    totModel.plotOn(dataFrame, RooFit::Name("dTot"));
    totModel.plotOn(dataFrame, RooFit::Name("dPart1"), RooFit::Components(mcModel), RooFit::LineStyle(7), RooFit::LineColor(2));
    totModel.plotOn(dataFrame, RooFit::Name("dPart2"), RooFit::Components(pdf_cheb),RooFit::LineStyle(7), RooFit::LineColor(40));
    double dataChi2 = dataFrame->chiSquare("dPart2", "dHist");


    TGraph* tg_dHist = (TGraph*)dataFrame->findObject("dHist");
    TGraph* tg_dTot  = (TGraph*)dataFrame->findObject("dTot");
    TGraph* tg_dPart1= (TGraph*)dataFrame->findObject("dPart1");
    TGraph* tg_dPart2= (TGraph*)dataFrame->findObject("dPart2");
    // TGraph* legEntry_dTot   = (TGraph*)tg_dTot  ->Clone();
    // TGraph* legEntry_dPart1 = (TGraph*)tg_dPart1->Clone();
    // TGraph* legEntry_dPart2 = (TGraph*)tg_dPart2->Clone();
    // legEntry_dTot  ->SetLineWidth(10);
    // legEntry_dPart1->SetLineWidth(10);
    // legEntry_dPart2->SetLineWidth(10);

    TLegend* legData = new TLegend( 0.6, 0.60, 0.85, 0.85 );
    legData->SetLineColor(0);
    legData->SetFillColor(4000);
    legData->SetFillStyle(4000);
    legData->AddEntry(tg_dHist, "data", "P");
    legData->AddEntry(tg_dTot  ,"total fitting result", "LP");
    legData->AddEntry(tg_dPart1,"PDF: signal MC", "LP");
    legData->AddEntry(tg_dPart2,"PDF: polynomial","LP");

    TPaveText* txt =  new TPaveText(0.11, 0.15, 0.4, 0.50, "NDC");
    char tmpName[256];
    sprintf( tmpName, "#frac{#chi^{2}}{nDoF} = %.2e", dataChi2 );
    txt->AddText( tmpName );
    txt->AddText( ""      );
    sprintf( tmpName, "# of Sig: %.2e", ns.getVal() );
    txt->AddText( tmpName );
    sprintf( tmpName, "# of Bkg: %.2e", nb.getVal() );
    txt->AddText( tmpName );

    txt->SetFillColor(4000);
    txt->SetFillStyle(4000);
    //legData->AddEntry(legEntry_dPart1,"PDF: signal MC", "LP");
    //legData->AddEntry(legEntry_dPart2,"PDF: polynomial","LP");
    dataFrame->Draw();
    legData->Draw("same");
    txt->Draw("same");
    //legData->AddEntry(legEntry_dTot  ,  "total fitting result", "LP");
    c1->SaveAs("storefig/h_fittingResult.Data.eps");
    c1->SaveAs("storefig/h_fittingResult.Data.png");
    c1->SaveAs("storefig/resultHisto.pdf");

    //varMass.setRange("signalRegion", 5.58, 5.64);
    //RooAbsReal* mcIntegral   = pdf_gaus.createIntegral( varMass, RooFit::NormSet(varMass), RooFit::Range("signalRegion") );
    RooAbsReal* mcIntegral   = mcModel.createIntegral( varMass, RooFit::NormSet(varMass), RooFit::Range("signalRegion") );
    RooAbsReal* dataIntegral = pdf_cheb.createIntegral( varMass, RooFit::NormSet(varMass), RooFit::Range("signalRegion") );
    std::cout << "number of signal : " << ns.getVal() << ", in signal Region: " << ns.getVal()*mcIntegral->getVal() << std::endl;
    std::cout << "number of bkg    : " << nb.getVal() << ", in signal Region: " << nb.getVal()*dataIntegral->getVal() << std::endl;
    std::cout << "significance: " << (ns.getVal()*mcIntegral->getVal())/sqrt(nb.getVal()*dataIntegral->getVal()) << std::endl;
    std::cout << "mc chi2/nDoF : " << mcChi2 << ", data chi2/nDoF : " << dataChi2 << std::endl;
    std::cout << "intermediate chi2/nDoF = " << tmpChi2 << std::endl;
    c1->SaveAs("storefig/resultHisto.pdf]");
    std::cout << "signal mean = " << par_mean.getVal() << ", width = " << par_width.getVal() << std::endl;
    delete txt;
    delete c1;
    delete h_Data;
    delete h_MC;
    mcFile->Close();
    dataFile->Close();
}
