#include "histProduce/histProduce/interface/hMainGenInfo.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "math.h"


histMain_GenInformation::histMain_GenInformation( TFileDirectory* d ) :
    histMain( d, histMain::Label("", "", "") )
{
    createHisto( "ptGenInfo_kaon"   ,  70, 0.,  7. );
    createHisto( "ptGenInfo_pion"   ,  70, 0.,  7. );
    createHisto( "ptGenInfo_proton" ,  70, 0.,  7. );
    createHisto( "momGenInfo_kaon"  , 100, 0., 20. );
    createHisto( "momGenInfo_pion"  , 100, 0., 20. );
    createHisto( "momGenInfo_proton", 150, 0., 30. );
    createHisto( "momGenInfo_kaonSignal"  , 100, 0., 20. );
    createHisto( "momGenInfo_protonSignal", 150, 0., 30. );
    createHisto( "massGenInfo_LbSignal",  80, 5.4, 5.8 );
}
void histMain_GenInformation::Process( fwlite::Event* ev )
{
    try 
    {
        if ( !ev->isValid() ) return;
        genHandle.getByLabel( *ev, "genParticles", "", "HLT" );
        beamSpotHandle.getByLabel( *ev,"offlineBeamSpot", "", "RECO"  );
        if ( !genHandle.isValid() ) return;
        if ( !genHandle->size() ) return;
        if ( !beamSpotHandle.isValid() ) return;
        const reco::Vertex bs( (*beamSpotHandle).position(), (*beamSpotHandle).covariance3D() );
        std::vector< reco::GenParticle >::const_iterator iter = genHandle->begin();
        std::vector< reco::GenParticle >::const_iterator iend = genHandle->end  ();
        while ( iter != iend )
        {
            const reco::GenParticle& gParticle = *iter++;
            if ( gParticle.status() < 2 )
            {
                if ( fabs( gParticle.pdgId() ) == 321 )
                {
                    fillHisto( "ptGenInfo_kaon", gParticle.pt() );
                    fillHisto( "momGenInfo_kaon", gParticle.p() );
                }
                if ( fabs( gParticle.pdgId() ) == 211 )
                {
                    fillHisto( "ptGenInfo_pion", gParticle.pt() );
                    fillHisto( "momGenInfo_pion", gParticle.p() );
                }
                if ( fabs( gParticle.pdgId() ) == 2212 )
                {
                    fillHisto( "ptGenInfo_proton", gParticle.pt() );
                    fillHisto( "momGenInfo_proton", gParticle.p() );
                }
                if ( fabs( gParticle.pdgId() ) == 321 )
                    if ( fabs( gParticle.mother()->pdgId() ) == 5122 && 
                    gParticle.mother()->numberOfDaughters() == 3
                   )
                    fillHisto( "momGenInfo_kaonSignal", gParticle.p() );
                if ( fabs( gParticle.pdgId() ) == 2212 &&
                     fabs( gParticle.mother()->pdgId() ) == 5122 && 
                    gParticle.mother()->numberOfDaughters() == 3
                   )
                    fillHisto( "momGenInfo_protonSignal", gParticle.p() );
            }
            if ( fabs( gParticle.pdgId() ) == 5122 )
                fillHisto( "massGenInfo_LbSignal", gParticle.mass() );

        }

    } catch (...) {}
}

void histMain_GenInformation::Clear()
{
}
//double histMain_GenInformation::getFlightDistance( const pat::CompositeCandidate& cand, const reco::Vertex* _bs )
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
//double histMain_GenInformation::getCosa2d( const pat::CompositeCandidate& cand, const reco::Vertex* _bs )
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
