#ifndef __usefulFuncs_h__
#define __usefulFuncs_h__
#include <vector>
#include <utility> // std::pair
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

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
template< typename key, typename val >
    void sortingByFirstValue( std::vector< std::pair<key, val> >& myVector )
    {
        int n = myVector.size();
        for ( int i=0; i < n; ++i )
            for ( int j = i; j < n; ++j )
            {
                std::pair< key, val > tmpPair;
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

    double getFlightDistance( const pat::CompositeCandidate& cand, const reco::Vertex* _pv=nullptr );
    double getCosa2d( const pat::CompositeCandidate& cand, const reco::Vertex* _pv=nullptr );

}
#endif
