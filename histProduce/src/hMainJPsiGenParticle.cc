#include "histProduce/histProduce/interface/hMainJPsiGenParticle.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "math.h"


histMain_JPsiGenParticle::histMain_JPsiGenParticle( TFileDirectory* d ) :
    histMain( d, histMain::Label("lbWriteSpecificDecay", "oniaFitted", "bphAnalysis") ),
    protonMass( 0.9382720813 ), pionMass( 0.13957061 )
{

    createHisto("parJPsiGen_PDGID_MuPos", 10000, -5000, 5000 );
    createHisto("parJPsiGen_PDGID_MuNeg", 10000, -5000, 5000 );
    createHisto("parJPsiGen_minDeltaR", 150, 0., 1.5 );
    createHisto("parJPsiGen_ptDiffBetweenPairs", 120, -3. ,  3. );
    createHisto("parJPsiGen_momentumDiffBetweenPairs", 200, -5. ,  5. );
    createHisto("parJPsiGen_momentumDiffMuNeg", 200, -5. ,  5. );
    createHisto("parJPsiGen_momentumDiffMuPos", 200, -5.,  5. );
    createHisto("parJPsiGen_MuPosDeDx"  , 100, 0., 5., 100, 0., 10. );
    createHisto("parJPsiGen_MuNegDeDx"  , 100, 0., 5., 100, 0., 10. );
}
void histMain_JPsiGenParticle::Process( fwlite::Event* ev )

    try 
    {
        if ( !ev->isValid() ) return;
        _handle.getByLabel( *ev, _label.module.c_str(), _label.label.c_str(), _label.process.c_str() );
        genHandle.getByLabel( *ev, "genParticles", "", "HLT" );


        if ( !genHandle.isValid() ) return;
        if ( _handle->size()  == 0 ) return;
        if ( genHandle->size() == 0 ) return;
    
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
            if ( !cand.hasUserData("MuPos.fitMom") ) continue;
            if ( !cand.hasUserData("MuNeg.fitMom") ) continue;
            if ( !cand.hasUserFloat("MuPos.dEdx.Harmonic") ) continue;
            if ( !cand.hasUserFloat("MuNeg.dEdx.Harmonic") ) continue;
            if ( !cand.hasUserData("primaryVertex") ) continue;
            const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
            if ( _vtx == nullptr ) continue;
            candsSorted.emplace_back( vtxprob, &cand );
        } // preselection end }}}
        usefulFuncs::sortingByFirstValue( candsSorted );
        if ( candsSorted.size() == 0 ) return;
        

        std::vector< std::pair<double, const pat::CompositeCandidate*> >::const_iterator iter = candsSorted.begin();
        std::vector< std::pair<double, const pat::CompositeCandidate*> >::const_iterator iend = candsSorted.end  ();

        while ( iter != iend )
        {
            const pat::CompositeCandidate& cand = *(iter++->second);
            
            // first one is proton and second one is kaon ( consider bigger momentum with heavier particle )
            const GlobalVector* dPTR[2] = {nullptr};
            dPTR[0] = cand.userData<GlobalVector>("MuPos.fitMom");
            dPTR[1] = cand.userData<GlobalVector>("MuNeg.fitMom");
            //fillHisto("ptTkTk_MuPos", dPTR[0]->transverse() );
            //fillHisto("ptTkTk_MuNeg"  , dPTR[1]->transverse() );
            
            fourMom pTk ( dPTR[0]->x(), dPTR[0]->y(), dPTR[0]->z() );
            fourMom nTk ( dPTR[1]->x(), dPTR[1]->y(), dPTR[1]->z() );
            const std::pair< double, reco::GenParticle>& pParticle = searchForGenParticle( &pTk );
            const std::pair< double, reco::GenParticle>& nParticle = searchForGenParticle( &nTk );
            //if ( !pParticle.status() ) continue;
            //if ( !nParticle.status() ) continue;
            fillHisto( "parJPsiGen_PDGID_ptk", pParticle.second.pdgId() );
            fillHisto( "parJPsiGen_PDGID_ntk", nParticle.second.pdgId() );
            fillHisto( "parJPsiGen_minDeltaR", pParticle.first );
            fillHisto( "parJPsiGen_minDeltaR", nParticle.first );
            fillHisto( "parJPsiGen_ptDiffBetweenPairs", pParticle.second.pt() - pTk.transverse() );
            fillHisto( "parJPsiGen_ptDiffBetweenPairs", nParticle.second.pt() - nTk.transverse() );
            fillHisto( "parJPsiGen_momentumDiffBetweenPairs", pParticle.second.p() - pTk.Momentum() );
            fillHisto( "parJPsiGen_momentumDiffBetweenPairs", nParticle.second.p() - nTk.Momentum() );
            if ( fabs( pParticle.second.pdgId() ) ==  13 )
            {
                fillHisto( "parJPsiGen_MuNugDeDx"  , nTk.Momentum(), cand.userFloat(  "MuNeg.dEdx.Harmonic") );
                fillHisto( "parJPsiGen_momentumDiffMuNeg", nParticle.second.p() - nTk.Momentum() );
            }
        }
    } catch (...) {}
}

void histMain_JPsiGenParticle::Clear()
{
}
