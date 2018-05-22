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
#include <TTree.h>

#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"
#include "PhysicsTools/FWLite/interface/CommandLineParser.h"

#include "histProduce/histProduce/interface/GeneticAlgorithmLbTk.h"

// load data and use generic algorithm to get cuts:
// use TTree to load data
// this file just write down:
// 1. the argument you should or can input.
// 2. apply general cuts known.
// 3. loop the event.
//
// usage: modification is needed!.
//     genericAlgorithm testfile=a.root outputFile=result_GA.txt
// not available commands:
//     genericAlgorithm filelist=data_RunG outTxt=result_GA.txt Ncore=a

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
    parser.stringValue  ("outputFile" ) = "log_GA.txt";

    // parse arguments
    parser.parseArguments (argc, argv);



    if ( parser.stringValue("testFile").empty() )
    {
        printf( "you need to input a file for training! use 'testFile' option \n" );
        exit(0);
    }
    const std::string inputFile_ = parser.stringValue("testFile");
    const std::string outLog = parser.stringValue("outputFile");
    // parser setting end }}}



    // set main code.
    std::vector<GeneticAlgorithm*> mainCode;
    mainCode.push_back( new GeneticAlgorithm_LbTk(1000, 1000) );

        // ----------------------------------------------------------------------
        // Second Part:
        //
        //  * loop the events in the input file
        //  * receive the collections of interest via fwlite::Handle
        //  * fill the histograms
        //  * after the loop close the input file
        // ----------------------------------------------------------------------
    TFile* inFile = TFile::Open( ("file://"+inputFile_).c_str() );
    if ( !inFile )
    { printf( "test file not found! check it\n" ); exit(1); }
    printf( "check end, start to evolution\n" );
    for ( auto& _main : mainCode )
    {
        _main->SetData( inFile );
        _main->SetLogFile( outLog.c_str() );
        _main->Evolution();
    }
    inFile->Close();

    for ( auto& _main : mainCode )
    { delete _main; }

    return 0;
}
