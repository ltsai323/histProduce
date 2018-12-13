#include "histProduce/histProduce/interface/formatTreepnLbTk.h"
#include <stdio.h>
#include <stdlib.h>

void formatTree_plusminus_LbTk::RegFormatTree(TTree* t)
{
    if ( t == nullptr )
    {
        printf("formatTree pnLbTk:        error to get tree to register!\n");
        exit(1);
    }

    t->Branch( "plbtkMass", &dataD[plbtkMass], "plbtkMass/D" );
    t->Branch( "plbtkFD2d", &dataD[plbtkFlightDistance2d], "plbtkFD2d/D" );
    t->Branch( "plbtkFDSig", &dataD[plbtkFlightDistanceSig], "plbtkFDSig/D" );
    t->Branch( "plbtkVtxprob", &dataD[plbtkVtxprob], "plbtkVtxprob/D" );
    t->Branch( "plbtkCosa2d", &dataD[plbtkCosa2d], "plbtkCosa2d/D" );

    t->Branch( "ptargetJpsiP_mass", &dataD[ptargetJpsiP_mass], "ptargetJpsiP_mass/D" );
    t->Branch( "ptargetJpsiP_pt", &dataD[ptargetJpsiP_pt], "ptargetJpsiP_pt/D" );
    t->Branch( "ptargetJpsiPBar_mass", &dataD[ptargetJpsiPBar_mass], "ptargetJpsiPBar_mass/D" );
    t->Branch( "ptargetJpsiPBar_pt", &dataD[ptargetJpsiPBar_pt], "ptargetJpsiPBar_pt/D" );

    t->Branch( "plbtkMom", &dataD[plbtkMom], "plbtkMom/D" );
    t->Branch( "plbtkPt", &dataD[plbtkPt], "plbtkPt/D" );
    t->Branch( "ptktkPt", &dataD[ptktkPt], "ptktkPt/D" );
    t->Branch( "ptktkMom", &dataD[ptktkMom], "ptktkMom/D" );

    t->Branch( "pfake_Lam0Mass", &dataD[pfake_Lam0Mass], "pfake_Lam0Mass/D" );
    t->Branch( "pfake_LbL0Mass", &dataD[pfake_LbL0Mass], "pfake_LbL0Mass/D" );
    t->Branch( "pfake_KstarMass", &dataD[pfake_KstarMass], "pfake_KstarMass/D" );
    t->Branch( "pfake_BdMass", &dataD[pfake_BdMass], "pfake_BdMass/D" );
    t->Branch( "pfake_PhiMass", &dataD[pfake_PhiMass], "pfake_PhiMass/D" );
    t->Branch( "pfake_BsMass", &dataD[pfake_BsMass], "pfake_BsMass/D" );
    t->Branch( "pfake_KshortMass", &dataD[pfake_KshortMass], "pfake_KshortMass/D" );
    t->Branch( "pfake_mumupipiMass", &dataD[pfake_mumupipiMass], "pfake_mumupipiMass/D" );

    t->Branch( "pptonPt", &dataD[pptonPt], "pptonPt/D" );
    t->Branch( "pptonMom", &dataD[pptonMom], "pptonMom/D" );
    t->Branch( "pptonDEDX.Harmonic", &dataD[pptonDEDX_Harmonic], "pptonDEDX.Harmonic/D" );
    t->Branch( "pptonDEDX.pixelHrm", &dataD[pptonDEDX_pixelHrm], "pptonDEDX.pixelHrm/D" );
    t->Branch( "pptonIPt", &dataD[pptonIPt], "pptonIPt/D" );
    t->Branch( "pptonIPtErr", &dataD[pptonIPtErr], "pptonIPtErr/D" );

    t->Branch( "pkaonPt", &dataD[pkaonPt], "pkaonPt/D" );
    t->Branch( "pkaonMom", &dataD[pkaonMom], "pkaonMom/D" );
    t->Branch( "pkaonDEDX.Harmonic", &dataD[pkaonDEDX_Harmonic], "pkaonDEDX.Harmonic/D" );
    t->Branch( "pkaonDEDX.pixelHrm", &dataD[pkaonDEDX_pixelHrm], "pkaonDEDX.pixelHrm/D" );
    t->Branch( "pkaonIPt", &dataD[pkaonIPt], "pkaonIPt/D" );
    t->Branch( "pkaonIPtErr", &dataD[pkaonIPtErr], "pkaonIPtErr/D" );




    t->Branch( "nlbtkMass", &dataD[nlbtkMass], "nlbtkMass/D" );
    t->Branch( "nlbtkFD2d", &dataD[nlbtkFlightDistance2d], "nlbtkFD2d/D" );
    t->Branch( "nlbtkFDSig", &dataD[nlbtkFlightDistanceSig], "nlbtkFDSig/D" );
    t->Branch( "nlbtkVtxprob", &dataD[nlbtkVtxprob], "nlbtkVtxprob/D" );
    t->Branch( "nlbtkCosa2d", &dataD[nlbtkCosa2d], "nlbtkCosa2d/D" );

    t->Branch( "ntargetJpsiP_mass", &dataD[ntargetJpsiP_mass], "ntargetJpsiP_mass/D" );
    t->Branch( "ntargetJpsiP_pt", &dataD[ntargetJpsiP_pt], "ntargetJpsiP_pt/D" );
    t->Branch( "ntargetJpsiPBar_mass", &dataD[ntargetJpsiPBar_mass], "ntargetJpsiPBar_mass/D" );
    t->Branch( "ntargetJpsiPBar_pt", &dataD[ntargetJpsiPBar_pt], "ntargetJpsiPBar_pt/D" );

    t->Branch( "nlbtkMom", &dataD[nlbtkMom], "nlbtkMom/D" );
    t->Branch( "nlbtkPt", &dataD[nlbtkPt], "nlbtkPt/D" );
    t->Branch( "ntktkPt", &dataD[ntktkPt], "ntktkPt/D" );
    t->Branch( "ntktkMom", &dataD[ntktkMom], "ntktkMom/D" );

    t->Branch( "nfake_Lam0Mass", &dataD[nfake_Lam0Mass], "nfake_Lam0Mass/D" );
    t->Branch( "nfake_LbL0Mass", &dataD[nfake_LbL0Mass], "nfake_LbL0Mass/D" );
    t->Branch( "nfake_KstarMass", &dataD[nfake_KstarMass], "nfake_KstarMass/D" );
    t->Branch( "nfake_BdMass", &dataD[nfake_BdMass], "nfake_BdMass/D" );
    t->Branch( "nfake_PhiMass", &dataD[nfake_PhiMass], "nfake_PhiMass/D" );
    t->Branch( "nfake_BsMass", &dataD[nfake_BsMass], "nfake_BsMass/D" );
    t->Branch( "nfake_KshortMass", &dataD[nfake_KshortMass], "nfake_KshortMass/D" );
    t->Branch( "nfake_mumupipiMass", &dataD[nfake_mumupipiMass], "nfake_mumupipiMass/D" );

    t->Branch( "nptonPt", &dataD[nptonPt], "nptonPt/D" );
    t->Branch( "nptonMom", &dataD[nptonMom], "nptonMom/D" );
    t->Branch( "nptonDEDX.Harmonic", &dataD[nptonDEDX_Harmonic], "nptonDEDX.Harmonic/D" );
    t->Branch( "nptonDEDX.pixelHrm", &dataD[nptonDEDX_pixelHrm], "nptonDEDX.pixelHrm/D" );
    t->Branch( "nptonIPt", &dataD[nptonIPt], "nptonIPt/D" );
    t->Branch( "nptonIPtErr", &dataD[nptonIPtErr], "nptonIPtErr/D" );

    t->Branch( "nkaonPt", &dataD[nkaonPt], "nkaonPt/D" );
    t->Branch( "nkaonMom", &dataD[nkaonMom], "nkaonMom/D" );
    t->Branch( "nkaonDEDX.Harmonic", &dataD[nkaonDEDX_Harmonic], "nkaonDEDX.Harmonic/D" );
    t->Branch( "nkaonDEDX.pixelHrm", &dataD[nkaonDEDX_pixelHrm], "nkaonDEDX.pixelHrm/D" );
    t->Branch( "nkaonIPt", &dataD[nkaonIPt], "nkaonIPt/D" );
    t->Branch( "nkaonIPtErr", &dataD[nkaonIPtErr], "nkaonIPtErr/D" );
    return;
}


