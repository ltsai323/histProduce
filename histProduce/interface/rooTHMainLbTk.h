#ifndef __rooTHMainLbTk_h__
#define __rooTHMainLbTk_h__
#include "histProduce/histProduce/interface/rooTHMain.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include <vector>
#include <utility> // std::pair

class root_TreeHistoMain_LbTk : public root_TreeHistoMain
{
public:
    root_TreeHistoMain_LbTk (TFileDirectory* d);
    virtual void Process( unsigned int pIdx ) override;
    virtual void Clear() override;
    virtual void RegTree() override;
    virtual void RegHisto() override;
    virtual void LoadSourceBranch() override;
    virtual void SummaryCalc() override;
private:
    const double kaonMass, protonMass, pionMass;

public:
    // tree structure in loaded data. And create new file also with the same structure
    enum readVarD
    {
        lbtkMass, lbtkFlightDistance2d, lbtkFlightDistanceSig, lbtkVtxprob, lbtkCosa2d,
        targetJpsiP_mass, targetJpsiP_pt,
        targetJpsiPBar_mass, targetJpsiPBar_pt,
        lbtkPt, lbtkMom,
        tktkPt, tktkMom,
        fake_Lam0Mass, fake_KstarMass, fake_PhiMass, fake_KshortMass,
        fake_LbL0Mass, fake_BdMass   , fake_BsMass, fake_mumupipiMass,
        ptkPt, ptkMom, ptkDEDX_Harmonic, ptkDEDX_pixelHrm, ptkIPt, ptkIPtErr,
        ntkPt, ntkMom, ntkDEDX_Harmonic, ntkDEDX_pixelHrm, ntkIPt, ntkIPtErr,
        totNumD
    };
    enum readVarI
    {
        totNumI
    };

    double readD[totNumD];
    int readI[totNumI];

private:
    // if it is needed to add addiditional branch to new tree. added in this
    enum addVarD
    {
        addNumD
    };
    enum addVarI
    {
        //ptkPID,
        //ntkPID,
        addNumI
    };

    // double data to store variables
    double dataD[totNumD+addNumD];
    // int data to store variables
    int dataI[totNumI+addNumI];
    // store result of genetic algorithm, the values are written in constructor
    std::vector<double> gaRes;
};


#endif
