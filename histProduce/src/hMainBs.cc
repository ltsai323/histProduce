#include "histProduce/histProduce/interface/hMainBs.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"


histMain_Bs::histMain_Bs( TFileDirectory* d ) :
    histMain( d, histMain::Label("lbWriteSpecificDecay", "bsFitted", "bphAnalysis") )
{
    createHisto( "massBs", 50, 5.0, 6.0 );
    createHisto( "ptBs",  20, 0., 20. );
    createHisto( "ptKaon",  20, 0., 20. );
}
void histMain_Bs::Process( fwlite::Event* ev )
{
    _handle.getByLabel( *ev, _label.module.c_str(), _label.label.c_str(), _label.process.c_str() );
    int i;
    int n = _handle->size();
    for ( i = 0; i< n; ++i )
    {

    //std::vector<pat::CompositeCandidate>::const_iterator iter = _handle->begin();
    //std::vector<pat::CompositeCandidate>::const_iterator iend = _handle->end  ();
    //while ( iter != iend )
    //{
        //const pat::CompositeCandidate& cand = *iter++;
        const pat::CompositeCandidate& cand = _handle->at( i );
        bool cutTag = false;
        const std::vector<myCut::generalCutList*>* generalCut = getCutList();
        std::vector<myCut::generalCutList*>::const_iterator iter = generalCut->begin();
        std::vector<myCut::generalCutList*>::const_iterator iend = generalCut->end  ();
        while ( iter != iend )
            if ( (*iter++)->accept( cand ) )
            { cutTag = true; break; }
        if ( cutTag ) continue;

        if ( cand.hasUserFloat("fitMass") )
            fillHisto( "massBs", cand.userFloat("fitMass") );
        if ( cand.hasUserData("fitMomentum") )
            fillHisto( "ptBs", cand.userData<GlobalVector>("fitMomentum")->transverse() );

        if ( cand.hasUserData("Phi/KPos.fitMom") )
            fillHisto( "ptKaon", cand.userData<GlobalVector>("Phi/KPos.fitMom")->transverse() );
    }
}

