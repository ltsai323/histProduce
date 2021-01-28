
#include "histProduce/treeFormat/interface/ggPhoton_formatTreeArrayF.h"
#include "histProduce/treeFormat/interface/untuplizer.h"
#include <stdio.h>
#include <stdlib.h>

void ggPhoton_formatTreeArrayF::RegFormatTree(TTree* t)
{
    if ( t == nullptr )
    {
        printf("ggPhoton_formatTreeArrayF:        ERROR   no input tree, program cannot register tree to record data!\n");
        exit(1);
    }
    this->ReadyForOutputTreeContent();

    t->Branch( "ggPhotonCandSize", &candSize, "ggPhotonCandSize/I" );

    t->Branch( "phoE", data[phoE], "phoE[ggPhotonCandSize]/F" );
    t->Branch( "phoSigmaE", data[phoSigmaE], "phoSigmaE[ggPhotonCandSize]/F" );
    t->Branch( "phoEt", data[phoEt], "phoEt[ggPhotonCandSize]/F" );
    t->Branch( "phoEta", data[phoEta], "phoEta[ggPhotonCandSize]/F" );
    t->Branch( "phoPhi", data[phoPhi], "phoPhi[ggPhotonCandSize]/F" );
    t->Branch( "phoCalibE", data[phoCalibE], "phoCalibE[ggPhotonCandSize]/F" );
    t->Branch( "phoCalibEt", data[phoCalibEt], "phoCalibEt[ggPhotonCandSize]/F" );
    t->Branch( "phoSCE", data[phoSCE], "phoSCE[ggPhotonCandSize]/F" );
    t->Branch( "phoSCRawE", data[phoSCRawE], "phoSCRawE[ggPhotonCandSize]/F" );
    t->Branch( "phoESEnP1", data[phoESEnP1], "phoESEnP1[ggPhotonCandSize]/F" );
    t->Branch( "phoESEnP2", data[phoESEnP2], "phoESEnP2[ggPhotonCandSize]/F" );
    t->Branch( "phoSCEta", data[phoSCEta], "phoSCEta[ggPhotonCandSize]/F" );
    t->Branch( "phoSCPhi", data[phoSCPhi], "phoSCPhi[ggPhotonCandSize]/F" );
    t->Branch( "phoSCEtaWidth", data[phoSCEtaWidth], "phoSCEtaWidth[ggPhotonCandSize]/F" );
    t->Branch( "phoSCPhiWidth", data[phoSCPhiWidth], "phoSCPhiWidth[ggPhotonCandSize]/F" );
    t->Branch( "phoSCBrem", data[phoSCBrem], "phoSCBrem[ggPhotonCandSize]/F" );
    t->Branch( "phoR9", data[phoR9], "phoR9[ggPhotonCandSize]/F" );
    t->Branch( "phoHoverE", data[phoHoverE], "phoHoverE[ggPhotonCandSize]/F" );
    t->Branch( "phoESEffSigmaRR", data[phoESEffSigmaRR], "phoESEffSigmaRR[ggPhotonCandSize]/F" );
    t->Branch( "phoSigmaIEtaIEtaFull5x5", data[phoSigmaIEtaIEtaFull5x5], "phoSigmaIEtaIEtaFull5x5[ggPhotonCandSize]/F" );
    t->Branch( "phoSigmaIEtaIPhiFull5x5", data[phoSigmaIEtaIPhiFull5x5], "phoSigmaIEtaIPhiFull5x5[ggPhotonCandSize]/F" );
    t->Branch( "phoSigmaIPhiIPhiFull5x5", data[phoSigmaIPhiIPhiFull5x5], "phoSigmaIPhiIPhiFull5x5[ggPhotonCandSize]/F" );
    t->Branch( "phoE2x2Full5x5", data[phoE2x2Full5x5], "phoE2x2Full5x5[ggPhotonCandSize]/F" );
    t->Branch( "phoE5x5Full5x5", data[phoE5x5Full5x5], "phoE5x5Full5x5[ggPhotonCandSize]/F" );
    t->Branch( "phoR9Full5x5", data[phoR9Full5x5], "phoR9Full5x5[ggPhotonCandSize]/F" );
    t->Branch( "phoPFChIso", data[phoPFChIso], "phoPFChIso[ggPhotonCandSize]/F" );
    t->Branch( "phoPFChPVIso", data[phoPFChPVIso], "phoPFChPVIso[ggPhotonCandSize]/F" );
    t->Branch( "phoPFPhoIso", data[phoPFPhoIso], "phoPFPhoIso[ggPhotonCandSize]/F" );
    t->Branch( "phoPFNeuIso", data[phoPFNeuIso], "phoPFNeuIso[ggPhotonCandSize]/F" );
    t->Branch( "phoPFChWorstIso", data[phoPFChWorstIso], "phoPFChWorstIso[ggPhotonCandSize]/F" );
    t->Branch( "phoPFChWorstVetoIso", data[phoPFChWorstVetoIso], "phoPFChWorstVetoIso[ggPhotonCandSize]/F" );
    t->Branch( "phoEcalPFClusterIso", data[phoEcalPFClusterIso], "phoEcalPFClusterIso[ggPhotonCandSize]/F" );
    t->Branch( "phoHcalPFClusterIso", data[phoHcalPFClusterIso], "phoHcalPFClusterIso[ggPhotonCandSize]/F" );
    t->Branch( "phoIDMVA", data[phoIDMVA], "phoIDMVA[ggPhotonCandSize]/F" );
    t->Branch( "phoSeedTime", data[phoSeedTime], "phoSeedTime[ggPhotonCandSize]/F" );
    t->Branch( "phoSeedEnergy", data[phoSeedEnergy], "phoSeedEnergy[ggPhotonCandSize]/F" );
    t->Branch( "phoMIPTotEnergy", data[phoMIPTotEnergy], "phoMIPTotEnergy[ggPhotonCandSize]/F" );
    t->Branch( "phoMIPChi2", data[phoMIPChi2], "phoMIPChi2[ggPhotonCandSize]/F" );
    t->Branch( "phoMIPSlope", data[phoMIPSlope], "phoMIPSlope[ggPhotonCandSize]/F" );
    t->Branch( "phoMIPIntercept", data[phoMIPIntercept], "phoMIPIntercept[ggPhotonCandSize]/F" );
    t->Branch( "phoMIPNhitCone", data[phoMIPNhitCone], "phoMIPNhitCone[ggPhotonCandSize]/F" );
    t->Branch( "phoMIPIsHalo", data[phoMIPIsHalo], "phoMIPIsHalo[ggPhotonCandSize]/F" );
    t->Branch( "phoScale_stat_up", data[phoScale_stat_up], "phoScale_stat_up[ggPhotonCandSize]/F" );
    t->Branch( "phoScale_stat_dn", data[phoScale_stat_dn], "phoScale_stat_dn[ggPhotonCandSize]/F" );
    t->Branch( "phoScale_syst_up", data[phoScale_syst_up], "phoScale_syst_up[ggPhotonCandSize]/F" );
    t->Branch( "phoScale_syst_dn", data[phoScale_syst_dn], "phoScale_syst_dn[ggPhotonCandSize]/F" );
    t->Branch( "phoScale_gain_up", data[phoScale_gain_up], "phoScale_gain_up[ggPhotonCandSize]/F" );
    t->Branch( "phoScale_gain_dn", data[phoScale_gain_dn], "phoScale_gain_dn[ggPhotonCandSize]/F" );
    t->Branch( "phoResol_rho_up", data[phoResol_rho_up], "phoResol_rho_up[ggPhotonCandSize]/F" );
    t->Branch( "phoResol_rho_dn", data[phoResol_rho_dn], "phoResol_rho_dn[ggPhotonCandSize]/F" );
    t->Branch( "phoResol_phi_up", data[phoResol_phi_up], "phoResol_phi_up[ggPhotonCandSize]/F" );
    t->Branch( "phoResol_phi_dn", data[phoResol_phi_dn], "phoResol_phi_dn[ggPhotonCandSize]/F" );

    return;
}

