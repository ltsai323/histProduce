#ifndef __FORMATTREE_PLUSMINUS_LBTK_H__
#define __FORMATTREE_PLUSMINUS_LBTK_H__
#include "histProduce/histProduce/interface/formatTree.h"

struct formatTree_plusminus_LbTk : public formatTree
{
    public:
        enum readVarD
        {
            plbtkMass, plbtkFlightDistance2d, plbtkFlightDistanceSig, plbtkVtxprob, plbtkCosa2d,
            ptargetJpsiP_mass, ptargetJpsiP_pt,
            ptargetJpsiPBar_mass, ptargetJpsiPBar_pt,
            plbtkPt, plbtkMom,
            ptktkPt, ptktkMom,
            pfake_Lam0Mass, pfake_KstarMass, pfake_PhiMass, pfake_KshortMass,
            pfake_LbL0Mass, pfake_BdMass   , pfake_BsMass, pfake_mumupipiMass,
            pptonPt, pptonMom, pptonDEDX_Harmonic, pptonDEDX_pixelHrm, pptonIPt, pptonIPtErr,
            pkaonPt, pkaonMom, pkaonDEDX_Harmonic, pkaonDEDX_pixelHrm, pkaonIPt, pkaonIPtErr,

            nlbtkMass, nlbtkFlightDistance2d, nlbtkFlightDistanceSig, nlbtkVtxprob, nlbtkCosa2d,
            ntargetJpsiP_mass, ntargetJpsiP_pt,
            ntargetJpsiPBar_mass, ntargetJpsiPBar_pt,
            nlbtkPt, nlbtkMom,
            ntktkPt, ntktkMom,
            nfake_Lam0Mass, nfake_KstarMass, nfake_PhiMass, nfake_KshortMass,
            nfake_LbL0Mass, nfake_BdMass   , nfake_BsMass, nfake_mumupipiMass,
            nptonPt, nptonMom, nptonDEDX_Harmonic, nptonDEDX_pixelHrm, nptonIPt, nptonIPtErr,
            nkaonPt, nkaonMom, nkaonDEDX_Harmonic, nkaonDEDX_pixelHrm, nkaonIPt, nkaonIPtErr,
            comNumD
        };
        enum readVarI
        {
            comNumI
        };
        virtual void RegFormatTree() override;
        virtual void LoadFormatSourceBranch() override;

        formatTree_plusminus_LbTk( int totNumD, int totNumI ) : formatTree ( totNumD, totNumI ) {}
        ~formatTree_plusminus_LbTk() {}
};

#endif
