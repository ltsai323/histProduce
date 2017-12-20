#include "histProduce/histProduce/interface/hMainfindLam0FDdiff.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "math.h"


histMain_findLam0FlightDistanceDiff::histMain_findLam0FlightDistanceDiff( TFileDirectory* d ) :
    histMain( d, histMain::Label("lbWriteSpecificDecay", "Lam0Cand", "bphAnalysis") )
{
    _nMap[fd030] = "fd030";
    _nMap[fd060] = "fd060";
    _nMap[fd090] = "fd090";
    _nMap[fd120] = "fd120";
    _nMap[fd150] = "fd150";
    _nMap[fd180] = "fd180";
    _nMap[fd210] = "fd210";
    _nMap[fd240] = "fd240";
    for ( const auto& _dName : _nMap )
    {
        const std::string& name = _dName.second;
        createHisto( name+"massLam0_Lam0"        ,  50, 1.10, 1.15 );
        createHisto( name+"ptProton",  50, 0., 20. );
        createHisto( name+"ptPion", 50., 0., 20. );
    }
}
void histMain_findLam0FlightDistanceDiff::Process( fwlite::Event* ev )
{
    try 
    {
        if ( !ev->isValid() ) return;
        _handle.getByLabel( *ev, _label.module.c_str(), _label.label.c_str(), _label.process.c_str() );

        // pv not recorded, use BS
        fwlite::Handle< reco::BeamSpot > beamSpotHandle;
        beamSpotHandle.getByLabel( *ev,"offlineBeamSpot", "", "RECO"  );
        if ( !beamSpotHandle.isValid() ) return;
        const reco::Vertex bs( (*beamSpotHandle).position(), (*beamSpotHandle).covariance3D() );
    
        std::map< double, const pat::CompositeCandidate*> vtxprobChooser;
        std::vector<pat::CompositeCandidate>::const_iterator handleIter = _handle->begin();
        std::vector<pat::CompositeCandidate>::const_iterator handleIend = _handle->end  ();
        while( handleIter != handleIend )
        {
            const pat::CompositeCandidate& cand = *handleIter++;
            const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
            if ( _vtx == nullptr ) continue;
            vtxprobChooser.insert( std::make_pair( TMath::Prob( _vtx->chi2(), _vtx->ndof() ), &cand ) );
        }
        if ( vtxprobChooser.size() == 0 ) std::cout << "warining nothing in the event\n";
        fillHisto("candInEvent", vtxprobChooser.size() );

        std::map< double, const pat::CompositeCandidate*>::const_reverse_iterator iter = vtxprobChooser.rbegin();
        std::map< double, const pat::CompositeCandidate*>::const_reverse_iterator iend = vtxprobChooser.rend  ();
        //int vtxprobSortLimit = 0;

        while ( iter != iend )
        {
            const pat::CompositeCandidate& cand = *(iter++->second);
            if ( !cand.hasUserFloat( "fitMass" ) ) continue;

            double fd = usefulFuncs::getFlightDistance ( cand, &bs );
            fillHisto( "parLam0_FlightDistance", fd );
            if ( fd < 0.1 ) continue;

            double cos2d = usefulFuncs::getCosa2d( cand, &bs );
            fillHisto( "parLam0_cosa2d", cos2d );
            if ( cos2d < 0.99 ) continue;

            for ( const auto& name : _nMap )
            {
                const std::string& dirname = name.second;
                double i ( name.first );
                if ( fd < 0.3*i*0.3*i ) continue;
                fillHisto( dirname+"massLam0_Lam0", cand.userFloat("fitMass") );
            }
        }
    } catch (...) {}
}

void histMain_findLam0FlightDistanceDiff::Clear()
{
}
//double histMain_findLam0FlightDistanceDiff::getFlightDistance( const pat::CompositeCandidate& cand, const reco::Vertex* _bs )
//{
//    if ( !cand.hasUserData( "fitVertex" ) ) return -999.;
//    const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
//    if ( _vtx == nullptr ) return -999.;
//    if ( _bs == nullptr ) return -999.;
//
//    double _x ( _vtx->x() ); double _y ( _vtx->y() );
//    double _px( _bs->x() ); double _py( _bs->y() );
//    double dist ( (_x-_px)*(_x-_px) + (_y-_py)*(_y-_py) );
//    return sqrt ( dist );
//}
//double histMain_findLam0FlightDistanceDiff::getCosa2d( const pat::CompositeCandidate& cand, const reco::Vertex* _bs )
//{
//    const GlobalVector* _mom = usefulFuncs::get<GlobalVector>( cand, "fitMomentum" );
//    if ( _mom == nullptr ) return -999.;
//    const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
//    if ( _vtx == nullptr ) return -999.;
//    if ( _bs == nullptr ) return -999.;
//    double _x = _vtx->x() - _bs->x();
//    double _y = _vtx->y() - _bs->y();
//    double _r = sqrt( _x*_x+_y*_y );
//    double mx = _mom->x();
//    double my = _mom->y();
//    double cosa2d = (mx*_x + my*_y) / (_mom->transverse()*_r);
//    return cosa2d;
//}
