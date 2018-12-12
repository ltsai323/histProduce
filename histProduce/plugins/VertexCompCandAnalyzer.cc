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

#include <memory>
#include <string>

#include "histProduce/histProduce/interface/fourMom.h"
#include "histProduce/histProduce/plugins/VertexCompCandAnalyzer.h"

using namespace std;
// Constructor
VertexCompCandAnalyzer::VertexCompCandAnalyzer(const edm::ParameterSet& iConfig) :
pL0BCandsLabel(iConfig.getParameter<std::string>("pL0BCandsLabel")),
nL0BCandsLabel(iConfig.getParameter<std::string>("nL0BCandsLabel")),
LbL0CandsLabel(iConfig.getParameter<std::string>("LbL0CandsLabel")),
LbLoCandsLabel(iConfig.getParameter<std::string>("LbLoCandsLabel")),
pL0BCandsToken(consumes < vector < reco::VertexCompositeCandidate > >(pL0BCandsLabel) ),
nL0BCandsToken(consumes < vector < reco::VertexCompositeCandidate > >(nL0BCandsLabel) ),
LbL0CandsToken(consumes < vector < reco::VertexCompositeCandidate > >(LbL0CandsLabel) ),
LbLoCandsToken(consumes < vector < reco::VertexCompositeCandidate > >(LbLoCandsLabel) ),
  bsPointToken(consumes < reco::BeamSpot > (bsPointLabel) )
{


    usepL0B = (pL0BCandsLabel != "");
    usenL0B = (nL0BCandsLabel != "");
    useLbL0 = (LbL0CandsLabel != "");
    useLbLo = (LbLoCandsLabel != "");
    useBS   = (bsPointLabel   != "");

    std::cout << "  usdpL0B=" << usepL0B;
    std::cout << "  usdnL0B=" << usenL0B;
    std::cout << "  usdLbL0=" << useLbL0;
    std::cout << "  usdLbLo=" << useLbLo;
    std::cout<<std::endl;

    pL0BTree = fs->make < TTree > ("pLbTk", "pLbTk");
    nL0BTree = fs->make < TTree > ("nLbTk", "nLbTk");
    LbL0Tree = fs->make < TTree > ("pLbL0", "pLbL0");
    LbLoTree = fs->make < TTree > ("nLbL0", "nLbL0");
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
    desc.add < string > ("bsPointLabel", "");
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
    //if (!beamSpotHandle.isValid())
    //    return;
    //const reco::BeamSpot & bs = *beamSpotHandle;


    //////////// LbTk ////////////

    if (usepL0B)                // Lb->JPsi p K {{{
    {
        edm::Handle < vector < reco::VertexCompositeCandidate > >pL0BCands;
        ev.getByToken( pL0BCandsToken, pL0BCands );
        // preselection {{{
        if (!pL0BCands.isValid()) return;
        if (pL0BCands->size() == 0) return;

        std::vector < reco::VertexCompositeCandidate >::const_iterator handleIter = pL0BCands->cbegin();
        std::vector < reco::VertexCompositeCandidate >::const_iterator handleIend = pL0BCands->cend  ();
        std::vector < std::pair < double, const reco::VertexCompositeCandidate * >>selectedCandList;
        selectedCandList.clear();
        selectedCandList.reserve(pL0BCands->size());
        handleIter = pL0BCands->begin();
        handleIend = pL0BCands->end();
        while (handleIter != handleIend)
        {
            const reco::VertexCompositeCandidate & cand = *handleIter++;
            const reco::RecoChargedCandidate* muPosCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( cand.daughter("MuPos") );
            const reco::RecoChargedCandidate* muNegCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( cand.daughter("MuNeg") );
            const reco::RecoChargedCandidate* tkPosCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( cand.daughter("Proton") );
            const reco::RecoChargedCandidate* tkNegCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( cand.daughter("Kaon") );

            reco::Particle::LorentzVector tkPosP4 = tkPosCandPtr->p4(); tkPosP4.SetE(sqrt(tkPosP4.P2()+protonMASS*protonMASS));
            reco::Particle::LorentzVector tkNegP4 = tkNegCandPtr->p4(); tkNegP4.SetE(sqrt(tkNegP4.P2()+kaonMASS*kaonMASS));
            //reco::Particle::LorentzVector tktkPreSelP4 = tkPosP4+tkNegP4;
            // preselection
            if (sqrt(muPosCandPtr->momentum().Perp2()) < 4.0) continue;
            if (sqrt(muNegCandPtr->momentum().Perp2()) < 4.0) continue;
            if (sqrt(tkPosCandPtr->momentum().Perp2()) < 0.8) continue;
            if (sqrt(tkNegCandPtr->momentum().Perp2()) < 0.3) continue;

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
            pL0B.Clear();
            const double candVtxprob = selectedCandList[i].first;
            const reco::VertexCompositeCandidate& selCand = *(selectedCandList[i].second);

            const reco::RecoChargedCandidate* muPosCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( selCand.daughter("MuPos") );
            const reco::RecoChargedCandidate* muNegCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( selCand.daughter("MuNeg") );
            const reco::RecoChargedCandidate* tkPosCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( selCand.daughter("Proton") );
            const reco::RecoChargedCandidate* tkNegCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( selCand.daughter("Kaon") );

            reco::Particle::LorentzVector muPosP4 = muPosCandPtr->p4(); muPosP4.SetE(sqrt(muPosP4.P2()+muonMASS*muonMASS));
            reco::Particle::LorentzVector muNegP4 = muNegCandPtr->p4(); muNegP4.SetE(sqrt(muNegP4.P2()+muonMASS*muonMASS));
            reco::Particle::LorentzVector tkPosP4 = tkPosCandPtr->p4(); tkPosP4.SetE(sqrt(tkPosP4.P2()+protonMASS*protonMASS));
            reco::Particle::LorentzVector tkNegP4 = tkNegCandPtr->p4(); tkNegP4.SetE(sqrt(tkNegP4.P2()+kaonMASS*kaonMASS));
            reco::Particle::LorentzVector tktkSelP4 = tkPosP4+tkNegP4;
            reco::Particle::LorentzVector fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;
            // check particles {{{
            {
                // bd
                tkPosP4.SetE(sqrt(tkPosP4.P2()+kaonMASS*kaonMASS));
                tkNegP4.SetE(sqrt(tkNegP4.P2()+pionMASS*pionMASS));
                fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;

                if ( fabs( fourTkSelP4.mag() - bdMASS ) < 2.5* bdWIDTH )
                    pL0B.dataI[LbTkRecord::mightBeOtherParticle] += 1<<1;

                // bd bar
                tkPosP4.SetE(sqrt(tkPosP4.P2()+pionMASS*pionMASS));
                tkNegP4.SetE(sqrt(tkNegP4.P2()+kaonMASS*kaonMASS));
                fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;

                if ( fabs( fourTkSelP4.mag() - bdMASS ) < 2.5* bdWIDTH )
                    pL0B.dataI[LbTkRecord::mightBeOtherParticle] += 1<<2;

                // bs
                tkPosP4.SetE(sqrt(tkPosP4.P2()+kaonMASS*kaonMASS));
                tkNegP4.SetE(sqrt(tkNegP4.P2()+kaonMASS*kaonMASS));
                fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;
                tktkSelP4 = tkPosP4+tkNegP4;

                if ( fabs( fourTkSelP4.mag() - bsMASS ) < 2.5* bsWIDTH &&
                     fabs(   tktkSelP4.mag() -phiMASS ) < 2.5*phiWIDTH )
                    pL0B.dataI[LbTkRecord::mightBeOtherParticle] += 1<<4;

            } // check particles end }}}


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
            //pL0B.dataD[LbTkRecord::lbtkFlightDistance2d] = usefulFuncs::getFlightDistance(selCand, &bs);
            //pL0B.dataD[LbTkRecord::lbtkFlightDistanceSig] = usefulFuncs::getFlightDistanceSignificance(selCand, &bs);
            //const reco::Particle::Point& candVtx = selCand.vertex();
            //pL0B.dataD[LbTkRecord::lbtkCosa2d] = usefulFuncs::getCosa2d(selCand, &bs);
            pL0B.dataD[LbTkRecord::lbtkVtxprob] = candVtxprob;
            pL0B.dataD[LbTkRecord::lbtknChi2] = selCand.vertexChi2() / selCand.vertexNdof();

            pL0B.dataD[LbTkRecord::tktkMass] = tktkMom.mass();
            pL0B.dataD[LbTkRecord::tktkPt] = tktkMom.Pt();
            pL0B.dataD[LbTkRecord::tktkEta] = tktkMom.Eta();
            pL0B.dataD[LbTkRecord::tktkY] = tktkMom.Rapidity();
            pL0B.dataD[LbTkRecord::tktkPhi] = tktkMom.Phi();
            //pL0B.dataD[LbTkRecord::tktkFlightDistance2d] = usefulFuncs::getFlightDistance(tktkCand, &bs);
            //pL0B.dataD[LbTkRecord::tktkFlightDistanceSig] = usefulFuncs::getFlightDistanceSignificance(tktkCand, &bs);
            //const reco::Particle::Point& tktkCandVtx = tkPosCandPtr->vertex();
            //pL0B.dataD[LbTkRecord::tktkCosa2d] = usefulFuncs::getCosa2d(tktkCand, &bs);
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
            //pL0B.dataI[LbTkRecord::eventSize] = usefulFuncs::recordEventSizeWithSeparator(int (N), eventSeparator_pL0B);



            pL0BTree->Fill();
        }
        //eventSeparator_pL0B = usefulFuncs::inverter(eventSeparator_pL0B);
    }                           // Lb->Jpsi p K end }}}

    //////////// anti LbTk ////////////

    if (usenL0B)                // Lb->JPsi P k {{{
    {
        edm::Handle < vector < reco::VertexCompositeCandidate > >nL0BCands;
        ev.getByToken( nL0BCandsToken, nL0BCands );
        // preselection {{{
        if (!nL0BCands.isValid()) return;
        if (nL0BCands->size() == 0) return;

        std::vector < reco::VertexCompositeCandidate >::const_iterator handleIter = nL0BCands->cbegin();
        std::vector < reco::VertexCompositeCandidate >::const_iterator handleIend = nL0BCands->cend  ();
        std::vector < std::pair < double, const reco::VertexCompositeCandidate * >>selectedCandList;
        selectedCandList.clear();
        selectedCandList.reserve(nL0BCands->size());
        handleIter = nL0BCands->begin();
        handleIend = nL0BCands->end();
        while (handleIter != handleIend)
        {
            const reco::VertexCompositeCandidate & cand = *handleIter++;
            const reco::RecoChargedCandidate* muPosCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( cand.daughter("MuPos") );
            const reco::RecoChargedCandidate* muNegCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( cand.daughter("MuNeg") );
            const reco::RecoChargedCandidate* tkPosCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( cand.daughter("Kaon") );
            const reco::RecoChargedCandidate* tkNegCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( cand.daughter("Proton") );

            reco::Particle::LorentzVector tkPosP4 = tkPosCandPtr->p4(); tkPosP4.SetE(sqrt(tkPosP4.P2()+protonMASS*protonMASS));
            reco::Particle::LorentzVector tkNegP4 = tkNegCandPtr->p4(); tkNegP4.SetE(sqrt(tkNegP4.P2()+kaonMASS*kaonMASS));
            //reco::Particle::LorentzVector tktkPreSelP4 = tkPosP4+tkNegP4;
            // preselection
            if (sqrt(muPosCandPtr->momentum().Perp2()) < 4.0) continue;
            if (sqrt(muNegCandPtr->momentum().Perp2()) < 4.0) continue;
            if (sqrt(tkPosCandPtr->momentum().Perp2()) < 0.8) continue;
            if (sqrt(tkNegCandPtr->momentum().Perp2()) < 0.3) continue;

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

            const reco::RecoChargedCandidate* muPosCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( selCand.daughter("MuPos") );
            const reco::RecoChargedCandidate* muNegCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( selCand.daughter("MuNeg") );
            const reco::RecoChargedCandidate* tkPosCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( selCand.daughter("Proton") );
            const reco::RecoChargedCandidate* tkNegCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( selCand.daughter("Kaon") );

            reco::Particle::LorentzVector muPosP4 = muPosCandPtr->p4(); muPosP4.SetE(sqrt(muPosP4.P2()+muonMASS*muonMASS));
            reco::Particle::LorentzVector muNegP4 = muNegCandPtr->p4(); muNegP4.SetE(sqrt(muNegP4.P2()+muonMASS*muonMASS));
            reco::Particle::LorentzVector tkPosP4 = tkPosCandPtr->p4(); tkPosP4.SetE(sqrt(tkPosP4.P2()+kaonMASS*kaonMASS));
            reco::Particle::LorentzVector tkNegP4 = tkNegCandPtr->p4(); tkNegP4.SetE(sqrt(tkNegP4.P2()+protonMASS*protonMASS));
            reco::Particle::LorentzVector tktkSelP4 = tkPosP4+tkNegP4;
            reco::Particle::LorentzVector fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;
            // check particles {{{
            {

                // bd
                tkPosP4.SetE(sqrt(tkPosP4.P2()+kaonMASS*kaonMASS));
                tkNegP4.SetE(sqrt(tkNegP4.P2()+pionMASS*pionMASS));
                fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;

                if ( fabs( fourTkSelP4.mag() - bdMASS ) < 2.5* bdWIDTH )
                    nL0B.dataI[LbTkRecord::mightBeOtherParticle] += 1<<1;

                // bd bar
                tkPosP4.SetE(sqrt(tkPosP4.P2()+pionMASS*pionMASS));
                tkNegP4.SetE(sqrt(tkNegP4.P2()+kaonMASS*kaonMASS));
                fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;

                if ( fabs( fourTkSelP4.mag() - bdMASS ) < 2.5* bdWIDTH )
                    nL0B.dataI[LbTkRecord::mightBeOtherParticle] += 1<<2;

                // bs
                tkPosP4.SetE(sqrt(tkPosP4.P2()+kaonMASS*kaonMASS));
                tkNegP4.SetE(sqrt(tkNegP4.P2()+kaonMASS*kaonMASS));
                fourTkSelP4 = muPosP4+muNegP4+tkPosP4+tkNegP4;
                tktkSelP4 = tkPosP4+tkNegP4;

                if ( fabs( fourTkSelP4.mag() - bsMASS ) < 2.5* bsWIDTH &&
                     fabs(   tktkSelP4.mag() -phiMASS ) < 2.5*phiWIDTH )
                    nL0B.dataI[LbTkRecord::mightBeOtherParticle] += 1<<4;

            } // check particles end }}}


            tkPosP4.SetE(sqrt(tkPosP4.P2()+kaonMASS*kaonMASS));
            tkNegP4.SetE(sqrt(tkNegP4.P2()+protonMASS*protonMASS));
            reco::Particle::LorentzVector fourTkMom = selCand.p4();
            reco::Particle::LorentzVector tktkMom = tkPosP4+tkNegP4;
            //const GlobalVector *twoTk = tktkCand.userData < GlobalVector > ("fitMomentum");
            //TLorentzVector twoTkMom(twoTk->x(), twoTk->y(), twoTk->z(),
            //                        sqrt(twoTk->x() * twoTk->x() + twoTk->y() * twoTk->y() + twoTk->z() * twoTk->z() +
            //                             tktkCand.userFloat("fitMass") * tktkCand.userFloat("fitMass")));
            nL0B.dataD[LbTkRecord::lbtkMass] = selCand.mass();
            nL0B.dataD[LbTkRecord::lbtkPt] = fourTkMom.Pt();
            nL0B.dataD[LbTkRecord::lbtkEta] = fourTkMom.Eta();
            nL0B.dataD[LbTkRecord::lbtkY] = fourTkMom.Rapidity();
            nL0B.dataD[LbTkRecord::lbtkPhi] = fourTkMom.Phi();
            //nL0B.dataD[LbTkRecord::lbtkFlightDistance2d] = usefulFuncs::getFlightDistance(selCand, &bs);
            //nL0B.dataD[LbTkRecord::lbtkFlightDistanceSig] = usefulFuncs::getFlightDistanceSignificance(selCand, &bs);
            //const reco::Particle::Point& candVtx = selCand.vertex();
            //nL0B.dataD[LbTkRecord::lbtkCosa2d] = usefulFuncs::getCosa2d(selCand, &bs);
            nL0B.dataD[LbTkRecord::lbtkVtxprob] = candVtxprob;
            nL0B.dataD[LbTkRecord::lbtknChi2] = selCand.vertexChi2() / selCand.vertexNdof();

            nL0B.dataD[LbTkRecord::tktkMass] = tktkMom.mass();
            nL0B.dataD[LbTkRecord::tktkPt] = tktkMom.Pt();
            nL0B.dataD[LbTkRecord::tktkEta] = tktkMom.Eta();
            nL0B.dataD[LbTkRecord::tktkY] = tktkMom.Rapidity();
            nL0B.dataD[LbTkRecord::tktkPhi] = tktkMom.Phi();
            //nL0B.dataD[LbTkRecord::tktkFlightDistance2d] = usefulFuncs::getFlightDistance(tktkCand, &bs);
            //nL0B.dataD[LbTkRecord::tktkFlightDistanceSig] = usefulFuncs::getFlightDistanceSignificance(tktkCand, &bs);
            //const reco::Particle::Point& tktkCandVtx = tkPosCandPtr->vertex();
            //nL0B.dataD[LbTkRecord::tktkCosa2d] = usefulFuncs::getCosa2d(tktkCand, &bs);
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
            //nL0B.dataI[LbTkRecord::eventSize] = usefulFuncs::recordEventSizeWithSeparator(int (N), eventSeparator_nL0B);



            nL0BTree->Fill();
        }
        //eventSeparator_nL0B = usefulFuncs::inverter(eventSeparator_nL0B);
    }                           // Lb->Jpsi P k end }}}

    //////////// LbL0 ////////////

    if (useLbL0)                // Lb->JPsi Lam0 {{{
    {
        edm::Handle < vector < reco::VertexCompositeCandidate > >LbL0Cands;
        ev.getByToken( LbL0CandsToken, LbL0Cands );
        // preselection {{{
        if (!LbL0Cands.isValid()) return;
        if (LbL0Cands->size() == 0) return;

        std::vector < reco::VertexCompositeCandidate >::const_iterator handleIter = LbL0Cands->cbegin();
        std::vector < reco::VertexCompositeCandidate >::const_iterator handleIend = LbL0Cands->cend  ();
        std::vector < std::pair < double, const reco::VertexCompositeCandidate * >>selectedCandList;
        selectedCandList.clear();
        selectedCandList.reserve(LbL0Cands->size());
        handleIter = LbL0Cands->begin();
        handleIend = LbL0Cands->end();
        while (handleIter != handleIend)
        {
            const reco::VertexCompositeCandidate & cand = *handleIter++;
            const reco::RecoChargedCandidate* muPosCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( cand.daughter("MuPos") );
            const reco::RecoChargedCandidate* muNegCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( cand.daughter("MuNeg") );
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
            LbL0.Clear();
            const double candVtxprob = selectedCandList[i].first;
            const reco::VertexCompositeCandidate& selCand = *(selectedCandList[i].second);

            const reco::RecoChargedCandidate* muPosCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( selCand.daughter("MuPos") );
            const reco::RecoChargedCandidate* muNegCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( selCand.daughter("MuNeg") );
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
            //LbL0.dataD[LbTkRecord::lbtkFlightDistance2d] = usefulFuncs::getFlightDistance(selCand, &bs);
            //LbL0.dataD[LbTkRecord::lbtkFlightDistanceSig] = usefulFuncs::getFlightDistanceSignificance(selCand, &bs);
            //const reco::Particle::Point& candVtx = selCand.vertex();
            //LbL0.dataD[LbTkRecord::lbtkCosa2d] = usefulFuncs::getCosa2d(selCand, &bs);
            LbL0.dataD[LbTkRecord::lbtkVtxprob] = candVtxprob;
            LbL0.dataD[LbTkRecord::lbtknChi2] = selCand.vertexChi2() / selCand.vertexNdof();

            LbL0.dataD[LbTkRecord::tktkMass] = tktkMom.mass();
            LbL0.dataD[LbTkRecord::tktkPt] = tktkMom.Pt();
            LbL0.dataD[LbTkRecord::tktkEta] = tktkMom.Eta();
            LbL0.dataD[LbTkRecord::tktkY] = tktkMom.Rapidity();
            LbL0.dataD[LbTkRecord::tktkPhi] = tktkMom.Phi();
            //LbL0.dataD[LbTkRecord::tktkFlightDistance2d] = usefulFuncs::getFlightDistance(tktkCand, &bs);
            //LbL0.dataD[LbTkRecord::tktkFlightDistanceSig] = usefulFuncs::getFlightDistanceSignificance(tktkCand, &bs);
            //const reco::Particle::Point& tktkCandVtx = twoTkCandPtr->vertex();
            //LbL0.dataD[LbTkRecord::tktkCosa2d] = usefulFuncs::getCosa2d(tktkCand, &bs);
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
            //LbL0.dataI[LbTkRecord::eventSize] = usefulFuncs::recordEventSizeWithSeparator(int (N), eventSeparator_LbL0);



            LbL0Tree->Fill();
        }
        //eventSeparator_LbL0 = usefulFuncs::inverter(eventSeparator_LbL0);
    }                           // Lb->Jpsi Lam0 end }}}


    //////////// LbLo ////////////

    if (useLbLo)                // Lb->JPsi anti Lam0 {{{
    {
        edm::Handle < vector < reco::VertexCompositeCandidate > >LbLoCands;
        ev.getByToken( LbLoCandsToken, LbLoCands );
        // preselection {{{
        if (!LbLoCands.isValid()) return;
        if (LbLoCands->size() == 0) return;

        std::vector < reco::VertexCompositeCandidate >::const_iterator handleIter = LbLoCands->cbegin();
        std::vector < reco::VertexCompositeCandidate >::const_iterator handleIend = LbLoCands->cend  ();
        std::vector < std::pair < double, const reco::VertexCompositeCandidate * >>selectedCandList;
        selectedCandList.clear();
        selectedCandList.reserve(LbLoCands->size());
        handleIter = LbLoCands->begin();
        handleIend = LbLoCands->end();
        while (handleIter != handleIend)
        {
            const reco::VertexCompositeCandidate & cand = *handleIter++;
            const reco::RecoChargedCandidate* muPosCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( cand.daughter("MuPos") );
            const reco::RecoChargedCandidate* muNegCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( cand.daughter("MuNeg") );
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

            const reco::RecoChargedCandidate* muPosCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( selCand.daughter("MuPos") );
            const reco::RecoChargedCandidate* muNegCandPtr = dynamic_cast<const reco::RecoChargedCandidate*>( selCand.daughter("MuNeg") );
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


            reco::Particle::LorentzVector fourTkMom = selCand.p4();
            reco::Particle::LorentzVector tktkMom = twoTkP4;
            //const GlobalVector *twoTk = tktkCand.userData < GlobalVector > ("fitMomentum");
            //TLorentzVector twoTkMom(twoTk->x(), twoTk->y(), twoTk->z(),
            //                        sqrt(twoTk->x() * twoTk->x() + twoTk->y() * twoTk->y() + twoTk->z() * twoTk->z() +
            //                             tktkCand.userFloat("fitMass") * tktkCand.userFloat("fitMass")));
            LbLo.dataD[LbTkRecord::lbtkMass] = selCand.mass();
            LbLo.dataD[LbTkRecord::lbtkPt] = fourTkMom.Pt();
            LbLo.dataD[LbTkRecord::lbtkEta] = fourTkMom.Eta();
            LbLo.dataD[LbTkRecord::lbtkY] = fourTkMom.Rapidity();
            LbLo.dataD[LbTkRecord::lbtkPhi] = fourTkMom.Phi();
            //LbLo.dataD[LbTkRecord::lbtkFlightDistance2d] = usefulFuncs::getFlightDistance(selCand, &bs);
            //LbLo.dataD[LbTkRecord::lbtkFlightDistanceSig] = usefulFuncs::getFlightDistanceSignificance(selCand, &bs);
            //const reco::Particle::Point& candVtx = selCand.vertex();
            //LbLo.dataD[LbTkRecord::lbtkCosa2d] = usefulFuncs::getCosa2d(selCand, &bs);
            LbLo.dataD[LbTkRecord::lbtkVtxprob] = candVtxprob;
            LbLo.dataD[LbTkRecord::lbtknChi2] = selCand.vertexChi2() / selCand.vertexNdof();

            LbLo.dataD[LbTkRecord::tktkMass] = tktkMom.mass();
            LbLo.dataD[LbTkRecord::tktkPt] = tktkMom.Pt();
            LbLo.dataD[LbTkRecord::tktkEta] = tktkMom.Eta();
            LbLo.dataD[LbTkRecord::tktkY] = tktkMom.Rapidity();
            LbLo.dataD[LbTkRecord::tktkPhi] = tktkMom.Phi();
            //LbLo.dataD[LbTkRecord::tktkFlightDistance2d] = usefulFuncs::getFlightDistance(tktkCand, &bs);
            //LbLo.dataD[LbTkRecord::tktkFlightDistanceSig] = usefulFuncs::getFlightDistanceSignificance(tktkCand, &bs);
            //const reco::Particle::Point& tktkCandVtx = twoTkCandPtr->vertex();
            //LbLo.dataD[LbTkRecord::tktkCosa2d] = usefulFuncs::getCosa2d(tktkCand, &bs);
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
            //LbLo.dataI[LbTkRecord::eventSize] = usefulFuncs::recordEventSizeWithSeparator(int (N), eventSeparator_LbLo);



            LbLoTree->Fill();
        }
        //eventSeparator_LbLo = usefulFuncs::inverter(eventSeparator_LbLo);
    }                           // Lb->Jpsi anti Lam0 end }}}

    return;
}


void VertexCompCandAnalyzer::beginJob()
{
    // this event separator will have 3 value: 0,1,2.
    // 0 shows the first event of the file.
    // 1 and 2 are showing the changing event.
    // so that the event separator is 0 for first event. Then 1,2,1,2,1,2 ...

    eventSeparator_pL0B = 0;
    eventSeparator_nL0B = 0;
    eventSeparator_LbL0 = 0;
    eventSeparator_LbLo = 0;
}


void VertexCompCandAnalyzer::endJob()
{
}

//define this as a plug-in
#include "FWCore/PluginManager/interface/ModuleDef.h"

DEFINE_FWK_MODULE(VertexCompCandAnalyzer);
