#include "histProduce/histProduce/interface/hMainfindFDdiff.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
//
// this code find Lambda0 & K_short by separate flight distance.
// consider the track with larger pt as proton track ( for Lambda0 )
//

namespace myMass
{
    const double protonMass(0.9382720813);
    const double   kaonMass(0.493667);
    const double   pionMass(0.13957061);
    const double   muonMass(0.1056583715);
};

histMain_findFlightDistanceDiff::histMain_findFlightDistanceDiff( TFileDirectory* d ) :
    histMain( d, histMain::Label("lbWriteSpecificDecay", "LbToTkTkFitted", "bphAnalysis") )
{
    using namespace myCut;

    _nMap[fd05] = "fd05";
    _nMap[fd10] = "fd10";
    _nMap[fd15] = "fd15";
    _nMap[fd20] = "fd20";
    _nMap[fd25] = "fd25";
    for ( const auto& _dName : _nMap )
    {
        const std::string& name = _dName.second;
        createHisto( name+"massFakeLam0",  50, 1.10, 1.15 );
        createHisto( name+"massFakeLbL0", 200, 5.0, 6.0 );
        createHisto( name+"massFakeLam0_withCut",  50, 1.10, 1.15 );
        createHisto( name+"massFakeLbL0_withCut", 200, 5.0, 6.0 );
        createHisto( name+"massFakeKs",  60, 0.2, 0.8);
    }
    createHisto( "massFakeLam0", 120, 1.10, 1.50 );
    createHisto( "massFakeLbL0", 400, 5.0, 6.0 );
    createHisto( "flightDistTkTkinLb", 1051, -0.050, 1.000 );

}
void histMain_findFlightDistanceDiff::Process( fwlite::Event* ev )
{
    try 
    {
        if ( !ev->isValid() ) return;
        
        _handle.getByLabel( *ev, _label.module.c_str(), _label.label.c_str(), _label.process.c_str() );
    
        std::map< double, const pat::CompositeCandidate*> vtxprobChooser;
        std::vector<pat::CompositeCandidate>::const_iterator handleIter = _handle->begin();
        std::vector<pat::CompositeCandidate>::const_iterator handleIend = _handle->end  ();
        while( handleIter != handleIend )
        {
            const pat::CompositeCandidate& cand = *handleIter++;
            const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
            if ( _vtx == nullptr ) continue;

            const pat::CompositeCandidate* jpsiCand = usefulFuncs::getByRef<pat::CompositeCandidate>( cand, "refToJPsi" );
            if ( jpsiCand == nullptr ) continue;
            const pat::CompositeCandidate* tktkCand = usefulFuncs::getByRef<pat::CompositeCandidate>( cand, "refToTkTk" );
            if ( tktkCand == nullptr ) continue;
            const reco::Vertex* tktkVtx = usefulFuncs::get<reco::Vertex>( *tktkCand, "fitVertex" );
            if ( tktkVtx == nullptr ) continue;

            if ( !cand.hasUserFloat("TkTk/Proton.IPt") ) continue;
            if ( !cand.hasUserFloat("TkTk/Kaon.IPt") ) continue;
            if ( !cand.hasUserFloat("TkTk/Proton.IPt.Error") ) continue;
            if ( !cand.hasUserFloat("TkTk/Kaon.IPt.Error") ) continue;
            if ( !cand.hasUserFloat("fitMass") ) continue;
            vtxprobChooser.insert( std::make_pair( TMath::Prob( _vtx->chi2(), _vtx->ndof() ), &cand ) );
        }

        std::map< double, const pat::CompositeCandidate*>::const_reverse_iterator iter = vtxprobChooser.rbegin();
        std::map< double, const pat::CompositeCandidate*>::const_reverse_iterator iend = vtxprobChooser.rend  ();
        int vtxprobSortLimit = 0;
        while ( iter != iend )
        {
            const pat::CompositeCandidate& cand = *(iter++->second);
            bool cutTag = false;
            const std::vector<myCut::generalCutList*>* generalCut = getCutList();
            std::vector<myCut::generalCutList*>::const_iterator gcIter = generalCut->begin();
            std::vector<myCut::generalCutList*>::const_iterator gcIend = generalCut->end  ();
            while ( gcIter != gcIend )
            {
                myCut::generalCutList* gCut = *gcIter++;
                if ( !gCut->accept(cand) )
                { cutTag = true; break; }
            }
            if ( cutTag ) continue;

            if ( ++vtxprobSortLimit > 2 ) break;

            // boolean int to recognize the tags, if all tags are false, fill in the histogram: massLbTk_withCuts
            if ( cand.hasUserFloat("fitMass") )
                fillHisto( "massLbTk", cand.userFloat("fitMass") );
            if ( cand.hasUserData("fitMomentum") )
                fillHisto( "ptLbTk", cand.userData<GlobalVector>("fitMomentum")->transverse() );
    
            const pat::CompositeCandidate* tktkCand = usefulFuncs::getByRef<pat::CompositeCandidate>( cand, "refToTkTk" );
            const pat::CompositeCandidate* jpsiCand = usefulFuncs::getByRef<pat::CompositeCandidate>( cand, "refToJPsi" );
            const reco::Vertex* tktkVtx = usefulFuncs::get<reco::Vertex>( *tktkCand, "fitVertex" );
            const reco::Vertex* __pv = usefulFuncs::getByRef<reco::Vertex>( *jpsiCand, "primaryVertex" );
            double transFD = ( tktkVtx->x() - __pv->x() )*( tktkVtx->x()-__pv->x() ) + (tktkVtx->y()-__pv->y())*(tktkVtx->y()-__pv->y());
            fillHisto( "flightDistTkTkinLb", sqrt( transFD ) );
            if ( transFD < 0.01*0.01 ) continue;
            

            if ( cand.hasUserData("TkTk/Proton.fitMom") && cand.hasUserData("TkTk/Kaon.fitMom") )
                if ( cand.hasUserData("JPsi/MuPos.fitMom") && cand.hasUserData("JPsi/MuNeg.fitMom") )
                {
                    const GlobalVector* pTkMom = cand.userData<GlobalVector>("TkTk/Proton.fitMom");
                    const GlobalVector* nTkMom = cand.userData<GlobalVector>("TkTk/Kaon.fitMom");
                    const GlobalVector* pmuMom = cand.userData<GlobalVector>("JPsi/MuPos.fitMom");
                    const GlobalVector* nmuMom = cand.userData<GlobalVector>("JPsi/MuNeg.fitMom");
                    bool pTkBigger = false;
                    if ( pTkMom->transverse() > nTkMom->transverse() ) pTkBigger = true;
    
                    fourMom pTk( pTkMom->x(), pTkMom->y(), pTkMom->z() );
                    fourMom nTk( nTkMom->x(), nTkMom->y(), nTkMom->z() );
                    fourMom pmu( pmuMom->x(), pmuMom->y(), pmuMom->z() );
                    fourMom nmu( nmuMom->x(), nmuMom->y(), nmuMom->z() );
    
                    if ( pTkBigger )
                    {
                        pTk.setMass( myMass::protonMass );
                        nTk.setMass( myMass::  pionMass );
                    }
                    else
                    {
                        nTk.setMass( myMass::protonMass );
                        pTk.setMass( myMass::  pionMass );
                    }
                    pmu.setMass( myMass::muonMass );
                    nmu.setMass( myMass::muonMass );
    
                    fourMom fourTk = pTk + nTk + pmu + nmu;
                    fourMom twoTk = pTk + nTk;
                    fillHisto( "massFakeLam0", twoTk.Mag() );
                    fillHisto( "massFakeLbL0", fourTk.Mag() );
                    for ( const auto&  name : _nMap )
                    {
                        const std::string& dirname = name.second;
                        double i( name.first );
                        if ( transFD < 0.5*i*0.5*i ) continue;
                        fillHisto( dirname+"massFakeLam0", twoTk.Mag() );
                        fillHisto( dirname+"massFakeLbL0", fourTk.Mag() );
                        if ( twoTk.Mag() > 1.110 && twoTk.Mag() < 1.120 )
                            fillHisto( dirname+"massFakeLbL0_withCut", fourTk.Mag() );
                        if ( fourTk.Mag() > 5.58 && fourTk.Mag() < 5.63 )
                            fillHisto( dirname+"massFakeLam0_withCut", twoTk.Mag() );


                    }
                    pTk.setMass( myMass::pionMass );
                    nTk.setMass( myMass::pionMass );
                    twoTk = pTk + nTk;
                    for ( const auto&  name : _nMap )
                    {
                        const std::string& dirname = name.second;
                        double i( name.first );
                        if ( transFD < 0.05*i*0.05*i ) continue;
                        fillHisto( dirname+"massFakeKs", twoTk.Mag() );
                    }




                } // double if ended

        }
    } catch ( ... ) {}
}

void histMain_findFlightDistanceDiff::Clear()
{
}

