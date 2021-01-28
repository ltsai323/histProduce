
#ifndef __UPDATED_ggPhoton_formatTreeArrayI__
#define __UPDATED_ggPhoton_formatTreeArrayI__
#include "histProduce/treeFormat/interface/formatTreeArray.hpp"
struct ggPhoton_formatTreeArrayI : public formatTreeArray<int>
{
public:
    enum readVar
    {
        phohasPixelSeed,
        phoEleVeto,
        comNum
    };
    virtual void RegFormatTree(TTree* t) override;
    // LoadFormatSourceBranch and LoadEvtContent are two methods to load data. choose one.
    virtual void LoadFormatSourceBranch(TTree* t) override;
    virtual void LoadEvtContent(TreeReader& evtInfo) override;

    explicit ggPhoton_formatTreeArrayI( int totNum=comNum, int maxCSize=128 ) : formatTreeArray ( totNum, maxCSize ) {
    }
    virtual ~ggPhoton_formatTreeArrayI() {
    }
};

#endif
