#include <iostream>
#include <vector>
#include <utility>
#include <string>
#include <cstdlib>
#include <stdlib.h>

#include "TTree.h"
#include "TFile.h"
#include "TH1D.h"
#include "TNtupleD.h"
#include "TCanvas.h"

#include "histProduce/histProduce/interface/formatTreeLbTknew.h"
#include "histProduce/histProduce/interface/TriggerBooking.h"

typedef formatTree_newLbTk readData;

//typedef root_TreeHistoMain_GenInfo_plusminus_LbTk readMC;#defineuyiytiyufytHLT_DoubleMu4_JpsiTrk_Displaced
//static const int HLTSELECTED = HLTList::HLT_DoubleMu4_Jpsi_Displaced;
static const int HLTSELECTED = HLTList::HLT_Dimuon16_Jpsi;
static const char* OUTPUTFILENAME = "smallNTupleVertexProducer_pLbTk.root";

//static const char* TREENAME = "VertexCompCandAnalyzer/pLbTk";
static const char* TREENAME = "treeCreatingSpecificDecay/pLbTk";

//#define HLTSELECTED HLT_DoubleMu4_Jpsi_Displaced;
//#define OUTPUTFILENAME "outputFileName";
//#define TREENAME #VertexCompCandAnalyzer/pLbTk;

//std::vector<std::string> treeNames = { "pLbTk", "nLbTk", "pLbL0", "nLbL0" };

int main()
{
	// settings {{{
	std::vector<double> gaRes;
	gaRes = { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0, 0., 0., 0., 0., 0. };

	std::vector< std::pair<std::string, std::string> > fileNameLabels;

	//fileNameLabels.emplace_back("/home/ltsai/Work/LbFrame/workspace/storeroot/tree_vertexProducer_2016RunBCDEFGH_HLTRecord.root", "2016Data");
	fileNameLabels.emplace_back("/home/ltsai/Work/LbFrame/workspace/test.root", "test");

	//fileNameLabels.push_back("storeroot/tReduced/tree_forGA_removeBsBdOnly/data_2017RunBCDEF.root", "2017Data");

	//const double Nsep = 14;
	//const double massStep = 0.05;
	//const double mjpsipStart = 4.1;

	// settings end }}}

	TFile* tmpFile = new TFile("/tmp/ltsai/tmpFile.root", "RECREATE");
	TCanvas* c1 = new TCanvas("c1","",1600,1000);
	c1->SaveAs("storefig/resultHisto.pdf[");

	for ( const auto& fileNameLabel : fileNameLabels )
	{
		const std::string& fName  = fileNameLabel.first;
		const std::string& fLabel = fileNameLabel.second;

		TFile* dataFile = TFile::Open(fName.c_str());
		TTree* dataTree = (TTree*) dataFile->Get(TREENAME);

		readData data;
		data.LoadFormatSourceBranch( dataTree );

		TNtupleD* ntData = new TNtupleD( ("nt_"+fLabel).c_str(), ("ntuple for "+fLabel).c_str(), "lbtkMass:lbtkPt:lbtkFlightDistanceSig:lbtkVtxprob:tktkMass:tktkPt:tktkFlightDistanceSig:tktkVtxprob:tktkCosa2d:pmuPt:nmuPt:tk1Pt:tk2Pt" );
		ntData->SetDirectory(tmpFile);

		unsigned i = 0;
		unsigned N = dataTree->GetEntries();
		unsigned hltCounter = 0;
		unsigned eventCounter = 0;

		// testing
		unsigned iTrigVanish = 0;
		unsigned nTrigVanish = 0;
		unsigned iTrigNotRun = 0;
		unsigned nTrigNotRun = 0;
		unsigned iTrigReject = 0;
		unsigned nTrigReject = 0;
		unsigned iTrigError  = 0;
		unsigned nTrigError  = 0;
		unsigned iTotallyTriggered = 0;
		unsigned nTotallyTriggered = 0;

		// tested
		while ( i != N )
		{
			dataTree->GetEntry(i++);
			++eventCounter;

			//if ( !HLTList::decodeHLT(data.readI[readData::totallyTriggered], HLTSELECTED) ) continue;
			++nTrigVanish;       if ( HLTList::hasHLT(data.readI[readData::trigVanish], HLTSELECTED) ) ++iTrigVanish;
			++nTrigNotRun;       if ( HLTList::hasHLT(data.readI[readData::trigNotRun], HLTSELECTED) ) ++iTrigNotRun;
			++nTrigReject;       if ( HLTList::hasHLT(data.readI[readData::trigReject], HLTSELECTED) ) ++iTrigReject;
			++nTrigError;        if ( HLTList::hasHLT(data.readI[readData::trigError ], HLTSELECTED) ) ++iTrigError;
			++nTotallyTriggered; if ( HLTList::hasHLT(data.readI[readData::totallyTriggered], HLTSELECTED) ) ++iTotallyTriggered;

			if ( data.readD[readData::lbtkFlightDistanceSig] < gaRes[ 0] ) continue;
			if ( data.readD[readData::lbtkVtxprob          ] < gaRes[ 1] ) continue;
			if ( data.readD[readData::lbtkPt               ] < gaRes[ 2] ) continue;
			if ( data.readD[readData::tktkPt               ] < gaRes[ 4] ) continue;
			if ( data.readD[readData::tk1Pt                ] < gaRes[ 6] ) continue;
			if ( data.readD[readData::tk2Pt                ] < gaRes[ 8] ) continue;

			ntData->Fill(
				data.readD[readData::lbtkMass], data.readD[readData::lbtkPt], data.readD[readData::lbtkFlightDistanceSig], data.readD[readData::lbtkVtxprob],
				data.readD[readData::tktkMass], data.readD[readData::tktkPt], data.readD[readData::tktkFlightDistanceSig], data.readD[readData::tktkVtxprob], data.readD[readData::tktkCosa2d],
				data.readD[readData::pmuPt], data.readD[readData::nmuPt], data.readD[readData::tk1Pt], data.readD[readData::tk2Pt]
				);
		}

		dataFile->Close();
		delete ntData;
		printf("this file %s with %d/%d event passed HLT\n", fName.c_str(),hltCounter,eventCounter);
		printf("HLT results:\n");
		printf("\ttrigVanish : %d/%d\n", iTrigVanish,nTrigVanish);
		printf("\ttrigNotRun : %d/%d\n", iTrigNotRun,nTrigNotRun);
		printf("\ttrigReject : %d/%d\n", iTrigReject,nTrigReject);
		printf("\ttrigError  : %d/%d\n", iTrigError,nTrigError );
		printf("\ttrigPassed : %d/%d\n", iTotallyTriggered,nTotallyTriggered);
	}	// for loop for fileNameLabel

	delete c1;
	tmpFile->Write();
	tmpFile->Close();
	delete tmpFile;

	char mvCommand[256];
	sprintf( mvCommand, "mv /tmp/ltsai/tmpFile.root %s", OUTPUTFILENAME );
	system(mvCommand);
}
