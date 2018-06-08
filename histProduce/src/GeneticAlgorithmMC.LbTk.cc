#include "histProduce/histProduce/interface/GeneticAlgorithmMC.LbTk.h"
#include "histProduce/histProduce/interface/rooTHMainLbTk.h"

#include "TRandom3.h"
#include "TTree.h"

#include "RooRealVar.h"
#include "RooDataHist.h"
#include "RooArgList.h"
#include "RooGaussian.h"
#include "RooPolynomial.h"
#include "RooArgSet.h"
#include "RooAddPdf.h"
#include "RooAbsReal.h"
#include "RooGlobalFunc.h" // Minos command
    // use bin-fit in calculate fitness.
    // to fit background shape and estimate the background number

    // variable names:
    // mlbtkFlightDistanceSig, mlbtkVtxprob, mlbtkMom, mlbtkPt, mtktkPt, mtktkMom, mptkPt, mptkMom, mntkPt, mntkMom, m(ptkDEDX_Harmonic/ptkMom), M(ptkDEDX_Harmonic/ptkMom), m(ntkDEDX_Harmonic/ntkMom), M(ntkDEDX_Harmonic/ntkMom), m(ptkIPt/ptkIPtErr), M(ptkIPt/ptkIPtErr), m(ntkIPt/ntkIPtErr), M(ntkIPt/ntkIPtErr)
namespace
{
    enum varNames
    {
        mlbtkFDSig, mlbtkVtxprob, mlbtkPt, mlbtkMom, mtktkPt, mtktkMom, mptkPt, mptkMom, mntkPt, mntkMom,
        mptkDEDX_Mom_ratio, MptkDEDX_Mom_ratio, mntkDEDX_Mom_ratio, MntkDEDX_Mom_ratio, mptkIPtSig, MptkIPtSig, mntkIPtSig, MntkIPtSig,
        NVAR
    };
}


