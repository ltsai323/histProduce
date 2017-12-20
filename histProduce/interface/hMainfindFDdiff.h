#ifndef __hMainfindFlightDistanceDiff_h__
#define __hMainfindFlightDistanceDiff_h__
#include "histProduce/histProduce/interface/hMain.h"

class histMain_findFlightDistanceDiff : public histMain
{
public:
    histMain_findFlightDistanceDiff ( TFileDirectory* d);
    virtual void Process( fwlite::Event* ev ) override;
    virtual void Clear() override;
    enum dirName { fd020=1, fd040, fd060, fd080, fd100, fd120, fd140, fd160, fd180 };
    std::vector<myCut::generalCutList*> myCutLists;
    std::map< dirName, std::string > _nMap;
    //inline double getFlightDistance( const pat::CompositeCandidate& cand, const reco::Vertex* _bs );
    //inline double getCosa2d( const pat::CompositeCandidate& cand, const reco::Vertex* _bs );
};
#endif


