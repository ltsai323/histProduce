#include "histProduce/histProduce/interface/hMainLb_findParDiff.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"


histMain_Lb_findParDiff::histMain_Lb_findParDiff( TFileDirectory* d ) :
    histMain( d, histMain::Label("lbWriteSpecificDecay", "LbToTkTkFitted", "bphAnalysis") )
{
    using namespace myCut;

    origDir = d;
    _nMap[fd020 ] = "fd020";
    _nMap[fd025 ] = "fd025";
    _nMap[fd030 ] = "fd030";
    _nMap[fd035 ] = "fd035";
    _nMap[fd040 ] = "fd040";
    testCuts.insert( std::make_pair( fd020, new flightDist2DCut(0.2 ,-99. ) ) );
    testCuts.insert( std::make_pair( fd025, new flightDist2DCut(0.25 ,-99.) ) );
    testCuts.insert( std::make_pair( fd030, new flightDist2DCut(0.30,-99. ) ) );
    testCuts.insert( std::make_pair( fd035, new flightDist2DCut(0.35,-99. ) ) );
    testCuts.insert( std::make_pair( fd040, new flightDist2DCut(0.40,-99. ) ) );
    for ( const auto& _dName : _nMap )
    {
        const std::string& name = _dName.second;
        createHisto( name+"massLbTk", 50, 5.0, 6.0 );
        createHisto( name+"massFakeBd", 50, 5.0, 6.0 );
        createHisto( name+"massFakeBd_withCuts", 50, 5.0, 6.0 );
        createHisto( name+"massFakeBs", 50, 5.0, 6.0 );
        createHisto( name+"massFakeBs_withCuts", 50, 5.0, 6.0 );
        createHisto( name+"massTkTk", 50, 1., 1.5 );
        createHisto( name+"ptLbTk", 60, 8., 20. );
        createHisto( name+"ptPTk", 60, 0., 6. );
        createHisto( name+"massFakePhi1020", 80, 0.9, 1.3 );
        createHisto( name+"massFakeK892", 80, 0.7, 1.1 );
    }
    myCutLists.push_back( new      vtxprobCut(0.15,-99. ) );
    myCutLists.push_back( new         massCut(5.0 ,  6.0) );
    myCutLists.push_back( new       cosa2dCut(0.99      ) );
    myCutLists.push_back( new           ptCut(15  ,-99. ) );
}
void histMain_Lb_findParDiff::Process( fwlite::Event* ev )
{
    try 
    {
        if ( ev->isValid() )
        {
        _handle.getByLabel( *ev, _label.module.c_str(), _label.label.c_str(), _label.process.c_str() );
    
            std::vector<pat::CompositeCandidate>::const_iterator iter = _handle->begin();
            std::vector<pat::CompositeCandidate>::const_iterator iend = _handle->end  ();
            while ( iter != iend )
            {
                const pat::CompositeCandidate& cand = *iter++;
                bool cutTag = false;
                std::vector<myCut::generalCutList*>::const_iterator iter = myCutLists.begin();
                std::vector<myCut::generalCutList*>::const_iterator iend = myCutLists.end  ();
                std::cout << "asdfasdf\n";
                while ( iter != iend )
                    if ( !((*iter++)->accept(cand)) )
                    { cutTag = true; break; }
                if ( cutTag ) continue;
   

for ( auto& name : _nMap )
{
    std::cout << "hii-1\n";
    if ( testCuts.find( name.first ) == testCuts.end() ) continue;
    myCut::generalCutList* spCut = testCuts.find( name.first )->second;
    std::cout << "hii0\n";
    if ( !spCut->accept(cand) ) continue;
    const std::string& dirname = name.second;
    std::cout << "hii\n";

                if ( cand.hasUserFloat("fitMass") )
                    fillHisto( dirname+"massLbTk",  cand.userFloat("fitMass") );
                if ( cand.hasUserData("fitMomentum") )
                    fillHisto( dirname+"ptLbTk",  cand.userData<GlobalVector>("fitMomentum")->transverse() );
    
                if ( cand.hasUserData("TkTk/Proton.fitMom") && cand.hasUserData("TkTk/Kaon.fitMom") )
                    if ( cand.hasUserData("JPsi/MuPos.fitMom") && cand.hasUserData("JPsi/MuNeg.fitMom") )
                    {
                        const GlobalVector* pTkMom = cand.userData<GlobalVector>("TkTk/Proton.fitMom");
                        const GlobalVector* nTkMom = cand.userData<GlobalVector>("TkTk/Kaon.fitMom");
                        const GlobalVector* pmuMom = cand.userData<GlobalVector>("JPsi/MuPos.fitMom");
                        const GlobalVector* nmuMom = cand.userData<GlobalVector>("JPsi/MuNeg.fitMom");
                        fillHisto( dirname+"ptPTk",  pTkMom->transverse() );
                        fillHisto( dirname+"ptPTk",  nTkMom->transverse() );
    
                        fourMom pTk( pTkMom->x(), pTkMom->y(), pTkMom->z() );
                        fourMom nTk( nTkMom->x(), nTkMom->y(), nTkMom->z() );
                        fourMom pmu( pmuMom->x(), pmuMom->y(), pmuMom->z() );
                        fourMom nmu( nmuMom->x(), nmuMom->y(), nmuMom->z() );
    
                        nTk.setMass( 0.493667 );
                        pTk.setMass( 0.9382720813 );
                        nTk.setMass( 0.13957061 );
                        pmu.setMass( 0.1056583715 );
                        nmu.setMass( 0.1056583715 );
    
                        fourMom fourTk = pTk + nTk + pmu + nmu;
                        fourMom twoTk = pTk + nTk;
                        fillHisto( dirname+"massTkTk",  twoTk.Mag() );

                        //pTk.setMass( 0.9382720813 );
                        pTk.setMass( 0.493667 );
                        nTk.setMass( 0.13957061 );
                        fourTk = pTk + nTk + pmu + nmu;
                        twoTk = pTk + nTk;
    
                        fillHisto( dirname+"massFakeBd",  fourTk.Mag() );
                        //fillHisto( "massTkTk", twoTk.Mag() );
                        if ( twoTk.Mag() > 0.870 && twoTk.Mag() < 0.930 )
                            fillHisto( dirname+"massFakeBd_withCuts",  fourTk.Mag() );
                        if ( fourTk.Mag() > 5.279+0.02 && fourTk.Mag() < 5.279-0.02 )
                            fillHisto( dirname+"massFakeK892",  twoTk.Mag() );

                        //pTk.setMass( 0.9382720813 );
                        pTk.setMass( 0.493667 );
                        nTk.setMass( 0.493667 );
                        fourTk = pTk + nTk + pmu + nmu;
                        twoTk = pTk + nTk;
    
                        fillHisto( dirname+"massFakeBs",  fourTk.Mag() );
                        //fillHisto( "massTkTk", twoTk.Mag() );
                        if ( twoTk.Mag() > 1.019460-0.001 && twoTk.Mag() < 1.019460+0.001 )
                            fillHisto( dirname+"massFakeBs_withCuts",  fourTk.Mag() );
                        if ( fourTk.Mag() > 5.279-0.02 && fourTk.Mag() < 5.279+0.02 )
                            fillHisto( dirname+"massFakePhi1020",  twoTk.Mag() );
                    }
}
    
            }
        }
    } catch (...) {}
}

