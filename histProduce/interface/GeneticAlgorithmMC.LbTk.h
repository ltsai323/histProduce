#ifndef __GA_MC_LbTk_H__
#define __GA_MC_LbTk_H__
#include "histProduce/histProduce/interface/GeneticAlgorithmMC.h"
#include "histProduce/histProduce/interface/rooTHMainLbTk.h"
#include "histProduce/histProduce/interface/rooTHMainGenLbTk.h"
#include "TH1D.h"

typedef root_TreeHistoMain_LbTk readData;
typedef root_TreeHistoMain_GenInfo_LbTk readMC;
class GeneticAlgorithmMC_LbTk : public GeneticAlgorithmMC
{
public:
    GeneticAlgorithmMC_LbTk( const unsigned Nchromo, const unsigned maxGen );
    ~GeneticAlgorithmMC_LbTk();

    virtual void SetData( TFile* file ) override;
    virtual void SetSignalMC( TFile* file ) override;
    virtual void SetBackgroundMC( TFile* file ) override;
    virtual void PrepareChromosomes() override;
    virtual void CalculateFitness( const unsigned idx, double& fitness, double& fitnessErr ) override;
    virtual bool BasicCheck() override;

    // signal region: mean +- 0.5width
    void setSignalRegion( const double mean, const double width );
    bool inSignalRegion( const double mass ) const;
    bool inCalculationRegion( const double mass ) const;
private:
    readData lbData;
    readMC sigMC, bkgMC;
    double** chromosRange;
    double mWindow, MWindow;
    double mSig, MSig;
    TH1D* hData;
    TH1D* hSigMC;
    TH1D* hbkgMC;
};
#endif
