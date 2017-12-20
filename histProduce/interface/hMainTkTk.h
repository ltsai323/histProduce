#ifndef __hMainTkTk_h__
#define __hMainTkTk_h__
#include "histProduce/histProduce/interface/hMain.h"

class histMain_TkTk : public histMain
{
public:
    histMain_TkTk ( TFileDirectory* d);
    virtual void Process( fwlite::Event* ev ) override;
    virtual void Clear() override;
private:
};




#endif


