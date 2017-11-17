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
#include <TLatex.h>

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

// merge plots with the same last name, 
// like: fd0S_myMassLb, fd1S_myMassLb, ...
// Once you execute this, you can get lots of plots.
//
// usage:
//     hMerge plotset=histoMergePar testfile=mySource.root


struct secondaryPlotSettingRecorder
{
    std::string preName;
    std::string title;
    bool setFill;
    int lineColor;
    int lineWidth;
    int fillColor;
    int fillStyle;
    secondaryPlotSettingRecorder( const std::string& n, const std::string& t, bool sF, int lC, int lW, int fC, int fS ) :
        preName ( n ), title ( t ), setFill( sF ), lineColor( lC ), lineWidth( lW ), fillColor( fC ), fillStyle( fS ) {}
};
struct mergedPlotsRecorder
{
    std::string mergedName;
    std::string plotsTitle;
    double lmin; double lmax; 
    std::string xname; double xmin; double xmax;
    std::string yname; double ymin; double ymax;
    mergedPlotsRecorder( const std::string& mN, const std::string& pT, double lm, double lM, const std::string& xN, double xm, double xM, const std::string& yN, double ym, double yM ) :
        mergedName( mN ), plotsTitle( pT ), lmin( lm ), lmax( lM ), xname( xN ), xmin( xm ), xmax( xM ), yname( yN ), ymin( ym ), ymax( yM ) {}
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
    parser.addOption("plotSet",optutl::CommandLineParser::kString,"input the python file records parameter to plot","histoMergePar");

    // parse arguments
    parser.parseArguments (argc, argv);

    // deal with python file path
    const std::string configFile = parser.stringValue("plotSet");
    const std::string dPath      = "/src/histProduce/histProduce/python/";
    const std::string confPython = sysPath+dPath+configFile+"_cfi.py";
    const edm::ParameterSet& ioOption = edm::readPSetsFrom(confPython.c_str())->getParameter<edm::ParameterSet>("process");
    const edm::ParameterSet& generalSet = ioOption.getParameter<edm::ParameterSet>("GeneralSet");
    const edm::ParameterSet& legendSet = ioOption.getParameter<edm::ParameterSet>("LegendSet");
    const std::vector<edm::ParameterSet>& privateSet = ioOption.getParameter< std::vector<edm::ParameterSet> >("SecondaryPlotSetting");

    const std::vector<edm::ParameterSet>& mergedPlotFromPython = ioOption.getParameter<std::vector<edm::ParameterSet>>("MergedPlots");
    int pSize = privateSet.size();
    std::vector<secondaryPlotSettingRecorder> plotSetting;
    plotSetting.reserve( pSize );
    for ( const edm::ParameterSet& _set : privateSet )
        plotSetting.emplace_back(
                _set.getParameter< std::string >( "PreName" ) ,
                _set.getParameter< std::string >( "TitleName" ),
                _set.getParameter< bool >( "SetFill" ),
                _set.getParameter< int >( "LineColor" ),
                _set.getParameter< int >( "LineWidth" ),
                _set.getParameter< int >( "FillColor" ),
                _set.getParameter< int >( "FillStyle" )
                );
    std::vector<mergedPlotsRecorder> mergedPlots;
    mergedPlots.reserve( mergedPlotFromPython.size() );
    for ( const edm::ParameterSet& _set : mergedPlotFromPython )
        mergedPlots.emplace_back(
                _set.getParameter< std::string >( "MergedName" ),
                _set.getParameter< std::string >( "PlotsTitle" ),
                _set.getParameter< double      >( "SRegionMin" ),
                _set.getParameter< double      >( "SRegionMax" ),
                _set.getParameter< std::string >( "XaxisTitle" ),
                _set.getParameter< double      >( "XaxisMin" ),
                _set.getParameter< double      >( "XaxisMax" ),
                _set.getParameter< std::string >( "YaxisTitle" ),
                _set.getParameter< double      >( "YaxisMin" ),
                _set.getParameter< double      >( "YaxisMax" )
                );


    // parser setting end }}}
    printf("load parameter sucessful\n");
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
    printf("load file sucessful\n");


    TGaxis::SetMaxDigits(3);
    for ( const mergedPlotsRecorder& mergedObj : mergedPlots )
    {
        TCanvas* cc = new TCanvas( ("canvas."+mergedObj.mergedName).c_str(), "", 1600, 1000 );
        cc->cd();
        
        TH1D* h[ pSize ];
        for ( int i=0; i<pSize; ++i )
            h[i] = (TH1D*) inFile->Get( (generalSet.getParameter<std::string>("defaultTree")+"/"+plotSetting[i].preName+mergedObj.mergedName).c_str() );
    
        int ymin = 999;
        int ymax = 0;
        // get y axis range {{{
        if ( mergedObj.ymin == -1 )
        {
            for ( int i=0; i<pSize; ++i )
            {
                int val = h[i]->GetMinimum();
                if ( ymin > val )
                    ymin = val;
            }
        }
        else 
            ymin = mergedObj.ymin;
        if ( mergedObj.ymax == -1 )
        {
            for ( int i=0; i<pSize; ++i )
            {
                double val = h[i]->GetMaximum();
                if ( ymax < val )
                    ymax = val;
            }
        }
        else 
            ymax = mergedObj.ymax;
        ymax = ymax * 1.4;
        // get y axis range }}}
    
        for ( int i=0; i<pSize; ++i )
        {
            h[i]->SetTitle( mergedObj.plotsTitle.c_str() );
            h[i]->SetStats(kFALSE);
            h[i]->SetLineWidth( plotSetting[i].lineWidth );
            h[i]->SetLineColor( plotSetting[i].lineColor );
            if ( plotSetting[i].setFill )
            {
                h[i]->SetFillColor( plotSetting[i].fillColor );
                h[i]->SetFillStyle( plotSetting[i].fillStyle );
            }
            h[i]->GetXaxis()->SetRangeUser( mergedObj.xmin, mergedObj.xmax );
            h[i]->GetXaxis()->SetTitle( mergedObj.xname.c_str() );
            h[i]->GetYaxis()->SetRangeUser( ymin, ymax );
            h[i]->GetYaxis()->SetTitle( mergedObj.yname.c_str() );
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
        if ( legendSet.getParameter<bool>("SetTransparent") )
        {
            leg->SetFillColor(4000);
            leg->SetFillStyle(4000);
        }
        leg->SetBorderSize(0);
        leg->SetTextFont(43);
        leg->Draw();
    
        TLine* lLeft  = nullptr;
        TLine* lRight = nullptr;
        double lmin = mergedObj.lmin;
        double lmax = mergedObj.lmax;
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
        std::string outputName = "hMerged_"+mergedObj.mergedName;
        cc->SaveAs( (outputName+generalSet.getParameter<std::string>("outFormat")).c_str() );
    
        delete cc;
        delete lLeft;
        delete lRight;
    }
    inFile->Close();
    return 0;
}
