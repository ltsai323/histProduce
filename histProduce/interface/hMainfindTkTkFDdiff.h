#ifndef __hMainfindTkTkFlightDistanceDiff_h__
#define __hMainfindTkTkFlightDistanceDiff_h__
#include "histProduce/histProduce/interface/hMain.h"

class histMain_findTkTkFlightDistanceDiff : public histMain
{
public:
    histMain_findTkTkFlightDistanceDiff ( TFileDirectory* d);
    virtual void Process( fwlite::Event* ev ) override;
    virtual void Clear() override;
    //enum dirName { fd05 = 1, fd10, fd15, fd20, fd25 };
    enum dirName { fd030 = 1, fd060, fd090, fd120, fd150, fd180, fd210, fd240 };
    std::vector<myCut::generalCutList*> myCutLists;
    std::map< dirName, std::string > _nMap;
private:
    double getFlightDistance( const pat::CompositeCandidate& cand, const reco::Vertex* _bs );
    double getCosa2d( const pat::CompositeCandidate& cand, const reco::Vertex* _bs );
};
#endif


