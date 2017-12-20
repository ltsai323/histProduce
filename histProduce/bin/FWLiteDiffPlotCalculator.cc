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
#include <TGaxis.h>

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

// based on FWLiteMergePlots.cc
// calculate the difference between two histogram.
// That is use to calculate different cut, comparing to first cut, the second cut cuts out how much background?
//
// usage:
//     hDiff plotset=histoMergePar testfile=mySource.root


struct plotPrivateSet
{
    std::string preNameTar;
    std::string preNameObj;
    std::string title;
    bool setFill;
    int lineColor;
    int lineWidth;
    int fillColor;
    int fillStyle;
    plotPrivateSet ( const std::string& n1, const std::string& n2, const std::string& t, bool sF, int lC, int lW, int fC, int fS ) :
        preNameTar ( n1 ), preNameObj ( n2 ), title ( t ), setFill( sF ), lineColor( lC ), lineWidth( lW ), fillColor( fC ), fillStyle( fS ) {}
    void show() const
    {
        printf( "preNameTar = %s, PreNameObj = %s, title = %s\nsetFill = %d\nlineColor = %d, lineWidth = %d\nfillColor = %d, fillStyle = %d\n",
                preNameTar.c_str(), preNameObj.c_str(), title.c_str(), setFill, lineColor, lineWidth, fillColor, fillStyle );
    }
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
    parser.addOption("plotSet",optutl::CommandLineParser::kString,"input the python file records parameter to plot","histoDiffPar");

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

    //const std::vector<std::string>& mergedPlots = generalSet.getParameter<std::vector<std::string>>("mergedPlots");
    const std::vector<edm::ParameterSet>& mergedPlots = generalSet.getParameter<std::vector<edm::ParameterSet>>("mergedPlots");
    int pSize = privateSet.size();
    std::vector<plotPrivateSet> plotSetting;
    plotSetting.reserve( pSize );
    for ( const edm::ParameterSet& _set : privateSet )
        plotSetting.emplace_back(
                _set.getParameter< std::string >( "PreNameTar" ) ,
                _set.getParameter< std::string >( "PreNameObj" ) ,
                _set.getParameter< std::string >( "TitleName" ),
                _set.getParameter< bool >( "SetFill" ),
                _set.getParameter< int >( "LineColor" ),
                _set.getParameter< int >( "LineWidth" ),
                _set.getParameter< int >( "FillColor" ),
                _set.getParameter< int >( "FillStyle" )
                );

    // parser setting end }}}

    // input files
    std::vector<std::string> inputFiles_;
    if ( parser.stringValue("testFile").empty() )
    {
        printf( "you need to put testFile, use --help to get more option!\n" );
        exit(128);
    }


    TFile* inFile = TFile::Open( ( "file://"+parser.stringValue("testFile") ).c_str() );
    if ( !inFile ) 
    {
        printf("testFile not found!\n");
        exit(128);
    }


    TGaxis::SetMaxDigits(3);
    for ( const edm::ParameterSet& mergedObj : mergedPlots )
    {
        const std::string& mergedName = mergedObj.getParameter<std::string>("MergedName");
        TCanvas* cc = new TCanvas( ("canvas."+mergedName).c_str(), "", 1600, 1000 );
        
        TH1D* hTar[ pSize ] = { nullptr };
        TH1D* hObj[ pSize ] = { nullptr };
        for ( int i=0; i<pSize; ++i )
        {
            hTar[i] = (TH1D*)(  inFile->Get( (generalSet.getParameter<std::string>("defaultTree")+"/"+plotSetting[i].preNameTar+mergedName).c_str() )  )->Clone();
            hObj[i] = (TH1D*)(  inFile->Get( (generalSet.getParameter<std::string>("defaultTree")+"/"+plotSetting[i].preNameObj+mergedName).c_str() )  )->Clone();
            hObj[i]->Add( hTar[i], -1. );
        }
    
        int ymin = 999;
        int ymax = 0;
        // get y axis range {{{
        if ( generalSet.getParameter<int>("YaxisMin") == -1 )
        {
            for ( int i=0; i<pSize; ++i )
            {
                int val = hObj[i]->GetMinimum();
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
                double val = hObj[i]->GetMaximum();
                if ( ymax < val )
                    ymax = val;
            }
        }
        else 
            ymax = generalSet.getParameter<int>("YaxisMax");
        ymax = ymax * 1.6;
        // get y axis range }}}
    
        for ( int i=0; i<pSize; ++i )
        {
            // wanted title : "mass_Bs using vtxprob sorging"
            hObj[i]->SetTitle( ( mergedName+generalSet.getParameter<std::string>("ComplementTitle") ).c_str() );
            hObj[i]->SetStats(kFALSE);
            hObj[i]->SetLineWidth( plotSetting[i].lineWidth );
            hObj[i]->SetLineColor( plotSetting[i].lineColor );
            if ( plotSetting[i].setFill )
            {
                hObj[i]->SetFillColor( plotSetting[i].fillColor );
                hObj[i]->SetFillStyle( plotSetting[i].fillStyle );
            }
            hObj[i]->GetXaxis()->SetRangeUser( generalSet.getParameter<double>("XaxisMin"), generalSet.getParameter<double>("XaxisMax") );
            hObj[i]->GetXaxis()->SetTitle( generalSet.getParameter<std::string>("XaxisName").c_str() );
            hObj[i]->GetYaxis()->SetRangeUser( ymin, ymax );
            hObj[i]->GetYaxis()->SetTitle( generalSet.getParameter<std::string>("YaxisName").c_str() );
            if ( i == 0 )
                hObj[i]->Draw();
            else
                hObj[i]->Draw("same");
        }
    
    
        TLine* lLeft  = nullptr;
        TLine* lRight = nullptr;
        double lmin = mergedObj.getParameter<double>("SignalRegionMin");
        double lmax = mergedObj.getParameter<double>("SignalRegionMax");
        if ( lmin > 0. )
        {
            //lLeft = new TLine( lmin, cc->GetUymin(), lmin, cc->GetUymax() );
            lLeft = new TLine( lmin, ymin, lmin, ymax );
            lLeft->SetLineWidth(1);
            lLeft->SetLineColor(1);
            lLeft->SetLineStyle(2);
            lLeft->Draw();
        }
        if ( lmax > 0. )
        {
            lRight = new TLine( lmax, ymin, lmax, ymax );
            lRight->SetLineWidth(1);
            lRight->SetLineColor(1);
            lRight->SetLineStyle(2);
            lRight->Draw();
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
            leg->AddEntry( hObj[i], plotSetting[i].title.c_str(), "lepf" );
        if ( legendSet.getParameter<bool>("SetTransparent") )
        {
            leg->SetFillColor(4000);
            leg->SetFillStyle(4000);
        }
        leg->SetBorderSize(0);
        leg->SetTextFont( 43 );
        leg->Draw();
        std::string outputName = "hDiff_"+mergedName;
        cc->SaveAs( (outputName+generalSet.getParameter<std::string>("outFormat")).c_str() );
    
        delete cc;
        delete leg;
        delete lLeft;
        delete lRight;
        for ( int i=0;i<pSize;++i )
        {
            delete hTar[i];
            delete hObj[i];
        }

    }
    return 0;
}
