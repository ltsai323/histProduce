#include "histProduce/histProduce/interface/hMainLbL0GenParticle.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "math.h"


histMain_LbL0GenParticle::histMain_LbL0GenParticle( TFileDirectory* d ) :
    histMain( d, histMain::Label("lbWriteSpecificDecay", "LbToLam0Fitted", "bphAnalysis"), "LbL0GenInfo" ),
    protonMass( 0.9382720813 ), pionMass( 0.13957061 )
{
    createHisto("parLbL0Gen_PDGID_ptk", 10000, -5000, 5000 );
    createHisto("parLbL0Gen_PDGID_ntk", 10000, -5000, 5000 );
    createHisto("parLbL0Gen_minDeltaR", 150, 0., 1.5 );
    createHisto("parLbL0Gen_momentumDiffBetweenPairs", 200, -5. ,  5. );
    createHisto("parLbL0Gen_ptDiffBetweenPairs", 120, -3. ,  3. );
    createHisto("parLbL0Gen_momentumDiffProton", 200, -5. ,  5. );
    createHisto("parLbL0Gen_protonDeDx", 100, 0., 5., 100, 0., 10. );
    createHisto("parLbL0Gen_pionDeDx"  , 100, 0., 5., 100, 0., 10. );
    createHisto("momLbL0Gen_proton", 100, 0., 10. );
    createHisto("momLbL0Gen_pion", 100, 0., 10. );
}
void histMain_LbL0GenParticle::Process( fwlite::Event* ev )
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
            if ( !cand.hasUserData("Lam0/Proton.fitMom") ) continue;
            if ( !cand.hasUserData("Lam0/Pion.fitMom") ) continue;

            if ( !cand.hasUserFloat("Lam0/Proton.dEdx.Harmonic") )
            { printf( "owns Lam0/Proton Harmonic\n" ); continue; }
            if ( !cand.hasUserFloat(  "Lam0/Pion.dEdx.Harmonic") )
            { printf( "owns Lam0/Pion Harmonic\n" ); continue; }
            if ( !cand.hasUserFloat("Lam0/Proton.dEdx.pixelHrm") )
            { printf( "owns Lam0/Proton pixelHrm\n" ); /*continue;*/ }
            if ( !cand.hasUserFloat(  "Lam0/Pion.dEdx.pixelHrm") )
            { printf( "owns Lam0/Pion pixelHrm\n" ); /*continue;*/ }
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
            if ( cand.userFloat("fitMass") > 5.65 ) continue;
            if ( cand.userFloat("fitMass") > 5.58 ) continue;

            // first one is proton and second one is pion ( consider bigger momentum with heavier particle )
            const GlobalVector* dPTR[2] = {nullptr};
            dPTR[0] = cand.userData<GlobalVector>("Lam0/Proton.fitMom");
            dPTR[1] = cand.userData<GlobalVector>("Lam0/Pion.fitMom");

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
                fillHisto( "parLbL0Gen_PDGID_ptk", pParticle.second.pdgId() );
                fillHisto( "parLbL0Gen_minDeltaR", pParticle.first );
                fillHisto( "parLbL0Gen_ptDiffBetweenPairs", pParticle.second.pt() - pTk.transverse() );
                fillHisto( "parLbL0Gen_momentumDiffBetweenPairs", pParticle.second.p() - pTk.Momentum() );
                if ( fabs( pParticle.second.pdgId() ) ==  321 )
                {
                    fillHisto( "parLbL0Gen_pionDeDx"  , pTk.Momentum(), cand.userFloat("Lam0/Proton.dEdx.Harmonic") );
                    fillHisto( "parLbL0Gen_momentumDiffPion", pParticle.second.p() - pTk.Momentum() );
                }
                if ( fabs( pParticle.second.pdgId() ) == 2212 )
                {
                    fillHisto( "parLbL0Gen_protonDeDx", pTk.Momentum(), cand.userFloat("Lam0/Proton.dEdx.Harmonic") );
                    fillHisto( "parLbL0Gen_momentumDiffProton", pParticle.second.p() - pTk.Momentum() );
                    fillHisto( "momLbL0Gen_proton", pTk.Momentum() );
                }
                //if ( fabs( pParticle.second.pdgId() ) ==  211 )
                    //fillHisto( "parLbL0Gen_pionDeDx",  pTk.Momentum(), cand.userFloat( "Lam0/Proton.dEdx.Harmonic") );
            }

            if ( nParticle.first < 0.1 )
                if ( fabs( nParticle.second.p() - nTk.Momentum() ) < 0.1 )
            {
                fillHisto( "parLbL0Gen_PDGID_ntk", nParticle.second.pdgId() );
                fillHisto( "parLbL0Gen_minDeltaR", nParticle.first );
                fillHisto( "parLbL0Gen_ptDiffBetweenPairs", nParticle.second.pt() - nTk.transverse() );
                fillHisto( "parLbL0Gen_momentumDiffBetweenPairs", nParticle.second.p() - nTk.Momentum() );
                if ( fabs( nParticle.second.pdgId() ) ==  321 )
                {
                    fillHisto( "parLbL0Gen_pionDeDx"  , nTk.Momentum(), cand.userFloat(  "Lam0/Pion.dEdx.Harmonic") );
                    fillHisto( "parLbL0Gen_momentumDiffPion", nParticle.second.p() - nTk.Momentum() );
                    fillHisto( "momLbL0Gen_pion", nTk.Momentum() );
                }
                if ( fabs( nParticle.second.pdgId() ) == 2212 )
                {
                    fillHisto( "parLbL0Gen_protonDeDx", nTk.Momentum(), cand.userFloat(  "Lam0/Pion.dEdx.Harmonic") );
                    fillHisto( "parLbL0Gen_momentumDiffProton", nParticle.second.p() - nTk.Momentum() );
                }
                //if ( fabs( nParticle.second.pdgId() ) ==  211 )
                    //fillHisto( "parLbL0Gen_pionDeDx",  nTk.Momentum(), cand.userFloat(   "Lam0/Pion.dEdx.Harmonic") );
            }
        }
    } catch (...) {}
}

void histMain_LbL0GenParticle::Clear()
{
}
