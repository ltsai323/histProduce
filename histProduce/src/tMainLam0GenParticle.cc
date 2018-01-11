#include "histProduce/histProduce/interface/tMainLam0GenParticle.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "math.h"


treeMain_Lam0GenParticle::treeMain_Lam0GenParticle( TFileDirectory* d ) :
    treeMain( d, treeMain::Label("lbWriteSpecificDecay", "Lam0Fitted", "bphAnalysis"), "Lam0GenInfo" ),
    protonMass( 0.9382720813 ), pionMass( 0.13957061 )
{
    regTree();
}
void treeMain_Lam0GenParticle::Process( fwlite::Event* ev )
{
    try
    {
        // preselect events {{{
        if ( !ev->isValid() ) return;
        _handle.getByLabel( *ev, _label.module.c_str(), _label.label.c_str(), _label.process.c_str() );
        beamSpotHandle.getByLabel( *ev,"offlineBeamSpot", "", "RECO"  );
        genHandle.getByLabel( *ev, "genParticles", "", "HLT" );


        if ( !beamSpotHandle.isValid() ) return;
        if ( !genHandle.isValid() ) return;
        if ( _handle->size()  == 0 ) return;
        if ( genHandle->size() == 0 ) return;
        const reco::Vertex bs( (*beamSpotHandle).position(), (*beamSpotHandle).covariance3D() );

        std::vector< std::pair< double, const pat::CompositeCandidate*> > candsSorted;
        candsSorted.reserve( _handle->size() );
        std::vector<pat::CompositeCandidate>::const_iterator handleIter = _handle->begin();
        std::vector<pat::CompositeCandidate>::const_iterator handleIend = _handle->end  ();
        while( handleIter != handleIend )
        {
            const pat::CompositeCandidate& cand = *handleIter++;
            if ( !cand.hasUserFloat("fitMass") ) continue;
            if ( !cand.hasUserData("Proton.fitMom") ) continue;
            if ( !cand.hasUserData("Pion.fitMom") ) continue;
            if ( !cand.hasUserFloat("Proton.dEdx.Harmonic") ) continue;
            if ( !cand.hasUserFloat(  "Pion.dEdx.Harmonic") ) continue;
            //if ( !cand.hasUserFloat("Proton.dEdx.pixelHrm") ) continue;
            //if ( !cand.hasUserFloat(  "Pion.dEdx.pixelHrm") ) continue;
            const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
            if ( _vtx == nullptr ) continue;
            double fd = usefulFuncs::getFlightDistance ( cand, &bs );
            if ( fd < 0.5 ) continue;
            double vtxprob = TMath::Prob( _vtx->chi2(), _vtx->ndof() );
            candsSorted.emplace_back( vtxprob, &cand );
        }
        usefulFuncs::sortingByFirstValue( candsSorted );
        if ( candsSorted.size() == 0 ) return;
        // preselect events end }}}

        double vtxSortingNumber = 0;
        std::vector< std::pair<double, const pat::CompositeCandidate*> >::const_iterator iter = candsSorted.begin();
        std::vector< std::pair<double, const pat::CompositeCandidate*> >::const_iterator iend = candsSorted.end  ();

        // sort the candidate with vtxprob, and choose first N candidates.
        //int recordCandInEachEvent = 10;

        while ( iter != iend )
        {
            Clear();
            lam0Vtxprob = iter->first;
            const pat::CompositeCandidate& cand = *(iter++->second);
            //if ( recordCandInEachEvent-- == 0 ) break;
            lam0Mass = cand.userFloat("fitMass");
            lam0Pt = cand.userData<GlobalVector>( "fitMomentum" ) -> transverse();
            lam0FlightDistance2d = usefulFuncs::getFlightDistance ( cand, &bs );
            lam0SortingNumber = vtxSortingNumber++;

            // first one is proton and second one is pion ( consider bigger momentum with heavier particle )
            const GlobalVector* dPTR[2] = {nullptr};
            dPTR[0] = cand.userData<GlobalVector>("Proton.fitMom");
            dPTR[1] = cand.userData<GlobalVector>("Pion.fitMom");

            fourMom pTk ( dPTR[0]->x(), dPTR[0]->y(), dPTR[0]->z() );
            fourMom nTk ( dPTR[1]->x(), dPTR[1]->y(), dPTR[1]->z() );

            ptkMom = pTk.Momentum();
            ntkMom = nTk.Momentum();
            ptkDEDX_Harmonic = cand.userFloat( "Proton.dEdx.Harmonic");
            ntkDEDX_Harmonic = cand.userFloat(   "Pion.dEdx.Harmonic");
            ptkIPt = cand.userFloat( "Proton.IPt" );
            ntkIPt = cand.userFloat(   "Pion.IPt" );
            // pair< deltaR, GenParticle >
            //const std::pair< double, reco::GenParticle>& pParticle = searchForGenParticle( &pTk );
            //const std::pair< double, reco::GenParticle>& nParticle = searchForGenParticle( &nTk );
            //if ( pParticle.first < 0.1 )
            //    if ( fabs( pParticle.second.p() - pTk.Momentum() ) < 0.1 )
            //{
            //    if ( fabs( pParticle.second.pdgId() ) == 2212 )
            //    {
            //        ptkMom = pTk.Momentum();
            //        ptkDEDX_Harmonic = cand.userFloat( "Proton.dEdx.Harmonic");
            //        ptkIPt = cand.userFloat( "Proton.IPt" );
            //    }
            //    if ( fabs( pParticle.second.pdgId() ) ==  211 )
            //    {
            //        ntkMom = pTk.Momentum();
            //        ntkDEDX_Harmonic = cand.userFloat( "Proton.dEdx.Harmonic");
            //        ntkIPt = cand.userFloat( "Proton.IPt" );
            //    }
            //}
//
//
            //if ( nParticle.first < 0.1 )
            //    if ( fabs( nParticle.second.p() - nTk.Momentum() ) < 0.1 )
            //{
            //    if ( fabs( nParticle.second.pdgId() ) == 2212 )
            //    {
            //        ptkMom = nTk.Momentum();
            //        ptkDEDX_Harmonic = cand.userFloat( "Pion.dEdx.Harmonic");
            //        ptkIPt = cand.userFloat( "Pion.IPt" );
            //    }
            //    if ( fabs( nParticle.second.pdgId() ) ==  211 )
            //    {
            //        ntkMom = nTk.Momentum();
            //        ntkDEDX_Harmonic = cand.userFloat(   "Pion.dEdx.Harmonic");
            //        ntkIPt = cand.userFloat(   "Pion.IPt" );
            //    }
            //}
            thisTree()->Fill();
        }
    } catch (...) {}
}

