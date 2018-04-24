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
    double getFlightDistanceSignificance( const pat::CompositeCandidate& cand, const reco::Vertex* _pv )
    {
        if ( !cand.hasUserData( "fitVertex" ) ) return -999.;
        const reco::Vertex* _vtx = get<reco::Vertex>( cand, "fitVertex" );
        if ( _vtx == nullptr ) return -999.;
        if ( _pv == nullptr ) return -999.;
    
        double _x ( _vtx->x() ); double _y ( _vtx->y() );
        double _px( _pv->x() ); double _py( _pv->y() );
        double dist = sqrt( (_x-_px)*(_x-_px) + (_y-_py)*(_y-_py) );

        double _xErr( _vtx->xError() ); double _yErr( _vtx->yError() );
        double _pxErr( _pv->xError() ); double _pyErr( _pv->yError() );
        double totErr = ( _x*_x*_xErr*_xErr + _y*_y*_yErr*_yErr + _px*_px*_pxErr*_pxErr + _py*_py*_pyErr*_pyErr
                        + _x*_y*_xErr*_yErr + _px*_py*_pxErr*_pyErr ) / dist;
        return dist/totErr;
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
