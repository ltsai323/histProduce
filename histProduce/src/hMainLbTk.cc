#include "histProduce/histProduce/interface/hMainLbTk.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "math.h"


histMain_LbTk::histMain_LbTk( TFileDirectory* d ) :
    histMain( d, histMain::Label("lbWriteSpecificDecay", "LbToTkTkFitted", "bphAnalysis") )
{
    createHisto( "massLbTk", 500, 5.0, 6.0 );
    createHisto( "massLbTk_withCuts", 500, 5.0, 6.0 );
    createHisto( "massFakeBd", 500, 5.0, 6.0 );
    createHisto( "massFakeBd_withCuts", 500, 5.0, 6.0 );
    createHisto( "massFakeBs", 500, 5.0, 6.0 );
    createHisto( "massFakeBs_withCuts", 500, 5.0, 6.0 );
    createHisto( "massTkTk", 65, 1.35, 2.0 );
    createHisto( "ptLbTk",  600, 8., 20. );
    createHisto( "ptPTk",  200, 0., 6. );
    createHisto( "ptNTk",  200, 0., 6. );
    createHisto( "massFakePhi1020", 800, 0.9, 1.3 );
    createHisto( "massFakeK892", 800, 0.7, 1.1 );
    createHisto( "massFakePiPi",1800, 0.3, 1.2 );
    createHisto( "parIPtransverse", 2000, 0., 0.4 );
    createHisto( "eventsInEvent", 100, 0., 100. );
}
void histMain_LbTk::Process( fwlite::Event* ev )
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
            if ( !cand.hasUserFloat("TkTk/Proton.IPt") ) continue;
            if ( !cand.hasUserFloat("TkTk/Kaon.IPt") ) continue;
            if ( !cand.hasUserFloat("TkTk/Proton.IPt.Error") ) continue;
            if ( !cand.hasUserFloat("TkTk/Kaon.IPt.Error") ) continue;
            if ( !cand.hasUserFloat("fitMass") ) continue;
            vtxprobChooser.insert( std::make_pair( TMath::Prob( _vtx->chi2(), _vtx->ndof() ), &cand ) );
        }
        fillHisto("eventsInEvent", vtxprobChooser.size() );

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
            int wrongParticleConfirmed = 0;
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
                    fillHisto( "ptNTk", nTkMom->transverse() );
    
                    fourMom pTk( pTkMom->x(), pTkMom->y(), pTkMom->z() );
                    fourMom nTk( nTkMom->x(), nTkMom->y(), nTkMom->z() );
                    fourMom pmu( pmuMom->x(), pmuMom->y(), pmuMom->z() );
                    fourMom nmu( nmuMom->x(), nmuMom->y(), nmuMom->z() );
    
                    nTk.setMass( 0.493667 );
                    pTk.setMass( 0.9382720813 );
                    pmu.setMass( 0.1056583715 );
                    nmu.setMass( 0.1056583715 );
    
                    fourMom fourTk = pTk + nTk + pmu + nmu;
                    fourMom twoTk = pTk + nTk;
                    fillHisto( "massTkTk", twoTk.Mag() );

                    pTk.setMass( 0.493667 );
                    nTk.setMass( 0.13957061 );
                    fourTk = pTk + nTk + pmu + nmu;
                    twoTk = pTk + nTk;
    
                    fillHisto( "massFakeBd", fourTk.Mag() );
                    if ( twoTk.Mag() > 0.850 && twoTk.Mag() < 0.950 )
                        fillHisto( "massFakeBd_withCuts", fourTk.Mag() );
                    if ( fourTk.Mag() > 5.2 && fourTk.Mag() < 5.35 )
                        fillHisto( "massFakeK892", twoTk.Mag() );

                    // consider this event form Bd, and two track form K(892)
                    if ( twoTk.Mag() > 0.850 && twoTk.Mag() < 0.950 && fourTk.Mag() > 5.2 && fourTk.Mag() < 5.35 )
                        wrongParticleConfirmed += 1 << 0;

                    //pTk.setMass( 0.9382720813 );
                    pTk.setMass( 0.493667 );
                    nTk.setMass( 0.493667 );
                    fourTk = pTk + nTk + pmu + nmu;
                    twoTk = pTk + nTk;
    
                    fillHisto( "massFakeBs", fourTk.Mag() );
                    if ( twoTk.Mag() > 1.0 && twoTk.Mag() < 1.05 )
                        fillHisto( "massFakeBs_withCuts", fourTk.Mag() );
                    if ( fourTk.Mag() > 5.3 && fourTk.Mag() < 5.5 )
                        fillHisto( "massFakePhi1020", twoTk.Mag() );
                    // consider this event form Bd, and two track form phi(1020)
                    if ( twoTk.Mag() > 1.0 && twoTk.Mag() < 1.05 && fourTk.Mag() > 5.3 && fourTk.Mag() < 5.5 )
                        wrongParticleConfirmed += 1 << 1;

                    pTk.setMass( 0.13957061 );
                    nTk.setMass( 0.13957061 );
                    twoTk = pTk + nTk;
                    fillHisto( "massFakePiPi", twoTk.Mag() );

                    // if cands are not Bs or Bd
                    if ( wrongParticleConfirmed == 0 )
                        fillHisto( "massLbTk_withCuts", cand.userFloat("fitMass") );
                } // double if ended

        }
    } catch (...) {}
}

void histMain_LbTk::Clear()
{
}
