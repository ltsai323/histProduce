#include "histProduce/histProduce/interface/hMainLam0GenParticle.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "math.h"


histMain_Lam0GenParticle::histMain_Lam0GenParticle( TFileDirectory* d ) :
    histMain( d, histMain::Label("lbWriteSpecificDecay", "Lam0Fitted", "bphAnalysis"), "Lam0GenInfo" ),
    protonMass( 0.9382720813 ), pionMass( 0.13957061 )
{
    createHisto("parLam0Gen_PDGID_ptk", 10000, -5000, 5000 );
    createHisto("parLam0Gen_PDGID_ntk", 10000, -5000, 5000 );
    createHisto("parLam0Gen_minDeltaR", 150, 0., 1.5 );
    createHisto("parLam0Gen_momentumDiffBetweenPairs", 200, -5. ,  5. );
    createHisto("parLam0Gen_ptDiffBetweenPairs", 120, -3. ,  3. );
    createHisto("parLam0Gen_momentumDiffProton", 200, -5. ,  5. );
    createHisto("parLam0Gen_momentumDiffPion", 200, -5.,  5. );
    createHisto("parLam0Gen_protonDeDx", 100, 0., 5., 100, 0., 10. );
    createHisto("parLam0Gen_pionDeDx"  , 100, 0., 5., 100, 0., 10. );
}
void histMain_Lam0GenParticle::Process( fwlite::Event* ev )
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
            if ( !cand.hasUserData("Pion.fitMom") ) continue;
            //if ( !cand.hasUserFloat("Proton.dEdx.Harmonic") ) continue;
            //if ( !cand.hasUserFloat(  "Pion.dEdx.Harmonic") ) continue;
            //if ( !cand.hasUserFloat("Proton.dEdx.pixelHrm") ) continue;
            //if ( !cand.hasUserFloat(  "Pion.dEdx.pixelHrm") ) continue;
            if ( !cand.hasUserFloat("Proton.dEdx.Harmonic") )
            { printf( "owns Proton Harmonic\n" ); continue; }
            if ( !cand.hasUserFloat(  "Pion.dEdx.Harmonic") )
            { printf( "owns Pion Harmonic\n" ); continue; }
            if ( !cand.hasUserFloat("Proton.dEdx.pixelHrm") )
            { printf( "owns Proton pixelHrm\n" ); /*continue;*/ }
            if ( !cand.hasUserFloat(  "Pion.dEdx.pixelHrm") )
            { printf( "owns Pion pixelHrm\n" ); /*continue;*/ }
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
            fillHisto("massLam0_Lam0", cand.userFloat("fitMass") );

            // first one is proton and second one is pion ( consider bigger momentum with heavier particle )
            const GlobalVector* dPTR[2] = {nullptr};
            dPTR[0] = cand.userData<GlobalVector>("Proton.fitMom");
            dPTR[1] = cand.userData<GlobalVector>("Pion.fitMom");
            //fillHisto("ptLam0_Proton", dPTR[0]->transverse() );
            //fillHisto("ptLam0_Pion"  , dPTR[1]->transverse() );

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
                fillHisto( "parLam0Gen_PDGID_ptk", pParticle.second.pdgId() );
                fillHisto( "parLam0Gen_minDeltaR", pParticle.first );
                fillHisto( "parLam0Gen_ptDiffBetweenPairs", pParticle.second.pt() - pTk.transverse() );
                fillHisto( "parLam0Gen_momentumDiffBetweenPairs", pParticle.second.p() - pTk.Momentum() );
                if ( fabs( pParticle.second.pdgId() ) ==  321 )
                {
                    fillHisto( "parLam0Gen_pionDeDx"  , pTk.Momentum(), cand.userFloat("Proton.dEdx.Harmonic") );
                    fillHisto( "parLam0Gen_momentumDiffPion", pParticle.second.p() - pTk.Momentum() );
                }
                if ( fabs( pParticle.second.pdgId() ) == 2212 )
                {
                    fillHisto( "parLam0Gen_protonDeDx", pTk.Momentum(), cand.userFloat("Proton.dEdx.Harmonic") );
                    fillHisto( "parLam0Gen_momentumDiffProton", pParticle.second.p() - pTk.Momentum() );
                }
                if ( fabs( pParticle.second.pdgId() ) ==  211 )
                    fillHisto( "parLam0Gen_pionDeDx",  pTk.Momentum(), cand.userFloat( "Proton.dEdx.Harmonic") );
            }

            if ( nParticle.first < 0.1 )
                if ( fabs( nParticle.second.p() - nTk.Momentum() ) < 0.1 )
            {
                fillHisto( "parLam0Gen_PDGID_ntk", nParticle.second.pdgId() );
                fillHisto( "parLam0Gen_minDeltaR", nParticle.first );
                fillHisto( "parLam0Gen_ptDiffBetweenPairs", nParticle.second.pt() - nTk.transverse() );
                fillHisto( "parLam0Gen_momentumDiffBetweenPairs", nParticle.second.p() - nTk.Momentum() );
                if ( fabs( nParticle.second.pdgId() ) ==  321 )
                {
                    fillHisto( "parLam0Gen_pionDeDx"  , nTk.Momentum(), cand.userFloat(  "Pion.dEdx.Harmonic") );
                    fillHisto( "parLam0Gen_momentumDiffPion", nParticle.second.p() - nTk.Momentum() );
                }
                if ( fabs( nParticle.second.pdgId() ) == 2212 )
                {
                    fillHisto( "parLam0Gen_protonDeDx", nTk.Momentum(), cand.userFloat(  "Pion.dEdx.Harmonic") );
                    fillHisto( "parLam0Gen_momentumDiffProton", nParticle.second.p() - nTk.Momentum() );
                }
                if ( fabs( nParticle.second.pdgId() ) ==  211 )
                    fillHisto( "parLam0Gen_pionDeDx",  nTk.Momentum(), cand.userFloat(   "Pion.dEdx.Harmonic") );
            }
        }
    } catch (...) {}
}

void histMain_Lam0GenParticle::Clear()
{
}
