#include "histProduce/histProduce/interface/hMainfindFDdiff.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
//
// this code find Lambda0 & K_short by separate flight distance.
// consider the track with larger pt as proton track ( for Lambda0 )
//
#define GAP 0.2

namespace myMass
{
    const double protonMass(0.9382720813);
    const double   kaonMass(0.493667);
    const double   pionMass(0.13957061);
    const double   muonMass(0.1056583715);
};

histMain_findFlightDistanceDiff::histMain_findFlightDistanceDiff( TFileDirectory* d ) :
    histMain( d, histMain::Label("lbWriteSpecificDecay", "TkTkFitted", "bphAnalysis"), "FDdiff" )
{
    using namespace myCut;

    _nMap[fd020] = "fd020";
    _nMap[fd040] = "fd040";
    _nMap[fd060] = "fd060";
    _nMap[fd080] = "fd080";
    _nMap[fd100] = "fd100";
    _nMap[fd120] = "fd120";
    _nMap[fd140] = "fd140";
    _nMap[fd160] = "fd160";
    _nMap[fd180] = "fd180";
    for ( const auto& _dName : _nMap )
    {
        const std::string& name = _dName.second;
        createHisto( name+"massTkTk_FakeLam0",  100, 1.10, 1.15 );
        createHisto( name+"massTkTk_FakeKs",  100, 0.45, 0.55);
        createHisto( name+"parTkTk_Harmonic_proton_inLam0", 100, 0., 20., 1000, 0., 20. );
        createHisto( name+"parTkTk_PixelHrm_proton_inLam0", 100, 0., 20., 1000, 0., 20. );
        createHisto( name+"parTkTk_Harmonic_pion_inLam0"  , 100, 0., 20., 1000, 0., 20. );
        createHisto( name+"parTkTk_PixelHrm_pion_inLam0"  , 100, 0., 20., 1000, 0., 20. );
        createHisto( name+"parSignalIPt_proton_inLam0", 400, 0, 4.0 );
        createHisto( name+"parSignalIPt_pion_inLam0"  , 400, 0, 4.0 );
        createHisto( name+"parBackgroundIPt_proton_inLam0", 400, 0, 4.0 );
        createHisto( name+"parBackgroundIPt_pion_inLam0"  , 400, 0, 4.0 );
    }
    createHisto( "flightDistTkTk", 3051, -0.050, 3.000 );

}
void histMain_findFlightDistanceDiff::Process( fwlite::Event* ev )
{
    try
    {
        if ( !ev->isValid() ) return;

        _handle.getByLabel( *ev, _label.module.c_str(), _label.label.c_str(), _label.process.c_str() );
        fwlite::Handle< reco::BeamSpot > beamSpotHandle;
        beamSpotHandle.getByLabel( *ev,"offlineBeamSpot", "", "RECO"  );
        if ( !beamSpotHandle.isValid() ) return;
        if ( _handle->size()  == 0 ) return;
        const reco::Vertex bs( (*beamSpotHandle).position(), (*beamSpotHandle).covariance3D() );

        std::vector< std::pair<double, const pat::CompositeCandidate*> > candsSorted;
        candsSorted.reserve( _handle->size() );
        std::vector<pat::CompositeCandidate>::const_iterator handleIter = _handle->begin();
        std::vector<pat::CompositeCandidate>::const_iterator handleIend = _handle->end  ();
        while( handleIter != handleIend )
        {
            const pat::CompositeCandidate& cand = *handleIter++;
            const reco::Vertex* tktkVtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
            if ( tktkVtx == nullptr ) continue;

            if ( !cand.hasUserFloat("fitMass") ) continue;
            if ( !cand.hasUserFloat("Proton.IPt") ) continue;
            if ( !cand.hasUserFloat(  "Kaon.IPt") ) continue;
            if ( !cand.hasUserFloat("Proton.IPt.Error") ) continue;
            if ( !cand.hasUserFloat(  "Kaon.IPt.Error") ) continue;
            if ( !cand.hasUserFloat("Proton.dEdx.Harmonic") ) continue;
            if ( !cand.hasUserFloat(  "Kaon.dEdx.Harmonic") ) continue;
            if ( !cand.hasUserFloat("Proton.dEdx.pixelHrm") ) continue;
            if ( !cand.hasUserFloat(  "Kaon.dEdx.pixelHrm") ) continue;

            const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
            if ( _vtx == nullptr ) continue;
            double fd = usefulFuncs::getFlightDistance ( cand, &bs );
            double cos2d = usefulFuncs::getCosa2d( cand, &bs );
            double vtxprob = TMath::Prob( _vtx->chi2(), _vtx->ndof() );
            fillHisto( "parTkTk_vtxprob", vtxprob );
            if ( fd < 0.1 ) continue;
            if ( cos2d < 0.99 ) continue;
            if ( vtxprob < 0.15 ) continue;
            if ( cand.userFloat("Proton.IPt") < 0.03 || cand.userFloat("Kaon.IPt") < 0.03 ) continue;
            candsSorted.emplace_back( std::pair<double, const pat::CompositeCandidate*>( TMath::Prob( _vtx->chi2(), _vtx->ndof() ), &cand ) );
        }
        usefulFuncs::sortingByFirstValue( candsSorted );

        std::vector< std::pair< double, const pat::CompositeCandidate*> >::const_iterator iter = candsSorted.begin();
        std::vector< std::pair< double, const pat::CompositeCandidate*> >::const_iterator iend = candsSorted.end  ();
        int vtxprobSortLimit = 0;
        while ( iter != iend )
        {
            const pat::CompositeCandidate& cand = *(iter++->second);
            if ( ++vtxprobSortLimit > 20 ) break;

            const reco::Vertex* tktkVtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
            double transFD = ( tktkVtx->x() - bs.x() )*( tktkVtx->x()-bs.x() ) + (tktkVtx->y()-bs.y())*(tktkVtx->y()-bs.y());
            fillHisto( "flightDistTkTkinLb", sqrt( transFD ) );
            if ( transFD < 0.01*0.01 ) continue;

            if ( cand.hasUserData("Proton.fitMom") && cand.hasUserData("Kaon.fitMom") )
            {
                const GlobalVector* pTkMom = cand.userData<GlobalVector>("Proton.fitMom");
                const GlobalVector* nTkMom = cand.userData<GlobalVector>("Kaon.fitMom");

                fourMom pTk( pTkMom->x(), pTkMom->y(), pTkMom->z() );
                fourMom nTk( nTkMom->x(), nTkMom->y(), nTkMom->z() );

                fourMom twoTk = pTk + nTk;
                for ( const auto&  name : _nMap )
                {
                    pTk.setMass( myMass::protonMass );
                    nTk.setMass( myMass::  pionMass );
                    twoTk = pTk + nTk;
                    const std::string& dirname = name.second;
                    double i( name.first );
                    if ( transFD < GAP*i*GAP*i ) continue;
                    if ( twoTk.Mag() > 1.10 && twoTk.Mag() < 1.15 )
                    {
                        fillHisto( dirname+"massTkTk_FakeLam0", twoTk.Mag() );
                        if ( twoTk.Mag() > 1.114 && twoTk.Mag() < 1.118 )
                        {
                            fillHisto( dirname+"parTkTk_Harmonic_proton_inLam0" , pTk.Momentum(), cand.userFloat("Proton.dEdx.Harmonic") );
                            fillHisto( dirname+"parTkTk_PixelHrm_proton_inLam0" , pTk.Momentum(), cand.userFloat("Proton.dEdx.pixelHrm") );
                            fillHisto( dirname+"parTkTk_Harmonic_pion_inLam0" , nTk.Momentum(), cand.userFloat("Kaon.dEdx.Harmonic") );
                            fillHisto( dirname+"parTkTk_PixelHrm_pion_inLam0" , nTk.Momentum(), cand.userFloat("Kaon.dEdx.pixelHrm") );
                            fillHisto( dirname+"parSignalIPt_proton_inLam0" , cand.userFloat("Proton.IPt") );
                            fillHisto( dirname+"parSignalIPt_pion_inLam0"   , cand.userFloat(  "Kaon.IPt") );
                        }
                        else
                        {
                            fillHisto( dirname+"parBackgroundIPt_proton_inLam0" , cand.userFloat("Proton.IPt") );
                            fillHisto( dirname+"parBackgroundIPt_pion_inLam0"   , cand.userFloat(  "Kaon.IPt") );
                        }
                    }
                }
                pTk.setMass( myMass::pionMass );
                nTk.setMass( myMass::pionMass );
                twoTk = pTk + nTk;
                for ( const auto&  name : _nMap )
                {
                    const std::string& dirname = name.second;
                    double i( name.first );
                    if ( transFD < GAP*i*GAP*i ) continue;
                    if ( twoTk.Mag() > 0.45 && twoTk.Mag() < 0.55 )
                        fillHisto( dirname+"massTkTk_FakeKs", twoTk.Mag() );
                        if ( twoTk.Mag() > 0.51 && twoTk.Mag() < 0.51 )
                        {
                            fillHisto( dirname+"parTkTk_Harmonic_pion_inKshort" , pTk.Momentum(), cand.userFloat("Proton.dEdx.Harmonic") );
                            fillHisto( dirname+"parTkTk_PixelHrm_pion_inKshort" , pTk.Momentum(), cand.userFloat("Proton.dEdx.pixelHrm") );
                            fillHisto( dirname+"parTkTk_Harmonic_pion_inKshort" , nTk.Momentum(), cand.userFloat("Kaon.dEdx.Harmonic") );
                            fillHisto( dirname+"parTkTk_PixelHrm_pion_inKshort" , nTk.Momentum(), cand.userFloat("Kaon.dEdx.pixelHrm") );
                            fillHisto( dirname+"parSignalIPt_proton_inKshort" , cand.userFloat("Proton.IPt") );
                            fillHisto( dirname+"parSignalIPt_pion_inKshort"   , cand.userFloat(  "Kaon.IPt") );
                        }
                        else
                        {
                            fillHisto( dirname+"parBackgroundIPt_proton_inKshort" , cand.userFloat("Proton.IPt") );
                            fillHisto( dirname+"parBackgroundIPt_pion_inKshort"   , cand.userFloat(  "Kaon.IPt") );
                        }
                }
            } // double if ended
        }
    } catch ( ... ) {}
}

void histMain_findFlightDistanceDiff::Clear()
{
}

//double histMain_findFlightDistanceDiff::getFlightDistance( const pat::CompositeCandidate& cand, const reco::Vertex* _bs )
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
//double histMain_findFlightDistanceDiff::getCosa2d( const pat::CompositeCandidate& cand, const reco::Vertex* _bs )
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
