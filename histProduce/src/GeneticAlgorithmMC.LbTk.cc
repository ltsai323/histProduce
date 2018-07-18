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
#include "RooChebychev.h"
#include "RooPlot.h"

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
    //setSignalRegion( 5.6195, 0.04 );
    setHistoRange( 5.2, 5.95 );
    setHistoNBins( 75 );
    chromosRange = new double*[NVAR];
    for ( int i=0; i<NVAR; ++i )
        chromosRange[i] = new double[2];
    chromosRange[mlbtkFDSig        ][0] =   2.111;
    chromosRange[mlbtkFDSig        ][1] =  10.   ;
    chromosRange[mlbtkVtxprob      ][0] =   0.13 ;
    chromosRange[mlbtkVtxprob      ][1] =   0.4  ;
    chromosRange[mlbtkPt           ][0] =  10.   ;
    chromosRange[mlbtkPt           ][1] =  25.   ;
    chromosRange[mtktkPt           ][0] =   3.   ;
    chromosRange[mtktkPt           ][1] =   6.   ;
    chromosRange[mptkPt            ][0] =   1.0  ;
    chromosRange[mptkPt            ][1] =   3.0  ;
    chromosRange[mntkPt            ][0] =   1.0  ;
    chromosRange[mntkPt            ][1] =   2.0  ;
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

    hData  = new TH1D("hData" , "hData" , h_bins, h_min, h_max );
    hSigMC = new TH1D("hSigMC", "hSigMC", h_bins, h_min, h_max );
    hbkgMC = new TH1D("hBkgMC", "hBkgMC", h_bins, h_min, h_max );
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
    if ( hSigMC->GetEntries() != 0 )
    { printf("histo MC not cleared!\n"); return; }
    if ( hData->GetEntries() != 0 )
    { printf("histo MC not cleared!\n"); return; }



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
            if ( sigMC.readD[readMC::lbtkMass] > h_max ) continue;
            if ( sigMC.readD[readMC::lbtkMass] < h_min ) continue;

            if ( chromos[idx][mlbtkFDSig        ] > sigMC.readD[readMC::lbtkFlightDistanceSig] ) continue;
            if ( chromos[idx][mlbtkVtxprob      ] > sigMC.readD[readMC::lbtkVtxprob          ] ) continue;
            if ( chromos[idx][mlbtkPt           ] > sigMC.readD[readMC::lbtkPt               ] ) continue;
            if ( chromos[idx][mtktkPt           ] > sigMC.readD[readMC::tktkPt               ] ) continue;
            if ( chromos[idx][mptkPt            ] > sigMC.readD[readMC::ptkPt                ] ) continue;
            if ( chromos[idx][mntkPt            ] > sigMC.readD[readMC::ntkPt                ] ) continue;

            //if ( chromos[idx][mptkDEDX_Mom_ratio] > sigMC.readD[readData::ptkDEDX_Harmonic]/sigMC.readD[readData::ptkMom]    ) continue;
            //if ( chromos[idx][MptkDEDX_Mom_ratio] < sigMC.readD[readData::ptkDEDX_Harmonic]/sigMC.readD[readData::ptkMom]    ) continue;
            //if ( chromos[idx][mntkDEDX_Mom_ratio] > sigMC.readD[readData::ntkDEDX_Harmonic]/sigMC.readD[readData::ntkMom]    ) continue;
            //if ( chromos[idx][MntkDEDX_Mom_ratio] < sigMC.readD[readData::ntkDEDX_Harmonic]/sigMC.readD[readData::ntkMom]    ) continue;
            // if ( chromos[idx][mptkIPtSig        ] > sigMC.readD[readData::ptkIPt          ]/sigMC.readD[readData::ptkIPtErr] ) continue;
            // if ( chromos[idx][MptkIPtSig        ] < sigMC.readD[readData::ptkIPt          ]/sigMC.readD[readData::ptkIPtErr] ) continue;
            // if ( chromos[idx][mntkIPtSig        ] > sigMC.readD[readData::ntkIPt          ]/sigMC.readD[readData::ntkIPtErr] ) continue;
            // if ( chromos[idx][MntkIPtSig        ] < sigMC.readD[readData::ntkIPt          ]/sigMC.readD[readData::ntkIPtErr] ) continue;

            double candMass = sigMC.readD[readMC::lbtkMass];
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
            if ( lbData.readD[readData::lbtkMass] > h_max ) continue;
            if ( lbData.readD[readData::lbtkMass] < h_min ) continue;

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
            //if ( chromos[idx][mptkIPtSig        ] > lbData.readD[readData::ptkIPt          ]/lbData.readD[readData::ptkIPtErr] ) continue;
            //if ( chromos[idx][MptkIPtSig        ] < lbData.readD[readData::ptkIPt          ]/lbData.readD[readData::ptkIPtErr] ) continue;
            //if ( chromos[idx][mntkIPtSig        ] > lbData.readD[readData::ntkIPt          ]/lbData.readD[readData::ntkIPtErr] ) continue;
            //if ( chromos[idx][MntkIPtSig        ] < lbData.readD[readData::ntkIPt          ]/lbData.readD[readData::ntkIPtErr] ) continue;

            double candMass = lbData.readD[readData::lbtkMass];
            hData->Fill(candMass);
        }
    } // data part end }}}
    if ( hSigMC->GetEntries() < 300 ) return;
    if ( hData ->GetEntries() < 300 ) return;

    // start fitting
    RooRealVar varMass( "varMass", "#Lambda^{0}_{b} mass(GeV)", h_min, h_max );
    RooDataHist binnedMC("binnedMC", "lbMass in MC", RooArgList(varMass), hSigMC);
    RooDataHist binnedData("binnedData", "lbMass in Data", RooArgList(varMass), hData);

    // fit MC
    RooRealVar par_mean( "mean", "parameter to gaussian: mean", 5.637, h_min, h_max );
    RooRealVar par_width("width","parameter to gaussian: width",0.01, 0.000001, 10. );
    RooGaussian pdf_gaus("gaus", "PDF : gaussian", varMass, par_mean, par_width );
    RooRealVar par_mean2( "mean2", "parameter to gaussian: mean", 5.6142, h_min, h_max );
    RooRealVar par_width2("width2","parameter to gaussian: width", 0.0342, 0.000001, 10. );
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

    double constr_mean  = par_mean .getVal();
    double constr_width = par_width.getVal();
    // set mean +- 3 sigma to be signal region, and exclude mean +- 5 sigma to be background region
    varMass.setRange("sidebandRegion1", h_min, constr_mean - 5.*constr_width);
    varMass.setRange("sidebandRegion2", constr_mean + 5.*constr_width, h_max);
    varMass.setRange("signalRegion", constr_mean - 3.*constr_width, constr_mean + 3.*constr_width);

    RooRealVar par_ch1("ch1", "parameter to chebychev : 1st order",  0.5, -10., 10. );
    RooRealVar par_ch2("ch2", "parameter to chebychev : 2nd order", -0.2, -10., 10. );
    RooRealVar par_ch3("ch3", "parameter to chebychev : 3rd order", -0.2, -10., 10. );
    RooRealVar par_ch4("ch4", "parameter to chebychev : 4th order", -0.2, -10., 10. );
    RooChebychev pdf_cheb("cheb", "PDF : chebychev", varMass, RooArgSet(par_ch1, par_ch2) );

    pdf_cheb.fitTo(binnedData);
    RooPlot* tmpFrame = varMass.frame(RooFit::Title("d"));
    binnedData.plotOn(tmpFrame, RooFit::Name("a"));
    pdf_cheb.plotOn( tmpFrame , RooFit::Name("b"));
    double tmpChi2 = tmpFrame->chiSquare("b", "a");
    if ( tmpChi2 >  100. ) return;
    // fit Data with MC result
    double maxevent = (double)hData->GetEntries();
    RooRealVar ns( "nSig", "number of signal", maxevent/1000., 0., maxevent );
    RooRealVar nb( "nBkg", "number of background", maxevent, 0., maxevent);
    RooAddPdf totModel( "totModel", "PDF : data + MC", RooArgList(mcModel, pdf_cheb), RooArgList(ns,nb));
    totModel.fitTo(binnedData);



    //RooAbsReal* mcIntegral   = pdf_gaus.createIntegral( varMass, RooFit::NormSet(varMass), RooFit::Range("signalRegion") );
    RooAbsReal* mcIntegral   = mcModel.createIntegral( varMass, RooFit::NormSet(varMass), RooFit::Range("signalRegion") );
    RooAbsReal* dataIntegral = pdf_cheb.createIntegral( varMass, RooFit::NormSet(varMass), RooFit::Range("signalRegion") );
    double nsig =   mcIntegral->getVal() * ns.getVal();
    double nbkg = dataIntegral->getVal() * nb.getVal();
    fitness = nsig / sqrt(nsig+nbkg);

    // temperary be used to represent background fit chi2
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
void GeneticAlgorithmMC_LbTk::setSignalRegion( const double mean, const double width )
{
    mWindow = mean - width;
    MWindow = mean + width;
    mSig = mean - 0.5 * width;
    MSig = mean + 0.5 * width;
    return;
}
void GeneticAlgorithmMC_LbTk::setHistoRange( const double m, const double M )
{
    h_min = m;
    h_max = M;
    return;
}
void GeneticAlgorithmMC_LbTk::setHistoNBins( const int n )
{ h_bins = n; return; }
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
