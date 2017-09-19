#include "histProduce/histProduce/interface/hMainLbTk.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"


histMain_LbTk::histMain_LbTk( TFileDirectory* d ) :
    histMain( d, histMain::Label("lbWriteSpecificDecay", "LbToTkTkFitted", "bphAnalysis") )
{
    createHisto( "massLbTk", 50, 5.0, 6.0 );
    createHisto( "massFakeBd", 50, 5.0, 6.0 );
    createHisto( "massFakeBd_withCuts", 50, 5.0, 6.0 );
    createHisto( "massFakeBs", 50, 5.0, 6.0 );
    createHisto( "massFakeBs_withCuts", 50, 5.0, 6.0 );
    createHisto( "massTkTk", 50, 1., 1.5 );
    createHisto( "ptLbTk",  60, 8., 20. );
    createHisto( "ptPTk",  60, 0., 6. );
    createHisto( "massFakePhi1020", 80, 0.9, 1.3 );
    createHisto( "massFakeK892", 80, 0.7, 1.1 );
    createHisto( "massFakePiPi",180, 0.3, 1.2 );
}
void histMain_LbTk::Process( fwlite::Event* ev )
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
            const std::vector<myCut::generalCutList*>* generalCut = getCutList();
            std::vector<myCut::generalCutList*>::const_iterator iter = generalCut->begin();
            std::vector<myCut::generalCutList*>::const_iterator iend = generalCut->end  ();
            while ( iter != iend )
            {
                myCut::generalCutList* gCut = *iter++;
                if ( !gCut->accept(cand) )
                { cutTag = true; break; }
            }
            if ( cutTag ) continue;

            if ( cand.hasUserFloat("fitMass") )
                fillHisto( "massLbTk", cand.userFloat("fitMass") );
            if ( cand.hasUserData("fitMomentum") )
                fillHisto( "ptLbTk", cand.userData<GlobalVector>("fitMomentum")->transverse() );
    
            if ( cand.hasUserData("TkTk/Proton.fitMom") && cand.hasUserData("TkTk/Kaon.fitMom") )
                if ( cand.hasUserData("JPsi/MuPos.fitMom") && cand.hasUserData("JPsi/MuNeg.fitMom") )
                {
                    const GlobalVector* pTkMom = cand.userData<GlobalVector>("TkTk/Proton.fitMom");
                    const GlobalVector* nTkMom = cand.userData<GlobalVector>("TkTk/Kaon.fitMom");
                    const GlobalVector* pmuMom = cand.userData<GlobalVector>("JPsi/MuPos.fitMom");
                    const GlobalVector* nmuMom = cand.userData<GlobalVector>("JPsi/MuNeg.fitMom");
                    fillHisto( "ptPTk", pTkMom->transverse() );
                    fillHisto( "ptPTk", nTkMom->transverse() );
    
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
                    fillHisto( "massTkTk", twoTk.Mag() );

                    //pTk.setMass( 0.9382720813 );
                    pTk.setMass( 0.493667 );
                    nTk.setMass( 0.13957061 );
                    fourTk = pTk + nTk + pmu + nmu;
                    twoTk = pTk + nTk;
    
                    fillHisto( "massFakeBd", fourTk.Mag() );
                    //fillHisto( "massTkTk", twoTk.Mag() );
                    if ( twoTk.Mag() > 0.850 && twoTk.Mag() < 0.950 )
                        fillHisto( "massFakeBd_withCuts", fourTk.Mag() );
                    if ( fourTk.Mag() > 5.2 && fourTk.Mag() < 5.35 )
                        fillHisto( "massFakeK892", twoTk.Mag() );

                    //pTk.setMass( 0.9382720813 );
                    pTk.setMass( 0.493667 );
                    nTk.setMass( 0.493667 );
                    fourTk = pTk + nTk + pmu + nmu;
                    twoTk = pTk + nTk;
    
                    fillHisto( "massFakeBs", fourTk.Mag() );
                    //fillHisto( "massTkTk", twoTk.Mag() );
                    if ( twoTk.Mag() > 1.0 && twoTk.Mag() < 1.05 )
                        fillHisto( "massFakeBs_withCuts", fourTk.Mag() );
                    if ( fourTk.Mag() > 5.3 && fourTk.Mag() < 5.5 )
                        fillHisto( "massFakePhi1020", twoTk.Mag() );

                    pTk.setMass( 0.13957061 );
                    nTk.setMass( 0.13957061 );
                    twoTk = pTk + nTk;
                    fillHisto( "massFakePiPi", twoTk.Mag() );
                }
    
        }
        }
    } catch (...) {}
}

void histMain_LbTk::Clear()
{
}
