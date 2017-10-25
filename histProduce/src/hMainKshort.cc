#include "histProduce/histProduce/interface/hMainKshort.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "math.h"


histMain_Kshort::histMain_Kshort( TFileDirectory* d ) :
    histMain( d, histMain::Label("lbWriteSpecificDecay", "KshortCand", "bphAnalysis") )
{
    createHisto( "candInEvent_Kshort", 1000, 0.0, 30000. );
    createHisto( "candInVtxSort_Kshort", 80, 0.0, 2000. );
    createHisto( "parKshort_cosa2d" , 100, -1.0, 1.0 );
    createHisto( "parKshort_vtxprob", 100, 0., 1.);
    createHisto( "parKshort_FlightDistance"   , 105,-0.05,1.000);
    createHisto( "ptKshort_PosPion", 100., 0., 20.);
    createHisto( "ptKshort_NegPion", 100., 0., 20.);
    createHisto( "massKshort_Kshort", 40, 0.40, 0.6);
    createHisto( "massKshort_FakeLam0", 50, 1.10, 1.15 );
    createHisto( "massKshort_FakeKshort", 40, 0.40, 0.6 );
    createHisto( "specialPtKshort_FakeLam0", 150, 0., 30. );
    createHisto( "specialPtKshort_FakeKshort", 150, 0., 30. );
    createHisto( "specialParKshort", 8, 0., 8. );
}
void histMain_Kshort::Process( fwlite::Event* ev )
{
    try 
    {
        if ( !ev->isValid() ) return;
        _handle.getByLabel( *ev, _label.module.c_str(), _label.label.c_str(), _label.process.c_str() );

        // pv not recorded, use BS
        fwlite::Handle< reco::BeamSpot > beamSpotHandle;
        beamSpotHandle.getByLabel( *ev,"offlineBeamSpot", "", "RECO"  );
        if ( !beamSpotHandle.isValid() ) return;
        const reco::Vertex bs( (*beamSpotHandle).position(), (*beamSpotHandle).covariance3D() );
    
        std::map< double, const pat::CompositeCandidate*> vtxprobChooser;
        std::vector<pat::CompositeCandidate>::const_iterator handleIter = _handle->begin();
        std::vector<pat::CompositeCandidate>::const_iterator handleIend = _handle->end  ();
        while( handleIter != handleIend )
        {
            const pat::CompositeCandidate& cand = *handleIter++;
            if ( !cand.hasUserFloat( "fitMass" ) ) continue;
            if ( cand.userFloat( "fitMass" ) > 0.6 && cand.userFloat("fitMass") < 0.4 ) continue;
            if ( !cand.hasUserData("PiPos.fitMom") ) continue;
            if ( !cand.hasUserData("PiNeg.fitMom") ) continue;
            const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
            if ( _vtx == nullptr ) continue;
            double vtxprob = TMath::Prob( _vtx->chi2(), _vtx->ndof() );
            double fd = getFlightDistance ( cand, &bs );
            double cos2d = getCosa2d( cand, &bs );
            fillHisto( "parKshort_FlightDistance", fd );
            fillHisto( "parKshort_cosa2d", cos2d );
            fillHisto( "parKshort_vrxprob", vtxprob );
            if ( fd < 0.1 ) continue;
            if ( cos2d < 0.99 ) continue;
            if ( vtxprob < 0.2 ) continue;
            vtxprobChooser.insert( std::make_pair( vtxprob, &cand ) );
        }
        if ( vtxprobChooser.size() == 0 ) return;
        fillHisto("candInEvent_Kshort", _handle->size() );
        fillHisto("candInVtxSort_Kshort", vtxprobChooser.size() );

        std::map< double, const pat::CompositeCandidate*>::const_reverse_iterator iter = vtxprobChooser.rbegin();
        std::map< double, const pat::CompositeCandidate*>::const_reverse_iterator iend = vtxprobChooser.rend  ();

        //double kaonMass ( 0.493667 );
        double protonMass ( 0.9382720813 );
        double pionMass ( 0.13957061 );
        int getFirstNEvent = 0;
        while ( iter != iend )
        {
            if ( ++getFirstNEvent > 50 ) break;
            const pat::CompositeCandidate& cand = *(iter++->second);

            if ( !cand.hasUserFloat( "fitMass" ) ) continue;
            fillHisto( "massKshort_Kshort", cand.userFloat("fitMass") );
            
            const GlobalVector* dPTR[2] = { nullptr };
            dPTR[0] = cand.userData<GlobalVector>("PiPos.fitMom");
            dPTR[1] = cand.userData<GlobalVector>("PiNeg.fitMom");

            fillHisto( "ptKshort_PosPion", dPTR[0]->transverse() );
            fillHisto( "ptKshort_NegPion", dPTR[1]->transverse() );
            fourMom pTk ( dPTR[0]->x(), dPTR[0]->y(), dPTR[0]->z() );
            fourMom nTk ( dPTR[1]->x(), dPTR[1]->y(), dPTR[1]->z() );
            // reconstruct lambda0
            pTk.setMass( protonMass );
            nTk.setMass(   pionMass );


            fourMom twoTk = pTk + nTk;
            double mass = twoTk.Mag();
            int specialTag = 0;
            if ( mass > 1.1 && mass < 1.15 )
                fillHisto( "massKshort_FakeLam0", mass );
            if ( mass - protonMass - pionMass < 0.01 )
            {
                specialTag += 1 << 1;
                fillHisto( "specialPtKshort_FakeLam0", twoTk.transverse() );
            }
            
            // reconstruct k short
            pTk.setMass( pionMass );
            twoTk = pTk + nTk;
            mass = twoTk.Mag();
            if ( mass > 0.20 && mass < 1.0 )
                fillHisto( "massKshort_FakeKshort", mass );
            if ( mass - 2*pionMass < 0.01 )
            {
                specialTag += 1 << 2;
                fillHisto( "specialPtKshort_FakeKshort", twoTk.transverse() );
            }
            if ( specialTag )
                fillHisto( "specialParKshort", specialTag );
        }
    } catch (...) {}
}

void histMain_Kshort::Clear()
{
}
double histMain_Kshort::getFlightDistance( const pat::CompositeCandidate& cand, const reco::Vertex* _bs )
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
double histMain_Kshort::getCosa2d( const pat::CompositeCandidate& cand, const reco::Vertex* _bs )
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
