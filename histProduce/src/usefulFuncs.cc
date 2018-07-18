#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include <math.h>

namespace usefulFuncs
{
    SNRes SignalNumberCalculator( TH1* data, const double sigMin, const double sigMax )
    {
        int mBin = data->FindFixBin( sigMin );
        int MBin = data->FindFixBin( sigMax );

        // check number of bins in signal region is twice of some value.
        // ex: sig = [11, 16], then sideband = [8, 10] && [17, 19]
        if ( (MBin-mBin)%2 == 0 ) ++mBin;
        int deltaBin = (MBin-(mBin-1))/2;

        int Nsig = 0; int Nsid = 0;
        for ( int ibin = mBin; ibin <= MBin; ++ibin )
            Nsig += data->GetBinContent( ibin );

        for ( int ibin = mBin-deltaBin; ibin < mBin; ++ibin )
            Nsid += data->GetBinContent( ibin );
        for ( int ibin = MBin+1; ibin <= MBin+deltaBin; ++ibin )
            Nsid += data->GetBinContent( ibin );

        int S = Nsig-Nsid;
        double dS=sqrt( double(Nsig+Nsid) );
        
        SNRes res;
        res.Nsig = S; res.N = Nsig; res.ErrSig = dS;
        return res;
    }
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
    double getFlightDistance( const pat::CompositeCandidate& cand, const reco::BeamSpot* bs )
    {
        if ( !cand.hasUserData( "fitVertex" ) ) return -999.;
        const reco::Vertex* _vtx = get<reco::Vertex>( cand, "fitVertex" );
        if ( _vtx == nullptr ) return -999.;
        const reco::BeamSpot& vertexBeamSpot = *bs;
    
	    reco::Vertex::Point vpoint=_vtx->position();
	    //translate to global point, should be improved
	    GlobalPoint secondaryVertex (vpoint.x(), vpoint.y(), vpoint.z());

	    reco::Vertex::Error verr = _vtx->error();
	    // translate to global error, should be improved
	    GlobalError err(verr.At(0,0), verr.At(1,0), verr.At(1,1), verr.At(2,0), verr.At(2,1), verr.At(2,2) );

	    GlobalPoint displacementFromBeamspot( 
                              -1*((vertexBeamSpot.x0() - secondaryVertex.x()) + (secondaryVertex.z() - vertexBeamSpot.z0()) * vertexBeamSpot.dxdz()),
        					  -1*((vertexBeamSpot.y0() - secondaryVertex.y()) + (secondaryVertex.z() - vertexBeamSpot.z0()) * vertexBeamSpot.dydz()), 0);

        double lxy = displacementFromBeamspot.perp();
        return lxy;
    }
    double getFlightDistanceSignificance( const pat::CompositeCandidate& cand, const reco::BeamSpot* bs )
    {
        if ( !cand.hasUserData( "fitVertex" ) ) return -999.;
        const reco::Vertex* _vtx = get<reco::Vertex>( cand, "fitVertex" );
        if ( _vtx == nullptr ) return -999.;
        if (   bs == nullptr ) return -999.;
        const reco::BeamSpot& vertexBeamSpot = *bs;
    
	    reco::Vertex::Point vpoint=_vtx->position();
	    //translate to global point, should be improved
	    GlobalPoint secondaryVertex (vpoint.x(), vpoint.y(), vpoint.z());

	    reco::Vertex::Error verr = _vtx->error();
	    // translate to global error, should be improved
	    GlobalError err(verr.At(0,0), verr.At(1,0), verr.At(1,1), verr.At(2,0), verr.At(2,1), verr.At(2,2) );

	    GlobalPoint displacementFromBeamspot( 
                              -1*((vertexBeamSpot.x0() - secondaryVertex.x()) + (secondaryVertex.z() - vertexBeamSpot.z0()) * vertexBeamSpot.dxdz()),
        					  -1*((vertexBeamSpot.y0() - secondaryVertex.y()) + (secondaryVertex.z() - vertexBeamSpot.z0()) * vertexBeamSpot.dydz()), 0);

        double lxy = displacementFromBeamspot.perp();
        double lxyerr = sqrt(err.rerr(displacementFromBeamspot));
        return lxy/lxyerr;
    }
    double getFlightDistanceSignificance( const pat::CompositeCandidate& cand, const reco::Vertex* _pv )
    {
        if ( !cand.hasUserData( "fitVertex" ) ) return -999.;
        const reco::Vertex* _vtx = get<reco::Vertex>( cand, "fitVertex" );
        if ( _vtx == nullptr ) return -999.;
        if ( _pv == nullptr ) return -999.;
    
	    reco::Vertex::Point vpoint=_vtx->position();
	    //translate to global point, should be improved
	    GlobalPoint secondaryVertex (vpoint.x(), vpoint.y(), vpoint.z());

	    reco::Vertex::Error verr = _vtx->error();
	    // translate to global error, should be improved
	    GlobalError err(verr.At(0,0), verr.At(1,0), verr.At(1,1), verr.At(2,0), verr.At(2,1), verr.At(2,2) );

	    GlobalPoint displacementFromPV( -1*(_pv->x() -  secondaryVertex.x()),
         					            -1*(_pv->y() -  secondaryVertex.y()), 0);

        double lxy = displacementFromPV.perp();
        double lxyerr = sqrt(err.rerr(displacementFromPV));
        return lxy/lxyerr;
    }
    double getCosa2d( const pat::CompositeCandidate& cand, const reco::Vertex* _pv )
    {
        const GlobalVector* _mom = get<GlobalVector>( cand, "fitMomentum" );
        if ( _mom == nullptr ) return -999.;
        const reco::Vertex* _vtx = get<reco::Vertex>( cand, "fitVertex" );
        if ( _vtx == nullptr ) return -999.;
        if ( _pv == nullptr ) return -999.;

        math::XYZVector pperp(_mom->x(), _mom->y(), _mom->z() );
	    reco::Vertex::Point vpoint=_vtx->position();
	    GlobalPoint secondaryVertex (vpoint.x(), vpoint.y(), vpoint.z());
	    GlobalPoint displacementFromPV( -1*(_pv->x() -  secondaryVertex.x()),
         					            -1*(_pv->y() -  secondaryVertex.y()), 0);

	    reco::Vertex::Point vperp(displacementFromPV.x(),displacementFromPV.y(),0.);

        double cosAlpha = vperp.Dot(pperp)/(vperp.R()*pperp.R());
        return cosAlpha;

    }
    double getCosa2d( const pat::CompositeCandidate& cand, const reco::BeamSpot* _bs )
    {
        const GlobalVector* _mom = get<GlobalVector>( cand, "fitMomentum" );
        if ( _mom == nullptr ) return -999.;
        const reco::Vertex* _vtx = get<reco::Vertex>( cand, "fitVertex" );
        if ( _vtx == nullptr ) return -999.;
        if ( _bs == nullptr ) return -999.;
        const reco::BeamSpot& vertexBeamSpot = *_bs;
        math::XYZVector pperp(_mom->x(), _mom->y(), _mom->z() );
	    reco::Vertex::Point vpoint=_vtx->position();
	    GlobalPoint secondaryVertex (vpoint.x(), vpoint.y(), vpoint.z());
	    GlobalPoint displacementFromBeamspot( 
                              -1*((vertexBeamSpot.x0() - secondaryVertex.x()) + (secondaryVertex.z() - vertexBeamSpot.z0()) * vertexBeamSpot.dxdz()),
        					  -1*((vertexBeamSpot.y0() - secondaryVertex.y()) + (secondaryVertex.z() - vertexBeamSpot.z0()) * vertexBeamSpot.dydz()), 0);

	    reco::Vertex::Point vperp(displacementFromBeamspot.x(),displacementFromBeamspot.y(),0.);

        double cosAlpha = vperp.Dot(pperp)/(vperp.R()*pperp.R());
        return cosAlpha;
    }
    double getCosa3d( const pat::CompositeCandidate& cand, const reco::Vertex* _pv )
    {
        const GlobalVector* _mom = get<GlobalVector>( cand, "fitMomentum" );
        if ( _mom == nullptr ) return -999.;
        const reco::Vertex* _vtx = get<reco::Vertex>( cand, "fitVertex" );
        if ( _vtx == nullptr ) return -999.;
        if ( _pv == nullptr ) return -999.;
        double _x = _vtx->x() - _pv->x();
        double _y = _vtx->y() - _pv->y();
        double _z = _vtx->z() - _pv->z();
        double _r = sqrt( _x*_x+_y*_y+_z*_z );
        double mx = _mom->x();
        double my = _mom->y();
        double mz = _mom->z();
        double cosa3d = (mx*_x + my*_y + mz*_z) / (_mom->mag()*_r);
        return cosa3d;
    }
    double getCosAngleToVtx_PV_BS( const pat::CompositeCandidate& cand, const reco::Vertex& pv, const reco::BeamSpot& bs)
    {
        // use Law of cosines to calculate angle to cand-PV-BS.
        const reco::Vertex* _vtx = get<reco::Vertex>( cand, "fitVertex" );
        double len_candToBS = (_vtx->x() - bs.x(_vtx->z()))*(_vtx->x() - bs.x(_vtx->z())) + (_vtx->y() - bs.y(_vtx->z()))*(_vtx->y() - bs.y(_vtx->z()));
        double len_candToPV = (_vtx->x() - pv.x())*(_vtx->x() - pv.x()) + (_vtx->y() - pv.y())*(_vtx->y() - pv.y());
        double len_PVToBS   = (pv.x() - bs.x(_vtx->z()))*(pv.x() - bs.x(_vtx->z())) + (pv.y() - bs.y(_vtx->z()))*(pv.y() - bs.y(_vtx->z()));
        double calcInter = 2. * sqrt( len_candToPV * len_PVToBS );
        double cosAngle = -1. * ( len_candToBS - len_candToPV - len_PVToBS ) / calcInter;
        return cosAngle;
    }


}
