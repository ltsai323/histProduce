// -*- C++ -*-

//
// Package:    V0Producer
// Class:      V0Producer
//

/**\class V0Producer V0Producer.cc MyProducers/V0Producer/src/V0Producer.cc

   Description: <one line class summary>

   Implementation:
     <Notes on implementation>
 */

//
// Original Author:  Brian Drell
//         Created:  Fri May 18 22:57:40 CEST 2007
// $Id: V0Producer.cc,v 1.11 2009/12/18 20:45:10 wmtan Exp $
//
//

#include "DataFormats/PatCandidates/interface/Muon.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "CommonTools/Statistics/interface/ChiSquaredProbability.h"

#include "DataFormats/RecoCandidate/interface/RecoChargedCandidate.h"
#include "RecoVertex/VertexTools/interface/VertexDistanceXY.h"
#include "TMath.h"
#include "Math/VectorUtil.h"
#include "TVector3.h"

#include <TH1.h>
#include <TFile.h>
#include <TLorentzVector.h>

// system include files
#include <set>
#include <string>
#include <iostream>
#include <fstream>
#include <math.h>
#include <stdio.h>

#include <memory>
#include <string>

#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "histProduce/histProduce/plugins/VertexCompCandAnalyzer.h"
using namespace std;

// Constructor
VertexCompCandAnalyzer::VertexCompCandAnalyzer(const edm::ParameterSet& iConfig) :
	pL0BCandsLabel(iConfig.getParameter<std::string>("pL0BCandsLabel")),
	nL0BCandsLabel(iConfig.getParameter<std::string>("nL0BCandsLabel")),
	LbL0CandsLabel(iConfig.getParameter<std::string>("LbL0CandsLabel")),
	LbLoCandsLabel(iConfig.getParameter<std::string>("LbLoCandsLabel")),
	HLTRecordLabel(iConfig.getParameter<std::string>("HLTRecordLabel")),
	MCReserveLabel(iConfig.getParameter<std::string>("MCReserveLabel")),
	bsPointLabel(iConfig.getParameter<std::string>(  "bsPointLabel")),
	pL0BCandsToken(consumes < vector < reco::VertexCompositeCandidate > >(pL0BCandsLabel) ),
	nL0BCandsToken(consumes < vector < reco::VertexCompositeCandidate > >(nL0BCandsLabel) ),
	LbL0CandsToken(consumes < vector < reco::VertexCompositeCandidate > >(LbL0CandsLabel) ),
	LbLoCandsToken(consumes < vector < reco::VertexCompositeCandidate > >(LbLoCandsLabel) ),
	HLTRecordToken(consumes < edm::TriggerResults                       >(HLTRecordLabel) ),
	MCReserveToken(consumes < vector < reco::GenParticle >              >(MCReserveLabel) ),
	bsPointToken(consumes < reco::BeamSpot > (bsPointLabel) )
{
	usepL0B = (pL0BCandsLabel != "");
	usenL0B = (nL0BCandsLabel != "");
	useLbL0 = (LbL0CandsLabel != "");
	useLbLo = (LbLoCandsLabel != "");
	useHLT  = (HLTRecordLabel != "");
	useMC   = (MCReserveLabel != "");
	useBS   = (  bsPointLabel != "");

    LbTkTree = fs->make < TTree > ("LbTkTot", "LbTkTot");
	pL0BTree = fs->make < TTree > ("pLbTk", "pLbTk");
	nL0BTree = fs->make < TTree > ("nLbTk", "nLbTk");
	LbL0Tree = fs->make < TTree > ("pLbL0", "pLbL0");
	LbLoTree = fs->make < TTree > ("nLbL0", "nLbL0");
	if ( useMC )
	{
		mcTree = fs->make < TTree > ("mc", "mc" );
		mc.RegFormatTree(mcTree);
	}
	pL0B.RegFormatTree(pL0BTree);
	nL0B.RegFormatTree(nL0BTree);
	LbL0.RegFormatTree(LbL0Tree);
	LbLo.RegFormatTree(LbLoTree);
    LbTk.RegFormatTree(LbTkTree);
    LbTk.RegFormatTree(LbTkTree);
	return;
}

// (Empty) Destructor
VertexCompCandAnalyzer::~VertexCompCandAnalyzer()
{
	return;
}

void VertexCompCandAnalyzer::fillDescriptions( edm::ConfigurationDescriptions& descriptions )
{
	edm::ParameterSetDescription desc;
	desc.add < string > ("pL0BCandsLabel", "");
	desc.add < string > ("nL0BCandsLabel", "");
	desc.add < string > ("LbL0CandsLabel", "");
	desc.add < string > ("LbLoCandsLabel", "");
	desc.add < string > ("HLTRecordLabel", "");
	desc.add < string > ("MCReserveLabel", "");
	desc.add < string > (  "bsPointLabel", "");
	descriptions.add("process.treeCreatingSpecificDecay", desc);
	return;
}

//
// Methods
//

