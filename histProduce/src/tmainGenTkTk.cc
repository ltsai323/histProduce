#include "histProduce/histProduce/interface/tmainGenTkTk.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "math.h"


treeMainGen_TkTk::treeMainGen_TkTk( TFileDirectory* d ) :
    treeMainGen( d, treeMain::Label("lbWriteSpecificDecay", "TkTkFitted", "bphAnalysis"), "TkTkGenInfo" ), formatTree_TkTk( totNumD, totNumI ),
    protonMass( 0.9382720813 ), pionMass( 0.13957061 )
{
    SetNewFormatTree(thisTree());
    RegTree();
}
void treeMainGen_TkTk::Process( fwlite::Event* ev )
{
    try
    {
        // preselect events {{{
        if ( !ev->isValid() ) return;
        GetByLabel( ev );

        if ( !beamSpotHandle.isValid() ) return;
        if ( !_genHandle.isValid() ) return;
        if ( _handle->size()  == 0 ) return;
        if ( _genHandle->size() == 0 ) return;
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
            if ( !cand.hasUserData("Kaon.fitMom") ) continue;
            if ( !cand.hasUserFloat("Proton.dEdx.Harmonic") ) continue;
            if ( !cand.hasUserFloat(  "Kaon.dEdx.Harmonic") ) continue;
            if ( !cand.hasUserFloat("Proton.dEdx.pixelHrm") ) continue;
            if ( !cand.hasUserFloat(  "Kaon.dEdx.pixelHrm") ) continue;
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

        int vtxSortingNumber = 0;
        std::vector< std::pair<double, const pat::CompositeCandidate*> >::const_iterator iter = candsSorted.begin();
        std::vector< std::pair<double, const pat::CompositeCandidate*> >::const_iterator iend = candsSorted.end  ();

        // sort the candidate with vtxprob, and choose first N candidates.
        //int recordCandInEachEvent = 10;

        while ( iter != iend )
        {
            Clear();
            dataD[tktkVtxprob] = iter->first;
            const pat::CompositeCandidate& cand = *(iter++->second);
            //if ( recordCandInEachEvent-- == 0 ) break;
            dataD[tktkMass] = cand.userFloat("fitMass");
            dataD[tktkPt] = cand.userData<GlobalVector>( "fitMomentum" ) -> transverse();
            dataD[tktkFlightDistance2d] = usefulFuncs::getFlightDistance ( cand, &bs );
            dataI[tktkSortingNumber] = vtxSortingNumber++;

            // first one is proton and second one is pion ( consider bigger momentum with heavier particle )
            const GlobalVector* dPTR[2] = {nullptr};
            dPTR[0] = cand.userData<GlobalVector>("Proton.fitMom");
            dPTR[1] = cand.userData<GlobalVector>("Kaon.fitMom");

            fourMom pTk ( dPTR[0]->x(), dPTR[0]->y(), dPTR[0]->z() );
            fourMom nTk ( dPTR[1]->x(), dPTR[1]->y(), dPTR[1]->z() );

            dataD[ptkMom] = pTk.Momentum();
            dataD[ntkMom] = nTk.Momentum();
            dataD[ptkDEDX_Harmonic] = cand.userFloat( "Proton.dEdx.Harmonic");
            dataD[ntkDEDX_Harmonic] = cand.userFloat(   "Kaon.dEdx.Harmonic");
            dataD[ptkIPt] = cand.userFloat( "Proton.IPt" );
            dataD[ntkIPt] = cand.userFloat(   "Kaon.IPt" );
            // find for PID.
            std::vector<fourMom> daug;
            daug.reserve( 2 );
            daug.emplace_back( pTk );
            daug.emplace_back( nTk );
            MatchRes mcRes = matchMC_CompositeCand( cand, daug, 0.3 );

            if ( !mcRes.isValid() ) continue;
            for ( unsigned int i=0; i< mcRes.getNDaughterRecorded(); ++i )
            {
                int mcDaugID = mcRes.getmcDaugID(i);
                const reco::GenParticle* matchCand = mcRes.getCand();
                const reco::Candidate* mcDaug = matchCand->daughter(mcDaugID);
                if ( mcRes.getDaugID(i) == 0 )
                    dataI[ptkPID] = mcDaug->pdgId();
                if ( mcRes.getDaugID(i) == 1 )
                    dataI[ntkPID] = mcDaug->pdgId();
            }
            thisTree()->Fill();
        }
    } catch (...) {}
}

void treeMainGen_TkTk::RegTree()
{
    RegFormatTree();

    thisTree()->Branch( "ptkPID", &dataI[ptkPID], "ptkPID/I" );
    thisTree()->Branch( "ntkPID", &dataI[ntkPID], "ntkPID/I" );
    thisTree()->Branch( "tktkSortingNumber", &dataI[tktkSortingNumber], "tktkSortingNumber/I" );
}
void treeMainGen_TkTk::GetByLabel( fwlite::Event* ev )
{
    getByLabel_Cand( ev );
    getByLabel_genParticle( ev );
    getByLabel_BS( ev );
}

inline void treeMainGen_TkTk::getByLabel_BS( fwlite::Event* ev )
{ beamSpotHandle.getByLabel( *ev,"offlineBeamSpot", "", "RECO"  ); return; }