void treeMain_Lam0GenParticle::Clear()
{
    // total number of tree variables
    int NumOfVars = 14;
    memset( &lam0Mass, 0x00, NumOfVars*sizeof( double ) );
}
void treeMain_Lam0GenParticle::regTree()
{
    thisTree()->Branch( "lam0Mass", &lam0Mass, "lam0Mass/D" );
    thisTree()->Branch( "lam0FlightDistance2d", &lam0FlightDistance2d, "lam0FlightDistance2d/D" );
    thisTree()->Branch( "lam0Cosa2d", &lam0Cosa2d, "lam0Cosa2d/D" );
    thisTree()->Branch( "lam0Vtxprob", &lam0Vtxprob, "lam0Vtxprob/D" );
    thisTree()->Branch( "lam0Pt", &lam0Pt, "lam0Pt/D" );
    thisTree()->Branch( "lam0SortingNumber", &lam0SortingNumber, "lam0SortingNumber/D" );
    thisTree()->Branch( "ptkMom", &ptkMom, "ptkMom/D" );
    thisTree()->Branch( "ptkDEDX.Harmonic", &ptkDEDX_Harmonic, "ptkDEDX.Harmonic/D" );
    thisTree()->Branch( "ptkDEDX.pixelHrm", &ptkDEDX_pixelHrm, "ptkDEDX.pixelHrm/D" );
    thisTree()->Branch( "ptkIPt", &ptkIPt, "ptkIPt/D" );
    thisTree()->Branch( "ntkMom", &ntkMom, "ntkMom/D" );
    thisTree()->Branch( "ntkDEDX.Harmonic", &ntkDEDX_Harmonic, "ntkDEDX.Harmonic/D" );
    thisTree()->Branch( "ntkDEDX.pixelHrm", &ntkDEDX_pixelHrm, "ntkDEDX.pixelHrm/D" );
    thisTree()->Branch( "ntkIPt", &ntkIPt, "ntkIPt/D" );
}
