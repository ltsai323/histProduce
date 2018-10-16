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
#include "TNtupleD.h"

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
#include "RooExponential.h"

typedef root_TreeHistoMain_GenInfo_plusminus_LbTk readMC;
//typedef root_TreeHistoMain_LbTk readData;
typedef root_TreeHistoMain_plusminus_LbTk readData;
const bool ptonBkaonSwitch = true;

TH1D* CalcBkgPullDist( RooWorkspace& wspace, RooRealVar& var, RooExtendPdf& expdf, RooRealVar& nsig );
int main()
{
    std::vector<double> gaRes;
    //gaRes = { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0, 0., 0., 0., 0., 0. };
    //gaRes = { 3.87,0.15,17.43,0.04,4.33,0.14,2.77,0.00,1.29,-0.13,0.14,0.08,-0.02,0.09,0.02,0.34,0.01,0.00 };
    gaRes = { 4.46,-0.25,15.87,-0.15,2.39,0.10,1.87,-0.17,1.02,-0.21,-0.15,-0.18,-0.04,-0.08,-0.03,-0.04,0.21,0.00 };
    TFile* tmpFile = new TFile("/tmp/ltsai/tmpfile.root", "RECREATE");
 

    TFile* mcFile = TFile::Open("storeroot/tReduced/tree_forGA_removeBsBdOnly/mc_LbToJPsipK_13TeV_noPU.root");
    readMC mc(nullptr);
    mc.SetInputFile( mcFile );

    TTree* mcTree = mc.readTree();

    TNtupleD* ntMC =  new TNtupleD("mcNtuple", "ntuple to MC", "pMass:pJpsip:nMass:nJpsip");
    ntMC->SetDirectory(tmpFile);

    TGaxis::SetMaxDigits(3);
    unsigned i = 0;
    unsigned N = mcTree->GetEntries();
    while ( i != N )
    {
        mcTree->GetEntry(i++);
        if ( mc.readD[readMC::plbtkMass             ] > 5.90 &&
             mc.readD[readMC::nlbtkMass             ] > 5.90 ) continue;
        if ( mc.readD[readMC::plbtkMass             ] < 5.4  &&
             mc.readD[readMC::nlbtkMass             ] < 5.4  ) continue; 
        if ( mc.readD[readMC::ptargetJpsiP_mass     ] > 4.90 &&
             mc.readD[readMC::ntargetJpsiP_mass     ] > 4.90 ) continue; 

        // kill bd
        if ( mc.readD[readMC::pfake_BdMass      ]>5.2&&mc.readD[readMC::pfake_BdMass    ]<5.35)
            continue;
        // kill bd->Kstar(892)
        if ( mc.readD[readMC::pfake_BdMass      ]>5.2&&mc.readD[readMC::pfake_BdMass    ]<5.35&&
             mc.readD[readMC::pfake_KstarMass   ]>0.7&&mc.readD[readMC::pfake_KstarMass ]<1.1 )
            continue;
        // kill bd->Kstar(1432)
        if ( mc.readD[readMC::pfake_BdMass      ]>5.2&&mc.readD[readMC::pfake_BdMass    ]<5.35&&
             mc.readD[readMC::pfake_KstarMass   ]>1.3&&mc.readD[readMC::pfake_KstarMass ]<1.5 )
            continue;
        // kill bs->phi(1020)
        if ( mc.readD[readMC::pfake_BsMass      ]>5.3&&mc.readD[readMC::pfake_BdMass    ]<5.45&&
             mc.readD[readMC::pfake_KstarMass   ]>1.0&&mc.readD[readMC::pfake_KstarMass ]<1.05)
            continue;
        //  if ( mc.readD[readMC::plbtkFlightDistanceSig] < gaRes[ 0] ) continue;
        //  if ( mc.readD[readMC::plbtkVtxprob          ] < gaRes[ 1] ) continue;
        //  if ( mc.readD[readMC::plbtkPt               ] < gaRes[ 2] ) continue;
        //  if ( mc.readD[readMC::ptktkPt               ] < gaRes[ 4] ) continue;
        //  if ( mc.readD[readMC::pptonPt               ] < gaRes[ 6] ) continue;
        //  if ( mc.readD[readMC::pkaonPt               ] < gaRes[ 8] ) continue;
        ntMC->Fill(
                mc.readD[readMC::plbtkMass],
                mc.readD[readMC::ptargetJpsiP_mass],
                mc.readD[readMC::nlbtkMass],
                mc.readD[readMC::ntargetJpsiP_mass]
                );
    }   
        

    //TFile* dataFile = TFile::Open("storeroot/tReduced/tree_forGA_removeBsBdOnly/data_2016RunBCDEFGH.root");
    TFile* dataFile = TFile::Open("storeroot/tReduced/tree_forGA_removeBsBdOnly/data_2017RunBCDEF.root");
    readData data(nullptr);
    data.SetInputFile( dataFile );
    TTree* dataTree = data.readTree();
    //TNtupleD* ntdata =  new TNtupleD("dataNtuple", "ntuple to data", "pMass:pJpsip:pFD2D:pFD2DSig:plbtkPt:ptktkPt:pptonPt:pkaonPt:nMass:nJpsip:nFD2D:nFD2DSig:nlbtkPt:ntktkPt:nptonPt:nkaonPt");
    TNtupleD* ntdata =  new TNtupleD("dataNtuple", "ntuple to data", "pMass:pJpsip:pFD2D:pFD2DSig:plbtkPt:pptonPt:pkaonPt:nMass:nJpsip:nFD2D:nFD2DSig:nlbtkPt:nptonPt:nkaonPt");
    ntdata->SetDirectory(tmpFile);

    i = 0;
    N = dataTree->GetEntries();
    while ( i != N )
    {
        dataTree->GetEntry(i++);

        // cut data range
        if ( data.readD[readData::plbtkMass             ] > 5.90 &&
             data.readD[readData::nlbtkMass             ] > 5.90 ) continue; 
        if ( data.readD[readData::plbtkMass             ] < 5.4  &&
             data.readD[readData::nlbtkMass             ] < 5.4  ) continue; 
        if ( data.readD[readData::ptargetJpsiP_mass     ] > 4.9  &&
             data.readD[readData::ntargetJpsiP_mass     ] > 4.9  ) continue; 

        // kill bd
        if ( data.readD[readData::pfake_BdMass      ]>5.2&&data.readD[readData::pfake_BdMass    ]<5.35)
            continue;
        if ( data.readD[readData::nfake_BdMass      ]>5.2&&data.readD[readData::nfake_BdMass    ]<5.35)
            continue;

        // kill bd->Kstar(892)
        if ( data.readD[readData::pfake_BdMass      ]>5.2&&data.readD[readData::pfake_BdMass    ]<5.35&&
             data.readD[readData::pfake_KstarMass   ]>0.7&&data.readD[readData::pfake_KstarMass ]<1.1 )
            continue;
        if ( data.readD[readData::nfake_BdMass      ]>5.2&&data.readD[readData::nfake_BdMass    ]<5.35&&
             data.readD[readData::nfake_KstarMass   ]>0.7&&data.readD[readData::nfake_KstarMass ]<1.1 )
            continue;

        // kill bd->Kstar(1432)
        if ( data.readD[readData::pfake_BdMass      ]>5.2&&data.readD[readData::pfake_BdMass    ]<5.35&&
             data.readD[readData::pfake_KstarMass   ]>1.3&&data.readD[readData::pfake_KstarMass ]<1.5 )
            continue;
        if ( data.readD[readData::nfake_BdMass      ]>5.2&&data.readD[readData::nfake_BdMass    ]<5.35&&
             data.readD[readData::nfake_KstarMass   ]>1.3&&data.readD[readData::nfake_KstarMass ]<1.5 )
            continue;

        // kill bs->phi(1020)
        if ( data.readD[readData::pfake_BsMass      ]>5.3&&data.readD[readData::pfake_BdMass    ]<5.45&&
             data.readD[readData::pfake_KstarMass   ]>1.0&&data.readD[readData::pfake_KstarMass ]<1.05)
            continue;
        if ( data.readD[readData::nfake_BsMass      ]>5.3&&data.readD[readData::nfake_BdMass    ]<5.45&&
             data.readD[readData::nfake_KstarMass   ]>1.0&&data.readD[readData::nfake_KstarMass ]<1.05)
            continue;

        ntdata->Fill(
                data.readD[readData::plbtkMass],
                data.readD[readData::ptargetJpsiP_mass],
                data.readD[readData::plbtkFlightDistance2d],
                data.readD[readData::plbtkFlightDistanceSig],
                data.readD[readData::plbtkPt],
                //data.readD[readData::ptktkPt],
                data.readD[readData::pptonPt],
                data.readD[readData::pkaonPt],
                data.readD[readData::nlbtkMass],
                data.readD[readData::ntargetJpsiP_mass],
                data.readD[readData::nlbtkFlightDistance2d],
                data.readD[readData::nlbtkFlightDistanceSig],
                data.readD[readData::nlbtkPt],
                //data.readD[readData::ntktkPt],
                data.readD[readData::nptonPt],
                data.readD[readData::nkaonPt]
                );
    }

    TCanvas* c1 = new TCanvas("c1","",1600,1000);
    c1->SaveAs("storefig/resultHisto.pdf[");
    ntdata->Draw("pMass");
    c1->SaveAs("storefig/resultHisto.pdf");


    // roofit vars{{{
    RooRealVar plbMass( "pMass", "pMass", 5.6, 5.45, 5.75, "GeV" );
    RooRealVar nlbMass( "nMass", "nMass", 5.6, 5.45, 5.75, "GeV" );
    RooRealVar ppQMass( "pJpsip", "pJPsipMass", 4.4, 4.0, 4.9, "GeV" );
    RooRealVar npQMass( "nJpsip", "nJPsipMass", 4.4, 4.0, 4.9, "GeV" );
    RooRealVar fd2d   ("pFD2D", "pFD2D", 0., 2.);
    RooDataSet fitMC  ("unbinData", "unbin data", ntMC,   RooArgSet(plbMass,nlbMass,ppQMass,npQMass));
    RooDataSet fitData("unbinData", "unbin data", ntdata, RooArgSet(plbMass,nlbMass,ppQMass,npQMass,fd2d));

    // start to fit MC
    RooRealVar par_mean( "mean", "parameter to gaussian: mean", 5.637, 5.55, 5.7 );
    RooRealVar par_width("width","parameter to gaussian: width",0.01, 0.000001, 10. );
    RooGaussian pdf_gaus("gaus", "PDF : gaussian", plbMass, par_mean, par_width );
    RooRealVar par_mean2( "mean2", "parameter to gaussian: mean", 5.6142, 5.55, 5.7 );
    RooRealVar par_width2("width2","parameter to gaussian: width", 0.0342, 0.000001, 1. );
    RooGaussian pdf_gaus2("gaus2", "PDF : gaussian", plbMass, par_mean2, par_width2 );
    RooRealVar frac( "frac", "fraction to gaussian & polynomial", 0.7, 0.001, 1.00 );
    RooAddPdf mcModel( "mcModel", "model to MC", RooArgList(pdf_gaus, pdf_gaus2), RooArgList(frac) );
    mcModel.fitTo(fitMC);
    par_mean  .setConstant(true);
    par_mean2 .setConstant(true);
    par_width .setConstant(true);
    par_width2.setConstant(true);
    frac.setConstant(true);

    // plot mc fitting result
    RooPlot* mcFrame = plbMass.frame(RooFit::Title("signal MC : #Lambda^{0}_{b} #rightarrow J/#Psi p K"));
    mcFrame->GetXaxis()->SetTitle("GeV");
    fitMC.plotOn(mcFrame,RooFit::Name("mHist"));
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

    // start to fit Data
    RooRealVar par_ch1("ch1", "parameter to chebychev : 1st order",  0.5, -10., 10. );
    RooRealVar par_ch2("ch2", "parameter to chebychev : 2nd order", -0.2, -10., 10. );
    RooRealVar par_ch3("ch3", "parameter to chebychev : 3rd order", -0.2, -10., 10. );
    //RooChebychev pdf_cheb("cheb", "PDF : chebychev", plbMass, RooArgSet(par_ch1, par_ch2) );
    RooChebychev pdf_cheb("cheb", "PDF : chebychev", plbMass, RooArgSet(par_ch1, par_ch2, par_ch3) );
    RooRealVar N_cheb("N_cheb", "extend number : chebchev", (double)ntdata->GetEntries(), 0., 2.*(double)ntdata->GetEntries());
    RooExtendPdf extend_pdf_cheb("ext_cheb", "PDF: chebychev", pdf_cheb, N_cheb);

    // for test!
    //RooRealVar par_exp("parExp", "par to exponent", -0.05, -10., 1.);
    //RooExponential pdf_exp("expo", "PDF : exponential", plbMass, par_exp);
    //RooExtendPdf extend_pdf_cheb("ext_cheb", "PDF: chebychev", pdf_exp, N_cheb);

    double constr_mean  = par_mean .getVal();
    double constr_width = par_width.getVal();
    plbMass.setRange("sR1", 5.4, constr_mean - 5.*constr_width);
    plbMass.setRange("sR2", constr_mean + 5.*constr_width, 5.90);
    plbMass.setRange("tot", 5.4, 5.90);
    plbMass.setRange("signalRegion", constr_mean - 3.*constr_width, constr_mean + 3.*constr_width);
    //pdf_cheb.fitTo(binnedData);
    //pdf_cheb.fitTo(binnedData, RooFit::Range("sR1,sR2"), RooFit::Minos(kTRUE));
    extend_pdf_cheb.fitTo(fitData, RooFit::Minos(kTRUE));
    RooPlot* tmpFrame = plbMass.frame(RooFit::Title("d"));
    fitData.plotOn(tmpFrame, RooFit::Name("a"));
    pdf_cheb.plotOn( tmpFrame , RooFit::Name("b"));
    double tmpChi2 = tmpFrame->chiSquare("b", "a");
    tmpFrame->Draw();
    c1->SaveAs("storefig/resultHisto.pdf");
    c1->SaveAs("storefig/h_fittingResult.intermediate.eps");


    double maxevent = (double)ntdata->GetEntries();
    RooRealVar ns( "nSig", "number of signal", maxevent/1000., 0., maxevent );
    RooRealVar nb( "nBkg", "number of background", maxevent, 0., maxevent);
    RooAddPdf totModel( "totModel", "PDF : data + MC", RooArgList(mcModel, pdf_cheb), RooArgList(ns,nb));
    totModel.fitTo(fitData);
    // roofit vars end}}}

    double dataChi2 = 0.;
    // plot fit result.
    {
        RooPlot* dataFrame = plbMass.frame(RooFit::Title("data : #Lambda^{0}_{b} #rightarrow J/#Psi p K"));
        dataFrame->GetXaxis()->SetTitle("GeV");
    
        fitData.plotOn(dataFrame,RooFit::Name("dHist"));
        totModel.plotOn(dataFrame, RooFit::Name("dTot"));
        totModel.plotOn(dataFrame, RooFit::Name("dPart1"), RooFit::Components(mcModel), RooFit::LineStyle(7), RooFit::LineColor(2));
        totModel.plotOn(dataFrame, RooFit::Name("dPart2"), RooFit::Components(pdf_cheb),RooFit::LineStyle(7), RooFit::LineColor(40));
        dataChi2 = dataFrame->chiSquare("dPart2", "dHist");
    
    
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
        c1->SaveAs("storefig/resultHisto.pdf");
        delete legData;
        delete txt;
    }

    // plot pq & anti pq
    {
        RooPlot* pFrame = ppQMass.frame();
        RooPlot* nFrame = npQMass.frame();
        fitData.plotOn(pFrame, RooFit::LineColor(30));
        fitData.plotOn(nFrame, RooFit::LineColor(40));
        pFrame->Draw();
        c1->SaveAs("storefig/h_fittingResult.Data.pPQMass.allData.eps");
        c1->SaveAs("storefig/resultHisto.pdf");
        nFrame->Draw();
        c1->SaveAs("storefig/h_fittingResult.Data.nPQMass,allData.eps");
        c1->SaveAs("storefig/resultHisto.pdf");
    }

    // plot reduced data
    RooDataSet* redPfitData = (RooDataSet*) fitData.reduce("pJpsip<4.9&&pFD2D>0.3");
    RooDataSet* redNfitData = (RooDataSet*) fitData.reduce("nJpsip<4.9&&pFD2D>0.3");
    RooDataSet* redDataP = (RooDataSet*) fitData.reduce("pMass>5.6&&pMass<5.63&&pJpsip<4.9&&pFD2D>0.3");
    RooDataSet* redDataN = (RooDataSet*) fitData.reduce("nMass>5.6&&nMass<5.63&&nJpsip<4.9&&pFD2D>0.3");
    // plot reduced data
    {
        totModel.fitTo(*redPfitData);

        RooPlot* plbtkframe = plbMass.frame(RooFit::Title("#Lambda^{0}_{b} in cuts"));
        plbtkframe->GetXaxis()->SetTitle("GeV");

        redPfitData->plotOn(plbtkframe,RooFit::Name("dHist"));
        totModel.plotOn(plbtkframe, RooFit::Name("dTot"));
        totModel.plotOn(plbtkframe, RooFit::Name("dPart1"), RooFit::Components(mcModel), RooFit::LineStyle(7), RooFit::LineColor(2));
        totModel.plotOn(plbtkframe, RooFit::Name("dPart2"), RooFit::Components(pdf_cheb),RooFit::LineStyle(7), RooFit::LineColor(40));
        double indataChi2 = plbtkframe->chiSquare("dPart2", "dHist");


        TGraph* tg_dHist = (TGraph*)plbtkframe->findObject("dHist");
        TGraph* tg_dTot  = (TGraph*)plbtkframe->findObject("dTot");
        TGraph* tg_dPart1= (TGraph*)plbtkframe->findObject("dPart1");
        TGraph* tg_dPart2= (TGraph*)plbtkframe->findObject("dPart2");

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
        sprintf( tmpName, "#frac{#chi^{2}}{nDoF} = %.2e", indataChi2 );
        txt->AddText( tmpName );
        txt->AddText( ""      );
        sprintf( tmpName, "# of Sig: %.2e", ns.getVal() );
        txt->AddText( tmpName );
        sprintf( tmpName, "# of Bkg: %.2e", nb.getVal() );
        txt->AddText( tmpName );

        txt->SetFillColor(4000);
        txt->SetFillStyle(4000);
        plbtkframe->Draw();
        legData->Draw("same");
        txt->Draw("same");


        c1->SaveAs("storefig/h_fittingResult.Data.plbMass_cutFD2DB0.3.eps");
        c1->SaveAs("storefig/resultHisto.pdf");
        delete legData;
        delete txt;
    }


    // plot signal pq & anti pq
    {
        RooPlot* pFrame = ppQMass.frame();
        RooPlot* nFrame = npQMass.frame();
        redDataP->plotOn(pFrame, RooFit::LineColor(30));
        redDataN->plotOn(nFrame, RooFit::LineColor(40));
        pFrame->Draw();
        c1->SaveAs("storefig/h_fittingResult.Data.pPQMass.signalRegion.eps");
        c1->SaveAs("storefig/resultHisto.pdf");
        nFrame->Draw();
        c1->SaveAs("storefig/h_fittingResult.Data.nPQMass.signalRegion.eps");
        c1->SaveAs("storefig/resultHisto.pdf");
    }
    // plot signal ant tot FD2d
    {
        RooPlot* fd2dFrame = fd2d.frame();
        RooDataSet* redDataN = (RooDataSet*) fitData.reduce("nMass>5.6&&nMass<5.63&&nJpsip<4.9");
        //double scale = fitData.getRange(fd2d, 0.2, 0.25) / redDataN->getRange(fd2d, 0.2, 0.25);
        fitData.plotOn(fd2dFrame);
        //redDataN->plotOn(fd2dFrame, RooFit::Rescale(scale));
        fd2dFrame->Draw();
        c1->SaveAs("storefig/h_fittingResult.Data.FD2d.eps");
        c1->SaveAs("storefig/resultHisto.pdf");
    }
        

    //plbMass.setRange("signalRegion", 5.58, 5.64);
    //RooAbsReal* mcIntegral   = pdf_gaus.createIntegral( plbMass, RooFit::NormSet(plbMass), RooFit::Range("signalRegion") );
    RooAbsReal* mcIntegral   = mcModel.createIntegral( plbMass, RooFit::NormSet(plbMass), RooFit::Range("signalRegion") );
    RooAbsReal* dataIntegral = pdf_cheb.createIntegral( plbMass, RooFit::NormSet(plbMass), RooFit::Range("signalRegion") );
    std::cout << "number of signal : " << ns.getVal() << ", in signal Region: " << ns.getVal()*mcIntegral->getVal() << std::endl;
    std::cout << "number of bkg    : " << nb.getVal() << ", in signal Region: " << nb.getVal()*dataIntegral->getVal() << std::endl;
    std::cout << "significance: " << (ns.getVal()*mcIntegral->getVal())/sqrt(nb.getVal()*dataIntegral->getVal()) << std::endl;
    std::cout << "mc chi2/nDoF : " << mcChi2 << ", data chi2/nDoF : " << dataChi2 << std::endl;
    std::cout << "intermediate chi2/nDoF = " << tmpChi2 << std::endl;
    c1->SaveAs("storefig/resultHisto.pdf]");
    std::cout << "signal mean = " << par_mean.getVal() << ", width = " << par_width.getVal() << std::endl;
    delete c1;
    delete ntMC;
    delete ntdata;

    mcFile->Close();
    dataFile->Close();
    tmpFile->Close();
    delete tmpFile;
}
