#include "histProduce/histProduce/plugins/treeCreatingSpecificDecay.h"
#include "BPHAnalysis/SpecificDecay/interface/BPHParticleMasses.h"

#include "FWCore/Framework/interface/MakerMacros.h"

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


treeCreatingSpecificDecay::treeCreatingSpecificDecay( const edm::ParameterSet& ps ) :
    dir( fs->mkdir("SDanalyzer") ), myData( &dir )
{

  useOnia = ( SET_LABEL( oniaCandsLabel, ps ) != "" );
  useTkTk = ( SET_LABEL( tktkCandsLabel, ps ) != "" );
  useLbTk = ( SET_LABEL( LbTkCandsLabel, ps ) != "" );
  useBS   = ( SET_LABEL(   bsPointLabel, ps ) != "" );
  if ( useOnia ) consume< vector<pat::CompositeCandidate> >( oniaCandsToken,
                                                             oniaCandsLabel );
 if ( useTkTk )  consume< vector<pat::CompositeCandidate> >( tktkCandsToken,
                                                             tktkCandsLabel );
  if ( useLbTk ) consume< vector<pat::CompositeCandidate> >( LbTkCandsToken,
                                                             LbTkCandsLabel );
  if ( useBS )   consume< reco::BeamSpot                  >( bsPointToken,
                                                             bsPointLabel );

}


treeCreatingSpecificDecay::~treeCreatingSpecificDecay() {

}


void treeCreatingSpecificDecay::fillDescriptions(
                            edm::ConfigurationDescriptions& descriptions ) {
   edm::ParameterSetDescription desc;
   desc.add<string>( "oniaCandsLabel", "" );
   desc.add<string>( "tktkCandsLabel", "" );
   desc.add<string>( "LbTkCandsLabel", "" );
   desc.add<string>( "bsPointLabel", "" );
   descriptions.add( "process.treeCreatingSpecificDecay", desc );
   return;
}


void treeCreatingSpecificDecay::beginJob() {

  return;
}

