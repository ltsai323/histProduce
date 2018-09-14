
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

#include <TH1D.h>
#include <TROOT.h>
#include <TFile.h>
#include <TSystem.h>
#include <TTree.h>

#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"
#include "PhysicsTools/FWLite/interface/CommandLineParser.h"

#include "histProduce/histProduce/interface/rooTHMain.h"
//#include "histProduce/histProduce/interface/rooTHMainTkTk.h"
#include "histProduce/histProduce/interface/rooTHMainLbTk.h"
#include "histProduce/histProduce/interface/rooTHMainpnLbTk.h"
#include "histProduce/histProduce/interface/rooTHMainGenLbTk.h"
#include "histProduce/histProduce/interface/rooTHMainGenpnLbTk.h"

// create trees from.tree based file.
// use FWLIte to load data.
// this file just write down:
// 1. the argument you should or can input.
// 2. apply general cuts known.
// 3. loop the event.
//
// And the content in the loop is written in histProduce/histProduce/src/tMain????.cc
// every tMain??? decides what to do in each event.
// Normally, create a tree and fill it. Save it into root file.
//
// usage: modification is needed!. this is old illustration
//     tCreate filelist=data_RunG maxEvents=1000 readTPath=lbSpecificDecay/LbTk
//     tCreate testfile=a.root maxEvents=1000 outputEvery=1 outputFile=b.root readTPath=lbSpecificDecay/LbTk
//
// create TTree.

