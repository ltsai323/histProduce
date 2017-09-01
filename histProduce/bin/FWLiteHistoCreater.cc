#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

#include <TH1D.h>
#include <TROOT.h>
#include <TFile.h>
#include <TSystem.h>

#include "DataFormats/FWLite/interface/Event.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "PhysicsTools/FWLite/interface/TFileService.h"
#include "PhysicsTools/FWLite/interface/CommandLineParser.h"
#include "histProduce/histProduce/interface/histoMAP.h"


int main(int argc, char* argv[])
{
    // define what muon you are using; this is necessary as FWLite is not
    // capable of reading edm::Views
    using reco::Muon;

    // ----------------------------------------------------------------------
    // First Part:
    //
    //  * enable the AutoLibraryLoader
    //  * book the histograms of interest
    //  * open the input file
    // ----------------------------------------------------------------------

    // load framework libraries
    gSystem->Load( "libFWCoreFWLite" );
    AutoLibraryLoader::enable();


    // check if the system depends on CMSSW or not
    std::string sysPath = std::string( getenv("CMSSW_BASE") );
    try
    {
        if ( sysPath == "" )
            throw "----- error, you need to setup CMSSW envirnment! -----";
    }
    catch ( const char* e )
    { printf( "%s", e ); }

    // initialize command line parser
    optutl::CommandLineParser parser ("Analyze FWLite Histograms");

    // set defaults
    parser.stringValue  ("inputFile"  ) = "";
    parser.stringValue  ("fileList"   ) = "fileList";
    parser.stringValue  ("configFile" ) = "histogramPlotParameter";

    // parse arguments
    parser.parseArguments (argc, argv);

    // deal with python file path
    std::string configFile = parser.stringValue("configFile");
    std::string listFile   = parser.stringValue("fileList");
    std::string dPath      = "/src/histProduce/histProduce/python/";
    std::string confPython = sysPath+dPath+configFile+"_cfi.py";
    std::string fOptPython = sysPath+dPath+listFile+"_cfi.py";
    // load IO option from python file. default is fileList_cfi.py
    const edm::ParameterSet& ioOption = edm::readPSetsFrom(fOptPython.c_str())->getParameter<edm::ParameterSet>("process");
    const edm::ParameterSet& runSetting = ioOption.getParameter<edm::ParameterSet>("runSetting");
    const edm::ParameterSet& inputFileNames = ioOption.getParameter<edm::ParameterSet>("inputFiles");
    const std::vector<std::string>& fileNames = inputFileNames.getParameter< std::vector<std::string> >("fileNames");
    int maxEvents_          = runSetting.getParameter<int>("maxEvents");
    unsigned outputEvery_   = runSetting.getParameter<unsigned>("outputEvery");
    std::string outputFile_ = runSetting.getParameter<std::string>("outputFile");

    // load cut configuration from python file. default is histogramCutParameter_cfi.py
    const edm::ParameterSet& cutOption = edm::readPSetsFrom(confPython.c_str())->getParameter<edm::ParameterSet>("process");
    const std::vector<edm::ParameterSet>& runSetting_ = cutOption.getParameter< std::vector<edm::ParameterSet> >("cutSet");


    // input files
    std::vector<std::string> inputFiles_;

    if ( parser.stringValue("inputFile") != "" )
        inputFiles_.push_back( parser.stringValue("inputFile") );
    else
        inputFiles_ = fileNames;

    // book a set of histograms
    fwlite::TFileService fs = fwlite::TFileService(outputFile_.c_str());
    TFileDirectory dir = fs.mkdir("lbSpecificDecay");
    histoMAP hMap;
    hMap.createHisto( &dir, "oniaMass", 50, 2.8, 3.3 );
    hMap.createHisto( &dir, "tktkMass",160, 1.4, 3.0 );
    hMap.createHisto( &dir, "lam0Mass", 50, 1.1, 1.15);
    hMap.createHisto( &dir, "LbL0Mass", 50, 5.0, 6.0 );
    hMap.createHisto( &dir, "LbTkMass", 50, 5.0, 6.0 );
    hMap.createHisto( &dir, "BsBsMass", 50, 5.0, 6.0 );


    //// cuts applied
    //vector<CutList*> cutLists;
    //cutLists.push_back( new      vtxprobCut(0.15,-99. , root->refitPos.getAdd()) );
    //cutLists.push_back( new         massCut(5.0 ,  6.0, root->refitMom.getAdd()) );
    //cutLists.push_back( new       cosa2dCut(0.99,       root->refitPos.getAdd(), root->refitMom.getAdd(), root->primaryV.getAdd()) );
    //cutLists.push_back( new           ptCut(15  ,-99. , root->refitMom.getAdd()) );
    //cutLists.push_back( new flightDist2DCut(0.2 ,  0.6, root->refitPos.getAdd(), root->primaryV.getAdd()) );

    for ( const auto& file : inputFiles_ )
    {
        TFile* inFile = TFile::Open( ("file://"+file).c_str() );
        if( inFile )
        {
            // ----------------------------------------------------------------------
            // Second Part: 
            //
            //  * loop the events in the input file 
            //  * receive the collections of interest via fwlite::Handle
            //  * fill the histograms
            //  * after the loop close the input file
            // ----------------------------------------------------------------------
            fwlite::Event ev(inFile);
            int ievt=0;
            for(ev.toBegin(); !ev.atEnd(); ++ev, ++ievt)
            {
	            edm::EventBase const & event = ev;
	            // break loop if maximal number of events is reached 
	            if(maxEvents_>0 ? ievt+1>maxEvents_ : false) break;
	            // simple event counter
                if ( ievt > 0 && ievt%outputEvery_ == 0 ) printf( "  processing event: %i \n", ievt );

                //for ( const auto& info : PlotInfo )
                //{
                    //const std::string& hName = runSetting.getParameter< std::string >( "HistoName" );
                    //const std::string& title = runSetting.getParameter< std::string >( "TitleName" );
                    //hMap

                    // Handle to the muon collection
                    fwlite::Handle< std::vector<pat::CompositeCandidate> > oniaHandle;
                    oniaHandle.getByLabel( ev, "lbWriteSpecificDecay", "oniaFitted", "bphAnalysis" );
    
                    // loop muon collection and fill histograms
                    std::vector< pat::CompositeCandidate >::const_iterator iter;
                    std::vector< pat::CompositeCandidate >::const_iterator iend;
                    iter = oniaHandle->begin();
                    iend = oniaHandle->end  ();
                    while ( iter != iend )
                    {
                        const pat::CompositeCandidate& cand = *iter++;
                        hMap.fillHisto("oniaMass", cand.mass() );
                    }
    
                    
                    fwlite::Handle< std::vector<pat::CompositeCandidate> > lam0Handle;
                    lam0Handle.getByLabel( ev, "lbWriteSpecificDecay", "Lam0Cand", "bphAnalysis" );
    
                    // loop muon collection and fill histograms
                    iter = lam0Handle->begin();
                    iend = lam0Handle->end  ();
                    while ( iter != iend )
                    {
                        const pat::CompositeCandidate& cand = *iter++;
                        hMap.fillHisto("lam0mass", cand.mass() );
                    }
    
    
                    fwlite::Handle< std::vector<pat::CompositeCandidate> > tktkHandle;
                    tktkHandle.getByLabel( ev, "lbWriteSpecificDecay", "TkTkFitted", "bphAnalysis" );
    
                    // loop muon collection and fill histograms
                    iter = tktkHandle->begin();
                    iend = tktkHandle->end  ();
                    while ( iter != iend )
                    {
                        const pat::CompositeCandidate& cand = *iter++;
                        hMap.fillHisto("tktkMass", cand.mass() );
                    }
    
    
                    fwlite::Handle< std::vector<pat::CompositeCandidate> > LbL0Handle;
                    LbL0Handle.getByLabel( ev, "lbWriteSpecificDecay", "LbToLam0Fitted", "bphAnalysis" );
    
                    // loop muon collection and fill histograms
                    iter = LbL0Handle->begin();
                    iend = LbL0Handle->end  ();
                    while ( iter != iend )
                    {
                        const pat::CompositeCandidate& cand = *iter++;
                        hMap.fillHisto("LbL0Mass", cand.mass() );
                    }
    
                    fwlite::Handle< std::vector<pat::CompositeCandidate> > LbTkHandle;
                    LbTkHandle.getByLabel( ev, "lbWriteSpecificDecay", "LbToTkTkFitted", "bphAnalysis" );
    
                    // loop muon collection and fill histograms
                    iter = LbTkHandle->begin();
                    iend = LbTkHandle->end  ();
                    while ( iter != iend )
                    {
                        const pat::CompositeCandidate& cand = *iter++;
                        hMap.fillHisto("LbTkMass", cand.mass() );
                        if ( cand.hasUserData("JPsi/MuPos.charge") && cand.hasUserData("JPsi/MuNeg.charge") )
                        {
                            const int* pCharge = cand.userData<int>("JPsi/MuPos.charge");
                            const int* nCharge = cand.userData<int>("JPsi/MuNeg.charge");

                            if ( *pCharge * *nCharge  == -1 )
                                std::cout << "Lb is neutral!\n";
                            else
                                std::cout << "Lb is not neutral\n";
                        }
                        else std::cout << "jpsi daughters are not filled!\n";



                    }
                    fwlite::Handle< std::vector<pat::CompositeCandidate> > BsBsHandle;
                    BsBsHandle.getByLabel( ev, "lbWriteSpecificDecay", "bsFitted", "bphAnalysis" );
    
                    // loop muon collection and fill histograms
                    iter = BsBsHandle->begin();
                    iend = BsBsHandle->end  ();
                    while ( iter != iend )
                    {
                        const pat::CompositeCandidate& cand = *iter++;
                        hMap.fillHisto("BsBsMass", cand.mass() );
                        //if ( cand.hasUserData("JPsi/MuPos.charge") && cand.hasUserData("JPsi/MuNeg.charge") )
                        //{
                        //    const int* pCharge = cand.userData<int>("JPsi/MuPos.charge");
                        //    const int* nCharge = cand.userData<int>("JPsi/MuNeg.charge");

                        //    if ( *pCharge * *nCharge  == -1 )
                        //        std::cout << "Lb is neutral!\n";
                        //    else
                        //        std::cout << "Lb is not neutral\n";
                        //}
                        //else std::cout << "jpsi daughters are not filled!\n";



                    }
                //}
                // close input file
                inFile->Close();
                if(maxEvents_>0 ? ievt+1>maxEvents_ : false) break;
            }
        } // if inFile
        // break loop if maximal number of events is reached:
        // this has to be done twice to stop the file loop as well
    }
    return 0;
}
