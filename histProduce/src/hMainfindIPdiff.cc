#include "histProduce/histProduce/interface/hMainfindIPdiff.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include <math.h>

histMain_findIPdiff::histMain_findIPdiff( TFileDirectory* d ) :
    histMain( d, histMain::Label("lbWriteSpecificDecay", "LbToTkTkFitted", "bphAnalysis") )
{
    using namespace myCut;

    origDir = d;
    _nMap[IPt0S] = "IPt0S";
    _nMap[IPt1S] = "IPt1S";
    _nMap[IPt2S] = "IPt2S";
    _nMap[IPt3S] = "IPt3S";
    _nMap[IPt4S] = "IPt4S";
    _nMap[IPt5S] = "IPt5S";
    for ( const auto& _dName : _nMap )
    {
        const std::string& name = _dName.second;
        createHisto( name+"_ptk_"+"massLbTk", 50, 5.0, 6.0 );
        createHisto( name+"_ptk_"+"massTkTk",120, 1.3, 2.5 );
        createHisto( name+"_ptk_"+"massFakeBd", 50, 5.0, 6.0 );
        createHisto( name+"_ptk_"+"massFakeBd_withCuts", 50, 5.0, 6.0 );
        createHisto( name+"_ptk_"+"massFakeBs", 50, 5.0, 6.0 );
        createHisto( name+"_ptk_"+"massFakeBs_withCuts", 50, 5.0, 6.0 );
        createHisto( name+"_ptk_"+"massFakePhi1020", 80, 0.9, 1.3 );
        createHisto( name+"_ptk_"+"massFakeK892", 80, 0.7, 1.1 );
        createHisto( name+"_ptk_"+"massFakePiPi",180, 0.3, 1.2 );

        createHisto( name+"_ntk_"+"massLbTk", 50, 5.0, 6.0 );
        createHisto( name+"_ntk_"+"massTkTk",120, 1.3, 2.5 );
        createHisto( name+"_ntk_"+"massFakeBd", 50, 5.0, 6.0 );
        createHisto( name+"_ntk_"+"massFakeBd_withCuts", 50, 5.0, 6.0 );
        createHisto( name+"_ntk_"+"massFakeBs", 50, 5.0, 6.0 );
        createHisto( name+"_ntk_"+"massFakeBs_withCuts", 50, 5.0, 6.0 );
        createHisto( name+"_ntk_"+"massFakePhi1020", 80, 0.9, 1.3 );
        createHisto( name+"_ntk_"+"massFakeK892", 80, 0.7, 1.1 );
        createHisto( name+"_ntk_"+"massFakePiPi",180, 0.3, 1.2 );

        createHisto( name+"_both_"+"massLbTk", 50, 5.0, 6.0 );
        createHisto( name+"_both_"+"massTkTk",120, 1.3, 2.5 );
        createHisto( name+"_both_"+"massFakeBd", 50, 5.0, 6.0 );
        createHisto( name+"_both_"+"massFakeBd_withCuts", 50, 5.0, 6.0 );
        createHisto( name+"_both_"+"massFakeBs", 50, 5.0, 6.0 );
        createHisto( name+"_both_"+"massFakeBs_withCuts", 50, 5.0, 6.0 );
        createHisto( name+"_both_"+"massFakePhi1020", 80, 0.9, 1.3 );
        createHisto( name+"_both_"+"massFakeK892", 80, 0.7, 1.1 );
        createHisto( name+"_both_"+"massFakePiPi",180, 0.3, 1.2 );
    }
    //createHisto( "errFD2D", 100, 0, 0.15 );
    //createHisto( "parIPt_BsSideBand", 200, 0, 0.4 );
    //createHisto( "parIPt_BsSignal", 200, 0, 0.4 );
    //createHisto( "parIPt_BdSideBand", 200, 0, 0.4 );
    //createHisto( "parIPt_BdSignal", 200, 0, 0.4 );
    //createHisto( "parIPt_ptk", 200, 0, 0.4 );
    //createHisto( "parIPt_ntk", 200, 0, 0.4 );
    createHisto( "par_ptk_IPt/Err", 200, 0, 5. );
    createHisto( "par_ntk_IPt/Err", 200, 0, 5. );
    
    myCutLists.push_back( new      vtxprobCut(0.15,-99. ) );
    myCutLists.push_back( new         massCut(5.0 ,  6.0) );
    myCutLists.push_back( new       cosa2dCut(0.99      ) );
    myCutLists.push_back( new           ptCut(15  ,-99. ) );
}
void histMain_findIPdiff::Process( fwlite::Event* ev )
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
   

            if ( !cand.hasUserFloat("TkTk/Proton.IPt") ) continue;
            if ( !cand.hasUserFloat("TkTk/Kaon.IPt") ) continue;
            if ( !cand.hasUserFloat("TkTk/Proton.IPt.Error") ) continue;
            if ( !cand.hasUserFloat("TkTk/Kaon.IPt.Error") ) continue;
            double pIP( cand.userFloat("TkTk/Proton.IPt") );
            double nIP( cand.userFloat("TkTk/Kaon.IPt") );
            double pIPE( cand.userFloat("TkTk/Proton.IPt.Error") );
            double nIPE( cand.userFloat("TkTk/Kaon.IPt.Error") );
            fillHisto( "par_ptk_IPt/Err", pIP/pIPE );
            fillHisto( "par_ntk_IPt/Err", nIP/nIPE );





            for ( auto& name : _nMap )
            {
                const std::string& dirname = name.second;
                double i = name.first;
                bool pTag = false;
                bool nTag = false;
                bool twoTag = false;
                if ( fabs(pIP) > i*pIPE ) pTag = true;
                if ( fabs(nIP) > i*nIPE ) nTag = true;
                if ( fabs(pIP) > i*pIPE && fabs(nIP) > i*nIPE ) twoTag = true;
                if ( !pTag && !nTag ) continue;
                if ( !cand.hasUserFloat("fitMass") ) continue;
                if ( pTag )
                    fillHisto( dirname+"_ptk_"+"massLbTk", cand.userFloat("fitMass") );
                if ( nTag )
                    fillHisto( dirname+"_ntk_"+"massLbTk", cand.userFloat("fitMass") );
                if ( twoTag )
                    fillHisto( dirname+"_both_"+"massLbTk", cand.userFloat("fitMass") );


                if ( !cand.hasUserData("TkTk/Proton.fitMom") || !cand.hasUserData("TkTk/Kaon.fitMom") ) continue;
                if ( !cand.hasUserData("JPsi/MuPos.fitMom") || !cand.hasUserData("JPsi/MuNeg.fitMom") ) continue;

                const GlobalVector* pTkMom = cand.userData<GlobalVector>("TkTk/Proton.fitMom");
                const GlobalVector* nTkMom = cand.userData<GlobalVector>("TkTk/Kaon.fitMom");

                fourMom pTk( pTkMom->x(), pTkMom->y(), pTkMom->z() );
                fourMom nTk( nTkMom->x(), nTkMom->y(), nTkMom->z() );

                nTk.setMass( 0.493667 );
                pTk.setMass( 0.9382720813 );

                fourMom twoTk = pTk + nTk;
                if ( pTag )
                    fillHisto( dirname+"_ptk_"+"massTkTk", twoTk.Mag() );
                if ( nTag )
                    fillHisto( dirname+"_ntk_"+"massTkTk", twoTk.Mag() );
                if ( twoTag )
                    fillHisto( dirname+"_both_"+"massTkTk", twoTk.Mag() );

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

                if ( pTag )
                    fillHisto( dirname+"_ptk_"+"massFakeBd", fourTk.Mag() );
                if ( nTag )
                    fillHisto( dirname+"_ntk_"+"massFakeBd", fourTk.Mag() );
                if ( twoTag )
                    fillHisto( dirname+"_both_"+"massFakeBd", fourTk.Mag() );
                if ( twoTk.Mag() > 0.850 && twoTk.Mag() < 0.950 )
                {
                    if ( pTag )
                        fillHisto( dirname+"_ptk_"+"massFakeBd_withCuts", fourTk.Mag() );
                    if ( nTag )
                        fillHisto( dirname+"_ntk_"+"massFakeBd_withCuts", fourTk.Mag() );
                    if ( twoTag )
                        fillHisto( dirname+"_both_"+"massFakeBd_withCuts", fourTk.Mag() );
                }
                if ( fourTk.Mag() > 5.2 && fourTk.Mag() < 5.35 )
                {
                    if ( pTag )
                        fillHisto( dirname+"_ptk_"+"massFakeK892", twoTk.Mag() );
                    if ( nTag )
                        fillHisto( dirname+"_ntk_"+"massFakeK892", twoTk.Mag() );
                    if ( twoTag )
                        fillHisto( dirname+"_both_"+"massFakeK892", twoTk.Mag() );
                }
    
                pTk.setMass( 0.493667 );
                nTk.setMass( 0.493667 );
                fourTk = pTk + nTk + pmu + nmu;
                twoTk = pTk + nTk;

                if ( pTag )
                    fillHisto( dirname+"_ptk_"+"massFakeBs", fourTk.Mag() );
                if ( nTag )
                    fillHisto( dirname+"_ntk_"+"massFakeBs", fourTk.Mag() );
                if ( twoTag )
                    fillHisto( dirname+"_both_"+"massFakeBs", fourTk.Mag() );
                if ( twoTk.Mag() > 1.0 && twoTk.Mag() < 1.05 )
                {
                    if ( pTag )
                        fillHisto( dirname+"_ptk_"+"massFakeBs_withCuts", fourTk.Mag() );
                    if ( nTag )
                        fillHisto( dirname+"_ntk_"+"massFakeBs_withCuts", fourTk.Mag() );
                    if ( twoTag )
                        fillHisto( dirname+"_both_"+"massFakeBs_withCuts", fourTk.Mag() );
                }
                if ( fourTk.Mag() > 5.3 && fourTk.Mag() < 5.5 )
                {
                    if ( pTag )
                        fillHisto( dirname+"_ptk_"+"massFakePhi1020", twoTk.Mag() );
                    if ( nTag )
                        fillHisto( dirname+"_ntk_"+"massFakePhi1020", twoTk.Mag() );
                    if ( twoTag )
                        fillHisto( dirname+"_both_"+"massFakePhi1020", twoTk.Mag() );
                }
    
                pTk.setMass( 0.13957061 );
                nTk.setMass( 0.13957061 );
                twoTk = pTk + nTk;
                if ( pTag )
                    fillHisto( dirname+"_ptk_"+"massFakePiPi", twoTk.Mag() );
                if ( nTag )
                    fillHisto( dirname+"_ntk_"+"massFakePiPi", twoTk.Mag() );
                if ( twoTag )
                    fillHisto( dirname+"_both_"+"massFakePiPi", twoTk.Mag() );
            } // for _nMap end
        } // while end
    } catch (...) {}
    return;
}

void histMain_findIPdiff::specialClearHisto()
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


void histMain_findIPdiff::Clear()
{
    for ( auto& generalCut : myCutLists )
        delete generalCut;
    for ( auto& testcuts : testCuts )
        delete testcuts.second;
}

