#ifndef __GA_MC_H__
#define __GA_MC_H__
// this file is to control Genetic algorithm works data with MC.
// it is used to data and signal MC.
#include "histProduce/histProduce/interface/GeneticAlgorithm.h"

class GeneticAlgorithmMC : public GeneticAlgorithm
{
public:
    GeneticAlgorithmMC( const unsigned Nvars, const unsigned NChromo=10000, const unsigned MaxGen=1000 ) : GeneticAlgorithm(Nvars,NChromo,MaxGen)
    {
    }
    virtual ~GeneticAlgorithmMC()
    {
    }

    // needed to be inherited
    virtual void PrepareChromosomes() override = 0;
    virtual void CalculateFitness( const unsigned idx, double& fitness, double& fitnessErr ) override = 0;
    virtual bool BasicCheck() override = 0;
    // main function of GA
    virtual void SetData( TFile* file ) override = 0;
    virtual void SetSignalMC( TFile* file ) = 0;
    virtual void SetBackgroundMC( TFile* file ) = 0;
};
#endif
