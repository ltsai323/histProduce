#include "histProduce/histProduce/interface/GeneticAlgorithm_LbTk.h"
#include "histProduce/histProduce/interface/rooTHMainLbTk.h"

#include "TRandom3.h"
#include "TTree.h"

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


GeneticAlgorithm_LbTk::GeneticAlgorithm_LbTk( unsigned Nchromo = 10000, unsigned maxGen = 1000 ) :
    GeneticAlgorithm( NVAR, Nchromo, maxGen ), lbtk(nullptr)
{
    setSignalRegion( 5.6195, 0.08 );
    chromosRange = new double*[NVAR];
    for ( int i=0; i<NVAR; ++i )
        chromosRange[i] = new double[2];
    chromosRange[mlbtkFDSig        ][0] =   0.111;
    chromosRange[mlbtkFDSig        ][1] =  10.   ;
    chromosRange[mlbtkVtxprob      ][0] =   0.13 ;
    chromosRange[mlbtkVtxprob      ][1] =   0.4  ;
    chromosRange[mlbtkPt           ][0] =  10.   ;
    chromosRange[mlbtkPt           ][1] =  25.   ;
    chromosRange[mlbtkMom          ][0] =  10.   ;
    chromosRange[mlbtkMom          ][1] =  25.   ;
    chromosRange[mtktkPt           ][0] =   3.   ;
    chromosRange[mtktkPt           ][1] =   6.   ;
    chromosRange[mtktkMom          ][0] =   3.   ;
    chromosRange[mtktkMom          ][1] =   6.   ;
    chromosRange[mptkPt            ][0] =   2.0  ;
    chromosRange[mptkPt            ][1] =   4.0  ;
    chromosRange[mptkMom           ][0] =   2.0  ;
    chromosRange[mptkMom           ][1] =   4.0  ;
    chromosRange[mntkPt            ][0] =   1.0  ;
    chromosRange[mntkPt            ][1] =   3.0  ;
    chromosRange[mntkMom           ][0] =   1.0  ;
    chromosRange[mntkMom           ][1] =   3.0  ;
    chromosRange[mptkDEDX_Mom_ratio][0] =   0.   ;
    chromosRange[mptkDEDX_Mom_ratio][1] =   1.   ;
    chromosRange[MptkDEDX_Mom_ratio][0] =  10.   ;
    chromosRange[MptkDEDX_Mom_ratio][1] =  20.   ;
    chromosRange[mntkDEDX_Mom_ratio][0] =   0.   ;
    chromosRange[mntkDEDX_Mom_ratio][1] =   1.   ;
    chromosRange[MntkDEDX_Mom_ratio][0] =  10.   ;
    chromosRange[MntkDEDX_Mom_ratio][1] =  20.   ;
    chromosRange[mptkIPtSig        ][0] = -10.   ;
    chromosRange[mptkIPtSig        ][1] =   0.   ;
    chromosRange[MptkIPtSig        ][0] =   0.   ;
    chromosRange[MptkIPtSig        ][1] =  10.   ;
    chromosRange[mntkIPtSig        ][0] = -10.   ;
    chromosRange[mntkIPtSig        ][1] =   0.   ;
    chromosRange[MntkIPtSig        ][0] =   0.   ;
    chromosRange[MntkIPtSig        ][1] =  10.   ;
    return;
}
GeneticAlgorithm_LbTk::~GeneticAlgorithm_LbTk()
{
    int idx = NVAR;
    while ( idx-- )
        delete chromosRange[idx];
    delete chromosRange;
    chromosRange = nullptr;
    return;
}
void GeneticAlgorithm_LbTk::SetData( TFile* file )
{
    lbtk.SetInputFile( file );
    setNEntries( lbtk.readTree()->GetEntries() );
    return;
}
void GeneticAlgorithm_LbTk::PrepareChromosomes()
{
    TRandom3* rnd = getRnd();
    unsigned nchromos = getNChromos();
    for ( unsigned i = 0; i < nchromos; ++i )
        for ( unsigned j = 0; j < NVAR; ++j )
            chromos[i][j] = rnd->Uniform( chromosRange[j][0], chromosRange[j][1] );
}
inline void GeneticAlgorithm_LbTk::CalculateFitness( const unsigned idx, double& fitness, double& fitnessErr )
{
    TTree* tree = lbtk.readTree();
    unsigned iEntry = 0;
    unsigned NEntries = getNEntries();
    //unsigned NEntries = 10000;

    unsigned nsig = 0;
    unsigned nbkg = 0;
    while ( iEntry != NEntries )
    {
        tree->GetEntry( iEntry++ );

        if ( chromos[idx][mlbtkFDSig        ] > lbtk.readD[readData::lbtkFlightDistanceSig] ) continue;
        if ( chromos[idx][mlbtkVtxprob      ] > lbtk.readD[readData::lbtkVtxprob          ] ) continue;
        if ( chromos[idx][mlbtkPt           ] > lbtk.readD[readData::lbtkPt               ] ) continue;
        if ( chromos[idx][mlbtkMom          ] > lbtk.readD[readData::lbtkMom              ] ) continue;
        if ( chromos[idx][mtktkPt           ] > lbtk.readD[readData::tktkPt               ] ) continue;
        if ( chromos[idx][mtktkMom          ] > lbtk.readD[readData::tktkMom              ] ) continue;
        if ( chromos[idx][mptkPt            ] > lbtk.readD[readData::ptkPt                ] ) continue;
        if ( chromos[idx][mptkMom           ] > lbtk.readD[readData::ptkMom               ] ) continue;
        if ( chromos[idx][mntkPt            ] > lbtk.readD[readData::ntkPt                ] ) continue;
        if ( chromos[idx][mntkMom           ] > lbtk.readD[readData::ntkMom               ] ) continue;

        //if ( chromos[idx][mptkDEDX_Mom_ratio] > lbtk.readD[readData::ptkDEDX_Harmonic]/lbtk.readD[readData::ptkMom]    ) continue;
        //if ( chromos[idx][MptkDEDX_Mom_ratio] < lbtk.readD[readData::ptkDEDX_Harmonic]/lbtk.readD[readData::ptkMom]    ) continue;
        //if ( chromos[idx][mntkDEDX_Mom_ratio] > lbtk.readD[readData::ntkDEDX_Harmonic]/lbtk.readD[readData::ntkMom]    ) continue;
        //if ( chromos[idx][MntkDEDX_Mom_ratio] < lbtk.readD[readData::ntkDEDX_Harmonic]/lbtk.readD[readData::ntkMom]    ) continue;
        if ( chromos[idx][mptkIPtSig        ] > lbtk.readD[readData::ptkIPt          ]/lbtk.readD[readData::ptkIPtErr] ) continue;
        if ( chromos[idx][MptkIPtSig        ] < lbtk.readD[readData::ptkIPt          ]/lbtk.readD[readData::ptkIPtErr] ) continue;
        if ( chromos[idx][mntkIPtSig        ] > lbtk.readD[readData::ntkIPt          ]/lbtk.readD[readData::ntkIPtErr] ) continue;
        if ( chromos[idx][MntkIPtSig        ] < lbtk.readD[readData::ntkIPt          ]/lbtk.readD[readData::ntkIPtErr] ) continue;

        double candMass = lbtk.readD[readData::lbtkMass];
        if ( !inCalculationRegion(candMass) ) continue;
        if ( inSignalRegion(candMass) ) ++nsig;
        else                            ++nbkg;
    }
    if ( nsig == 0 || nsig<nbkg )
    {
        fitness = 0.;
        fitnessErr = 0.;
        return;
    }
    fitness = double( nsig-nbkg ) / sqrt( double(nsig) );
    fitnessErr = 0.;
    return;
}




bool GeneticAlgorithm_LbTk::BasicCheck()
{
    printf( "Basic Checking...\n" );
    if ( !lbtk.readTree() )
    { printf( "\tdataTree not load!\n" ); return false; }
    if ( !lbtk.readTree()->GetEntries() )
    { printf( "\tthere is no entries in the tree!\n" ); return false; }
    printf( "\tLbTk part checking : Done!\n" );

    return true;
}
void GeneticAlgorithm_LbTk::setSignalRegion( double mean, double width )
{
    mWindow = mean - width;
    MWindow = mean + width;
    mSig = mean - 0.5 * width;
    MSig = mean + 0.5 * width;
    return;
}
bool GeneticAlgorithm_LbTk::inSignalRegion( double mass ) const
{
    if ( mSig > mass ) return false;
    if ( MSig < mass ) return false;
    return true;
}
bool GeneticAlgorithm_LbTk::inCalculationRegion( double mass ) const
{
    if ( mWindow > mass ) return false;
    if ( MWindow < mass ) return false;
    return true;
}