void ggPhoton_formatTreeArrayF::LoadFormatSourceBranch(TTree* t)
{
    this->ReadyForInputTreeContent();
    return;

    // t->SetBranchAddress( "ggPhotonCandSize", &candSize ); // this line need to be modified.

    t->SetBranchAddress( "phoE", read[phoE] );
    t->SetBranchAddress( "phoSigmaE", read[phoSigmaE] );
    t->SetBranchAddress( "phoEt", read[phoEt] );
    t->SetBranchAddress( "phoEta", read[phoEta] );
    t->SetBranchAddress( "phoPhi", read[phoPhi] );
    t->SetBranchAddress( "phoCalibE", read[phoCalibE] );
    t->SetBranchAddress( "phoCalibEt", read[phoCalibEt] );
    t->SetBranchAddress( "phoSCE", read[phoSCE] );
    t->SetBranchAddress( "phoSCRawE", read[phoSCRawE] );
    t->SetBranchAddress( "phoESEnP1", read[phoESEnP1] );
    t->SetBranchAddress( "phoESEnP2", read[phoESEnP2] );
    t->SetBranchAddress( "phoSCEta", read[phoSCEta] );
    t->SetBranchAddress( "phoSCPhi", read[phoSCPhi] );
    t->SetBranchAddress( "phoSCEtaWidth", read[phoSCEtaWidth] );
    t->SetBranchAddress( "phoSCPhiWidth", read[phoSCPhiWidth] );
    t->SetBranchAddress( "phoSCBrem", read[phoSCBrem] );
    t->SetBranchAddress( "phoR9", read[phoR9] );
    t->SetBranchAddress( "phoHoverE", read[phoHoverE] );
    t->SetBranchAddress( "phoESEffSigmaRR", read[phoESEffSigmaRR] );
    t->SetBranchAddress( "phoSigmaIEtaIEtaFull5x5", read[phoSigmaIEtaIEtaFull5x5] );
    t->SetBranchAddress( "phoSigmaIEtaIPhiFull5x5", read[phoSigmaIEtaIPhiFull5x5] );
    t->SetBranchAddress( "phoSigmaIPhiIPhiFull5x5", read[phoSigmaIPhiIPhiFull5x5] );
    t->SetBranchAddress( "phoE2x2Full5x5", read[phoE2x2Full5x5] );
    t->SetBranchAddress( "phoE5x5Full5x5", read[phoE5x5Full5x5] );
    t->SetBranchAddress( "phoR9Full5x5", read[phoR9Full5x5] );
    t->SetBranchAddress( "phoPFChIso", read[phoPFChIso] );
    t->SetBranchAddress( "phoPFChPVIso", read[phoPFChPVIso] );
    t->SetBranchAddress( "phoPFPhoIso", read[phoPFPhoIso] );
    t->SetBranchAddress( "phoPFNeuIso", read[phoPFNeuIso] );
    t->SetBranchAddress( "phoPFChWorstIso", read[phoPFChWorstIso] );
    t->SetBranchAddress( "phoPFChWorstVetoIso", read[phoPFChWorstVetoIso] );
    t->SetBranchAddress( "phoEcalPFClusterIso", read[phoEcalPFClusterIso] );
    t->SetBranchAddress( "phoHcalPFClusterIso", read[phoHcalPFClusterIso] );
    t->SetBranchAddress( "phoIDMVA", read[phoIDMVA] );
    t->SetBranchAddress( "phoSeedTime", read[phoSeedTime] );
    t->SetBranchAddress( "phoSeedEnergy", read[phoSeedEnergy] );
    t->SetBranchAddress( "phoMIPTotEnergy", read[phoMIPTotEnergy] );
    t->SetBranchAddress( "phoMIPChi2", read[phoMIPChi2] );
    t->SetBranchAddress( "phoMIPSlope", read[phoMIPSlope] );
    t->SetBranchAddress( "phoMIPIntercept", read[phoMIPIntercept] );
    t->SetBranchAddress( "phoMIPNhitCone", read[phoMIPNhitCone] );
    t->SetBranchAddress( "phoMIPIsHalo", read[phoMIPIsHalo] );
    t->SetBranchAddress( "phoScale_stat_up", read[phoScale_stat_up] );
    t->SetBranchAddress( "phoScale_stat_dn", read[phoScale_stat_dn] );
    t->SetBranchAddress( "phoScale_syst_up", read[phoScale_syst_up] );
    t->SetBranchAddress( "phoScale_syst_dn", read[phoScale_syst_dn] );
    t->SetBranchAddress( "phoScale_gain_up", read[phoScale_gain_up] );
    t->SetBranchAddress( "phoScale_gain_dn", read[phoScale_gain_dn] );
    t->SetBranchAddress( "phoResol_rho_up", read[phoResol_rho_up] );
    t->SetBranchAddress( "phoResol_rho_dn", read[phoResol_rho_dn] );
    t->SetBranchAddress( "phoResol_phi_up", read[phoResol_phi_up] );
    t->SetBranchAddress( "phoResol_phi_dn", read[phoResol_phi_dn] );
    return;
}
void ggPhoton_formatTreeArrayF::LoadEvtContent(TreeReader& evtInfo)
{
    if ( evtInfo.isEmpty() )
        throw std::invalid_argument("ggPhoton_formatTreeArrayF::LoadEvtContent() : imported TTreeReader is empty\n");
    candSize = evtInfo.GetInt("nPho");
    read[phoE] = evtInfo.GetPtrFloat("phoE");
    read[phoSigmaE] = evtInfo.GetPtrFloat("phoSigmaE");
    read[phoEt] = evtInfo.GetPtrFloat("phoEt");
    read[phoEta] = evtInfo.GetPtrFloat("phoEta");
    read[phoPhi] = evtInfo.GetPtrFloat("phoPhi");
    read[phoCalibE] = evtInfo.GetPtrFloat("phoCalibE");
    read[phoCalibEt] = evtInfo.GetPtrFloat("phoCalibEt");
    read[phoSCE] = evtInfo.GetPtrFloat("phoSCE");
    read[phoSCRawE] = evtInfo.GetPtrFloat("phoSCRawE");
    read[phoESEnP1] = evtInfo.GetPtrFloat("phoESEnP1");
    read[phoESEnP2] = evtInfo.GetPtrFloat("phoESEnP2");
    read[phoSCEta] = evtInfo.GetPtrFloat("phoSCEta");
    read[phoSCPhi] = evtInfo.GetPtrFloat("phoSCPhi");
    read[phoSCEtaWidth] = evtInfo.GetPtrFloat("phoSCEtaWidth");
    read[phoSCPhiWidth] = evtInfo.GetPtrFloat("phoSCPhiWidth");
    read[phoSCBrem] = evtInfo.GetPtrFloat("phoSCBrem");
    read[phoR9] = evtInfo.GetPtrFloat("phoR9");
    read[phoHoverE] = evtInfo.GetPtrFloat("phoHoverE");
    read[phoESEffSigmaRR] = evtInfo.GetPtrFloat("phoESEffSigmaRR");
    read[phoSigmaIEtaIEtaFull5x5] = evtInfo.GetPtrFloat("phoSigmaIEtaIEtaFull5x5");
    read[phoSigmaIEtaIPhiFull5x5] = evtInfo.GetPtrFloat("phoSigmaIEtaIPhiFull5x5");
    read[phoSigmaIPhiIPhiFull5x5] = evtInfo.GetPtrFloat("phoSigmaIPhiIPhiFull5x5");
    read[phoE2x2Full5x5] = evtInfo.GetPtrFloat("phoE2x2Full5x5");
    read[phoE5x5Full5x5] = evtInfo.GetPtrFloat("phoE5x5Full5x5");
    read[phoR9Full5x5] = evtInfo.GetPtrFloat("phoR9Full5x5");
    read[phoPFChIso] = evtInfo.GetPtrFloat("phoPFChIso");
    read[phoPFChPVIso] = evtInfo.GetPtrFloat("phoPFChPVIso");
    read[phoPFPhoIso] = evtInfo.GetPtrFloat("phoPFPhoIso");
    read[phoPFNeuIso] = evtInfo.GetPtrFloat("phoPFNeuIso");
    read[phoPFChWorstIso] = evtInfo.GetPtrFloat("phoPFChWorstIso");
    read[phoPFChWorstVetoIso] = evtInfo.GetPtrFloat("phoPFChWorstVetoIso");
    read[phoEcalPFClusterIso] = evtInfo.GetPtrFloat("phoEcalPFClusterIso");
    read[phoHcalPFClusterIso] = evtInfo.GetPtrFloat("phoHcalPFClusterIso");
    read[phoIDMVA] = evtInfo.GetPtrFloat("phoIDMVA");
    read[phoSeedTime] = evtInfo.GetPtrFloat("phoSeedTime");
    read[phoSeedEnergy] = evtInfo.GetPtrFloat("phoSeedEnergy");
    read[phoMIPTotEnergy] = evtInfo.GetPtrFloat("phoMIPTotEnergy");
    read[phoMIPChi2] = evtInfo.GetPtrFloat("phoMIPChi2");
    read[phoMIPSlope] = evtInfo.GetPtrFloat("phoMIPSlope");
    read[phoMIPIntercept] = evtInfo.GetPtrFloat("phoMIPIntercept");
    read[phoMIPNhitCone] = evtInfo.GetPtrFloat("phoMIPNhitCone");
    read[phoMIPIsHalo] = evtInfo.GetPtrFloat("phoMIPIsHalo");
    read[phoScale_stat_up] = evtInfo.GetPtrFloat("phoScale_stat_up");
    read[phoScale_stat_dn] = evtInfo.GetPtrFloat("phoScale_stat_dn");
    read[phoScale_syst_up] = evtInfo.GetPtrFloat("phoScale_syst_up");
    read[phoScale_syst_dn] = evtInfo.GetPtrFloat("phoScale_syst_dn");
    read[phoScale_gain_up] = evtInfo.GetPtrFloat("phoScale_gain_up");
    read[phoScale_gain_dn] = evtInfo.GetPtrFloat("phoScale_gain_dn");
    read[phoResol_rho_up] = evtInfo.GetPtrFloat("phoResol_rho_up");
    read[phoResol_rho_dn] = evtInfo.GetPtrFloat("phoResol_rho_dn");
    read[phoResol_phi_up] = evtInfo.GetPtrFloat("phoResol_phi_up");
    read[phoResol_phi_dn] = evtInfo.GetPtrFloat("phoResol_phi_dn");

    return;
}
