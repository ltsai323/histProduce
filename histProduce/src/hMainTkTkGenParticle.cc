#include "histProduce/histProduce/interface/hMainTkTkGenParticle.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "math.h"


histMain_TkTkGenParticle::histMain_TkTkGenParticle( TFileDirectory* d ) :
    histMain( d, histMain::Label("lbWriteSpecificDecay", "TkTkFitted", "bphAnalysis") ),
    protonMass( 0.9382720813 ), pionMass( 0.13957061 )
{
    createHisto("candInEvent_TkTkGen", 100, 0., 100. );
    createHisto("candInVtxsort_TkTkGen", 100, 0., 100.);
    createHisto("candInGenHandle_TkTkGen", 1000, 0., 1000.);
    createHisto("parTkTkGen_PDGID_ptk", 10000, -5000, 5000 );
    createHisto("parTkTkGen_PDGID_ntk", 10000, -5000, 5000 );
    createHisto("parTkTkGen_eventLost", 10, 0., 10. );
    createHisto("parTkTkGen_minDeltaR", 1000, 0., 100. );
}
void histMain_TkTkGenParticle::Process( fwlite::Event* ev )
{
    try 
    {
        int nnn=0;
        fillHisto( "parTkTkGen_eventLost", nnn++ );
        if ( !ev->isValid() ) return;
        _handle.getByLabel( *ev, _label.module.c_str(), _label.label.c_str(), _label.process.c_str() );
        beamSpotHandle.getByLabel( *ev,"offlineBeamSpot", "", "RECO"  );
        genHandle.getByLabel( *ev, "genParticles", "", "HLT" );
        fillHisto( "parTkTkGen_eventLost", nnn++ );


        if ( !beamSpotHandle.isValid() ) return;
        fillHisto( "parTkTkGen_eventLost", nnn++ );
        if ( !genHandle.isValid() ) return;
        fillHisto( "candInGenHandle_TkTkGen", genHandle->size() );
        fillHisto( "parTkTkGen_eventLost", nnn++ );
        if ( _handle->size()  == 0 ) return;
        fillHisto( "parTkTkGen_eventLost", nnn++ );
        if ( genHandle->size() == 0 ) return;
        fillHisto( "parTkTkGen_eventLost", nnn++ );
        const reco::Vertex bs( (*beamSpotHandle).position(), (*beamSpotHandle).covariance3D() );
    
        //std::map< double, const pat::CompositeCandidate*> vtxprobChooser;
        std::vector< std::pair< double, const pat::CompositeCandidate*> > candsSorted;
        candsSorted.reserve( _handle->size() );
        //vtxprobChooser.clear();
        std::vector<pat::CompositeCandidate>::const_iterator handleIter = _handle->begin();
        std::vector<pat::CompositeCandidate>::const_iterator handleIend = _handle->end  ();
        while( handleIter != handleIend ) // preselection {{{
        {
            const pat::CompositeCandidate& cand = *handleIter++;
            if ( !cand.hasUserFloat("fitMass") ) continue;
            if ( !cand.hasUserData("Proton.fitMom") ) continue;
            if ( !cand.hasUserData("Kaon.fitMom") ) continue;
            //if ( !cand.hasUserFloat("Proton.dEdx.Harmonic") ) continue;
            //if ( !cand.hasUserFloat(  "Kaon.dEdx.Harmonic") ) continue;
            //if ( !cand.hasUserFloat("Proton.dEdx.pixelHrm") ) continue;
            //if ( !cand.hasUserFloat(  "Kaon.dEdx.pixelHrm") ) continue;
            const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
            if ( _vtx == nullptr ) continue;
            double fd = usefulFuncs::getFlightDistance ( cand, &bs );
            double cos2d = usefulFuncs::getCosa2d( cand, &bs );
            fillHisto( "parTkTk_FlightDistance", fd );
            fillHisto( "parTkTk_cosa2d", cos2d );
            double vtxprob = TMath::Prob( _vtx->chi2(), _vtx->ndof() );
            fillHisto( "parTkTk_vtxprob", vtxprob );
            //if ( fd < 0.1 ) continue;
            //if ( cos2d < 0.99 ) continue;
            //if ( vtxprob < 0.1 ) continue;
            candsSorted.emplace_back( vtxprob, &cand );
        } // preselection end }}}
        usefulFuncs::sortingByFirstValue( candsSorted );
        if ( candsSorted.size() == 0 ) return;
        
        fillHisto( "parTkTkGen_eventLost", nnn++ );


        fillHisto("candInEvent_TkTkGen", _handle->size() );
        fillHisto("candInVtxsort_TkTkGen", candsSorted.size() );

        std::vector< std::pair<double, const pat::CompositeCandidate*> >::const_iterator iter = candsSorted.begin();
        std::vector< std::pair<double, const pat::CompositeCandidate*> >::const_iterator iend = candsSorted.end  ();

        //double kaonMass ( 0.493667 );
        int getFirstNEvent = 0;
        while ( iter != iend )
        {
            //if ( ++getFirstNEvent > 20 ) break;
            const pat::CompositeCandidate& cand = *(iter++->second);
            //fillHisto("massTkTk_TkTk", cand.userFloat("fitMass") );
            
            // first one is proton and second one is kaon ( consider bigger momentum with heavier particle )
            const GlobalVector* dPTR[2] = {nullptr};
            dPTR[0] = cand.userData<GlobalVector>("Proton.fitMom");
            dPTR[1] = cand.userData<GlobalVector>("Kaon.fitMom");
            //fillHisto("ptTkTk_Proton", dPTR[0]->transverse() );
            //fillHisto("ptTkTk_Kaon"  , dPTR[1]->transverse() );
            
            fourMom pTk ( dPTR[0]->x(), dPTR[0]->y(), dPTR[0]->z() );
            fourMom nTk ( dPTR[1]->x(), dPTR[1]->y(), dPTR[1]->z() );
            const reco::GenParticle pParticle = searchForGenParticle( &pTk, 2.0 );
            const reco::GenParticle nParticle = searchForGenParticle( &nTk, 2.0 );
            //if ( !pParticle.status() ) continue;
            //if ( !nParticle.status() ) continue;
            fillHisto( "parTkTkGen_PDGID_ptk", pParticle.pdgId() );
            fillHisto( "parTkTkGen_PDGID_ntk", nParticle.pdgId() );
//            // reconstruct lambda0
//            pTk.setMass( protonMass );
//            nTk.setMass(   pionMass );
//
//
//            fourMom twoTk = pTk + nTk;
//            double mass = twoTk.Mag();
//            int specialTag = 0;
//            if ( mass > 1.10 && mass < 1.15 )
//                fillHisto( "massTkTk_FakeLam0", mass );
//            if ( mass - protonMass - pionMass < 0.01 )
//            {
//                specialTag += 1 << 1;
//                fillHisto( "specialPtTkTk_FakeLam0", twoTk.transverse() );
//            }
//            // reconstruct k short
//            pTk.setMass( pionMass );
//            twoTk = pTk + nTk;
//            mass = twoTk.Mag();
//            if ( mass > 0.40 && mass < 0.6 )
//            {
//                fillHisto( "massTkTk_FakeKshort", mass );
//                const reco::Candidate* trackMom[2] = {nullptr};
//                trackMom[0] = cand.daughter(0)->pt() > cand.daughter(1)->pt() ? cand.daughter(0) : cand.daughter(1);
//                trackMom[1] = cand.daughter(0)->pt() < cand.daughter(1)->pt() ? cand.daughter(0) : cand.daughter(1);
//                fourMom origPTk ( trackMom[0]->px(), trackMom[0]->py(), trackMom[0]->pz() );
//                fourMom origNTk ( trackMom[1]->px(), trackMom[1]->py(), trackMom[1]->pz() );
//                origPTk.setMass(   pionMass );
//                origNTk.setMass(   pionMass );
//                fourMom origTwoTrack = origPTk + origNTk;
//                fillHisto( "massSpecialTkTk_findoutDiffFromOrigToReVtx", fabs( origTwoTrack.Mag() - twoTk.Mag() ) );
//                if ( mass > 0.49 && mass < 0.51 )
//                {
//                    fillHisto( "parTkTk_PixelHrm_proton", pTk.Momentum(), cand.userFloat("Proton.dEdx.pixelHrm") );
//                    fillHisto( "parTkTk_PixelHrm_pion"  , nTk.Momentum(), cand.userFloat(  "Kaon.dEdx.pixelHrm") );
//                    fillHisto( "parTkTk_Harmonic_proton", pTk.Momentum(), cand.userFloat("Proton.dEdx.Harmonic") );
//                    fillHisto( "parTkTk_Harmonic_pion"  , nTk.Momentum(), cand.userFloat(  "Kaon.dEdx.Harmonic") );
//                }
//            }
//
//            if ( mass - 2*pionMass < 0.01 )
//            {
//                specialTag += 1 << 2;
//                fillHisto( "specialPtTkTk_FakeKshort", twoTk.transverse() );
//            }
//            if ( specialTag )
//                fillHisto( "specialParTkTk", specialTag );
//
//            double pIP( cand.userFloat("Proton.IPt") );
//            double nIP( cand.userFloat("Kaon.IPt") );
//            double pIPE( cand.userFloat("Proton.IPt.Error") );
//            double nIPE( cand.userFloat("Kaon.IPt.Error") );
//            fillHisto( "parTkTk_pIPt", pIP );
//            fillHisto( "parTkTk_nIPt", nIP );
//            fillHisto( "parTkTk_pIPt/Err", pIP/pIPE );
//            fillHisto( "parTkTk_nIPt/Err", nIP/nIPE );
        }
    } catch (...) {}
}

void histMain_TkTkGenParticle::Clear()
{
}
