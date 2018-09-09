#ifndef __FORMATTREE_TKTK_H__
#define __FORMATTREE_TKTK_H__
#include "histProduce/histProduce/interface/formatTree.h"

struct formatTree_TkTk : public formatTree
{
    public:
        enum readVarD
        {
            tktkMass, tktkFlightDistance2d, tktkVtxprob, tktkPt,
            fake_Lam0Mass, fake_KstarMass, fake_KshortMass, fake_PhiMass,
            ptkMom, ptkDEDX_Harmonic, ptkDEDX_pixelHrm, ptkIPt, ptkIPtErr,
            ntkMom, ntkDEDX_Harmonic, ntkDEDX_pixelHrm, ntkIPt, ntkIPtErr,
            comNumD
        };
        enum readVarI
        {
            comNumI
        };
        virtual void RegFormatTree() override;
        virtual void LoadFormatSourceBranch() override;

        formatTree_TkTk( int totNumD, int totNumI ) : formatTree ( totNumD, totNumI ) {}
        ~formatTree_TkTk() {}
};

#endif
