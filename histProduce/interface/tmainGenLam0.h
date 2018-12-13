#ifndef __tMainGenLam0_h__
#define __tMainGenLam0_h__
#include "histProduce/histProduce/interface/tmainGen.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include <vector>
#include <utility> // std::pair

class treeMainGen_Lam0 : public treeMainGen
{
public:
    treeMainGen_Lam0 ( TFileDirectory* d);
    virtual void Process( fwlite::Event* ev ) override;
    virtual void Clear();
    virtual void RegTree() override;
    virtual void GetByLabel( fwlite::Event* ev ) override;
private:
    void getByLabel_BS( fwlite::Event* ev );
    fwlite::Handle< reco::BeamSpot > beamSpotHandle;

    const double protonMass;
    const double   pionMass;

    enum dataVarD
    {
        lam0Mass, lam0FlightDistance2d, lam0Cosa2d,lam0Vtxprob, lam0Pt, lam0SortingNumber,
        ptkMom, ptkDEDX_Harmonic, ptkDEDX_pixelHrm, ptkIPt,
        ntkMom, ntkDEDX_Harmonic, ntkDEDX_pixelHrm, ntkIPt,
        totNumD
    };
    enum dataVarI
    {
        ptkPID,
        ntkPID,
        totNumI
    };
    // double data to store variables
    double dataD[totNumD];
    // int data to store variables
    int dataI[totNumI];
};


#endif
