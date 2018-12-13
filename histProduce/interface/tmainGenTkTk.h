#ifndef __tMainGenTkTk_h__
#define __tMainGenTkTk_h__
#include "histProduce/histProduce/interface/tmainGen.h"
#include "histProduce/histProduce/interface/formatTreeTkTk.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include <vector>
#include <utility> // std::pair

class treeMainGen_TkTk : public treeMainGen, public formatTree_TkTk
{
public:
    treeMainGen_TkTk ( TFileDirectory* d);
    virtual void Process( fwlite::Event* ev ) override;
    virtual void RegTree() override;
    virtual void GetByLabel( fwlite::Event* ev ) override;
private:
    void getByLabel_BS( fwlite::Event* ev );
    fwlite::Handle< std::vector<reco::GenParticle> > genHandle;
    fwlite::Handle< reco::BeamSpot > beamSpotHandle;
    // store the result of 'particle-MC genParticle pair'
    //std::vector< std::pair<const void*, const reco::GenParticle*> > pgPair;
    const double protonMass;
    const double   pionMass;


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
        ptkPID,
        ntkPID,
        tktkSortingNumber,
        totNumI
    };
};


#endif
