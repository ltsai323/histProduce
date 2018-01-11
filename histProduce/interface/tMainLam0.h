#ifndef __tMainLam0_h__
#define __tMainLam0_h__
#include "histProduce/histProduce/interface/tMain.h"

class treeMain_Lam0 : public treeMain
{
public:
    treeMain_Lam0 ( TFileDirectory* d);
    virtual void Process( fwlite::Event* ev ) override;
    virtual void Clear() override;

    // automatically called by constructor.
    virtual void regTree() override;
private:
    double lam0Mass;
    double lam0FlightDistance2d, lam0Cosa2d;
    double ptkMom, ptkDEDX_Harmonic, ptkDEDX_pixelHrm, ptkIPt;
    double ntkMom, ntkDEDX_Harmonic, ntkDEDX_pixelHrm, ntkIPt;

};




#endif
