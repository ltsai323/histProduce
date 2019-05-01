# ifndef __UPDATED_formatTreeArray_LbTk__
# define __UPDATED_formatTreeArray_LbTk__
# include "histProduce/histProduce/interface/formatTreeArray.h"
struct formatTreeArray_LbTk : public formatTreeArray
{
public:
	enum readVarD
	{
        lbtkMass, lbtkPt, lbtkEta, lbtkY, lbtkPhi, lbtkFlightDistance2d, lbtkFlightDistanceSig, lbtkVtxprob, lbtkCosa2d, lbtknChi2,
		tktkMass, tktkPt, tktkEta, tktkY, tktkPhi, tktkFlightDistance2d, tktkFlightDistanceSig, tktkVtxprob, tktkCosa2d, tktknChi2,
		pmuPt, pmuP0, pmuP1, pmuP2, pmuP3,
		nmuPt, nmuP0, nmuP1, nmuP2, nmuP3,
		tk1Pt, tk1P0, tk1P1, tk1P2, tk1P3, tk1DEDX_Harmonic, tk1DEDX_pixelHrm, tk1IPt, tk1IPtErr,
		tk2Pt, tk2P0, tk2P1, tk2P2, tk2P3, tk2DEDX_Harmonic, tk2DEDX_pixelHrm, tk2IPt, tk2IPtErr,
		comNumD
	};
	enum readVarI
    {
        eventEntry, trigVanish, trigNotRun, trigReject, trigError, totallyTriggered,
		comNumI
	};
	virtual void RegFormatTree(TTree* t) override;
	virtual void LoadFormatSourceBranch(TTree* t) override;

	formatTreeArray_LbTk( int totNumD=comNumD, int totNumI=comNumI ) : formatTreeArray ( totNumD, totNumI ) {
	}
	~formatTreeArray_LbTk() {
	}
};

# endif
