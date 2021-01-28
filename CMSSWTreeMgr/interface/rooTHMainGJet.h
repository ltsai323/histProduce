#ifndef __rooTHMainGJet_h__
#define __rooTHMainGJet_h__
#include "histProduce/CMSSWTreeMgr/interface/rooTHMain.h"
#include "histProduce/treeFormat/interface/ggPhoton_formatTreeArrayF.h"
#include "histProduce/treeFormat/interface/ggPhoton_formatTreeArrayI.h"
//#include "histProduce/treeFormat/interface/formatTreeObject.hpp"
#include "histProduce/treeFormat/interface/untuplizer.h"
#include <vector>
#include <utility> // std::pair

typedef ggPhoton_formatTreeArrayF pF;
typedef ggPhoton_formatTreeArrayI pI;
class root_TreeHistoMain_GJet : public root_TreeHistoMain
{
public:
    //root_TreeHistoMain_GJet (TFile* d);
    root_TreeHistoMain_GJet (TFileDirectory* d);
    virtual ~root_TreeHistoMain_GJet();
    virtual void Process( unsigned int evtIdx ) override;
    virtual void GetEntry( unsigned int evtIdx ) override;
    virtual void RegTree() override;
    virtual void RegHisto() override;
    virtual void LoadSourceBranch() override;
    virtual void SummaryCalc() override;

    ggPhoton_formatTreeArrayF phoLoadF;
    ggPhoton_formatTreeArrayI phoLoadI;
    //TreeReader interData;

};


#endif
