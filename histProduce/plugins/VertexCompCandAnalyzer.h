// -*- C++ -*-

//
// Package:    V0Producer
// Class:      V0Producer
//

/**\class V0Producer V0Producer.h RecoVertex/V0Producer/interface/V0Producer.h

   Description: <one line class summary>

   Implementation:
   <Notes on implementation>
 */

//
// Original Author:  Brian Drell
//         Created:  Fri May 18 22:57:40 CEST 2007
// $Id: V0Producer.h,v 1.7 2009/03/10 22:58:47 drell Exp $
//
//

#ifndef __VertexCompCandAnalyzer_H__
#define __VertexCompCandAnalyzer_H__

// system include files
#include <memory>

// user include files

#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "DataFormats/Candidate/interface/VertexCompositeCandidate.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

//#include "DataFormats/V0Candidate/interface/V0Candidate.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

//#include "vertexProducer/vertexProducer/interface/ccLoader.h"
#include "histProduce/histProduce/interface/formatTreeLbTknew.h"
#include "histProduce/histProduce/interface/formatTreeMC_fourDaughter.h"
#include "histProduce/histProduce/interface/TriggerBooking.h"
typedef formatTree_LbTknew LbTkRecord;
typedef formatTree_MC_fourDaughter MCRecord;

namespace
{
	const double muonMASS = 0.1056583715;
	const double protonMASS = 0.938272046;
	const double pionMASS = 0.13957018;
	const double kaonMASS = 0.493667;
	const double bdMASS = 5.27953;
	const double bsMASS = 5.3663;
	const double phiMASS = 1.019461;

	const double bdWIDTH = 0.5;
	const double bsWIDTH = 0.5;
	const double phiWIDTH = 0.004266;
}

class VertexCompCandAnalyzer : public edm::EDAnalyzer
{
public:
	explicit VertexCompCandAnalyzer(const edm::ParameterSet&);
	~VertexCompCandAnalyzer();

	virtual void beginJob() override;
	virtual void analyze(const edm::Event&, const edm::EventSetup&);
	virtual void endJob() override;

	static void fillDescriptions( edm::ConfigurationDescriptions& descriptions );

private:
	edm::Service<TFileService> fs;
	std::vector<TFileDirectory> dirs;

	std::string pL0BCandsLabel;
	std::string nL0BCandsLabel;
	std::string LbL0CandsLabel;
	std::string LbLoCandsLabel;
	std::string HLTRecordLabel;
	std::string MCReserveLabel;
	std::string bsPointLabel;
	edm::EDGetTokenT< reco::VertexCompositeCandidateCollection > pL0BCandsToken;
	edm::EDGetTokenT< reco::VertexCompositeCandidateCollection > nL0BCandsToken;
	edm::EDGetTokenT< reco::VertexCompositeCandidateCollection > LbL0CandsToken;
	edm::EDGetTokenT< reco::VertexCompositeCandidateCollection > LbLoCandsToken;
	edm::EDGetTokenT< edm::TriggerResults                      > HLTRecordToken;
	edm::EDGetTokenT< std::vector< reco::GenParticle >         > MCReserveToken;
	edm::EDGetTokenT< reco::BeamSpot > bsPointToken;
	bool usepL0B;
	bool usenL0B;
	bool useLbL0;
	bool useLbLo;
	bool useHLT;
	bool useMC;
	bool useBS;

	LbTkRecord pL0B, nL0B, LbTk;
	LbTkRecord LbL0, LbLo;
	MCRecord mc;

	TTree* pL0BTree;
	TTree* nL0BTree;
    TTree* LbTkTree;
	TTree* LbL0Tree;
	TTree* LbLoTree;
	TTree* mcTree;
	int entry;
};

#endif
