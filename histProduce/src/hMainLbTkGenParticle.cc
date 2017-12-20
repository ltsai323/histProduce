#include "histProduce/histProduce/interface/hMainLbTkGenParticle.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "math.h"


histMain_LbTkGenParticle::histMain_LbTkGenParticle( TFileDirectory* d ) :
    histMain( d, histMain::Label("lbWriteSpecificDecay", "LbToTkTkFitted", "bphAnalysis") ),
    protonMass( 0.9382720813 ), pionMass( 0.13957061 )
{
    createHisto("parLbTkGen_PDGID_ptk", 10000, -5000, 5000 );
    createHisto("parLbTkGen_PDGID_ntk", 10000, -5000, 5000 );
    createHisto("parLbTkGen_minDeltaR", 150, 0., 1.5 );
    createHisto("parLbTkGen_momentumDiffBetweenPairs", 200, -5. ,  5. );
    createHisto("parLbTkGen_ptDiffBetweenPairs", 120, -3. ,  3. );
    createHisto("parLbTkGen_momentumDiffProton", 200, -5. ,  5. );
    createHisto("parLbTkGen_momentumDiffKaon", 200, -5.,  5. );
    createHisto("parLbTkGen_protonDeDx", 100, 0., 5., 100, 0., 10. );
    createHisto("parLbTkGen_kaonDeDx"  , 100, 0., 5., 100, 0., 10. );
    createHisto("parLbTkGen_pionDeDx"  , 100, 0., 5., 100, 0., 10. );
    createHisto("momLbTkGen_proton", 100, 0., 10. );
    createHisto("momLbTkGen_kaon", 100, 0., 10. );
}
void histMain_LbTkGenParticle::Process( fwlite::Event* ev )
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
            if ( !cand.hasUserData("TkTk/Proton.fitMom") ) continue;
            if ( !cand.hasUserData("TkTk/Kaon.fitMom") ) continue;
            if ( !cand.hasUserFloat("TkTk/Proton.dEdx.Harmonic") ) continue;
            if ( !cand.hasUserFloat(  "TkTk/Kaon.dEdx.Harmonic") ) continue;
            //if ( !cand.hasUserFloat("TkTk/Proton.dEdx.pixelHrm") ) continue;
            //if ( !cand.hasUserFloat(  "TkTk/Kaon.dEdx.pixelHrm") ) continue;
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
            
            // first one is proton and second one is kaon ( consider bigger momentum with heavier particle )
            const GlobalVector* dPTR[2] = {nullptr};
            dPTR[0] = cand.userData<GlobalVector>("TkTk/Proton.fitMom");
            dPTR[1] = cand.userData<GlobalVector>("TkTk/Kaon.fitMom");
            
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
                fillHisto( "parLbTkGen_PDGID_ptk", pParticle.second.pdgId() );
                fillHisto( "parLbTkGen_minDeltaR", pParticle.first );
                fillHisto( "parLbTkGen_ptDiffBetweenPairs", pParticle.second.pt() - pTk.transverse() );
                fillHisto( "parLbTkGen_momentumDiffBetweenPairs", pParticle.second.p() - pTk.Momentum() );
                if ( fabs( pParticle.second.pdgId() ) ==  321 )
                {
                    fillHisto( "parLbTkGen_kaonDeDx"  , pTk.Momentum(), cand.userFloat("TkTk/Proton.dEdx.Harmonic") );
                    fillHisto( "parLbTkGen_momentumDiffKaon", pParticle.second.p() - pTk.Momentum() );
                }
                if ( fabs( pParticle.second.pdgId() ) == 2212 )
                {
                    fillHisto( "parLbTkGen_protonDeDx", pTk.Momentum(), cand.userFloat("TkTk/Proton.dEdx.Harmonic") );
                    fillHisto( "parLbTkGen_momentumDiffProton", pParticle.second.p() - pTk.Momentum() );
                    fillHisto( "momLbTkGen_proton", pTk.Momentum() );
                }
                if ( fabs( pParticle.second.pdgId() ) ==  211 )
                    fillHisto( "parLbTkGen_pionDeDx",  pTk.Momentum(), cand.userFloat( "TkTk/Proton.dEdx.Harmonic") );
            }

            if ( nParticle.first < 0.1 )
                if ( fabs( nParticle.second.p() - nTk.Momentum() ) < 0.1 )
            {
                fillHisto( "parLbTkGen_PDGID_ntk", nParticle.second.pdgId() );
                fillHisto( "parLbTkGen_minDeltaR", nParticle.first );
                fillHisto( "parLbTkGen_ptDiffBetweenPairs", nParticle.second.pt() - nTk.transverse() );
                fillHisto( "parLbTkGen_momentumDiffBetweenPairs", nParticle.second.p() - nTk.Momentum() );
                if ( fabs( nParticle.second.pdgId() ) ==  321 )
                {
                    fillHisto( "parLbTkGen_kaonDeDx"  , nTk.Momentum(), cand.userFloat(  "TkTk/Kaon.dEdx.Harmonic") );
                    fillHisto( "parLbTkGen_momentumDiffKaon", nParticle.second.p() - nTk.Momentum() );
                    fillHisto( "momLbTkGen_kaon", nTk.Momentum() );
                }
                if ( fabs( nParticle.second.pdgId() ) == 2212 )
                {
                    fillHisto( "parLbTkGen_protonDeDx", nTk.Momentum(), cand.userFloat(  "TkTk/Kaon.dEdx.Harmonic") );
                    fillHisto( "parLbTkGen_momentumDiffProton", nParticle.second.p() - nTk.Momentum() );
                }
                if ( fabs( nParticle.second.pdgId() ) ==  211 )
                    fillHisto( "parLbTkGen_pionDeDx",  nTk.Momentum(), cand.userFloat(   "TkTk/Kaon.dEdx.Harmonic") );
            }
        }
    } catch (...) {}
}

void histMain_LbTkGenParticle::Clear()
{
}
