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
        //const reco::Vertex bs( (*beamSpotHandle).position(), (*beamSpotHandle).covariance3D() );
        const reco::BeamSpot& bs = *beamSpotHandle;

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

            const GlobalVector* dPTR[2] = {nullptr};
            dPTR[0] = cand.userData<GlobalVector>("TkTk/Proton.fitMom");
            dPTR[1] = cand.userData<GlobalVector>("TkTk/Kaon.fitMom");

            const GlobalVector* muPTR[2] = {nullptr};
            muPTR[0] = cand.userData<GlobalVector>("JPsi/MuPos.fitMom");
            muPTR[1] = cand.userData<GlobalVector>("JPsi/MuNeg.fitMom");

            fourMom pMu ( muPTR[0]->x(), muPTR[0]->y(), muPTR[0]->z() );
            fourMom nMu ( muPTR[1]->x(), muPTR[1]->y(), muPTR[1]->z() );
            fourMom pTk ( dPTR[0]->x(), dPTR[0]->y(), dPTR[0]->z() );
            fourMom nTk ( dPTR[1]->x(), dPTR[1]->y(), dPTR[1]->z() );
            //fourMom tktk = pTk+nTk;
            fourMom fourTk = pTk+nTk+pMu+nMu;
            // preselection
            if ( pMu.transverse() < 5.0 ) continue;
            if ( nMu.transverse() < 5.0 ) continue;
            if ( pMu.Momentum()   < 5.0 ) continue;
            if ( nMu.Momentum()   < 5.0 ) continue;
            if ( pTk.transverse() < 1.0 ) continue;
            if ( nTk.transverse() < 1.0 ) continue;
            if ( pTk.Momentum()   < 1.0 ) continue;
            if ( nTk.Momentum()   < 1.0 ) continue;

            if ( fourTk.transverse()< 12. ) continue;
            if ( fourTk.Momentum()  < 12. ) continue;


            const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
            if ( _vtx == nullptr ) continue;
            double fdSig = usefulFuncs::getFlightDistanceSignificance ( cand, &bs );
            if ( fdSig < 3.0 ) continue;
            double cos2d = usefulFuncs::getCosa2d( cand, &bs );
            double vtxprob = TMath::Prob( _vtx->chi2(), _vtx->ndof() );
            if ( cos2d < 0.99 ) continue;
            if ( vtxprob < 0.15 ) continue;

            candsSorted.emplace_back( vtxprob, &cand );
        }
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

            // search for pentaQuark
            // jpsip = jpsi + proton, jpsiP = jpsi + anti-proton
            fourMom jpsip, jpsipBar;
            pTk.setMass( protonMass );
            nTk.setMass( protonMass );
            jpsip = pMu + nMu + pTk;
            jpsipBar = pMu + nMu + nTk;

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
            dataD[lbtkFlightDistance2d] = usefulFuncs::getFlightDistance ( cand, &bs );
            dataD[lbtkFlightDistanceSig]= usefulFuncs::getFlightDistanceSignificance ( cand, &bs );
            const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
            dataD[lbtkVtxprob] = TMath::Prob( _vtx->chi2(), _vtx->ndof() );
            dataD[lbtkCosa2d] = usefulFuncs::getCosa2d(cand,&bs);

            dataD[targetJpsiP_mass] = jpsip.Mag();
            dataD[targetJpsiP_pt] = jpsip.transverse();
            dataD[targetJpsiPBar_mass] = jpsipBar.Mag();
            dataD[targetJpsiPBar_pt] = jpsipBar.transverse();

            dataD[lbtkMom]= fourTk.Momentum();
            dataD[lbtkPt] = fourTk.transverse();
            dataD[tktkPt] = tktk.transverse();
            dataD[tktkMom]= tktk.Momentum();
            dataD[ptkPt]  = pTk.transverse();
            dataD[ntkPt]  = nTk.transverse();
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
    thisTree()->Branch( "lbtkFDSig", &dataD[lbtkFlightDistanceSig], "lbtkFDSig/D" );
    thisTree()->Branch( "lbtkVtxprob", &dataD[lbtkVtxprob], "lbtkVtxprob/D" );
    thisTree()->Branch( "lbtkCosa2d", &dataD[lbtkCosa2d], "lbtkCosa2d/D" );

    thisTree()->Branch( "targetJpsiP_mass", &dataD[targetJpsiP_mass], "targetJpsiP_mass/D" );
    thisTree()->Branch( "targetJpsiP_pt", &dataD[targetJpsiP_pt], "targetJpsiP_pt/D" );
    thisTree()->Branch( "targetJpsiPBar_mass", &dataD[targetJpsiPBar_mass], "targetJpsiPBar_mass/D" );
    thisTree()->Branch( "targetJpsiPBar_pt", &dataD[targetJpsiPBar_pt], "targetJpsiPBar_pt/D" );

    thisTree()->Branch( "lbtkMom", &dataD[lbtkMom], "lbtkMom/D" );
    thisTree()->Branch( "lbtkPt", &dataD[lbtkPt], "lbtkPt/D" );
    thisTree()->Branch( "tktkPt", &dataD[tktkPt], "tktkPt/D" );
    thisTree()->Branch( "tktkMom", &dataD[tktkMom], "tktkMom/D" );

    thisTree()->Branch( "fake_Lam0Mass", &dataD[fake_Lam0Mass], "fake_Lam0Mass/D" );
    thisTree()->Branch( "fake_LbL0Mass", &dataD[fake_LbL0Mass], "fake_LbL0Mass/D" );
    thisTree()->Branch( "fake_KstarMass", &dataD[fake_KstarMass], "fake_KstarMass/D" );
    thisTree()->Branch( "fake_BdMass", &dataD[fake_BdMass], "fake_BdMass/D" );
    thisTree()->Branch( "fake_PhiMass", &dataD[fake_PhiMass], "fake_PhiMass/D" );
    thisTree()->Branch( "fake_BsMass", &dataD[fake_BsMass], "fake_BsMass/D" );
    thisTree()->Branch( "fake_KshortMass", &dataD[fake_KshortMass], "fake_KshortMass/D" );
    thisTree()->Branch( "fake_mumupipiMass", &dataD[fake_mumupipiMass], "fake_mumupipiMass/D" );

    thisTree()->Branch( "ptkPt", &dataD[ptkPt], "ptkPt/D" );
    thisTree()->Branch( "ptkMom", &dataD[ptkMom], "ptkMom/D" );
    thisTree()->Branch( "ptkDEDX.Harmonic", &dataD[ptkDEDX_Harmonic], "ptkDEDX.Harmonic/D" );
    thisTree()->Branch( "ptkDEDX.pixelHrm", &dataD[ptkDEDX_pixelHrm], "ptkDEDX.pixelHrm/D" );
    thisTree()->Branch( "ptkIPt", &dataD[ptkIPt], "ptkIPt/D" );
    thisTree()->Branch( "ptkIPtErr", &dataD[ptkIPtErr], "ptkIPtErr/D" );

    thisTree()->Branch( "ntkPt", &dataD[ntkPt], "ntkPt/D" );
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


