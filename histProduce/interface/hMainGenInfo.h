#ifndef __hMainGenInfo_h__
#define __hMainGenInfo_h__
#include "histProduce/histProduce/interface/hMain.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

class histMain_GenInformation : public histMain
{
public:
    histMain_GenInformation ( TFileDirectory* d);
    virtual void Process( fwlite::Event* ev ) override;
    virtual void Clear() override;
private:
    fwlite::Handle< std::vector<reco::GenParticle> > genHandle;
    fwlite::Handle< reco::BeamSpot > beamSpotHandle;
};




#endif


