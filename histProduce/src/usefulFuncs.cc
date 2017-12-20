#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include <math.h>

namespace usefulFuncs
{
    double getFlightDistance( const pat::CompositeCandidate& cand, const reco::Vertex* _pv )
    {
        if ( !cand.hasUserData( "fitVertex" ) ) return -999.;
        const reco::Vertex* _vtx = get<reco::Vertex>( cand, "fitVertex" );
        if ( _vtx == nullptr ) return -999.;
        if ( _pv == nullptr ) return -999.;
    
        double _x ( _vtx->x() ); double _y ( _vtx->y() );
        double _px( _pv->x() ); double _py( _pv->y() );
        double dist ( (_x-_px)*(_x-_px) + (_y-_py)*(_y-_py) );
        return sqrt ( dist );
    }
    double getCosa2d( const pat::CompositeCandidate& cand, const reco::Vertex* _pv )
    {
        const GlobalVector* _mom = get<GlobalVector>( cand, "fitMomentum" );
        if ( _mom == nullptr ) return -999.;
        const reco::Vertex* _vtx = get<reco::Vertex>( cand, "fitVertex" );
        if ( _vtx == nullptr ) return -999.;
        if ( _pv == nullptr ) return -999.;
        double _x = _vtx->x() - _pv->x();
        double _y = _vtx->y() - _pv->y();
        double _r = sqrt( _x*_x+_y*_y );
        double mx = _mom->x();
        double my = _mom->y();
        double cosa2d = (mx*_x + my*_y) / (_mom->transverse()*_r);
        return cosa2d;
    }
}
