
# ifndef __UPDATED_formatTreeArray_MC_fourDaughter__
# define __UPDATED_formatTreeArray_MC_fourDaughter__
# include "histProduce/histProduce/interface/formatTreeArray.h"
struct formatTreeArray_MC_fourDaughter : public formatTreeArray
{
public:
	enum readVarD
	{
        candMass, candPt, candEta, candY, candPhi, candFlightDistance2d, candCosa2d,
		tktkMass, tktkPt, tktkEta, tktkY, tktkPhi, tktkFlightDistance2d, tktkCosa2d,
		muPosPt, muPosEta, muPosY, muPosPhi,
		muNegPt, muNegEta, muNegY, muNegPhi,
		tkPosPt, tkPosEta, tkPosY, tkPosPhi,
		tkNegPt, tkNegEta, tkNegY, tkNegPhi,
		comNumD
	};
	enum readVarI
    {
        eventEntry, candPID, muPosPID, muNegPID, tkPosPID, tkNegPID,
		comNumI
	};
	virtual void RegFormatTree(TTree* t) override;
	virtual void LoadFormatSourceBranch(TTree* t) override;

	formatTreeArray_MC_fourDaughter( int totNumD=comNumD, int totNumI=comNumI ) : formatTreeArray ( totNumD, totNumI ) {
	}
	~formatTreeArray_MC_fourDaughter() {
	}
};

# endif
