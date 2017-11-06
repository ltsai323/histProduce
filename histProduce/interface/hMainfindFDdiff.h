#ifndef __hMainfindFlightDistanceDiff_h__
#define __hMainfindFlightDistanceDiff_h__
#include "histProduce/histProduce/interface/hMain.h"

class histMain_findFlightDistanceDiff : public histMain
{
public:
    histMain_findFlightDistanceDiff ( TFileDirectory* d);
    virtual void Process( fwlite::Event* ev ) override;
    virtual void Clear() override;
    enum dirName { fd10 = 1, fd20, fd30, fd40, fd50, fd60, fd70, fd80, fd90 };
    std::vector<myCut::generalCutList*> myCutLists;
    std::map< dirName, std::string > _nMap;
    double getFlightDistance( const pat::CompositeCandidate& cand, const reco::Vertex* _bs );
    double getCosa2d( const pat::CompositeCandidate& cand, const reco::Vertex* _bs );
};
#endif


