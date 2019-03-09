
# include "histProduce/histProduce/interface/formatTreeArrayLbTk.h"
# include <stdio.h>
# include <stdlib.h>

void formatTreeArray_LbTk::RegFormatTree(TTree* t)
{
    if ( t == nullptr )
    {
        printf("formatTreeArray LbTk:        error to get tree to register!\n");
        exit(1);
    }
    t->Branch( "candSize", &candSize, "candSize/I" );
    t->Branch( "lbtkMass", dataD[lbtkMass], "lbtkMass[candSize]/D" );
	t->Branch( "lbtkPt", dataD[lbtkPt], "lbtkPt[candSize]/D" );
	t->Branch( "lbtkEta", dataD[lbtkEta], "lbtkEta[candSize]/D" );
	t->Branch( "lbtkY", dataD[lbtkY], "lbtkY[candSize]/D" );
	t->Branch( "lbtkPhi", dataD[lbtkPhi], "lbtkPhi[candSize]/D" );
	t->Branch( "lbtkFlightDistance2d", dataD[lbtkFlightDistance2d], "lbtkFlightDistance2d[candSize]/D" );
	t->Branch( "lbtkFlightDistanceSig", dataD[lbtkFlightDistanceSig], "lbtkFlightDistanceSig[candSize]/D" );
	t->Branch( "lbtkVtxprob", dataD[lbtkVtxprob], "lbtkVtxprob[candSize]/D" );
	t->Branch( "lbtkCosa2d", dataD[lbtkCosa2d], "lbtkCosa2d[candSize]/D" );
	t->Branch( "lbtknChi2", dataD[lbtknChi2], "lbtknChi2[candSize]/D" );
	t->Branch( "tktkMass", dataD[tktkMass], "tktkMass[candSize]/D" );
	t->Branch( "tktkPt", dataD[tktkPt], "tktkPt[candSize]/D" );
	t->Branch( "tktkEta", dataD[tktkEta], "tktkEta[candSize]/D" );
	t->Branch( "tktkY", dataD[tktkY], "tktkY[candSize]/D" );
	t->Branch( "tktkPhi", dataD[tktkPhi], "tktkPhi[candSize]/D" );
	t->Branch( "tktkFlightDistance2d", dataD[tktkFlightDistance2d], "tktkFlightDistance2d[candSize]/D" );
	t->Branch( "tktkFlightDistanceSig", dataD[tktkFlightDistanceSig], "tktkFlightDistanceSig[candSize]/D" );
	t->Branch( "tktkVtxprob", dataD[tktkVtxprob], "tktkVtxprob[candSize]/D" );
	t->Branch( "tktkCosa2d", dataD[tktkCosa2d], "tktkCosa2d[candSize]/D" );
	t->Branch( "tktknChi2", dataD[tktknChi2], "tktknChi2[candSize]/D" );
	t->Branch( "pmuPt", dataD[pmuPt], "pmuPt[candSize]/D" );
	t->Branch( "pmuP0", dataD[pmuP0], "pmuP0[candSize]/D" );
	t->Branch( "pmuP1", dataD[pmuP1], "pmuP1[candSize]/D" );
	t->Branch( "pmuP2", dataD[pmuP2], "pmuP2[candSize]/D" );
	t->Branch( "pmuP3", dataD[pmuP3], "pmuP3[candSize]/D" );
	t->Branch( "nmuPt", dataD[nmuPt], "nmuPt[candSize]/D" );
	t->Branch( "nmuP0", dataD[nmuP0], "nmuP0[candSize]/D" );
	t->Branch( "nmuP1", dataD[nmuP1], "nmuP1[candSize]/D" );
	t->Branch( "nmuP2", dataD[nmuP2], "nmuP2[candSize]/D" );
	t->Branch( "nmuP3", dataD[nmuP3], "nmuP3[candSize]/D" );
	t->Branch( "tk1Pt", dataD[tk1Pt], "tk1Pt[candSize]/D" );
	t->Branch( "tk1P0", dataD[tk1P0], "tk1P0[candSize]/D" );
	t->Branch( "tk1P1", dataD[tk1P1], "tk1P1[candSize]/D" );
	t->Branch( "tk1P2", dataD[tk1P2], "tk1P2[candSize]/D" );
	t->Branch( "tk1P3", dataD[tk1P3], "tk1P3[candSize]/D" );
	t->Branch( "tk1DEDX_Harmonic", dataD[tk1DEDX_Harmonic], "tk1DEDX_Harmonic[candSize]/D" );
	t->Branch( "tk1DEDX_pixelHrm", dataD[tk1DEDX_pixelHrm], "tk1DEDX_pixelHrm[candSize]/D" );
	t->Branch( "tk1IPt", dataD[tk1IPt], "tk1IPt[candSize]/D" );
	t->Branch( "tk1IPtErr", dataD[tk1IPtErr], "tk1IPtErr[candSize]/D" );
	t->Branch( "tk2Pt", dataD[tk2Pt], "tk2Pt[candSize]/D" );
	t->Branch( "tk2P0", dataD[tk2P0], "tk2P0[candSize]/D" );
	t->Branch( "tk2P1", dataD[tk2P1], "tk2P1[candSize]/D" );
	t->Branch( "tk2P2", dataD[tk2P2], "tk2P2[candSize]/D" );
	t->Branch( "tk2P3", dataD[tk2P3], "tk2P3[candSize]/D" );
	t->Branch( "tk2DEDX_Harmonic", dataD[tk2DEDX_Harmonic], "tk2DEDX_Harmonic[candSize]/D" );
	t->Branch( "tk2DEDX_pixelHrm", dataD[tk2DEDX_pixelHrm], "tk2DEDX_pixelHrm[candSize]/D" );
	t->Branch( "tk2IPt", dataD[tk2IPt], "tk2IPt[candSize]/D" );
	t->Branch( "tk2IPtErr", dataD[tk2IPtErr], "tk2IPtErr[candSize]/D" );

    t->Branch( "eventEntry", dataI[eventEntry], "eventEntry[candSize]/I" );
	t->Branch( "trigVanish", dataI[trigVanish], "trigVanish[candSize]/I" );
	t->Branch( "trigNotRun", dataI[trigNotRun], "trigNotRun[candSize]/I" );
	t->Branch( "trigReject", dataI[trigReject], "trigReject[candSize]/I" );
	t->Branch( "trigError", dataI[trigError], "trigError[candSize]/I" );
	t->Branch( "totallyTriggered", dataI[totallyTriggered], "totallyTriggered[candSize]/I" );

    return;
}

