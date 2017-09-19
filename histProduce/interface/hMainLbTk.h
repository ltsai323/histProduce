#ifndef __hMainLbTk_h__
#define __hMainLbTk_h__
#include "histProduce/histProduce/interface/hMain.h"

class histMain_LbTk : public histMain
{
public:
    histMain_LbTk ( TFileDirectory* d);
    virtual void Process( fwlite::Event* ev ) override;
    virtual void Clear() override;
};
#endif


