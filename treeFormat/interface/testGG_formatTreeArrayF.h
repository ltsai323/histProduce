#ifndef __UPDATED_testGG_formatTreeArrayF__
#define __UPDATED_testGG_formatTreeArrayF__
#include "histProduce/treeFormat/interface/formatTreeArray.hpp"
struct testGG_formatTreeArrayF : public formatTreeArray<float>
{
public:
	enum readVar
    {
        phoEta,
		phoPhi,
		phoCalibEt,
		phoR9,
		phoSCEta,
		phoHoverE,
		phoPFChIso,
		phoPFNeuIso,
		phoPFPhoIso,
		phoSCRawE,
		phoSCEtaWidth,
		phoSCPhiWidth,
		phoESEnP1,
		phoESEnP2,
		phoESEffSigmaRR,
		phoPFChWorstIso,
		phoSigmaIEtaIEtaFull5x5,
		phoSigmaIEtaIPhiFull5x5,
		phoSigmaIPhiIPhiFull5x5,
		phoR9Full5x5,
		phoE2x2Full5x5,
		phoE5x5Full5x5,
		phoIDMVA,
		phoSeedEnergy,
		comNum
	};
	virtual void RegFormatTree(TTree* t) override;
	virtual void LoadFormatSourceBranch(TTree* t) override;
    virtual void LoadEvtContent(TreeReader& evtInfo) override;

	testGG_formatTreeArrayF( int totNum=comNum, int maxCSize=128 ) : formatTreeArray ( totNum, maxCSize ) {
	}
	~testGG_formatTreeArrayF() {
	}
};

#endif
