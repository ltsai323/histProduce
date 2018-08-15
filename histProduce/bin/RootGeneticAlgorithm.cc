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
#include "histProduce/histProduce/interface/GeneticAlgorithmMC.LbTk.h"

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

    parser.addOption("dataFile"     ,optutl::CommandLineParser::kString,"input data root file name"    ,"");
    parser.addOption("mcFile"       ,optutl::CommandLineParser::kString,"input mc   root file name"    ,"");
    parser.stringValue  ("outputFile" ) = "log_GA.txt";

    // parse arguments
    parser.parseArguments (argc, argv);



    if ( parser.stringValue("dataFile").empty() )
    {
        printf( "you need to input a data file for training! use 'dataFile' option \n" );
        exit(0);
    }
    if ( parser.stringValue("mcFile").empty() )
    {
        printf( "you need to input a mc file for training! use 'mcFile' option \n" );
        exit(0);
    }
    const std::string dataFile_ = parser.stringValue("dataFile");
    const std::string mcFile_ = parser.stringValue("mcFile");
    const std::string outLog = parser.stringValue("outputFile");
    // parser setting end }}}



    // set main code.

    // used without MC
    //std::vector<GeneticAlgorithm*> mainCode;
    //mainCode.push_back( new GeneticAlgorithm_LbTk(1000, 1000) );
    // used with MC
    std::vector<GeneticAlgorithmMC*> mainCode;
    mainCode.push_back( new GeneticAlgorithmMC_LbTk(1000, 1000) );

        // ----------------------------------------------------------------------
        // Second Part:
        //
        //  * loop the events in the input file
        //  * receive the collections of interest via fwlite::Handle
        //  * fill the histograms
        //  * after the loop close the input file
        // ----------------------------------------------------------------------
    TFile* dFile = TFile::Open( ("file://"+dataFile_).c_str() );
    if ( !dFile )
    { printf( "data file not found! check it\n" ); exit(1); }
    TFile* mFile = TFile::Open( ("file://"+mcFile_).c_str() );
    if ( !mFile )
    { printf( "mc file not found! check it\n" ); exit(1); }
    printf( "check end, start to evolution\n" );
    for ( auto& _main : mainCode )
    {
        printf("start to set data file\n");
        _main->SetData( dFile );
        printf("sucess to set data file into process, start to set mc file\n");
        _main->SetSignalMC( mFile );
        printf("sucess to set mc file into process, start to setup log file\n");
        _main->SetLogFile( outLog.c_str() );
        printf("sucess to setup log file\n");
        printf("start to evolute GA\n");
        _main->Evolution();
        printf("evolution END\n");
    }
    dFile->Close();
    mFile->Close();

    for ( auto& _main : mainCode )
    { delete _main; }

    return 0;
}
