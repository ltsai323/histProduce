#ifndef __tMainpnLbTk_h__
#define __tMainpnLbTk_h__
#include "histProduce/histProduce/interface/tmain.h"
#include "histProduce/histProduce/interface/formatTreepnLbTk.h"
#include <math.h>

class treeMain_pnLbTk : public treeMain, public formatTree_plusminus_LbTk
{
public:
    treeMain_pnLbTk ( TFileDirectory* d);
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
    void getByLabel_antiCand( fwlite::Event* ev );
    fwlite::Handle< reco::BeamSpot > beamSpotHandle;
    fwlite::Handle< std::vector<pat::CompositeCandidate> > antiCandHandle;
    treeMain::Label antiCandLabel;
public:
    std::vector< std::pair< std::pair<double,const pat::CompositeCandidate*>, std::pair<double,const pat::CompositeCandidate*> > > pairpnCands
    ( std::vector<std::pair<double,const pat::CompositeCandidate*> >& pCands, std::vector<std::pair<double, const pat::CompositeCandidate*> >& nCands );
    double deltaRCalc;
    bool sameVal( double val1, double val2, double minValue )
    { if ( fabs(val1-val2) < minValue ) return true; return false; }
    bool theSameDaugs( const reco::Candidate* daug1, const reco::Candidate* daug2, double minValue )
    { if ( sameVal(daug1->eta(), daug2->eta(), minValue) && sameVal(daug1->phi(), daug2->phi(), minValue) ) return true; return false; }
    bool theSameCompCand( const pat::CompositeCandidate* cand1, const pat::CompositeCandidate* cand2 );


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
