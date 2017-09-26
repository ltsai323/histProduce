#ifndef __hMainParPlot_h__
#define __hMainParPlot_h__
#include "histProduce/histProduce/interface/hMain.h"

class histMain_ParPlot : public histMain
{
public:
    histMain_ParPlot ( TFileDirectory* d);
    virtual void Process( fwlite::Event* ev ) override;
    virtual void Clear() override;
};
#endif


