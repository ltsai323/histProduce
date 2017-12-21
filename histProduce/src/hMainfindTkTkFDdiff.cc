#include "histProduce/histProduce/interface/hMainfindTkTkFDdiff.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "math.h"


histMain_findTkTkFlightDistanceDiff::histMain_findTkTkFlightDistanceDiff( TFileDirectory* d ) :
    histMain( d, histMain::Label("lbWriteSpecificDecay", "TkTkFitted", "bphAnalysis"), "TkTkFDdiff" )
{
    _nMap[fd030] = "fd030";
    _nMap[fd060] = "fd060";
    _nMap[fd090] = "fd090";
    _nMap[fd120] = "fd120";
    _nMap[fd150] = "fd150";
    _nMap[fd180] = "fd180";
    _nMap[fd210] = "fd210";
    _nMap[fd240] = "fd240";
    for ( const auto& _dName : _nMap )
    {
        const std::string& name = _dName.second;
        createHisto( name+"massTkTk_FakeLam0"        ,  50, 1.10, 1.15 );
        createHisto( name+"massTkTk_FakeKshort"      , 160, 0.2, 1.0 );
    }
}
void histMain_findTkTkFlightDistanceDiff::Process( fwlite::Event* ev )
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
            double fd = usefulFuncs::getFlightDistance ( cand, &bs );
            if ( fd < 0.1 ) continue;

            double cos2d = usefulFuncs::getCosa2d( cand, &bs );
            if ( cos2d < 0.99 ) continue;

            //if ( ++vtxprobSortLimit > 2 ) break;

            // boolean int to recognize the tags, if all tags are false, fill in the histogram: massTkTk_withCuts
            std::map< double, const reco::Candidate* > dauMap;
            for ( unsigned i=0; i<cand.numberOfDaughters(); ++i )
            {
                const reco::Candidate* dPtr = cand.daughter(i);
                dauMap.insert( std::make_pair( dPtr->pt(), dPtr ) );
            }

            if ( dauMap.size() != 2 ) continue;
            std::map< double, const reco::Candidate* >::const_reverse_iterator dauRIter = dauMap.rbegin();
            std::map< double, const reco::Candidate* >::const_reverse_iterator dauRIend = dauMap.rend  ();
            // first one is proton and second one is kaon ( consider bigger momentum with heavier particle )
            const reco::Candidate* dPTR[2] = {nullptr};
            int IDX = 0;
            while ( dauRIter != dauRIend )
                dPTR[IDX++] = dauRIter++->second;

            fourMom pTk ( dPTR[0]->px(), dPTR[0]->py(), dPTR[0]->pz() );
            fourMom nTk ( dPTR[1]->px(), dPTR[1]->py(), dPTR[1]->pz() );
            // reconstruct lambda0
            pTk.setMass( protonMass );
            nTk.setMass(   pionMass );


            fourMom twoTk = pTk + nTk;
            for ( const auto& name : _nMap )
            {
                const std::string& dirname = name.second;
                double i ( name.first );
                if ( fd < 0.3*i*0.3*i ) continue;
                fillHisto( dirname+"massTkTk_FakeLam0", twoTk.Mag() );
            }
            // reconstruct k short
            pTk.setMass( pionMass );
            twoTk = pTk + nTk;
            for ( const auto& name : _nMap )
            {
                const std::string& dirname = name.second;
                double i ( name.first );
                if ( fd < 0.1*i*0.1*i ) continue;
                fillHisto( dirname+"massTkTk_FakeKshort", twoTk.Mag() );
            }

        }
    } catch (...) {}
}

void histMain_findTkTkFlightDistanceDiff::Clear()
{
}
//double histMain_findTkTkFlightDistanceDiff::getFlightDistance( const pat::CompositeCandidate& cand, const reco::Vertex* _bs )
//{
//    if ( !cand.hasUserData( "fitVertex" ) ) return -999.;
//    const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
//    if ( _vtx == nullptr ) return -999.;
//    if ( _bs == nullptr ) return -999.;
//
//    double _x ( _vtx->x() ); double _y ( _vtx->y() );
//    double _px( _bs->x() ); double _py( _bs->y() );
//    double dist ( (_x-_px)*(_x-_px) + (_y-_py)*(_y-_py) );
//    return sqrt ( dist );
//}
//double histMain_findTkTkFlightDistanceDiff::getCosa2d( const pat::CompositeCandidate& cand, const reco::Vertex* _bs )
//{
//    const GlobalVector* _mom = usefulFuncs::get<GlobalVector>( cand, "fitMomentum" );
//    if ( _mom == nullptr ) return -999.;
//    const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
//    if ( _vtx == nullptr ) return -999.;
//    if ( _bs == nullptr ) return -999.;
//    double _x = _vtx->x() - _bs->x();
//    double _y = _vtx->y() - _bs->y();
//    double _r = sqrt( _x*_x+_y*_y );
//    double mx = _mom->x();
//    double my = _mom->y();
//    double cosa2d = (mx*_x + my*_y) / (_mom->transverse()*_r);
//    return cosa2d;
//}