int main(int argc, char* argv[])
{
    // ----------------------------------------------------------------------
    // First Part:
    //
    //  * enable the AutoLibraryLoader
    //  * book the histograms of interest
    //  * open the input files
    // ----------------------------------------------------------------------

    // load framework libraries
    gSystem->Load( "libFWCoreFWLite" );
    AutoLibraryLoader::enable();

    // check default path {{{
    std::string sysPath = std::string( getenv("CMSSW_BASE") );
    try
    {
        if ( sysPath == "" )
            throw "----- error, you need to setup CMSSW envirnment! -----";
    }
    catch ( const char* e )
    { printf( "%s", e ); }
    // check default path end }}}

    // parser setting {{{
    // initialize command line parser
    optutl::CommandLineParser parser ("Analyze FWLite Histograms");

    parser.addOption("testFile"     ,optutl::CommandLineParser::kString,"test input file, recommend to use fileList_cfi.py to put files"    ,"");
    parser.addOption("fileList"     ,optutl::CommandLineParser::kString,"input the python file records file list."                          ,"fileList");
    parser.addOption("configFile"   ,optutl::CommandLineParser::kString,"the plot options recorded in python file"                          ,"histogramPlotParameter");

    // if not set, use the default value in rooTHMainABCD.cc files
    // the default values are like   lbSpecificDecay/LbTk
    parser.addOption("readTPath"     ,optutl::CommandLineParser::kString,"the path indicating to tree in root file"                          ,"");
    // set defaults for testFile is assigned
    parser.integerValue ("maxEvents"  ) = -1;

    // if the value smaller than 0, do not print anything on screen
    parser.integerValue ("outputEvery") = 100;
    parser.stringValue  ("outputFile" ) = "reducedTHOutput.root";

    // parse arguments
    parser.parseArguments (argc, argv);

    // deal with python file path
    const std::string configFile = parser.stringValue("configFile");
    const std::string listFile   = parser.stringValue("fileList");
    const std::string dPath      = "/src/histProduce/histProduce/python/";
    const std::string confPython = sysPath+dPath+configFile+"_cfi.py";
    const std::string fOptPython = sysPath+dPath+listFile+"_cfi.py";
    // load IO option from python file. default is fileList_cfi.py
    const edm::ParameterSet& ioOption = edm::readPSetsFrom(fOptPython.c_str())->getParameter<edm::ParameterSet>("process");
    const edm::ParameterSet& runSetting = ioOption.getParameter<edm::ParameterSet>("runSetting");
    const edm::ParameterSet& inputFileNames = ioOption.getParameter<edm::ParameterSet>("inputFiles");
    const std::vector<std::string>& fileNames = inputFileNames.getParameter< std::vector<std::string> >("fileNames");
    int maxEvents_          = runSetting.getParameter<int>("maxEvents");
    unsigned outputEvery_   = runSetting.getParameter<unsigned>("outEvery");
    std::string outputFile_ = runSetting.getParameter<std::string>("outName");

    // load cut configuration from python file. default is histogramCutParameter_cfi.py
    //const edm::ParameterSet& cutOption = edm::readPSetsFrom(confPython.c_str())->getParameter<edm::ParameterSet>("process");
    //const std::vector<edm::ParameterSet>& runSetting_ = cutOption.getParameter< std::vector<edm::ParameterSet> >("cutSet");
    // parser setting end }}}

    // input files
    std::vector<std::string> inputFiles_;

    // if inputFile not set, use python file : fileList_cfi.py
    if ( parser.stringValue("testFile").empty() )
    {
        inputFiles_ = fileNames;
        if ( parser.integerValue("maxEvents")>0 )
            maxEvents_ = parser.integerValue("maxEvents");
    }
    else
    {
        // only if testFile is set, parameters from parser valid
        inputFiles_.push_back( parser.stringValue("testFile") );
        outputFile_ = parser.stringValue("outputFile");
        maxEvents_ = parser.integerValue("maxEvents");
        outputEvery_ = parser.integerValue("outputEvery");
    }

    // book a root file to store data.
    fwlite::TFileService fs = fwlite::TFileService(outputFile_.c_str());
    TFileDirectory dir = fs.mkdir("lbSpecificDecay");
    //TFileDirectory dir = fs.mkdir(parser.stringValue("readPath").c_str());


    //root_TreeHistoMain::setCutList( &cutLists );

    // set main code.
    std::vector<root_TreeHistoMain*> mainCode;
    //mainCode.push_back( new root_TreeHistoMain_TkTk(&dir) );
    //mainCode.push_back( new root_TreeHistoMain_LbTk(&dir) );
    //mainCode.push_back( new root_TreeHistoMain_GenInfo_LbTk(&dir) );
    mainCode.push_back( new root_TreeHistoMain_plusminus_LbTk(&dir) );
    //mainCode.push_back( new root_TreeHistoMain_GenInfo_plusminus_LbTk(&dir) );

    // if maxEvent = -1, MEvent to be MAX of unsigned.
    unsigned MEvent = maxEvents_;


    // useless code
    if ( !outputEvery_ )
        outputEvery_ = outputEvery_;
    for ( const auto& file : inputFiles_ )
    {
        TFile* inFile = TFile::Open( ("file://"+file).c_str() );
        if( !inFile ) continue;

        // ----------------------------------------------------------------------
        // Second Part:
        //
        //  * loop the events in the input file
        //  * receive the collections of interest via fwlite::Handle
        //  * fill the histograms
        //  * after the loop close the input file
        // ----------------------------------------------------------------------
        
        if ( !mainCode.size() ) break;
        unsigned Mevent[ mainCode.size() ];
        for ( unsigned int i=0; i< mainCode.size(); ++i )
            Mevent[i]=MEvent;
        unsigned j=0;
        for ( const auto& _main : mainCode )
        {
            if ( parser.stringValue("readTPath") != "" )
                _main->ResetInputTreeName( parser.stringValue("readTPath") );
            bool keepGoing = _main->SetInputFile( inFile );


            // use MEvent to be the counter, if MEvent to be 0, stop the code.
            if ( keepGoing )
                _main->LoopEvents( Mevent[++j] );
        }
        inFile->Close();
        if ( Mevent[0] == 0 ) break;
        MEvent = Mevent[0];
        // break loop if maximal number of events is reached:
        // this has to be done twice to stop the file loop as well
    }

    fs.file().Write();
    // for general cuts
    //for ( auto& cut : cutLists )
    //    delete cut;
    for ( auto& _main : mainCode )
    {
        _main->SummaryCalc();
        delete _main;
    }

    return 0;
}
