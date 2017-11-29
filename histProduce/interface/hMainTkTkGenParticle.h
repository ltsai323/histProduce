#ifndef __hMainTkTkGenParticle_h__
#define __hMainTkTkGenParticle_h__
#include "histProduce/histProduce/interface/hMain.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include <vector>
#include <utility> // std::pair

class histMain_TkTkGenParticle : public histMain
{
public:
    histMain_TkTkGenParticle ( TFileDirectory* d);
    virtual void Process( fwlite::Event* ev ) override;
    virtual void Clear() override;
private:
    fwlite::Handle< std::vector<reco::GenParticle> > genHandle;
    fwlite::Handle< reco::BeamSpot > beamSpotHandle;
    // store the result of 'particle-MC genParticle pair'
    //std::vector< std::pair<const void*, const reco::GenParticle*> > pgPair;
    const double protonMass;
    const double   pionMass;

template< typename myParticle >
    reco::GenParticle searchForGenParticle( const myParticle* cand, double minDeltaR = 0.05 )
    {
        reco::GenParticle gParticle;

        if ( !genHandle->size() ) return gParticle;
        std::vector<reco::GenParticle>::const_iterator iter = genHandle->begin();
        std::vector<reco::GenParticle>::const_iterator iend = genHandle->end  ();

        double minDeltaRVal = 999.;
        while ( iter != iend )
        {
            const reco::GenParticle& _genParticle = *iter++;
            double tmpDeltaRVal = usefulFuncs::mcMatchVal( cand, &_genParticle );
            if ( tmpDeltaRVal < minDeltaRVal )
            {
                minDeltaRVal = tmpDeltaRVal;
                gParticle = _genParticle;
            }
        }
        //int matchSample = 0;
        //while ( iter != iend )
        //{
        //    const reco::GenParticle& _genParticle = *iter++;
        //    if ( usefulFuncs::mcMatch( cand, &_genParticle, minDeltaR ) )
        //    {
        //        ++matchSample;
        //        gParticle = _genParticle;
        //    }
        //}
        //if ( matchSample > 1 ) printf ( "error!!!! there are more than one candidate\n");
        //if ( matchSample ==0 ) printf ( "error!!!! nothing found\n\n");
            
        return gParticle;
    }
};


#endif


