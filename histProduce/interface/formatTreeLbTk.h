#ifndef __FORMATTREE_LBTK_H__
#define __FORMATTREE_LBTK_H__
#include "histProduce/histProduce/interface/formatTree.h"

struct formatTree_LbTk : public formatTree
{
    public:
        enum readVarD
        {
            lbtkMass, lbtkFlightDistance2d, lbtkFlightDistanceSig, lbtkVtxprob, lbtkCosa2d,
            targetJpsiP_mass, targetJpsiP_pt,
            targetJpsiPBar_mass, targetJpsiPBar_pt,
            lbtkPt, lbtkMom,
            tktkPt, tktkMom,
            fake_Lam0Mass, fake_KstarMass, fake_PhiMass, fake_KshortMass,
            fake_LbL0Mass, fake_BdMass   , fake_BsMass, fake_mumupipiMass,
            ptkPt, ptkMom, ptkDEDX_Harmonic, ptkDEDX_pixelHrm, ptkIPt, ptkIPtErr,
            ntkPt, ntkMom, ntkDEDX_Harmonic, ntkDEDX_pixelHrm, ntkIPt, ntkIPtErr,
            comNumD
        };
        enum readVarI
        {
            comNumI
        };
        virtual void RegFormatTree() override;
        virtual void LoadFormatSourceBranch() override;

        formatTree_LbTk( int totNumD, int totNumI ) : formatTree ( totNumD, totNumI ) {}
        ~formatTree_LbTk() {}
};

#endif