GeneticAlgorithmMC_LbTk::GeneticAlgorithmMC_LbTk( unsigned Nchromo = 10000, unsigned maxGen = 1000 ) :
    GeneticAlgorithmMC( NVAR, Nchromo, maxGen ), lbData(nullptr), sigMC(nullptr), bkgMC(nullptr)
{
    setSignalRegion( 5.6195, 0.04 );
    chromosRange = new double*[NVAR];
    for ( int i=0; i<NVAR; ++i )
        chromosRange[i] = new double[2];
    chromosRange[mlbtkFDSig        ][0] =   0.111;
    chromosRange[mlbtkFDSig        ][1] =  10.   ;
    chromosRange[mlbtkVtxprob      ][0] =   0.13 ;
    chromosRange[mlbtkVtxprob      ][1] =   0.4  ;
    chromosRange[mlbtkPt           ][0] =  10.   ;
    chromosRange[mlbtkPt           ][1] =  25.   ;
    chromosRange[mtktkPt           ][0] =   3.   ;
    chromosRange[mtktkPt           ][1] =   6.   ;
    chromosRange[mptkPt            ][0] =   2.0  ;
    chromosRange[mptkPt            ][1] =   4.0  ;
    chromosRange[mntkPt            ][0] =   1.0  ;
    chromosRange[mntkPt            ][1] =   3.0  ;
//    chromosRange[mptkDEDX_Mom_ratio][0] =   0.   ;
//    chromosRange[mptkDEDX_Mom_ratio][1] =   1.   ;
//    chromosRange[MptkDEDX_Mom_ratio][0] =  10.   ;
//    chromosRange[MptkDEDX_Mom_ratio][1] =  20.   ;
//    chromosRange[mntkDEDX_Mom_ratio][0] =   0.   ;
//    chromosRange[mntkDEDX_Mom_ratio][1] =   1.   ;
//    chromosRange[MntkDEDX_Mom_ratio][0] =  10.   ;
//    chromosRange[MntkDEDX_Mom_ratio][1] =  20.   ;
//    chromosRange[mptkIPtSig        ][0] = -10.   ;
//    chromosRange[mptkIPtSig        ][1] =   0.   ;
//    chromosRange[MptkIPtSig        ][0] =   0.   ;
//    chromosRange[MptkIPtSig        ][1] =  10.   ;
//    chromosRange[mntkIPtSig        ][0] = -10.   ;
//    chromosRange[mntkIPtSig        ][1] =   0.   ;
//    chromosRange[MntkIPtSig        ][0] =   0.   ;
//    chromosRange[MntkIPtSig        ][1] =  10.   ;

    hData  = new TH1D("hData" , "hData" , 75, 5.2, 5.95 );
    hSigMC = new TH1D("hSigMC", "hSigMC", 75, 5.2, 5.95 );
    hbkgMC = new TH1D("hBkgMC", "hBkgMC", 75, 5.2, 5.95 );
    return;
}
GeneticAlgorithmMC_LbTk::~GeneticAlgorithmMC_LbTk()
{
    int idx = NVAR;
    while ( idx-- )
        delete chromosRange[idx];
    delete chromosRange;
    chromosRange = nullptr;
    return;
}
void GeneticAlgorithmMC_LbTk::SetData( TFile* file )
{
    lbData.SetInputFile( file );
    setNEntries( lbData.readTree()->GetEntries() );
    return;
}
void GeneticAlgorithmMC_LbTk::SetSignalMC( TFile* file )
{
    sigMC.SetInputFile( file );
    return;
}
void GeneticAlgorithmMC_LbTk::SetBackgroundMC( TFile* file )
{
    bkgMC.SetInputFile( file );
    return;
}
void GeneticAlgorithmMC_LbTk::PrepareChromosomes()
{
    TRandom3* rnd = getRnd();
    unsigned nchromos = getNChromos();
    for ( unsigned i = 0; i < nchromos; ++i )
        for ( unsigned j = 0; j < NVAR; ++j )
            chromos[i][j] = rnd->Uniform( chromosRange[j][0], chromosRange[j][1] );
}
inline void GeneticAlgorithmMC_LbTk::CalculateFitness( const unsigned idx, double& fitness, double& fitnessErr )
{
    // initialize result and histogram
    fitness = fitnessErr = 0;
    hSigMC->Reset();
    hData ->Reset();

    // sigMC part {{{
    if ( sigMC.readTree() )
    {
        TTree* sigMCtree = sigMC.readTree();
        unsigned iEntry = 0;
        unsigned NEntries = sigMCtree->GetEntries();
        if ( NEntries == 0 ) return;
        while ( iEntry != NEntries )
        {
            sigMCtree->GetEntry( iEntry++ );

            if ( chromos[idx][mlbtkFDSig        ] > sigMC.readD[readData::lbtkFlightDistanceSig] ) continue;
            if ( chromos[idx][mlbtkVtxprob      ] > sigMC.readD[readData::lbtkVtxprob          ] ) continue;
            if ( chromos[idx][mlbtkPt           ] > sigMC.readD[readData::lbtkPt               ] ) continue;
            if ( chromos[idx][mtktkPt           ] > sigMC.readD[readData::tktkPt               ] ) continue;
            if ( chromos[idx][mptkPt            ] > sigMC.readD[readData::ptkPt                ] ) continue;
            if ( chromos[idx][mntkPt            ] > sigMC.readD[readData::ntkPt                ] ) continue;

            //if ( chromos[idx][mptkDEDX_Mom_ratio] > sigMC.readD[readData::ptkDEDX_Harmonic]/sigMC.readD[readData::ptkMom]    ) continue;
            //if ( chromos[idx][MptkDEDX_Mom_ratio] < sigMC.readD[readData::ptkDEDX_Harmonic]/sigMC.readD[readData::ptkMom]    ) continue;
            //if ( chromos[idx][mntkDEDX_Mom_ratio] > sigMC.readD[readData::ntkDEDX_Harmonic]/sigMC.readD[readData::ntkMom]    ) continue;
            //if ( chromos[idx][MntkDEDX_Mom_ratio] < sigMC.readD[readData::ntkDEDX_Harmonic]/sigMC.readD[readData::ntkMom]    ) continue;
            // if ( chromos[idx][mptkIPtSig        ] > sigMC.readD[readData::ptkIPt          ]/sigMC.readD[readData::ptkIPtErr] ) continue;
            // if ( chromos[idx][MptkIPtSig        ] < sigMC.readD[readData::ptkIPt          ]/sigMC.readD[readData::ptkIPtErr] ) continue;
            // if ( chromos[idx][mntkIPtSig        ] > sigMC.readD[readData::ntkIPt          ]/sigMC.readD[readData::ntkIPtErr] ) continue;
            // if ( chromos[idx][MntkIPtSig        ] < sigMC.readD[readData::ntkIPt          ]/sigMC.readD[readData::ntkIPtErr] ) continue;

            double candMass = sigMC.readD[readData::lbtkMass];
            // if ( !inCalculationRegion(candMass) ) continue;
            // if ( inSignalRegion(candMass) ) ++nsig;
            // else                            ++nbkg;
            hSigMC->Fill(candMass);
        }
    } // sigMC part end }}}
    // data part {{{
    if ( lbData.readTree() )
    {
        TTree* tree = lbData.readTree();
        unsigned iEntry = 0;
        unsigned NEntries = getNEntries();
        //unsigned NEntries = 10000;

        while ( iEntry != NEntries )
        {
            tree->GetEntry( iEntry++ );

            if ( chromos[idx][mlbtkFDSig        ] > lbData.readD[readData::lbtkFlightDistanceSig] ) continue;
            if ( chromos[idx][mlbtkVtxprob      ] > lbData.readD[readData::lbtkVtxprob          ] ) continue;
            if ( chromos[idx][mlbtkPt           ] > lbData.readD[readData::lbtkPt               ] ) continue;
            if ( chromos[idx][mtktkPt           ] > lbData.readD[readData::tktkPt               ] ) continue;
            if ( chromos[idx][mptkPt            ] > lbData.readD[readData::ptkPt                ] ) continue;
            if ( chromos[idx][mntkPt            ] > lbData.readD[readData::ntkPt                ] ) continue;

            //if ( chromos[idx][mptkDEDX_Mom_ratio] > lbData.readD[readData::ptkDEDX_Harmonic]/lbData.readD[readData::ptkMom]    ) continue;
            //if ( chromos[idx][MptkDEDX_Mom_ratio] < lbData.readD[readData::ptkDEDX_Harmonic]/lbData.readD[readData::ptkMom]    ) continue;
            //if ( chromos[idx][mntkDEDX_Mom_ratio] > lbData.readD[readData::ntkDEDX_Harmonic]/lbData.readD[readData::ntkMom]    ) continue;
            //if ( chromos[idx][MntkDEDX_Mom_ratio] < lbData.readD[readData::ntkDEDX_Harmonic]/lbData.readD[readData::ntkMom]    ) continue;
            if ( chromos[idx][mptkIPtSig        ] > lbData.readD[readData::ptkIPt          ]/lbData.readD[readData::ptkIPtErr] ) continue;
            if ( chromos[idx][MptkIPtSig        ] < lbData.readD[readData::ptkIPt          ]/lbData.readD[readData::ptkIPtErr] ) continue;
            if ( chromos[idx][mntkIPtSig        ] > lbData.readD[readData::ntkIPt          ]/lbData.readD[readData::ntkIPtErr] ) continue;
            if ( chromos[idx][MntkIPtSig        ] < lbData.readD[readData::ntkIPt          ]/lbData.readD[readData::ntkIPtErr] ) continue;

            double candMass = lbData.readD[readData::lbtkMass];
            // if ( !inCalculationRegion(candMass) ) continue;
            // if ( inSignalRegion(candMass) ) ++nsig;
            // else                            ++nbkg;
            hData->Fill(candMass);
        }
    } // data part end }}}
    if ( hSigMC->GetEntries() < 300 ) return;
    if ( hData ->GetEntries() < 300 ) return;
//
//    // result calculation
//    RooRealVar varMass( "varMass", "varMass", 5.55, 5.7 );
//    // to read data stored in dHistogram
//    RooDataHist binnedData("binnedData", "bindata", RooArgList(varMass), hData  );
//    RooDataHist binnedMC  ("binnedMC"  , "binmc"  , RooArgList(varMass), hSigMC );
//
//    // start to fit
//    RooRealVar par_mean( "mean", "parameter to gaussian: mean", 5.619, 5.6, 5.64 );
//    RooRealVar par_width("width","parameter to gaussian: width",0.1, 0.0001, 1. );
//    RooGaussian pdf_gaus("gaus", "PDF : gaussian", varMass, par_mean, par_width );
//    pdf_gaus.fitTo( binnedMC );
//
//    // use the result of fitting MC to fit data.
//    par_mean.setConstant(true);
//    par_width.setConstant(true);
//
//
//    RooRealVar par_c1( "c1", "parameter to polynomial 1st order", 0.3, -10., 10. );
//    RooRealVar par_c2( "c2", "parameter to polynomial 2nd order", 0.3, -10., 10. );
//    RooPolynomial pdf_poly( "pdf_poly", "PDF : polynomial", varMass, RooArgSet(par_c1, par_c2) );
//
//    RooRealVar frac ( "frac", "fraction to gaus/poly", 0.15, 0.000001, 1.0 );
//    RooAddPdf model( "model", "PDF : poly + gaus", RooArgList( pdf_gaus, pdf_poly ), RooArgList( frac ) );
//
//    model.fitTo( binnedData, RooFit::Minos(true) );
//
//
    RooRealVar varMass( "varMass", "#Lambda^{0}_{b} mass(GeV)", 5.2, 5.95 );
    RooDataHist binnedMC("binnedMC", "lbMass in MC", RooArgList(varMass), hSigMC);
    RooDataHist binnedData("binnedData", "lbMass in Data", RooArgList(varMass), hData);

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

    // keep the MC fitting result.
    par_mean.setConstant(true);
    par_width.setConstant(true);
    par_mean2.setConstant(true);
    par_width2.setConstant(true);
    frac.setConstant(true);

    // start to fit Data
    RooRealVar par_c1("c1", "parameter to polynomial : 1st order", -0.02, -10., 10. );
    RooRealVar par_c2("c2", "parameter to polynomial : 2nd order", -0.02, -10., 10. );
    RooPolynomial pdf_poly("poly", "PDF : polynomial", varMass, RooArgSet(par_c1, par_c2) );
    double maxevent = (double)hData->GetEntries();
    RooRealVar ns( "nSig", "number of signal", maxevent/1000., 0., maxevent );
    RooRealVar nb( "nBkg", "number of background", maxevent, 0., maxevent);
    RooAddPdf totModel( "totModel", "PDF : data + MC", RooArgList(mcModel, pdf_poly), RooArgList(ns,nb));
    totModel.fitTo(binnedData);



    varMass.setRange("signalRegion", mSig, MSig);
    //RooAbsReal* mcIntegral   = pdf_gaus.createIntegral( varMass, RooFit::NormSet(varMass), RooFit::Range("signalRegion") );
    RooAbsReal* mcIntegral   = mcModel.createIntegral( varMass, RooFit::NormSet(varMass), RooFit::Range("signalRegion") );
    RooAbsReal* dataIntegral = pdf_poly.createIntegral( varMass, RooFit::NormSet(varMass), RooFit::Range("signalRegion") );
    double nsig = mcIntegral->getVal() * ns.getVal();
    double nbkg = dataIntegral->getVal() * nb.getVal();
    fitness = nsig / sqrt(nsig+nbkg);
    fitnessErr = 0.;
    return;
}




