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


struct plotPrivateSet
{
    std::string nameInTree;
    std::string title;
    bool setFill;
    int lineColor; int lineWidth;
    int fillColor; int fillStyle;
    std::string xTitle;
    double xMin; double xMax;
    std::string yTitle;
    int yMin; int yMax;
    double lLineXvalue; double rLineXvalue;
    

    plotPrivateSet ( 
            const std::string& n, const std::string& t, 
            bool f, int lC, int lW, int fC, int fS,
            const std::string& xT, double xm, double xM,
            const std::string& yT, int    ym, int    yM,
            double lL, double rL 
            ) :
        nameInTree ( n ), title ( t ), 
        setFill( f ), lineColor( lC ), lineWidth( lW ), fillColor( fC ), fillStyle( fS ),
        xTitle(xT), xMin(xm), xMax(xM),
        yTitle(yT), yMin(ym), yMax(yM),
        lLineXvalue(lL), rLineXvalue(rL)
    {}
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
    parser.addOption("plotSet",optutl::CommandLineParser::kString,"input the python file records parameter to plot","hSinglePar");

    // parse arguments
    parser.parseArguments (argc, argv);

    // deal with python file path
    const std::string configFile = parser.stringValue("plotSet");
    const std::string dPath      = "/src/histProduce/histProduce/python/";
    const std::string confPython = sysPath+dPath+configFile+"_cfi.py";
    const edm::ParameterSet& ioOption = edm::readPSetsFrom(confPython.c_str())->getParameter<edm::ParameterSet>("process");
    const edm::ParameterSet& generalSet = ioOption.getParameter<edm::ParameterSet>("generalSet");
    const std::vector<edm::ParameterSet>& privateSet = ioOption.getParameter< std::vector<edm::ParameterSet> >("plotSet");

    int pSize = privateSet.size();
    std::vector<plotPrivateSet> plotSetting;
    plotSetting.reserve( pSize );
    for ( const edm::ParameterSet& _set : privateSet )
        plotSetting.emplace_back(
                _set.getParameter< std::string >( "NameInTree" ) ,
                _set.getParameter< std::string >( "TitleName" ),
                _set.getParameter< bool >( "SetFill" ),
                _set.getParameter< int >( "LineColor" ),
                _set.getParameter< int >( "LineWidth" ),
                _set.getParameter< int >( "FillColor" ),
                _set.getParameter< int >( "FillStyle" ),
                _set.getParameter< std::string >( "XaxisTitle" ),
                _set.getParameter< double >( "XaxisMin" ),
                _set.getParameter< double >( "XaxisMax" ),
                _set.getParameter< std::string >( "YaxisTitle" ),
                _set.getParameter< int >( "YaxisMin" ),
                _set.getParameter< int >( "YaxisMax" ),
                _set.getParameter< double >( "LeftLineX" ),
                _set.getParameter< double >( "RightLineX" )
                );

    // parser setting end }}}

    // input files
    std::vector<std::string> inputFiles_;
    if ( parser.stringValue("testFile").empty() )
    {
        printf( "you need to put testFile, use --help to get more option!\n" );
        exit(128);
        return 128;
    }


    TFile* inFile = TFile::Open( ( "file://"+parser.stringValue("testFile") ).c_str() );
    if ( !inFile ) 
    {
        printf("testFile not found!\n");
        return 128;
    }


    for ( const plotPrivateSet& histSet : plotSetting )
    {
        TCanvas* cc = new TCanvas( ("canvas."+histSet.nameInTree).c_str(), "", 1600, 1000 );
        // testZone
        cc->SetFillColor(0);
        cc->SetBorderMode(0);
        cc->SetBorderSize(2);
        cc->SetTickx(0);
        cc->SetTicky(0);
        cc->SetLeftMargin(0.10);
        cc->SetRightMargin(0.05);
        cc->SetTopMargin(0.1);
        cc->SetBottomMargin(0.10);
        cc->SetFrameBorderMode(0);
        cc->SetFrameBorderMode(0);
        
        TH1D* h;
            h = (TH1D*) inFile->Get( (generalSet.getParameter<std::string>("defaultTree")+"/"+histSet.nameInTree).c_str() );
    
        int ymin = 999;
        int ymax = 0;
        // get y axis range {{{
        if ( histSet.yMin == -1 )
        {
            int val = h->GetMinimum();
            if ( ymin > val )
                ymin = val;
        }
        else 
            ymin = histSet.yMin;
        if ( histSet.yMax == -1 )
        {
            double val = h->GetMaximum();
            if ( ymax < val )
                ymax = val;
        }
        else 
            ymax = histSet.yMax;
        ymax = ymax * 1.2;
        // get y axis range }}}
    
        h->SetTitle( histSet.title.c_str() );
        //h->SetStats(kFALSE);
        h->SetLineWidth( histSet.lineWidth );
        h->SetLineColor( histSet.lineColor );
        if ( histSet.setFill )
        {
            h->SetFillColor( histSet.fillColor );
            h->SetFillStyle( histSet.fillStyle );
        }
        h->GetXaxis()->SetRangeUser( histSet.xMin, histSet.xMax );
        h->GetXaxis()->SetTitle( histSet.xTitle.c_str() );
        h->GetYaxis()->SetRangeUser( ymin, ymax );
        h->GetYaxis()->SetTitle( histSet.yTitle.c_str() );
        // test zone 2
        h->GetXaxis()->SetLabelSize(0.03);
        h->GetXaxis()->SetTitleSize(0.03);
        h->GetXaxis()->SetTitleOffset(1.0);
        h->GetYaxis()->SetLabelSize(0.03);
        h->GetYaxis()->SetTitleSize(0.03);
        h->GetYaxis()->SetTitleOffset(1.4);
        h->Draw();
        cc->Update();
    
   

        TLine* lLeft = new TLine( histSet.lLineXvalue, cc->GetUymin(), histSet.lLineXvalue, cc->GetUymax() );
        lLeft->SetLineWidth(1);
        lLeft->SetLineColor(1);
        lLeft->SetLineStyle(2);
        lLeft->Draw();
        TLine* lRight = new TLine( histSet.rLineXvalue, cc->GetUymin(), histSet.rLineXvalue, cc->GetUymax() );
        lRight->SetLineWidth(1);
        lRight->SetLineColor(1);
        lRight->SetLineStyle(2);
        lRight->Draw();

        std::string outputName = "hSinglePlot_"+histSet.nameInTree;
        cc->SaveAs( (outputName+generalSet.getParameter<std::string>("outFormat")).c_str() );
    
        delete cc;
        delete lLeft;
        delete lRight;
        delete h;
    }
    return 0;
}
