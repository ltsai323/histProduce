#include "histProduce/histProduce/interface/hMainLbL0.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"


histMain_LbL0::histMain_LbL0( TFileDirectory* d ) :
    histMain( d, histMain::Label("lbWriteSpecificDecay", "LbToLam0Fitted", "bphAnalysis"), "LbL0" )
{
    createHisto( "numOfEventInHandle", 15, 0., 15. );
    createHisto( "massLb", 200, 5.0, 6.0 );
    createHisto( "massLam0",  50, 1.10, 1.15 );
    createHisto( "massLb_withCuts", 100, 5.0, 6.0 );
    createHisto( "massLam0_withCuts",  50, 1.10, 1.15 );

}
void histMain_LbL0::Process( fwlite::Event* ev )
{
    try
    {
        _handle.getByLabel( *ev, _label.module.c_str(), _label.label.c_str(), _label.process.c_str() );
        fillHisto( "numOfEventInHandle", _handle->size() );

        std::vector<pat::CompositeCandidate>::const_iterator iter = _handle->begin();
        std::vector<pat::CompositeCandidate>::const_iterator iend = _handle->end  ();
        while ( iter != iend )
        {
            const pat::CompositeCandidate& cand = *iter++;
            double lbMass = 0;
            if ( cand.hasUserFloat("fitMass") )
            {
                lbMass =  cand.userFloat( "fitMass" );
                fillHisto( "massLb", lbMass );
            }
            bool cutTag = false;
            const std::vector<myCut::generalCutList*>* generalCut = getCutList();
            std::vector<myCut::generalCutList*>::const_iterator iter = generalCut->begin();
            std::vector<myCut::generalCutList*>::const_iterator iend = generalCut->end  ();
            while ( iter != iend )
                if ( !( (*iter++)->accept(cand) ) )
                { cutTag = true; break; }
            if ( cutTag ) continue;
//
            //if ( cand.hasUserFloat("fitMass") ) continue;
            const pat::CompositeCandidate* lam0Cand = usefulFuncs::getByRef<pat::CompositeCandidate>( cand, "refToLam0" );
            const pat::CompositeCandidate* jpsiCand = usefulFuncs::getByRef<pat::CompositeCandidate>( cand, "refToJPsi" );
            if ( !jpsiCand )
            {
                cutTag = true;
            }
            if ( lam0Cand == nullptr )
            {
                cutTag = true;
            }
            if ( cutTag ) continue;
            const reco::Vertex* pv = usefulFuncs::getByRef<reco::Vertex>( *jpsiCand, "primaryVertex" );
            if ( !pv )
            {
                cutTag = true;
            }
            if ( cutTag ) continue;
            if ( !lam0Cand->hasUserFloat( "fitMass" ) ) continue;
            double lam0Mass( lam0Cand->userFloat( "fitMass" ) );


            fillHisto("massLam0", lam0Mass );
            if ( lam0Mass > 1.110 && lam0Mass < 1.125 )
                fillHisto( "massLb_withCuts", lbMass );
            if ( lbMass > 5.58 && lbMass < 5.63 )
                fillHisto( "massLam0_withCuts", lam0Mass );


        }
    } catch ( ... ) {}
}

void histMain_LbL0::Clear()
{
}