// Producer Method
void VertexCompCandAnalyzer::analyze(const edm::Event& ev, const edm::EventSetup& es)
{
	// get magnetic field
	edm::ESHandle < MagneticField > magneticField;
	es.get < IdealMagneticFieldRecord > ().get(magneticField);

	// get object collections
	// collections are got through "BPHTokenWrapper" interface to allow
	// uniform access in different CMSSW versions
	bool fillCounter = false;

	edm::Handle < reco::BeamSpot > beamSpotHandle;
	if (useBS)
		ev.getByToken( bsPointToken, beamSpotHandle );
	if (!beamSpotHandle.isValid())
		return;
	edm::Handle < edm::TriggerResults > HLTRecordHandle;
	if (useHLT)
	{
		ev.getByToken( HLTRecordToken, HLTRecordHandle );
		if ( !HLTRecordHandle.isValid() )
		{ printf("---- no HLT valid! -----\n"); exit(1); }
	}

	const reco::BeamSpot & bs = *beamSpotHandle;

	int hltRec[LbTkRecord::comNumI] = {0};

	if ( useHLT )
	{
		const edm::TriggerNames& trgNamePool = ev.triggerNames( *HLTRecordHandle );
		for ( int iTrig = 0; iTrig != HLTList::totNum; ++iTrig )
		{
			unsigned trgIndex = trgNamePool.size();
			for ( unsigned iPool = 0; iPool != trgNamePool.size(); ++iPool )
			{
				const std::string& trimmedName = HLTConfigProvider::removeVersion( trgNamePool.triggerName(iPool) );
				if ( HLTList::trigName[iTrig] == trimmedName )
				{ trgIndex = trgNamePool.triggerIndex( trgNamePool.triggerName(iPool) ); break; }
			}
			if ( trgIndex == trgNamePool.size() )
			{ hltRec[LbTkRecord::trigVanish] += HLTList::encodeHLT(iTrig); }		// the trigger path is not recorded in the event.
			else if ( !HLTRecordHandle->wasrun(trgIndex) )
			{ hltRec[LbTkRecord::trigNotRun] += HLTList::encodeHLT(iTrig); }		// the trigger was not run in the event.
			else if ( !HLTRecordHandle->accept(trgIndex) )
			{ hltRec[LbTkRecord::trigReject] += HLTList::encodeHLT(iTrig); }		// the trigger was not accepted in the event.
			else if ( HLTRecordHandle->error(trgIndex) )
			{ hltRec[LbTkRecord::trigError] += HLTList::encodeHLT(iTrig); }			// there is error in the trigger.
			else
			{ hltRec[LbTkRecord::totallyTriggered] += HLTList::encodeHLT(iTrig); }	// pass the HLT
		}
		if ( hltRec[LbTkRecord::totallyTriggered] == 0 ) return;
	}

	//////////// LbTk ////////////

	if (usepL0B)				// Lb->JPsi p K {{{
	{
		edm::Handle < vector < reco::VertexCompositeCandidate > >pL0BCands;
		ev.getByToken( pL0BCandsToken, pL0BCands );


		// preselection {{{
		if (!pL0BCands.isValid()) goto endOfpL0B;

		std::vector < reco::VertexCompositeCandidate >::const_iterator handleIter = pL0BCands->cbegin();
		std::vector < reco::VertexCompositeCandidate >::const_iterator handleIend = pL0BCands->cend  ();
		std::vector < std::pair < double, const reco::VertexCompositeCandidate * > >selectedCandList;
		selectedCandList.clear();
		selectedCandList.reserve(pL0BCands->size());
		handleIter = pL0BCands->begin();
		handleIend = pL0BCands->end();
		while (handleIter != handleIend)
		{
			const reco::VertexCompositeCandidate & cand = *handleIter++;
			const reco::VertexCompositeCandidate* mumuCandPtr = dynamic_cast<const reco::VertexCompositeCandidate*>( cand.daughter("JPsi") );
			const reco::VertexCompositeCandidate* tktkCandPtr = dynamic_cast<const reco::VertexCompositeCandidate*>( cand.daughter("TkTk") );
			const reco::RecoChargedCandidate* muPosCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( mumuCandPtr->daughter("MuPos") );
			const reco::RecoChargedCandidate* muNegCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( mumuCandPtr->daughter("MuNeg") );
			const reco::RecoChargedCandidate* tkPosCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( tktkCandPtr->daughter("Proton") );
			const reco::RecoChargedCandidate* tkNegCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( tktkCandPtr->daughter("Kaon") );

			reco::Particle::LorentzVector tkPosP4 = tkPosCandPtr->p4(); tkPosP4.SetE(sqrt(tkPosP4.P2()+protonMASS*protonMASS));
			reco::Particle::LorentzVector tkNegP4 = tkNegCandPtr->p4(); tkNegP4.SetE(sqrt(tkNegP4.P2()+kaonMASS*kaonMASS));

			//reco::Particle::LorentzVector tktkPreSelP4 = tkPosP4+tkNegP4;
			// preselection
			if (sqrt(muPosCandPtr->momentum().Perp2()) < 4.0) continue;
			if (sqrt(muNegCandPtr->momentum().Perp2()) < 4.0) continue;
			if (sqrt(tkPosCandPtr->momentum().Perp2()) < 0.8) continue;
			if (sqrt(tkNegCandPtr->momentum().Perp2()) < 0.3) continue;

			if (sqrt(cand.momentum().Perp2()) < 10.) continue;

			//const GlobalPoint
			//const reco::Particle::Point& _vtx = cand.vertex();
			//double fdSig = usefulFuncs::getFlightDistanceSignificance(cand, &bs);
			//if (fdSig < 3.0) continue;
			//double cos2d = usefulFuncs::getCosa2d(cand, &bs);
			double vtxprob = TMath::Prob(cand.vertexChi2(), cand.vertexNdof());

			//if (cos2d < 0.95) continue;
			if (vtxprob < 0.03) continue;
			selectedCandList.emplace_back(vtxprob, &cand);
		}

		// preselection end }}}

		unsigned N = selectedCandList.size();
		N = selectedCandList.size();
		for (unsigned i = 0; i < N; ++i)
		{
			pL0B.Clear();
			const double candVtxprob = selectedCandList[i].first;
			const reco::VertexCompositeCandidate& selCand = *(selectedCandList[i].second);

			const reco::VertexCompositeCandidate* mumuCandPtr = dynamic_cast<const reco::VertexCompositeCandidate*>( selCand.daughter("JPsi") );
			const reco::VertexCompositeCandidate* tktkCandPtr = dynamic_cast<const reco::VertexCompositeCandidate*>( selCand.daughter("TkTk") );
			const reco::RecoChargedCandidate* muPosCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( mumuCandPtr->daughter("MuPos") );
			const reco::RecoChargedCandidate* muNegCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( mumuCandPtr->daughter("MuNeg") );
			const reco::RecoChargedCandidate* tkPosCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( tktkCandPtr->daughter("Proton") );
			const reco::RecoChargedCandidate* tkNegCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( tktkCandPtr->daughter("Kaon") );

			reco::Particle::LorentzVector muPosP4 = muPosCandPtr->p4(); muPosP4.SetE(sqrt(muPosP4.P2()+muonMASS*muonMASS));
			reco::Particle::LorentzVector muNegP4 = muNegCandPtr->p4(); muNegP4.SetE(sqrt(muNegP4.P2()+muonMASS*muonMASS));
			reco::Particle::LorentzVector tkPosP4 = tkPosCandPtr->p4(); tkPosP4.SetE(sqrt(tkPosP4.P2()+protonMASS*protonMASS));
			reco::Particle::LorentzVector tkNegP4 = tkNegCandPtr->p4(); tkNegP4.SetE(sqrt(tkNegP4.P2()+kaonMASS*kaonMASS));
			reco::Particle::LorentzVector tktkSelP4 = tkPosP4+tkNegP4;

			//reco::Particle::LorentzVector fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;

//			// check particles {{{
//			{
//				// bd
//				tkPosP4.SetE(sqrt(tkPosP4.P2()+kaonMASS*kaonMASS));
//				tkNegP4.SetE(sqrt(tkNegP4.P2()+pionMASS*pionMASS));
//				fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;
//
//				if ( fabs( fourTkSelP4.mag() - bdMASS ) < 2.5* bdWIDTH )
//					pL0B.dataI[LbTkRecord::mightBeOtherParticle][candSize] += 1<<1;
//
//				// bd bar
//				tkPosP4.SetE(sqrt(tkPosP4.P2()+pionMASS*pionMASS));
//				tkNegP4.SetE(sqrt(tkNegP4.P2()+kaonMASS*kaonMASS));
//				fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;
//
//				if ( fabs( fourTkSelP4.mag() - bdMASS ) < 2.5* bdWIDTH )
//					pL0B.dataI[LbTkRecord::mightBeOtherParticle][candSize] += 1<<2;
//
//				// bs
//				tkPosP4.SetE(sqrt(tkPosP4.P2()+kaonMASS*kaonMASS));
//				tkNegP4.SetE(sqrt(tkNegP4.P2()+kaonMASS*kaonMASS));
//				fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;
//				tktkSelP4 = tkPosP4+tkNegP4;
//
//				if ( fabs( fourTkSelP4.mag() - bsMASS ) < 2.5* bsWIDTH &&
//                   fabs(   tktkSelP4.mag() -phiMASS ) < 2.5*phiWIDTH )
//					pL0B.dataI[LbTkRecord::mightBeOtherParticle][candSize] += 1<<4;
//			}				// check particles end }}}
			GlobalPoint bsVtx2D( bs.x( tktkCandPtr->vertex().z() ), bs.y( tktkCandPtr->vertex().z() ), 0. );
			GlobalPoint tktkVtx2D( tktkCandPtr->vertex().x(),tktkCandPtr->vertex().y(),  0. );
			GlobalPoint mumuVtx2D( mumuCandPtr->vertex().x(),mumuCandPtr->vertex().y(),  0. );
			GlobalPoint fourTkVtx2D( selCand.vertex().x(), selCand.vertex().y(), 0. );
			GlobalVector tktkMom2D( tktkSelP4.x(), tktkSelP4.y(), 0. );
			GlobalVector fourTkMom2D( selCand.momentum().x(), selCand.momentum().y(), 0. );
			const usefulFuncs::SMatrixSym3D& bsCOV = bs.rotatedCovariance3D();
			const usefulFuncs::SMatrixSym3D& tktkCOV = tktkCandPtr->vertexCovariance();
			const usefulFuncs::SMatrixSym3D& mumuCOV = mumuCandPtr->vertexCovariance();
			const usefulFuncs::SMatrixSym3D& fourTkCOV = selCand.vertexCovariance();

			tkPosP4.SetE(sqrt(tkPosP4.P2()+protonMASS*protonMASS));
			tkNegP4.SetE(sqrt(tkNegP4.P2()+kaonMASS*kaonMASS));
			reco::Particle::LorentzVector fourTkMom = selCand.p4();
			reco::Particle::LorentzVector tktkMom = tkPosP4+tkNegP4;

			//const GlobalVector *twoTk = tktkCand.userData < GlobalVector > ("fitMomentum");
			//TLorentzVector twoTkMom(twoTk->x(), twoTk->y(), twoTk->z(),
			//                        sqrt(twoTk->x() * twoTk->x() + twoTk->y() * twoTk->y() + twoTk->z() * twoTk->z() +
			//                             tktkCand.userFloat("fitMass") * tktkCand.userFloat("fitMass")));
			pL0B.dataD[LbTkRecord::lbtkMass][pL0B.candSize] = selCand.mass();
			pL0B.dataD[LbTkRecord::lbtkPt][pL0B.candSize] = fourTkMom.Pt();
			pL0B.dataD[LbTkRecord::lbtkEta][pL0B.candSize] = fourTkMom.Eta();
			pL0B.dataD[LbTkRecord::lbtkY][pL0B.candSize] = fourTkMom.Rapidity();
			pL0B.dataD[LbTkRecord::lbtkPhi][pL0B.candSize] = fourTkMom.Phi();
			pL0B.dataD[LbTkRecord::lbtkFlightDistance2d][pL0B.candSize] = usefulFuncs::getFlightDistance(fourTkVtx2D, bsVtx2D);
			pL0B.dataD[LbTkRecord::lbtkFlightDistanceSig][pL0B.candSize] = usefulFuncs::getFlightDistanceSignificance(fourTkVtx2D, fourTkCOV, bsVtx2D, bsCOV);
			pL0B.dataD[LbTkRecord::lbtkCosa2d][pL0B.candSize] = usefulFuncs::getCosAngle(fourTkMom2D, fourTkVtx2D, bsVtx2D);
			pL0B.dataD[LbTkRecord::lbtkVtxprob][pL0B.candSize] = candVtxprob;
			pL0B.dataD[LbTkRecord::lbtknChi2][pL0B.candSize] = selCand.vertexChi2() / selCand.vertexNdof();

			pL0B.dataD[LbTkRecord::tktkMass][pL0B.candSize] = tktkMom.mass();
			pL0B.dataD[LbTkRecord::tktkPt][pL0B.candSize] = tktkMom.Pt();
			pL0B.dataD[LbTkRecord::tktkEta][pL0B.candSize] = tktkMom.Eta();
			pL0B.dataD[LbTkRecord::tktkY][pL0B.candSize] = tktkMom.Rapidity();
			pL0B.dataD[LbTkRecord::tktkPhi][pL0B.candSize] = tktkMom.Phi();
			pL0B.dataD[LbTkRecord::tktkFlightDistance2d][pL0B.candSize] = usefulFuncs::getFlightDistance(tktkVtx2D, mumuVtx2D);
			pL0B.dataD[LbTkRecord::tktkFlightDistanceSig][pL0B.candSize] = usefulFuncs::getFlightDistanceSignificance(tktkVtx2D, tktkCOV, mumuVtx2D, mumuCOV);
			pL0B.dataD[LbTkRecord::tktkCosa2d][pL0B.candSize] = usefulFuncs::getCosAngle(tktkMom2D, tktkVtx2D, mumuVtx2D);
			pL0B.dataD[LbTkRecord::tktkVtxprob][pL0B.candSize] = TMath::Prob(tkPosCandPtr->vertexChi2(), tkPosCandPtr->vertexNdof());
			pL0B.dataD[LbTkRecord::tktknChi2][pL0B.candSize] = tkPosCandPtr->vertexChi2() / tkPosCandPtr->vertexNdof();

			pL0B.dataD[LbTkRecord::pmuPt][pL0B.candSize] = sqrt(muPosP4.Perp2());
			pL0B.dataD[LbTkRecord::pmuP0][pL0B.candSize] = muPosP4.E();
			pL0B.dataD[LbTkRecord::pmuP1][pL0B.candSize] = muPosP4.Px();
			pL0B.dataD[LbTkRecord::pmuP2][pL0B.candSize] = muPosP4.Py();
			pL0B.dataD[LbTkRecord::pmuP3][pL0B.candSize] = muPosP4.Pz();
			pL0B.dataD[LbTkRecord::nmuPt][pL0B.candSize] = sqrt(muNegP4.Perp2());
			pL0B.dataD[LbTkRecord::nmuP0][pL0B.candSize] = muNegP4.E();
			pL0B.dataD[LbTkRecord::nmuP1][pL0B.candSize] = muNegP4.Px();
			pL0B.dataD[LbTkRecord::nmuP2][pL0B.candSize] = muNegP4.Py();
			pL0B.dataD[LbTkRecord::nmuP3][pL0B.candSize] = muNegP4.Pz();

			pL0B.dataD[LbTkRecord::tk1Pt][pL0B.candSize] = sqrt(tkPosP4.Perp2());
			pL0B.dataD[LbTkRecord::tk1P0][pL0B.candSize] = tkPosP4.E();
			pL0B.dataD[LbTkRecord::tk1P1][pL0B.candSize] = tkPosP4.Px();
			pL0B.dataD[LbTkRecord::tk1P2][pL0B.candSize] = tkPosP4.Py();
			pL0B.dataD[LbTkRecord::tk1P3][pL0B.candSize] = tkPosP4.Pz();
			pL0B.dataD[LbTkRecord::tk2Pt][pL0B.candSize] = sqrt(tkNegP4.Perp2());
			pL0B.dataD[LbTkRecord::tk2P0][pL0B.candSize] = tkNegP4.E();
			pL0B.dataD[LbTkRecord::tk2P1][pL0B.candSize] = tkNegP4.Px();
			pL0B.dataD[LbTkRecord::tk2P2][pL0B.candSize] = tkNegP4.Py();
			pL0B.dataD[LbTkRecord::tk2P3][pL0B.candSize] = tkNegP4.Pz();

			//pL0B.dataD[LbTkRecord::tk1IPt][pL0B.candSize] = selCand.userFloat("TkTk/Proton.IPt");
			//pL0B.dataD[LbTkRecord::tk2IPt][pL0B.candSize] = selCand.userFloat("TkTk/Kaon.IPt");
			//pL0B.dataD[LbTkRecord::tk1IPtErr][pL0B.candSize] = selCand.userFloat("TkTk/Proton.IPt.Error");
			//pL0B.dataD[LbTkRecord::tk2IPtErr][pL0B.candSize] = selCand.userFloat("TkTk/Kaon.IPt.Error");
			//if (selCand.hasUserFloat("TkTk/Proton.dEdx.pixelHrm"))
			//    pL0B.dataD[LbTkRecord::tk1DEDX_pixelHrm][pL0B.candSize] = selCand.userFloat("TkTk/Proton.dEdx.pixelHrm");
			//if (selCand.hasUserFloat("TkTk/Kaon.dEdx.pixelHrm"))
			//    pL0B.dataD[LbTkRecord::tk2DEDX_pixelHrm][pL0B.candSize] = selCand.userFloat("TkTk/Kaon.dEdx.pixelHrm");
			//if (selCand.hasUserFloat("TkTk/Proton.dEdx.Harmonic"))
			//    pL0B.dataD[LbTkRecord::tk1DEDX_Harmonic][pL0B.candSize] = selCand.userFloat("TkTk/Proton.dEdx.Harmonic");
			//if (selCand.hasUserFloat("TkTk/Kaon.dEdx.Harmonic"))
			//    pL0B.dataD[LbTkRecord::tk2DEDX_Harmonic][pL0B.candSize] = selCand.userFloat("TkTk/Kaon.dEdx.Harmonic");
			pL0B.dataI[LbTkRecord::eventEntry][pL0B.candSize] = entry;

			pL0B.dataI[LbTkRecord::trigVanish][pL0B.candSize]  = hltRec[LbTkRecord::trigVanish];	// the trigger path is not recorded in the event.
			pL0B.dataI[LbTkRecord::trigNotRun][pL0B.candSize]  = hltRec[LbTkRecord::trigNotRun];	// the trigger was not run in the event.
			pL0B.dataI[LbTkRecord::trigReject][pL0B.candSize]  = hltRec[LbTkRecord::trigReject];	// the trigger was not accepted in the event.
			pL0B.dataI[LbTkRecord::trigError][pL0B.candSize]   = hltRec[LbTkRecord::trigError ];	// there is error in the trigger.
			pL0B.dataI[LbTkRecord::totallyTriggered][pL0B.candSize] = hltRec[LbTkRecord::totallyTriggered];	// pass the HLT

            ++pL0B.candSize;
			fillCounter = true;
		}
        pL0BTree->Fill();

		//eventSeparator_pL0B = usefulFuncs::inverter(eventSeparator_pL0B);
	}							// Lb->Jpsi p K end }}}
endOfpL0B:

	//////////// anti LbTk ////////////

	if (usenL0B)				// Lb->JPsi P k {{{
	{
		edm::Handle < vector < reco::VertexCompositeCandidate > >nL0BCands;
		ev.getByToken( nL0BCandsToken, nL0BCands );

		// preselection {{{
		if (!nL0BCands.isValid()) goto endOfnL0B;

		std::vector < reco::VertexCompositeCandidate >::const_iterator handleIter = nL0BCands->cbegin();
		std::vector < reco::VertexCompositeCandidate >::const_iterator handleIend = nL0BCands->cend  ();
		std::vector < std::pair < double, const reco::VertexCompositeCandidate * > >selectedCandList;
		selectedCandList.clear();
		selectedCandList.reserve(nL0BCands->size());
		while (handleIter != handleIend)
		{
			const reco::VertexCompositeCandidate & cand = *handleIter++;
			const reco::VertexCompositeCandidate* mumuCandPtr = dynamic_cast<const reco::VertexCompositeCandidate*>( cand.daughter("JPsi") );
			const reco::VertexCompositeCandidate* tktkCandPtr = dynamic_cast<const reco::VertexCompositeCandidate*>( cand.daughter("TkTk") );
			const reco::RecoChargedCandidate* muPosCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( mumuCandPtr->daughter("MuPos") );
			const reco::RecoChargedCandidate* muNegCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( mumuCandPtr->daughter("MuNeg") );
			const reco::RecoChargedCandidate* tkPosCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( tktkCandPtr->daughter("Kaon") );
			const reco::RecoChargedCandidate* tkNegCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( tktkCandPtr->daughter("Proton") );

			reco::Particle::LorentzVector tkPosP4 = tkPosCandPtr->p4(); tkPosP4.SetE(sqrt(tkPosP4.P2()+protonMASS*protonMASS));
			reco::Particle::LorentzVector tkNegP4 = tkNegCandPtr->p4(); tkNegP4.SetE(sqrt(tkNegP4.P2()+kaonMASS*kaonMASS));

			//reco::Particle::LorentzVector tktkPreSelP4 = tkPosP4+tkNegP4;
			// preselection
			if (sqrt(muPosCandPtr->momentum().Perp2()) < 4.0) continue;
			if (sqrt(muNegCandPtr->momentum().Perp2()) < 4.0) continue;
			if (sqrt(tkPosCandPtr->momentum().Perp2()) < 0.3) continue;
			if (sqrt(tkNegCandPtr->momentum().Perp2()) < 0.8) continue;

			if (sqrt(cand.momentum().Perp2()) < 10.) continue;

			//const reco::Particle::Point& _vtx = cand.vertex();
			//double fdSig = usefulFuncs::getFlightDistanceSignificance(cand, &bs);
			//if (fdSig < 3.0) continue;
			//double cos2d = usefulFuncs::getCosa2d(cand, &bs);
			double vtxprob = TMath::Prob(cand.vertexChi2(), cand.vertexNdof());

			//if (cos2d < 0.95) continue;
			if (vtxprob < 0.03) continue;
			selectedCandList.emplace_back(vtxprob, &cand);
		}

		// preselection end }}}

		unsigned N = selectedCandList.size();
		for (unsigned i = 0; i < N; ++i)
		{
			nL0B.Clear();
			const double candVtxprob = selectedCandList[i].first;
			const reco::VertexCompositeCandidate& selCand = *(selectedCandList[i].second);

			const reco::VertexCompositeCandidate* mumuCandPtr = dynamic_cast<const reco::VertexCompositeCandidate*>( selCand.daughter("JPsi") );
			const reco::VertexCompositeCandidate* tktkCandPtr = dynamic_cast<const reco::VertexCompositeCandidate*>( selCand.daughter("TkTk") );
			const reco::RecoChargedCandidate* muPosCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( mumuCandPtr->daughter("MuPos") );
			const reco::RecoChargedCandidate* muNegCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( mumuCandPtr->daughter("MuNeg") );
			const reco::RecoChargedCandidate* tkPosCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( tktkCandPtr->daughter("Kaon") );
			const reco::RecoChargedCandidate* tkNegCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( tktkCandPtr->daughter("Proton") );

			reco::Particle::LorentzVector muPosP4 = muPosCandPtr->p4(); muPosP4.SetE(sqrt(muPosP4.P2()+muonMASS*muonMASS));
			reco::Particle::LorentzVector muNegP4 = muNegCandPtr->p4(); muNegP4.SetE(sqrt(muNegP4.P2()+muonMASS*muonMASS));
			reco::Particle::LorentzVector tkPosP4 = tkPosCandPtr->p4(); tkPosP4.SetE(sqrt(tkPosP4.P2()+kaonMASS*kaonMASS));
			reco::Particle::LorentzVector tkNegP4 = tkNegCandPtr->p4(); tkNegP4.SetE(sqrt(tkNegP4.P2()+protonMASS*protonMASS));
			reco::Particle::LorentzVector tktkSelP4 = tkPosP4+tkNegP4;

			//reco::Particle::LorentzVector fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;

//			// check particles {{{
//			{
//				// bd
//				tkPosP4.SetE(sqrt(tkPosP4.P2()+kaonMASS*kaonMASS));
//				tkNegP4.SetE(sqrt(tkNegP4.P2()+pionMASS*pionMASS));
//				fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;
//
//				if ( fabs( fourTkSelP4.mag() - bdMASS ) < 2.5* bdWIDTH )
//					nL0B.dataI[LbTkRecord::mightBeOtherParticle][candSize] += 1<<1;
//
//				// bd bar
//				tkPosP4.SetE(sqrt(tkPosP4.P2()+pionMASS*pionMASS));
//				tkNegP4.SetE(sqrt(tkNegP4.P2()+kaonMASS*kaonMASS));
//				fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;
//
//				if ( fabs( fourTkSelP4.mag() - bdMASS ) < 2.5* bdWIDTH )
//					nL0B.dataI[LbTkRecord::mightBeOtherParticle][candSize] += 1<<2;
//
//				// bs
//				tkPosP4.SetE(sqrt(tkPosP4.P2()+kaonMASS*kaonMASS));
//				tkNegP4.SetE(sqrt(tkNegP4.P2()+kaonMASS*kaonMASS));
//				fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;
//				tktkSelP4 = tkPosP4+tkNegP4;
//
//				if ( fabs( fourTkSelP4.mag() - bsMASS ) < 2.5* bsWIDTH &&
//				    fabs(   tktkSelP4.mag() -phiMASS ) < 2.5*phiWIDTH )
//					nL0B.dataI[LbTkRecord::mightBeOtherParticle][candSize] += 1<<4;
//			}				// check particles end }}}
			GlobalPoint bsVtx2D( bs.x( tktkCandPtr->vertex().z() ), bs.y( tktkCandPtr->vertex().z() ), 0. );
			GlobalPoint tktkVtx2D( tktkCandPtr->vertex().x(),tktkCandPtr->vertex().y(),  0. );
			GlobalPoint mumuVtx2D( mumuCandPtr->vertex().x(),mumuCandPtr->vertex().y(),  0. );
			GlobalPoint fourTkVtx2D( selCand.vertex().x(), selCand.vertex().y(), 0. );
			GlobalVector tktkMom2D( tktkSelP4.x(), tktkSelP4.y(), 0. );
			GlobalVector fourTkMom2D( selCand.momentum().x(), selCand.momentum().y(), 0. );
			const usefulFuncs::SMatrixSym3D& bsCOV = bs.rotatedCovariance3D();
			const usefulFuncs::SMatrixSym3D& tktkCOV = tktkCandPtr->vertexCovariance();
			const usefulFuncs::SMatrixSym3D& mumuCOV = mumuCandPtr->vertexCovariance();
			const usefulFuncs::SMatrixSym3D& fourTkCOV = selCand.vertexCovariance();

			tkPosP4.SetE(sqrt(tkPosP4.P2()+kaonMASS*kaonMASS));
			tkNegP4.SetE(sqrt(tkNegP4.P2()+protonMASS*protonMASS));
			reco::Particle::LorentzVector fourTkMom = selCand.p4();
			reco::Particle::LorentzVector tktkMom = tkPosP4+tkNegP4;
			nL0B.dataD[LbTkRecord::lbtkMass][nL0B.candSize] = selCand.mass();
			nL0B.dataD[LbTkRecord::lbtkPt][nL0B.candSize] = fourTkMom.Pt();
			nL0B.dataD[LbTkRecord::lbtkEta][nL0B.candSize] = fourTkMom.Eta();
			nL0B.dataD[LbTkRecord::lbtkY][nL0B.candSize] = fourTkMom.Rapidity();
			nL0B.dataD[LbTkRecord::lbtkPhi][nL0B.candSize] = fourTkMom.Phi();
			nL0B.dataD[LbTkRecord::lbtkFlightDistance2d][nL0B.candSize] = usefulFuncs::getFlightDistance(fourTkVtx2D, bsVtx2D);
			nL0B.dataD[LbTkRecord::lbtkFlightDistanceSig][nL0B.candSize] = usefulFuncs::getFlightDistanceSignificance(fourTkVtx2D, fourTkCOV, bsVtx2D, bsCOV);
			nL0B.dataD[LbTkRecord::lbtkCosa2d][nL0B.candSize] = usefulFuncs::getCosAngle(fourTkMom2D, fourTkVtx2D, bsVtx2D);
			nL0B.dataD[LbTkRecord::lbtkVtxprob][nL0B.candSize] = candVtxprob;
			nL0B.dataD[LbTkRecord::lbtknChi2][nL0B.candSize] = selCand.vertexChi2() / selCand.vertexNdof();

			nL0B.dataD[LbTkRecord::tktkMass][nL0B.candSize] = tktkMom.mass();
			nL0B.dataD[LbTkRecord::tktkPt][nL0B.candSize] = tktkMom.Pt();
			nL0B.dataD[LbTkRecord::tktkEta][nL0B.candSize] = tktkMom.Eta();
			nL0B.dataD[LbTkRecord::tktkY][nL0B.candSize] = tktkMom.Rapidity();
			nL0B.dataD[LbTkRecord::tktkPhi][nL0B.candSize] = tktkMom.Phi();
			nL0B.dataD[LbTkRecord::tktkFlightDistance2d][nL0B.candSize] = usefulFuncs::getFlightDistance(tktkVtx2D, mumuVtx2D);
			nL0B.dataD[LbTkRecord::tktkFlightDistanceSig][nL0B.candSize] = usefulFuncs::getFlightDistanceSignificance(tktkVtx2D, tktkCOV, mumuVtx2D, mumuCOV);
			nL0B.dataD[LbTkRecord::tktkCosa2d][nL0B.candSize] = usefulFuncs::getCosAngle(tktkMom2D, tktkVtx2D, mumuVtx2D);
			nL0B.dataD[LbTkRecord::tktkVtxprob][nL0B.candSize] = TMath::Prob(tkPosCandPtr->vertexChi2(), tkPosCandPtr->vertexNdof());
			nL0B.dataD[LbTkRecord::tktknChi2][nL0B.candSize] = tkPosCandPtr->vertexChi2() / tkPosCandPtr->vertexNdof();

			nL0B.dataD[LbTkRecord::pmuPt][nL0B.candSize] = sqrt(muPosP4.Perp2());
			nL0B.dataD[LbTkRecord::pmuP0][nL0B.candSize] = muPosP4.E();
			nL0B.dataD[LbTkRecord::pmuP1][nL0B.candSize] = muPosP4.Px();
			nL0B.dataD[LbTkRecord::pmuP2][nL0B.candSize] = muPosP4.Py();
			nL0B.dataD[LbTkRecord::pmuP3][nL0B.candSize] = muPosP4.Pz();
			nL0B.dataD[LbTkRecord::nmuPt][nL0B.candSize] = sqrt(muNegP4.Perp2());
			nL0B.dataD[LbTkRecord::nmuP0][nL0B.candSize] = muNegP4.E();
			nL0B.dataD[LbTkRecord::nmuP1][nL0B.candSize] = muNegP4.Px();
			nL0B.dataD[LbTkRecord::nmuP2][nL0B.candSize] = muNegP4.Py();
			nL0B.dataD[LbTkRecord::nmuP3][nL0B.candSize] = muNegP4.Pz();

			nL0B.dataD[LbTkRecord::tk1Pt][nL0B.candSize] = sqrt(tkPosP4.Perp2());
			nL0B.dataD[LbTkRecord::tk1P0][nL0B.candSize] = tkPosP4.E();
			nL0B.dataD[LbTkRecord::tk1P1][nL0B.candSize] = tkPosP4.Px();
			nL0B.dataD[LbTkRecord::tk1P2][nL0B.candSize] = tkPosP4.Py();
			nL0B.dataD[LbTkRecord::tk1P3][nL0B.candSize] = tkPosP4.Pz();
			nL0B.dataD[LbTkRecord::tk2Pt][nL0B.candSize] = sqrt(tkNegP4.Perp2());
			nL0B.dataD[LbTkRecord::tk2P0][nL0B.candSize] = tkNegP4.E();
			nL0B.dataD[LbTkRecord::tk2P1][nL0B.candSize] = tkNegP4.Px();
			nL0B.dataD[LbTkRecord::tk2P2][nL0B.candSize] = tkNegP4.Py();
			nL0B.dataD[LbTkRecord::tk2P3][nL0B.candSize] = tkNegP4.Pz();

			//nL0B.dataD[LbTkRecord::tk1IPt][nL0B.candSize] = selCand.userFloat("TkTk/Proton.IPt");
			//nL0B.dataD[LbTkRecord::tk2IPt][nL0B.candSize] = selCand.userFloat("TkTk/Kaon.IPt");
			//nL0B.dataD[LbTkRecord::tk1IPtErr][nL0B.candSize] = selCand.userFloat("TkTk/Proton.IPt.Error");
			//nL0B.dataD[LbTkRecord::tk2IPtErr][nL0B.candSize] = selCand.userFloat("TkTk/Kaon.IPt.Error");
			//if (selCand.hasUserFloat("TkTk/Proton.dEdx.pixelHrm"))
			//    nL0B.dataD[LbTkRecord::tk1DEDX_pixelHrm][nL0B.candSize] = selCand.userFloat("TkTk/Proton.dEdx.pixelHrm");
			//if (selCand.hasUserFloat("TkTk/Kaon.dEdx.pixelHrm"))
			//    nL0B.dataD[LbTkRecord::tk2DEDX_pixelHrm][nL0B.candSize] = selCand.userFloat("TkTk/Kaon.dEdx.pixelHrm");
			//if (selCand.hasUserFloat("TkTk/Proton.dEdx.Harmonic"))
			//    nL0B.dataD[LbTkRecord::tk1DEDX_Harmonic][nL0B.candSize] = selCand.userFloat("TkTk/Proton.dEdx.Harmonic");
			//if (selCand.hasUserFloat("TkTk/Kaon.dEdx.Harmonic"))
			//    nL0B.dataD[LbTkRecord::tk2DEDX_Harmonic][nL0B.candSize] = selCand.userFloat("TkTk/Kaon.dEdx.Harmonic");
			nL0B.dataI[LbTkRecord::eventEntry][nL0B.candSize] = entry;

			nL0B.dataI[LbTkRecord::trigVanish][nL0B.candSize]  = hltRec[LbTkRecord::trigVanish];	// the trigger path is not recorded in the event.
			nL0B.dataI[LbTkRecord::trigNotRun][nL0B.candSize]  = hltRec[LbTkRecord::trigNotRun];	// the trigger was not run in the event.
			nL0B.dataI[LbTkRecord::trigReject][nL0B.candSize]  = hltRec[LbTkRecord::trigReject];	// the trigger was not accepted in the event.
			nL0B.dataI[LbTkRecord::trigError][nL0B.candSize]   = hltRec[LbTkRecord::trigError ];	// there is error in the trigger.
			nL0B.dataI[LbTkRecord::totallyTriggered][nL0B.candSize] = hltRec[LbTkRecord::totallyTriggered];	// pass the HLT

            ++nL0B.candSize;
			fillCounter = true;
		}
        nL0BTree->Fill();

		//eventSeparator_nL0B = usefulFuncs::inverter(eventSeparator_nL0B);
	}							// Lb->Jpsi P k end }}}
endOfnL0B:

	//////////// LbL0 ////////////

	if (useLbL0)				// Lb->JPsi Lam0 {{{
	{
		edm::Handle < vector < reco::VertexCompositeCandidate > >LbL0Cands;
		ev.getByToken( LbL0CandsToken, LbL0Cands );

		// preselection {{{
		if (!LbL0Cands.isValid()) goto endOfLbL0;

		std::vector < reco::VertexCompositeCandidate >::const_iterator handleIter = LbL0Cands->cbegin();
		std::vector < reco::VertexCompositeCandidate >::const_iterator handleIend = LbL0Cands->cend  ();
		std::vector < std::pair < double, const reco::VertexCompositeCandidate * > >selectedCandList;
		selectedCandList.clear();
		selectedCandList.reserve(LbL0Cands->size());
		while (handleIter != handleIend)
		{
			const reco::VertexCompositeCandidate & cand = *handleIter++;
			const reco::VertexCompositeCandidate* mumuCandPtr = dynamic_cast<const reco::VertexCompositeCandidate*>( cand.daughter("JPsi") );
			const reco::RecoChargedCandidate* muPosCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( mumuCandPtr->daughter("MuPos") );
			const reco::RecoChargedCandidate* muNegCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( mumuCandPtr->daughter("MuNeg") );
			const reco::VertexCompositeCandidate* twoTkCandPtr = dynamic_cast<const reco::VertexCompositeCandidate*>( cand.daughter("Lam0")  );

			//const reco::RecoChargedCandidate* tkPosCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( cand.daughter("Kaon") );
			//const reco::RecoChargedCandidate* tkNegCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( cand.daughter("Proton") );

			//reco::Particle::LorentzVector tkPosP4 = tkPosCandPtr->p4(); tkPosP4.SetE(sqrt(tkPosP4.P2()+protonMASS*protonMASS));
			//reco::Particle::LorentzVector tkNegP4 = tkNegCandPtr->p4(); tkNegP4.SetE(sqrt(tkNegP4.P2()+kaonMASS*kaonMASS));
			//reco::Particle::LorentzVector tktkPreSelP4 = tkPosP4+tkNegP4;
			// preselection
			if (sqrt(muPosCandPtr->momentum().Perp2()) < 4.0) continue;
			if (sqrt(muNegCandPtr->momentum().Perp2()) < 4.0) continue;
			if (sqrt(twoTkCandPtr->momentum().Perp2()) < 1.3) continue;

			//if (sqrt(tkPosCandPtr->momentum().Perp2()) < 0.8) continue;
			//if (sqrt(tkNegCandPtr->momentum().Perp2()) < 0.3) continue;

			if (sqrt(cand.momentum().Perp2()) < 10.) continue;

			//const reco::Particle::Point& _vtx = cand.vertex();
			//double fdSig = usefulFuncs::getFlightDistanceSignificance(cand, &bs);
			//if (fdSig < 3.0) continue;
			//double cos2d = usefulFuncs::getCosa2d(cand, &bs);
			double vtxprob = TMath::Prob(cand.vertexChi2(), cand.vertexNdof());

			//if (cos2d < 0.95) continue;
			if (vtxprob < 0.03) continue;
			selectedCandList.emplace_back(vtxprob, &cand);
		}

		// preselection end }}}

		unsigned N = selectedCandList.size();
		for (unsigned i = 0; i < N; ++i)
		{
			LbL0.Clear();
			const double candVtxprob = selectedCandList[i].first;
			const reco::VertexCompositeCandidate& selCand = *(selectedCandList[i].second);

			const reco::VertexCompositeCandidate* mumuCandPtr = dynamic_cast<const reco::VertexCompositeCandidate*>( selCand.daughter("JPsi") );
			const reco::RecoChargedCandidate* muPosCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( mumuCandPtr->daughter("MuPos") );
			const reco::RecoChargedCandidate* muNegCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( mumuCandPtr->daughter("MuNeg") );
			const reco::VertexCompositeCandidate* twoTkCandPtr = dynamic_cast<const reco::VertexCompositeCandidate*>( selCand.daughter("Lam0")  );

			//const reco::RecoChargedCandidate* tkPosCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( selCand.daughter("Proton") );
			//const reco::RecoChargedCandidate* tkNegCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( selCand.daughter("Kaon") );

			reco::Particle::LorentzVector muPosP4 = muPosCandPtr->p4();
			reco::Particle::LorentzVector muNegP4 = muNegCandPtr->p4();
			reco::Particle::LorentzVector twoTkP4 = twoTkCandPtr->p4();

			//reco::Particle::LorentzVector tkPosP4 = tkPosCandPtr->p4(); tkPosP4.SetE(sqrt(tkPosP4.P2()+kaonMASS*kaonMASS));
			//reco::Particle::LorentzVector tkNegP4 = tkNegCandPtr->p4(); tkNegP4.SetE(sqrt(tkNegP4.P2()+protonMASS*protonMASS));
			//reco::Particle::LorentzVector tktkSelP4 = tkPosP4+tkNegP4;
			//reco::Particle::LorentzVector fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;
			//// check particles {{{
			//{
			//    // bd
			//    tkPosP4.SetE(sqrt(tkPosP4.P2()+kaonMASS*kaonMASS));
			//    tkNegP4.SetE(sqrt(tkNegP4.P2()+pionMASS*pionMASS));
			//    fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;

			//    if ( fabs( fourTkSelP4.mag() - bdMASS ) < 2.5* bdWIDTH )
			//        LbL0.dataI[LbTkRecord::mightBeOtherParticle][candSize] += 1<<1;

			//    // bd bar
			//    tkPosP4.SetE(sqrt(tkPosP4.P2()+pionMASS*pionMASS));
			//    tkNegP4.SetE(sqrt(tkNegP4.P2()+kaonMASS*kaonMASS));
			//    fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;

			//    if ( fabs( fourTkSelP4.mag() - bdMASS ) < 2.5* bdWIDTH )
			//        LbL0.dataI[LbTkRecord::mightBeOtherParticle][candSize] += 1<<2;

			//    // bs
			//    tkPosP4.SetE(sqrt(tkPosP4.P2()+kaonMASS*kaonMASS));
			//    tkNegP4.SetE(sqrt(tkNegP4.P2()+kaonMASS*kaonMASS));
			//    fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;
			//    tktkSelP4 = tkPosP4+tkNegP4;

			//    if ( fabs( fourTkSelP4.mag() - bsMASS ) < 2.5* bsWIDTH &&
			//         fabs(   tktkSelP4.mag() -phiMASS ) < 2.5*phiWIDTH )
			//        LbL0.dataI[LbTkRecord::mightBeOtherParticle][candSize] += 1<<4;

			//} // check particles end }}}
			GlobalPoint bsVtx2D( bs.x( twoTkCandPtr->vertex().z() ), bs.y( twoTkCandPtr->vertex().z() ), 0. );
			GlobalPoint tktkVtx2D( twoTkCandPtr->vertex().x(),twoTkCandPtr->vertex().y(),  0. );
			GlobalPoint mumuVtx2D( muPosCandPtr->vertex().x(),muPosCandPtr->vertex().y(),  0. );
			GlobalPoint fourTkVtx2D( selCand.vertex().x(), selCand.vertex().y(), 0. );
			GlobalVector tktkMom2D( twoTkP4.x(), twoTkP4.y(), 0. );
			GlobalVector fourTkMom2D( selCand.momentum().x(), selCand.momentum().y(), 0. );
			const usefulFuncs::SMatrixSym3D& bsCOV = bs.rotatedCovariance3D();
			const usefulFuncs::SMatrixSym3D& tktkCOV = twoTkCandPtr->vertexCovariance();
			const usefulFuncs::SMatrixSym3D& mumuCOV = mumuCandPtr->vertexCovariance();
			const usefulFuncs::SMatrixSym3D& fourTkCOV = selCand.vertexCovariance();

			reco::Particle::LorentzVector fourTkMom = selCand.p4();
			reco::Particle::LorentzVector tktkMom = twoTkP4;

			//const GlobalVector *twoTk = tktkCand.userData < GlobalVector > ("fitMomentum");
			//TLorentzVector twoTkMom(twoTk->x(), twoTk->y(), twoTk->z(),
			//                        sqrt(twoTk->x() * twoTk->x() + twoTk->y() * twoTk->y() + twoTk->z() * twoTk->z() +
			//                             tktkCand.userFloat("fitMass") * tktkCand.userFloat("fitMass")));
			LbL0.dataD[LbTkRecord::lbtkMass][LbL0.candSize] = selCand.mass();
			LbL0.dataD[LbTkRecord::lbtkPt][LbL0.candSize] = fourTkMom.Pt();
			LbL0.dataD[LbTkRecord::lbtkEta][LbL0.candSize] = fourTkMom.Eta();
			LbL0.dataD[LbTkRecord::lbtkY][LbL0.candSize] = fourTkMom.Rapidity();
			LbL0.dataD[LbTkRecord::lbtkPhi][LbL0.candSize] = fourTkMom.Phi();
			LbL0.dataD[LbTkRecord::lbtkVtxprob][LbL0.candSize] = candVtxprob;
			LbL0.dataD[LbTkRecord::lbtknChi2][LbL0.candSize] = selCand.vertexChi2() / selCand.vertexNdof();
			nL0B.dataD[LbTkRecord::lbtkFlightDistance2d][LbL0.candSize] = usefulFuncs::getFlightDistance(fourTkVtx2D, bsVtx2D);
			nL0B.dataD[LbTkRecord::lbtkFlightDistanceSig][LbL0.candSize] = usefulFuncs::getFlightDistanceSignificance(fourTkVtx2D, fourTkCOV, bsVtx2D, bsCOV);
			nL0B.dataD[LbTkRecord::lbtkCosa2d][LbL0.candSize] = usefulFuncs::getCosAngle(fourTkMom2D, fourTkVtx2D, bsVtx2D);

			LbL0.dataD[LbTkRecord::tktkMass][LbL0.candSize] = tktkMom.mass();
			LbL0.dataD[LbTkRecord::tktkPt][LbL0.candSize] = tktkMom.Pt();
			LbL0.dataD[LbTkRecord::tktkEta][LbL0.candSize] = tktkMom.Eta();
			LbL0.dataD[LbTkRecord::tktkY][LbL0.candSize] = tktkMom.Rapidity();
			LbL0.dataD[LbTkRecord::tktkPhi][LbL0.candSize] = tktkMom.Phi();
			LbL0.dataD[LbTkRecord::tktkFlightDistance2d][LbL0.candSize] = usefulFuncs::getFlightDistance(tktkVtx2D, mumuVtx2D);
			LbL0.dataD[LbTkRecord::tktkFlightDistanceSig][LbL0.candSize] = usefulFuncs::getFlightDistanceSignificance(tktkVtx2D, tktkCOV, mumuVtx2D, mumuCOV);
			LbL0.dataD[LbTkRecord::tktkCosa2d][LbL0.candSize] = usefulFuncs::getCosAngle(tktkMom2D, tktkVtx2D, mumuVtx2D);
			LbL0.dataD[LbTkRecord::tktkVtxprob][LbL0.candSize] = TMath::Prob(twoTkCandPtr->vertexChi2(), twoTkCandPtr->vertexNdof());
			LbL0.dataD[LbTkRecord::tktknChi2][LbL0.candSize] = twoTkCandPtr->vertexChi2() / twoTkCandPtr->vertexNdof();

			LbL0.dataD[LbTkRecord::pmuPt][LbL0.candSize] = sqrt(muPosP4.Perp2());
			LbL0.dataD[LbTkRecord::pmuP0][LbL0.candSize] = muPosP4.E();
			LbL0.dataD[LbTkRecord::pmuP1][LbL0.candSize] = muPosP4.Px();
			LbL0.dataD[LbTkRecord::pmuP2][LbL0.candSize] = muPosP4.Py();
			LbL0.dataD[LbTkRecord::pmuP3][LbL0.candSize] = muPosP4.Pz();
			LbL0.dataD[LbTkRecord::nmuPt][LbL0.candSize] = sqrt(muNegP4.Perp2());
			LbL0.dataD[LbTkRecord::nmuP0][LbL0.candSize] = muNegP4.E();
			LbL0.dataD[LbTkRecord::nmuP1][LbL0.candSize] = muNegP4.Px();
			LbL0.dataD[LbTkRecord::nmuP2][LbL0.candSize] = muNegP4.Py();
			LbL0.dataD[LbTkRecord::nmuP3][LbL0.candSize] = muNegP4.Pz();

			//LbL0.dataD[LbTkRecord::tk1Pt][LbL0.candSize] = sqrt(tkPosP4.Perp2());
			//LbL0.dataD[LbTkRecord::tk1P0][LbL0.candSize] = tkPosP4.E();
			//LbL0.dataD[LbTkRecord::tk1P1][LbL0.candSize] = tkPosP4.Px();
			//LbL0.dataD[LbTkRecord::tk1P2][LbL0.candSize] = tkPosP4.Py();
			//LbL0.dataD[LbTkRecord::tk1P3][LbL0.candSize] = tkPosP4.Pz();
			//LbL0.dataD[LbTkRecord::tk2Pt][LbL0.candSize] = sqrt(tkNegP4.Perp2());
			//LbL0.dataD[LbTkRecord::tk2P0][LbL0.candSize] = tkNegP4.E();
			//LbL0.dataD[LbTkRecord::tk2P1][LbL0.candSize] = tkNegP4.Px();
			//LbL0.dataD[LbTkRecord::tk2P2][LbL0.candSize] = tkNegP4.Py();
			//LbL0.dataD[LbTkRecord::tk2P3][LbL0.candSize] = tkNegP4.Pz();

			//LbL0.dataD[LbTkRecord::tk1IPt][LbL0.candSize] = selCand.userFloat("TkTk/Proton.IPt");
			//LbL0.dataD[LbTkRecord::tk2IPt][LbL0.candSize] = selCand.userFloat("TkTk/Kaon.IPt");
			//LbL0.dataD[LbTkRecord::tk1IPtErr][LbL0.candSize] = selCand.userFloat("TkTk/Proton.IPt.Error");
			//LbL0.dataD[LbTkRecord::tk2IPtErr][LbL0.candSize] = selCand.userFloat("TkTk/Kaon.IPt.Error");
			//if (selCand.hasUserFloat("TkTk/Proton.dEdx.pixelHrm"))
			//    LbL0.dataD[LbTkRecord::tk1DEDX_pixelHrm][LbL0.candSize] = selCand.userFloat("TkTk/Proton.dEdx.pixelHrm");
			//if (selCand.hasUserFloat("TkTk/Kaon.dEdx.pixelHrm"))
			//    LbL0.dataD[LbTkRecord::tk2DEDX_pixelHrm][LbL0.candSize] = selCand.userFloat("TkTk/Kaon.dEdx.pixelHrm");
			//if (selCand.hasUserFloat("TkTk/Proton.dEdx.Harmonic"))
			//    LbL0.dataD[LbTkRecord::tk1DEDX_Harmonic][LbL0.candSize] = selCand.userFloat("TkTk/Proton.dEdx.Harmonic");
			//if (selCand.hasUserFloat("TkTk/Kaon.dEdx.Harmonic"))
			//    LbL0.dataD[LbTkRecord::tk2DEDX_Harmonic][LbL0.candSize] = selCand.userFloat("TkTk/Kaon.dEdx.Harmonic");
			LbL0.dataI[LbTkRecord::eventEntry][LbL0.candSize] = entry;

			LbL0.dataI[LbTkRecord::trigVanish][LbL0.candSize]  = hltRec[LbTkRecord::trigVanish];	// the trigger path is not recorded in the event.
			LbL0.dataI[LbTkRecord::trigNotRun][LbL0.candSize]  = hltRec[LbTkRecord::trigNotRun];	// the trigger was not run in the event.
			LbL0.dataI[LbTkRecord::trigReject][LbL0.candSize]  = hltRec[LbTkRecord::trigReject];	// the trigger was not accepted in the event.
			LbL0.dataI[LbTkRecord::trigError][LbL0.candSize]   = hltRec[LbTkRecord::trigError ];	// there is error in the trigger.
			LbL0.dataI[LbTkRecord::totallyTriggered][LbL0.candSize] = hltRec[LbTkRecord::totallyTriggered];	// pass the HLT

            ++LbL0.candSize;
			fillCounter = true;
		}
        LbL0Tree->Fill();

		//eventSeparator_LbL0 = usefulFuncs::inverter(eventSeparator_LbL0);
	}							// Lb->Jpsi Lam0 end }}}
endOfLbL0:

	//////////// LbLo ////////////

	if (useLbLo)				// Lb->JPsi anti Lam0 {{{
	{
		edm::Handle < vector < reco::VertexCompositeCandidate > >LbLoCands;
		ev.getByToken( LbLoCandsToken, LbLoCands );

		// preselection {{{
		if (!LbLoCands.isValid()) goto endOfLbLo;

		std::vector < reco::VertexCompositeCandidate >::const_iterator handleIter = LbLoCands->cbegin();
		std::vector < reco::VertexCompositeCandidate >::const_iterator handleIend = LbLoCands->cend  ();
		std::vector < std::pair < double, const reco::VertexCompositeCandidate * > >selectedCandList;
		selectedCandList.clear();
		selectedCandList.reserve(LbLoCands->size());
		while (handleIter != handleIend)
		{
			const reco::VertexCompositeCandidate & cand = *handleIter++;
			const reco::VertexCompositeCandidate* mumuCandPtr = dynamic_cast<const reco::VertexCompositeCandidate*>( cand.daughter("JPsi") );
			const reco::RecoChargedCandidate* muPosCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( mumuCandPtr->daughter("MuPos") );
			const reco::RecoChargedCandidate* muNegCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( mumuCandPtr->daughter("MuNeg") );
			const reco::VertexCompositeCandidate* twoTkCandPtr = dynamic_cast<const reco::VertexCompositeCandidate*>( cand.daughter("Lam0")  );

			//const reco::RecoChargedCandidate* twoTkCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( cand.daughter("Lam0")  );
			//const reco::RecoChargedCandidate* tkPosCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( cand.daughter("Kaon") );
			//const reco::RecoChargedCandidate* tkNegCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( cand.daughter("Proton") );

			//reco::Particle::LorentzVector tkPosP4 = tkPosCandPtr->p4(); tkPosP4.SetE(sqrt(tkPosP4.P2()+protonMASS*protonMASS));
			//reco::Particle::LorentzVector tkNegP4 = tkNegCandPtr->p4(); tkNegP4.SetE(sqrt(tkNegP4.P2()+kaonMASS*kaonMASS));
			//reco::Particle::LorentzVector tktkPreSelP4 = tkPosP4+tkNegP4;
			// preselection
			if (sqrt(muPosCandPtr->momentum().Perp2()) < 4.0) continue;
			if (sqrt(muNegCandPtr->momentum().Perp2()) < 4.0) continue;
			if (sqrt(twoTkCandPtr->momentum().Perp2()) < 1.3) continue;

			//if (sqrt(tkPosCandPtr->momentum().Perp2()) < 0.8) continue;
			//if (sqrt(tkNegCandPtr->momentum().Perp2()) < 0.3) continue;

			if (sqrt(cand.momentum().Perp2()) < 10.) continue;

			//const reco::Particle::Point& _vtx = cand.vertex();
			//double fdSig = usefulFuncs::getFlightDistanceSignificance(cand, &bs);
			//if (fdSig < 3.0) continue;
			//double cos2d = usefulFuncs::getCosa2d(cand, &bs);
			double vtxprob = TMath::Prob(cand.vertexChi2(), cand.vertexNdof());

			//if (cos2d < 0.95) continue;
			if (vtxprob < 0.03) continue;
			selectedCandList.emplace_back(vtxprob, &cand);
		}

		// preselection end }}}

		unsigned N = selectedCandList.size();
		for (unsigned i = 0; i < N; ++i)
		{
			LbLo.Clear();
			const double candVtxprob = selectedCandList[i].first;
			const reco::VertexCompositeCandidate& selCand = *(selectedCandList[i].second);

			const reco::VertexCompositeCandidate* mumuCandPtr = dynamic_cast<const reco::VertexCompositeCandidate*>( selCand.daughter("JPsi") );
			const reco::RecoChargedCandidate* muPosCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( mumuCandPtr->daughter("MuPos") );
			const reco::RecoChargedCandidate* muNegCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( mumuCandPtr->daughter("MuNeg") );
			const reco::VertexCompositeCandidate* twoTkCandPtr = dynamic_cast<const reco::VertexCompositeCandidate*>( selCand.daughter("Lam0")  );

			//const reco::RecoChargedCandidate* tkPosCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( selCand.daughter("Proton") );
			//const reco::RecoChargedCandidate* tkNegCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( selCand.daughter("Kaon") );

			reco::Particle::LorentzVector muPosP4 = muPosCandPtr->p4();
			reco::Particle::LorentzVector muNegP4 = muNegCandPtr->p4();
			reco::Particle::LorentzVector twoTkP4 = twoTkCandPtr->p4();

			//reco::Particle::LorentzVector tkPosP4 = tkPosCandPtr->p4(); tkPosP4.SetE(sqrt(tkPosP4.P2()+kaonMASS*kaonMASS));
			//reco::Particle::LorentzVector tkNegP4 = tkNegCandPtr->p4(); tkNegP4.SetE(sqrt(tkNegP4.P2()+protonMASS*protonMASS));
			//reco::Particle::LorentzVector tktkSelP4 = tkPosP4+tkNegP4;
			//reco::Particle::LorentzVector fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;
			//// check particles {{{
			//{
			//    // bd
			//    tkPosP4.SetE(sqrt(tkPosP4.P2()+kaonMASS*kaonMASS));
			//    tkNegP4.SetE(sqrt(tkNegP4.P2()+pionMASS*pionMASS));
			//    fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;

			//    if ( fabs( fourTkSelP4.mag() - bdMASS ) < 2.5* bdWIDTH )
			//        LbLo.dataI[LbTkRecord::mightBeOtherParticle][candSize] += 1<<1;

			//    // bd bar
			//    tkPosP4.SetE(sqrt(tkPosP4.P2()+pionMASS*pionMASS));
			//    tkNegP4.SetE(sqrt(tkNegP4.P2()+kaonMASS*kaonMASS));
			//    fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;

			//    if ( fabs( fourTkSelP4.mag() - bdMASS ) < 2.5* bdWIDTH )
			//        LbLo.dataI[LbTkRecord::mightBeOtherParticle][candSize] += 1<<2;

			//    // bs
			//    tkPosP4.SetE(sqrt(tkPosP4.P2()+kaonMASS*kaonMASS));
			//    tkNegP4.SetE(sqrt(tkNegP4.P2()+kaonMASS*kaonMASS));
			//    fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;
			//    tktkSelP4 = tkPosP4+tkNegP4;

			//    if ( fabs( fourTkSelP4.mag() - bsMASS ) < 2.5* bsWIDTH &&
			//         fabs(   tktkSelP4.mag() -phiMASS ) < 2.5*phiWIDTH )
			//        LbLo.dataI[LbTkRecord::mightBeOtherParticle][candSize] += 1<<4;

			//} // check particles end }}}
			GlobalPoint bsVtx2D( bs.x( twoTkCandPtr->vertex().z() ), bs.y( twoTkCandPtr->vertex().z() ), 0. );
			GlobalPoint tktkVtx2D( twoTkCandPtr->vertex().x(),twoTkCandPtr->vertex().y(),  0. );
			GlobalPoint mumuVtx2D( muPosCandPtr->vertex().x(),muPosCandPtr->vertex().y(),  0. );
			GlobalPoint fourTkVtx2D( selCand.vertex().x(), selCand.vertex().y(), 0. );
			GlobalVector tktkMom2D( twoTkP4.x(), twoTkP4.y(), 0. );
			GlobalVector fourTkMom2D( selCand.momentum().x(), selCand.momentum().y(), 0. );
			const usefulFuncs::SMatrixSym3D& bsCOV = bs.rotatedCovariance3D();
			const usefulFuncs::SMatrixSym3D& tktkCOV = twoTkCandPtr->vertexCovariance();
			const usefulFuncs::SMatrixSym3D& mumuCOV = mumuCandPtr->vertexCovariance();
			const usefulFuncs::SMatrixSym3D& fourTkCOV = selCand.vertexCovariance();

			reco::Particle::LorentzVector fourTkMom = selCand.p4();
			reco::Particle::LorentzVector tktkMom = twoTkP4;

			LbLo.dataD[LbTkRecord::lbtkMass][LbLo.candSize] = selCand.mass();
			LbLo.dataD[LbTkRecord::lbtkPt][LbLo.candSize] = fourTkMom.Pt();
			LbLo.dataD[LbTkRecord::lbtkEta][LbLo.candSize] = fourTkMom.Eta();
			LbLo.dataD[LbTkRecord::lbtkY][LbLo.candSize] = fourTkMom.Rapidity();
			LbLo.dataD[LbTkRecord::lbtkPhi][LbLo.candSize] = fourTkMom.Phi();
			LbLo.dataD[LbTkRecord::lbtkFlightDistance2d][LbLo.candSize] = usefulFuncs::getFlightDistance(fourTkVtx2D, bsVtx2D);
			LbLo.dataD[LbTkRecord::lbtkFlightDistanceSig][LbLo.candSize] = usefulFuncs::getFlightDistanceSignificance(fourTkVtx2D, fourTkCOV, bsVtx2D, bsCOV);
			LbLo.dataD[LbTkRecord::lbtkCosa2d][LbLo.candSize] = usefulFuncs::getCosAngle(fourTkMom2D, fourTkVtx2D, bsVtx2D);
			LbLo.dataD[LbTkRecord::lbtkVtxprob][LbLo.candSize] = candVtxprob;
			LbLo.dataD[LbTkRecord::lbtknChi2][LbLo.candSize] = selCand.vertexChi2() / selCand.vertexNdof();

			LbLo.dataD[LbTkRecord::tktkMass][LbLo.candSize] = tktkMom.mass();
			LbLo.dataD[LbTkRecord::tktkPt][LbLo.candSize] = tktkMom.Pt();
			LbLo.dataD[LbTkRecord::tktkEta][LbLo.candSize] = tktkMom.Eta();
			LbLo.dataD[LbTkRecord::tktkY][LbLo.candSize] = tktkMom.Rapidity();
			LbLo.dataD[LbTkRecord::tktkPhi][LbLo.candSize] = tktkMom.Phi();
			LbLo.dataD[LbTkRecord::tktkFlightDistance2d][LbLo.candSize] = usefulFuncs::getFlightDistance(tktkVtx2D, mumuVtx2D);
			LbLo.dataD[LbTkRecord::tktkFlightDistanceSig][LbLo.candSize] = usefulFuncs::getFlightDistanceSignificance(tktkVtx2D, tktkCOV, mumuVtx2D, mumuCOV);
			LbLo.dataD[LbTkRecord::tktkCosa2d][LbLo.candSize] = usefulFuncs::getCosAngle(tktkMom2D, tktkVtx2D, mumuVtx2D);
			LbLo.dataD[LbTkRecord::tktkVtxprob][LbLo.candSize] = TMath::Prob(twoTkCandPtr->vertexChi2(), twoTkCandPtr->vertexNdof());
			LbLo.dataD[LbTkRecord::tktknChi2][LbLo.candSize] = twoTkCandPtr->vertexChi2() / twoTkCandPtr->vertexNdof();

			LbLo.dataD[LbTkRecord::pmuPt][LbLo.candSize] = sqrt(muPosP4.Perp2());
			LbLo.dataD[LbTkRecord::pmuP0][LbLo.candSize] = muPosP4.E();
			LbLo.dataD[LbTkRecord::pmuP1][LbLo.candSize] = muPosP4.Px();
			LbLo.dataD[LbTkRecord::pmuP2][LbLo.candSize] = muPosP4.Py();
			LbLo.dataD[LbTkRecord::pmuP3][LbLo.candSize] = muPosP4.Pz();
			LbLo.dataD[LbTkRecord::nmuPt][LbLo.candSize] = sqrt(muNegP4.Perp2());
			LbLo.dataD[LbTkRecord::nmuP0][LbLo.candSize] = muNegP4.E();
			LbLo.dataD[LbTkRecord::nmuP1][LbLo.candSize] = muNegP4.Px();
			LbLo.dataD[LbTkRecord::nmuP2][LbLo.candSize] = muNegP4.Py();
			LbLo.dataD[LbTkRecord::nmuP3][LbLo.candSize] = muNegP4.Pz();

			//LbLo.dataD[LbTkRecord::tk1Pt][LbLo.candSize] = sqrt(tkPosP4.Perp2());
			//LbLo.dataD[LbTkRecord::tk1P0][LbLo.candSize] = tkPosP4.E();
			//LbLo.dataD[LbTkRecord::tk1P1][LbLo.candSize] = tkPosP4.Px();
			//LbLo.dataD[LbTkRecord::tk1P2][LbLo.candSize] = tkPosP4.Py();
			//LbLo.dataD[LbTkRecord::tk1P3][LbLo.candSize] = tkPosP4.Pz();
			//LbLo.dataD[LbTkRecord::tk2Pt][LbLo.candSize] = sqrt(tkNegP4.Perp2());
			//LbLo.dataD[LbTkRecord::tk2P0][LbLo.candSize] = tkNegP4.E();
			//LbLo.dataD[LbTkRecord::tk2P1][LbLo.candSize] = tkNegP4.Px();
			//LbLo.dataD[LbTkRecord::tk2P2][LbLo.candSize] = tkNegP4.Py();
			//LbLo.dataD[LbTkRecord::tk2P3][LbLo.candSize] = tkNegP4.Pz();

			//LbLo.dataD[LbTkRecord::tk1IPt][LbLo.candSize] = selCand.userFloat("TkTk/Proton.IPt");
			//LbLo.dataD[LbTkRecord::tk2IPt][LbLo.candSize] = selCand.userFloat("TkTk/Kaon.IPt");
			//LbLo.dataD[LbTkRecord::tk1IPtErr][LbLo.candSize] = selCand.userFloat("TkTk/Proton.IPt.Error");
			//LbLo.dataD[LbTkRecord::tk2IPtErr][LbLo.candSize] = selCand.userFloat("TkTk/Kaon.IPt.Error");
			//if (selCand.hasUserFloat("TkTk/Proton.dEdx.pixelHrm"))
			//    LbLo.dataD[LbTkRecord::tk1DEDX_pixelHrm][LbLo.candSize] = selCand.userFloat("TkTk/Proton.dEdx.pixelHrm");
			//if (selCand.hasUserFloat("TkTk/Kaon.dEdx.pixelHrm"))
			//    LbLo.dataD[LbTkRecord::tk2DEDX_pixelHrm][LbLo.candSize] = selCand.userFloat("TkTk/Kaon.dEdx.pixelHrm");
			//if (selCand.hasUserFloat("TkTk/Proton.dEdx.Harmonic"))
			//    LbLo.dataD[LbTkRecord::tk1DEDX_Harmonic][LbLo.candSize] = selCand.userFloat("TkTk/Proton.dEdx.Harmonic");
			//if (selCand.hasUserFloat("TkTk/Kaon.dEdx.Harmonic"))
			//    LbLo.dataD[LbTkRecord::tk2DEDX_Harmonic][LbLo.candSize] = selCand.userFloat("TkTk/Kaon.dEdx.Harmonic");
			LbLo.dataI[LbTkRecord::eventEntry][LbLo.candSize] = entry;

			LbLo.dataI[LbTkRecord::trigVanish][LbLo.candSize]  = hltRec[LbTkRecord::trigVanish];	// the trigger path is not recorded in the event.
			LbLo.dataI[LbTkRecord::trigNotRun][LbLo.candSize]  = hltRec[LbTkRecord::trigNotRun];	// the trigger was not run in the event.
			LbLo.dataI[LbTkRecord::trigReject][LbLo.candSize]  = hltRec[LbTkRecord::trigReject];	// the trigger was not accepted in the event.
			LbLo.dataI[LbTkRecord::trigError][LbLo.candSize]   = hltRec[LbTkRecord::trigError ];	// there is error in the trigger.
			LbLo.dataI[LbTkRecord::totallyTriggered][LbLo.candSize] = hltRec[LbTkRecord::totallyTriggered];	// pass the HLT

            ++LbLo.candSize;
			fillCounter = true;
		}
        LbLoTree->Fill();
	}							// Lb->Jpsi anti Lam0 end }}}

endOfLbLo:
	if ( useMC )
	{	// MC {{{
		edm::Handle < vector < reco::GenParticle > > mcCands;
		ev.getByToken( MCReserveToken, mcCands );
		if (!mcCands.isValid()) goto endOfMC;

		std::vector<reco::GenParticle>::const_iterator iter = mcCands->cbegin();
		std::vector<reco::GenParticle>::const_iterator iend = mcCands->cend  ();
		while ( iter != iend )
		{	// loop all particles
			const reco::GenParticle& mcCand = *iter++;

			// 0 : LambdaB // 1 : anti LambdaB
			// 2 : jpsi // 3 : lambda0 // 4 : anti lambda0
			// 5 : proton + // 6 : proton - // 7 : kaon + // 8 : kaon - // 9 : pion + //10 : pion -
			// 11: pentaQuark+ // 12: anti-pentaQuark-
			unsigned candTag = 0;
			unsigned pTkIdx = -1;
			unsigned nTkIdx = -1;
			unsigned tktkIdx = -1;
			unsigned mumuIdx = -1;
            unsigned penQIdx = -1;

			// check for LambdaB->Jpsi p+ K-
			if ( mcCand.pdgId() == 5122 ) candTag += 1 << 0;
			if ( mcCand.pdgId() ==-5122 ) candTag += 1 << 1;
			if ( candTag==0 ) continue;

			for ( unsigned iDau = 0; iDau != mcCand.numberOfDaughters(); ++iDau )
			{	// check daughter
				const reco::GenParticle* daug = dynamic_cast<const reco::GenParticle*>(mcCand.daughter(iDau));
				if ( daug->charge() == 0 )
				{	// if J/psi, Lambda0, anti-Lambda0 found
					switch ( daug->pdgId() )
					{
					case   443:	// for jpsi
						candTag += 1 << 2; mumuIdx = iDau; break;
					case  3122:	// lambda0
						candTag += 1 << 3; tktkIdx = iDau; break;
					case -3122:	// anti lambda0
						candTag += 1 << 4; tktkIdx = iDau; break;
					default:
						break;
					}
				}
				else
				{
					if ( daug->numberOfDaughters() == 0 )
					{	// if p+, K- found
						switch ( daug->pdgId() )
						{
						case  2212:	// for proton+
							candTag += 1 << 5; pTkIdx = iDau; break;
						case -2212:	// for proton-
							candTag += 1 << 6; nTkIdx = iDau; break;
						case   321:	// for kaon+
							candTag += 1 << 7; pTkIdx = iDau; break;
						case  -321:	// for kaon-
							candTag += 1 << 8; nTkIdx = iDau; break;
						default:
							break;
						}
					}
					else
					{	// if pentaQuark->J/psi p+ channel found.
						for ( unsigned idDau = 0; idDau != daug->numberOfDaughters(); ++idDau )
						{
							const reco::GenParticle* ddaug = dynamic_cast<const reco::GenParticle*>(daug->daughter(idDau));
							switch ( ddaug->pdgId() )
							{
							case   443:	// for jpsi
								candTag += 1 << 2; mumuIdx = idDau; penQIdx = iDau; break;
							case  2212:	// for proton+
								candTag += 1 << 5;  pTkIdx = idDau; penQIdx = iDau; break;
							case -2212:	// for proton-
								candTag += 1 << 6;  nTkIdx = idDau; penQIdx = iDau; break;
							default:
								break;
							}
						}
						switch ( daug->pdgId() )
						{
						case  4414:	// for my user assigned pentaQuark, Xi_cc*+ reassignes mass.
							candTag += 1 << 11; break;
						case -4414:	// for my user assigned anti-pentaQuark, anti-Xi_cc*- reassignes mass.
							candTag += 1 << 12; break;
						}
					}
				}
			}	// check daughter end (for loop)

			if ( (candTag>>3)%2 || (candTag>>4)%2 )
			{	// if there is lambda0 or anti lambda0
				const reco::GenParticle* dMom = dynamic_cast<const reco::GenParticle*>(mcCand.daughter(tktkIdx));
				for ( unsigned iDDau = 0; iDDau != dMom->numberOfDaughters(); ++iDDau )
				{
					const reco::GenParticle* ddaug = dynamic_cast<const reco::GenParticle*>(dMom->daughter(iDDau));
					switch ( ddaug->pdgId() )
					{
					case  2212:	// for proton+
						candTag += 1 << 5; pTkIdx = iDDau; break;
					case -2212:	// for proton-
						candTag += 1 << 6; nTkIdx = iDDau; break;
					case   211:	// for pion+
						candTag += 1 << 9; pTkIdx = iDDau; break;
					case  -211:	// for poin-
						candTag += 1 <<10; nTkIdx = iDDau; break;
					default:
						break;
					}
				}
			}
			if ( pTkIdx == (unsigned) -1 || nTkIdx == (unsigned) -1 || mumuIdx == (unsigned) -1 )
				continue;

            const reco::GenParticle* mumuMomPtr = penQIdx != (unsigned) -1 ? dynamic_cast<const reco::GenParticle*>(mcCand.daughter(penQIdx)) : &mcCand;
            const reco::GenParticle* ptkMomPtr  = penQIdx != (unsigned) -1 ? dynamic_cast<const reco::GenParticle*>(mcCand.daughter(penQIdx)) : 
                                                ( tktkIdx != (unsigned) -1 ? dynamic_cast<const reco::GenParticle*>(mcCand.daughter(tktkIdx)) : &mcCand );
            const reco::GenParticle* ntkMomPtr  = penQIdx != (unsigned) -1 ? dynamic_cast<const reco::GenParticle*>(mcCand.daughter(penQIdx)) : 
                                                ( tktkIdx != (unsigned) -1 ? dynamic_cast<const reco::GenParticle*>(mcCand.daughter(tktkIdx)) : &mcCand );
            
            const reco::GenParticle* mumuCand = dynamic_cast<const reco::GenParticle*>(mumuMomPtr->daughter(mumuIdx));

			const reco::GenParticle* pMuCand = dynamic_cast<const reco::GenParticle*>(mumuCand->daughter(0));
			const reco::GenParticle* nMuCand = dynamic_cast<const reco::GenParticle*>(mumuCand->daughter(1));
			const reco::GenParticle* pTkCand = dynamic_cast<const reco::GenParticle*>(ptkMomPtr->daughter(pTkIdx));
			const reco::GenParticle* nTkCand = dynamic_cast<const reco::GenParticle*>(ntkMomPtr->daughter(nTkIdx));

			if ( pMuCand->charge() < 0) std::cout<<"----VertexCompCandAnalyzer::analyze() : pos mu own neg charge\n";
			if ( nMuCand->charge() > 0) std::cout<<"----VertexCompCandAnalyzer::analyze() : neg mu own pos charge\n";

			GlobalPoint bsVtx2D( bs.x( mcCand.vertex().z() ), bs.y( mcCand.vertex().z() ), 0. );
			GlobalPoint mumuVtx2D( mumuCand->vertex().x(), mumuCand->vertex().y(), 0. );
			GlobalPoint fourTkVtx2D( mcCand.vertex().x(), mcCand.vertex().y(), 0. );
			GlobalVector fourTkMom2D( mcCand.momentum().x(), mcCand.momentum().y(), 0. );

			mc.dataD[MCRecord::candMass][mc.candSize] = mcCand.mass();
			mc.dataD[MCRecord::candPt][mc.candSize] = mcCand.pt();
			mc.dataD[MCRecord::candEta][mc.candSize] = mcCand.eta();
			mc.dataD[MCRecord::candY][mc.candSize] = mcCand.rapidity();
			mc.dataD[MCRecord::candPhi][mc.candSize] = mcCand.phi();
			mc.dataD[MCRecord::candFlightDistance2d][mc.candSize] = usefulFuncs::getFlightDistance(fourTkVtx2D, bsVtx2D);
			mc.dataD[MCRecord::candCosa2d][mc.candSize] = usefulFuncs::getCosAngle(fourTkMom2D, fourTkVtx2D, bsVtx2D);

			if ( tktkIdx != (unsigned) -1 )
			{
                const reco::GenParticle* tktkCand = dynamic_cast<const reco::GenParticle*>(mcCand.daughter(tktkIdx));
                GlobalPoint tktkVtx2D( tktkCand->vertex().x(), tktkCand->vertex().y(), 0. );
                GlobalVector tktkMom2D( tktkCand->momentum().x(), tktkCand->momentum().y(), 0. );
				mc.dataD[MCRecord::tktkMass][mc.candSize] = tktkCand->mass();
				mc.dataD[MCRecord::tktkPt][mc.candSize] = tktkCand->pt();
				mc.dataD[MCRecord::tktkEta][mc.candSize] = tktkCand->eta();
				mc.dataD[MCRecord::tktkY][mc.candSize] = tktkCand->rapidity();
				mc.dataD[MCRecord::tktkPhi][mc.candSize] = tktkCand->phi();
				mc.dataD[MCRecord::tktkFlightDistance2d][mc.candSize] = usefulFuncs::getFlightDistance(tktkVtx2D, mumuVtx2D);
				mc.dataD[MCRecord::tktkCosa2d][mc.candSize] = usefulFuncs::getCosAngle(tktkMom2D, tktkVtx2D, mumuVtx2D);
			}

			mc.dataD[MCRecord::muPosPt][mc.candSize]  = pMuCand->pt();
			mc.dataD[MCRecord::muPosEta][mc.candSize] = pMuCand->eta();
			mc.dataD[MCRecord::muPosPhi][mc.candSize] = pMuCand->phi();
			mc.dataD[MCRecord::muPosY][mc.candSize]   = pMuCand->rapidity();

			mc.dataD[MCRecord::muNegPt][mc.candSize]  = nMuCand->pt();
			mc.dataD[MCRecord::muNegEta][mc.candSize] = nMuCand->eta();
			mc.dataD[MCRecord::muNegPhi][mc.candSize] = nMuCand->phi();
			mc.dataD[MCRecord::muNegY][mc.candSize]   = nMuCand->rapidity();

			mc.dataD[MCRecord::tkPosPt][mc.candSize]  = pTkCand->pt();
			mc.dataD[MCRecord::tkPosEta][mc.candSize] = pTkCand->eta();
			mc.dataD[MCRecord::tkPosPhi][mc.candSize] = pTkCand->phi();
			mc.dataD[MCRecord::tkPosY][mc.candSize]   = pTkCand->rapidity();

			mc.dataD[MCRecord::tkNegPt][mc.candSize]  = nTkCand->pt();
			mc.dataD[MCRecord::tkNegEta][mc.candSize] = nTkCand->eta();
			mc.dataD[MCRecord::tkNegPhi][mc.candSize] = nTkCand->phi();
			mc.dataD[MCRecord::tkNegY][mc.candSize]   = nTkCand->rapidity();

			mc.dataI[MCRecord::candPID][mc.candSize] = mcCand.pdgId();
			mc.dataI[MCRecord::muPosPID][mc.candSize] = pMuCand->pdgId();
			mc.dataI[MCRecord::muNegPID][mc.candSize] = nMuCand->pdgId();
			mc.dataI[MCRecord::tkPosPID][mc.candSize] = pTkCand->pdgId();
			mc.dataI[MCRecord::tkNegPID][mc.candSize] = nTkCand->pdgId();
			mc.dataI[MCRecord::eventEntry][mc.candSize] = entry;

            ++mc.candSize;
			fillCounter = true;
		}	// loop all particles end (while loop)
        mcTree->Fill();
	}	// MC end }}}
endOfMC:

	if ( fillCounter )
		++entry;
	return;
}

void VertexCompCandAnalyzer::beginJob()
{
	entry = 0;
	return;
}

void VertexCompCandAnalyzer::endJob()
{
}

//define this as a plug-in
#include "FWCore/PluginManager/interface/ModuleDef.h"

DEFINE_FWK_MODULE(VertexCompCandAnalyzer);
