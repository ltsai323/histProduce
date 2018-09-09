#include "histProduce/histProduce/interface/formatTreeLbTk.h"
#include <stdio.h>
#include <stdlib.h>

void formatTree_LbTk::RegFormatTree()
{
    TTree* t = getNewFormatTree();
    if ( t == nullptr )
    {
        printf("formatTree LbTk:        error to get tree to register!\n");
        exit(1);
    }
    t->Branch( "lbtkMass", &dataD[lbtkMass], "lbtkMass/D" );
    t->Branch( "lbtkFD2d", &dataD[lbtkFlightDistance2d], "lbtkFD2d/D" );
    t->Branch( "lbtkFDSig", &dataD[lbtkFlightDistanceSig], "lbtkFDSig/D" );
    t->Branch( "lbtkVtxprob", &dataD[lbtkVtxprob], "lbtkVtxprob/D" );
    t->Branch( "lbtkCosa2d", &dataD[lbtkCosa2d], "lbtkCosa2d/D" );

    t->Branch( "targetJpsiP_mass", &dataD[targetJpsiP_mass], "targetJpsiP_mass/D" );
    t->Branch( "targetJpsiP_pt", &dataD[targetJpsiP_pt], "targetJpsiP_pt/D" );
    t->Branch( "targetJpsiPBar_mass", &dataD[targetJpsiPBar_mass], "targetJpsiPBar_mass/D" );
    t->Branch( "targetJpsiPBar_pt", &dataD[targetJpsiPBar_pt], "targetJpsiPBar_pt/D" );

    t->Branch( "lbtkMom", &dataD[lbtkMom], "lbtkMom/D" );
    t->Branch( "lbtkPt", &dataD[lbtkPt], "lbtkPt/D" );
    t->Branch( "tktkPt", &dataD[tktkPt], "tktkPt/D" );
    t->Branch( "tktkMom", &dataD[tktkMom], "tktkMom/D" );

    t->Branch( "fake_Lam0Mass", &dataD[fake_Lam0Mass], "fake_Lam0Mass/D" );
    t->Branch( "fake_LbL0Mass", &dataD[fake_LbL0Mass], "fake_LbL0Mass/D" );
    t->Branch( "fake_KstarMass", &dataD[fake_KstarMass], "fake_KstarMass/D" );
    t->Branch( "fake_BdMass", &dataD[fake_BdMass], "fake_BdMass/D" );
    t->Branch( "fake_PhiMass", &dataD[fake_PhiMass], "fake_PhiMass/D" );
    t->Branch( "fake_BsMass", &dataD[fake_BsMass], "fake_BsMass/D" );
    t->Branch( "fake_KshortMass", &dataD[fake_KshortMass], "fake_KshortMass/D" );
    t->Branch( "fake_mumupipiMass", &dataD[fake_mumupipiMass], "fake_mumupipiMass/D" );

    t->Branch( "ptkPt", &dataD[ptkPt], "ptkPt/D" );
    t->Branch( "ptkMom", &dataD[ptkMom], "ptkMom/D" );
    t->Branch( "ptkDEDX.Harmonic", &dataD[ptkDEDX_Harmonic], "ptkDEDX.Harmonic/D" );
    t->Branch( "ptkDEDX.pixelHrm", &dataD[ptkDEDX_pixelHrm], "ptkDEDX.pixelHrm/D" );
    t->Branch( "ptkIPt", &dataD[ptkIPt], "ptkIPt/D" );
    t->Branch( "ptkIPtErr", &dataD[ptkIPtErr], "ptkIPtErr/D" );

    t->Branch( "ntkPt", &dataD[ntkPt], "ntkPt/D" );
    t->Branch( "ntkMom", &dataD[ntkMom], "ntkMom/D" );
    t->Branch( "ntkDEDX.Harmonic", &dataD[ntkDEDX_Harmonic], "ntkDEDX.Harmonic/D" );
    t->Branch( "ntkDEDX.pixelHrm", &dataD[ntkDEDX_pixelHrm], "ntkDEDX.pixelHrm/D" );
    t->Branch( "ntkIPt", &dataD[ntkIPt], "ntkIPt/D" );
    t->Branch( "ntkIPtErr", &dataD[ntkIPtErr], "ntkIPtErr/D" );

    return;
}


void formatTree_LbTk::LoadFormatSourceBranch()
{
    TTree* t = getOldFormatTree();
    if ( t == nullptr )
    {
        printf("formatTree LbTk:        error to old tree to read!\n");
        exit(1);
    }
    t->SetBranchAddress( "lbtkMass", &readD[lbtkMass] );
    t->SetBranchAddress( "lbtkFD2d", &readD[lbtkFlightDistance2d] );
    t->SetBranchAddress( "lbtkFDSig", &readD[lbtkFlightDistanceSig] );
    t->SetBranchAddress( "lbtkVtxprob", &readD[lbtkVtxprob] );
    t->SetBranchAddress( "lbtkCosa2d", &readD[lbtkCosa2d] );

    t->SetBranchAddress( "targetJpsiP_mass", &readD[targetJpsiP_mass] );
    t->SetBranchAddress( "targetJpsiP_pt", &readD[targetJpsiP_pt] );
    t->SetBranchAddress( "targetJpsiPBar_mass", &readD[targetJpsiPBar_mass] );
    t->SetBranchAddress( "targetJpsiPBar_pt", &readD[targetJpsiPBar_pt] );

    t->SetBranchAddress( "lbtkMom", &readD[lbtkMom] );
    t->SetBranchAddress( "lbtkPt", &readD[lbtkPt] );
    t->SetBranchAddress( "tktkPt", &readD[tktkPt] );
    t->SetBranchAddress( "tktkMom", &readD[tktkMom] );

    t->SetBranchAddress( "fake_Lam0Mass", &readD[fake_Lam0Mass] );
    t->SetBranchAddress( "fake_LbL0Mass", &readD[fake_LbL0Mass] );
    t->SetBranchAddress( "fake_KstarMass", &readD[fake_KstarMass] );
    t->SetBranchAddress( "fake_BdMass", &readD[fake_BdMass] );
    t->SetBranchAddress( "fake_PhiMass", &readD[fake_PhiMass] );
    t->SetBranchAddress( "fake_BsMass", &readD[fake_BsMass] );
    t->SetBranchAddress( "fake_KshortMass", &readD[fake_KshortMass] );
    t->SetBranchAddress( "fake_mumupipiMass", &readD[fake_mumupipiMass] );

    t->SetBranchAddress( "ptkPt", &readD[ptkPt] );
    t->SetBranchAddress( "ptkMom", &readD[ptkMom] );
    t->SetBranchAddress( "ptkDEDX.Harmonic", &readD[ptkDEDX_Harmonic] );
    t->SetBranchAddress( "ptkDEDX.pixelHrm", &readD[ptkDEDX_pixelHrm] );
    t->SetBranchAddress( "ptkIPt", &readD[ptkIPt] );
    t->SetBranchAddress( "ptkIPtErr", &readD[ptkIPtErr] );

    t->SetBranchAddress( "ntkPt", &readD[ntkPt] );
    t->SetBranchAddress( "ntkMom", &readD[ntkMom] );
    t->SetBranchAddress( "ntkDEDX.Harmonic", &readD[ntkDEDX_Harmonic] );
    t->SetBranchAddress( "ntkDEDX.pixelHrm", &readD[ntkDEDX_pixelHrm] );
    t->SetBranchAddress( "ntkIPt", &readD[ntkIPt] );
    t->SetBranchAddress( "ntkIPtErr", &readD[ntkIPtErr] );
    return;
}

