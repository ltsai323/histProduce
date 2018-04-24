#ifndef __GA_H__
#define __GA_H__

#include <iostream>
#include <stdio.h>
#include <string.h>

#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"


class GeneticAlgorithm
{
public:
    GeneticAlgorithm( const unsigned Nvars, const unsigned NChromo=10000, const unsigned MaxGen=1000 ) : nvariables(Nvars), Nentries(0), nchromos(NChromo), maxGen(MaxGen), rnd(1234), logFile( nullptr )
    {
        chromos = new double*[nchromos*2];
        for ( unsigned i=0; i<nchromos*2; ++i )
            chromos[i] = new double[nvariables];
        fitness = new double[nchromos*2];
        fitnessErr = new double[nchromos*2];
        return;
    }
    virtual ~GeneticAlgorithm()
    {
        if ( !chromos && !fitness ) return;
        for ( unsigned i=0; i<nchromos*2; ++i )
            delete chromos[i];
        delete fitness;
        delete fitnessErr;
        delete chromos;
        fitness=nullptr;
        chromos=nullptr;
        nchromos = 0;
        return;
    }

    // needed to be inherited
    virtual void PrepareChromosomes() = 0;
    virtual void CalculateFitness( const unsigned idx, double& fitness, double& fitnessErr ) = 0;
    virtual bool BasicCheck() = 0;
    // main function of GA
    virtual void SetData( TFile* file ) = 0;
    void SetLogFile( const char* txt );
    void Evolution();


    void ApplyMutation();
    void ApplyCrossOver();

    TRandom3* getRnd() { return &rnd; }
    unsigned getNChromos() const { return nchromos; }
    void setNEntries( const unsigned N ) { Nentries = N; return; }
    unsigned getNEntries() const { return Nentries; }
    void PrintSummary( const double avg, const double rms, const unsigned gen, const bool printToScreen ) const;

    double* fitness;
    double* fitnessErr;
    double** chromos;
private:
    unsigned nvariables;
    unsigned Nentries;
    unsigned nchromos, maxGen;
    TRandom3 rnd;
    FILE* logFile;
};


#endif
