#ifndef __rooTHMainGGPhoton_h__
#define __rooTHMainGGPhoton_h__
#include "histProduce/CMSSWTreeMgr/interface/rooTHMain.h"
#include "histProduce/treeFormat/interface/testGG_formatTreeArrayF.h"
//#include "histProduce/treeFormat/interface/formatTreeObject.hpp"
#include "histProduce/treeFormat/interface/untuplizer.h"
#include <vector>
#include <utility> // std::pair

typedef testGG_formatTreeArrayF varF;
class root_TreeHistoMain_GGPhoton : public root_TreeHistoMain
{
public:
    //root_TreeHistoMain_GGPhoton (TFile* d);
    root_TreeHistoMain_GGPhoton (TFileDirectory* d);
    virtual ~root_TreeHistoMain_GGPhoton();
    virtual void Process( unsigned int evtIdx ) override;
    virtual void GetEntry( unsigned int evtIdx ) override;
    virtual void RegTree() override;
    virtual void RegHisto() override;
    virtual void LoadSourceBranch() override;
    virtual void SummaryCalc() override;

    testGG_formatTreeArrayF phoLoadF;
    TreeReader interData;

};


#endif