void treeMain_LbTk::setBranchAddress( TTree* inputTree )
{
    inputTree->SetBranchAddress( "lbtkMass", &dataD[lbtkMass] );
    inputTree->SetBranchAddress( "lbtkFD2d", &dataD[lbtkFlightDistance2d] );
    inputTree->SetBranchAddress( "lbtkFDSig", &dataD[lbtkFlightDistanceSig] );
    inputTree->SetBranchAddress( "lbtkVtxprob", &dataD[lbtkVtxprob] );
    inputTree->SetBranchAddress( "lbtkCosa2d", &dataD[lbtkCosa2d] );

    inputTree->SetBranchAddress( "targetJpsiP_mass", &dataD[targetJpsiP_mass] );
    inputTree->SetBranchAddress( "targetJpsiP_pt", &dataD[targetJpsiP_pt] );
    inputTree->SetBranchAddress( "targetJpsiPBar_mass", &dataD[targetJpsiPBar_mass] );
    inputTree->SetBranchAddress( "targetJpsiPBar_pt", &dataD[targetJpsiPBar_pt] );

    inputTree->SetBranchAddress( "lbtkMom", &dataD[lbtkMom] );
    inputTree->SetBranchAddress( "lbtkPt", &dataD[lbtkPt] );
    inputTree->SetBranchAddress( "tktkPt", &dataD[tktkPt] );
    inputTree->SetBranchAddress( "tktkMom", &dataD[tktkMom] );

    inputTree->SetBranchAddress( "fake_Lam0Mass", &dataD[fake_Lam0Mass] );
    inputTree->SetBranchAddress( "fake_LbL0Mass", &dataD[fake_LbL0Mass] );
    inputTree->SetBranchAddress( "fake_KstarMass", &dataD[fake_KstarMass] );
    inputTree->SetBranchAddress( "fake_BdMass", &dataD[fake_BdMass] );
    inputTree->SetBranchAddress( "fake_PhiMass", &dataD[fake_PhiMass] );
    inputTree->SetBranchAddress( "fake_BsMass", &dataD[fake_BsMass] );
    inputTree->SetBranchAddress( "fake_KshortMass", &dataD[fake_KshortMass] );
    inputTree->SetBranchAddress( "fake_mumupipiMass", &dataD[fake_mumupipiMass] );

    inputTree->SetBranchAddress( "ptkPt", &dataD[ptkPt] );
    inputTree->SetBranchAddress( "ptkMom", &dataD[ptkMom] );
    inputTree->SetBranchAddress( "ptkDEDX.Harmonic", &dataD[ptkDEDX_Harmonic] );
    inputTree->SetBranchAddress( "ptkDEDX.pixelHrm", &dataD[ptkDEDX_pixelHrm] );
    inputTree->SetBranchAddress( "ptkIPt", &dataD[ptkIPt] );
    inputTree->SetBranchAddress( "ptkIPtErr", &dataD[ptkIPtErr] );

    inputTree->SetBranchAddress( "ntkPt", &dataD[ntkPt] );
    inputTree->SetBranchAddress( "ntkMom", &dataD[ntkMom] );
    inputTree->SetBranchAddress( "ntkDEDX.Harmonic", &dataD[ntkDEDX_Harmonic] );
    inputTree->SetBranchAddress( "ntkDEDX.pixelHrm", &dataD[ntkDEDX_pixelHrm] );
    inputTree->SetBranchAddress( "ntkIPt", &dataD[ntkIPt] );
    inputTree->SetBranchAddress( "ntkIPtErr", &dataD[ntkIPtErr] );
    return;
}
