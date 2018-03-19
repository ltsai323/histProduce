#include "histProduce/histProduce/interface/tmainLbTk.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "math.h"
#include <stdio.h>


treeMain_LbTk::treeMain_LbTk( TFileDirectory* d ) :
    treeMain( d, treeMain::Label("lbWriteSpecificDecay", "LbToTkTkFitted", "bphAnalysis"), "LbTk" ),
    kaonMass ( 0.493667 ), protonMass ( 0.9382720813 ), pionMass ( 0.13957061 )
{
    RegTree();
}
void treeMain_LbTk::Process( fwlite::Event* ev )
{
    try
    {
        if ( !ev->isValid() ) return;
        GetByLabel( ev );

        // pv not recorded, use BS
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
            if ( !cand.hasUserData("TkTk/Proton.fitMom") ) continue;
            if ( !cand.hasUserData(  "TkTk/Kaon.fitMom") ) continue;
            if ( !cand.hasUserFloat("TkTk/Proton.IPt") ) continue;
            if ( !cand.hasUserFloat(  "TkTk/Kaon.IPt") ) continue;
            if ( !cand.hasUserData( "fitVertex" ) ) continue;
            if ( !cand.hasUserData( "refToJPsi" ) ) continue;

            const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
            if ( _vtx == nullptr ) continue;
            double fd = usefulFuncs::getFlightDistance ( cand, &bs );
            double cos2d = usefulFuncs::getCosa2d( cand, &bs );
            double vtxprob = TMath::Prob( _vtx->chi2(), _vtx->ndof() );
            if ( fd < 0.1 ) continue;
            //if ( fd > 5. ) continue;
            if ( cos2d < 0.99 ) continue;
            if ( vtxprob < 0.1 ) continue;

            candsSorted.emplace_back( vtxprob, &cand );
        }
        //if ( vtxprobChooser.size() == 0 ) return;
        if ( candsSorted.size() == 0 ) return;

        std::vector< std::pair<double, const pat::CompositeCandidate*> >::const_iterator iter = candsSorted.begin();
        std::vector< std::pair<double, const pat::CompositeCandidate*> >::const_iterator iend = candsSorted.end  ();

        while ( iter != iend )
        {
            Clear();
            const pat::CompositeCandidate& cand = *(iter++->second);

            // first one is proton and second one is kaon ( consider bigger momentum with heavier particle )
            const GlobalVector* dPTR[2] = {nullptr};
            dPTR[0] = cand.userData<GlobalVector>("TkTk/Proton.fitMom");
            dPTR[1] = cand.userData<GlobalVector>("TkTk/Kaon.fitMom");

            const GlobalVector* muPTR[2] = {nullptr};
            muPTR[0] = cand.userData<GlobalVector>("JPsi/MuPos.fitMom");
            muPTR[1] = cand.userData<GlobalVector>("JPsi/MuNeg.fitMom");

            fourMom pMu ( muPTR[0]->x(), muPTR[0]->y(), muPTR[0]->z() );
            fourMom nMu ( muPTR[1]->x(), muPTR[1]->y(), muPTR[1]->z() );
            pMu.setMass( 0.1056583745 );
            nMu.setMass( 0.1056583745 );
            fourMom pTk ( dPTR[0]->x(), dPTR[0]->y(), dPTR[0]->z() );
            fourMom nTk ( dPTR[1]->x(), dPTR[1]->y(), dPTR[1]->z() );
            fourMom tktk;
            fourMom fourTk;

            // search for lam0
            pTk.setMass( protonMass );
            nTk.setMass(   pionMass );
            tktk = pTk+nTk;
            fourTk = pMu + nMu + tktk;

            dataD[fake_Lam0Mass] = tktk.Mag();
            dataD[fake_LbL0Mass] = fourTk.Mag();

            // search for k short
            pTk.setMass( pionMass );
            nTk.setMass( pionMass );
            tktk = pTk+nTk;
            fourTk = pMu + nMu + tktk;

            dataD[fake_KshortMass] = tktk.Mag();
            dataD[fake_mumupipiMass] = fourTk.Mag();

            // search for k(892)
            pTk.setMass( kaonMass );
            nTk.setMass( pionMass );
            tktk = pTk+nTk;
            fourTk = pMu + nMu + tktk;

            dataD[fake_KstarMass] = tktk.Mag();
            dataD[fake_BdMass] = fourTk.Mag();

            // search for phi(1020)
            pTk.setMass( kaonMass );
            nTk.setMass( kaonMass );
            tktk = pTk+nTk;
            fourTk = pMu + nMu + tktk;

            dataD[fake_PhiMass] = tktk.Mag();
            dataD[fake_BsMass] = fourTk.Mag();


            const pat::CompositeCandidate* jpsi = usefulFuncs::getByRef<pat::CompositeCandidate>( cand, "refToJPsi" );
            const reco::Vertex* pv = usefulFuncs::get<reco::Vertex>( *jpsi, "fitVertex" );

            dataD[lbtkMass] = cand.userFloat( "fitMass" );
            dataD[lbtkFlightDistance2d] = usefulFuncs::getFlightDistance ( cand, pv );

            dataD[ptkMom] = pTk.Momentum();
            dataD[ntkMom] = nTk.Momentum();
            dataD[ptkIPt] = cand.userFloat("TkTk/Proton.IPt");
            dataD[ntkIPt] = cand.userFloat("TkTk/Kaon.IPt");
            dataD[ptkIPtErr] = cand.userFloat("TkTk/Proton.IPt.Error");
            dataD[ntkIPtErr] = cand.userFloat("TkTk/Kaon.IPt.Error");
            if ( cand.hasUserFloat("TkTk/Proton.dEdx.pixelHrm") )
                dataD[ptkDEDX_pixelHrm] = cand.userFloat( "TkTk/Proton.dEdx.pixelHrm" );
            if ( cand.hasUserFloat(  "TkTk/Kaon.dEdx.pixelHrm") )
                dataD[ntkDEDX_pixelHrm] = cand.userFloat( "TkTk/Kaon.dEdx.pixelHrm" );
            if ( cand.hasUserFloat("TkTk/Proton.dEdx.Harmonic") )
                dataD[ptkDEDX_Harmonic] = cand.userFloat( "TkTk/Proton.dEdx.Harmonic" );
            if ( cand.hasUserFloat(  "TkTk/Kaon.dEdx.Harmonic") )
                dataD[ntkDEDX_Harmonic] = cand.userFloat(   "TkTk/Kaon.dEdx.Harmonic" );
            thisTree()->Fill();
        }
    } catch (...) {}
}