void formatTree_plusminus_LbTk::LoadFormatSourceBranch(TTree* t)
{
    if ( t == nullptr )
    {
        printf("formatTree pnLbTk:        error to old tree to read!\n");
        exit(1);
    }
    SetReadMode();

    t->SetBranchAddress( "plbtkMass", &readD[plbtkMass] );
    t->SetBranchAddress( "plbtkFD2d", &readD[plbtkFlightDistance2d] );
    t->SetBranchAddress( "plbtkFDSig", &readD[plbtkFlightDistanceSig] );
    t->SetBranchAddress( "plbtkVtxprob", &readD[plbtkVtxprob] );
    t->SetBranchAddress( "plbtkCosa2d", &readD[plbtkCosa2d] );

    t->SetBranchAddress( "ptargetJpsiP_mass", &readD[ptargetJpsiP_mass] );
    t->SetBranchAddress( "ptargetJpsiP_pt", &readD[ptargetJpsiP_pt] );
    t->SetBranchAddress( "ptargetJpsiPBar_mass", &readD[ptargetJpsiPBar_mass] );
    t->SetBranchAddress( "ptargetJpsiPBar_pt", &readD[ptargetJpsiPBar_pt] );

    t->SetBranchAddress( "plbtkMom", &readD[plbtkMom] );
    t->SetBranchAddress( "plbtkPt", &readD[plbtkPt] );
    t->SetBranchAddress( "ptktkPt", &readD[ptktkPt] );
    t->SetBranchAddress( "ptktkMom", &readD[ptktkMom] );

    t->SetBranchAddress( "pfake_Lam0Mass", &readD[pfake_Lam0Mass] );
    t->SetBranchAddress( "pfake_LbL0Mass", &readD[pfake_LbL0Mass] );
    t->SetBranchAddress( "pfake_KstarMass", &readD[pfake_KstarMass] );
    t->SetBranchAddress( "pfake_BdMass", &readD[pfake_BdMass] );
    t->SetBranchAddress( "pfake_PhiMass", &readD[pfake_PhiMass] );
    t->SetBranchAddress( "pfake_BsMass", &readD[pfake_BsMass] );
    t->SetBranchAddress( "pfake_KshortMass", &readD[pfake_KshortMass] );
    t->SetBranchAddress( "pfake_mumupipiMass", &readD[pfake_mumupipiMass] );

    t->SetBranchAddress( "pptonPt", &readD[pptonPt] );
    t->SetBranchAddress( "pptonMom", &readD[pptonMom] );
    t->SetBranchAddress( "pptonDEDX.Harmonic", &readD[pptonDEDX_Harmonic] );
    t->SetBranchAddress( "pptonDEDX.pixelHrm", &readD[pptonDEDX_pixelHrm] );
    t->SetBranchAddress( "pptonIPt", &readD[pptonIPt] );
    t->SetBranchAddress( "pptonIPtErr", &readD[pptonIPtErr] );

    t->SetBranchAddress( "pkaonPt", &readD[pkaonPt] );
    t->SetBranchAddress( "pkaonMom", &readD[pkaonMom] );
    t->SetBranchAddress( "pkaonDEDX.Harmonic", &readD[pkaonDEDX_Harmonic] );
    t->SetBranchAddress( "pkaonDEDX.pixelHrm", &readD[pkaonDEDX_pixelHrm] );
    t->SetBranchAddress( "pkaonIPt", &readD[pkaonIPt] );
    t->SetBranchAddress( "pkaonIPtErr", &readD[pkaonIPtErr] );




    t->SetBranchAddress( "nlbtkMass", &readD[nlbtkMass] );
    t->SetBranchAddress( "nlbtkFD2d", &readD[nlbtkFlightDistance2d] );
    t->SetBranchAddress( "nlbtkFDSig", &readD[nlbtkFlightDistanceSig] );
    t->SetBranchAddress( "nlbtkVtxprob", &readD[nlbtkVtxprob] );
    t->SetBranchAddress( "nlbtkCosa2d", &readD[nlbtkCosa2d] );

    t->SetBranchAddress( "ntargetJpsiP_mass", &readD[ntargetJpsiP_mass] );
    t->SetBranchAddress( "ntargetJpsiP_pt", &readD[ntargetJpsiP_pt] );
    t->SetBranchAddress( "ntargetJpsiPBar_mass", &readD[ntargetJpsiPBar_mass] );
    t->SetBranchAddress( "ntargetJpsiPBar_pt", &readD[ntargetJpsiPBar_pt] );

    t->SetBranchAddress( "nlbtkMom", &readD[nlbtkMom] );
    t->SetBranchAddress( "nlbtkPt", &readD[nlbtkPt] );
    t->SetBranchAddress( "ntktkPt", &readD[ntktkPt] );
    t->SetBranchAddress( "ntktkMom", &readD[ntktkMom] );

    t->SetBranchAddress( "nfake_Lam0Mass", &readD[nfake_Lam0Mass] );
    t->SetBranchAddress( "nfake_LbL0Mass", &readD[nfake_LbL0Mass] );
    t->SetBranchAddress( "nfake_KstarMass", &readD[nfake_KstarMass] );
    t->SetBranchAddress( "nfake_BdMass", &readD[nfake_BdMass] );
    t->SetBranchAddress( "nfake_PhiMass", &readD[nfake_PhiMass] );
    t->SetBranchAddress( "nfake_BsMass", &readD[nfake_BsMass] );
    t->SetBranchAddress( "nfake_KshortMass", &readD[nfake_KshortMass] );
    t->SetBranchAddress( "nfake_mumupipiMass", &readD[nfake_mumupipiMass] );

    t->SetBranchAddress( "nptonPt", &readD[nptonPt] );
    t->SetBranchAddress( "nptonMom", &readD[nptonMom] );
    t->SetBranchAddress( "nptonDEDX.Harmonic", &readD[nptonDEDX_Harmonic] );
    t->SetBranchAddress( "nptonDEDX.pixelHrm", &readD[nptonDEDX_pixelHrm] );
    t->SetBranchAddress( "nptonIPt", &readD[nptonIPt] );
    t->SetBranchAddress( "nptonIPtErr", &readD[nptonIPtErr] );

    t->SetBranchAddress( "nkaonPt", &readD[nkaonPt] );
    t->SetBranchAddress( "nkaonMom", &readD[nkaonMom] );
    t->SetBranchAddress( "nkaonDEDX.Harmonic", &readD[nkaonDEDX_Harmonic] );
    t->SetBranchAddress( "nkaonDEDX.pixelHrm", &readD[nkaonDEDX_pixelHrm] );
    t->SetBranchAddress( "nkaonIPt", &readD[nkaonIPt] );
    t->SetBranchAddress( "nkaonIPtErr", &readD[nkaonIPtErr] );
    return;
}