//inline void histMain_Lb_findParDiff::createHisto( const std::string& name, dirName dName, int nbin, double min, double max )
//{
//    if ( _hMap.find( dName ) == _hMap.end() ) return;
//    if ( _hMap[dName].find( name ) != _hMap[dName].end() )
//    {
//        printf ( "histogram recreated! please check the createHisto function\n" );
//        return;
//    }
//    if ( _dirMap.find(dName) == _dirMap.end() ) return;
//    TFileDirectory* _d =  &( _dirMap.find(dName)->second );
//    _hMap[dName][name] = _d->make<TH1D>( name.c_str(), name.c_str(), nbin, min, max );
//    return;
//}
//inline void histMain_Lb_findParDiff::fillHisto( const std::string& name, dirName dName, double value )
//{
//    if ( _hMap.find( dName ) == _hMap.end() ) return;
//    std::map<std::string,TH1D*>::iterator iter = _hMap[dName].find( name );
//    std::map<std::string,TH1D*>::iterator iend = _hMap[dName].end();
//    if ( iter == iend ) return;
//    iter->second->Fill( value );
//    return;
//}

void histMain_Lb_findParDiff::specialClearHisto()
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


void histMain_Lb_findParDiff::Clear()
{
    for ( auto& generalCut : myCutLists )
        delete generalCut;
    for ( auto& testcuts : testCuts )
        delete testcuts.second;
}

