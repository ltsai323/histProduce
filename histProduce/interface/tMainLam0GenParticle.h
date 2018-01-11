#ifndef __tMainLam0GenParticle_h__
#define __tMainLam0GenParticle_h__
#include "histProduce/histProduce/interface/tMain.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include <vector>
#include <utility> // std::pair

class treeMain_Lam0GenParticle : public treeMain
{
public:
    treeMain_Lam0GenParticle ( TFileDirectory* d);
    virtual void Process( fwlite::Event* ev ) override;
    virtual void Clear() override;
    virtual void regTree() override;
private:
    fwlite::Handle< std::vector<reco::GenParticle> > genHandle;
    fwlite::Handle< reco::BeamSpot > beamSpotHandle;
    // store the result of 'particle-MC genParticle pair'
    //std::vector< std::pair<const void*, const reco::GenParticle*> > pgPair;
    const double protonMass;
    const double   pionMass;

    // output the pair< deltaR, GenParticle >
template< typename myParticle >
    std::pair<double, reco::GenParticle> searchForGenParticle( const myParticle* cand, double minDeltaR = 0.8 )
    {
        reco::GenParticle gParticle;

        if ( !genHandle->size() ) return std::pair<double, reco::GenParticle>( 999., gParticle );
        std::vector<reco::GenParticle>::const_iterator iter = genHandle->begin();
        std::vector<reco::GenParticle>::const_iterator iend = genHandle->end  ();

        double minDeltaRVal = 999.;
        while ( iter != iend )
        {
            const reco::GenParticle& _genParticle = *iter++;
            // only keep pion, kaon, proton
            if ( _genParticle.status() != 1 ) continue;
            if ( fabs( _genParticle.pdgId() ) != 211 )
                if ( fabs( _genParticle.pdgId() ) != 321 )
                    if ( fabs( _genParticle.pdgId() ) != 2212 )
                        continue;
            double deltaPT = fabs( cand->transverse() - _genParticle.pt() );
            if ( deltaPT/cand->transverse() > 1.0 ) continue;

            //if ( fabs( _genParticle.pt() - cand->transverse() ) > 0.5 ) continue;
            double tmpDeltaRVal = usefulFuncs::mcMatchVal( cand, &_genParticle );
            if ( tmpDeltaRVal < minDeltaRVal )
            {
                minDeltaRVal = tmpDeltaRVal;
                gParticle = _genParticle;
            }
        }

        return std::pair<double, reco::GenParticle>( minDeltaRVal, gParticle );
    }

    double lam0Mass;
    double lam0FlightDistance2d, lam0Cosa2d, lam0Vtxprob, lam0Pt, lam0SortingNumber;
    double ptkMom, ptkDEDX_Harmonic, ptkDEDX_pixelHrm, ptkIPt;
    double ntkMom, ntkDEDX_Harmonic, ntkDEDX_pixelHrm, ntkIPt;
};


#endif
