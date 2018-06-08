#ifndef __tMain_h__
#define __tMain_h__

#include <string>
#include <vector>
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "PhysicsTools/FWLite/interface/TFileService.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"


class treeMain
{
public:
    struct Label
    {
        std::string module,  label, process;
        Label( const std::string& m, const std::string& l, const std::string& p ) :
            module( m ), label( l ), process( p ) {}
    };

    // constructors & destructors
    treeMain( TFileDirectory* d, Label l, const std::string& pName );
    virtual ~treeMain();

    // functions needs to be redefined in further class
    virtual void Process( fwlite::Event* ev ) = 0;
    virtual void Clear() = 0;
    virtual void RegTree() = 0;
    virtual void GetByLabel( fwlite::Event* ev ) = 0;

    virtual TTree* thisTree() final { return _storageTree; }
    // add general cuts in the whole analysis
    static void setCutList( std::vector<myCut::generalCutList*>* in ) { _cutLists = in; }
    static std::vector<myCut::generalCutList*>* getCutList()  { return _cutLists; }
    virtual std::string getFullName( const std::string& name ) final;
    virtual void regName( const std::string& _preName ) final;
    void getByLabel_Cand( fwlite::Event* ev );

    virtual TH1D* createHisto( const std::string& name, int nbin, double xmin, double xmax ) final;
    virtual TH2D* createHisto( const std::string& name, int nbin, double xmin, double xmax, int mbin, double ymin, double ymax ) final;

    fwlite::Handle< std::vector<pat::CompositeCandidate> > _handle;
private:
    const Label _label;
    TFileDirectory* dir;
    const std::string preName;
    static std::vector< std::string > nameReg;
    static std::vector<myCut::generalCutList*>* _cutLists;

    TTree* _storageTree;
};



#endif
