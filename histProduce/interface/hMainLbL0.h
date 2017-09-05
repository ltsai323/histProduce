#ifndef __hMainLbL0_h__
#define __hMainLbL0_h__
#include "histProduce/histProduce/interface/hMain.h"

class histMain_LbL0 : public histMain
{
public:
    histMain_LbL0 ( TFileDirectory* d);
    virtual void Process( fwlite::Event* ev );
};
#endif


