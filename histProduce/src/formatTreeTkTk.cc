#include "histProduce/histProduce/interface/formatTreeTkTk.h"
#include <stdio.h>
#include <stdlib.h>

void formatTree_TkTk::RegFormatTree()
{
    TTree* t = getNewFormatTree();
    if ( t == nullptr )
    {
        printf("formatTree TkTk:        error to get tree to register!\n");
        exit(1);
    }
    t->Branch( "tktkMass", &dataD[tktkMass], "tktkMass/D" );
    t->Branch( "tktkFD2d", &dataD[tktkFlightDistance2d], "tktkFD2d/D" );
    t->Branch( "tktkVtxprob", &dataD[tktkVtxprob], "tktkVtxprob/D" );
    t->Branch( "tktkPt", &dataD[tktkPt], "tktkPt/D" );
    t->Branch( "fake_Lam0Mass", &dataD[fake_Lam0Mass], "fake_Lam0Mass/D" );
    t->Branch( "fake_KstarMass", &dataD[fake_KstarMass], "fake_KstarMass/D" );
    t->Branch( "fake_KshortMass", &dataD[fake_KshortMass], "fake_KshortMass/D" );
    t->Branch( "fake_PhiMass", &dataD[fake_PhiMass], "fake_PhiMass/D" );
    t->Branch( "ptkMom", &dataD[ptkMom], "ptkMom/D" );
    t->Branch( "ptkDEDX.Harmonic", &dataD[ptkDEDX_Harmonic], "ptkDEDX.Harmonic/D" );
    t->Branch( "ptkDEDX.pixelHrm", &dataD[ptkDEDX_pixelHrm], "ptkDEDX.pixelHrm/D" );
    t->Branch( "ptkIPt", &dataD[ptkIPt], "ptkIPt/D" );
    t->Branch( "ptkIPtErr", &dataD[ptkIPtErr], "ptkIPtErr/D" );
    t->Branch( "ntkMom", &dataD[ntkMom], "ntkMom/D" );
    t->Branch( "ntkDEDX.Harmonic", &dataD[ntkDEDX_Harmonic], "ntkDEDX.Harmonic/D" );
    t->Branch( "ntkDEDX.pixelHrm", &dataD[ntkDEDX_pixelHrm], "ntkDEDX.pixelHrm/D" );
    t->Branch( "ntkIPt", &dataD[ntkIPt], "ntkIPt/D" );
    t->Branch( "ntkIPtErr", &dataD[ntkIPtErr], "ntkIPtErr/D" );

    return;
}


void formatTree_TkTk::LoadFormatSourceBranch()
{
    TTree* t = getOldFormatTree();
    if ( t == nullptr )
    {
        printf("formatTree TkTk:        error to old tree to read!\n");
        exit(1);
    }
    t->SetBranchAddress( "tktkMass", &readD[tktkMass] );
    t->SetBranchAddress( "tktkFD2d", &readD[tktkFlightDistance2d] );
    t->SetBranchAddress( "tktkVtxprob", &readD[tktkVtxprob] );
    t->SetBranchAddress( "tktkPt", &readD[tktkPt] );
    t->SetBranchAddress( "fake_Lam0Mass", &readD[fake_Lam0Mass] );
    t->SetBranchAddress( "fake_KstarMass", &readD[fake_KstarMass] );
    t->SetBranchAddress( "fake_KshortMass", &readD[fake_KshortMass] );
    t->SetBranchAddress( "fake_PhiMass", &readD[fake_PhiMass] );
    t->SetBranchAddress( "ptkMom", &readD[ptkMom] );
    t->SetBranchAddress( "ptkDEDX.Harmonic", &readD[ptkDEDX_Harmonic] );
    t->SetBranchAddress( "ptkDEDX.pixelHrm", &readD[ptkDEDX_pixelHrm] );
    t->SetBranchAddress( "ptkIPt", &readD[ptkIPt] );
    t->SetBranchAddress( "ptkIPtErr", &readD[ptkIPtErr] );
    t->SetBranchAddress( "ntkMom", &readD[ntkMom] );
    t->SetBranchAddress( "ntkDEDX.Harmonic", &readD[ntkDEDX_Harmonic] );
    t->SetBranchAddress( "ntkDEDX.pixelHrm", &readD[ntkDEDX_pixelHrm] );
    t->SetBranchAddress( "ntkIPt", &readD[ntkIPt] );
    t->SetBranchAddress( "ntkIPtErr", &readD[ntkIPtErr] );
    return;
}

