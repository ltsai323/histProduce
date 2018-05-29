#include "histProduce/histProduce/interface/tmainGen.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "math.h"
#include <stdio.h>

treeMainGen::treeMainGen( TFileDirectory* d, treeMain::Label l, const std::string& pName ) : treeMain( d, l, pName )
{
    // do something
}
treeMainGen::~treeMainGen()
{
    // do something
}
void treeMainGen::getByLabel_genParticle( fwlite::Event* ev )
{ _genHandle.getByLabel( *ev, "genParticles", "", "HLT" ); return;}

MatchRes treeMainGen::matchMC_CompositeCand( const pat::CompositeCandidate& cand, const std::vector<fourMom>& daugs, const double minDeltaR = 0.1 )
 {
    // match for a composite candidate, and their daughters. if three of them
    // this method is to find deltaR in three candidates, not for one particle.
    // For one particle, deltaR between genParticle & recoParticle may not be smallest one.
    // because the SIM for detectors & reconstructions owns error. the errors lead to the deltaR
    // will not become minimum value if you compare true particle & other particle in genParticle with recoParticle.
    // So this method is to find out deltaR of daughters & mother.
    // there are two algorithms:
    // 1. find out product of deltaRs and find minimum value.
    // 2. set a minimum deltaR value. three of them must be smaller than that.
    if ( !_genHandle->size() ) return MatchRes( false );
    std::vector<reco::GenParticle>::const_iterator iter = _genHandle->begin();
    std::vector<reco::GenParticle>::const_iterator iend = _genHandle->end  ();
    std::vector< std::pair< double, const reco::GenParticle* > > ccList;
    ccList.reserve(15);

    // find for composite candidate {{{
    // use pt ratio & status() (cand&daug) to roughly find out gpCand
    while ( iter != iend )
    {
        const reco::GenParticle& _genParticle = *iter++;

        // find for composite candidate.
        if ( _genParticle.status() != 2 ) continue;
        if ( _genParticle.numberOfDaughters() < daugs.size() ) continue;
        bool ownFinalStateParticle = false;
        for ( unsigned i = 0; i < _genParticle.numberOfDaughters(); ++i )
            if ( _genParticle.daughter(i)->status() == 1 ) ownFinalStateParticle = true;
        if ( !ownFinalStateParticle ) continue;

        double deltaPTRatio = fabs( cand.pt() - _genParticle.pt() ) / cand.pt();
        if ( deltaPTRatio > 0.5 ) continue;

        double tmpDeltaRVal = mcMatchVal( &cand, &_genParticle );

        if ( tmpDeltaRVal < minDeltaR )
        {
            std::pair< double, const reco::GenParticle* > tmpCands( tmpDeltaRVal, &_genParticle );
            ccList.emplace_back( tmpCands );
        }
    }
    // find for composite candidate end }}}

    // calculate deltaR = deltaR_dau1*deltaR_dau2*...
    // ccDeltaRresult is to find every composite candidate deltaR product. ( deltaR = deltaR_cc * deltaR_dau1 * ... )
    std::vector<MatchRes> ccDeltaRresult;
    ccDeltaRresult.reserve( ccList.size() );
    for ( const auto& ccPair : ccList )
    {
        std::vector<MatchRes> result;
        result.reserve( 10 );
        std::vector<unsigned> tmpIdx;
        calculateAllDeltaR( ccPair, daugs, tmpIdx, result, minDeltaR );

        // from result to find out the smallest deltaR product daughter combination.
        unsigned idx_smallestDR = 99;
        double smallestDR = 999.;
        for ( unsigned i=0; i<result.size();++i )
        {
            if ( result[i].getNDaughterRecorded() < daugs.size() ) continue;
            double tmpDR = result[i].getDeltaR();
            if ( tmpDR < smallestDR )
            {
                smallestDR = tmpDR;
                idx_smallestDR = i;
            }
        }
        if ( idx_smallestDR != 99 )
            ccDeltaRresult.emplace_back( result[idx_smallestDR] );
    }

    // find out the smallest deltaR candidate from ccDeltaRresult
    unsigned idx_smallestDR = 99;
    double smallestDR = 999.;
    for ( unsigned i=0; i<ccDeltaRresult.size(); ++i )
    {
        double tmpDR = ccDeltaRresult[i].getDeltaR();
        if ( tmpDR < smallestDR )
        {
           smallestDR = tmpDR;
           idx_smallestDR = i;
        }
    }
    if ( idx_smallestDR != 99 )
        return ccDeltaRresult[idx_smallestDR];
    return MatchRes( false );
}


// recursive function to check deltaR for all candidates.
// if you want to find lambdaB(with 3 daughters JPsi,p,K). Then you need to put 3 daughters!
// if you put 2 daughters (p,K). Most probably you will get a JPsi(with 2 daughters).
inline void treeMainGen::calculateAllDeltaR( const std::pair<double, const reco::GenParticle*> compCand, const std::vector<fourMom>& daugs, const std::vector<unsigned>& idxs, std::vector<MatchRes>& res, const double minDeltaR=0.1 )
{
    if ( idxs.size() == daugs.size() )
    {
        // calculate for deltaR_tot = deltaR1 * deltaR2 * ...
        double deltaR = compCand.first;
        const reco::GenParticle* gpCompCand = compCand.second;


        for ( unsigned i=0; i!=idxs.size(); ++i )
        {
            const fourMom* __d__ = &daugs[i];
            const reco::Candidate* __mcDau__ = gpCompCand->daughter(idxs[i]);
            //double dDeltaR =  mcMatchVal( &(daugs[i]), gpCompCand->daughter(idxs[i]) );
            double dDeltaR =  mcMatchVal( __d__, __mcDau__ );
            // cut for minDeltaR to each daughter
            if ( dDeltaR > minDeltaR ) return;
            deltaR *= dDeltaR;
        }
        MatchRes result( deltaR, gpCompCand );
        for ( unsigned i=0; i<idxs.size(); ++i )
            result.storeMatchRes( i, idxs[i] );
        res.push_back( result );
    }
    else if ( idxs.size() > daugs.size() ) return;
    else
    {
        // collect for daughters.
        // for example, if you have 4 daughters in the gpCompCand, but the daugs you input only 3
        // this algorithm calculates the combination:
        // (0,1,2), (0,1,3), (0,2,1), (0,2,3), (0,3,1), (0,3,2), ... and so on
        // the only constriant is to forbid the same index.
        //int N = daugs.size();
        unsigned N = compCand.second->numberOfDaughters();
        for ( unsigned i=0; i<N; ++i )
        {
            bool sameIdx = false;
            for ( const unsigned idx : idxs )
                if ( i == idx )
                {
                    sameIdx = true;
                    break;
                }
            if ( sameIdx ) continue;

            std::vector<unsigned> tmpIdx = idxs;
            tmpIdx.push_back( i );

            calculateAllDeltaR( compCand, daugs, tmpIdx, res, minDeltaR );
        }
    }
    return;
}
