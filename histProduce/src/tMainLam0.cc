#include "histProduce/histProduce/interface/tMainLam0.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "math.h"
#include <stdio.h>


treeMain_Lam0::treeMain_Lam0( TFileDirectory* d ) :
    treeMain( d, treeMain::Label("lbWriteSpecificDecay", "Lam0Fitted", "bphAnalysis"), "Lam0" )
{
    regTree();
}
void treeMain_Lam0::Process( fwlite::Event* ev )
{
    try
    {
        // preselect event {{{
        if ( !ev->isValid() ) return;
        _handle.getByLabel( *ev, _label.module.c_str(), _label.label.c_str(), _label.process.c_str() );

        // pv not recorded, use BS
        fwlite::Handle< reco::BeamSpot > beamSpotHandle;
        beamSpotHandle.getByLabel( *ev,"offlineBeamSpot", "", "RECO"  );
        if ( !beamSpotHandle.isValid() ) return;
        if ( _handle->size()  == 0 ) return;
        const reco::Vertex bs( (*beamSpotHandle).position(), (*beamSpotHandle).covariance3D() );

        std::vector< std::pair< double, const pat::CompositeCandidate*> > candsSorted;
        candsSorted.clear();
        candsSorted.reserve( _handle->size() );
        std::vector<pat::CompositeCandidate>::const_iterator handleIter = _handle->begin();
        std::vector<pat::CompositeCandidate>::const_iterator handleIend = _handle->end  ();
        while( handleIter != handleIend )
        {
            const pat::CompositeCandidate& cand = *handleIter++;
            if ( !cand.hasUserFloat("fitMass") ) continue;
            if ( !cand.hasUserData("Proton.fitMom") ) continue;
            if ( !cand.hasUserData("Pion.fitMom") ) continue;
            if ( !cand.hasUserFloat("Proton.IPt") ) continue;
            if ( !cand.hasUserFloat(  "Pion.IPt") ) continue;
            const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
            if ( _vtx == nullptr ) continue;
            double fd = usefulFuncs::getFlightDistance ( cand, &bs );
            double cos2d = usefulFuncs::getCosa2d( cand, &bs );
            double vtxprob = TMath::Prob( _vtx->chi2(), _vtx->ndof() );
            if ( fd < 1. ) continue;
            //if ( fd > 5. ) continue;
            if ( cos2d < 0.99 ) continue;
            if ( vtxprob < 0.2 ) continue;
            candsSorted.emplace_back( vtxprob, &cand );
        }
        if ( candsSorted.size() == 0 ) return;
        // preselect events end }}}

        std::vector< std::pair<double, const pat::CompositeCandidate*> >::const_iterator iter = candsSorted.begin();
        std::vector< std::pair<double, const pat::CompositeCandidate*> >::const_iterator iend = candsSorted.end  ();

        //double kaonMass ( 0.493667 );
        double protonMass ( 0.9382720813 );
        double pionMass ( 0.13957061 );

        // sort the candidate with vtxprob, and choose first N candidates.
        int recordCandInEachEvent = 10;

        while ( iter != iend )
        {
            Clear();
            const pat::CompositeCandidate& cand = *(iter++->second);
            if ( recordCandInEachEvent-- == 0 ) break;

            // first one is proton and second one is kaon ( consider bigger momentum with heavier particle )
            const GlobalVector* dPTR[2] = {nullptr};
            dPTR[0] = cand.userData<GlobalVector>("Proton.fitMom");
            dPTR[1] = cand.userData<GlobalVector>("Pion.fitMom");

            fourMom pTk ( dPTR[0]->x(), dPTR[0]->y(), dPTR[0]->z() );
            fourMom nTk ( dPTR[1]->x(), dPTR[1]->y(), dPTR[1]->z() );
            // reconstruct lambda0
            pTk.setMass( protonMass );
            nTk.setMass(   pionMass );

            //if ( lam0Mass < 1.120 && lam0Mass > 1.110 )
            //{
                if ( cand.hasUserFloat("Proton.dEdx.pixelHrm") )
                {
                    ptkMom = pTk.Momentum();
                    ptkDEDX_pixelHrm = cand.userFloat( "Proton.dEdx.pixelHrm" );
                }
                if ( cand.hasUserFloat(  "Pion.dEdx.pixelHrm") )
                {
                    ntkMom = nTk.Momentum();
                    ntkDEDX_pixelHrm = cand.userFloat( "Pion.dEdx.pixelHrm" );
                }
                if ( cand.hasUserFloat("Proton.dEdx.Harmonic") )
                {
                    ptkMom = pTk.Momentum();
                    ptkDEDX_Harmonic = cand.userFloat( "Proton.dEdx.Harmonic" );
                }
                if ( cand.hasUserFloat(  "Pion.dEdx.Harmonic") )
                {
                    ntkMom = nTk.Momentum();
                    ntkDEDX_Harmonic = cand.userFloat(   "Pion.dEdx.Harmonic" );
                }
            //}

            lam0Mass = cand.userFloat( "fitMass" );
            lam0FlightDistance2d = usefulFuncs::getFlightDistance ( cand, &bs );
            lam0Cosa2d = usefulFuncs::getCosa2d( cand, &bs );
            ptkIPt = cand.userFloat("Proton.IPt");
            ntkIPt = cand.userFloat("Pion.IPt");
            if ( ptkMom && ptkDEDX_Harmonic && ntkMom && ntkDEDX_Harmonic )
                thisTree()->Fill();
        }
    } catch (...) {}
}

