#include "histProduce/histProduce/interface/hMainLbTk.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "math.h"

namespace myMass
{
    const double protonMass(0.9382720813);
    const double   kaonMass(0.493667);
    const double   pionMass(0.13957061);
    const double   muonMass(0.1056583715);
};

histMain_LbTk::histMain_LbTk( TFileDirectory* d ) :
    histMain( d, histMain::Label("lbWriteSpecificDecay", "LbToTkTkFitted", "bphAnalysis") )
{
    createHisto( "candInEventLbTk"          , 160,  0.0, 1600.);
    createHisto( "candInVtxsortLbTk"        ,  40,  0.0, 400.);
    createHisto( "parLbTk_pTkMom"           , 100,   0., 20. );
    createHisto( "parLbTk_nTkMom"           , 100,   0., 20. );
    createHisto( "massLbTk_LbTk"            , 100, 5.00, 6.00);
    createHisto( "massLbTk_TkTk"            , 100, 1.40, 2.20);
    createHisto( "massLbTk_FakeLam0"        ,  50, 1.10, 1.15);
    createHisto( "massLbTk_FakeKshort"      ,  40,  0.4, 0.6 );
    createHisto( "massLbTk_FakeK892"        ,  50, 0.75, 1.00);
    createHisto( "massLbTk_FakePhi1020"     ,  60, 0.90, 1.20);
    createHisto( "massLbTk_FakeBs"          , 100, 5.00, 6.00);
    createHisto( "massLbTk_FakeBd"          , 100, 5.00, 6.00);
    createHisto( "massLbTk_FakeLbL0"        , 100, 5.00, 6.00);
    createHisto( "parLbTk_cosa2d"           , 100, -1.0, 1.0 );
    createHisto( "parLbTk_cosa2dBS"         , 100, -1.0, 1.0 );
    createHisto( "parLbTk_vtxprob"          , 100,   0.,   1.);
    createHisto( "parLbTk_FlightDistance"   ,  65,-0.05,0.060);
    createHisto( "parLbTk_FDBS"             ,  75,-0.05,0.700);
    createHisto( "parLbTk_distBS_PV"        , 105,-0.05,1.000);

    createHisto( "parLbTk_Harmonic_Signal_ptk", 100, 0., 5., 100, 0., 10. );
    createHisto( "parLbTk_PixelHrm_Signal_ptk", 100, 0., 5., 100, 0., 10. );
    createHisto( "parLbTk_Harmonic_Signal_ntk", 100, 0., 5., 100, 0., 10. );
    createHisto( "parLbTk_PixelHrm_Signal_ntk", 100, 0., 5., 100, 0., 10. );
    createHisto( "parLbTk_Harmonic_Backgd_ptk", 100, 0., 5., 100, 0., 10. );
    createHisto( "parLbTk_PixelHrm_Backgd_ptk", 100, 0., 5., 100, 0., 10. );
    createHisto( "parLbTk_Harmonic_Backgd_ntk", 100, 0., 5., 100, 0., 10. );
    createHisto( "parLbTk_PixelHrm_Backgd_ntk", 100, 0., 5., 100, 0., 10. );

    createHisto( "parLbTk_Signal_pIPt", 200, 0, 0.4 );
    createHisto( "parLbTk_Signal_nIPt", 200, 0, 0.4 );
    createHisto( "parLbTk_Signal_pIPt/Err", 200, 0, 5. );
    createHisto( "parLbTk_Signal_nIPt/Err", 200, 0, 5. );
    createHisto( "parLbTk_Backgd_pIPt", 200, 0, 0.4 );
    createHisto( "parLbTk_Backgd_nIPt", 200, 0, 0.4 );
    createHisto( "parLbTk_Backgd_pIPt/Err", 200, 0, 5. );
    createHisto( "parLbTk_Backgd_nIPt/Err", 200, 0, 5. );
}
void histMain_LbTk::Process( fwlite::Event* ev )
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
    
        //std::map< double, const pat::CompositeCandidate*> vtxprobChooser;
        std::vector< std::pair<double, const pat::CompositeCandidate*> > candsSorted;
        candsSorted.reserve( _handle->size() );
        std::vector<pat::CompositeCandidate>::const_iterator handleIter = _handle->begin();
        std::vector<pat::CompositeCandidate>::const_iterator handleIend = _handle->end  ();
        while( handleIter != handleIend )
        {
            const pat::CompositeCandidate& cand = *handleIter++;
            if ( !cand.hasUserFloat("fitMass") ) continue;
            if ( !cand.hasUserFloat("TkTk/Proton.IPt") ) continue;
            if ( !cand.hasUserFloat("TkTk/Kaon.IPt") ) continue;
            if ( !cand.hasUserFloat("TkTk/Proton.IPt.Error") ) continue;
            if ( !cand.hasUserFloat("TkTk/Kaon.IPt.Error") ) continue;
            if ( !cand.hasUserFloat("TkTk/Proton.dEdx.Harmonic") ) continue;
            if ( !cand.hasUserFloat("TkTk/Kaon.dEdx.Harmonic") ) continue;
            //if ( !cand.hasUserFloat("TkTk/Proton.dEdx.pixelHrm") ) continue;
            //if ( !cand.hasUserFloat("TkTk/Kaon.dEdx.pixelHrm") ) continue;
            if ( !cand.hasUserData("fitMomentum") ) continue;
            if ( !cand.hasUserData("fitVertex") ) continue;
            if ( !cand.hasUserData("TkTk/Proton.fitMom") ) continue;
            if ( !cand.hasUserData("TkTk/Kaon.fitMom") ) continue;
            if ( !cand.hasUserData("JPsi/MuPos.fitMom") ) continue;
            if ( !cand.hasUserData("JPsi/MuNeg.fitMom") ) continue;
            if ( !cand.hasUserData("refToJPsi") ) continue;

            const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
            if ( _vtx == nullptr ) continue;
            const pat::CompositeCandidate* jpsiCand = usefulFuncs::getByRef<pat::CompositeCandidate>( cand, "refToJPsi" );
            if ( jpsiCand == nullptr ) continue;
            const reco::Vertex* _pv = usefulFuncs::get<reco::Vertex>( *jpsiCand, "fitVertex" );
            if ( _pv == nullptr ) continue;
            double fdbs = usefulFuncs::getFlightDistance ( cand, &bs );
            double fd = usefulFuncs::getFlightDistance ( cand, _pv );
            double cos2d = usefulFuncs::getCosa2d( cand, _pv );
            double cos2dBS = usefulFuncs::getCosa2d( cand, &bs );
            double vtxprob = TMath::Prob( _vtx->chi2(), _vtx->ndof() );
            fillHisto( "parLbTk_vtxprob", vtxprob );
            fillHisto( "parLbTk_cosa2d", cos2d );
            fillHisto( "parLbTk_cosa2dBS", cos2dBS );
            fillHisto( "parLbTk_FlightDistance", fd );
            fillHisto( "parLbTk_FDBS", fdbs );
            fillHisto( "parLbTk_distBS_PV", sqrt(  pow( (_pv->x()-bs.x()), 2 ) + pow( (_pv->y()-bs.y()), 2 )  ) );
            //if ( fd < 0.1 ) continue;
            //if ( cos2d < 0.99 ) continue;
            if ( vtxprob < 0.15 ) continue;
            //if ( cand.userFloat("Proton.IPt") < 0.03 || cand.userFloat("Kaon.IPt") < 0.03 ) continue;
            candsSorted.emplace_back( std::pair<double, const pat::CompositeCandidate*>( TMath::Prob( _vtx->chi2(), _vtx->ndof() ), &cand ) );
            //vtxprobChooser.insert( std::make_pair( TMath::Prob( _vtx->chi2(), _vtx->ndof() ), &cand ) );
        }
        usefulFuncs::sortingByFirstValue( candsSorted );
        std::vector< std::pair< double, const pat::CompositeCandidate*> >::const_iterator iter = candsSorted.begin();
        std::vector< std::pair< double, const pat::CompositeCandidate*> >::const_iterator iend = candsSorted.end  ();
        //int vtxprobSortLimit = 0;
        while ( iter != iend )
        {
            const pat::CompositeCandidate& cand = *(iter++->second);
            //if ( ++vtxprobSortLimit > 20 ) break;
    
            const GlobalVector* pMuMom = cand.userData<GlobalVector>("JPsi/MuPos.fitMom");
            const GlobalVector* nMuMom = cand.userData<GlobalVector>("JPsi/MuNeg.fitMom");
            const GlobalVector* pTkMom = cand.userData<GlobalVector>("TkTk/Proton.fitMom");
            const GlobalVector* nTkMom = cand.userData<GlobalVector>("TkTk/Kaon.fitMom");

            fourMom pTk( pTkMom->x(), pTkMom->y(), pTkMom->z() );
            fourMom nTk( nTkMom->x(), nTkMom->y(), nTkMom->z() );
            fourMom pMu( pMuMom->x(), pMuMom->y(), pMuMom->z() );
            fourMom nMu( nMuMom->x(), nMuMom->y(), nMuMom->z() );
            fillHisto( "parLbTk_pTkMom", pTk.Momentum() );
            fillHisto( "parLbTk_nTkMom", nTk.Momentum() );

            float fMass = cand.userFloat("fitMass");
            fillHisto( "massLbTk_LbTk", fMass );
            if ( fMass > 5.58 && fMass < 5.63 )
            {
                fillHisto( "parLbTk_Harmonic_Signal_ptk" , pTk.Momentum(), cand.userFloat("TkTk/Proton.dEdx.Harmonic") );
                //fillHisto( "parLbTk_PixelHrm_Signal_ptk" , pTk.Momentum(), cand.userFloat("TkTk/Proton.dEdx.pixelHrm") );
                fillHisto( "parLbTk_Harmonic_Signal_ntk" , nTk.Momentum(), cand.userFloat("TkTk/Kaon.dEdx.Harmonic") );
                //fillHisto( "parLbTk_PixelHrm_Signal_ntk" , nTk.Momentum(), cand.userFloat("TkTk/Kaon.dEdx.pixelHrm") );
                fillHisto( "parLbTk_Signal_pIPt" , cand.userFloat("TkTk/Proton.IPt") );
                fillHisto( "parLbTk_Signal_pIPt" , cand.userFloat(  "TkTk/Kaon.IPt") );
                fillHisto( "parLbTk_Signal_pIPt/Err" , cand.userFloat("TkTk/Proton.IPt") / cand.userFloat("TkTk/Proton.IPt.Error") );
                fillHisto( "parLbTk_Signal_pIPt/Err" , cand.userFloat(  "TkTk/Kaon.IPt") / cand.userFloat(  "TkTk/Kaon.IPt.Error") );
            }
            else
            {
                fillHisto( "parLbTk_Harmonic_Backgd_ptk" , pTk.Momentum(), cand.userFloat("TkTk/Proton.dEdx.Harmonic") );
                //fillHisto( "parLbTk_PixelHrm_Backgd_ptk" , pTk.Momentum(), cand.userFloat("TkTk/Proton.dEdx.pixelHrm") );
                fillHisto( "parLbTk_Harmonic_Backgd_ntk" , nTk.Momentum(), cand.userFloat("TkTk/Kaon.dEdx.Harmonic") );
                //fillHisto( "parLbTk_PixelHrm_Backgd_ntk" , nTk.Momentum(), cand.userFloat("TkTk/Kaon.dEdx.pixelHrm") );
                fillHisto( "parLbTk_Backgd_pIPt" , cand.userFloat("TkTk/Proton.IPt") );
                fillHisto( "parLbTk_Backgd_pIPt" , cand.userFloat(  "TkTk/Kaon.IPt") );
                fillHisto( "parLbTk_Backgd_pIPt/Err" , cand.userFloat("TkTk/Proton.IPt") / cand.userFloat("TkTk/Proton.IPt.Error") );
                fillHisto( "parLbTk_Backgd_pIPt/Err" , cand.userFloat(  "TkTk/Kaon.IPt") / cand.userFloat(  "TkTk/Kaon.IPt.Error") );
            }

            pTk.setMass( myMass::protonMass );
            nTk.setMass( myMass::  kaonMass );
            pMu.setMass( myMass::  muonMass );
            nMu.setMass( myMass::  muonMass );
            fourMom twoTk = pTk + nTk;
            fourMom jPsi = pMu + nMu;
            fourMom fourTk = twoTk + jPsi;
            fillHisto( "massLbTk_TkTk", twoTk.Mag() );


            // Lam0
            pTk.setMass( myMass::protonMass );
            nTk.setMass( myMass::  pionMass );
            twoTk = pTk + nTk;
            fourTk = twoTk + jPsi;
            if ( twoTk.Mag() > 1.10 && twoTk.Mag() < 1.15 )
                fillHisto( "massLbTk_FakeLam0", twoTk.Mag() );
            if ( fourTk.Mag() > 5.00 && fourTk.Mag() < 6.00 )
                fillHisto( "massLbTk_FakeLbL0", fourTk.Mag() );
            // Kshort
            pTk.setMass( myMass::  pionMass );
            nTk.setMass( myMass::  pionMass );
            twoTk = pTk + nTk;
            fourTk = twoTk + jPsi;
            if ( twoTk.Mag() > 0.4  && twoTk.Mag() < 0.6 )
                fillHisto( "massLbTk_FakeKshort", twoTk.Mag() );
            // K(892)
            pTk.setMass( myMass::  kaonMass );
            nTk.setMass( myMass::  pionMass );
            twoTk = pTk + nTk;
            fourTk = twoTk + jPsi;
            if ( twoTk.Mag() > 0.75 && twoTk.Mag() < 1.00 )
                fillHisto( "massLbTk_FakeK892", twoTk.Mag() );
            if ( fourTk.Mag() > 5.00 && fourTk.Mag() < 6.00 )
                fillHisto( "massLbTk_FakeBd", fourTk.Mag() );
            // phi1020
            pTk.setMass( myMass::  kaonMass );
            nTk.setMass( myMass::  kaonMass );
            twoTk = pTk + nTk;
            fourTk = twoTk + jPsi;
            if ( twoTk.Mag() > 0.90 && twoTk.Mag() < 1.20 )
                fillHisto( "massLbTk_FakePhi1020", twoTk.Mag() );
            if ( fourTk.Mag() > 5.00 && fourTk.Mag() < 6.00 )
                fillHisto( "massLbTk_FakeBs", fourTk.Mag() );
        }
    } catch (...) {}
}

void histMain_LbTk::Clear()
{
}
