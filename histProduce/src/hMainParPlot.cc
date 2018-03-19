#include "histProduce/histProduce/interface/hMainParPlot.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "math.h"


histMain_ParPlot::histMain_ParPlot( TFileDirectory* d ) :
    histMain( d, histMain::Label("lbWriteSpecificDecay", "LbToTkTkFitted", "bphAnalysis"), "parPlots" )
{
    createHisto( "parQValueLbTk", 200, 0, 20. );
    createHisto( "parQValueFakeBd", 200, 0, 20. );
    createHisto( "parQValueFakeBs", 200, 0, 20. );
    createHisto( "parQValueFakePiPi", 200, 0, 20. );

    createHisto( "parmassFakeBs_twoIP", 50, 5.0, 6.0 );
    createHisto( "parmassFakeBd_twoIP", 50, 5.0, 6.0 );
    createHisto( "paretaLbTk", 50, 5.0, 6.0 );
    createHisto( "parETALbTk", 50, 5.0, 6.0 );
    createHisto( "par_ptk_IPFakeBs", 2000, -1.00, 1.00 );
    createHisto( "par_ntk_IPFakeBs", 2000, -1.00, 1.00 );
    createHisto( "par_ptk_IPFakeBd", 2000, -1.00, 1.00 );
    createHisto( "par_ntk_IPFakeBd", 2000, -1.00, 1.00 );
    createHisto( "par_ALL_IPFakeBs", 2000, -1.00, 1.00 );
    createHisto( "par_ALL_IPFakeBd", 2000, -1.00, 1.00 );
    createHisto( "par_ptk_IP/ErrFakeBs", 400, -10., 10. );
    createHisto( "par_ntk_IP/ErrFakeBs", 400, -10., 10. );
    createHisto( "par_ptk_IP/ErrFakeBd", 400, -10., 10. );
    createHisto( "par_ntk_IP/ErrFakeBd", 400, -10., 10. );
}
void histMain_ParPlot::Process( fwlite::Event* ev )
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
            const std::vector<myCut::generalCutList*>* generalCut = getCutList();
            std::vector<myCut::generalCutList*>::const_iterator iter = generalCut->begin();
            std::vector<myCut::generalCutList*>::const_iterator iend = generalCut->end  ();
            while ( iter != iend )
            {
                myCut::generalCutList* gCut = *iter++;
                if ( !gCut->accept(cand) )
                {
                    cutTag = true;
                    break;
                }
            }
            if ( cutTag ) continue;

            if ( !cand.hasUserFloat("fitMass") ) continue;
            if ( !cand.hasUserData("fitMomentum") ) continue;
            if ( !cand.hasUserFloat("TkTk/Proton.IPt") ) continue;
            if ( !cand.hasUserFloat("TkTk/Kaon.IPt") ) continue;
            if ( !cand.hasUserFloat("TkTk/Proton.IPt.Error") ) continue;
            if ( !cand.hasUserFloat("TkTk/Kaon.IPt.Error") ) continue;

            double IPp ( cand.userFloat( "TkTk/Proton.IPt" ) );
            double IPn ( cand.userFloat( "TkTk/Kaon.IPt" ) );
            double IPpErr ( cand.userFloat( "TkTk/Proton.IPt.Error" ) );
            double IPnErr ( cand.userFloat( "TkTk/Kaon.IPt.Error" ) );

            if ( cand.userData<GlobalVector>("fitMomentum")->eta() < 1.4 )
                fillHisto( "paretaLbTk", cand.userFloat("fitMass") );
            else
                fillHisto( "parETALbTk", cand.userFloat("fitMass") );



            if ( !cand.hasUserData("TkTk/Proton.fitMom") )  continue;
            if ( !cand.hasUserData("TkTk/Kaon.fitMom") )    continue;
            if ( !cand.hasUserData("JPsi/MuPos.fitMom") )   continue;
            if ( !cand.hasUserData("JPsi/MuNeg.fitMom") )   continue;
            fillHisto( "par_ALL_IPFakeBs", IPp );
            fillHisto( "par_ALL_IPFakeBd", IPp );
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

            double protonMass(0.9382720813);
            double   kaonMass(0.493667);
            double   pionMass(0.13957061);
            double   muonMass(0.1056583715);
            nTk.setMass(   kaonMass );
            pTk.setMass( protonMass );
            pmu.setMass(   muonMass );
            nmu.setMass(   muonMass );

            fourMom fourTk = pTk + nTk + pmu + nmu;
            fourMom twoTk = pTk + nTk;
            fourMom jpsi = pmu+nmu;
            fillHisto( "parQValueLbTk", (cand.userFloat("fitMass")-jpsi.Mag()-protonMass-kaonMass) );

            //pTk.setMass( pprotonMass );
            pTk.setMass(   kaonMass );
            nTk.setMass(   pionMass );

            twoTk = pTk + nTk;
            fillHisto( "parQValueFakeBd", fourTk.Mag()-jpsi.Mag()-twoTk.Mag() );
            fillHisto( "massFakeBd", fourTk.Mag() );
            //fillHisto( "massTkTk", twoTk.Mag() );
            if ( twoTk.Mag() > 0.850 && twoTk.Mag() < 0.950 )
            {
                fillHisto( "par_ptk_IPFakeBd", IPp );
                fillHisto( "par_ptk_IP/ErrFakeBd", IPp/IPpErr );
                fillHisto( "par_ntk_IPFakeBd", IPn );
                fillHisto( "par_ntk_IP/ErrFakeBd", IPn/IPnErr );
            }
            if ( IPp  > IPpErr && IPn > IPnErr )
                fillHisto( "parmassFakeBd_twoIP", fourTk.Mag() );

            //pTk.setMass( protonMass );
            pTk.setMass(   kaonMass );
            nTk.setMass(   kaonMass );
            fourTk = pTk + nTk + pmu + nmu;
            twoTk = pTk + nTk;

            fillHisto( "parQValueFakeBs", fourTk.Mag()-jpsi.Mag()-twoTk.Mag() );
            if ( twoTk.Mag() > 1.0 && twoTk.Mag() < 1.05 )
            {
                fillHisto( "par_ptk_IPFakeBs", IPp );
                fillHisto( "par_ptk_IP/ErrFakeBs", IPp/IPpErr );
                fillHisto( "par_ntk_IPFakeBs", IPn );
                fillHisto( "par_ntk_IP/ErrFakeBs", IPn/IPnErr );
            }
            if ( IPp  > IPpErr && IPn > IPnErr )
                fillHisto( "parmassFakeBs_twoIP", fourTk.Mag() );

            pTk.setMass( 0.13957061 );
            nTk.setMass( 0.13957061 );
            twoTk = pTk + nTk;
            fillHisto( "parQValueFakePiPi", twoTk.Mag()-pionMass-pionMass );
        }

    }
    catch (...) {}
    return;
}

void histMain_ParPlot::Clear()
{
}
