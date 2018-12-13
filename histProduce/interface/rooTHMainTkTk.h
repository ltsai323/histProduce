#ifndef __rooTHMainTkTk_h__
#define __rooTHMainTkTk_h__
#include "histProduce/histProduce/interface/rooTHMain.h"
#include "histProduce/histProduce/interface/formatTreeTkTk.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include <vector>
#include <utility> // std::pair

class root_TreeHistoMain_TkTk : public root_TreeHistoMain, public formatTree_TkTk
{
public:
    root_TreeHistoMain_TkTk (TFileDirectory* d);
    virtual void Process( unsigned int pIdx ) override;
    virtual void RegTree() override;
    virtual void RegHisto() override;
    virtual void LoadSourceBranch() override;
    virtual void SummaryCalc() override;
private:
    const double kaonMass, protonMass, pionMass;

public:
    // if it is needed to add addiditional branch to new tree. added in this
    enum addVarD
    {
        originD = comNumD-1,
        totNumD
    };
    enum addVarI
    {
        originI = comNumI-1,
        //ptkPID,
        //ntkPID,
        totNumI
    };
};


#endif
