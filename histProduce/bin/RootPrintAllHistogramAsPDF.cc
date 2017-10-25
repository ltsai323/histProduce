#include "PhysicsTools/FWLite/interface/CommandLineParser.h"
#include "TROOT.h" // gROOT decalre
#include "TRint.h" // gROOT declare
#include "TFile.h"
#include "TDirectory.h"
#include "TCollection.h" // for TIter
#include "TKey.h"
#include "TCanvas.h"
#include "TClass.h"
#include "TH1.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;
void readDirAndPrintHisto( TDirectory* dir, TCanvas* c1, const string& outputFileName );
int main(int argc, char* argv[])
{
    // define what muon you are using; this is necessary as FWLite is not
    // capable of reading edm::Views

    // ----------------------------------------------------------------------
    // First Part:
    //
    //  * enable the AutoLibraryLoader
    //  * book the histograms of interest
    //  * open the input file
    // ----------------------------------------------------------------------

    // load framework libraries
    //gSystem->Load( "libFWCoreFWLite" );
    //AutoLibraryLoader::enable();


    // parser setting {{{
    // initialize command line parser
    optutl::CommandLineParser parser ("print out all of histograms in the root file");

    parser.stringValue  ("outputFile" ) = "histoTestOutput";

    // parse arguments
    parser.parseArguments (argc, argv);

    // parser setting end }}}

    // input files
    vector<string> inFiles = parser.stringVector("inputFiles");
    for ( const string& fName : inFiles )
    {
        string outputFileName = parser.stringValue( "outputFile" ) + "_" + fName + ".pdf";
        TFile* fileObj = TFile::Open( fName.c_str() );
        TCanvas* c1 = new TCanvas( "lotsOfPlots", "", 1600, 1000 );
        c1->Print( (outputFileName+"[").c_str() );
        readDirAndPrintHisto( fileObj, c1, outputFileName );
        c1->Print( (outputFileName+"]").c_str() );
        fileObj->Close();
    }
}

void readDirAndPrintHisto( TDirectory* dir, TCanvas* c1, const string& outputFileName )
{
    TDirectory* mySavedDir = gDirectory;
    TDirectory* currentDir = gDirectory;
    TIter next( dir->GetListOfKeys() );
    TKey* key;
    while( (key = (TKey*)next()) )
    {
        if ( key->IsFolder() )
        {
            dir->cd( key->GetName() );
            currentDir = gDirectory;
            readDirAndPrintHisto( currentDir, c1, outputFileName );
            mySavedDir->cd();
            continue;
        }
        TClass* cl = gROOT->GetClass( key->GetClassName() );
        if ( !cl->InheritsFrom("TH1") ) continue;
        TH1* h = (TH1*)key->ReadObj();

        h->Draw();
        c1->Print( outputFileName.c_str() );
    }
}
