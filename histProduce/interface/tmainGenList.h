#ifndef __tMainGenList_h__
#define __tMainGenList_h__
#include "histProduce/histProduce/interface/tmainGen.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include <vector>
#include <utility> // std::pair

class treeMainGen_List : public treeMainGen
{
public:
    treeMainGen_List ( TFileDirectory* d);
    virtual void Process( fwlite::Event* ev ) override;
    virtual void Clear() override;
    virtual void RegTree() override;
    virtual void GetByLabel( fwlite::Event* ev ) override;
private:
    const double protonMass;
    const double   pionMass;

    enum dataVarD
    {
        motherPt, motherMass,

        d1Pt, d1Mom,
        d2Pt, d2Mom,
        d3Pt, d3Mom,
        totNumD
    };
    enum dataVarI
    {
        motherPID, motherNumDaughter,
        d1PID,
        d2PID,
        d3PID,
        totNumI
    };
    // double data to store variables
    double dataD[totNumD];
    // int data to store variables
    int dataI[totNumI];
};


#endif
