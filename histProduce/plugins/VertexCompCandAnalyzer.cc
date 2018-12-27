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
//					pL0B.dataI[LbTkRecord::mightBeOtherParticle] += 1<<1;
//
//				// bd bar
//				tkPosP4.SetE(sqrt(tkPosP4.P2()+pionMASS*pionMASS));
//				tkNegP4.SetE(sqrt(tkNegP4.P2()+kaonMASS*kaonMASS));
//				fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;
//
//				if ( fabs( fourTkSelP4.mag() - bdMASS ) < 2.5* bdWIDTH )
//					pL0B.dataI[LbTkRecord::mightBeOtherParticle] += 1<<2;
//
//				// bs
//				tkPosP4.SetE(sqrt(tkPosP4.P2()+kaonMASS*kaonMASS));
//				tkNegP4.SetE(sqrt(tkNegP4.P2()+kaonMASS*kaonMASS));
//				fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;
//				tktkSelP4 = tkPosP4+tkNegP4;
//
//				if ( fabs( fourTkSelP4.mag() - bsMASS ) < 2.5* bsWIDTH &&
//                   fabs(   tktkSelP4.mag() -phiMASS ) < 2.5*phiWIDTH )
//					pL0B.dataI[LbTkRecord::mightBeOtherParticle] += 1<<4;
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
			pL0B.dataD[LbTkRecord::lbtkMass] = selCand.mass();
			pL0B.dataD[LbTkRecord::lbtkPt] = fourTkMom.Pt();
			pL0B.dataD[LbTkRecord::lbtkEta] = fourTkMom.Eta();
			pL0B.dataD[LbTkRecord::lbtkY] = fourTkMom.Rapidity();
			pL0B.dataD[LbTkRecord::lbtkPhi] = fourTkMom.Phi();
			pL0B.dataD[LbTkRecord::lbtkFlightDistance2d] = usefulFuncs::getFlightDistance(fourTkVtx2D, bsVtx2D);
			pL0B.dataD[LbTkRecord::lbtkFlightDistanceSig] = usefulFuncs::getFlightDistanceSignificance(fourTkVtx2D, fourTkCOV, bsVtx2D, bsCOV);
			pL0B.dataD[LbTkRecord::lbtkCosa2d] = usefulFuncs::getCosAngle(fourTkMom2D, fourTkVtx2D, bsVtx2D);
			pL0B.dataD[LbTkRecord::lbtkVtxprob] = candVtxprob;
			pL0B.dataD[LbTkRecord::lbtknChi2] = selCand.vertexChi2() / selCand.vertexNdof();

			pL0B.dataD[LbTkRecord::tktkMass] = tktkMom.mass();
			pL0B.dataD[LbTkRecord::tktkPt] = tktkMom.Pt();
			pL0B.dataD[LbTkRecord::tktkEta] = tktkMom.Eta();
			pL0B.dataD[LbTkRecord::tktkY] = tktkMom.Rapidity();
			pL0B.dataD[LbTkRecord::tktkPhi] = tktkMom.Phi();
			pL0B.dataD[LbTkRecord::tktkFlightDistance2d] = usefulFuncs::getFlightDistance(tktkVtx2D, bsVtx2D);
			pL0B.dataD[LbTkRecord::tktkFlightDistanceSig] = usefulFuncs::getFlightDistanceSignificance(tktkVtx2D, tktkCOV, mumuVtx2D, mumuCOV);
			pL0B.dataD[LbTkRecord::tktkCosa2d] = usefulFuncs::getCosAngle(tktkMom2D, tktkVtx2D, mumuVtx2D);
			pL0B.dataD[LbTkRecord::tktkVtxprob] = TMath::Prob(tkPosCandPtr->vertexChi2(), tkPosCandPtr->vertexNdof());
			pL0B.dataD[LbTkRecord::tktknChi2] = tkPosCandPtr->vertexChi2() / tkPosCandPtr->vertexNdof();

			pL0B.dataD[LbTkRecord::pmuPt] = sqrt(muPosP4.Perp2());
			pL0B.dataD[LbTkRecord::pmuP0] = muPosP4.E();
			pL0B.dataD[LbTkRecord::pmuP1] = muPosP4.Px();
			pL0B.dataD[LbTkRecord::pmuP2] = muPosP4.Py();
			pL0B.dataD[LbTkRecord::pmuP3] = muPosP4.Pz();
			pL0B.dataD[LbTkRecord::nmuPt] = sqrt(muNegP4.Perp2());
			pL0B.dataD[LbTkRecord::nmuP0] = muNegP4.E();
			pL0B.dataD[LbTkRecord::nmuP1] = muNegP4.Px();
			pL0B.dataD[LbTkRecord::nmuP2] = muNegP4.Py();
			pL0B.dataD[LbTkRecord::nmuP3] = muNegP4.Pz();

			pL0B.dataD[LbTkRecord::tk1Pt] = sqrt(tkPosP4.Perp2());
			pL0B.dataD[LbTkRecord::tk1P0] = tkPosP4.E();
			pL0B.dataD[LbTkRecord::tk1P1] = tkPosP4.Px();
			pL0B.dataD[LbTkRecord::tk1P2] = tkPosP4.Py();
			pL0B.dataD[LbTkRecord::tk1P3] = tkPosP4.Pz();
			pL0B.dataD[LbTkRecord::tk2Pt] = sqrt(tkNegP4.Perp2());
			pL0B.dataD[LbTkRecord::tk2P0] = tkNegP4.E();
			pL0B.dataD[LbTkRecord::tk2P1] = tkNegP4.Px();
			pL0B.dataD[LbTkRecord::tk2P2] = tkNegP4.Py();
			pL0B.dataD[LbTkRecord::tk2P3] = tkNegP4.Pz();

			//pL0B.dataD[LbTkRecord::tk1IPt] = selCand.userFloat("TkTk/Proton.IPt");
			//pL0B.dataD[LbTkRecord::tk2IPt] = selCand.userFloat("TkTk/Kaon.IPt");
			//pL0B.dataD[LbTkRecord::tk1IPtErr] = selCand.userFloat("TkTk/Proton.IPt.Error");
			//pL0B.dataD[LbTkRecord::tk2IPtErr] = selCand.userFloat("TkTk/Kaon.IPt.Error");
			//if (selCand.hasUserFloat("TkTk/Proton.dEdx.pixelHrm"))
			//    pL0B.dataD[LbTkRecord::tk1DEDX_pixelHrm] = selCand.userFloat("TkTk/Proton.dEdx.pixelHrm");
			//if (selCand.hasUserFloat("TkTk/Kaon.dEdx.pixelHrm"))
			//    pL0B.dataD[LbTkRecord::tk2DEDX_pixelHrm] = selCand.userFloat("TkTk/Kaon.dEdx.pixelHrm");
			//if (selCand.hasUserFloat("TkTk/Proton.dEdx.Harmonic"))
			//    pL0B.dataD[LbTkRecord::tk1DEDX_Harmonic] = selCand.userFloat("TkTk/Proton.dEdx.Harmonic");
			//if (selCand.hasUserFloat("TkTk/Kaon.dEdx.Harmonic"))
			//    pL0B.dataD[LbTkRecord::tk2DEDX_Harmonic] = selCand.userFloat("TkTk/Kaon.dEdx.Harmonic");
			pL0B.dataI[LbTkRecord::eventEntry] = entry;

			if ( useHLT )
			{
				const edm::TriggerNames& trgNamePool = ev.triggerNames( *HLTRecordHandle );
				for ( int iTrig = 0; iTrig != HLTList::totNum; ++iTrig )
				{
					unsigned trgIndex = trgNamePool.triggerIndex( HLTList::trigName[iTrig] );
					if ( trgIndex != trgNamePool.size() )
					{
						if ( HLTRecordHandle->wasrun(trgIndex) )
						{
							if ( HLTRecordHandle->accept(trgIndex) )
							{
								if ( !HLTRecordHandle->error(trgIndex) )
								{
									pL0B.dataI[LbTkRecord::totallyTriggered] += HLTList::encodeHLT(iTrig);										// pass the HLT
								}
								else
								{ pL0B.dataI[LbTkRecord::trigError] += HLTList::encodeHLT(iTrig); }									// there is error in the trigger.
							}
							else
							{ pL0B.dataI[LbTkRecord::trigReject] += HLTList::encodeHLT(iTrig); }							// the trigger was not accepted in the event.
						}
						else
						{ pL0B.dataI[LbTkRecord::trigNotRun] += HLTList::encodeHLT(iTrig); }						// the trigger was not run in the event.
					}
					else
					{ pL0B.dataI[LbTkRecord::trigVanish] += HLTList::encodeHLT(iTrig); }					// the trigger path is not recorded in the event.
				}
			}
			pL0BTree->Fill();
		}

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
		handleIter = nL0BCands->begin();
		handleIend = nL0BCands->end();
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
//					nL0B.dataI[LbTkRecord::mightBeOtherParticle] += 1<<1;
//
//				// bd bar
//				tkPosP4.SetE(sqrt(tkPosP4.P2()+pionMASS*pionMASS));
//				tkNegP4.SetE(sqrt(tkNegP4.P2()+kaonMASS*kaonMASS));
//				fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;
//
//				if ( fabs( fourTkSelP4.mag() - bdMASS ) < 2.5* bdWIDTH )
//					nL0B.dataI[LbTkRecord::mightBeOtherParticle] += 1<<2;
//
//				// bs
//				tkPosP4.SetE(sqrt(tkPosP4.P2()+kaonMASS*kaonMASS));
//				tkNegP4.SetE(sqrt(tkNegP4.P2()+kaonMASS*kaonMASS));
//				fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;
//				tktkSelP4 = tkPosP4+tkNegP4;
//
//				if ( fabs( fourTkSelP4.mag() - bsMASS ) < 2.5* bsWIDTH &&
//				    fabs(   tktkSelP4.mag() -phiMASS ) < 2.5*phiWIDTH )
//					nL0B.dataI[LbTkRecord::mightBeOtherParticle] += 1<<4;
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
			nL0B.dataD[LbTkRecord::lbtkMass] = selCand.mass();
			nL0B.dataD[LbTkRecord::lbtkPt] = fourTkMom.Pt();
			nL0B.dataD[LbTkRecord::lbtkEta] = fourTkMom.Eta();
			nL0B.dataD[LbTkRecord::lbtkY] = fourTkMom.Rapidity();
			nL0B.dataD[LbTkRecord::lbtkPhi] = fourTkMom.Phi();
			nL0B.dataD[LbTkRecord::lbtkFlightDistance2d] = usefulFuncs::getFlightDistance(fourTkVtx2D, bsVtx2D);
			nL0B.dataD[LbTkRecord::lbtkFlightDistanceSig] = usefulFuncs::getFlightDistanceSignificance(fourTkVtx2D, fourTkCOV, bsVtx2D, bsCOV);
			nL0B.dataD[LbTkRecord::lbtkCosa2d] = usefulFuncs::getCosAngle(fourTkMom2D, fourTkVtx2D, bsVtx2D);
			nL0B.dataD[LbTkRecord::lbtkVtxprob] = candVtxprob;
			nL0B.dataD[LbTkRecord::lbtknChi2] = selCand.vertexChi2() / selCand.vertexNdof();

			nL0B.dataD[LbTkRecord::tktkMass] = tktkMom.mass();
			nL0B.dataD[LbTkRecord::tktkPt] = tktkMom.Pt();
			nL0B.dataD[LbTkRecord::tktkEta] = tktkMom.Eta();
			nL0B.dataD[LbTkRecord::tktkY] = tktkMom.Rapidity();
			nL0B.dataD[LbTkRecord::tktkPhi] = tktkMom.Phi();
			pL0B.dataD[LbTkRecord::tktkFlightDistance2d] = usefulFuncs::getFlightDistance(tktkVtx2D, bsVtx2D);
			pL0B.dataD[LbTkRecord::tktkFlightDistanceSig] = usefulFuncs::getFlightDistanceSignificance(tktkVtx2D, tktkCOV, mumuVtx2D, mumuCOV);
			pL0B.dataD[LbTkRecord::tktkCosa2d] = usefulFuncs::getCosAngle(tktkMom2D, tktkVtx2D, mumuVtx2D);
			nL0B.dataD[LbTkRecord::tktkVtxprob] = TMath::Prob(tkPosCandPtr->vertexChi2(), tkPosCandPtr->vertexNdof());
			nL0B.dataD[LbTkRecord::tktknChi2] = tkPosCandPtr->vertexChi2() / tkPosCandPtr->vertexNdof();

			nL0B.dataD[LbTkRecord::pmuPt] = sqrt(muPosP4.Perp2());
			nL0B.dataD[LbTkRecord::pmuP0] = muPosP4.E();
			nL0B.dataD[LbTkRecord::pmuP1] = muPosP4.Px();
			nL0B.dataD[LbTkRecord::pmuP2] = muPosP4.Py();
			nL0B.dataD[LbTkRecord::pmuP3] = muPosP4.Pz();
			nL0B.dataD[LbTkRecord::nmuPt] = sqrt(muNegP4.Perp2());
			nL0B.dataD[LbTkRecord::nmuP0] = muNegP4.E();
			nL0B.dataD[LbTkRecord::nmuP1] = muNegP4.Px();
			nL0B.dataD[LbTkRecord::nmuP2] = muNegP4.Py();
			nL0B.dataD[LbTkRecord::nmuP3] = muNegP4.Pz();

			nL0B.dataD[LbTkRecord::tk1Pt] = sqrt(tkPosP4.Perp2());
			nL0B.dataD[LbTkRecord::tk1P0] = tkPosP4.E();
			nL0B.dataD[LbTkRecord::tk1P1] = tkPosP4.Px();
			nL0B.dataD[LbTkRecord::tk1P2] = tkPosP4.Py();
			nL0B.dataD[LbTkRecord::tk1P3] = tkPosP4.Pz();
			nL0B.dataD[LbTkRecord::tk2Pt] = sqrt(tkNegP4.Perp2());
			nL0B.dataD[LbTkRecord::tk2P0] = tkNegP4.E();
			nL0B.dataD[LbTkRecord::tk2P1] = tkNegP4.Px();
			nL0B.dataD[LbTkRecord::tk2P2] = tkNegP4.Py();
			nL0B.dataD[LbTkRecord::tk2P3] = tkNegP4.Pz();

			//nL0B.dataD[LbTkRecord::tk1IPt] = selCand.userFloat("TkTk/Proton.IPt");
			//nL0B.dataD[LbTkRecord::tk2IPt] = selCand.userFloat("TkTk/Kaon.IPt");
			//nL0B.dataD[LbTkRecord::tk1IPtErr] = selCand.userFloat("TkTk/Proton.IPt.Error");
			//nL0B.dataD[LbTkRecord::tk2IPtErr] = selCand.userFloat("TkTk/Kaon.IPt.Error");
			//if (selCand.hasUserFloat("TkTk/Proton.dEdx.pixelHrm"))
			//    nL0B.dataD[LbTkRecord::tk1DEDX_pixelHrm] = selCand.userFloat("TkTk/Proton.dEdx.pixelHrm");
			//if (selCand.hasUserFloat("TkTk/Kaon.dEdx.pixelHrm"))
			//    nL0B.dataD[LbTkRecord::tk2DEDX_pixelHrm] = selCand.userFloat("TkTk/Kaon.dEdx.pixelHrm");
			//if (selCand.hasUserFloat("TkTk/Proton.dEdx.Harmonic"))
			//    nL0B.dataD[LbTkRecord::tk1DEDX_Harmonic] = selCand.userFloat("TkTk/Proton.dEdx.Harmonic");
			//if (selCand.hasUserFloat("TkTk/Kaon.dEdx.Harmonic"))
			//    nL0B.dataD[LbTkRecord::tk2DEDX_Harmonic] = selCand.userFloat("TkTk/Kaon.dEdx.Harmonic");
			nL0B.dataI[LbTkRecord::eventEntry] = entry;

			if ( useHLT )
			{
				const edm::TriggerNames& trgNamePool = ev.triggerNames( *HLTRecordHandle );
				for ( int iTrig = 0; iTrig != HLTList::totNum; ++iTrig )
				{
					unsigned trgIndex = trgNamePool.triggerIndex( HLTList::trigName[iTrig] );
					if ( trgIndex != trgNamePool.size() )
					{
						if ( HLTRecordHandle->wasrun(trgIndex) )
						{
							if ( HLTRecordHandle->accept(trgIndex) )
							{
								if ( !HLTRecordHandle->error(trgIndex) )
								{
									nL0B.dataI[LbTkRecord::totallyTriggered] += HLTList::encodeHLT(iTrig);										// pass the HLT
								}
								else
								{ nL0B.dataI[LbTkRecord::trigError] += HLTList::encodeHLT(iTrig); }									// there is error in the trigger.
							}
							else
							{ nL0B.dataI[LbTkRecord::trigReject] += HLTList::encodeHLT(iTrig); }							// the trigger was not accepted in the event.
						}
						else
						{ nL0B.dataI[LbTkRecord::trigNotRun] += HLTList::encodeHLT(iTrig); }						// the trigger was not run in the event.
					}
					else
					{ nL0B.dataI[LbTkRecord::trigVanish] += HLTList::encodeHLT(iTrig); }					// the trigger path is not recorded in the event.
				}
			}
			nL0BTree->Fill();
		}

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
		handleIter = LbL0Cands->begin();
		handleIend = LbL0Cands->end();
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
			//        LbL0.dataI[LbTkRecord::mightBeOtherParticle] += 1<<1;

			//    // bd bar
			//    tkPosP4.SetE(sqrt(tkPosP4.P2()+pionMASS*pionMASS));
			//    tkNegP4.SetE(sqrt(tkNegP4.P2()+kaonMASS*kaonMASS));
			//    fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;

			//    if ( fabs( fourTkSelP4.mag() - bdMASS ) < 2.5* bdWIDTH )
			//        LbL0.dataI[LbTkRecord::mightBeOtherParticle] += 1<<2;

			//    // bs
			//    tkPosP4.SetE(sqrt(tkPosP4.P2()+kaonMASS*kaonMASS));
			//    tkNegP4.SetE(sqrt(tkNegP4.P2()+kaonMASS*kaonMASS));
			//    fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;
			//    tktkSelP4 = tkPosP4+tkNegP4;

			//    if ( fabs( fourTkSelP4.mag() - bsMASS ) < 2.5* bsWIDTH &&
			//         fabs(   tktkSelP4.mag() -phiMASS ) < 2.5*phiWIDTH )
			//        LbL0.dataI[LbTkRecord::mightBeOtherParticle] += 1<<4;

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
			LbL0.dataD[LbTkRecord::lbtkMass] = selCand.mass();
			LbL0.dataD[LbTkRecord::lbtkPt] = fourTkMom.Pt();
			LbL0.dataD[LbTkRecord::lbtkEta] = fourTkMom.Eta();
			LbL0.dataD[LbTkRecord::lbtkY] = fourTkMom.Rapidity();
			LbL0.dataD[LbTkRecord::lbtkPhi] = fourTkMom.Phi();
			LbL0.dataD[LbTkRecord::lbtkVtxprob] = candVtxprob;
			LbL0.dataD[LbTkRecord::lbtknChi2] = selCand.vertexChi2() / selCand.vertexNdof();
			nL0B.dataD[LbTkRecord::lbtkFlightDistance2d] = usefulFuncs::getFlightDistance(fourTkVtx2D, bsVtx2D);
			nL0B.dataD[LbTkRecord::lbtkFlightDistanceSig] = usefulFuncs::getFlightDistanceSignificance(fourTkVtx2D, fourTkCOV, bsVtx2D, bsCOV);
			nL0B.dataD[LbTkRecord::lbtkCosa2d] = usefulFuncs::getCosAngle(fourTkMom2D, fourTkVtx2D, bsVtx2D);

			LbL0.dataD[LbTkRecord::tktkMass] = tktkMom.mass();
			LbL0.dataD[LbTkRecord::tktkPt] = tktkMom.Pt();
			LbL0.dataD[LbTkRecord::tktkEta] = tktkMom.Eta();
			LbL0.dataD[LbTkRecord::tktkY] = tktkMom.Rapidity();
			LbL0.dataD[LbTkRecord::tktkPhi] = tktkMom.Phi();
			LbL0.dataD[LbTkRecord::tktkFlightDistance2d] = usefulFuncs::getFlightDistance(tktkVtx2D, bsVtx2D);
			LbL0.dataD[LbTkRecord::tktkFlightDistanceSig] = usefulFuncs::getFlightDistanceSignificance(tktkVtx2D, tktkCOV, mumuVtx2D, mumuCOV);
			LbL0.dataD[LbTkRecord::tktkCosa2d] = usefulFuncs::getCosAngle(tktkMom2D, tktkVtx2D, mumuVtx2D);
			LbL0.dataD[LbTkRecord::tktkVtxprob] = TMath::Prob(twoTkCandPtr->vertexChi2(), twoTkCandPtr->vertexNdof());
			LbL0.dataD[LbTkRecord::tktknChi2] = twoTkCandPtr->vertexChi2() / twoTkCandPtr->vertexNdof();

			LbL0.dataD[LbTkRecord::pmuPt] = sqrt(muPosP4.Perp2());
			LbL0.dataD[LbTkRecord::pmuP0] = muPosP4.E();
			LbL0.dataD[LbTkRecord::pmuP1] = muPosP4.Px();
			LbL0.dataD[LbTkRecord::pmuP2] = muPosP4.Py();
			LbL0.dataD[LbTkRecord::pmuP3] = muPosP4.Pz();
			LbL0.dataD[LbTkRecord::nmuPt] = sqrt(muNegP4.Perp2());
			LbL0.dataD[LbTkRecord::nmuP0] = muNegP4.E();
			LbL0.dataD[LbTkRecord::nmuP1] = muNegP4.Px();
			LbL0.dataD[LbTkRecord::nmuP2] = muNegP4.Py();
			LbL0.dataD[LbTkRecord::nmuP3] = muNegP4.Pz();

			//LbL0.dataD[LbTkRecord::tk1Pt] = sqrt(tkPosP4.Perp2());
			//LbL0.dataD[LbTkRecord::tk1P0] = tkPosP4.E();
			//LbL0.dataD[LbTkRecord::tk1P1] = tkPosP4.Px();
			//LbL0.dataD[LbTkRecord::tk1P2] = tkPosP4.Py();
			//LbL0.dataD[LbTkRecord::tk1P3] = tkPosP4.Pz();
			//LbL0.dataD[LbTkRecord::tk2Pt] = sqrt(tkNegP4.Perp2());
			//LbL0.dataD[LbTkRecord::tk2P0] = tkNegP4.E();
			//LbL0.dataD[LbTkRecord::tk2P1] = tkNegP4.Px();
			//LbL0.dataD[LbTkRecord::tk2P2] = tkNegP4.Py();
			//LbL0.dataD[LbTkRecord::tk2P3] = tkNegP4.Pz();

			//LbL0.dataD[LbTkRecord::tk1IPt] = selCand.userFloat("TkTk/Proton.IPt");
			//LbL0.dataD[LbTkRecord::tk2IPt] = selCand.userFloat("TkTk/Kaon.IPt");
			//LbL0.dataD[LbTkRecord::tk1IPtErr] = selCand.userFloat("TkTk/Proton.IPt.Error");
			//LbL0.dataD[LbTkRecord::tk2IPtErr] = selCand.userFloat("TkTk/Kaon.IPt.Error");
			//if (selCand.hasUserFloat("TkTk/Proton.dEdx.pixelHrm"))
			//    LbL0.dataD[LbTkRecord::tk1DEDX_pixelHrm] = selCand.userFloat("TkTk/Proton.dEdx.pixelHrm");
			//if (selCand.hasUserFloat("TkTk/Kaon.dEdx.pixelHrm"))
			//    LbL0.dataD[LbTkRecord::tk2DEDX_pixelHrm] = selCand.userFloat("TkTk/Kaon.dEdx.pixelHrm");
			//if (selCand.hasUserFloat("TkTk/Proton.dEdx.Harmonic"))
			//    LbL0.dataD[LbTkRecord::tk1DEDX_Harmonic] = selCand.userFloat("TkTk/Proton.dEdx.Harmonic");
			//if (selCand.hasUserFloat("TkTk/Kaon.dEdx.Harmonic"))
			//    LbL0.dataD[LbTkRecord::tk2DEDX_Harmonic] = selCand.userFloat("TkTk/Kaon.dEdx.Harmonic");
			LbL0.dataI[LbTkRecord::eventEntry] = entry;

			if ( useHLT )
			{
				const edm::TriggerNames& trgNamePool = ev.triggerNames( *HLTRecordHandle );
				for ( int iTrig = 0; iTrig != HLTList::totNum; ++iTrig )
				{
					unsigned trgIndex = trgNamePool.triggerIndex( HLTList::trigName[iTrig] );
					if ( trgIndex != trgNamePool.size() )
					{
						if ( HLTRecordHandle->wasrun(trgIndex) )
						{
							if ( HLTRecordHandle->accept(trgIndex) )
							{
								if ( !HLTRecordHandle->error(trgIndex) )
								{
									LbL0.dataI[LbTkRecord::totallyTriggered] += HLTList::encodeHLT(iTrig);										// pass the HLT
								}
								else
								{ LbL0.dataI[LbTkRecord::trigError] += HLTList::encodeHLT(iTrig); }									// there is error in the trigger.
							}
							else
							{ LbL0.dataI[LbTkRecord::trigReject] += HLTList::encodeHLT(iTrig); }							// the trigger was not accepted in the event.
						}
						else
						{ LbL0.dataI[LbTkRecord::trigNotRun] += HLTList::encodeHLT(iTrig); }						// the trigger was not run in the event.
					}
					else
					{ LbL0.dataI[LbTkRecord::trigVanish] += HLTList::encodeHLT(iTrig); }					// the trigger path is not recorded in the event.
				}
			}
			LbL0Tree->Fill();
		}

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
		handleIter = LbLoCands->begin();
		handleIend = LbLoCands->end();
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
			//        LbLo.dataI[LbTkRecord::mightBeOtherParticle] += 1<<1;

			//    // bd bar
			//    tkPosP4.SetE(sqrt(tkPosP4.P2()+pionMASS*pionMASS));
			//    tkNegP4.SetE(sqrt(tkNegP4.P2()+kaonMASS*kaonMASS));
			//    fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;

			//    if ( fabs( fourTkSelP4.mag() - bdMASS ) < 2.5* bdWIDTH )
			//        LbLo.dataI[LbTkRecord::mightBeOtherParticle] += 1<<2;

			//    // bs
			//    tkPosP4.SetE(sqrt(tkPosP4.P2()+kaonMASS*kaonMASS));
			//    tkNegP4.SetE(sqrt(tkNegP4.P2()+kaonMASS*kaonMASS));
			//    fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;
			//    tktkSelP4 = tkPosP4+tkNegP4;

			//    if ( fabs( fourTkSelP4.mag() - bsMASS ) < 2.5* bsWIDTH &&
			//         fabs(   tktkSelP4.mag() -phiMASS ) < 2.5*phiWIDTH )
			//        LbLo.dataI[LbTkRecord::mightBeOtherParticle] += 1<<4;

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

			LbLo.dataD[LbTkRecord::lbtkMass] = selCand.mass();
			LbLo.dataD[LbTkRecord::lbtkPt] = fourTkMom.Pt();
			LbLo.dataD[LbTkRecord::lbtkEta] = fourTkMom.Eta();
			LbLo.dataD[LbTkRecord::lbtkY] = fourTkMom.Rapidity();
			LbLo.dataD[LbTkRecord::lbtkPhi] = fourTkMom.Phi();
			LbLo.dataD[LbTkRecord::lbtkFlightDistance2d] = usefulFuncs::getFlightDistance(fourTkVtx2D, bsVtx2D);
			LbLo.dataD[LbTkRecord::lbtkFlightDistanceSig] = usefulFuncs::getFlightDistanceSignificance(fourTkVtx2D, fourTkCOV, bsVtx2D, bsCOV);
			LbLo.dataD[LbTkRecord::lbtkCosa2d] = usefulFuncs::getCosAngle(fourTkMom2D, fourTkVtx2D, bsVtx2D);
			LbLo.dataD[LbTkRecord::lbtkVtxprob] = candVtxprob;
			LbLo.dataD[LbTkRecord::lbtknChi2] = selCand.vertexChi2() / selCand.vertexNdof();

			LbLo.dataD[LbTkRecord::tktkMass] = tktkMom.mass();
			LbLo.dataD[LbTkRecord::tktkPt] = tktkMom.Pt();
			LbLo.dataD[LbTkRecord::tktkEta] = tktkMom.Eta();
			LbLo.dataD[LbTkRecord::tktkY] = tktkMom.Rapidity();
			LbLo.dataD[LbTkRecord::tktkPhi] = tktkMom.Phi();
			LbLo.dataD[LbTkRecord::tktkFlightDistance2d] = usefulFuncs::getFlightDistance(tktkVtx2D, bsVtx2D);
			LbLo.dataD[LbTkRecord::tktkFlightDistanceSig] = usefulFuncs::getFlightDistanceSignificance(tktkVtx2D, tktkCOV, mumuVtx2D, mumuCOV);
			LbLo.dataD[LbTkRecord::tktkCosa2d] = usefulFuncs::getCosAngle(tktkMom2D, tktkVtx2D, mumuVtx2D);
			LbLo.dataD[LbTkRecord::tktkVtxprob] = TMath::Prob(twoTkCandPtr->vertexChi2(), twoTkCandPtr->vertexNdof());
			LbLo.dataD[LbTkRecord::tktknChi2] = twoTkCandPtr->vertexChi2() / twoTkCandPtr->vertexNdof();

			LbLo.dataD[LbTkRecord::pmuPt] = sqrt(muPosP4.Perp2());
			LbLo.dataD[LbTkRecord::pmuP0] = muPosP4.E();
			LbLo.dataD[LbTkRecord::pmuP1] = muPosP4.Px();
			LbLo.dataD[LbTkRecord::pmuP2] = muPosP4.Py();
			LbLo.dataD[LbTkRecord::pmuP3] = muPosP4.Pz();
			LbLo.dataD[LbTkRecord::nmuPt] = sqrt(muNegP4.Perp2());
			LbLo.dataD[LbTkRecord::nmuP0] = muNegP4.E();
			LbLo.dataD[LbTkRecord::nmuP1] = muNegP4.Px();
			LbLo.dataD[LbTkRecord::nmuP2] = muNegP4.Py();
			LbLo.dataD[LbTkRecord::nmuP3] = muNegP4.Pz();

			//LbLo.dataD[LbTkRecord::tk1Pt] = sqrt(tkPosP4.Perp2());
			//LbLo.dataD[LbTkRecord::tk1P0] = tkPosP4.E();
			//LbLo.dataD[LbTkRecord::tk1P1] = tkPosP4.Px();
			//LbLo.dataD[LbTkRecord::tk1P2] = tkPosP4.Py();
			//LbLo.dataD[LbTkRecord::tk1P3] = tkPosP4.Pz();
			//LbLo.dataD[LbTkRecord::tk2Pt] = sqrt(tkNegP4.Perp2());
			//LbLo.dataD[LbTkRecord::tk2P0] = tkNegP4.E();
			//LbLo.dataD[LbTkRecord::tk2P1] = tkNegP4.Px();
			//LbLo.dataD[LbTkRecord::tk2P2] = tkNegP4.Py();
			//LbLo.dataD[LbTkRecord::tk2P3] = tkNegP4.Pz();

			//LbLo.dataD[LbTkRecord::tk1IPt] = selCand.userFloat("TkTk/Proton.IPt");
			//LbLo.dataD[LbTkRecord::tk2IPt] = selCand.userFloat("TkTk/Kaon.IPt");
			//LbLo.dataD[LbTkRecord::tk1IPtErr] = selCand.userFloat("TkTk/Proton.IPt.Error");
			//LbLo.dataD[LbTkRecord::tk2IPtErr] = selCand.userFloat("TkTk/Kaon.IPt.Error");
			//if (selCand.hasUserFloat("TkTk/Proton.dEdx.pixelHrm"))
			//    LbLo.dataD[LbTkRecord::tk1DEDX_pixelHrm] = selCand.userFloat("TkTk/Proton.dEdx.pixelHrm");
			//if (selCand.hasUserFloat("TkTk/Kaon.dEdx.pixelHrm"))
			//    LbLo.dataD[LbTkRecord::tk2DEDX_pixelHrm] = selCand.userFloat("TkTk/Kaon.dEdx.pixelHrm");
			//if (selCand.hasUserFloat("TkTk/Proton.dEdx.Harmonic"))
			//    LbLo.dataD[LbTkRecord::tk1DEDX_Harmonic] = selCand.userFloat("TkTk/Proton.dEdx.Harmonic");
			//if (selCand.hasUserFloat("TkTk/Kaon.dEdx.Harmonic"))
			//    LbLo.dataD[LbTkRecord::tk2DEDX_Harmonic] = selCand.userFloat("TkTk/Kaon.dEdx.Harmonic");
			LbLo.dataI[LbTkRecord::eventEntry] = entry;

			if ( useHLT )
			{
				const edm::TriggerNames& trgNamePool = ev.triggerNames( *HLTRecordHandle );
				for ( int iTrig = 0; iTrig != HLTList::totNum; ++iTrig )
				{
					unsigned trgIndex = trgNamePool.triggerIndex( HLTList::trigName[iTrig] );
					if ( trgIndex != trgNamePool.size() )
					{
						if ( HLTRecordHandle->wasrun(trgIndex) )
						{
							if ( HLTRecordHandle->accept(trgIndex) )
							{
								if ( !HLTRecordHandle->error(trgIndex) )
								{
									LbLo.dataI[LbTkRecord::totallyTriggered] += HLTList::encodeHLT(iTrig);										// pass the HLT
								}
								else
								{ LbLo.dataI[LbTkRecord::trigError] += HLTList::encodeHLT(iTrig); }									// there is error in the trigger.
							}
							else
							{ LbLo.dataI[LbTkRecord::trigReject] += HLTList::encodeHLT(iTrig); }							// the trigger was not accepted in the event.
						}
						else
						{ LbLo.dataI[LbTkRecord::trigNotRun] += HLTList::encodeHLT(iTrig); }						// the trigger was not run in the event.
					}
					else
					{ LbLo.dataI[LbTkRecord::trigVanish] += HLTList::encodeHLT(iTrig); }					// the trigger path is not recorded in the event.
				}
			}
			LbLoTree->Fill();
		}
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
			unsigned candTag = 0;
			unsigned pTkIdx = -1;
			unsigned nTkIdx = -1;
			unsigned tktkIdx = -1;
			unsigned mumuIdx = -1;

			// check for LambdaB->Jpsi p+ K-
			if ( mcCand.pdgId() == 5122 ) candTag += 1 << 0;
			if ( mcCand.pdgId() ==-5122 ) candTag += 1 << 1;
			if ( candTag==0 ) continue;

			for ( unsigned iDau = 0; iDau != mcCand.numberOfDaughters(); ++iDau )
			{	// check daughter
				const reco::GenParticle* daug = dynamic_cast<const reco::GenParticle*>(mcCand.daughter(iDau));
				if ( daug->charge() == 0 )
				{
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
			std::cout << "ptkidx, ntkidx, mumuidx = " << pTkIdx << ", " << nTkIdx << ", " << mumuIdx << "\n";

			const reco::GenParticle* mumuCand = dynamic_cast<const reco::GenParticle*>(mcCand.daughter(mumuIdx));
			const reco::GenParticle* tktkCand = tktkIdx == (unsigned) -1 ? &mcCand : dynamic_cast<const reco::GenParticle*>(mcCand.daughter(tktkIdx));

			const reco::GenParticle* pMuCand = dynamic_cast<const reco::GenParticle*>(mumuCand->daughter(0));
			const reco::GenParticle* nMuCand = dynamic_cast<const reco::GenParticle*>(mumuCand->daughter(1));
			const reco::GenParticle* pTkCand = dynamic_cast<const reco::GenParticle*>(tktkCand->daughter(pTkIdx));
			const reco::GenParticle* nTkCand = dynamic_cast<const reco::GenParticle*>(tktkCand->daughter(nTkIdx));

			if ( pMuCand->charge() < 0) std::cout<<"----VertexCompCandAnalyzer::analyze() : pos mu own neg charge\n";
			if ( nMuCand->charge() > 0) std::cout<<"----VertexCompCandAnalyzer::analyze() : neg mu own pos charge\n";

			GlobalPoint bsVtx2D( bs.x( mcCand.vertex().z() ), bs.y( mcCand.vertex().z() ), 0. );
			GlobalPoint tktkVtx2D( tktkCand->vertex().x(), tktkCand->vertex().y(), 0. );
			GlobalPoint mumuVtx2D( mumuCand->vertex().x(), mumuCand->vertex().y(), 0. );
			GlobalPoint fourTkVtx2D( mcCand.vertex().x(), mcCand.vertex().y(), 0. );
			GlobalVector tktkMom2D( tktkCand->momentum().x(), tktkCand->momentum().y(), 0. );
			GlobalVector fourTkMom2D( mcCand.momentum().x(), mcCand.momentum().y(), 0. );

			mc.dataD[MCRecord::candMass] = mcCand.mass();
			mc.dataD[MCRecord::candPt] = mcCand.pt();
			mc.dataD[MCRecord::candEta] = mcCand.eta();
			mc.dataD[MCRecord::candY] = mcCand.rapidity();
			mc.dataD[MCRecord::candPhi] = mcCand.phi();
			mc.dataD[MCRecord::candFlightDistance2d] = usefulFuncs::getFlightDistance(fourTkVtx2D, bsVtx2D);
			mc.dataD[MCRecord::candCosa2d] = usefulFuncs::getCosAngle(fourTkMom2D, fourTkVtx2D, bsVtx2D);

			if ( tktkIdx != (unsigned) -1 )
			{
				mc.dataD[MCRecord::tktkMass] = tktkCand->mass();
				mc.dataD[MCRecord::tktkPt] = tktkCand->pt();
				mc.dataD[MCRecord::tktkEta] = tktkCand->eta();
				mc.dataD[MCRecord::tktkY] = tktkCand->rapidity();
				mc.dataD[MCRecord::tktkPhi] = tktkCand->phi();
				mc.dataD[MCRecord::tktkFlightDistance2d] = usefulFuncs::getFlightDistance(tktkVtx2D, mumuVtx2D);
				mc.dataD[MCRecord::tktkCosa2d] = usefulFuncs::getCosAngle(tktkMom2D, tktkVtx2D, mumuVtx2D);
			}

			mc.dataD[MCRecord::muPosPt]  = pMuCand->pt();
			mc.dataD[MCRecord::muPosEta] = pMuCand->eta();
			mc.dataD[MCRecord::muPosPhi] = pMuCand->phi();
			mc.dataD[MCRecord::muPosY]   = pMuCand->rapidity();

			mc.dataD[MCRecord::muNegPt]  = nMuCand->pt();
			mc.dataD[MCRecord::muNegEta] = nMuCand->eta();
			mc.dataD[MCRecord::muNegPhi] = nMuCand->phi();
			mc.dataD[MCRecord::muNegY]   = nMuCand->rapidity();

			mc.dataD[MCRecord::tkPosPt]  = pTkCand->pt();
			mc.dataD[MCRecord::tkPosEta] = pTkCand->eta();
			mc.dataD[MCRecord::tkPosPhi] = pTkCand->phi();
			mc.dataD[MCRecord::tkPosY]   = pTkCand->rapidity();

			mc.dataD[MCRecord::tkNegPt]  = nTkCand->pt();
			mc.dataD[MCRecord::tkNegEta] = nTkCand->eta();
			mc.dataD[MCRecord::tkNegPhi] = nTkCand->phi();
			mc.dataD[MCRecord::tkNegY]   = nTkCand->rapidity();

			mc.dataI[MCRecord::candPID] = mcCand.pdgId();
			mc.dataI[MCRecord::muPosPID] = pMuCand->pdgId();
			mc.dataI[MCRecord::muNegPID] = nMuCand->pdgId();
			mc.dataI[MCRecord::tkPosPID] = pTkCand->pdgId();
			mc.dataI[MCRecord::tkNegPID] = nTkCand->pdgId();
			mc.dataI[MCRecord::eventEntry] = entry;

			mcTree->Fill();
		}	// loop all particles end (while loop)
	}	// MC end }}}
endOfMC:

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