void treeCreatingSpecificDecay::analyze( const edm::Event& ev,
                                     const edm::EventSetup& es ) {

  // get magnetic field
  edm::ESHandle<MagneticField> magneticField;
  es.get<IdealMagneticFieldRecord>().get( magneticField );

  // get object collections
  // collections are got through "BPHTokenWrapper" interface to allow
  // uniform access in different CMSSW versions

  //////////// quarkonia ////////////

  edm::Handle< vector<pat::CompositeCandidate> > oniaCands;
  if ( useOnia )
    oniaCandsToken.get( ev, oniaCands );

  edm::Handle< reco::BeamSpot > beamSpotHandle;
  if ( useBS )
      bsPointToken.get( ev, beamSpotHandle );
  if ( !beamSpotHandle.isValid() ) return;


  //////////// LbTk ////////////

  edm::Handle< vector<pat::CompositeCandidate> > lbTkCands;
  int ilbTk;
  int nlbTk = 0;
  if ( useLbTk ) {
    LbTkCandsToken.get( ev, lbTkCands );
    nlbTk = lbTkCands->size();
  }

  for ( ilbTk = 0; ilbTk < nlbTk; ++ ilbTk ) {
        // pv not recorded, use BS
        if ( lbTkCands->size()  == 0 ) return;
        const reco::BeamSpot& bs = *beamSpotHandle;

        std::vector< std::pair< double, const pat::CompositeCandidate*> > candsSorted;
        candsSorted.clear();
        candsSorted.reserve( lbTkCands->size() );
        std::vector<pat::CompositeCandidate>::const_iterator handleIter = lbTkCands->begin();
        std::vector<pat::CompositeCandidate>::const_iterator handleIend = lbTkCands->end  ();
        while( handleIter != handleIend )
        {
            const pat::CompositeCandidate& cand = *handleIter++;
            if ( !cand.hasUserFloat("fitMass") ) continue;
            if ( !cand.hasUserData("TkTk/Proton.fitMom") ) continue;
            if ( !cand.hasUserData(  "TkTk/Kaon.fitMom") ) continue;
            if ( !cand.hasUserFloat("TkTk/Proton.IPt") ) continue;
            if ( !cand.hasUserFloat(  "TkTk/Kaon.IPt") ) continue;
            if ( !cand.hasUserData( "fitVertex" ) ) continue;
            if ( !cand.hasUserData( "refToJPsi" ) ) continue;

            const GlobalVector* dPTR[2] = {nullptr};
            dPTR[0] = cand.userData<GlobalVector>("TkTk/Proton.fitMom");
            dPTR[1] = cand.userData<GlobalVector>("TkTk/Kaon.fitMom");

            const GlobalVector* muPTR[2] = {nullptr};
            muPTR[0] = cand.userData<GlobalVector>("JPsi/MuPos.fitMom");
            muPTR[1] = cand.userData<GlobalVector>("JPsi/MuNeg.fitMom");

            fourMom pMu ( muPTR[0]->x(), muPTR[0]->y(), muPTR[0]->z() );
            fourMom nMu ( muPTR[1]->x(), muPTR[1]->y(), muPTR[1]->z() );
            fourMom pTk ( dPTR[0]->x(), dPTR[0]->y(), dPTR[0]->z() );
            fourMom nTk ( dPTR[1]->x(), dPTR[1]->y(), dPTR[1]->z() );
            //fourMom tktk = pTk+nTk;
            fourMom fourTk = pTk+nTk+pMu+nMu;
            // preselection
            if ( pMu.transverse() < 5.0 ) continue;
            if ( nMu.transverse() < 5.0 ) continue;
            if ( pMu.Momentum()   < 5.0 ) continue;
            if ( nMu.Momentum()   < 5.0 ) continue;
            if ( pTk.transverse() < 1.0 ) continue;
            if ( nTk.transverse() < 1.0 ) continue;
            if ( pTk.Momentum()   < 1.0 ) continue;
            if ( nTk.Momentum()   < 1.0 ) continue;

            if ( fourTk.transverse()< 12. ) continue;
            if ( fourTk.Momentum()  < 12. ) continue;


            const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
            if ( _vtx == nullptr ) continue;
            double fdSig = usefulFuncs::getFlightDistanceSignificance ( cand, &bs );
            if ( fdSig < 3.0 ) continue;
            double cos2d = usefulFuncs::getCosa2d( cand, &bs );
            double vtxprob = TMath::Prob( _vtx->chi2(), _vtx->ndof() );
            if ( cos2d < 0.99 ) continue;
            if ( vtxprob < 0.15 ) continue;

            candsSorted.emplace_back( vtxprob, &cand );
        }
        if ( candsSorted.size() == 0 ) return;

        std::vector< std::pair<double, const pat::CompositeCandidate*> >::const_iterator iter = candsSorted.begin();
        std::vector< std::pair<double, const pat::CompositeCandidate*> >::const_iterator iend = candsSorted.end  ();

        while ( iter != iend )
        {
            myData.Clear();
            const pat::CompositeCandidate& cand = *(iter++->second);

            // first one is proton and second one is kaon ( consider bigger momentum with heavier particle )
            const GlobalVector* dPTR[2] = {nullptr};
            dPTR[0] = cand.userData<GlobalVector>("TkTk/Proton.fitMom");
            dPTR[1] = cand.userData<GlobalVector>("TkTk/Kaon.fitMom");

            const GlobalVector* muPTR[2] = {nullptr};
            muPTR[0] = cand.userData<GlobalVector>("JPsi/MuPos.fitMom");
            muPTR[1] = cand.userData<GlobalVector>("JPsi/MuNeg.fitMom");

            fourMom pMu ( muPTR[0]->x(), muPTR[0]->y(), muPTR[0]->z() );
            fourMom nMu ( muPTR[1]->x(), muPTR[1]->y(), muPTR[1]->z() );
            pMu.setMass( 0.1056583745 );
            nMu.setMass( 0.1056583745 );
            fourMom pTk ( dPTR[0]->x(), dPTR[0]->y(), dPTR[0]->z() );
            fourMom nTk ( dPTR[1]->x(), dPTR[1]->y(), dPTR[1]->z() );
            fourMom tktk;
            fourMom fourTk;

            // search for pentaQuark
            // jpsip = jpsi + proton, jpsiP = jpsi + anti-proton
            fourMom jpsip, jpsipBar;
            pTk.setMass( BPHParticleMasses::protonMass );
            nTk.setMass( BPHParticleMasses::protonMass );
            jpsip = pMu + nMu + pTk;
            jpsipBar = pMu + nMu + nTk;

            // search for lam0
            pTk.setMass( BPHParticleMasses::protonMass );
            nTk.setMass( BPHParticleMasses::  pionMass );
            tktk = pTk+nTk;
            fourTk = pMu + nMu + tktk;

            myData.dataD[dataRecord::fake_Lam0Mass] = tktk.Mag();
            myData.dataD[dataRecord::fake_LbL0Mass] = fourTk.Mag();

            // search for k short
            pTk.setMass( BPHParticleMasses::pionMass );
            nTk.setMass( BPHParticleMasses::pionMass );
            tktk = pTk+nTk;
            fourTk = pMu + nMu + tktk;

            myData.dataD[dataRecord::fake_KshortMass] = tktk.Mag();
            myData.dataD[dataRecord::fake_mumupipiMass] = fourTk.Mag();

            // search for k(892)
            pTk.setMass( BPHParticleMasses::kaonMass );
            nTk.setMass( BPHParticleMasses::pionMass );
            tktk = pTk+nTk;
            fourTk = pMu + nMu + tktk;

            myData.dataD[dataRecord::fake_KstarMass] = tktk.Mag();
            myData.dataD[dataRecord::fake_BdMass] = fourTk.Mag();

            // search for phi(1020)
            pTk.setMass( BPHParticleMasses::kaonMass );
            nTk.setMass( BPHParticleMasses::kaonMass );
            tktk = pTk+nTk;
            fourTk = pMu + nMu + tktk;

            myData.dataD[dataRecord::fake_PhiMass] = tktk.Mag();
            myData.dataD[dataRecord::fake_BsMass] = fourTk.Mag();


            const pat::CompositeCandidate* jpsi = usefulFuncs::getByRef<pat::CompositeCandidate>( cand, "refToJPsi" );
            //const reco::Vertex* pv = usefulFuncs::get<reco::Vertex>( *jpsi, "fitVertex" );

            myData.dataD[dataRecord::lbtkMass] = cand.userFloat( "fitMass" );
            myData.dataD[dataRecord::lbtkFlightDistance2d] = usefulFuncs::getFlightDistance ( cand, &bs );
            myData.dataD[dataRecord::lbtkFlightDistanceSig]= usefulFuncs::getFlightDistanceSignificance ( cand, &bs );
            const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
            myData.dataD[dataRecord::lbtkVtxprob] = TMath::Prob( _vtx->chi2(), _vtx->ndof() );
            myData.dataD[dataRecord::lbtkCosa2d] = usefulFuncs::getCosa2d(cand,&bs);

            myData.dataD[dataRecord::targetJpsiP_mass] = jpsip.Mag();
            myData.dataD[dataRecord::targetJpsiP_pt] = jpsip.transverse();
            myData.dataD[dataRecord::targetJpsiPBar_mass] = jpsipBar.Mag();
            myData.dataD[dataRecord::targetJpsiPBar_pt] = jpsipBar.transverse();

            myData.dataD[dataRecord::lbtkMom]= fourTk.Momentum();
            myData.dataD[dataRecord::lbtkPt] = fourTk.transverse();
            myData.dataD[dataRecord::tktkPt] = tktk.transverse();
            myData.dataD[dataRecord::tktkMom]= tktk.Momentum();
            myData.dataD[dataRecord::ptkPt]  = pTk.transverse();
            myData.dataD[dataRecord::ntkPt]  = nTk.transverse();
            myData.dataD[dataRecord::ptkMom] = pTk.Momentum();
            myData.dataD[dataRecord::ntkMom] = nTk.Momentum();
            myData.dataD[dataRecord::ptkIPt] = cand.userFloat("TkTk/Proton.IPt");
            myData.dataD[dataRecord::ntkIPt] = cand.userFloat("TkTk/Kaon.IPt");
            myData.dataD[dataRecord::ptkIPtErr] = cand.userFloat("TkTk/Proton.IPt.Error");
            myData.dataD[dataRecord::ntkIPtErr] = cand.userFloat("TkTk/Kaon.IPt.Error");
            if ( cand.hasUserFloat("TkTk/Proton.dEdx.pixelHrm") )
                myData.dataD[dataRecord::ptkDEDX_pixelHrm] = cand.userFloat( "TkTk/Proton.dEdx.pixelHrm" );
            if ( cand.hasUserFloat(  "TkTk/Kaon.dEdx.pixelHrm") )
                myData.dataD[dataRecord::ntkDEDX_pixelHrm] = cand.userFloat( "TkTk/Kaon.dEdx.pixelHrm" );
            if ( cand.hasUserFloat("TkTk/Proton.dEdx.Harmonic") )
                myData.dataD[dataRecord::ptkDEDX_Harmonic] = cand.userFloat( "TkTk/Proton.dEdx.Harmonic" );
            if ( cand.hasUserFloat(  "TkTk/Kaon.dEdx.Harmonic") )
                myData.dataD[dataRecord::ntkDEDX_Harmonic] = cand.userFloat(   "TkTk/Kaon.dEdx.Harmonic" );
            myData.thisTree()->Fill();
        }
  }
  return;

}


void treeCreatingSpecificDecay::endJob() {
  return;
}


void treeCreatingSpecificDecay::fillHisto( const string& name,
                                       const pat::CompositeCandidate& cand ) {
  float mass = ( cand.hasUserFloat( "fitMass" ) ?
                 cand.   userFloat( "fitMass" ) : -1 );
  fillHisto( "mass" + name, cand.mass() );
  fillHisto( "mfit" + name, mass );
  return;
}


void treeCreatingSpecificDecay::fillHisto( const string& name, float x ) {
  map<string,TH1F*>::iterator iter = histoMap.find( name );
  map<string,TH1F*>::iterator iend = histoMap.end();
  if ( iter == iend ) return;
  iter->second->Fill( x );
  return;
}


void treeCreatingSpecificDecay::createHisto( const string& name,
                                         int nbin, float hmin, float hmax ) {
  histoMap[name] = fs->make<TH1F>( name.c_str(), name.c_str(),
                                   nbin, hmin, hmax );
  return;
}

#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE( treeCreatingSpecificDecay );