bool GeneticAlgorithmMC_LbTk::BasicCheck()
{
    printf( "Basic Checking...\n" );
    if ( !lbData.readTree() )
    { printf( "\tdataTree not load!\n" ); return false; }
    if ( !sigMC.readTree() )
    { printf( "\tsigMC is not set!\n" ); return false; }
    if ( bkgMC.readTree() )
        printf( "bkgMC is set\n" );
    else
        printf( "\tbkgMC is not set\n" );

    if ( !lbData.readTree()->GetEntries() )
    { printf( "\tthere is no entries in the tree!\n" ); return false; }
    printf( "\tLbTk part checking : Done!\n" );

    return true;
}
void GeneticAlgorithmMC_LbTk::setSignalRegion( double mean, double width )
{
    mWindow = mean - width;
    MWindow = mean + width;
    mSig = mean - 0.5 * width;
    MSig = mean + 0.5 * width;
    return;
}
bool GeneticAlgorithmMC_LbTk::inSignalRegion( double mass ) const
{
    if ( mSig > mass ) return false;
    if ( MSig < mass ) return false;
    return true;
}
bool GeneticAlgorithmMC_LbTk::inCalculationRegion( double mass ) const
{
    if ( mWindow > mass ) return false;
    if ( MWindow < mass ) return false;
    return true;
}
