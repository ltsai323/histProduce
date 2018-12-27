
# include "histProduce/histProduce/interface/formatTreeMC_fourDaughter.h"
# include <stdio.h>
# include <stdlib.h>

void formatTree_MC_fourDaughter::RegFormatTree(TTree* t)
{
    if ( t == nullptr )
    {
        printf("formatTree MC_fourDaughter:        error to get tree to register!\n");
        exit(1);
    }
    t->Branch( "candMass", &dataD[candMass], "candMass/D" );
	t->Branch( "candPt", &dataD[candPt], "candPt/D" );
	t->Branch( "candEta", &dataD[candEta], "candEta/D" );
	t->Branch( "candY", &dataD[candY], "candY/D" );
	t->Branch( "candPhi", &dataD[candPhi], "candPhi/D" );
	t->Branch( "candFlightDistance2d", &dataD[candFlightDistance2d], "candFlightDistance2d/D" );
	t->Branch( "candCosa2d", &dataD[candCosa2d], "candCosa2d/D" );
	t->Branch( "tktkMass", &dataD[tktkMass], "tktkMass/D" );
	t->Branch( "tktkPt", &dataD[tktkPt], "tktkPt/D" );
	t->Branch( "tktkEta", &dataD[tktkEta], "tktkEta/D" );
	t->Branch( "tktkY", &dataD[tktkY], "tktkY/D" );
	t->Branch( "tktkPhi", &dataD[tktkPhi], "tktkPhi/D" );
	t->Branch( "tktkFlightDistance2d", &dataD[tktkFlightDistance2d], "tktkFlightDistance2d/D" );
	t->Branch( "tktkCosa2d", &dataD[tktkCosa2d], "tktkCosa2d/D" );
	t->Branch( "muPosPt", &dataD[muPosPt], "muPosPt/D" );
	t->Branch( "muPosEta", &dataD[muPosEta], "muPosEta/D" );
	t->Branch( "muPosY", &dataD[muPosY], "muPosY/D" );
	t->Branch( "muPosPhi", &dataD[muPosPhi], "muPosPhi/D" );
	t->Branch( "muNegPt", &dataD[muNegPt], "muNegPt/D" );
	t->Branch( "muNegEta", &dataD[muNegEta], "muNegEta/D" );
	t->Branch( "muNegY", &dataD[muNegY], "muNegY/D" );
	t->Branch( "muNegPhi", &dataD[muNegPhi], "muNegPhi/D" );
	t->Branch( "tkPosPt", &dataD[tkPosPt], "tkPosPt/D" );
	t->Branch( "tkPosEta", &dataD[tkPosEta], "tkPosEta/D" );
	t->Branch( "tkPosY", &dataD[tkPosY], "tkPosY/D" );
	t->Branch( "tkPosPhi", &dataD[tkPosPhi], "tkPosPhi/D" );
	t->Branch( "tkNegPt", &dataD[tkNegPt], "tkNegPt/D" );
	t->Branch( "tkNegEta", &dataD[tkNegEta], "tkNegEta/D" );
	t->Branch( "tkNegY", &dataD[tkNegY], "tkNegY/D" );
	t->Branch( "tkNegPhi", &dataD[tkNegPhi], "tkNegPhi/D" );

    t->Branch( "eventEntry", &dataI[eventEntry], "eventEntry/I" );
	t->Branch( "candPID", &dataI[candPID], "candPID/I" );
	t->Branch( "muPosPID", &dataI[muPosPID], "muPosPID/I" );
	t->Branch( "muNegPID", &dataI[muNegPID], "muNegPID/I" );
	t->Branch( "tkPosPID", &dataI[tkPosPID], "tkPosPID/I" );
	t->Branch( "tkNegPID", &dataI[tkNegPID], "tkNegPID/I" );

    return;
}

void formatTree_MC_fourDaughter::LoadFormatSourceBranch(TTree* t)
{
    if ( t == nullptr )
    {
        printf("formatTree MC_fourDaughter:        error to old tree to read!\n");
        exit(1);
    }
    SetReadMode();
    t->SetBranchAddress( "candMass", &readD[candMass] );
	t->SetBranchAddress( "candPt", &readD[candPt] );
	t->SetBranchAddress( "candEta", &readD[candEta] );
	t->SetBranchAddress( "candY", &readD[candY] );
	t->SetBranchAddress( "candPhi", &readD[candPhi] );
	t->SetBranchAddress( "candFlightDistance2d", &readD[candFlightDistance2d] );
	t->SetBranchAddress( "candCosa2d", &readD[candCosa2d] );
	t->SetBranchAddress( "tktkMass", &readD[tktkMass] );
	t->SetBranchAddress( "tktkPt", &readD[tktkPt] );
	t->SetBranchAddress( "tktkEta", &readD[tktkEta] );
	t->SetBranchAddress( "tktkY", &readD[tktkY] );
	t->SetBranchAddress( "tktkPhi", &readD[tktkPhi] );
	t->SetBranchAddress( "tktkFlightDistance2d", &readD[tktkFlightDistance2d] );
	t->SetBranchAddress( "tktkCosa2d", &readD[tktkCosa2d] );
	t->SetBranchAddress( "muPosPt", &readD[muPosPt] );
	t->SetBranchAddress( "muPosEta", &readD[muPosEta] );
	t->SetBranchAddress( "muPosY", &readD[muPosY] );
	t->SetBranchAddress( "muPosPhi", &readD[muPosPhi] );
	t->SetBranchAddress( "muNegPt", &readD[muNegPt] );
	t->SetBranchAddress( "muNegEta", &readD[muNegEta] );
	t->SetBranchAddress( "muNegY", &readD[muNegY] );
	t->SetBranchAddress( "muNegPhi", &readD[muNegPhi] );
	t->SetBranchAddress( "tkPosPt", &readD[tkPosPt] );
	t->SetBranchAddress( "tkPosEta", &readD[tkPosEta] );
	t->SetBranchAddress( "tkPosY", &readD[tkPosY] );
	t->SetBranchAddress( "tkPosPhi", &readD[tkPosPhi] );
	t->SetBranchAddress( "tkNegPt", &readD[tkNegPt] );
	t->SetBranchAddress( "tkNegEta", &readD[tkNegEta] );
	t->SetBranchAddress( "tkNegY", &readD[tkNegY] );
	t->SetBranchAddress( "tkNegPhi", &readD[tkNegPhi] );

    t->SetBranchAddress( "eventEntry", &readI[eventEntry] );
	t->SetBranchAddress( "candPID", &readI[candPID] );
	t->SetBranchAddress( "muPosPID", &readI[muPosPID] );
	t->SetBranchAddress( "muNegPID", &readI[muNegPID] );
	t->SetBranchAddress( "tkPosPID", &readI[tkPosPID] );
	t->SetBranchAddress( "tkNegPID", &readI[tkNegPID] );

    return;
}
