#include "histProduce/histProduce/interface/tmainGenLam0.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "math.h"


treeMainGen_Lam0::treeMainGen_Lam0( TFileDirectory* d ) :
    treeMainGen( d, treeMain::Label("lbWriteSpecificDecay", "Lam0Fitted", "bphAnalysis"), "Lam0GenInfo" ),
    protonMass( 0.9382720813 ), pionMass( 0.13957061 )
{
    RegTree();
}
void treeMainGen_Lam0::Process( fwlite::Event* ev )
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
            dataD[lam0Vtxprob] = iter->first;
            const pat::CompositeCandidate& cand = *(iter++->second);
            //if ( recordCandInEachEvent-- == 0 ) break;
            dataD[lam0Mass] = cand.userFloat("fitMass");
            dataD[lam0Pt] = cand.userData<GlobalVector>( "fitMomentum" ) -> transverse();
            dataD[lam0FlightDistance2d] = usefulFuncs::getFlightDistance ( cand, &bs );
            dataD[lam0SortingNumber] = vtxSortingNumber++;

            // first one is proton and second one is pion ( consider bigger momentum with heavier particle )
            const GlobalVector* dPTR[2] = {nullptr};
            dPTR[0] = cand.userData<GlobalVector>("Proton.fitMom");
            dPTR[1] = cand.userData<GlobalVector>("Pion.fitMom");

            fourMom pTk ( dPTR[0]->x(), dPTR[0]->y(), dPTR[0]->z() );
            fourMom nTk ( dPTR[1]->x(), dPTR[1]->y(), dPTR[1]->z() );

            dataD[ptkMom] = pTk.Momentum();
            dataD[ntkMom] = nTk.Momentum();
            dataD[ptkDEDX_Harmonic] = cand.userFloat( "Proton.dEdx.Harmonic");
            dataD[ntkDEDX_Harmonic] = cand.userFloat(   "Pion.dEdx.Harmonic");
            dataD[ptkIPt] = cand.userFloat( "Proton.IPt" );
            dataD[ntkIPt] = cand.userFloat(   "Pion.IPt" );
            // find for PID.
            std::vector<fourMom> daug;
            daug.reserve( 2 );
            daug.emplace_back( pTk );
            daug.emplace_back( nTk );
            MatchRes mcRes = matchMC_CompositeCand( cand, daug, 0.05 );

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

void treeMainGen_Lam0::Clear()
{
    memset( dataD, 0x00, totNumD*sizeof( double ) );
    memset( dataI, 0x00, totNumI*sizeof( int ) );
}
void treeMainGen_Lam0::RegTree()
{
    //thisTree()->Branch( "lam0Mass", &lam0Mass, "lam0Mass/D" );
    //thisTree()->Branch( "lam0FlightDistance2d", &lam0FlightDistance2d, "lam0FlightDistance2d/D" );
    //thisTree()->Branch( "lam0Cosa2d", &lam0Cosa2d, "lam0Cosa2d/D" );
    //thisTree()->Branch( "lam0Vtxprob", &lam0Vtxprob, "lam0Vtxprob/D" );
    //thisTree()->Branch( "lam0Pt", &lam0Pt, "lam0Pt/D" );
    //thisTree()->Branch( "lam0SortingNumber", &lam0SortingNumber, "lam0SortingNumber/D" );
    //thisTree()->Branch( "ptkMom", &ptkMom, "ptkMom/D" );
    //thisTree()->Branch( "ptkDEDX.Harmonic", &ptkDEDX_Harmonic, "ptkDEDX.Harmonic/D" );
    //thisTree()->Branch( "ptkDEDX.pixelHrm", &ptkDEDX_pixelHrm, "ptkDEDX.pixelHrm/D" );
    //thisTree()->Branch( "ptkIPt", &ptkIPt, "ptkIPt/D" );
    //thisTree()->Branch( "ntkMom", &ntkMom, "ntkMom/D" );
    //thisTree()->Branch( "ntkDEDX.Harmonic", &ntkDEDX_Harmonic, "ntkDEDX.Harmonic/D" );
    //thisTree()->Branch( "ntkDEDX.pixelHrm", &ntkDEDX_pixelHrm, "ntkDEDX.pixelHrm/D" );
    //thisTree()->Branch( "ntkIPt", &ntkIPt, "ntkIPt/D" );
//
    //thisTree()->Branch( "ptkPID", &ptkPID, "ptkPID/I" );
    //thisTree()->Branch( "ntkPID", &ntkPID, "ntkPID/I" );

    thisTree()->Branch( "lam0Mass", &dataD[lam0Mass], "lam0Mass/D" );
    thisTree()->Branch( "lam0FlightDistance2d", &dataD[lam0FlightDistance2d], "lam0FlightDistance2d/D" );
    thisTree()->Branch( "lam0Cosa2d", &dataD[lam0Cosa2d], "lam0Cosa2d/D" );
    thisTree()->Branch( "lam0Pt", &dataD[lam0Pt], "lam0Pt/D" );
    thisTree()->Branch( "lam0SortingNumber", &dataD[lam0SortingNumber], "lam0SortingNumber/D" );
    thisTree()->Branch( "ptkMom", &dataD[ptkMom], "ptkMom/D" );
    thisTree()->Branch( "ptkDEDX.Harmonic", &dataD[ptkDEDX_Harmonic], "ptkDEDX.Harmonic/D" );
    thisTree()->Branch( "ptkDEDX.pixelHrm", &dataD[ptkDEDX_pixelHrm], "ptkDEDX.pixelHrm/D" );
    thisTree()->Branch( "ptkIPt", &dataD[ptkIPt], "ptkIPt/D" );
    thisTree()->Branch( "ntkMom", &dataD[ntkMom], "ntkMom/D" );
    thisTree()->Branch( "ntkDEDX.Harmonic", &dataD[ntkDEDX_Harmonic], "ntkDEDX.Harmonic/D" );
    thisTree()->Branch( "ntkDEDX.pixelHrm", &dataD[ntkDEDX_pixelHrm], "ntkDEDX.pixelHrm/D" );
    thisTree()->Branch( "ntkIPt", &dataD[ntkIPt], "ntkIPt/D" );

    thisTree()->Branch( "ptkPID", &dataI[ptkPID], "ptkPID/I" );
    thisTree()->Branch( "ntkPID", &dataI[ntkPID], "ntkPID/I" );
}
void treeMainGen_Lam0::GetByLabel( fwlite::Event* ev )
{
    getByLabel_Cand( ev );
    getByLabel_genParticle( ev );
    getByLabel_BS( ev );
}

inline void treeMainGen_Lam0::getByLabel_BS( fwlite::Event* ev )
{ beamSpotHandle.getByLabel( *ev,"offlineBeamSpot", "", "RECO"  ); return; }
