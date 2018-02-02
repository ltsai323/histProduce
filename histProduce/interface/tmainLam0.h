#ifndef __tMainLam0_h__
#define __tMainLam0_h__
#include "histProduce/histProduce/interface/tmain.h"

class treeMain_Lam0 : public treeMain
{
public:
    treeMain_Lam0 ( TFileDirectory* d);
    virtual void Process( fwlite::Event* ev ) override;
    virtual void Clear() override;

    // automatically called by constructor.
    virtual void RegTree() override;
    virtual void GetByLabel( fwlite::Event* ev ) override;
private:
    void getByLabel_BS( fwlite::Event* ev );
    fwlite::Handle< reco::BeamSpot > beamSpotHandle;
    enum dataVarD
    {
        lam0Mass, lam0FlightDistance2d, lam0Cosa2d, lam0Pt,
        ptkMom, ptkDEDX_Harmonic, ptkDEDX_pixelHrm, ptkIPt,
        ntkMom, ntkDEDX_Harmonic, ntkDEDX_pixelHrm, ntkIPt,
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

    //double lam0Mass;
    //double lam0FlightDistance2d, lam0Cosa2d;
    //double ptkMom, ptkDEDX_Harmonic, ptkDEDX_pixelHrm, ptkIPt;
    //double ntkMom, ntkDEDX_Harmonic, ntkDEDX_pixelHrm, ntkIPt;

};




#endif
