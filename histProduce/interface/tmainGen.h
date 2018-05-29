#ifndef __tMainGen_h__
#define __tMainGen_h__
#include "histProduce/histProduce/interface/tmain.h"
#include "histProduce/histProduce/interface/fourMom.h"

#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

struct MatchRes;
class treeMainGen : public treeMain
{
public:
    treeMainGen( TFileDirectory* d, Label l, const std::string& pName );
    ~treeMainGen();
    void getByLabel_genParticle( fwlite::Event* ev );

    // functions needs to be redefined in further class, which comes from treeMain
    virtual void Process( fwlite::Event* ev ) override = 0;
    virtual void Clear() override = 0;
    virtual void RegTree() override = 0;
    virtual void GetByLabel( fwlite::Event* ev ) override = 0;

    virtual MatchRes matchMC_CompositeCand( const pat::CompositeCandidate& cand, const std::vector<fourMom>& daugs, const double minDeltaR );
    virtual void calculateAllDeltaR( const std::pair<double, const reco::GenParticle*> compCand, const std::vector<fourMom>& daugs, const std::vector<unsigned>& idxs, std::vector<MatchRes>& res, const double minDeltaR ) final;

// tmplate function
template< typename myParticle >
    double mcMatchVal( const myParticle* cand, const reco::Candidate* targetCand )
    {
        // test for deltaR == (eta)^2+(phi)^2
        if ( cand == nullptr ) return 99999.;
        if ( targetCand == nullptr ) return 99999.;
        double val1 = cand->eta() - targetCand->eta();
        // Phi_mpi_pi is to change phi=[a,b] -> phi=[-pi,pi]. don't chanbe the degree to radius degree
        double val2 = TVector2::Phi_mpi_pi( cand->phi() - targetCand->phi() );
        double deltaR2 = val1*val1+val2*val2;
        return sqrt(deltaR2);
    }
    fwlite::Handle< std::vector<reco::GenParticle> > _genHandle;
private:
};
struct MatchRes
{
    MatchRes( bool flag ) : _flag( flag ), cand( nullptr ) {}
    MatchRes( double deltaR, const reco::GenParticle* gpCand ) : _flag( true ), deltaRProduct( deltaR ), cand( gpCand )
    {
        daugIdx.reserve( cand->numberOfDaughters() );
        matchedDaugIdx.reserve( cand->numberOfDaughters() );
    }
    ~MatchRes() {}
    void storeMatchRes( unsigned dId, unsigned gpdId )
    {
        daugIdx.push_back( dId );
        matchedDaugIdx.push_back( gpdId );
    }
    double getDeltaR() const
    { return deltaRProduct; }
    unsigned getDaugID( unsigned i ) const
    { return daugIdx[i]; }
    unsigned getmcDaugID( unsigned i ) const
    { return matchedDaugIdx[i]; }
    unsigned getNDaughterRecorded() const
    { return matchedDaugIdx.size(); }
    bool isValid() const
    { return _flag; }
    const reco::GenParticle* getCand() const
    { return cand; }


private:
    const bool _flag;
    double deltaRProduct;
    const reco::GenParticle* const cand;
    std::vector< int > daugIdx;
    std::vector< int > matchedDaugIdx;
};

#endif