void treeMain_LbTk::Clear()
{
    memset( dataD, 0x00, totNumD*sizeof( double ) );
    memset( dataI, 0x00, totNumI*sizeof( int ) );
}
void treeMain_LbTk::RegTree()
{
    thisTree()->Branch( "lbtkMass", &dataD[lbtkMass], "lbtkMass/D" );
    thisTree()->Branch( "lbtkFD2d", &dataD[lbtkFlightDistance2d], "lbtkFD2d/D" );

    thisTree()->Branch( "fake_Lam0Mass", &dataD[fake_Lam0Mass], "fake_Lam0Mass/D" );
    thisTree()->Branch( "fake_LbL0Mass", &dataD[fake_LbL0Mass], "fake_LbL0Mass/D" );
    thisTree()->Branch( "fake_KstarMass", &dataD[fake_KstarMass], "fake_KstarMass/D" );
    thisTree()->Branch( "fake_BdMass", &dataD[fake_BdMass], "fake_BdMass/D" );
    thisTree()->Branch( "fake_PhiMass", &dataD[fake_PhiMass], "fake_PhiMass/D" );
    thisTree()->Branch( "fake_BsMass", &dataD[fake_BsMass], "fake_BsMass/D" );
    thisTree()->Branch( "fake_KshortMass", &dataD[fake_KshortMass], "fake_KshortMass/D" );
    thisTree()->Branch( "fake_mumupipiMass", &dataD[fake_mumupipiMass], "fake_mumupipiMass/D" );

    thisTree()->Branch( "ptkMom", &dataD[ptkMom], "ptkMom/D" );
    thisTree()->Branch( "ptkDEDX.Harmonic", &dataD[ptkDEDX_Harmonic], "ptkDEDX.Harmonic/D" );
    thisTree()->Branch( "ptkDEDX.pixelHrm", &dataD[ptkDEDX_pixelHrm], "ptkDEDX.pixelHrm/D" );
    thisTree()->Branch( "ptkIPt", &dataD[ptkIPt], "ptkIPt/D" );
    thisTree()->Branch( "ptkIPtErr", &dataD[ptkIPtErr], "ptkIPtErr/D" );

    thisTree()->Branch( "ntkMom", &dataD[ntkMom], "ntkMom/D" );
    thisTree()->Branch( "ntkDEDX.Harmonic", &dataD[ntkDEDX_Harmonic], "ntkDEDX.Harmonic/D" );
    thisTree()->Branch( "ntkDEDX.pixelHrm", &dataD[ntkDEDX_pixelHrm], "ntkDEDX.pixelHrm/D" );
    thisTree()->Branch( "ntkIPt", &dataD[ntkIPt], "ntkIPt/D" );
    thisTree()->Branch( "ntkIPtErr", &dataD[ntkIPtErr], "ntkIPtErr/D" );
}
void treeMain_LbTk::GetByLabel( fwlite::Event* ev )
{
    getByLabel_Cand( ev );
    getByLabel_BS( ev );
}

inline void treeMain_LbTk::getByLabel_BS( fwlite::Event* ev )
{ beamSpotHandle.getByLabel( *ev,"offlineBeamSpot", "", "RECO"  ); return; }
