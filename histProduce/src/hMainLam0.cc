#include "histProduce/histProduce/interface/hMainLam0.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "math.h"


histMain_Lam0::histMain_Lam0( TFileDirectory* d ) :
    histMain( d, histMain::Label("lbWriteSpecificDecay", "Lam0Cand", "bphAnalysis") )
{
    createHisto( "parLam0_FlightDistance"   , 1051, -0.050, 1.000 );
    createHisto( "candInEvent"              ,   40, 0.0, 40. );
    createHisto( "parLam0_cosa2d"           , 100, -1.0, 1.0 );
    createHisto( "massLam0_Lam0"        ,  50, 1.10, 1.15 );
    createHisto( "ptProton",  50, 0., 20. );
    createHisto( "ptPion", 50., 0., 20. );
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
        const reco::Vertex bs( (*beamSpotHandle).position(), (*beamSpotHandle).covariance3D() );
    
        std::map< double, const pat::CompositeCandidate*> vtxprobChooser;
        std::vector<pat::CompositeCandidate>::const_iterator handleIter = _handle->begin();
        std::vector<pat::CompositeCandidate>::const_iterator handleIend = _handle->end  ();
        while( handleIter != handleIend )
        {
            const pat::CompositeCandidate& cand = *handleIter++;
            const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
            if ( _vtx == nullptr ) continue;
            vtxprobChooser.insert( std::make_pair( TMath::Prob( _vtx->chi2(), _vtx->ndof() ), &cand ) );
        }
        if ( vtxprobChooser.size() == 0 ) std::cout << "warining nothing in the event\n";
        fillHisto("candInEvent", vtxprobChooser.size() );

        std::map< double, const pat::CompositeCandidate*>::const_reverse_iterator iter = vtxprobChooser.rbegin();
        std::map< double, const pat::CompositeCandidate*>::const_reverse_iterator iend = vtxprobChooser.rend  ();
        //int vtxprobSortLimit = 0;

        //double kaonMass ( 0.493667 );
        double protonMass ( 0.9382720813 );
        double pionMass ( 0.13957061 );
        while ( iter != iend )
        {
            const pat::CompositeCandidate& cand = *(iter++->second);
            if ( !cand.hasUserFloat( "fitMass" ) ) continue;
            fillHisto( "massLam0_Lam0", cand.userFloat("fitMass") );

            if ( cand.hasUserData("Proton.fitMom") )
                fillHisto( "ptProton", cand.userData<GlobalVector>("Proton.fitMom")->transverse() );
            else
                std::cout << "refit proton track not found\n";
            double fd = getFlightDistance ( cand, &bs );
            fillHisto( "parLam0_FlightDistance", fd );
            if ( fd < 0.1 ) continue;

            double cos2d = getCosa2d( cand, &bs );
            fillHisto( "parLam0_cosa2d", cos2d );
            if ( cos2d < 0.99 ) continue;
            

            //if ( ++vtxprobSortLimit > 2 ) break;
            
            //// boolean int to recognize the tags, if all tags are false, fill in the histogram: massLam0_withCuts
            //std::map< double, const reco::Candidate* > dauMap;
            //for ( unsigned i=0; i<cand.numberOfDaughters(); ++i )
            //{
            //    const reco::Candidate* dPtr = cand.daughter(i);
            //    dauMap.insert( std::make_pair( dPtr->pt(), dPtr ) );
            //}
            //
            //if ( dauMap.size() != 2 ) continue;
            //std::map< double, const reco::Candidate* >::const_reverse_iterator dauRIter = dauMap.rbegin();
            //std::map< double, const reco::Candidate* >::const_reverse_iterator dauRIend = dauMap.rend  ();
            //// first one is proton and second one is kaon ( consider bigger momentum with heavier particle )
            //const reco::Candidate* dPTR[2] = {nullptr};
            //int IDX = 0;
            //while ( dauRIter != dauRIend )
            //    dPTR[IDX++] = dauRIter++->second;
            //
            //fourMom pTk ( dPTR[0]->px(), dPTR[0]->py(), dPTR[0]->pz() );
            //fourMom nTk ( dPTR[1]->px(), dPTR[1]->py(), dPTR[1]->pz() );
            //// reconstruct lambda0
            //pTk.setMass( protonMass );
            //nTk.setMass(   pionMass );


            //fourMom twoTk = pTk + nTk;
            //fillHisto( "massLam0_FakeLam0", twoTk.Mag() );
            //// reconstruct k short
            //pTk.setMass( pionMass );
            //twoTk = pTk + nTk;
            //fillHisto( "massLam0_FakeKshort", twoTk.Mag() );

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
