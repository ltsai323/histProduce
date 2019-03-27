#ifndef __objectReadingFormatTreeArrayLbTk_h__
#define __objectReadingFormatTreeArrayLbTk_h__
#include "TLorentzVector.h"
#include "histProduce/histProduce/interface/formatTreeArrayLbTk.h"

class readingObject
{
public:
    readingObject( formatTreeArray_LbTk* ptr ) : idx(-1), data(ptr) {}
    void SetCandId( int i ) { idx = i; return; }
    TLorentzVector getP4();
    


private:
    int idx;
    const formatTreeArray_LbTk* const data;
};


#endif
