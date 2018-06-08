#ifndef __tMainGenLbTk_h__
#define __tMainGenLbTk_h__
#include "histProduce/histProduce/interface/tmainGen.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include <vector>
#include <utility> // std::pair
#include "TH2D.h"

class treeMainGen_LbTk : public treeMainGen
{
public:
    treeMainGen_LbTk ( TFileDirectory* d);
    virtual void Process( fwlite::Event* ev ) override;
    virtual void Clear() override;
    // automatically called by constructor.
    virtual void RegTree() override;
    virtual void GetByLabel( fwlite::Event* ev ) override;

    // used for final analysis
    void setBranchAddress( TTree* inputTree );
private:
    const double kaonMass, protonMass, pionMass;
    void getByLabel_BS( fwlite::Event* ev );
    fwlite::Handle< std::vector<reco::GenParticle> > genHandle;
    fwlite::Handle< reco::BeamSpot > beamSpotHandle;
    TH2D* hSummary;
    enum dataVarD
    {
        lbtkMass, lbtkFlightDistance2d, lbtkFlightDistanceSig, lbtkVtxprob,
        lbtkMom, lbtkPt,
        tktkPt, tktkMom,
        fake_Lam0Mass, fake_KstarMass, fake_PhiMass, fake_KshortMass,
        fake_LbL0Mass, fake_BdMass   , fake_BsMass, fake_mumupipiMass,
        ptkPt, ptkMom, ptkDEDX_Harmonic, ptkDEDX_pixelHrm, ptkIPt, ptkIPtErr,
        ntkPt, ntkMom, ntkDEDX_Harmonic, ntkDEDX_pixelHrm, ntkIPt, ntkIPtErr,
        totNumD
    };
    enum dataVarI
    {
        ptkPID, ptkMomPID,
        ntkPID, ntkMomPID,
        totNumI
    };
    // double data to store variables
    double dataD[totNumD];
    // int data to store variables
    int dataI[totNumI];

};


#endif
