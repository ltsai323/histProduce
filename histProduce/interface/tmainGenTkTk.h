#ifndef __tMainGenTkTk_h__
#define __tMainGenTkTk_h__
#include "histProduce/histProduce/interface/tmainGen.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include <vector>
#include <utility> // std::pair

class treeMainGen_TkTk : public treeMainGen
{
public:
    treeMainGen_TkTk ( TFileDirectory* d);
    virtual void Process( fwlite::Event* ev ) override;
    virtual void Clear() override;
    virtual void RegTree() override;
    virtual void GetByLabel( fwlite::Event* ev ) override;
private:
    void getByLabel_BS( fwlite::Event* ev );
    fwlite::Handle< std::vector<reco::GenParticle> > genHandle;
    fwlite::Handle< reco::BeamSpot > beamSpotHandle;
    // store the result of 'particle-MC genParticle pair'
    //std::vector< std::pair<const void*, const reco::GenParticle*> > pgPair;
    const double protonMass;
    const double   pionMass;


    enum dataVarD
    {
        tktkMass, tktkFlightDistance2d, tktkCosa2d,tktkVtxprob, tktkPt, tktkSortingNumber,
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
