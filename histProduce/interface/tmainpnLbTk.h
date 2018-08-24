#ifndef __tMainpnLbTk_h__
#define __tMainpnLbTk_h__
#include "histProduce/histProduce/interface/tmain.h"
#include <math.h>

class treeMain_pnLbTk : public treeMain
{
public:
    treeMain_pnLbTk ( TFileDirectory* d);
    virtual void Process( fwlite::Event* ev ) override;
    virtual void Clear() override;

    // automatically called by constructor.
    virtual void RegTree() override;
    virtual void GetByLabel( fwlite::Event* ev ) override;

    // used for final analysis
    void setBranchAddress( TTree* inputTree );

    friend class treeCreatingSpecificDecay;
private:
    const double kaonMass, protonMass, pionMass;
    void getByLabel_BS( fwlite::Event* ev );
    fwlite::Handle< reco::BeamSpot > beamSpotHandle;
    void getByLabel_antiCand( fwlite::Event* ev );
    fwlite::Handle< std::vector<pat::CompositeCandidate> > antiCandHandle;
    treeMain::Label antiCandLabel;
public:
    std::vector< std::pair< std::pair<double,const pat::CompositeCandidate*>, std::pair<double,const pat::CompositeCandidate*> > > pairpnCands
    ( std::vector<std::pair<double,const pat::CompositeCandidate*> >& pCands, std::vector<std::pair<double, const pat::CompositeCandidate*> >& nCands );
    double deltaRCalc;
    bool sameVal( double val1, double val2, double minValue )
    { if ( fabs(val1-val2) < minValue ) return true; return false; }
    bool theSameDaugs( const reco::Candidate* daug1, const reco::Candidate* daug2, double minValue )
    { if ( sameVal(daug1->eta(), daug2->eta(), minValue) && sameVal(daug1->phi(), daug2->phi(), minValue) ) return true; return false; }
    bool theSameCompCand( const pat::CompositeCandidate* cand1, const pat::CompositeCandidate* cand2 );
    enum dataVarD
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
        totNumD
    };
    enum dataVarI
    {
        totNumI
    };
    // double data to store variables
    double dataD[totNumD];
    // int data to store variables
    int dataI[totNumI];
    TH1D* hParEta  ;
    TH1D* hParCos2D;
    TH1D* hParCos3D;
    TH1D* hParCos2D_2;
    TH1D* hParCos2D_3;
};
#endif
