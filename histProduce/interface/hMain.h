#ifndef __hMain_h__
#define __hMain_h__

#include <string>
#include <vector>
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "PhysicsTools/FWLite/interface/TFileService.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "TH1D.h"
#include "TH2D.h"


// abstruct class to process main function.
// histMain::Label          : store the labels to be used in handle.getByLabel().
// histMain::preName        : preName is the name used to separate the histograms in TFile.
//                            the name used in TFile is preName_histName.
// histMain::Process()      : the main function of bin/FWLiteHistoCreater.cc
// histMain::createHisto()  : create histogram, which is managed & protected by a map.
// histMain::fillHisto()    : fill the histograms.
// histMain::setCutList()   : set general cut list assigned in src/FWLiteHistoCreater.cc.
// histMain::getCutList()   : get general cut list.
// histMain::regName()      : check the preName is the same or not. If it is duplicated, stop the code.
// histMain::Clear()        : if each Process owns something to be deleted at the end.

class histMain
{
public:
    struct Label
    {
        std::string module,  label, process;
        Label( const std::string& m, const std::string& l, const std::string& p ) :
            module( m ), label( l ), process( p ) {}
    };
    histMain( TFileDirectory* d, Label l, const std::string& pName ) : dir( d ), _label( l ), preName( pName ) {}
    virtual ~histMain() {}
    virtual void Process( fwlite::Event* ev ) = 0;
    virtual void Clear() = 0;

    // create TH1D & TH2D
    virtual void createHisto( const std::string& name, int nbin, double min, double max ) final;
    virtual void createHisto( const std::string& name, int nbin, double min, double max , int nbin2, double min2, double max2) final;
    // search name and fill TH1D & TH2D
    virtual void fillHisto( const std::string& name, double value ) final;
    virtual void fillHisto( const std::string& name, double valueX, double valueY ) final;
    // add general cuts in the whole analysis
    static void setCutList( std::vector<myCut::generalCutList*>* in ) { _cutLists = in; }
    static std::vector<myCut::generalCutList*>* getCutList()  { return _cutLists; }
    virtual std::string getFullName( const std::string& name ) final;
    virtual void regName( const std::string& _preName ) final;




    fwlite::Handle< std::vector<pat::CompositeCandidate> > _handle;
    TFileDirectory* dir;
    const Label _label;
private:
    const std::string preName;
    static std::vector< std::string > nameReg;
    static std::vector<myCut::generalCutList*>* _cutLists;
    std::map< std::string, TH1D* > hMap;
    std::map< std::string, TH2D* > hMap2D;

};



#endif
