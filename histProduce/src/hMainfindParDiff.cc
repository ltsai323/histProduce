#include "histProduce/histProduce/interface/hMainfindParDiff.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"


histMain_findParDiff::histMain_findParDiff( TFileDirectory* d ) :
    histMain( d, histMain::Label("lbWriteSpecificDecay", "LbToTkTkFitted", "bphAnalysis") )
{
    using namespace myCut;

    origDir = d;
    _nMap[fd1S] = "fd1S";
    _nMap[fd2S] = "fd2S";
    _nMap[fd3S] = "fd3S";
    _nMap[fd4S] = "fd4S";
    _nMap[fd5S] = "fd5S";
    for ( const auto& _dName : _nMap )
    {
        const std::string& name = _dName.second;
        createHisto( name+"massLbTk", 50, 5.0, 6.0 );
        createHisto( name+"massTkTk", 50, 1., 1.5 );
        createHisto( name+"massFakeBd", 50, 5.0, 6.0 );
        createHisto( name+"massFakeBd_withCuts", 50, 5.0, 6.0 );
        createHisto( name+"massFakeBs", 50, 5.0, 6.0 );
        createHisto( name+"massFakeBs_withCuts", 50, 5.0, 6.0 );
        createHisto( name+"massFakePhi1020", 80, 0.9, 1.3 );
        createHisto( name+"massFakeK892", 80, 0.7, 1.1 );
        createHisto( name+"massFakePiPi",180, 0.3, 1.2 );
    }
    createHisto( "errFD2D", 100, 0, 0.15 );
    myCutLists.push_back( new      vtxprobCut(0.15,-99. ) );
    myCutLists.push_back( new         massCut(5.0 ,  6.0) );
    myCutLists.push_back( new       cosa2dCut(0.99      ) );
    myCutLists.push_back( new           ptCut(15  ,-99. ) );
}
void histMain_findParDiff::Process( fwlite::Event* ev )
{
    try 
    {
        if ( !ev->isValid() ) return;
        _handle.getByLabel( *ev, _label.module.c_str(), _label.label.c_str(), _label.process.c_str() );

        std::vector<pat::CompositeCandidate>::const_iterator iter = _handle->begin();
        std::vector<pat::CompositeCandidate>::const_iterator iend = _handle->end  ();
        while ( iter != iend )
        {
            const pat::CompositeCandidate& cand = *iter++;
            bool cutTag = false;
            std::vector<myCut::generalCutList*>::const_iterator iter = myCutLists.begin();
            std::vector<myCut::generalCutList*>::const_iterator iend = myCutLists.end  ();
            while ( iter != iend )
            {
                myCut::generalCutList* gCut = *iter++;
                if ( !gCut->accept(cand) )
                { cutTag = true; break; }
            }
            if ( cutTag ) continue;
   

            const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
            if ( _vtx == nullptr ) continue;
            const pat::CompositeCandidate* _jpsi = usefulFuncs::getByRef<pat::CompositeCandidate>( cand, "refToJPsi" );
            if ( _jpsi == nullptr ) continue;
            const reco::Vertex* _pvx = usefulFuncs::getByRef<reco::Vertex>( *_jpsi, "primaryVertex" );
            if ( _pvx == nullptr ) continue;
            double denumerator=   ( _vtx->xError()*_vtx->xError()*_vtx->x()*_vtx->x() + 
                                    _vtx->yError()*_vtx->yError()*_vtx->y()*_vtx->y() + 
                                    _pvx->xError()*_pvx->xError()*_pvx->x()*_pvx->x() +
                                    _pvx->yError()*_pvx->yError()*_pvx->y()*_pvx->y() +
                                    _vtx->xError()*_vtx->yError()*_vtx->x()*_vtx->y() +
                                    _pvx->xError()*_pvx->yError()*_pvx->x()*_pvx->y() ) ;
            double   numerator=   ( (_vtx->x()-_pvx->x())*(_vtx->x()*_pvx->x()) +
                                    (_vtx->y()-_pvx->y())*(_vtx->y()*_pvx->y()) ) ;
            double err = denumerator/numerator;
            double dist= (_vtx->x()-_pvx->x())*(_vtx->x()-_pvx->x())+(_vtx->y()-_pvx->y())*(_vtx->y()-_pvx->y());
                                  
            fillHisto( "errFD2D", sqrt(err) );


            for ( auto& name : _nMap )
            {
                const std::string& dirname = name.second;
                double i = name.first;
                if ( dist < i*i*err ) continue;

                if ( cand.hasUserFloat("fitMass") )
                    fillHisto( dirname+"massLbTk",  cand.userFloat("fitMass") );
                if ( cand.hasUserData("fitMomentum") )
                    fillHisto( dirname+"ptLbTk",  cand.userData<GlobalVector>("fitMomentum")->transverse() );

                if ( !cand.hasUserData("TkTk/Proton.fitMom") || !cand.hasUserData("TkTk/Kaon.fitMom") ) continue;
                if ( !cand.hasUserData("JPsi/MuPos.fitMom") || !cand.hasUserData("JPsi/MuNeg.fitMom") ) continue;

                const GlobalVector* pTkMom = cand.userData<GlobalVector>("TkTk/Proton.fitMom");
                const GlobalVector* nTkMom = cand.userData<GlobalVector>("TkTk/Kaon.fitMom");
                fillHisto( dirname+"ptPTk",  pTkMom->transverse() );
                fillHisto( dirname+"ptPTk",  nTkMom->transverse() );

                fourMom pTk( pTkMom->x(), pTkMom->y(), pTkMom->z() );
                fourMom nTk( nTkMom->x(), nTkMom->y(), nTkMom->z() );

                nTk.setMass( 0.493667 );
                pTk.setMass( 0.9382720813 );

                fourMom twoTk = pTk + nTk;
                fillHisto( dirname+"massTkTk",  twoTk.Mag() );

                const GlobalVector* pmuMom = cand.userData<GlobalVector>("JPsi/MuPos.fitMom");
                const GlobalVector* nmuMom = cand.userData<GlobalVector>("JPsi/MuNeg.fitMom");

                fourMom pmu( pmuMom->x(), pmuMom->y(), pmuMom->z() );
                fourMom nmu( nmuMom->x(), nmuMom->y(), nmuMom->z() );

                pmu.setMass( 0.1056583715 );
                nmu.setMass( 0.1056583715 );

                fourMom fourTk = pTk + nTk + pmu + nmu;
    
                pTk.setMass( 0.493667 );
                nTk.setMass( 0.13957061 );
                fourTk = pTk + nTk + pmu + nmu;
                twoTk = pTk + nTk;

                fillHisto( dirname+"massFakeBd", fourTk.Mag() );
                if ( twoTk.Mag() > 0.850 && twoTk.Mag() < 0.950 )
                    fillHisto( dirname+"massFakeBd_withCuts", fourTk.Mag() );
                if ( fourTk.Mag() > 5.2 && fourTk.Mag() < 5.35 )
                    fillHisto( dirname+"massFakeK892", twoTk.Mag() );
    
                pTk.setMass( 0.493667 );
                nTk.setMass( 0.493667 );
                fourTk = pTk + nTk + pmu + nmu;
                twoTk = pTk + nTk;

                fillHisto( dirname+"massFakeBs", fourTk.Mag() );
                if ( twoTk.Mag() > 1.0 && twoTk.Mag() < 1.05 )
                    fillHisto( dirname+"massFakeBs_withCuts", fourTk.Mag() );
                if ( fourTk.Mag() > 5.3 && fourTk.Mag() < 5.5 )
                    fillHisto( dirname+"massFakePhi1020", twoTk.Mag() );
    
                pTk.setMass( 0.13957061 );
                nTk.setMass( 0.13957061 );
                twoTk = pTk + nTk;
                fillHisto( dirname+"massFakePiPi", twoTk.Mag() );
            } // for _nMap end
        } // while end
    } catch (...) {}
    return;
}

void histMain_findParDiff::specialClearHisto()
{
    std::map< dirName, std::map<std::string, TH1D*> >::iterator iter = _hMap.begin();
    std::map< dirName, std::map<std::string, TH1D*> >::iterator iend = _hMap.end  ();
    while ( iter != iend )
    {
        std::map<std::string, TH1D*>& secMap = iter++->second;
        for ( const auto& secIter : secMap )
            delete secIter.second;
    }
}


void histMain_findParDiff::Clear()
{
    for ( auto& generalCut : myCutLists )
        delete generalCut;
    for ( auto& testcuts : testCuts )
        delete testcuts.second;
}

