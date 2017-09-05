#ifndef __hMain_h__
#define __hMain_h__

#include <string>
#include <vector>
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "PhysicsTools/FWLite/interface/TFileService.h"
#include "histProduce/histProduce/interface/generalCutList.h"


// maybe you can change pat::CompositeCandidate to template 

class histMain
{
public:
    struct Label
    {
        std::string module,  label, process;
        Label( const std::string& m, const std::string& l, const std::string& p ) :
            module( m ), label( l ), process( p ) {}
    };
    histMain( TFileDirectory* d, Label l ) : dir( d ), _label( l ) {}
    virtual ~histMain() {}
    virtual void Process( fwlite::Event* ev ) = 0;
    void setRefitName( const std::string& name )
    { _refitName.emplace_back( name ); return; }

           virtual void createHisto( const std::string& name, int nbin, double min, double max ) final;
           virtual void fillHisto( const std::string& name, double value ) final;
    static void clearHisto();
    static void setCutList( std::vector<myCut::generalCutList*>* in ) { _cutLists = in; }
    std::vector<myCut::generalCutList*>* getCutList()  { return _cutLists; }


    fwlite::Handle< std::vector<pat::CompositeCandidate> > _handle;
    TFileDirectory* dir;
    Label _label;
private:
    std::vector<std::string> _refitName;
    static std::vector<myCut::generalCutList*>* _cutLists;
    static std::map< std::string, TH1D* > hMap;
};



#endif
