#ifndef __hMainfindIPdiff_h__
#define __hMainfindIPdiff_h__
#include "histProduce/histProduce/interface/hMain.h"

class histMain_findIPdiff : public histMain
{
public:
    histMain_findIPdiff ( TFileDirectory* d);
    virtual void Process( fwlite::Event* ev );
    virtual void Clear();
private:
    //enum dirName { fd020, fd025, fd030, fd035, fd040 };
    //enum dirName { fd1S = 1, fd2S, fd3S, fd4S, fd5S };
    enum dirName { IPt0S = 0, IPt1S, IPt2S, IPt3S, IPt4S, IPt5S };
    std::vector<myCut::generalCutList*> myCutLists;
    std::map< dirName, myCut::generalCutList*> testCuts;
    TFileDirectory* origDir;
    //virtual void createHisto( const std::string& name, dirName dName, int nbin, double min, double max ) ;
    //virtual void fillHisto( const std::string& name, dirName dName, double value ) ;
    virtual void specialClearHisto();
    std::map< dirName, std::map< std::string, TH1D* > > _hMap;
    std::map< dirName, std::string > _nMap;
    std::map< dirName, TFileDirectory > _dirMap;
};
#endif