void treeMain_Lam0::Clear()
{
    //lam0Mass= lam0FlightDistance2d= lam0Cosa2d= 0.;
    //ptkMom= ptkDEDX_Harmonic= ptkDEDX_pixelHrm= ptkIPt= 0.;
    //ntkMom= ntkDEDX_Harmonic= ntkDEDX_pixelHrm= ntkIPt= 0.;
    memset( &lam0Mass, 0x00, 11*sizeof( double ) );
    if ( lam0Mass != 0 ) std::cout << "not cleared01\n";
    if ( lam0FlightDistance2d != 0 ) std::cout << "not cleared02\n";
    if ( lam0Cosa2d != 0 ) std::cout << "not cleared03\n";
    if ( ptkMom != 0 ) std::cout << "not cleared04\n";
    if ( ptkDEDX_Harmonic != 0 ) std::cout << "not cleared05\n";
    if ( ptkDEDX_pixelHrm != 0 ) std::cout << "not cleared06\n";
    if ( ptkIPt != 0 ) std::cout << "not cleared07\n";
    if ( ntkMom != 0 ) std::cout << "not cleared08\n";
    if ( ntkDEDX_Harmonic != 0 ) std::cout << "not cleared09\n";
    if ( ntkDEDX_pixelHrm != 0 ) std::cout << "not cleared10\n";
    if ( ntkIPt != 0 ) std::cout << "not cleared11\n";
}
void treeMain_Lam0::regTree()
{
    thisTree()->Branch( "lam0Mass", &lam0Mass, "lam0Mass/D" );
    thisTree()->Branch( "lam0FlightDistance2d", &lam0FlightDistance2d, "lam0FlightDistance2d/D" );
    thisTree()->Branch( "lam0Cosa2d", &lam0Cosa2d, "lam0Cosa2d/D" );
    thisTree()->Branch( "ptkMom", &ptkMom, "ptkMom/D" );
    thisTree()->Branch( "ptkDEDX.Harmonic", &ptkDEDX_Harmonic, "ptkDEDX.Harmonic/D" );
    thisTree()->Branch( "ptkDEDX.pixelHrm", &ptkDEDX_pixelHrm, "ptkDEDX.pixelHrm/D" );
    thisTree()->Branch( "ptkIPt", &ptkIPt, "ptkIPt/D" );
    thisTree()->Branch( "ntkMom", &ntkMom, "ntkMom/D" );
    thisTree()->Branch( "ntkDEDX.Harmonic", &ntkDEDX_Harmonic, "ntkDEDX.Harmonic/D" );
    thisTree()->Branch( "ntkDEDX.pixelHrm", &ntkDEDX_pixelHrm, "ntkDEDX.pixelHrm/D" );
    thisTree()->Branch( "ntkIPt", &ntkIPt, "ntkIPt/D" );
}
