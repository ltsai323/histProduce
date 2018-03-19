#ifndef __tMainTkTk_h__
#define __tMainTkTk_h__
#include "histProduce/histProduce/interface/tmain.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include <vector>
#include <utility> // std::pair

class treeMain_TkTk : public treeMain
{
public:
    treeMain_TkTk ( TFileDirectory* d);
    virtual void Process( fwlite::Event* ev ) override;
    virtual void Clear() override;
    virtual void RegTree() override;
    virtual void GetByLabel( fwlite::Event* ev ) override;
private:
    const double kaonMass, protonMass, pionMass;
    void getByLabel_BS( fwlite::Event* ev );
    fwlite::Handle< reco::BeamSpot > beamSpotHandle;
    // store the result of 'particle-MC genParticle pair'
    //std::vector< std::pair<const void*, const reco::GenParticle*> > pgPair;


    enum dataVarD
    {
        tktkMass, tktkFlightDistance2d, tktkVtxprob, tktkPt,
        fake_Lam0Mass, fake_KstarMass, fake_KshortMass, fake_PhiMass,
        ptkMom, ptkDEDX_Harmonic, ptkDEDX_pixelHrm, ptkIPt, ptkIPtErr,
        ntkMom, ntkDEDX_Harmonic, ntkDEDX_pixelHrm, ntkIPt, ntkIPtErr,
        totNumD
    };
    enum dataVarI
    {
        //ptkPID,
        //ntkPID,
        totNumI
    };
    // double data to store variables
    double dataD[totNumD];
    // int data to store variables
    int dataI[totNumI];
};


#endif
