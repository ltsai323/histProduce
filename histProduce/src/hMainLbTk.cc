#include "histProduce/histProduce/interface/hMainLbTk.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"


histMain_LbTk::histMain_LbTk( TFileDirectory* d ) :
    histMain( d, histMain::Label("lbWriteSpecificDecay", "LbToTkTkFitted", "bphAnalysis") )
{
    createHisto( "massLbTk", 50, 5.0, 6.0 );
    createHisto( "massFakeBs", 50, 5.0, 6.0 );
    createHisto( "ptLbTk",  60, 8., 20. );
    createHisto( "ptPTk",  60, 0., 6. );
}
void histMain_LbTk::Process( fwlite::Event* ev )
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
            if ( (*iter++)->accept( cand ) )
            { cutTag = true; break; }
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
                pTk.setMass( 0.493667 );
                nTk.setMass( 0.493667 );
                pmu.setMass( 0.1056583715 );
                nmu.setMass( 0.1056583715 );

                fourMom bs = pTk + nTk + pmu + nmu;

                fillHisto( "massFakeBs", bs.Mag() );
            }


    }
}

