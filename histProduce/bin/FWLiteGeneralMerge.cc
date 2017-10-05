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
#include <TPad.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLine.h>

#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "PhysicsTools/FWLite/interface/TFileService.h"
#include "PhysicsTools/FWLite/interface/CommandLineParser.h"
#include "histProduce/histProduce/interface/histoMAP.h"
#include "histProduce/histProduce/interface/generalCutList.h"

// merge  any plots in root file.
// execute this once will create one figure.
//
// usage: 
//     hMergeGeneral plotset=histoMergeGeneralPar testfile=mySource.root

struct plotPrivateSet
{
    std::string nameInTree;
    std::string title;
    double scaleFactor;
    bool setFill;
    int lineColor;
    int lineWidth;
    int fillColor;
    int fillStyle;
    plotPrivateSet ( const std::string& n, const std::string& t, double S, bool sF, int lC, int lW, int fC, int fS ) :
        nameInTree ( n ), title ( t ), scaleFactor( S ), setFill( sF ), lineColor( lC ), lineWidth( lW ), fillColor( fC ), fillStyle( fS ) {}
};
    

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

    parser.addOption("testFile",optutl::CommandLineParser::kString,"test input file","");
    parser.addOption("plotSet",optutl::CommandLineParser::kString,"input the python file records parameter to plot","histoMergeGeneralPar");

    // parse arguments
    parser.parseArguments (argc, argv);

    // deal with python file path
    const std::string configFile = parser.stringValue("plotSet");
    const std::string dPath      = "/src/histProduce/histProduce/python/";
    const std::string confPython = sysPath+dPath+configFile+"_cfi.py";
    const edm::ParameterSet& ioOption = edm::readPSetsFrom(confPython.c_str())->getParameter<edm::ParameterSet>("process");
    const edm::ParameterSet& generalSet = ioOption.getParameter<edm::ParameterSet>("generalSet");
    const edm::ParameterSet& legendSet = ioOption.getParameter<edm::ParameterSet>("LegendSet");
    const std::vector<edm::ParameterSet>& privateSet = ioOption.getParameter< std::vector<edm::ParameterSet> >("plotSet");

    int pSize = privateSet.size();
    std::vector<plotPrivateSet> plotSetting;
    plotSetting.reserve( pSize );
    for ( const edm::ParameterSet& _set : privateSet )
        plotSetting.emplace_back(
                _set.getParameter< std::string >( "NameInTree" ) ,
                _set.getParameter< std::string >( "TitleName" ),
                _set.getParameter< double >( "ScaleFactor" ),
                _set.getParameter< bool >( "SetFill" ),
                _set.getParameter< int >( "LineColor" ),
                _set.getParameter< int >( "LineWidth" ),
                _set.getParameter< int >( "FillColor" ),
                _set.getParameter< int >( "FillStyle" )
                );
    const std::string& plotTitle = generalSet.getParameter<std::string>("plotTitle");
    // parser setting end }}}

    // input files
    std::vector<std::string> inputFiles_;
    if ( parser.stringValue("testFile").empty() )
    {
        printf( "you need to put testFile, use --help to get more option!\n" );
        exit(128);
        //return 128;
    }


    TFile* inFile = TFile::Open( ( "file://"+parser.stringValue("testFile") ).c_str() );
    if ( !inFile ) 
    {
        printf("testFile not found!\n");
        return 128;
    }


    TCanvas* cc = new TCanvas( ("canvas."+ generalSet.getParameter<std::string>("plotTitle") ).c_str(), "", 1600, 1000 );
    
    TH1D* h[ pSize ];
    for ( int i=0; i<pSize; ++i )
    {
        h[i] = (TH1D*) inFile->Get( (generalSet.getParameter<std::string>("defaultTree")+"/"+plotSetting[i].nameInTree).c_str() );
    }
    
    int ymin = 999;
    int ymax = 0;
    // get y axis range {{{
    if ( generalSet.getParameter<int>("YaxisMin") == -1 )
    {
        for ( int i=0; i<pSize; ++i )
        {
            int val = h[i]->GetMinimum();
            if ( ymin > val )
                ymin = val;
        }
    }
    else 
        ymin = generalSet.getParameter<int>("YaxisMin");
    if ( generalSet.getParameter<int>("YaxisMax") == -1 )
    {
        for ( int i=0; i<pSize; ++i )
        {
            double val = h[i]->GetMaximum();
            if ( ymax < val )
                ymax = val;
        }
    }
    else 
        ymax = generalSet.getParameter<int>("YaxisMax");
    ymax = ymax * 1.2;
    // get y axis range }}}
    
    for ( int i=0; i<pSize; ++i )
    {
        h[i]->SetTitle( plotTitle.c_str() );
        h[i]->SetStats(kFALSE);
        h[i]->SetLineWidth( plotSetting[i].lineWidth );
        h[i]->SetLineColor( plotSetting[i].lineColor );
        if ( plotSetting[i].setFill )
        {
            h[i]->SetFillColor( plotSetting[i].fillColor );
            h[i]->SetFillStyle( plotSetting[i].fillStyle );
        }
        h[i]->GetXaxis()->SetRangeUser( generalSet.getParameter<double>("XaxisMin"), generalSet.getParameter<double>("XaxisMax") );
        h[i]->GetXaxis()->SetTitle( generalSet.getParameter<std::string>("XaxisName").c_str() );
        h[i]->GetYaxis()->SetTitle( generalSet.getParameter<std::string>("YaxisName").c_str() );
        if ( plotSetting[i].scaleFactor != 1 )
        {
            h[i]->Scale( plotSetting[i].scaleFactor );
        }
        else
        {
            h[i]->GetYaxis()->SetRangeUser( ymin, ymax );
        }
        if ( i == 0 )
            h[i]->Draw();
        else
            h[i]->Draw("same");
    }
    
    TLegend* leg = new TLegend(
            legendSet.getParameter<double>("xLeft"),
            legendSet.getParameter<double>("yLeft"),
            legendSet.getParameter<double>("xRight"),
            legendSet.getParameter<double>("yRight"),
            legendSet.getParameter<std::string>("Title").c_str(),
            "NDC"
            );
    for ( int i=0;i<pSize; ++i )
        leg->AddEntry( h[i], plotSetting[i].title.c_str(), "lepf" );
    leg->SetBorderSize(0);
    leg->SetTextFont( 43 );
    leg->SetTextSize( legendSet.getParameter<int>("FontSize") );
    leg->Draw();
    
    std::string outputName = "hMerged_"+plotTitle;
    cc->SaveAs( (outputName+generalSet.getParameter<std::string>("outFormat")).c_str() );
    
    delete cc;
}
//    return 0;
//}