void formatTreeArray_LbTk::LoadFormatSourceBranch(TTree* t)
{
    if ( t == nullptr )
    {
        printf("formatTree LbTknew:        error to old tree to read!\n");
        exit(1);
    }
    SetReadMode();
    t->SetBranchAddress( "candSize", &candSize );
    t->SetBranchAddress( "lbtkMass", readD[lbtkMass] );
	t->SetBranchAddress( "lbtkPt", readD[lbtkPt] );
	t->SetBranchAddress( "lbtkEta", readD[lbtkEta] );
	t->SetBranchAddress( "lbtkY", readD[lbtkY] );
	t->SetBranchAddress( "lbtkPhi", readD[lbtkPhi] );
	t->SetBranchAddress( "lbtkFlightDistance2d", readD[lbtkFlightDistance2d] );
	t->SetBranchAddress( "lbtkFlightDistanceSig", readD[lbtkFlightDistanceSig] );
	t->SetBranchAddress( "lbtkVtxprob", readD[lbtkVtxprob] );
	t->SetBranchAddress( "lbtkCosa2d", readD[lbtkCosa2d] );
	t->SetBranchAddress( "lbtknChi2", readD[lbtknChi2] );
	t->SetBranchAddress( "tktkMass", readD[tktkMass] );
	t->SetBranchAddress( "tktkPt", readD[tktkPt] );
	t->SetBranchAddress( "tktkEta", readD[tktkEta] );
	t->SetBranchAddress( "tktkY", readD[tktkY] );
	t->SetBranchAddress( "tktkPhi", readD[tktkPhi] );
	t->SetBranchAddress( "tktkFlightDistance2d", readD[tktkFlightDistance2d] );
	t->SetBranchAddress( "tktkFlightDistanceSig", readD[tktkFlightDistanceSig] );
	t->SetBranchAddress( "tktkVtxprob", readD[tktkVtxprob] );
	t->SetBranchAddress( "tktkCosa2d", readD[tktkCosa2d] );
	t->SetBranchAddress( "tktknChi2", readD[tktknChi2] );
	t->SetBranchAddress( "pmuPt", readD[pmuPt] );
	t->SetBranchAddress( "pmuP0", readD[pmuP0] );
	t->SetBranchAddress( "pmuP1", readD[pmuP1] );
	t->SetBranchAddress( "pmuP2", readD[pmuP2] );
	t->SetBranchAddress( "pmuP3", readD[pmuP3] );
	t->SetBranchAddress( "nmuPt", readD[nmuPt] );
	t->SetBranchAddress( "nmuP0", readD[nmuP0] );
	t->SetBranchAddress( "nmuP1", readD[nmuP1] );
	t->SetBranchAddress( "nmuP2", readD[nmuP2] );
	t->SetBranchAddress( "nmuP3", readD[nmuP3] );
	t->SetBranchAddress( "tk1Pt", readD[tk1Pt] );
	t->SetBranchAddress( "tk1P0", readD[tk1P0] );
	t->SetBranchAddress( "tk1P1", readD[tk1P1] );
	t->SetBranchAddress( "tk1P2", readD[tk1P2] );
	t->SetBranchAddress( "tk1P3", readD[tk1P3] );
	t->SetBranchAddress( "tk1DEDX_Harmonic", readD[tk1DEDX_Harmonic] );
	t->SetBranchAddress( "tk1DEDX_pixelHrm", readD[tk1DEDX_pixelHrm] );
	t->SetBranchAddress( "tk1IPt", readD[tk1IPt] );
	t->SetBranchAddress( "tk1IPtErr", readD[tk1IPtErr] );
	t->SetBranchAddress( "tk2Pt", readD[tk2Pt] );
	t->SetBranchAddress( "tk2P0", readD[tk2P0] );
	t->SetBranchAddress( "tk2P1", readD[tk2P1] );
	t->SetBranchAddress( "tk2P2", readD[tk2P2] );
	t->SetBranchAddress( "tk2P3", readD[tk2P3] );
	t->SetBranchAddress( "tk2DEDX_Harmonic", readD[tk2DEDX_Harmonic] );
	t->SetBranchAddress( "tk2DEDX_pixelHrm", readD[tk2DEDX_pixelHrm] );
	t->SetBranchAddress( "tk2IPt", readD[tk2IPt] );
	t->SetBranchAddress( "tk2IPtErr", readD[tk2IPtErr] );

    t->SetBranchAddress( "eventEntry", readI[eventEntry] );
	t->SetBranchAddress( "trigVanish", readI[trigVanish] );
	t->SetBranchAddress( "trigNotRun", readI[trigNotRun] );
	t->SetBranchAddress( "trigReject", readI[trigReject] );
	t->SetBranchAddress( "trigError", readI[trigError] );
	t->SetBranchAddress( "totallyTriggered", readI[totallyTriggered] );

    return;
}
