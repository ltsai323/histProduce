#include "histProduce/histProduce/interface/hMainLam0.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "math.h"


histMain_Lam0::histMain_Lam0( TFileDirectory* d ) :
    histMain( d, histMain::Label("lbWriteSpecificDecay", "Lam0Cand", "bphAnalysis") )
{
    createHisto( "candInEvent_Lam0", 100, 0.0, 100. );
    createHisto( "candInVtxsort_Lam0", 50, 0.0, 50. );
    createHisto( "parLam0_cosa2d", 100, -1.0, 1.0 );
    createHisto( "parLam0_FlightDistance", 1051, -0.050, 1.000 );
    createHisto( "parLam0_vtxprob", 100, 0., 1.);
    createHisto( "ptLam0_Proton", 100, 0., 20. );
    createHisto( "ptLam0_Pion", 100., 0., 20. );
    createHisto( "massLam0_Lam0", 50, 1.10, 1.15 );
    createHisto( "massLam0_FakeLam0", 50, 1.10, 1.15 );
    createHisto( "massLam0_FakeKshort", 40, 0.40, 0.6 );
    createHisto( "specialPtLam0_FakeLam0", 150, 0., 30. );
    createHisto( "specialPtLam0_FakeKshort", 150, 0., 30. );
    createHisto( "specialPtDiffLam0_FakeLam0", 150, 0., 5. );
    createHisto( "specialPtDiffLam0_FakeKshort", 150, 0., 5. );
    createHisto( "specialParLam0", 8, 0., 8. );
}
void histMain_Lam0::Process( fwlite::Event* ev )
{
    try 
    {
        if ( !ev->isValid() ) return;
        _handle.getByLabel( *ev, _label.module.c_str(), _label.label.c_str(), _label.process.c_str() );

        // pv not recorded, use BS
        fwlite::Handle< reco::BeamSpot > beamSpotHandle;
        beamSpotHandle.getByLabel( *ev,"offlineBeamSpot", "", "RECO"  );
        if ( !beamSpotHandle.isValid() ) return;
        if ( _handle->size() == 0 ) return;
        const reco::Vertex bs( (*beamSpotHandle).position(), (*beamSpotHandle).covariance3D() );
    
        std::map< double, const pat::CompositeCandidate*> vtxprobChooser;
        std::vector<pat::CompositeCandidate>::const_iterator handleIter = _handle->begin();
        std::vector<pat::CompositeCandidate>::const_iterator handleIend = _handle->end  ();
        while( handleIter != handleIend )
        {
            const pat::CompositeCandidate& cand = *handleIter++;
            const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
            if ( !cand.hasUserFloat( "fitMass" ) ) continue;
            if ( _vtx == nullptr ) continue;
            if ( !cand.hasUserData("Proton.fitMom") ) continue;
            if ( !cand.hasUserData("Pion.fitMom") ) continue;

            fillHisto( "specialParLam0_massDifferent", fabs(cand.userFloat("fitMass") - cand.mass()) );
            fillHisto( "specialParLam0_pptDifferent", fabs(cand.userData<GlobalVector>("Proton.fitMom")->transverse() - cand.pt()) );
            fillHisto( "specialParLam0_nptDifferent", fabs(cand.userData<GlobalVector>("Pion.fitMom")->transverse() - cand.pt()) );

            double fd = getFlightDistance ( cand, &bs );
            double cos2d = getCosa2d( cand, &bs );
            double vtxprob = TMath::Prob( _vtx->chi2(), _vtx->ndof() );
            fillHisto( "parLam0_vtxprob", vtxprob );
            fillHisto( "parLam0_FlightDistance", fd );
            fillHisto( "parLam0_cosa2d", cos2d );
            if ( fd < 0.1 ) continue;
            if ( cos2d < 0.99 ) continue;
            if ( vtxprob < 0.1 ) continue;
            vtxprobChooser.insert( std::make_pair( vtxprob, &cand ) );
        }
        if ( vtxprobChooser.size() == 0 ) return;
        fillHisto("candInEvent_Lam0", _handle->size() );
        fillHisto("candInVtxsort_Lam0", vtxprobChooser.size() );

        std::map< double, const pat::CompositeCandidate*>::const_reverse_iterator iter = vtxprobChooser.rbegin();
        std::map< double, const pat::CompositeCandidate*>::const_reverse_iterator iend = vtxprobChooser.rend  ();

        //double kaonMass ( 0.493667 );
        double protonMass ( 0.9382720813 );
        double pionMass ( 0.13957061 );
        while ( iter != iend )
        {
            const pat::CompositeCandidate& cand = *(iter++->second);
            fillHisto( "massLam0_Lam0", cand.userFloat("fitMass") );
            
            const GlobalVector* dPTR[2] = { nullptr };

            dPTR[0] = cand.userData<GlobalVector>("Proton.fitMom");
            dPTR[1] = cand.userData<GlobalVector>("Pion.fitMom");
            
            fillHisto( "ptLam0_Proton", dPTR[0]->transverse() );
            fillHisto( "ptLam0_Pion"  , dPTR[1]->transverse() );
            fourMom pTk ( dPTR[0]->x(), dPTR[0]->y(), dPTR[0]->z() );
            fourMom nTk ( dPTR[1]->x(), dPTR[1]->y(), dPTR[1]->z() );
            // reconstruct lambda0
            pTk.setMass( protonMass );
            nTk.setMass(   pionMass );


            fourMom twoTk = pTk + nTk;
            double mass = twoTk.Mag();
            int specialTag = 0;
            if ( mass > 1.10 && mass < 1.15 )
                fillHisto( "massLam0_FakeLam0", mass );

            if ( mass - protonMass - pionMass < 0.01 )
            {
                specialTag += 1 << 1;
                fillHisto( "specialPtLam0_FakeLam0", twoTk.transverse() );
                fillHisto( "specialPtDiffLam0_FakeLam0", fabs( pTk.transverse() - nTk.transverse() ) );
            }
            // reconstruct k short
            pTk.setMass( pionMass );
            twoTk = pTk + nTk;
            mass = twoTk.Mag();
            if ( mass > 0.40 && mass < 0.6 )
                fillHisto( "massLam0_FakeKshort", mass );
            if ( mass - 2*pionMass < 0.01 )
            {
                specialTag += 1 << 2;
                fillHisto( "specialPtLam0_FakeKshort", twoTk.transverse() );
                fillHisto( "specialPtDiffLam0_FakeKshort", fabs( pTk.transverse() - nTk.transverse() ) );
            }
            if ( specialTag )
                fillHisto( "specialParLam0", specialTag );

        }
    } catch (...) {}
}

