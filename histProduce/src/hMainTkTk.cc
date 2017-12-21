#include "histProduce/histProduce/interface/hMainTkTk.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "math.h"


histMain_TkTk::histMain_TkTk( TFileDirectory* d ) :
    histMain( d, histMain::Label("lbWriteSpecificDecay", "TkTkFitted", "bphAnalysis"), "TkTk" )
{
    createHisto( "candInEvent_TkTk"         , 160,  0.0, 1600.);
    createHisto( "candInVtxsort_TkTk"       ,  40,  0.0, 400.);
    createHisto( "parTkTk_cosa2d"           , 100, -1.0, 1.0 );
    createHisto( "parTkTk_vtxprob"          , 100,   0.,   1.);
    createHisto( "parTkTk_FlightDistance"   , 105,-0.05,1.000);
    createHisto( "ptTkTk_Proton"            , 100,   0., 20. );
    createHisto( "ptTkTk_Kaon"              , 100,   0., 20. );
    createHisto( "massTkTk_TkTk"            , 100, 1.40, 2.20);
    createHisto( "massTkTk_FakeLam0"        ,  50, 1.10, 1.15);
    createHisto( "massTkTk_FakeKshort"      ,  40,  0.4, 0.6 );
    createHisto( "specialPtTkTk_FakeLam0", 150, 0., 30. );
    createHisto( "specialPtTkTk_FakeKshort", 150, 0., 30. );
    createHisto( "specialParTkTk", 8, 0., 8. );
    createHisto( "massSpecialTkTk_findoutDiffFromOrigToReVtx", 200, 0., 1.);

    createHisto( "parTkTk_Harmonic_proton", 100, 0., 20., 100, 0., 20. );
    createHisto( "parTkTk_PixelHrm_proton", 100, 0., 20., 100, 0., 20. );
    createHisto( "parTkTk_Harmonic_pion"  , 100, 0., 20., 100, 0., 20. );
    createHisto( "parTkTk_PixelHrm_pion"  , 100, 0., 20., 100, 0., 20. );
    createHisto( "parTkTk_pIPt", 200, 0, 0.4 );
    createHisto( "parTkTk_nIPt", 200, 0, 0.4 );
    createHisto( "parTkTk_pIPt/Err", 200, 0, 5. );
    createHisto( "parTkTk_nIPt/Err", 200, 0, 5. );
}
void histMain_TkTk::Process( fwlite::Event* ev )
{
    try
    {
        if ( !ev->isValid() ) return;
        _handle.getByLabel( *ev, _label.module.c_str(), _label.label.c_str(), _label.process.c_str() );

        // pv not recorded, use BS
        fwlite::Handle< reco::BeamSpot > beamSpotHandle;
        beamSpotHandle.getByLabel( *ev,"offlineBeamSpot", "", "RECO"  );
        if ( !beamSpotHandle.isValid() ) return;
        if ( _handle->size()  == 0 ) return;
        const reco::Vertex bs( (*beamSpotHandle).position(), (*beamSpotHandle).covariance3D() );

        std::map< double, const pat::CompositeCandidate*> vtxprobChooser;
        vtxprobChooser.clear();
        std::vector<pat::CompositeCandidate>::const_iterator handleIter = _handle->begin();
        std::vector<pat::CompositeCandidate>::const_iterator handleIend = _handle->end  ();
        while( handleIter != handleIend )
        {
            const pat::CompositeCandidate& cand = *handleIter++;
            if ( !cand.hasUserFloat("fitMass") ) continue;
            if ( !cand.hasUserData("Proton.fitMom") ) continue;
            if ( !cand.hasUserData("Kaon.fitMom") ) continue;
            if ( !cand.hasUserFloat("Proton.dEdx.Harmonic") ) continue;
            if ( !cand.hasUserFloat(  "Kaon.dEdx.Harmonic") ) continue;
            if ( !cand.hasUserFloat("Proton.dEdx.pixelHrm") ) continue;
            if ( !cand.hasUserFloat(  "Kaon.dEdx.pixelHrm") ) continue;
            const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
            if ( _vtx == nullptr ) continue;
            double fd = usefulFuncs::getFlightDistance ( cand, &bs );
            double cos2d = usefulFuncs::getCosa2d( cand, &bs );
            fillHisto( "parTkTk_FlightDistance", fd );
            fillHisto( "parTkTk_cosa2d", cos2d );
            double vtxprob = TMath::Prob( _vtx->chi2(), _vtx->ndof() );
            fillHisto( "parTkTk_vtxprob", vtxprob );
            if ( fd < 0.1 ) continue;
            if ( cos2d < 0.99 ) continue;
            if ( vtxprob < 0.1 ) continue;
            vtxprobChooser.insert( std::make_pair( vtxprob, &cand ) );
        }
        if ( vtxprobChooser.size() == 0 ) return;
        fillHisto("candInEvent_TkTk", _handle->size() );
        fillHisto("candInVtxsort_TkTk", vtxprobChooser.size() );

        std::map< double, const pat::CompositeCandidate*>::const_reverse_iterator iter = vtxprobChooser.rbegin();
        std::map< double, const pat::CompositeCandidate*>::const_reverse_iterator iend = vtxprobChooser.rend  ();

        //double kaonMass ( 0.493667 );
        double protonMass ( 0.9382720813 );
        double pionMass ( 0.13957061 );
        while ( iter != iend )
        {
            const pat::CompositeCandidate& cand = *(iter++->second);
            fillHisto("massTkTk_TkTk", cand.userFloat("fitMass") );

            // first one is proton and second one is kaon ( consider bigger momentum with heavier particle )
            const GlobalVector* dPTR[2] = {nullptr};
            dPTR[0] = cand.userData<GlobalVector>("Proton.fitMom");
            dPTR[1] = cand.userData<GlobalVector>("Kaon.fitMom");
            fillHisto("ptTkTk_Proton", dPTR[0]->transverse() );
            fillHisto("ptTkTk_Kaon"  , dPTR[1]->transverse() );


            fourMom pTk ( dPTR[0]->x(), dPTR[0]->y(), dPTR[0]->z() );
            fourMom nTk ( dPTR[1]->x(), dPTR[1]->y(), dPTR[1]->z() );
            // reconstruct lambda0
            pTk.setMass( protonMass );
            nTk.setMass(   pionMass );


            fourMom twoTk = pTk + nTk;
            double mass = twoTk.Mag();
            int specialTag = 0;
            if ( mass > 1.10 && mass < 1.15 )
                fillHisto( "massTkTk_FakeLam0", mass );
            if ( mass - protonMass - pionMass < 0.01 )
            {
                specialTag += 1 << 1;
                fillHisto( "specialPtTkTk_FakeLam0", twoTk.transverse() );
            }
            // reconstruct k short
            pTk.setMass( pionMass );
            twoTk = pTk + nTk;
            mass = twoTk.Mag();
            if ( mass > 0.40 && mass < 0.6 )
            {
                fillHisto( "massTkTk_FakeKshort", mass );
                const reco::Candidate* trackMom[2] = {nullptr};
                trackMom[0] = cand.daughter(0)->pt() > cand.daughter(1)->pt() ? cand.daughter(0) : cand.daughter(1);
                trackMom[1] = cand.daughter(0)->pt() < cand.daughter(1)->pt() ? cand.daughter(0) : cand.daughter(1);
                fourMom origPTk ( trackMom[0]->px(), trackMom[0]->py(), trackMom[0]->pz() );
                fourMom origNTk ( trackMom[1]->px(), trackMom[1]->py(), trackMom[1]->pz() );
                origPTk.setMass(   pionMass );
                origNTk.setMass(   pionMass );
                fourMom origTwoTrack = origPTk + origNTk;
                fillHisto( "massSpecialTkTk_findoutDiffFromOrigToReVtx", fabs( origTwoTrack.Mag() - twoTk.Mag() ) );
                if ( mass > 0.49 && mass < 0.51 )
                {
                    fillHisto( "parTkTk_PixelHrm_proton", pTk.Momentum(), cand.userFloat("Proton.dEdx.pixelHrm") );
                    fillHisto( "parTkTk_PixelHrm_pion"  , nTk.Momentum(), cand.userFloat(  "Kaon.dEdx.pixelHrm") );
                    fillHisto( "parTkTk_Harmonic_proton", pTk.Momentum(), cand.userFloat("Proton.dEdx.Harmonic") );
                    fillHisto( "parTkTk_Harmonic_pion"  , nTk.Momentum(), cand.userFloat(  "Kaon.dEdx.Harmonic") );
                }
            }

            if ( mass - 2*pionMass < 0.01 )
            {
                specialTag += 1 << 2;
                fillHisto( "specialPtTkTk_FakeKshort", twoTk.transverse() );
            }
            if ( specialTag )
                fillHisto( "specialParTkTk", specialTag );

            double pIP( cand.userFloat("Proton.IPt") );
            double nIP( cand.userFloat("Kaon.IPt") );
            double pIPE( cand.userFloat("Proton.IPt.Error") );
            double nIPE( cand.userFloat("Kaon.IPt.Error") );
            fillHisto( "parTkTk_pIPt", pIP );
            fillHisto( "parTkTk_nIPt", nIP );
            fillHisto( "parTkTk_pIPt/Err", pIP/pIPE );
            fillHisto( "parTkTk_nIPt/Err", nIP/nIPE );
        }
    } catch (...) {}
}

void histMain_TkTk::Clear()
{
}
//double histMain_TkTk::getFlightDistance( const pat::CompositeCandidate& cand, const reco::Vertex* _bs )
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
//double histMain_TkTk::getCosa2d( const pat::CompositeCandidate& cand, const reco::Vertex* _bs )
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
