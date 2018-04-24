#include "histProduce/histProduce/interface/GeneticAlgorithm.h"
#include <stdio.h>
#include <stdlib.h>

inline void GeneticAlgorithm::ApplyMutation()
{
    TRandom3* rndNum = getRnd();
    for(unsigned i=nchromos;i<nchromos*2;i++) {
        for(unsigned j=0;j<18;j++) {
            if (rndNum->Uniform()<0.10)
                chromos[i][j] += rndNum->Gaus(0.,0.5);
        }
    }
}
inline void GeneticAlgorithm::ApplyCrossOver()
{
    TRandom3* rndNum = getRnd();
    for ( unsigned i = nchromos; i < nchromos*2; ++i )
    {
        int s1 = (int)rndNum->Uniform(0., nchromos);
        int s2 = (int)rndNum->Uniform(0., nchromos);

        for ( unsigned j = 0; j < nvariables; ++j )
            if ( rnd.Uniform() < 0.5 )
                chromos[i][j] = chromos[s1][j];
            else
                chromos[i][j] = chromos[s2][j];
    }
}
void GeneticAlgorithm::Evolution()
{
    std::cout << "Evolution: 01\n";
    if ( !BasicCheck() )
    { printf( "error: something error in each branch!.\n" ); exit(1); }
    std::cout << "Evolution: 02\n";

    PrepareChromosomes();
    std::cout << "Evolution: 03\n";
    for(unsigned i=0;i<nchromos;i++)
        CalculateFitness(i, fitness[i], fitnessErr[i] );
    std::cout << "Evolution: 04\n";

    // Evolution loop
    for ( unsigned gen = 0; gen < maxGen; ++gen )
    {
        ApplyCrossOver();
        ApplyMutation();

        for ( unsigned i = nchromos; i < nchromos*2; ++i )
            CalculateFitness(i, fitness[i], fitnessErr[i] );
        // Sort by fitness
        for ( unsigned i=0; i < nchromos*2; ++i )
        {
            unsigned idx = i;
            for ( unsigned j = i+1; j < nchromos*2; ++j )
                if ( fitness[j] > fitness[idx] )
                    idx = j;

            double tmpNum = fitness[idx];
            fitness[idx] = fitness[i];
            fitness[i] = tmpNum;
            for ( unsigned k = 0; k < nvariables; ++k )
            {
                tmpNum = chromos[idx][k];
                chromos[idx][k] = chromos[i][k];
                chromos[i][k] = tmpNum;
            }
        }

        // calculate result
        double f_avg = 0.;
        double f_rms = 0.;
        for ( unsigned i=0; i<nchromos; ++i )
            f_avg += fitness[i];
        f_avg /= double(nchromos);
        for ( unsigned i=0; i<nchromos; ++i )
            f_rms += (fitness[i]-f_avg)*(fitness[i]-f_avg);
        f_rms /= double(nchromos);

        PrintSummary( f_avg, f_rms, gen, false );


        if (f_rms<f_avg*0.00001) break;
    }
    std::cout << "Evolution: 05\n";
}
void GeneticAlgorithm::SetLogFile( const char* txt )
{ logFile = fopen( txt, "w" ); return; }
inline void GeneticAlgorithm::PrintSummary( const double avg, const double rms, const unsigned gen, const bool printToScreen ) const
{
    if ( printToScreen )
    {
        // print Evolution
        printf( "---------------------- %5d generation -----------------------\n", gen+1 );
        printf( "Avergefitness = %.4f +- %.4f\n", avg, rms );
        for ( int i = 0; i < 10; ++i )
        {
            printf ( "%2d: fitness = %.4f +- %.4f, choromos : ", i, fitness[i], fitnessErr[i] );
            for ( unsigned j = 0; j < nvariables; ++j )
                printf ( "%.2f ", chromos[i][j] );
            printf( "\n" );
        }
    }
    else
    {
        if ( !logFile ) return;
        // print Evolution
        fprintf( logFile, "---------------------- %5d generation -----------------------\n", gen+1 );
        fprintf( logFile, "Avergefitness = %.4f +- %.4f\n", avg, rms );
        for ( int i = 0; i < 10; ++i )
        {
            fprintf ( logFile, "%2d: fitness = %.4f +- %.4f, choromos : ", i, fitness[i], fitnessErr[i] );
            for ( unsigned j = 0; j < nvariables; ++j )
                fprintf ( logFile, "%.2f ", chromos[i][j] );
            fprintf( logFile, "\n" );
        }
        fflush( logFile );
    }
    return;
}
