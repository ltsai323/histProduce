#ifndef __rooTHMain_h__
#define __rooTHMain_h__

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
#include "TFile.h"



class root_TreeHistoMain
{
public:
    // constructors & destructors
    root_TreeHistoMain( TFileDirectory* d, const std::string& pName );
    virtual ~root_TreeHistoMain();

    // functions needs to be redefined in further class
    virtual void Process( unsigned int pIdx ) = 0;
    virtual void Clear() = 0;
    virtual void RegTree() = 0;
    virtual void RegHisto() = 0;
    virtual void LoadSourceBranch() = 0;
    virtual void SummaryCalc() = 0;
    virtual bool SetInputFile( TFile* input );
    virtual void LoopEvents( unsigned& maxEvents ) final;
    virtual bool NoOutput() const;
            void ResetInputTreeName( const std::string& name );

    virtual TTree* thisTree() final { return outputTree; }
    virtual TTree* readTree() final { return  inputTree; }
    virtual const std::string readName() final { return inputTreeName; }

    // create TH1D & TH2D
    virtual void createHisto( const std::string& name, int nbin, double min, double max ) final;
    virtual void createHisto( const std::string& name, int nbin, double min, double max , int nbin2, double min2, double max2) final;
    // search name and fill TH1D & TH2D
    virtual void fillHisto( const std::string& name, double value ) final;
    virtual void fillHisto( const std::string& name, double valueX, double valueY ) final;

    virtual const std::map<std::string, TH1D*>& getHistos() const final;
    virtual const std::map<std::string, TH2D*>& getHistos2D() const final;
    // add general cuts in the whole analysis
    static void setCutList( std::vector<myCut::generalCutList*>* in ) { _cutLists = in; }
    static std::vector<myCut::generalCutList*>* getCutList() { return _cutLists; }

    std::string getFullName( const std::string& name ) const;
    const std::string getPreName() const { return preName; };
    void regName( const std::string& _preName );
    void setInputTreeName( const std::string& name );
    void setOutputAlive( bool tag );
private:
    // check if the output directory is set or not.
    bool noOutputFile;
    // information to output data
    TFileDirectory* dir;
    const std::string preName;

    // informations to input data
    TTree* inputTree;
    std::string inputTreeName;

    // check for tree recreate or not.
    static std::vector< std::string > nameReg;
    // global cuts
    static std::vector<myCut::generalCutList*>* _cutLists;

    TTree* outputTree;
    // histograms
    std::map< std::string, TH1D* > hMap;
    std::map< std::string, TH2D* > hMap2D;
};



#endif
