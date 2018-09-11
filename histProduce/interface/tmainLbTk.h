#ifndef __tMainLbTk_h__
#define __tMainLbTk_h__
#include "histProduce/histProduce/interface/tmain.h"
#include "histProduce/histProduce/interface/formatTreeLbTk.h"

class treeMain_LbTk : public treeMain, public formatTree_LbTk
{
public:
    treeMain_LbTk ( TFileDirectory* d);
    virtual void Process( fwlite::Event* ev ) override;

    // automatically called by constructor.
    virtual void RegTree() override;
    virtual void GetByLabel( fwlite::Event* ev ) override;

    // used for final analysis
    void setBranchAddress( TTree* inputTree );

    friend class treeCreatingSpecificDecay;
private:
    const double kaonMass, protonMass, pionMass;
    void getByLabel_BS( fwlite::Event* ev );
    fwlite::Handle< reco::BeamSpot > beamSpotHandle;

    TH1D* hParEta  ;
    TH1D* hParCos2D;
    TH1D* hParCos3D;
    TH1D* hParCos2D_2;
    TH1D* hParCos2D_3;
public:
    // if it is needed to add addiditional branch to new tree. added in this
    enum addVarD
    {
        originD = comNumD-1,
        totNumD
    };
    enum addVarI
    {
        originI = comNumI-1,
        //ptkPID,
        //ntkPID,
        totNumI
    };

};
#endif
