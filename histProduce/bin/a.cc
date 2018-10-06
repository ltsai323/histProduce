#include <iostream>
#include <vector>
//#include "histProduce/histProduce/interface/rooTHMainLbTk.h"
#include "histProduce/histProduce/interface/rooTHMainpnLbTk.h"
#include "histProduce/histProduce/interface/rooTHMainGenpnLbTk.h"

#include "TTree.h"
#include "TFile.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TGraph.h"
#include "TGaxis.h"
#include "TNtupleD.h"
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
typedef root_TreeHistoMain_plusminus_LbTk readData;

bool inSignalRegion( double mass );
int main()
{
    std::vector<double> gaRes;
    //gaRes = { 4.46,-0.25,15.87,-0.15,2.39,0.10,1.87,-0.17,1.02,-0.21,-0.15,-0.18,-0.04,-0.08,-0.03,-0.04,0.21,0.00 };
    gaRes = { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0, 0., 0., 0., 0., 0. };
 
    std::vector< std::pair<std::string,std::string> > fileNameLabels;
    fileNameLabels.push_back( std::make_pair("storeroot/tReduced/tree_forGA_removeBsBdOnly/data_2016RunBCDEFGH.root", "2016Data") );
    fileNameLabels.push_back( std::make_pair("storeroot/tReduced/tree_forGA_removeBsBdOnly/data_2017RunBCDEF.root" , "2017Data") );




    const unsigned Nhis = 14;
    const double   gapNum=0.05;
    std::vector<double> massFrameJPsip;
    massFrameJPsip.reserve(Nhis+1);
    for ( unsigned i=0; i<Nhis+1; ++i )
        massFrameJPsip.push_back(4.1+(double)i*gapNum);
    TFile* tmpFile = new TFile("/tmp/ltsai/tmp.root", "RECREATE");
    // use&fit MC {{{
    TFile* mcFile = TFile::Open("storeroot/tReduced/tree_forGA_removeBsBdOnly/mc_LbToJPsipK_13TeV_noPU.root");
    readMC mc(nullptr);
    mc.SetInputFile( mcFile );

    TTree* mcTree = mc.readTree();

    TNtupleD* nt_MC = new TNtupleD("ntupleMC", "", "pmass:pjpsipMass:nmass:njpsipMass");
    nt_MC->SetDirectory(tmpFile);
    unsigned iMC = 0;
    unsigned NMC = mcTree->GetEntries();
    while ( iMC != NMC )
    {
        mcTree->GetEntry(iMC++);
        if ( mc.readD[readMC::plbtkMass]>5.90 && mc.readD[readMC::nlbtkMass]>5.90 ) continue;
        if ( mc.readD[readMC::plbtkMass]<5.40 && mc.readD[readMC::nlbtkMass]<5.40 ) continue;
        if ( mc.readD[readMC::ptargetJpsiP_mass] < massFrameJPsip[0] && mc.readD[readMC::ntargetJpsiP_mass] < massFrameJPsip[0]   ) continue;
        if ( mc.readD[readMC::ptargetJpsiP_mass] > massFrameJPsip[Nhis] && mc.readD[readMC::ntargetJpsiP_mass] > massFrameJPsip[Nhis]) continue;

        // if ( mc.readD[readMC::plbtkFlightDistanceSig] < gaRes[0] ) continue;
        // if ( mc.readD[readMC::plbtkVtxprob          ] < gaRes[1] ) continue;
        // if ( mc.readD[readMC::plbtkPt               ] < gaRes[2] ) continue;
        // if ( mc.readD[readMC::ptktkPt               ] < gaRes[4] ) continue;
        // if ( mc.readD[readMC::pptonPt               ] < gaRes[6] ) continue;
        // if ( mc.readD[readMC::pkaonPt               ] < gaRes[8] ) continue;
        if ( mc.readI[readMC::pkaonMomPID] != 5122 && mc.readI[readMC::nkaonMomPID] != -5122 ) continue;
        nt_MC->Fill(mc.readD[readMC::plbtkMass], mc.readD[readMC::ptargetJpsiP_mass], mc.readD[readMC::nlbtkMass], mc.readD[readMC::ntargetJpsiP_mass]);
    }   
    RooRealVar pvarMass( "pmass", "", 5.6, 5.4, 5.90 );
    RooRealVar  npqMass( "pjpsipMass", "", 4.4, 4.1, 4.1+(double)(Nhis+1)*gapNum );
    RooRealVar nvarMass( "pmass", "", 5.6, 5.4, 5.90 );
    RooRealVar  npqMass( "pjpsipMass", "", 4.4, 4.1, 4.1+(double)(Nhis+1)*gapNum );
    RooDataSet mcDataset( "mcDataset", "MC from Ntuple", nt_MC, RooArgSet( varMass, pqMass ) );

    // start to fit MC
    RooRealVar par_mean( "mean", "parameter to gaussian: mean", 5.637, 5.55, 5.7 );
    RooRealVar par_width("width","parameter to gaussian: width",0.01, 0.000001, 10. );
    RooGaussian pdf_gaus("gaus", "PDF : gaussian", varMass, par_mean, par_width );
    RooRealVar par_mean2( "mean2", "parameter to gaussian: mean", 5.6142, 5.55, 5.7 );
    RooRealVar par_width2("width2","parameter to gaussian: width", 0.0342, 0.000001, 1. );
    RooGaussian pdf_gaus2("gaus2", "PDF : gaussian", varMass, par_mean2, par_width2 );
    RooRealVar frac( "frac", "fraction to gaussian & polynomial", 0.7, 0.001, 1.00 );
    RooAddPdf mcModel( "mcModel", "model to MC", RooArgList(pdf_gaus, pdf_gaus2), RooArgList(frac) );
    mcModel.fitTo(mcDataset);
    par_mean.setConstant(true);
    par_width.setConstant(true);
    par_mean2.setConstant(true);
    par_width2.setConstant(true);
    frac.setConstant(true);
    // use MC end }}}

    char myStr[256];
    TCanvas* c1 = new TCanvas("c1", "", 1600, 1000);
    c1->SaveAs("storefig/tmp2.sepRunsHistos.pdf[");
    RooPlot* mcFrame = varMass.frame();
    mcDataset.plotOn(mcFrame);
    mcModel.plotOn(mcFrame);
    mcFrame->Draw();
    c1->SaveAs("storefig/tmp2.hMC22.eps");

    const unsigned fileNum = fileNameLabels.size();
    for ( unsigned idx = 0; idx < fileNum; ++idx )
    {
        const std::string& fName = fileNameLabels[idx].first;
        const std::string& label = fileNameLabels[idx].second;
        TFile* dataFile = TFile::Open( fName.c_str() );
        readData data(nullptr);
        data.SetInputFile( dataFile );
        TTree* dataTree = data.readTree();
        TNtupleD* nt_data = new TNtupleD("ntupleData", "", "mass:jpsipMass");
        nt_data->SetDirectory(tmpFile);

        unsigned i = 0;
        unsigned N = dataTree->GetEntries();
        while ( i != N )
        {
            dataTree->GetEntry(i++);
            if ( data.readD[readData::plbtkMass             ] > 5.90 ) continue; 
            if ( data.readD[readData::plbtkMass             ] < 5.4  ) continue; 
            if ( data.readD[readData::ptargetJpsiP_mass     ] < massFrameJPsip[0]   ) continue;
            if ( data.readD[readData::ptargetJpsiP_mass     ] > massFrameJPsip[Nhis]) continue;

            if ( data.readD[readData::plbtkFlightDistanceSig] < gaRes[ 0] ) continue;
            if ( data.readD[readData::plbtkVtxprob          ] < gaRes[ 1] ) continue;
            if ( data.readD[readData::plbtkPt               ] < gaRes[ 2] ) continue;
            if ( data.readD[readData::ptktkPt               ] < gaRes[ 4] ) continue;
            if ( data.readD[readData::pptonPt               ] < gaRes[ 6] ) continue;
            if ( data.readD[readData::pkaonPt               ] < gaRes[ 8] ) continue;

            nt_data->Fill( data.readD[readData::plbtkMass], data.readD[readData::ptargetJpsiP_mass] );
        }

        RooDataSet totData = RooDataSet("dataDataset", "data from Ntuple", nt_data, RooArgList(varMass, pqMass));

        // draw the output histograms
        for ( unsigned hIdx = 0; hIdx < Nhis; ++hIdx )
        {
            unsigned kIdx=hIdx+1;
            if ( kIdx > 9 ) kIdx +=20;


            // choose ntuple data
            char ntCut[32];
            sprintf( ntCut, "jpsipMass>%.3f&&jpsipMass<%.3f", massFrameJPsip[hIdx], massFrameJPsip[hIdx+1] );
            RooDataSet* reducedData = (RooDataSet*) totData.reduce( RooFit::Cut(ntCut) );

            // declare background PDF
            RooRealVar par_ch1("ch1", "parameter to chebychev : 1st order",  0.5, -10., 10. );
            RooRealVar par_ch2("ch2", "parameter to chebychev : 2nd order", -0.2, -10., 10. );
            RooRealVar par_ch3("ch3", "parameter to chebychev : 3rd order", -0.2, -10., 10. );
            RooRealVar par_ch4("ch4", "parameter to chebycheb : 4th order", -0.2, -10., 10. );
            RooChebychev pdf_cheb("cheb", "PDF : chebychev", varMass, RooArgSet(par_ch1, par_ch2, par_ch3, par_ch4) );
            RooRealVar N_cheb("N_cheb", "extend number : chebchev", (double)reducedData->numEntries(), 0., 2.*(double)reducedData->numEntries());
            RooRealVar N_gaus("N_gaus", "extend number : gaussian signal", 0., 0., 2.*(double)reducedData->numEntries());
            RooExtendPdf extend_pdf_cheb("ext_cheb", "PDF: chebychev", pdf_cheb, N_cheb);
            RooExtendPdf extend_pdf_gaus("ext_gaus", "PDF: gaussian" , mcModel, N_gaus);
            RooAddPdf totModel("totPDF", "totModel", RooArgList(extend_pdf_gaus, extend_pdf_cheb));
            totModel.fitTo(*reducedData, RooFit::Minos(true));

            RooPlot* frame = varMass.frame();
            reducedData->plotOn(frame, RooFit::Name("pReducedData"));
            totModel.    plotOn(frame, RooFit::Name("pSignalModel"), RooFit::Components(mcModel), RooFit::LineStyle(7), RooFit::LineColor(30  ));
            totModel.    plotOn(frame, RooFit::Name("pTotModel")   ,                                                    RooFit::LineColor(kRed));
            frame->Draw();
            double dataChi2 = frame->chiSquare("pTotModel", "pReducedData");
            std::cout << ntCut << ", lbtk yield = " << N_gaus.getVal() << std::endl;
            TPaveText* pave = new TPaveText(0.6, 0.6, 0.89, 0.89, "NDC");
            pave->SetFillStyle(4000);
            pave->SetFillColor(4000);

            char myWord[128];
            sprintf(myWord, "At j/#psi p mass region=[ %.2f, %.2f]", massFrameJPsip[hIdx], massFrameJPsip[hIdx+1]);
            pave->AddText(myWord);
            sprintf(myWord, "#Lambda^{0}_{b} yield: %.0f in Ntries: %d", N_gaus.getVal(), reducedData->numEntries());
            pave->AddText(myWord);
            sprintf(myWord, "fitting frac{#chi^{2}}{nDoF} = %.2f ", dataChi2);
            pave->AddText(myWord);
            pave->Draw("same");


            
            sprintf( myStr, "storefig/tmp2.h_GARes_%s_jpsipFrameFrom%.2fTo%.2f.eps", label.c_str(), massFrameJPsip[hIdx], massFrameJPsip[hIdx+1] );
            c1->SaveAs( myStr );
            c1->SaveAs( "storefig/tmp2.sepRunsHistos.pdf" );
        }


        //TLegend* leg = new TLegend( 0.60, 0.40, 0.88, 0.85, "#Lambda^{0}_{b} Mass in each j/#Psi p mass region", "NDC" );
        //
        //for ( unsigned hIdx = 0; hIdx < Nhis; ++hIdx )
        //{

        //    sprintf( myStr, "[%.2f, %.2f]", massFrameJPsip[hIdx], massFrameJPsip[hIdx+1] );
        //    leg->AddEntry(hLbMass[idx*Nhis+hIdx], myStr, "L");
        //}
        //leg->SetFillStyle(4000);
        //leg->SetFillColor(4000);
        //leg->SetLineColor(0);
        //leg->SetNColumns(2);
        //leg->Draw("same");
        //c1->SaveAs( "storefig/h_GARes_LbMass_SummaryTopQSepArea.eps" );
        //c1->SaveAs( "storefig/tmp2.sepRunsHistos.pdf" );



        //delete leg;
        dataFile->Close();
        delete nt_data;
    }
    c1->Clear();
    c1->SaveAs( "storefig/tmp2.sepRunsHistos.pdf]");
    


    delete c1;
    delete nt_MC;
    tmpFile->Close();
}

// not yet complete!!!
bool inSignalRegion( double mass )
{
    if ( mass < 5.6196 - 1.00 ) return false;
    if ( mass > 5.6196 + 1.00 ) return false;
    return true;
}
