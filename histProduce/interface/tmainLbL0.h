#ifndef __tMainLbL0_h__
#define __tMainLbL0_h__
#include "histProduce/histProduce/interface/tmain.h"

class treeMain_LbL0 : public treeMain
{
public:
    treeMain_LbL0 ( TFileDirectory* d);
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
        lbl0Mass, lbl0FlightDistance2d, lbl0Cosa2d,
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
