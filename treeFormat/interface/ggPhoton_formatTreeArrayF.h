
#ifndef __UPDATED_ggPhoton_formatTreeArrayF__
#define __UPDATED_ggPhoton_formatTreeArrayF__
#include "histProduce/treeFormat/interface/formatTreeArray.hpp"
struct ggPhoton_formatTreeArrayF : public formatTreeArray<float>
{
public:
    enum readVar
    {
        phoE,
        phoSigmaE,
        phoEt,
        phoEta,
        phoPhi,
        phoCalibE,
        phoCalibEt,
        phoSCE,
        phoSCRawE,
        phoESEnP1,
        phoESEnP2,
        phoSCEta,
        phoSCPhi,
        phoSCEtaWidth,
        phoSCPhiWidth,
        phoSCBrem,
        phoR9,
        phoHoverE,
        phoESEffSigmaRR,
        phoSigmaIEtaIEtaFull5x5,
        phoSigmaIEtaIPhiFull5x5,
        phoSigmaIPhiIPhiFull5x5,
        phoE2x2Full5x5,
        phoE5x5Full5x5,
        phoR9Full5x5,
        phoPFChIso,
        phoPFChPVIso,
        phoPFPhoIso,
        phoPFNeuIso,
        phoPFChWorstIso,
        phoPFChWorstVetoIso,
        phoEcalPFClusterIso,
        phoHcalPFClusterIso,
        phoIDMVA,
        phoSeedTime,
        phoSeedEnergy,
        phoMIPTotEnergy,
        phoMIPChi2,
        phoMIPSlope,
        phoMIPIntercept,
        phoMIPNhitCone,
        phoMIPIsHalo,
        phoScale_stat_up,
        phoScale_stat_dn,
        phoScale_syst_up,
        phoScale_syst_dn,
        phoScale_gain_up,
        phoScale_gain_dn,
        phoResol_rho_up,
        phoResol_rho_dn,
        phoResol_phi_up,
        phoResol_phi_dn,
        comNum
    };
    virtual void RegFormatTree(TTree* t) override;
    // LoadFormatSourceBranch and LoadEvtContent are two methods to load data. choose one.
    virtual void LoadFormatSourceBranch(TTree* t) override;
    virtual void LoadEvtContent(TreeReader& evtInfo) override;

    explicit ggPhoton_formatTreeArrayF( int totNum=comNum, int maxCSize=128 ) : formatTreeArray ( totNum, maxCSize ) {
    }
    virtual ~ggPhoton_formatTreeArrayF() {
    }
};

#endif
