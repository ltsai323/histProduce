#ifndef __hMainBs_h__
#define __hMainBs_h__
#include "histProduce/histProduce/interface/hMain.h"

class histMain_Bs : public histMain
{
public:
    histMain_Bs ( TFileDirectory* d);
    virtual void Process( fwlite::Event* ev );
};
#endif


