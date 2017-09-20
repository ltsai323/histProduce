#ifndef __usefulFuncs_h__
#define __usefulFuncs_h__
#include <vector>
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"

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
}
#endif
