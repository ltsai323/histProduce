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





class treeMain
{
public:
    struct Label
    {
        std::string module,  label, process;
        Label( const std::string& m, const std::string& l, const std::string& p ) :
            module( m ), label( l ), process( p ) {}
    };
    treeMain( TFileDirectory* d, Label l, const std::string& pName ) : _label( l ), dir( d ),  preName( pName )
    {
        regName( preName );
        _storageTree = dir->make<TTree>( preName.c_str(), preName.c_str() );
    }
    virtual ~treeMain() {}
    virtual void Process( fwlite::Event* ev ) = 0;
    virtual void Clear() = 0;

    virtual void regTree() = 0;
    virtual TTree* thisTree() final { return _storageTree; }
    // add general cuts in the whole analysis
    static void setCutList( std::vector<myCut::generalCutList*>* in ) { _cutLists = in; }
    static std::vector<myCut::generalCutList*>* getCutList()  { return _cutLists; }
    virtual std::string getFullName( const std::string& name ) final;
    virtual void regName( const std::string& _preName ) final;




    fwlite::Handle< std::vector<pat::CompositeCandidate> > _handle;
    const Label _label;

private:
    TFileDirectory* dir;
    const std::string preName;
    static std::vector< std::string > nameReg;
    static std::vector<myCut::generalCutList*>* _cutLists;

    TTree* _storageTree;
};



#endif