void histMain_Lam0::Clear()
{
}
double histMain_Lam0::getFlightDistance( const pat::CompositeCandidate& cand, const reco::Vertex* _bs )
{
    if ( !cand.hasUserData( "fitVertex" ) ) return -999.;
    const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
    if ( _vtx == nullptr ) return -999.;
    if ( _bs == nullptr ) return -999.;

    double _x ( _vtx->x() ); double _y ( _vtx->y() );
    double _px( _bs->x() ); double _py( _bs->y() );
    double dist ( (_x-_px)*(_x-_px) + (_y-_py)*(_y-_py) );
    return sqrt ( dist );
}
double histMain_Lam0::getCosa2d( const pat::CompositeCandidate& cand, const reco::Vertex* _bs )
{
    const GlobalVector* _mom = usefulFuncs::get<GlobalVector>( cand, "fitMomentum" );
    if ( _mom == nullptr ) return -999.;
    const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
    if ( _vtx == nullptr ) return -999.;
    if ( _bs == nullptr ) return -999.;
    double _x = _vtx->x() - _bs->x();
    double _y = _vtx->y() - _bs->y();
    double _r = sqrt( _x*_x+_y*_y );
    double mx = _mom->x();
    double my = _mom->y();
    double cosa2d = (mx*_x + my*_y) / (_mom->transverse()*_r);
    return cosa2d;
}
