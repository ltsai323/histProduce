#ifndef __hMainKshort_h__
#define __hMainKshort_h__
#include "histProduce/histProduce/interface/hMain.h"

class histMain_Kshort : public histMain
{
public:
    histMain_Kshort ( TFileDirectory* d);
    virtual void Process( fwlite::Event* ev ) override;
    virtual void Clear() override;
private:
    //double getFlightDistance( const pat::CompositeCandidate& cand, const reco::Vertex* _bs );
    //double getCosa2d( const pat::CompositeCandidate& cand, const reco::Vertex* _bs );
};




#endif


