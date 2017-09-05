#ifndef __cutLists__
#define __cutLists__

#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "TMath.h"

namespace myCut
{
    class generalCutList
    {
    public:
        generalCutList( const double m, const double M ) : dmin(m), dmax(M) {}
        virtual bool accept( const pat::CompositeCandidate& cand ) const =0;
        virtual ~generalCutList() {}
    protected:
        const double dmin, dmax;
    };

    class vtxprobCut : public generalCutList // {{{
    {
    public:
        vtxprobCut( const double m, const double M=-1. ) : generalCutList( m, M ) {}
        bool accept( const pat::CompositeCandidate& cand ) const override 
        {
            if ( !cand.hasUserData( "fitVertex" ) ) return false;
            const reco::Vertex* _vtx = cand.userData<reco::Vertex>( "fitVertex" );
            return ( TMath::Prob( _vtx->chi2(), _vtx->ndof() ) > dmin );
        }
    }; // }}}

    class ptCut : public generalCutList // {{{
    {
    public:
        ptCut( const double m, const double M=-1. ) : generalCutList( m, M ) {}
        virtual bool accept( const pat::CompositeCandidate& cand ) const override
        {
            if ( !cand.hasUserData( "fitMomentum" ) ) return false;
            const GlobalVector* _mom = cand.userData<GlobalVector>("fitMomentum");
            double _pt = _mom->transverse();
            if ( _pt < dmin ) return false;
            if ( dmax < 0. ) return true;
            return ( _pt < dmax );
        }
    }; // }}}

    class massCut : public generalCutList // {{{
    {
    public:
        massCut( const double m, const double M=-1. ) : generalCutList( m, M ) {}
        virtual bool accept( const pat::CompositeCandidate& cand ) const override
        {
            if ( !cand.hasUserData( "fitMass" ) ) return false;
            const float _mass = cand.userFloat("fitMass");
            if ( _mass < dmin ) return false;
            if ( _mass > dmax ) return false;
            return true;
        }
    }; // }}}

    class flightDist2DCut : public generalCutList // {{{
    {
    public:
        flightDist2DCut( const double m, const double M=-1. ) : generalCutList( m, M ) {}
        virtual bool accept( const pat::CompositeCandidate& cand ) const override
        {
            if ( !cand.hasUserData( "primaryVertex" ) ) return false;
            if ( !cand.hasUserData( "fitVertex" ) ) return false;
            const reco::Vertex* _pvx = cand.userData<reco::Vertex>( "primaryVertex" );
            const reco::Vertex* _vtx = cand.userData<reco::Vertex>( "fitVertex" );
            double _x = _pvx->x() - _vtx->x();
            double _y = _pvx->y() - _vtx->y();
            return ( (_x*_x+_y*_y) > (dmin*dmin) );
        }
    }; // }}}

    class cosa2dCut : public generalCutList // {{{
    {
    public:
        cosa2dCut( const double m, const double M=-1. ) : generalCutList( m, M ) {}
        virtual bool accept( const pat::CompositeCandidate& cand ) const override
        {
            if ( !cand.hasUserData( "primaryVertex" ) ) return false;
            if ( !cand.hasUserData( "fitVertex" ) )     return false;
            if ( !cand.hasUserData( "fitMomentum" ) )   return false;
            const reco::Vertex* _pvx = cand.userData<reco::Vertex>( "primaryVertex" );
            const reco::Vertex* _vtx = cand.userData<reco::Vertex>( "fitVertex" );
            const GlobalVector* _mom = cand.userData<GlobalVector>( "fitMomentum" );
            double _x = _pvx->x() - _vtx->x();
            double _y = _pvx->y() - _vtx->y();
            double _r = sqrt( _x*_x+_y*_y );
            double mx = _mom->x();
            double my = _mom->y();
            return ( mx*_x+my*_y ) / ( _mom->transverse()*_r );
        }
    }; // }}}
}
#endif
