#ifndef __tMainGenLbTk_h__
#define __tMainGenLbTk_h__
#include "histProduce/histProduce/interface/tmainGen.h"
#include "histProduce/histProduce/interface/formatTreeLbTk.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include <vector>
#include <utility> // std::pair
#include "TH2D.h"

class treeMainGen_LbTk : public treeMainGen, public formatTree_LbTk
{
public:
    treeMainGen_LbTk ( TFileDirectory* d);
    virtual void Process( fwlite::Event* ev ) override;
    // automatically called by constructor.
    virtual void RegTree() override;
    virtual void GetByLabel( fwlite::Event* ev ) override;

    // used for final analysis
    void setBranchAddress( TTree* inputTree );

    // get the angle of cand-PV to PV-BS.
private:
    const double kaonMass, protonMass, pionMass;
    void getByLabel_BS( fwlite::Event* ev );
    void getByLabel_PV( fwlite::Event* ev );
    fwlite::Handle< std::vector<reco::GenParticle> > genHandle;
    fwlite::Handle< reco::BeamSpot > beamSpotHandle;
    fwlite::Handle< std::vector<reco::Vertex> > primaryVHandle;
    TH2D* hSummary;
    TH1D* hParEta  ;
    TH1D* hParCos2D;
    TH1D* hParCos2D_2;
    TH1D* hParCos2D_3;
    TH1D* hParCos3D;


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
        ptkPID, ptkMomPID,
        ntkPID, ntkMomPID,
        totNumI
    };

};


#endif
