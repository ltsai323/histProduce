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

typedef root_TreeHistoMain_GenInfo_plusminus_LbTk readMC;
//typedef root_TreeHistoMain_LbTk readData;
typedef root_TreeHistoMain_plusminus_LbTk readData;

TH1D* CalcBkgPullDist( RooWorkspace& wspace, RooRealVar& var, RooExtendPdf& expdf, RooRealVar& nsig );
int main()
{
    // settings {{{
    std::vector<double> gaRes;
    gaRes = { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0, 0., 0., 0., 0., 0. };
    //gaRes = { 4.46,-0.25,15.87,-0.15,2.39,0.10,1.87,-0.17,1.02,-0.21,-0.15,-0.18,-0.04,-0.08,-0.03,-0.04,0.21,0.00 };
 
    std::vector< std::pair<std::string, std::string> > fileNameLabels;
    fileNameLabels.push_back("storeroot/tReduced/tree_forGA_removeBsBdOnly/data_2016RunBCDEFGH.root", "2016Data");
    fileNameLabels.push_back("storeroot/tReduced/tree_forGA_removeBsBdOnly/data_2017RunBCDEF.root", "2017Data");

    const double Nsep = 14;
    const double massStep = 0.05;
    const double mjpsipStart = 4.1;

    std::vector<double> jpsipMassFrame;
    jpsipMassFrame.reserve(Nsep+1);
    for ( double i = 0; i < N+1; ++i )
        jpsipMassFrame.push_back( mjpsipStart + i * massStep );
    // settings end }}}


    TFile* mcFile = TFile::Open("storeroot/tReduced/tree_forGA_removeBsBdOnly/mc_LbToJPsipK_13TeV_noPU.root");
    readMC mc(nullptr);
    mc.SetInputFile( mcFile );

    TTree* mcTree = mc.readTree();

    TFile* tmpFile = new TFile("/tmp/ltsai/tmpFile.root", "RECREATE");
    TNtupleD* ntMC =  new TNtupleD("mcNtuple", "ntuple to MC", "pMass:pJpsip:nMass:nJpsip:pMPID:nMPID");
    ntMC->SetDirectory(tmpFile);


    TGaxis::SetMaxDigits(3);
    unsigned i = 0;
    unsigned N = mcTree->GetEntries();
    while ( i != N )
    {
        mcTree->GetEntry(i++);
        if ( mc.readD[readMC::plbtkMass             ] > 5.90 &&          
        if ( mc.readD[readMC::nlbtkMass             ] > 5.90 ) continue; 
        if ( mc.readD[readMC::plbtkMass             ] < 5.4  &&
        if ( mc.readD[readMC::nlbtkMass             ] < 5.4  ) continue; 
        if ( mc.readD[readMC::ptargetJpsiP_mass     ] > 4.9  &&
        if ( mc.readD[readMC::ntargetJpsiP_mass     ] > 4.9  ) continue;

        if ( mc.readD[readMC::plbtkFlightDistanceSig] < gaRes[ 0] ) continue;
        if ( mc.readD[readMC::plbtkVtxprob          ] < gaRes[ 1] ) continue;
        if ( mc.readD[readMC::plbtkPt               ] < gaRes[ 2] ) continue;
        if ( mc.readD[readMC::ptktkPt               ] < gaRes[ 4] ) continue;
        if ( mc.readD[readMC::pptonPt               ] < gaRes[ 6] ) continue;
        if ( mc.readD[readMC::pkaonPt               ] < gaRes[ 8] ) continue;
        
        ntMC->Fill(
            mc.readD[readMC::plbtkMass], mc.readD[readMC::ptargetJpsiP_mass],
            mc.readD[readMC::nlbtkMass], mc.readD[readMC::ntargetJpsiP_mass],
            mc.readI[readMC::pkaonMomPID], mc.readI[readMC::nkaonMomPID] );
    }   
        


    TCanvas* c1 = new TCanvas("c1","",1600,1000);
    c1->SaveAs("storefig/resultHisto.pdf[");

    for ( const auto& fileNameLabel : fileNameLabels )    
    {
        const std::string& fName  = fileNameLabel.first;
        const std::string& fLabel = fileNameLabel.second;
    
        TFile* dataFile = TFile::Open(fName);
    
        readData data(nullptr);
        data.SetInputFile( dataFile );
        TTree* dataTree = data.readTree();
    
        TNtupleD* ntData = new TNtupleD( ("nt_"+fLabel).c_str(), ("ntuple for "+fLabel).c_str(), "pMass:pJpsip:nMass:nJpsip" );
        RooRealVar pvarMass( "pMass", "#Lambda^{0}_{b} mass", 5.637, 5.4, 5.9 );
        RooRealVar nvarMass( "nMass", "#bar{#Lambda^{0}_{b}} mass", 5.637, 5.4, 5.9 );
        RooRealVar pPQMass ( "pJpsip", "jpsiP mass frame", 4.4, 4.1, 4.9 );
        RooRealVar nPQMass ( "nJpsip", "#bar{jpsiP} mass frame", 4.4, 4.1, 4.9 );
        RooRealVar pMPID   ( "pMPID", "pid for mother candidate", 0, -5500, 5500 );
        RooRealVar nMPID   ( "nMPID", "pid for mother candidate", 0, -5500, 5500 );
        ntData->SetDirectory(tmpFile);
    
        i = 0;
        N = dataTree->GetEntries();
        while ( i != N )
        {
            dataTree->GetEntry(i++);
            if ( data.readD[readData::plbtkMass             ] > 5.90 &&          
            if ( data.readD[readData::nlbtkMass             ] > 5.90 ) continue; 
            if ( data.readD[readData::plbtkMass             ] < 5.4  &&
            if ( data.readD[readData::nlbtkMass             ] < 5.4  ) continue; 
            if ( data.readD[readData::ptargetJpsiP_mass     ] > 4.9  &&
            if ( data.readD[readData::ntargetJpsiP_mass     ] > 4.9  ) continue;
    
            if ( data.readD[readData::plbtkFlightDistanceSig] < gaRes[ 0] ) continue;
            if ( data.readD[readData::plbtkVtxprob          ] < gaRes[ 1] ) continue;
            if ( data.readD[readData::plbtkPt               ] < gaRes[ 2] ) continue;
            if ( data.readD[readData::ptktkPt               ] < gaRes[ 4] ) continue;
            if ( data.readD[readData::pptonPt               ] < gaRes[ 6] ) continue;
            if ( data.readD[readData::pkaonPt               ] < gaRes[ 8] ) continue;
    
            ntData->Fill(
                data.readD[readData::plbtkMass], data.readD[readData::ptargetJpsiP_mass],
                data.readD[readData::nlbtkMass], data.readD[readData::ntargetJpsiP_mass]);
        }
        RooDataSet dDataset("dDataset", "dataset to data", ntData,RooArgSet(pvarMass, pPQMass, nvarMass, nPQMass) );
        RooDataSet mDataset("mDataset", "dataset to MC",   ntMC  ,RooArgSet(pvarMass, pPQMass, nvarMass, nPQMass, pMPID, nMPID) );
        RooRealvar* mass[2] = {&pvarMass, &nvarMass};
        char preName[2] = { 'p', 'n' };
    
        for ( int varIdx=0; varIdx<2; ++varIdx )
        {
            // fit lambdaB
            char reduceCut[256];
            sprintf( reduceCut, "%sMPID == 5122*(1-%d*2)", preName[varIdx]), varIdx );
            RooDataSet* mDataset = (RooDataSet*) mDataset->reduce(reduceCut);
            RooRealVar mean1("mean1", "parameter to gaus1 : mean", 5.637, 5.4, 5.9);
            RooRealVar width1("width1", "parameter to gaus1 : width", 0.01, 0.00001, 1.0);
            RooGaussian gaus1("pdfGau1", "PDF : gaus1", *mass[varIdx], mean1, width1 )
            RooRealVar mean2("mean2", "parameter to gaus1 : mean", 5.637, 5.4, 5.9);
            RooRealVar width2("width2", "parameter to gaus1 : width", 0.01, 0.00001, 1.0);
            RooGaussian gaus2("pdfGau2", "PDF : gaus1", *mass[varIdx], mean1, width1 );
        
            RooRealVar frac("pfracMC", "", 0.7, 0.0000001, 1.);
            RooAddPdf mcModel("MCmodel", "", RooArgList(gaus1, gaus2), RooArgList(frac));
            mcModel.fitTo(*mDataset);
            mcModel.fitTo(*mDataset);
            mean1.setConstant(true);
            width1.setConstant(true);
            mean2.setConstant(true);
            width2.SetConstant(true);
            frac.setConstant(true);
            
        
            
            for ( unsigned cutIdx = 0; cutIdx < massFrameJPsip.size(); ++cutIdx )
            {
                // choose ntuple data
                char ntCut[32];
                sprintf( ntCut, "%sjpsip>%.3f&&%sjpsip<%.3f", preName[varIdx], jpsipMassFrame[cutIdx], preName[varIdx], jpsipMassFrame[cutIdx+1] );
                RooDataSet* reducedData = (RooDataSet*) ntData->reduce( RooFit::Cut(ntCut) );
        
        
                // declare background PDF
                RooRealVar par_ch1("ch1", "parameter to chebychev : 1st order",  0.5, -10., 10. );
                RooRealVar par_ch2("ch2", "parameter to chebychev : 2nd order", -0.2, -10., 10. );
                RooRealVar par_ch3("ch3", "parameter to chebychev : 3rd order", -0.2, -10., 10. );
                RooRealVar par_ch4("ch4", "parameter to chebycheb : 4th order", -0.2, -10., 10. );
                RooChebychev pdf_cheb("cheb", "PDF : chebychev", *mass[varIdx], RooArgSet(par_ch1, par_ch2, par_ch3, par_ch4) );
                RooRealVar N_bkg("N_bkg", "extend number : chebchev", (double)reducedData->numEntries(), 0., 2.*(double)reducedData->numEntries());
                RooRealVar N_sig("N_sig", "extend number : gaussian signal", 0., 0., 2.*(double)reducedData->numEntries());
                RooExtendPdf extend_pdf_bkg("extPDF_sig", "extPDF: bkg 4th chebychev", pdf_cheb, N_bkg); RooExtendPdf extend_pdf_sig("ext_gaus", "PDF: gaussian" , pdf_sig, N_sig);
                RooExtendPdf extend_pdf_sig("extPDF_bkg", "extPDF: sig 2nd gaussian" , pdf_sig, N_sig);
                totModel->fitTo(*reducedData, RooFit::Minos(true));
        
                RooPlot* frame = mass[varIdx]->frame();
                reducedData->plotOn(frame, RooFit::Name("reducedData"));
                totModel   ->plotOn(frame, RooFit::Name("fitGaus")    , RooFit::Components(pdf_gaus), RooFit::ColorStyle(kDashed), RooFit::LineColor(30  ));
                totModel   ->plotOn(frame, RooFit::Name("fitMC")                                    , RooFit::ColorStyle(kDashed), RooFit::LineColor(kRed));
                frame->Draw();
                TPaveText* pave = new TPaveText(0.6, 0.5, 0.89, 0.89, "NDC");
                pave->SetFillColor(4000);
                pave->SetFillStyle(4000);
                
                double fitChi2 = frame->chiSquare("fitMC", "reducedData");
                char paveContent[128];
                sprintf( paveContent, "in j/#psi frame=[%.3f,%.3f]:", jpsipMassFrame[cutIdx], jpsipMassFrame[cutIdx+1] );
                pave->AddText( paveContent );
                sprintf( paveContent, "#frac{#chi^{2}}{nDoF} = %.3f", fitChi2 );
                pave->AddText( paveContent );
                sprintf( paveContent, "Get #Lambda^{0}_{b} in Entries : #frac{%.0f}{%.0f} = %.3f", Nsig->getVal(), reducedData->numEntries(), Nsig->getVal()/reducedData->numEntries() );
                pave->AddText( paveContent );
        
                pave->Draw("same");
                char epsName[128];
                sprintf( epsName, "storefig/tmp.lbMass.%sjpsipMassFrom%.3fTo%.3f.eps", preName[varIdx], jpsipMassFrame[cutIdx], jpsipMassFrame[cutIdx+1] );
                c1->SaveAS( epsName );
                c1->SaveAs("storefig/resultHisto.pdf");
    
                delete pave;
            } // for loop for cutIdx
        } // for loop for varIdx
        dataFile->Close();
        delete ntData;
    } // for loop for fileNameLabel

    delete c1;
    mcFile->Close();
    delete ntMC;
    tmpFile->Close();
    delete tmpFile;
}
