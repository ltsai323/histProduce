#include "histProduce/treeFormat/interface/testGG_formatTreeArrayF.h"
#include "histProduce/treeFormat/interface/untuplizer.h"
#include <stdio.h>
#include <stdlib.h>

void testGG_formatTreeArrayF::RegFormatTree(TTree* t)
{
    if ( t == nullptr )
    {
        printf("testGG_formatTreeArrayF:        ERROR   no input tree, program cannot register tree to record data!\n");
        exit(1);
    }
    this->ReadyForOutputTreeContent();
    t->Branch( "testGGCandSize", &candSize, "testGGCandSize/I" );

    t->Branch( "phoEta", data[phoEta], "phoEta[testGGCandSize]/F" );
	t->Branch( "phoPhi", data[phoPhi], "phoPhi[testGGCandSize]/F" );
	t->Branch( "phoCalibEt", data[phoCalibEt], "phoCalibEt[testGGCandSize]/F" );
	t->Branch( "phoR9", data[phoR9], "phoR9[testGGCandSize]/F" );
	t->Branch( "phoSCEta", data[phoSCEta], "phoSCEta[testGGCandSize]/F" );
	t->Branch( "phoHoverE", data[phoHoverE], "phoHoverE[testGGCandSize]/F" );
	t->Branch( "phoPFChIso", data[phoPFChIso], "phoPFChIso[testGGCandSize]/F" );
	t->Branch( "phoPFNeuIso", data[phoPFNeuIso], "phoPFNeuIso[testGGCandSize]/F" );
	t->Branch( "phoPFPhoIso", data[phoPFPhoIso], "phoPFPhoIso[testGGCandSize]/F" );
	t->Branch( "phoSCRawE", data[phoSCRawE], "phoSCRawE[testGGCandSize]/F" );
	t->Branch( "phoSCEtaWidth", data[phoSCEtaWidth], "phoSCEtaWidth[testGGCandSize]/F" );
	t->Branch( "phoSCPhiWidth", data[phoSCPhiWidth], "phoSCPhiWidth[testGGCandSize]/F" );
	t->Branch( "phoESEnP1", data[phoESEnP1], "phoESEnP1[testGGCandSize]/F" );
	t->Branch( "phoESEnP2", data[phoESEnP2], "phoESEnP2[testGGCandSize]/F" );
	t->Branch( "phoESEffSigmaRR", data[phoESEffSigmaRR], "phoESEffSigmaRR[testGGCandSize]/F" );
	t->Branch( "phoPFChWorstIso", data[phoPFChWorstIso], "phoPFChWorstIso[testGGCandSize]/F" );
	t->Branch( "phoSigmaIEtaIEtaFull5x5", data[phoSigmaIEtaIEtaFull5x5], "phoSigmaIEtaIEtaFull5x5[testGGCandSize]/F" );
	t->Branch( "phoSigmaIEtaIPhiFull5x5", data[phoSigmaIEtaIPhiFull5x5], "phoSigmaIEtaIPhiFull5x5[testGGCandSize]/F" );
	t->Branch( "phoSigmaIPhiIPhiFull5x5", data[phoSigmaIPhiIPhiFull5x5], "phoSigmaIPhiIPhiFull5x5[testGGCandSize]/F" );
	t->Branch( "phoR9Full5x5", data[phoR9Full5x5], "phoR9Full5x5[testGGCandSize]/F" );
	t->Branch( "phoE2x2Full5x5", data[phoE2x2Full5x5], "phoE2x2Full5x5[testGGCandSize]/F" );
	t->Branch( "phoE5x5Full5x5", data[phoE5x5Full5x5], "phoE5x5Full5x5[testGGCandSize]/F" );
	t->Branch( "phoIDMVA", data[phoIDMVA], "phoIDMVA[testGGCandSize]/F" );
	t->Branch( "phoSeedEnergy", data[phoSeedEnergy], "phoSeedEnergy[testGGCandSize]/F" );

    return;
}

void testGG_formatTreeArrayF::LoadFormatSourceBranch(TTree* t)
{ this->ReadyForInputTreeContent(); return; }
void testGG_formatTreeArrayF::LoadEvtContent(TreeReader& evtInfo)
{
    if ( evtInfo.isEmpty() )
        throw std::invalid_argument("testGG_formatTreeArrayF::LoadEvtContent() : imported TTreeReader is empty\n");
    candSize = evtInfo.GetInt("nPho");

    read[phoEta] = evtInfo.GetPtrFloat("phoEta");
	read[phoPhi] = evtInfo.GetPtrFloat("phoPhi");
	read[phoCalibEt] = evtInfo.GetPtrFloat("phoCalibEt");
	read[phoR9] = evtInfo.GetPtrFloat("phoR9");
	read[phoSCEta] = evtInfo.GetPtrFloat("phoSCEta");
	read[phoHoverE] = evtInfo.GetPtrFloat("phoHoverE");
	read[phoPFChIso] = evtInfo.GetPtrFloat("phoPFChIso");
	read[phoPFNeuIso] = evtInfo.GetPtrFloat("phoPFNeuIso");
	read[phoPFPhoIso] = evtInfo.GetPtrFloat("phoPFPhoIso");
	read[phoSCRawE] = evtInfo.GetPtrFloat("phoSCRawE");
	read[phoSCEtaWidth] = evtInfo.GetPtrFloat("phoSCEtaWidth");
	read[phoSCPhiWidth] = evtInfo.GetPtrFloat("phoSCPhiWidth");
	read[phoESEnP1] = evtInfo.GetPtrFloat("phoESEnP1");
	read[phoESEnP2] = evtInfo.GetPtrFloat("phoESEnP2");
	read[phoESEffSigmaRR] = evtInfo.GetPtrFloat("phoESEffSigmaRR");
	read[phoPFChWorstIso] = evtInfo.GetPtrFloat("phoPFChWorstIso");
	read[phoSigmaIEtaIEtaFull5x5] = evtInfo.GetPtrFloat("phoSigmaIEtaIEtaFull5x5");
	read[phoSigmaIEtaIPhiFull5x5] = evtInfo.GetPtrFloat("phoSigmaIEtaIPhiFull5x5");
	read[phoSigmaIPhiIPhiFull5x5] = evtInfo.GetPtrFloat("phoSigmaIPhiIPhiFull5x5");
	read[phoR9Full5x5] = evtInfo.GetPtrFloat("phoR9Full5x5");
	read[phoE2x2Full5x5] = evtInfo.GetPtrFloat("phoE2x2Full5x5");
	read[phoE5x5Full5x5] = evtInfo.GetPtrFloat("phoE5x5Full5x5");
	read[phoIDMVA] = evtInfo.GetPtrFloat("phoIDMVA");
	read[phoSeedEnergy] = evtInfo.GetPtrFloat("phoSeedEnergy");

    return;
}
