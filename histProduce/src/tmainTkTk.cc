#include "histProduce/histProduce/interface/tmainTkTk.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "math.h"


treeMain_TkTk::treeMain_TkTk( TFileDirectory* d ) :
    treeMain( d, treeMain::Label("lbWriteSpecificDecay", "TkTkFitted", "bphAnalysis"), "TkTk" ),
    kaonMass ( 0.493667 ), protonMass ( 0.9382720813 ), pionMass ( 0.13957061 )
{
    RegTree();
}
void treeMain_TkTk::Process( fwlite::Event* ev )
{
    try
    {
        // preselect events {{{
        if ( !ev->isValid() ) return;
        GetByLabel( ev );


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
            if ( !cand.hasUserData(  "Kaon.fitMom") ) continue;
            if ( !cand.hasUserFloat("Proton.dEdx.Harmonic") ) continue;
            if ( !cand.hasUserFloat(  "Kaon.dEdx.Harmonic") ) continue;
            if ( !cand.hasUserFloat("Proton.dEdx.pixelHrm") ) continue;
            if ( !cand.hasUserFloat(  "Kaon.dEdx.pixelHrm") ) continue;
            if ( cand.userFloat("Proton.IPt") < 0.01230) continue;
            if ( cand.userFloat(  "Kaon.IPt") >-0.03968) continue;

            const GlobalVector* dPTR[2] = {nullptr};
            dPTR[0] = cand.userData<GlobalVector>("Proton.fitMom");
            dPTR[1] = cand.userData<GlobalVector>("Kaon.fitMom");
            fourMom pTk ( dPTR[0]->x(), dPTR[0]->y(), dPTR[0]->z() );
            fourMom nTk ( dPTR[1]->x(), dPTR[1]->y(), dPTR[1]->z() );
            fourMom tktk = pTk+nTk;
            if ( tktk.transverse() < 3. ) continue;
            if ( tktk.Momentum()   < 3. ) continue;


            const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
            if ( _vtx == nullptr ) continue;
            double fd = usefulFuncs::getFlightDistance ( cand, &bs );
            if ( fd < 1.0 ) continue;
            double cos2d = usefulFuncs::getCosa2d( cand, &bs );
            double vtxprob = TMath::Prob( _vtx->chi2(), _vtx->ndof() );
            if ( cos2d < 0.99 ) continue;
            if ( vtxprob < 0.1 ) continue;


            candsSorted.emplace_back( vtxprob, &cand );
        }
        // sorting for vtxprob value. if it is used to be a cut.
        //usefulFuncs::sortingByFirstValue( candsSorted );
        if ( candsSorted.size() == 0 ) return;
        // preselect events end }}}

        //double vtxSortingNumber = 0;
        std::vector< std::pair<double, const pat::CompositeCandidate*> >::const_iterator iter = candsSorted.begin();
        std::vector< std::pair<double, const pat::CompositeCandidate*> >::const_iterator iend = candsSorted.end  ();

        // sort the candidate with vtxprob, and choose first N candidates.
        //int recordCandInEachEvent = 10;

        while ( iter != iend )
        {
            Clear();
            dataD[tktkVtxprob] = iter->first;
            const pat::CompositeCandidate& cand = *(iter++->second);
            const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
            //if ( recordCandInEachEvent-- == 0 ) break;
            dataD[tktkMass] = cand.userFloat("fitMass");
            dataD[tktkPt] = cand.userData<GlobalVector>( "fitMomentum" ) -> transverse();
            dataD[tktkFlightDistance2d] = usefulFuncs::getFlightDistance ( cand, &bs );
            dataD[tktkVtxprob] = TMath::Prob( _vtx->chi2(), _vtx->ndof() );

            // first one is proton and second one is pion ( consider bigger momentum with heavier particle )
            const GlobalVector* dPTR[2] = {nullptr};
            dPTR[0] = cand.userData<GlobalVector>("Proton.fitMom");
            dPTR[1] = cand.userData<GlobalVector>("Kaon.fitMom");

            fourMom pTk ( dPTR[0]->x(), dPTR[0]->y(), dPTR[0]->z() );
            fourMom nTk ( dPTR[1]->x(), dPTR[1]->y(), dPTR[1]->z() );
            fourMom tktk = pTk+nTk;

            // search for lam0
            pTk.setMass( protonMass );
            nTk.setMass(   pionMass );
            tktk = pTk+nTk;
            dataD[fake_Lam0Mass] = tktk.Mag();
            if ( tktk.Mag() > 1.15 ) continue;
            if ( tktk.Mag() < 1.05 ) continue;

            // search for k short
            pTk.setMass( pionMass );
            nTk.setMass( pionMass );
            tktk = pTk+nTk;
            dataD[fake_KshortMass] = tktk.Mag();

            // search for k(892)
            pTk.setMass( kaonMass );
            nTk.setMass( pionMass );
            tktk = pTk+nTk;
            dataD[fake_KstarMass] = tktk.Mag();

            // search for phi(1020)
            pTk.setMass( kaonMass );
            nTk.setMass( kaonMass );
            tktk = pTk+nTk;
            dataD[fake_PhiMass] = tktk.Mag();


            dataD[ptkMom] = pTk.Momentum();
            dataD[ntkMom] = nTk.Momentum();
            dataD[ptkDEDX_Harmonic] = cand.userFloat( "Proton.dEdx.Harmonic");
            dataD[ntkDEDX_Harmonic] = cand.userFloat(   "Kaon.dEdx.Harmonic");
            dataD[ptkDEDX_pixelHrm] = cand.userFloat( "Proton.dEdx.pixelHrm");
            dataD[ntkDEDX_pixelHrm] = cand.userFloat(   "Kaon.dEdx.pixelHrm");
            dataD[ptkIPt] = cand.userFloat( "Proton.IPt" );
            dataD[ntkIPt] = cand.userFloat(   "Kaon.IPt" );
            dataD[ptkIPtErr] = cand.userFloat( "Proton.IPt.Error" );
            dataD[ntkIPtErr] = cand.userFloat(   "Kaon.IPt.Error" );
            thisTree()->Fill();
        }
    } catch (...) {}
}

