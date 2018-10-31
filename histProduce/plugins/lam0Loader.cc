#include "histProduce/histProduce/plugins/lam0Loader.h"
#include "BPHAnalysis/SpecificDecay/interface/BPHParticleMasses.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"

#include "DataFormats/PatCandidates/interface/Muon.h"

#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "CommonTools/Statistics/interface/ChiSquaredProbability.h"

#include "RecoVertex/VertexTools/interface/VertexDistanceXY.h"
#include "TMath.h"
#include "Math/VectorUtil.h"
#include "TVector3.h"

#include <TH1.h>
#include <TFile.h>
#include <TLorentzVector.h>

#include <set>
#include <string>
#include <iostream>
#include <fstream>
#include <math.h>

#include "histProduce/histProduce/interface/fourMom.h"

using namespace std;

#define SET_LABEL(NAME,PSET) ( NAME = PSET.getParameter<string>( #NAME ) )
// SET_LABEL(xyz,ps);
// is equivalent to
// xyz = ps.getParameter<string>( "xyx" )


lam0Loader::lam0Loader(const edm::ParameterSet & ps)
{

    useLam0 = (SET_LABEL(Lam0CandsLabel, ps) != "");
    useMyL0 = (SET_LABEL(MyL0CandsLabel, ps) != "");
    useBS   = (SET_LABEL(bsPointLabel  , ps) != "");
    if (useLam0) consume < vector<reco::VertexCompositeCandidate> >(Lam0CandsToken, Lam0CandsLabel);
    if (useMyL0) consume < vector<pat::CompositeCandidate>        >(MyL0CandsToken, MyL0CandsLabel);
    if (useBS)   consume < reco::BeamSpot > (bsPointToken, bsPointLabel);

    createHisto("lam0Mass", 60, 1.0, 1.3 );
    createHisto("lam0FDSig", 40, 0., 40. );
}


lam0Loader::~lam0Loader()
{

}


void lam0Loader::fillDescriptions(edm::ConfigurationDescriptions & descriptions)
{
    edm::ParameterSetDescription desc;
    desc.add < string > ("Lam0CandsLabel", "");
    desc.add < string > ("MyL0CandsLabel", "");
    desc.add < string > ("bsPointLabel", "");
    descriptions.add("process.lam0Loader", desc);
    return;
}


void lam0Loader::beginJob()
{
    // this event separator will have 3 value: 0,1,2.
    // 0 shows the first event of the file.
    // 1 and 2 are showing the changing event.
    // so that the event separator is 0 for first event. Then 1,2,1,2,1,2 ...

    return;
}

void lam0Loader::analyze(const edm::Event & ev, const edm::EventSetup & es)
{

    // get magnetic field
    edm::ESHandle < MagneticField > magneticField;
    es.get < IdealMagneticFieldRecord > ().get(magneticField);

    edm::Handle < reco::BeamSpot > beamSpotHandle;
    if (useBS)
        bsPointToken.get(ev, beamSpotHandle);
    if (!beamSpotHandle.isValid())
    { printf("----no beam spot loaded!\n"); return; }
    const reco::BeamSpot & bs = *beamSpotHandle;
    // if (!useLam0) return;
    // edm::Handle < std::vector<reco::VertexCompositeCandidate> > lam0Handle;
    // Lam0CandsToken.get(ev, lam0Handle);
    // if ( !lam0Handle.isValid() )
    // { printf("no lambda0 found in the event\n"); return; }
    // std::vector<reco::VertexCompositeCandidate>::const_iterator iter = lam0Handle->cbegin();
    // std::vector<reco::VertexCompositeCandidate>::const_iterator iend = lam0Handle->cend  ();
    // while ( iter != iend )
    // {
    //     const reco::VertexCompositeCandidate& lam0Cand = *iter++;
    //     fillHisto("lam0Mass", lam0Cand.mass());
    // }
    if (useLam0)
    {
        edm::Handle < std::vector<reco::VertexCompositeCandidate> > lam0Handle;
        Lam0CandsToken.get(ev, lam0Handle);
        if ( !lam0Handle.isValid() )
        { printf("no lambda0 found in the event\n"); return; }
        std::vector<reco::VertexCompositeCandidate>::const_iterator iter = lam0Handle->cbegin();
        std::vector<reco::VertexCompositeCandidate>::const_iterator iend = lam0Handle->cend  ();
        while ( iter != iend )
        {
            const reco::VertexCompositeCandidate& lam0Cand = *iter++;
            fillHisto("lam0Mass", lam0Cand.mass());
        }
    }
    if (useMyL0)
    {
        edm::Handle < std::vector<pat::CompositeCandidate> > myL0Handle;
        MyL0CandsToken.get(ev, myL0Handle);
        if ( !myL0Handle.isValid() )
        { printf("no lambda0 found in the event\n"); return; }
        std::cout <<  "lam0 size: " <<  myL0Handle->size() << std::endl;
        std::vector<pat::CompositeCandidate>::const_iterator iter = myL0Handle->cbegin();
        std::vector<pat::CompositeCandidate>::const_iterator iend = myL0Handle->cend  ();
        while ( iter != iend )
        {
            const pat::CompositeCandidate& lam0Cand = *iter++;
            double fdSig = usefulFuncs::getFlightDistanceSignificance( lam0Cand, &bs );
            if ( fdSig < 5. ) continue;
            std::cout << "hiiii filled!     ";
            fillHisto("lam0Mass", lam0Cand.userFloat("fitMass"));
            //printf("lam0Mass = %.4f , lam0 fitMass = %.4f", lam0Cand.mass(), lam0Cand.userFloat("fitMass"));
        }
    }


    return;

}


void lam0Loader::endJob()
{
    return;
}


void lam0Loader::fillHisto(const string & name, float x)
{
    map < string, TH1F * >::iterator iter = histoMap.find(name);
    map < string, TH1F * >::iterator iend = histoMap.end();
    if (iter == iend)
        return;
    iter->second->Fill(x);
    return;
}
void lam0Loader::createHisto(const string & name, int nbin, float hmin, float hmax)
{
    histoMap[name] = fs->make < TH1F > (name.c_str(), name.c_str(), nbin, hmin, hmax);
    return;
}




#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(lam0Loader);
