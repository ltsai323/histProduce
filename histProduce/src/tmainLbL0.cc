#include "histProduce/histProduce/interface/tmainLbL0.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "math.h"
#include <stdio.h>


treeMain_LbL0::treeMain_LbL0( TFileDirectory* d ) :
    treeMain( d, treeMain::Label("lbWriteSpecificDecay", "LbToLam0Fitted", "bphAnalysis"), "LbL0" )
{
    RegTree();
}
void treeMain_LbL0::Process( fwlite::Event* ev )
{
    try
    {
        if ( !ev->isValid() ) return;
        GetByLabel( ev );

        // pv not recorded, use BS
        if ( !beamSpotHandle.isValid() ) return;
        if ( _handle->size()  == 0 ) return;
        const reco::Vertex bs( (*beamSpotHandle).position(), (*beamSpotHandle).covariance3D() );

        //std::map< double, const pat::CompositeCandidate*> vtxprobChooser;
        //vtxprobChooser.clear();
        std::vector< std::pair< double, const pat::CompositeCandidate*> > candsSorted;
        candsSorted.clear();
        candsSorted.reserve( _handle->size() );
        std::vector<pat::CompositeCandidate>::const_iterator handleIter = _handle->begin();
        std::vector<pat::CompositeCandidate>::const_iterator handleIend = _handle->end  ();
        while( handleIter != handleIend )
        {
            const pat::CompositeCandidate& cand = *handleIter++;
            if ( !cand.hasUserFloat("fitMass") ) continue;
            if ( !cand.hasUserData("Lam0/Proton.fitMom") ) continue;
            if ( !cand.hasUserData("Lam0/Pion.fitMom") ) continue;
            if ( !cand.hasUserFloat("Lam0/Proton.IPt") ) continue;
            if ( !cand.hasUserFloat(  "Lam0/Pion.IPt") ) continue;
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
        //if ( vtxprobChooser.size() == 0 ) return;
        if ( candsSorted.size() == 0 ) return;

        std::vector< std::pair<double, const pat::CompositeCandidate*> >::const_iterator iter = candsSorted.begin();
        std::vector< std::pair<double, const pat::CompositeCandidate*> >::const_iterator iend = candsSorted.end  ();

        //double kaonMass ( 0.493667 );
        double protonMass ( 0.9382720813 );
        double pionMass ( 0.13957061 );
        while ( iter != iend )
        {
            Clear();
            const pat::CompositeCandidate& cand = *(iter++->second);

            // first one is proton and second one is kaon ( consider bigger momentum with heavier particle )
            const GlobalVector* dPTR[2] = {nullptr};
            dPTR[0] = cand.userData<GlobalVector>("Proton.fitMom");
            dPTR[1] = cand.userData<GlobalVector>("Pion.fitMom");

            fourMom pTk ( dPTR[0]->x(), dPTR[0]->y(), dPTR[0]->z() );
            fourMom nTk ( dPTR[1]->x(), dPTR[1]->y(), dPTR[1]->z() );
            // reconstruct lambda0
            pTk.setMass( protonMass );
            nTk.setMass(   pionMass );

            //if ( lbl0Mass < 1.120 && lbl0Mass > 1.110 )
            //{
                if ( cand.hasUserFloat("Proton.dEdx.pixelHrm") )
                {
                    dataD[ptkMom] = pTk.Momentum();
                    dataD[ptkDEDX_pixelHrm] = cand.userFloat( "Proton.dEdx.pixelHrm" );
                }
                if ( cand.hasUserFloat(  "Pion.dEdx.pixelHrm") )
                {
                    dataD[ntkMom] = nTk.Momentum();
                    dataD[ntkDEDX_pixelHrm] = cand.userFloat( "Pion.dEdx.pixelHrm" );
                }
                if ( cand.hasUserFloat("Proton.dEdx.Harmonic") )
                {
                    dataD[ptkMom] = pTk.Momentum();
                    dataD[ptkDEDX_Harmonic] = cand.userFloat( "Proton.dEdx.Harmonic" );
                }
                if ( cand.hasUserFloat(  "Pion.dEdx.Harmonic") )
                {
                    dataD[ntkMom] = nTk.Momentum();
                    dataD[ntkDEDX_Harmonic] = cand.userFloat(   "Pion.dEdx.Harmonic" );
                }
            //}

            dataD[lbl0Mass] = cand.userFloat( "fitMass" );
            dataD[lbl0FlightDistance2d] = usefulFuncs::getFlightDistance ( cand, &bs );
            dataD[lbl0Cosa2d] = usefulFuncs::getCosa2d( cand, &bs );
            dataD[ptkIPt] = cand.userFloat("Proton.IPt");
            dataD[ntkIPt] = cand.userFloat("Pion.IPt");
            thisTree()->Fill();
        }
    } catch (...) {}
}

void treeMain_LbL0::Clear()
{
    //lbl0Mass= lbl0FlightDistance2d= lbl0Cosa2d= 0.;
    //ptkMom= ptkDEDX_Harmonic= ptkDEDX_pixelHrm= ptkIPt= 0.;
    //ntkMom= ntkDEDX_Harmonic= ntkDEDX_pixelHrm= ntkIPt= 0.;
    //memset( &lbl0Mass, 0x00, 11*sizeof( double ) );
    memset( dataD, 0x00, totNumD*sizeof( double ) );
    memset( dataI, 0x00, totNumI*sizeof( int ) );
}
void treeMain_LbL0::RegTree()
{
    thisTree()->Branch( "lbl0Mass", &dataD[lbl0Mass], "lbl0Mass/D" );
    thisTree()->Branch( "lbl0FlightDistance2d", &dataD[lbl0FlightDistance2d], "lbl0FlightDistance2d/D" );
    thisTree()->Branch( "lbl0Cosa2d", &dataD[lbl0Cosa2d], "lbl0Cosa2d/D" );
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
void treeMain_LbL0::GetByLabel( fwlite::Event* ev )
{
    getByLabel_Cand( ev );
    getByLabel_BS( ev );
}

inline void treeMain_LbL0::getByLabel_BS( fwlite::Event* ev )
{ beamSpotHandle.getByLabel( *ev,"offlineBeamSpot", "", "RECO"  ); return; }
