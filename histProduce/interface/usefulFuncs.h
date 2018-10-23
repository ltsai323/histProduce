#ifndef __usefulFuncs_h__
#define __usefulFuncs_h__
#include <vector>
#include <utility> // std::pair
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "TVector2.h"
#include "TH1.h"

namespace usefulFuncs
{
template< typename T >
    static const T* get( const pat::CompositeCandidate& cand, const std::string& name )
    {
        if ( cand.hasUserData(name) ) return cand.userData<T>( name );
        return nullptr;
    }
template< typename T >
    static const T* getByRef( const pat::CompositeCandidate& cand, const std::string& name )
    {
        if ( cand.hasUserData(name) )
        {
            typedef edm::Ref< std::vector<T> > objRef;
            const objRef* ref = cand.userData<objRef>( name );
            if ( ref ==      0 ) return nullptr;
            if ( ref->isNull() ) return nullptr;
            return ref->get();
        }
        return nullptr;
    }

// sort result: large key to small key
template< typename key, typename val >
    void sortingByFirstValue( std::vector< std::pair<key, val> >& myVector )
    {
        int n = myVector.size();
        std::pair< key, val > tmpPair;
        for ( int i=0; i < n; ++i )
            for ( int j = i; j < n; ++j )
            {
                // if first one is smaller than second one, exchange
                // meaning that smaller value will propagate to last
                if ( myVector[i].first < myVector[j].first )
                {
                    tmpPair = myVector[i];
                    myVector[i] = myVector[j];
                    myVector[j] = tmpPair;
                }
            }
    }

    // mc matching, return boolean
    template< typename myParticle >
    bool mcMatch( const myParticle* cand, const reco::GenParticle* gpCand, double minDeltaR )
    {
        // test for deltaR == (eta)^2+(phi)^2
        if ( cand == nullptr ) return false;
        if ( gpCand == nullptr ) return false;
        double val1 = cand->eta() - gpCand->eta();
        double val2 = TVector2::Phi_mpi_pi( cand->phi() - gpCand->phi() );
        double deltaR2 = val1*val1+val2*val2;
        return (deltaR2 < minDeltaR*minDeltaR) ? true : false;
    }

    // mc matching, return deltaR
    template< typename myParticle >
    double mcMatchVal( const myParticle* cand, const reco::GenParticle* gpCand )
    {
        // test for deltaR == (eta)^2+(phi)^2
        if ( cand == nullptr ) return 9999.;
        if ( gpCand == nullptr ) return 9999.;
        double val1 = cand->eta() - gpCand->eta();
        double val2 = TVector2::Phi_mpi_pi( cand->phi() - gpCand->phi() );
        double deltaR2 = val1*val1+val2*val2;
        return sqrt(deltaR2) ;
    }



    struct SNRes { int Nsig, N; double ErrSig; };

    SNRes SignalNumberCalculator( TH1* data, const double sigMin, const double sigMax );
    double getFlightDistance( const pat::CompositeCandidate& cand, const reco::Vertex* _pv=nullptr );
    double getFlightDistance( const pat::CompositeCandidate& cand, const reco::BeamSpot* bs=nullptr );
    double getFlightDistanceSignificance( const pat::CompositeCandidate& cand, const reco::BeamSpot* bs=nullptr );
    double getFlightDistanceSignificance( const pat::CompositeCandidate& cand, const reco::Vertex* _pv=nullptr );
    double getCosa2d( const pat::CompositeCandidate& cand, const reco::BeamSpot* bs=nullptr );
    double getCosa2d( const pat::CompositeCandidate& cand, const reco::Vertex* _pv=nullptr );
    double getCosa3d( const pat::CompositeCandidate& cand, const reco::Vertex* _pv=nullptr );

    double getCosAngleToVtx_PV_BS( const pat::CompositeCandidate& cand, const reco::Vertex& pv, const reco::BeamSpot& bs);
    double logDeltaRToCompcands( const pat::CompositeCandidate* cand1, const pat::CompositeCandidate* cand2 );
    //double logDeltaRToCompcands( pat::CompositeCandidate const* cand1, pat::CompositeCandidate const* cand2 );
    double logDeltaRToDaugs( const reco::Candidate* dau1, const reco::Candidate* dau2 );
    int recordEventSizeWithSeparator( int listSize, int sep ) { return (listSize<<2)+sep; }
    int getEventSizeFromSizeSeparator( int sizeSeparator ) { return sizeSeparator>>2; }
    int getSepFromSizeSeparator( int sizeSeparator ) { return sizeSeparator%4; }
    int inverter ( int sep ) { return sep==2 ? 1:2; }
}
#endif
