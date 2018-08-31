#ifndef __rooTHMainpnLbTk_h__
#define __rooTHMainpnLbTk_h__
#include "histProduce/histProduce/interface/rooTHMain.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include <vector>
#include <utility> // std::pair

class root_TreeHistoMain_plusminus_LbTk : public root_TreeHistoMain
{
public:
    root_TreeHistoMain_plusminus_LbTk (TFileDirectory* d);
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
        plbtkMass, plbtkFlightDistance2d, plbtkFlightDistanceSig, plbtkVtxprob, plbtkCosa2d,
        ptargetJpsiP_mass, ptargetJpsiP_pt,
        ptargetJpsiPBar_mass, ptargetJpsiPBar_pt,
        plbtkPt, plbtkMom,
        ptktkPt, ptktkMom,
        pfake_Lam0Mass, pfake_KstarMass, pfake_PhiMass, pfake_KshortMass,
        pfake_LbL0Mass, pfake_BdMass   , pfake_BsMass, pfake_mumupipiMass,
        pptonPt, pptonMom, pptonDEDX_Harmonic, pptonDEDX_pixelHrm, pptonIPt, pptonIPtErr,
        pkaonPt, pkaonMom, pkaonDEDX_Harmonic, pkaonDEDX_pixelHrm, pkaonIPt, pkaonIPtErr,

        nlbtkMass, nlbtkFlightDistance2d, nlbtkFlightDistanceSig, nlbtkVtxprob, nlbtkCosa2d,
        ntargetJpsiP_mass, ntargetJpsiP_pt,
        ntargetJpsiPBar_mass, ntargetJpsiPBar_pt,
        nlbtkPt, nlbtkMom,
        ntktkPt, ntktkMom,
        nfake_Lam0Mass, nfake_KstarMass, nfake_PhiMass, nfake_KshortMass,
        nfake_LbL0Mass, nfake_BdMass   , nfake_BsMass, nfake_mumupipiMass,
        nptonPt, nptonMom, nptonDEDX_Harmonic, nptonDEDX_pixelHrm, nptonIPt, nptonIPtErr,
        nkaonPt, nkaonMom, nkaonDEDX_Harmonic, nkaonDEDX_pixelHrm, nkaonIPt, nkaonIPtErr,
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
