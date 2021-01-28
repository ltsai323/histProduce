#ifndef __rooTHMainSampler_h__
#define __rooTHMainSampler_h__
#include "histProduce/CMSSWTreeMgr/interface/rooTHMain.h"
//#include "histProduce/treeFormat/interface/formatTreeLbTk.h"
#include <vector>
#include <utility> // std::pair

class root_TreeHistoMain_Sampler : public root_TreeHistoMain
{
public:
    //root_TreeHistoMain_Sampler (TFile* d);
    root_TreeHistoMain_Sampler (TFileDirectory* d);
    virtual void Process( unsigned int pIdx ) override;
    virtual void RegTree() override;
    virtual void RegHisto() override;
    virtual void LoadSourceBranch() override;
    virtual void SummaryCalc() override;

};


#endif