void treeMain_TkTk::Clear()
{
    memset( dataD, 0x00, totNumD*sizeof( double ) );
    memset( dataI, 0x00, totNumI*sizeof( int ) );
}
void treeMain_TkTk::RegTree()
{
    thisTree()->Branch( "tktkMass", &dataD[tktkMass], "tktkMass/D" );
    thisTree()->Branch( "tktkFD2d", &dataD[tktkFlightDistance2d], "tktkFD2d/D" );
    thisTree()->Branch( "tktkVtxprob", &dataD[tktkVtxprob], "tktkVtxprob/D" );
    thisTree()->Branch( "tktkPt", &dataD[tktkPt], "tktkPt/D" );
    thisTree()->Branch( "fake_Lam0Mass", &dataD[fake_Lam0Mass], "fake_Lam0Mass/D" );
    thisTree()->Branch( "fake_KstarMass", &dataD[fake_KstarMass], "fake_KstarMass/D" );
    thisTree()->Branch( "fake_KshortMass", &dataD[fake_KshortMass], "fake_KshortMass/D" );
    thisTree()->Branch( "fake_PhiMass", &dataD[fake_PhiMass], "fake_PhiMass/D" );
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

void treeMain_TkTk::GetByLabel( fwlite::Event* ev )
{
    getByLabel_Cand( ev );
    getByLabel_BS( ev );
}

inline void treeMain_TkTk::getByLabel_BS( fwlite::Event* ev )
{ beamSpotHandle.getByLabel( *ev,"offlineBeamSpot", "", "RECO"  ); return; }
