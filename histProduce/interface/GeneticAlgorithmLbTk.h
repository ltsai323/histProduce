#ifndef __GA_LbTk_H__
#define __GA_LbTk_H__
#include "histProduce/histProduce/interface/GeneticAlgorithm.h"
#include "histProduce/histProduce/interface/rooTHMainLbTk.h"

typedef root_TreeHistoMain_LbTk readData;
class GeneticAlgorithm_LbTk : public GeneticAlgorithm
{
public:
    GeneticAlgorithm_LbTk( const unsigned Nchromo, const unsigned maxGen );
    ~GeneticAlgorithm_LbTk();

    virtual void SetData( TFile* file ) override;
    virtual void PrepareChromosomes() override;
    virtual void CalculateFitness( const unsigned idx, double& fitness, double& fitnessErr ) override;
    virtual bool BasicCheck() override;

    // signal region: mean +- 0.5width
    void setSignalRegion( const double mean, const double width );
    bool inSignalRegion( const double mass ) const;
    bool inCalculationRegion( const double mass ) const;
private:
    readData lbtk;
    double** chromosRange;
    double mWindow, MWindow;
    double mSig, MSig;
};
#endif
