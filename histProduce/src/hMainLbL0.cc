#include "histProduce/histProduce/interface/hMainLbL0.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"


histMain_LbL0::histMain_LbL0( TFileDirectory* d ) :
    histMain( d, histMain::Label("lbWriteSpecificDecay", "LbToLam0Fitted", "bphAnalysis") )
{
    setRefitName( "JPsi/MuPos" );
    setRefitName( "JPsi/MuNeg" );
    setRefitName( "Lam0/Proton" );
    setRefitName( "Lam0/Pion" );

    createHisto( "massLb", 50, 5.0, 6.0 );
    createHisto( "ptProton",  20, 0., 20. );

}
void histMain_LbL0::Process( fwlite::Event* ev )
{
    try 
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
                if ( !( (*iter++)->accept(cand) ) )
                { cutTag = true; break; }
            if ( cutTag ) continue;
    
            if ( cand.hasUserFloat("fitMass") )
                fillHisto( "massLb", cand.userFloat("fitMass") );
            if ( cand.hasUserData("Lam0/Proton.fitMom") )
                fillHisto( "ptProton", cand.userData<GlobalVector>("Lam0/Proton.fitMom")->transverse() );
        }
    } catch ( ... ) {}
}

void histMain_LbL0::Clear()
{
}

