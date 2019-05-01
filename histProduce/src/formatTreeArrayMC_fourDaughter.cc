
# include "histProduce/histProduce/interface/formatTreeArrayMC_fourDaughter.h"
# include <stdio.h>
# include <stdlib.h>

void formatTreeArray_MC_fourDaughter::RegFormatTree(TTree* t)
{
    if ( t == nullptr )
    {
        printf("formatTreeArray MC_fourDaughter:        ERROR   no input tree, program cannot register tree to record data!\n");
        exit(1);
    }
    t->Branch( "candSize", &candSize, "candSize/I" );

    t->Branch( "candMass", dataD[candMass], "candMass[candSize]/D" );
	t->Branch( "candPt", dataD[candPt], "candPt[candSize]/D" );
	t->Branch( "candEta", dataD[candEta], "candEta[candSize]/D" );
	t->Branch( "candY", dataD[candY], "candY[candSize]/D" );
	t->Branch( "candPhi", dataD[candPhi], "candPhi[candSize]/D" );
	t->Branch( "candFlightDistance2d", dataD[candFlightDistance2d], "candFlightDistance2d[candSize]/D" );
	t->Branch( "candCosa2d", dataD[candCosa2d], "candCosa2d[candSize]/D" );
	t->Branch( "tktkMass", dataD[tktkMass], "tktkMass[candSize]/D" );
	t->Branch( "tktkPt", dataD[tktkPt], "tktkPt[candSize]/D" );
	t->Branch( "tktkEta", dataD[tktkEta], "tktkEta[candSize]/D" );
	t->Branch( "tktkY", dataD[tktkY], "tktkY[candSize]/D" );
	t->Branch( "tktkPhi", dataD[tktkPhi], "tktkPhi[candSize]/D" );
	t->Branch( "tktkFlightDistance2d", dataD[tktkFlightDistance2d], "tktkFlightDistance2d[candSize]/D" );
	t->Branch( "tktkCosa2d", dataD[tktkCosa2d], "tktkCosa2d[candSize]/D" );
	t->Branch( "muPosPt", dataD[muPosPt], "muPosPt[candSize]/D" );
	t->Branch( "muPosEta", dataD[muPosEta], "muPosEta[candSize]/D" );
	t->Branch( "muPosY", dataD[muPosY], "muPosY[candSize]/D" );
	t->Branch( "muPosPhi", dataD[muPosPhi], "muPosPhi[candSize]/D" );
	t->Branch( "muNegPt", dataD[muNegPt], "muNegPt[candSize]/D" );
	t->Branch( "muNegEta", dataD[muNegEta], "muNegEta[candSize]/D" );
	t->Branch( "muNegY", dataD[muNegY], "muNegY[candSize]/D" );
	t->Branch( "muNegPhi", dataD[muNegPhi], "muNegPhi[candSize]/D" );
	t->Branch( "tkPosPt", dataD[tkPosPt], "tkPosPt[candSize]/D" );
	t->Branch( "tkPosEta", dataD[tkPosEta], "tkPosEta[candSize]/D" );
	t->Branch( "tkPosY", dataD[tkPosY], "tkPosY[candSize]/D" );
	t->Branch( "tkPosPhi", dataD[tkPosPhi], "tkPosPhi[candSize]/D" );
	t->Branch( "tkNegPt", dataD[tkNegPt], "tkNegPt[candSize]/D" );
	t->Branch( "tkNegEta", dataD[tkNegEta], "tkNegEta[candSize]/D" );
	t->Branch( "tkNegY", dataD[tkNegY], "tkNegY[candSize]/D" );
	t->Branch( "tkNegPhi", dataD[tkNegPhi], "tkNegPhi[candSize]/D" );

    t->Branch( "eventEntry", dataI[eventEntry], "eventEntry[candSize]/I" );
	t->Branch( "candPID", dataI[candPID], "candPID[candSize]/I" );
	t->Branch( "muPosPID", dataI[muPosPID], "muPosPID[candSize]/I" );
	t->Branch( "muNegPID", dataI[muNegPID], "muNegPID[candSize]/I" );
	t->Branch( "tkPosPID", dataI[tkPosPID], "tkPosPID[candSize]/I" );
	t->Branch( "tkNegPID", dataI[tkNegPID], "tkNegPID[candSize]/I" );

    return;
}

void formatTreeArray_MC_fourDaughter::LoadFormatSourceBranch(TTree* t)
{
    if ( t == nullptr )
    {
        printf("formatTreeArray MC_fourDaughter:        ERROR   no input tree, program cannot load tree from data!\n");
        exit(1);
    }
    SetReadMode();
    t->SetBranchAddress( "candSize", &candSize );

    t->SetBranchAddress( "candMass", readD[candMass] );
	t->SetBranchAddress( "candPt", readD[candPt] );
	t->SetBranchAddress( "candEta", readD[candEta] );
	t->SetBranchAddress( "candY", readD[candY] );
	t->SetBranchAddress( "candPhi", readD[candPhi] );
	t->SetBranchAddress( "candFlightDistance2d", readD[candFlightDistance2d] );
	t->SetBranchAddress( "candCosa2d", readD[candCosa2d] );
	t->SetBranchAddress( "tktkMass", readD[tktkMass] );
	t->SetBranchAddress( "tktkPt", readD[tktkPt] );
	t->SetBranchAddress( "tktkEta", readD[tktkEta] );
	t->SetBranchAddress( "tktkY", readD[tktkY] );
	t->SetBranchAddress( "tktkPhi", readD[tktkPhi] );
	t->SetBranchAddress( "tktkFlightDistance2d", readD[tktkFlightDistance2d] );
	t->SetBranchAddress( "tktkCosa2d", readD[tktkCosa2d] );
	t->SetBranchAddress( "muPosPt", readD[muPosPt] );
	t->SetBranchAddress( "muPosEta", readD[muPosEta] );
	t->SetBranchAddress( "muPosY", readD[muPosY] );
	t->SetBranchAddress( "muPosPhi", readD[muPosPhi] );
	t->SetBranchAddress( "muNegPt", readD[muNegPt] );
	t->SetBranchAddress( "muNegEta", readD[muNegEta] );
	t->SetBranchAddress( "muNegY", readD[muNegY] );
	t->SetBranchAddress( "muNegPhi", readD[muNegPhi] );
	t->SetBranchAddress( "tkPosPt", readD[tkPosPt] );
	t->SetBranchAddress( "tkPosEta", readD[tkPosEta] );
	t->SetBranchAddress( "tkPosY", readD[tkPosY] );
	t->SetBranchAddress( "tkPosPhi", readD[tkPosPhi] );
	t->SetBranchAddress( "tkNegPt", readD[tkNegPt] );
	t->SetBranchAddress( "tkNegEta", readD[tkNegEta] );
	t->SetBranchAddress( "tkNegY", readD[tkNegY] );
	t->SetBranchAddress( "tkNegPhi", readD[tkNegPhi] );

    t->SetBranchAddress( "eventEntry", readI[eventEntry] );
	t->SetBranchAddress( "candPID", readI[candPID] );
	t->SetBranchAddress( "muPosPID", readI[muPosPID] );
	t->SetBranchAddress( "muNegPID", readI[muNegPID] );
	t->SetBranchAddress( "tkPosPID", readI[tkPosPID] );
	t->SetBranchAddress( "tkNegPID", readI[tkNegPID] );

    return;
}
