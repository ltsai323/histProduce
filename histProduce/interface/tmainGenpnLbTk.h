#ifndef __tMainGenpnLbTk_h__
#define __tMainGenpnLbTk_h__
#include "histProduce/histProduce/interface/tmainGen.h"
#include "histProduce/histProduce/interface/formatTreepnLbTk.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include <vector>
#include <utility> // std::pair
#include "TH2D.h"

class treeMainGen_pnLbTk : public treeMainGen, public formatTree_plusminus_LbTk
{
public:
    treeMainGen_pnLbTk ( TFileDirectory* d);
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
    void getByLabel_antiCand( fwlite::Event* ev );
    fwlite::Handle< std::vector<reco::GenParticle> > genHandle;
    fwlite::Handle< reco::BeamSpot > beamSpotHandle;
    fwlite::Handle< std::vector<reco::Vertex> > primaryVHandle;
    fwlite::Handle< std::vector<pat::CompositeCandidate> > antiCandHandle;
    treeMain::Label antiCandLabel;

    std::vector< std::pair< std::pair<double,const pat::CompositeCandidate*>, std::pair<double,const pat::CompositeCandidate*> > > pairpnCands
    ( std::vector<std::pair<double,const pat::CompositeCandidate*> >& pCands, std::vector<std::pair<double, const pat::CompositeCandidate*> >& nCands );
    double deltaRCalc;
    bool sameVal( double val1, double val2, double minValue )
    { if ( fabs(val1-val2) < minValue ) return true; return false; }
    bool theSameDaugs( const reco::Candidate* daug1, const reco::Candidate* daug2, double minValue )
    { if ( sameVal(daug1->eta(), daug2->eta(), minValue) && sameVal(daug1->phi(), daug2->phi(), minValue) ) return true; return false; }
    bool theSameCompCand( const pat::CompositeCandidate* cand1, const pat::CompositeCandidate* cand2 );

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
        pptonPID, pptonMomPID, pkaonPID, pkaonMomPID,
        nptonPID, nptonMomPID, nkaonPID, nkaonMomPID,
        totNumI
    };

};


#endif
