#ifndef __hMainfindFlightDistanceDiff_h__
#define __hMainfindFlightDistanceDiff_h__
#include "histProduce/histProduce/interface/hMain.h"

class histMain_findFlightDistanceDiff : public histMain
{
public:
    histMain_findFlightDistanceDiff ( TFileDirectory* d);
    virtual void Process( fwlite::Event* ev ) override;
    virtual void Clear() override;
    enum dirName { fd05 = 1, fd10, fd15, fd20, fd25 };
    std::vector<myCut::generalCutList*> myCutLists;
    std::map< dirName, std::string > _nMap;
};
#endif


