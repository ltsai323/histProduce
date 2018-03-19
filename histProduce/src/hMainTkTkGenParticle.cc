#include "histProduce/histProduce/interface/hMainTkTkGenParticle.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "math.h"


histMain_TkTkGenParticle::histMain_TkTkGenParticle( TFileDirectory* d ) :
    histMain( d, histMain::Label("lbWriteSpecificDecay", "TkTkFitted", "bphAnalysis"), "TkTkGenInfo" ),
    protonMass( 0.9382720813 ), pionMass( 0.13957061 )
{
    createHisto("parTkTkGen_PDGID_ptk", 10000, -5000, 5000 );
    createHisto("parTkTkGen_PDGID_ntk", 10000, -5000, 5000 );
    createHisto("parTkTkGen_minDeltaR", 150, 0., 1.5 );
    createHisto("parTkTkGen_momentumDiffBetweenPairs", 200, -5. ,  5. );
    createHisto("parTkTkGen_ptDiffBetweenPairs", 120, -3. ,  3. );
    createHisto("parTkTkGen_momentumDiffProton", 200, -5. ,  5. );
    createHisto("parTkTkGen_momentumDiffKaon", 200, -5.,  5. );
    createHisto("parTkTkGen_protonDeDx", 100, 0., 5., 100, 0., 10. );
    createHisto("parTkTkGen_kaonDeDx"  , 100, 0., 5., 100, 0., 10. );
    createHisto("parTkTkGen_pionDeDx"  , 100, 0., 5., 100, 0., 10. );
}
void histMain_TkTkGenParticle::Process( fwlite::Event* ev )
{
    try
    {
        if ( !ev->isValid() ) return;
        _handle.getByLabel( *ev, _label.module.c_str(), _label.label.c_str(), _label.process.c_str() );
        beamSpotHandle.getByLabel( *ev,"offlineBeamSpot", "", "RECO"  );
        genHandle.getByLabel( *ev, "genParticles", "", "HLT" );


        if ( !beamSpotHandle.isValid() ) return;
        if ( !genHandle.isValid() ) return;
        if ( _handle->size()  == 0 ) return;
        if ( genHandle->size() == 0 ) return;
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
            if ( !cand.hasUserFloat("Proton.dEdx.Harmonic") ) continue;
            if ( !cand.hasUserFloat(  "Kaon.dEdx.Harmonic") ) continue;
            //if ( !cand.hasUserFloat("Proton.dEdx.pixelHrm") ) continue;
            //if ( !cand.hasUserFloat(  "Kaon.dEdx.pixelHrm") ) continue;
            const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
            if ( _vtx == nullptr ) continue;
            double vtxprob = TMath::Prob( _vtx->chi2(), _vtx->ndof() );
            candsSorted.emplace_back( vtxprob, &cand );
        } // preselection end }}}
        usefulFuncs::sortingByFirstValue( candsSorted );
        if ( candsSorted.size() == 0 ) return;

        std::vector< std::pair<double, const pat::CompositeCandidate*> >::const_iterator iter = candsSorted.begin();
        std::vector< std::pair<double, const pat::CompositeCandidate*> >::const_iterator iend = candsSorted.end  ();

        while ( iter != iend )
        {
            const pat::CompositeCandidate& cand = *(iter++->second);
            fillHisto("massTkTk_TkTk", cand.userFloat("fitMass") );

            // first one is proton and second one is kaon ( consider bigger momentum with heavier particle )
            const GlobalVector* dPTR[2] = {nullptr};
            dPTR[0] = cand.userData<GlobalVector>("Proton.fitMom");
            dPTR[1] = cand.userData<GlobalVector>("Kaon.fitMom");
            //fillHisto("ptTkTk_Proton", dPTR[0]->transverse() );
            //fillHisto("ptTkTk_Kaon"  , dPTR[1]->transverse() );

            fourMom pTk ( dPTR[0]->x(), dPTR[0]->y(), dPTR[0]->z() );
            fourMom nTk ( dPTR[1]->x(), dPTR[1]->y(), dPTR[1]->z() );
            const std::pair< double, reco::GenParticle>& pParticle = searchForGenParticle( &pTk );
            const std::pair< double, reco::GenParticle>& nParticle = searchForGenParticle( &nTk );
            // if particle failed to match, skip this event
            //if ( !pParticle.status() ) continue;
            //if ( !nParticle.status() ) continue;
            if ( pParticle.first < 0.1 )
                if ( fabs( pParticle.second.p() - pTk.Momentum() ) < 0.1 )
            {
                fillHisto( "parTkTkGen_PDGID_ptk", pParticle.second.pdgId() );
                fillHisto( "parTkTkGen_minDeltaR", pParticle.first );
                fillHisto( "parTkTkGen_ptDiffBetweenPairs", pParticle.second.pt() - pTk.transverse() );
                fillHisto( "parTkTkGen_momentumDiffBetweenPairs", pParticle.second.p() - pTk.Momentum() );
                if ( fabs( pParticle.second.pdgId() ) ==  321 )
                {
                    fillHisto( "parTkTkGen_kaonDeDx"  , pTk.Momentum(), cand.userFloat("Proton.dEdx.Harmonic") );
                    fillHisto( "parTkTkGen_momentumDiffKaon", pParticle.second.p() - pTk.Momentum() );
                }
                if ( fabs( pParticle.second.pdgId() ) == 2212 )
                {
                    fillHisto( "parTkTkGen_protonDeDx", pTk.Momentum(), cand.userFloat("Proton.dEdx.Harmonic") );
                    fillHisto( "parTkTkGen_momentumDiffProton", pParticle.second.p() - pTk.Momentum() );
                }
                if ( fabs( pParticle.second.pdgId() ) ==  211 )
                    fillHisto( "parTkTkGen_pionDeDx",  pTk.Momentum(), cand.userFloat( "Proton.dEdx.Harmonic") );
            }

            if ( nParticle.first < 0.1 )
                if ( fabs( nParticle.second.p() - nTk.Momentum() ) < 0.1 )
            {
                fillHisto( "parTkTkGen_PDGID_ntk", nParticle.second.pdgId() );
                fillHisto( "parTkTkGen_minDeltaR", nParticle.first );
                fillHisto( "parTkTkGen_ptDiffBetweenPairs", nParticle.second.pt() - nTk.transverse() );
                fillHisto( "parTkTkGen_momentumDiffBetweenPairs", nParticle.second.p() - nTk.Momentum() );
                if ( fabs( nParticle.second.pdgId() ) ==  321 )
                {
                    fillHisto( "parTkTkGen_kaonDeDx"  , nTk.Momentum(), cand.userFloat(  "Kaon.dEdx.Harmonic") );
                    fillHisto( "parTkTkGen_momentumDiffKaon", nParticle.second.p() - nTk.Momentum() );
                }
                if ( fabs( nParticle.second.pdgId() ) == 2212 )
                {
                    fillHisto( "parTkTkGen_protonDeDx", nTk.Momentum(), cand.userFloat(  "Kaon.dEdx.Harmonic") );
                    fillHisto( "parTkTkGen_momentumDiffProton", nParticle.second.p() - nTk.Momentum() );
                }
                if ( fabs( nParticle.second.pdgId() ) ==  211 )
                    fillHisto( "parTkTkGen_pionDeDx",  nTk.Momentum(), cand.userFloat(   "Kaon.dEdx.Harmonic") );
            }
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
