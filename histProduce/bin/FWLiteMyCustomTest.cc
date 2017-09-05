#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

#include <TH1F.h>
#include <TROOT.h>
#include <TFile.h>
#include <TSystem.h>

#include "DataFormats/FWLite/interface/Event.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"

#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "PhysicsTools/FWLite/interface/TFileService.h"
#include "PhysicsTools/FWLite/interface/CommandLineParser.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/GeometryVector/interface/Vector3DBase.h"


int main(int argc, char* argv[])
{
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

    // initialize command line parser
    optutl::CommandLineParser parser ("Analyze FWLite Histograms");

    // set defaults
    parser.integerValue ("maxEvents"  ) = 1000;
    parser.integerValue ("outputEvery") =  100;
    parser.stringValue  ("outputFile" ) = "myTestOutputHistogram.root";

    // parse arguments
    parser.parseArguments (argc, argv);
    int maxEvents_ = parser.integerValue("maxEvents");
    unsigned int outputEvery_ = parser.integerValue("outputEvery");
    std::string outputFile_ = parser.stringValue("outputFile");
    const std::vector<std::string>& inputFiles_ = parser.stringVector("inputFiles");

    // book a set of histograms
    fwlite::TFileService fs = fwlite::TFileService(outputFile_.c_str());
    TFileDirectory dir = fs.mkdir("lbSpecificDecay");
    TH1F* hOniaMass = dir.make<TH1F>("oniaMass", "oniaMass", 50, 2.8, 3.3 );
    TH1F* hLam0Mass = dir.make<TH1F>("lam0Mass", "lam0Mass", 50, 1.1, 1.15);
    TH1F* hTkTkMass = dir.make<TH1F>("tktkMass", "tktkMass",150, 1.0, 2.50);
    TH1F* hLbL0Mass = dir.make<TH1F>("LbL0Mass", "LbL0Mass", 50, 5.0, 6.0 );
    TH1F* hLbTkMass = dir.make<TH1F>("LbTkMass", "LbTkMass", 50, 5.0, 6.0 );
    TH1F* hBsBsMass = dir.make<TH1F>( "BsBsMass", "BsBsMass", 50, 5.0, 6.0 );
    TH1F* hPhi_Mass = dir.make<TH1F>("phi_Mass", "phi_Mass", 70, 0.8, 1.50);
    TH1F* hTkTkMass2= dir.make<TH1F>("tktkMass2", "tktkMass2", 70, 0.8, 1.50);
    TH1F* hpTkPx = dir.make<TH1F>("ptkPx", "ptkPx",150,-20., 20. );
    TH1F* hnTkPx = dir.make<TH1F>("ntkPx", "ntkPx",150,-20., 20. );

    // loop the events
    int ievt=0;
    std::cout << "inputFiles.size() : " << inputFiles_.size() << std::endl;
    for(unsigned int iFile=0; iFile<inputFiles_.size(); ++iFile)
    {
        // open input file (can be located on castor)
        TFile* inFile = TFile::Open(inputFiles_[iFile].c_str());
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
            for(ev.toBegin(); !ev.atEnd(); ++ev, ++ievt)
            {
                edm::EventBase const & event = ev;
                // break loop if maximal number of events is reached
                if(maxEvents_>0 ? ievt+1>maxEvents_ : false) break;
                //// simple event counter
                if(outputEvery_!=0 ? (ievt>0 && ievt%outputEvery_==0) : false)
                    std::cout << "  processing event: " << ievt << std::endl;
                std::vector< pat::CompositeCandidate >::const_iterator iter;
                std::vector< pat::CompositeCandidate >::const_iterator iend;

                // Handle to the muon collection
                fwlite::Handle< std::vector<pat::CompositeCandidate> > oniaHandle;
                oniaHandle.getByLabel( ev, "lbWriteSpecificDecay", "oniaFitted", "bphAnalysis" );

                // loop muon collection and fill histograms
                iter = oniaHandle->begin();
                iend = oniaHandle->end  ();
                while ( iter != iend )
                {
                    const pat::CompositeCandidate& cand = *iter++;
                    hOniaMass->Fill( cand.mass() );
                }

                
//                fwlite::Handle< std::vector<pat::CompositeCandidate> > lam0Handle;
//                lam0Handle.getByLabel( ev, "lbWriteSpecificDecay", "Lam0Cand", "bphAnalysis" );
//
//                // loop muon collection and fill histograms
//                iter = lam0Handle->begin();
//                iend = lam0Handle->end  ();
//                while ( iter != iend )
//                {
//                    const pat::CompositeCandidate& cand = *iter++;
//                    hLam0Mass->Fill( cand.mass() );
//                }
                hLam0Mass = hLam0Mass;
                hLbL0Mass = hLbL0Mass;

                fwlite::Handle< std::vector<pat::CompositeCandidate> > tktkHandle;
                tktkHandle.getByLabel( ev, "lbWriteSpecificDecay", "TkTkFitted", "bphAnalysis" );

                // loop muon collection and fill histograms
                iter = tktkHandle->begin();
                iend = tktkHandle->end  ();
                while ( iter != iend )
                {
                    const pat::CompositeCandidate& cand = *iter++;
                    if ( cand.mass() < 1.5 && cand.mass() > 0.8 )
                        hTkTkMass2->Fill( cand.mass() );
                    hTkTkMass->Fill( cand.mass() );

                }


//                fwlite::Handle< std::vector<pat::CompositeCandidate> > LbL0Handle;
//                LbL0Handle.getByLabel( ev, "lbWriteSpecificDecay", "LbToLam0Fitted", "bphAnalysis" );
//
//                // loop muon collection and fill histograms
//                iter = LbL0Handle->begin();
//                iend = LbL0Handle->end  ();
//                while ( iter != iend )
//                {
//                    const pat::CompositeCandidate& cand = *iter++;
//                    hLbL0Mass->Fill( cand.mass() );
//                }

                fwlite::Handle< std::vector<pat::CompositeCandidate> > LbTkHandle;
                LbTkHandle.getByLabel( ev, "lbWriteSpecificDecay", "LbToTkTkFitted", "bphAnalysis" );

                // loop muon collection and fill histograms
                iter = LbTkHandle->begin();
                iend = LbTkHandle->end  ();
                while ( iter != iend )
                {
                    const pat::CompositeCandidate& cand = *iter++;
                    hLbTkMass->Fill( cand.mass() );
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
                    if ( cand.hasUserData("TkTk/KaonPos.fitMom") && cand.hasUserData("TkTk/KaonNeg.fitMom") )
                    {
                        const Vector3DBase<float,GlobalTag>* ptkMom = cand.userData<Vector3DBase<float,GlobalTag>>("TkTk/KaonPos.fitMom");
                        const Vector3DBase<float,GlobalTag>* ntkMom = cand.userData<Vector3DBase<float,GlobalTag>>("TkTk/KaonNeg.fitMom");
                        hpTkPx->Fill( ptkMom->x() );
                        hnTkPx->Fill( ntkMom->x() );
                    }
                    else printf("there is not tktk secondary vertex daughter recorded!\n");
                }
                    fwlite::Handle< std::vector<pat::CompositeCandidate> > BsBsHandle;
                    BsBsHandle.getByLabel( ev, "lbWriteSpecificDecay", "bsFitted", "bphAnalysis" );
    
                    // loop muon collection and fill histograms
                    iter = BsBsHandle->begin();
                    iend = BsBsHandle->end  ();
                    while ( iter != iend )
                    {
                        const pat::CompositeCandidate& cand = *iter++;
                        hBsBsMass->Fill( cand.mass() );
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
                fwlite::Handle< std::vector<pat::CompositeCandidate> > phi_Handle;
                phi_Handle.getByLabel( ev, "lbWriteSpecificDecay", "phiCand", "bphAnalysis" );

                // loop muon collection and fill histograms
                iter = phi_Handle->begin();
                iend = phi_Handle->end  ();
                while ( iter != iend )
                {
                    const pat::CompositeCandidate& cand = *iter++;
                    if ( cand.mass() < 1.5 && cand.mass() > 0.8 )
                    hPhi_Mass->Fill( cand.mass() );
                }
            }
            // close input file
            inFile->Close();
        }
        // break loop if maximal number of events is reached:
        // this has to be done twice to stop the file loop as well
        if(maxEvents_>0 ? ievt+1>maxEvents_ : false) break;
    }
    return 0;
}
