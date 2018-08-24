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
  usepTks = ( SET_LABEL( pTksCandsLabel, ps ) != "" );
  usepL0B = ( SET_LABEL( pL0BCandsLabel, ps ) != "" );
  usenTks = ( SET_LABEL( nTksCandsLabel, ps ) != "" );
  usenL0B = ( SET_LABEL( nL0BCandsLabel, ps ) != "" );
  useBS   = ( SET_LABEL(   bsPointLabel, ps ) != "" );
  if ( useOnia ) consume< vector<pat::CompositeCandidate> >( oniaCandsToken,
                                                             oniaCandsLabel );
  if ( usepTks ) consume< vector<pat::CompositeCandidate> >( pTksCandsToken,
                                                             pTksCandsLabel );
  if ( usepL0B ) consume< vector<pat::CompositeCandidate> >( pL0BCandsToken,
                                                             pL0BCandsLabel );
  if ( usenTks ) consume< vector<pat::CompositeCandidate> >( nTksCandsToken,
                                                             nTksCandsLabel );
  if ( usenL0B ) consume< vector<pat::CompositeCandidate> >( nL0BCandsToken,
                                                             nL0BCandsLabel );
  if ( useBS )   consume< reco::BeamSpot                  >( bsPointToken,
                                                             bsPointLabel );

}


treeCreatingSpecificDecay::~treeCreatingSpecificDecay() {

}


void treeCreatingSpecificDecay::fillDescriptions(
                            edm::ConfigurationDescriptions& descriptions ) {
   edm::ParameterSetDescription desc;
   desc.add<string>( "oniaCandsLabel", "" );
   desc.add<string>( "pTksCandsLabel", "" );
   desc.add<string>( "pL0BCandsLabel", "" );
   desc.add<string>( "nTksCandsLabel", "" );
   desc.add<string>( "nL0BCandsLabel", "" );
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
  const reco::BeamSpot& bs = *beamSpotHandle;


  //////////// LbTk ////////////

  edm::Handle< vector<pat::CompositeCandidate> > pL0BCands;
  edm::Handle< vector<pat::CompositeCandidate> > nL0BCands;
  if ( usepL0B )
    pL0BCandsToken.get( ev, pL0BCands );
  if ( usenL0B )
    nL0BCandsToken.get( ev, nL0BCands );




  std::vector<pat::CompositeCandidate>::const_iterator handleIter;
  std::vector<pat::CompositeCandidate>::const_iterator handleIend;
  // preselection for pCand {{{
  if ( !pL0BCands.isValid() ) return;
  if ( pL0BCands->size() == 0 ) return;

  std::vector< std::pair< double, const pat::CompositeCandidate*> > candsSorted;
  candsSorted.clear();
  candsSorted.reserve( pL0BCands->size() );
  handleIter = pL0BCands->begin();
  handleIend = pL0BCands->end  ();
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
  // preselect for pCand end }}}
  // preselection for nCand {{{
  if ( !nL0BCands.isValid() ) return;
  if ( nL0BCands->size() == 0 ) return;

  std::vector< std::pair< double, const pat::CompositeCandidate*> > antiCandSort;
  antiCandSort.clear();
  antiCandSort.reserve( nL0BCands->size() );
  handleIter = nL0BCands->begin();
  handleIend = nL0BCands->end  ();
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

      antiCandSort.emplace_back( vtxprob, &cand );
  }
  if ( antiCandSort.size() == 0 ) return;
  // preselect for nCand end }}}

  std::vector< std::pair< std::pair<double,const pat::CompositeCandidate*>, std::pair<double,const pat::CompositeCandidate*> > > pairCandidates;
  pairCandidates = pairpnCands( candsSorted, antiCandSort ); 
  

  std::vector< std::pair< std::pair<double,const pat::CompositeCandidate*>, std::pair<double,const pat::CompositeCandidate*> > >::const_iterator iter = pairCandidates.cbegin();
  std::vector< std::pair< std::pair<double,const pat::CompositeCandidate*>, std::pair<double,const pat::CompositeCandidate*> > >::const_iterator iend = pairCandidates.cend  ();

  while ( iter != iend )
  {
      myData.Clear();
      const std::pair<double,const pat::CompositeCandidate*>& pSet = iter->first;
      const std::pair<double,const pat::CompositeCandidate*>& nSet = iter->second;
      ++iter;

      const pat::CompositeCandidate* pCand = pSet.second;
      const pat::CompositeCandidate* nCand = nSet.second;

      // pos particle rebuilt {{{
      // first one is proton and second one is kaon ( consider bigger momentum with heavier particle )
      const GlobalVector* pDaugsPTR[4] = {nullptr};
      pDaugsPTR[0] = pCand->userData<GlobalVector>("JPsi/MuPos.fitMom");
      pDaugsPTR[1] = pCand->userData<GlobalVector>("JPsi/MuNeg.fitMom");
      pDaugsPTR[2] = pCand->userData<GlobalVector>("TkTk/Proton.fitMom");
      pDaugsPTR[3] = pCand->userData<GlobalVector>("TkTk/Kaon.fitMom");

      fourMom pMuPos ( pDaugsPTR[0]->x(), pDaugsPTR[0]->y(), pDaugsPTR[0]->z() );
      fourMom pMuNeg ( pDaugsPTR[1]->x(), pDaugsPTR[1]->y(), pDaugsPTR[1]->z() );
      pMuPos.setMass( 0.1056583745 );
      pMuNeg.setMass( 0.1056583745 );
      fourMom pTkPos ( pDaugsPTR[2]->x(), pDaugsPTR[2]->y(), pDaugsPTR[2]->z() );
      fourMom pTkNeg ( pDaugsPTR[3]->x(), pDaugsPTR[3]->y(), pDaugsPTR[3]->z() );
      fourMom pTkTk;
      fourMom pFourTk;

      // search for pentaQuark
      // jpsip = jpsi + proton, jpsiP = jpsi + anti-proton
      fourMom pJpsip, pJpsipBar;
      pTkPos.setMass( BPHParticleMasses::protonMass );
      pTkNeg.setMass( BPHParticleMasses::protonMass );
      pJpsip = pMuPos + pMuNeg + pTkPos;
      pJpsipBar = pMuPos + pMuNeg + pTkNeg;

      myData.dataD[dataRecord::ptargetJpsiP_mass] = pJpsip.Mag();
      myData.dataD[dataRecord::ptargetJpsiP_pt] = pJpsip.transverse();
      myData.dataD[dataRecord::ptargetJpsiPBar_mass] = pJpsipBar.Mag();
      myData.dataD[dataRecord::ptargetJpsiPBar_pt] = pJpsipBar.transverse();

      // search for lam0
      pTkPos.setMass( BPHParticleMasses::protonMass );
      pTkNeg.setMass( BPHParticleMasses::  pionMass );
      pTkTk = pTkPos + pTkNeg;
      pFourTk = pMuPos + pMuNeg + pTkTk;

      myData.dataD[dataRecord::pfake_Lam0Mass] = pTkTk.Mag();
      myData.dataD[dataRecord::pfake_LbL0Mass] = pFourTk.Mag();

      // search for k short
      pTkPos.setMass( BPHParticleMasses::pionMass );
      pTkNeg.setMass( BPHParticleMasses::pionMass );
      pTkTk = pTkPos + pTkNeg;
      pFourTk = pMuPos + pMuNeg + pTkTk;

      myData.dataD[dataRecord::pfake_KshortMass] = pTkTk.Mag();
      myData.dataD[dataRecord::pfake_mumupipiMass] = pFourTk.Mag();

      // search for k(892)
      pTkPos.setMass( BPHParticleMasses::kaonMass );
      pTkNeg.setMass( BPHParticleMasses::pionMass );
      pTkTk = pTkPos + pTkNeg;
      pFourTk = pMuPos + pMuNeg + pTkTk;

      myData.dataD[dataRecord::pfake_KstarMass] = pTkTk.Mag();
      myData.dataD[dataRecord::pfake_BdMass] = pFourTk.Mag();

      // search for phi(1020)
      pTkPos.setMass( BPHParticleMasses::kaonMass );
      pTkNeg.setMass( BPHParticleMasses::kaonMass );
      pTkTk = pTkPos + pTkNeg;
      pFourTk = pMuPos + pMuNeg + pTkTk;

      myData.dataD[dataRecord::pfake_PhiMass] = pTkTk.Mag();
      myData.dataD[dataRecord::pfake_BsMass] = pFourTk.Mag();
      // pos particle rebuilt end }}}
      // neg particle rebuilt {{{
      // first one is proton and second one is kaon ( consider bigger momentum with heavier particle )
      const GlobalVector* nDaugsPTR[4] = {nullptr};
      nDaugsPTR[0] = nCand->userData<GlobalVector>("JPsi/MuPos.fitMom");
      nDaugsPTR[1] = nCand->userData<GlobalVector>("JPsi/MuNeg.fitMom");
      nDaugsPTR[2] = nCand->userData<GlobalVector>("TkTk/Proton.fitMom");
      nDaugsPTR[3] = nCand->userData<GlobalVector>("TkTk/Kaon.fitMom");

      fourMom nMuPos ( nDaugsPTR[0]->x(), nDaugsPTR[0]->y(), nDaugsPTR[0]->z() );
      fourMom nMuNeg ( nDaugsPTR[1]->x(), nDaugsPTR[1]->y(), nDaugsPTR[1]->z() );
      nMuPos.setMass( 0.1056583745 );
      nMuNeg.setMass( 0.1056583745 );
      fourMom nTkPos ( nDaugsPTR[2]->x(), nDaugsPTR[2]->y(), nDaugsPTR[2]->z() );
      fourMom nTkNeg ( nDaugsPTR[3]->x(), nDaugsPTR[3]->y(), nDaugsPTR[3]->z() );
      fourMom nTkTk;
      fourMom nFourTk;

      // search for pentaQuark
      // jpsip = jpsi + proton, jpsiP = jpsi + anti-proton
      fourMom nJpsip, nJpsipBar;
      nTkPos.setMass( BPHParticleMasses::protonMass );
      nTkNeg.setMass( BPHParticleMasses::protonMass );
      nJpsip = nMuPos + nMuNeg + nTkPos;
      nJpsipBar = nMuPos + nMuNeg + nTkNeg;

      myData.dataD[dataRecord::ntargetJpsiP_mass] = nJpsip.Mag();
      myData.dataD[dataRecord::ntargetJpsiP_pt] = nJpsip.transverse();
      myData.dataD[dataRecord::ntargetJpsiPBar_mass] = nJpsipBar.Mag();
      myData.dataD[dataRecord::ntargetJpsiPBar_pt] = nJpsipBar.transverse();

      // search for lam0
      nTkPos.setMass( BPHParticleMasses::protonMass );
      nTkNeg.setMass( BPHParticleMasses::  pionMass );
      nTkTk = nTkPos + nTkNeg;
      nFourTk = nMuPos + nMuNeg + nTkTk;

      myData.dataD[dataRecord::nfake_Lam0Mass] = nTkTk.Mag();
      myData.dataD[dataRecord::nfake_LbL0Mass] = nFourTk.Mag();

      // search for k short
      nTkPos.setMass( BPHParticleMasses::pionMass );
      nTkNeg.setMass( BPHParticleMasses::pionMass );
      nTkTk = nTkPos + nTkNeg;
      nFourTk = nMuPos + nMuNeg + nTkTk;

      myData.dataD[dataRecord::nfake_KshortMass] = nTkTk.Mag();
      myData.dataD[dataRecord::nfake_mumupipiMass] = nFourTk.Mag();

      // search for k(892)
      nTkPos.setMass( BPHParticleMasses::kaonMass );
      nTkNeg.setMass( BPHParticleMasses::pionMass );
      nTkTk = nTkPos + nTkNeg;
      nFourTk = nMuPos + nMuNeg + nTkTk;

      myData.dataD[dataRecord::nfake_KstarMass] = nTkTk.Mag();
      myData.dataD[dataRecord::nfake_BdMass] = nFourTk.Mag();

      // search for phi(1020)
      nTkPos.setMass( BPHParticleMasses::kaonMass );
      nTkNeg.setMass( BPHParticleMasses::kaonMass );
      nTkTk = nTkPos + nTkNeg;
      nFourTk = nMuPos + nMuNeg + nTkTk;

      myData.dataD[dataRecord::nfake_PhiMass] = nTkTk.Mag();
      myData.dataD[dataRecord::nfake_BsMass] = nFourTk.Mag();
      // neg particle rebuilt end }}}


      //const pat::CompositeCandidate* pJpsi = usefulFuncs::getByRef<pat::CompositeCandidate>( *pCand, "refToJPsi" );
      //const reco::Vertex* pv = usefulFuncs::get<reco::Vertex>( *pJpsi, "fitVertex" );

      myData.dataD[dataRecord::plbtkMass] = pCand->userFloat( "fitMass" );
      myData.dataD[dataRecord::plbtkFlightDistance2d] = usefulFuncs::getFlightDistance ( *pCand, &bs );
      myData.dataD[dataRecord::plbtkFlightDistanceSig]= usefulFuncs::getFlightDistanceSignificance ( *pCand, &bs );

      const reco::Vertex* pCandVtx = usefulFuncs::get<reco::Vertex>( *pCand, "fitVertex" );
      myData.dataD[dataRecord::plbtkVtxprob] = TMath::Prob( pCandVtx->chi2(), pCandVtx->ndof() );
      myData.dataD[dataRecord::plbtkCosa2d] = usefulFuncs::getCosa2d(*pCand,&bs);

      myData.dataD[dataRecord::plbtkMom]= pFourTk.Momentum();
      myData.dataD[dataRecord::plbtkPt] = pFourTk.transverse();
      myData.dataD[dataRecord::ptktkPt] = pTkTk.transverse();
      myData.dataD[dataRecord::ptktkMom]= pTkTk.Momentum();
      myData.dataD[dataRecord::pptonPt]  = pTkPos.transverse();
      myData.dataD[dataRecord::pkaonPt]  = pTkNeg.transverse();
      myData.dataD[dataRecord::pptonMom] = pTkPos.Momentum();
      myData.dataD[dataRecord::pkaonMom] = pTkNeg.Momentum();
      myData.dataD[dataRecord::pptonIPt] = pCand->userFloat("TkTk/Proton.IPt");
      myData.dataD[dataRecord::pkaonIPt] = pCand->userFloat("TkTk/Kaon.IPt");
      myData.dataD[dataRecord::pptonIPtErr] = pCand->userFloat("TkTk/Proton.IPt.Error");
      myData.dataD[dataRecord::pkaonIPtErr] = pCand->userFloat("TkTk/Kaon.IPt.Error");
      if ( pCand->hasUserFloat("TkTk/Proton.dEdx.pixelHrm") )
          myData.dataD[dataRecord::pptonDEDX_pixelHrm] = pCand->userFloat( "TkTk/Proton.dEdx.pixelHrm" );
      if ( pCand->hasUserFloat(  "TkTk/Kaon.dEdx.pixelHrm") )
          myData.dataD[dataRecord::pkaonDEDX_pixelHrm] = pCand->userFloat( "TkTk/Kaon.dEdx.pixelHrm" );
      if ( pCand->hasUserFloat("TkTk/Proton.dEdx.Harmonic") )
          myData.dataD[dataRecord::pptonDEDX_Harmonic] = pCand->userFloat( "TkTk/Proton.dEdx.Harmonic" );
      if ( pCand->hasUserFloat(  "TkTk/Kaon.dEdx.Harmonic") )
          myData.dataD[dataRecord::pkaonDEDX_Harmonic] = pCand->userFloat(   "TkTk/Kaon.dEdx.Harmonic" );




      //const pat::CompositeCandidate* nJpsi = usefulFuncs::getByRef<pat::CompositeCandidate>( *nCand, "refToJPsi" );
      //const reco::Vertex* pv = usefulFuncs::get<reco::Vertex>( *nJpsi, "fitVertex" );

      myData.dataD[dataRecord::nlbtkMass] = nCand->userFloat( "fitMass" );
      myData.dataD[dataRecord::nlbtkFlightDistance2d] = usefulFuncs::getFlightDistance ( *nCand, &bs );
      myData.dataD[dataRecord::nlbtkFlightDistanceSig]= usefulFuncs::getFlightDistanceSignificance ( *nCand, &bs );

      const reco::Vertex* nCandVtx = usefulFuncs::get<reco::Vertex>( *nCand, "fitVertex" );
      myData.dataD[dataRecord::nlbtkVtxprob] = TMath::Prob( nCandVtx->chi2(), nCandVtx->ndof() );
      myData.dataD[dataRecord::nlbtkCosa2d] = usefulFuncs::getCosa2d(*nCand,&bs);

      myData.dataD[dataRecord::nlbtkMom]= nFourTk.Momentum();
      myData.dataD[dataRecord::nlbtkPt] = nFourTk.transverse();
      myData.dataD[dataRecord::ntktkPt] = nTkTk.transverse();
      myData.dataD[dataRecord::ntktkMom]= nTkTk.Momentum();
      myData.dataD[dataRecord::nptonPt]  = nTkPos.transverse();
      myData.dataD[dataRecord::nkaonPt]  = nTkNeg.transverse();
      myData.dataD[dataRecord::nptonMom] = nTkPos.Momentum();
      myData.dataD[dataRecord::nkaonMom] = nTkNeg.Momentum();
      myData.dataD[dataRecord::nptonIPt] = nCand->userFloat("TkTk/Proton.IPt");
      myData.dataD[dataRecord::nkaonIPt] = nCand->userFloat("TkTk/Kaon.IPt");
      myData.dataD[dataRecord::nptonIPtErr] = nCand->userFloat("TkTk/Proton.IPt.Error");
      myData.dataD[dataRecord::nkaonIPtErr] = nCand->userFloat("TkTk/Kaon.IPt.Error");
      if ( nCand->hasUserFloat("TkTk/Proton.dEdx.pixelHrm") )
          myData.dataD[dataRecord::nptonDEDX_pixelHrm] = nCand->userFloat( "TkTk/Proton.dEdx.pixelHrm" );
      if ( nCand->hasUserFloat(  "TkTk/Kaon.dEdx.pixelHrm") )
          myData.dataD[dataRecord::nkaonDEDX_pixelHrm] = nCand->userFloat( "TkTk/Kaon.dEdx.pixelHrm" );
      if ( nCand->hasUserFloat("TkTk/Proton.dEdx.Harmonic") )
          myData.dataD[dataRecord::nptonDEDX_Harmonic] = nCand->userFloat( "TkTk/Proton.dEdx.Harmonic" );
      if ( nCand->hasUserFloat(  "TkTk/Kaon.dEdx.Harmonic") )
          myData.dataD[dataRecord::nkaonDEDX_Harmonic] = nCand->userFloat(   "TkTk/Kaon.dEdx.Harmonic" );


      myData.thisTree()->Fill();
  }






//  ooooo
//
//
//
//
//
//        std::vector< std::pair< double, const pat::CompositeCandidate*> > candsSorted;
//        candsSorted.clear();
//        candsSorted.reserve( lbTkCands->size() );
//        std::vector<pat::CompositeCandidate>::const_iterator handleIter = lbTkCands->begin();
//        std::vector<pat::CompositeCandidate>::const_iterator handleIend = lbTkCands->end  ();
//        while( handleIter != handleIend )
//        {
//            const pat::CompositeCandidate& cand = *handleIter++;
//            if ( !cand.hasUserFloat("fitMass") ) continue;
//            if ( !cand.hasUserData("TkTk/Proton.fitMom") ) continue;
//            if ( !cand.hasUserData(  "TkTk/Kaon.fitMom") ) continue;
//            if ( !cand.hasUserFloat("TkTk/Proton.IPt") ) continue;
//            if ( !cand.hasUserFloat(  "TkTk/Kaon.IPt") ) continue;
//            if ( !cand.hasUserData( "fitVertex" ) ) continue;
//            if ( !cand.hasUserData( "refToJPsi" ) ) continue;
//
//            const GlobalVector* dPTR[2] = {nullptr};
//            dPTR[0] = cand.userData<GlobalVector>("TkTk/Proton.fitMom");
//            dPTR[1] = cand.userData<GlobalVector>("TkTk/Kaon.fitMom");
//
//            const GlobalVector* muPTR[2] = {nullptr};
//            muPTR[0] = cand.userData<GlobalVector>("JPsi/MuPos.fitMom");
//            muPTR[1] = cand.userData<GlobalVector>("JPsi/MuNeg.fitMom");
//
//            fourMom pMu ( muPTR[0]->x(), muPTR[0]->y(), muPTR[0]->z() );
//            fourMom nMu ( muPTR[1]->x(), muPTR[1]->y(), muPTR[1]->z() );
//            fourMom pTk ( dPTR[0]->x(), dPTR[0]->y(), dPTR[0]->z() );
//            fourMom nTk ( dPTR[1]->x(), dPTR[1]->y(), dPTR[1]->z() );
//            //fourMom tktk = pTk+nTk;
//            fourMom fourTk = pTk+nTk+pMu+nMu;
//            // preselection
//            if ( pMu.transverse() < 5.0 ) continue;
//            if ( nMu.transverse() < 5.0 ) continue;
//            if ( pMu.Momentum()   < 5.0 ) continue;
//            if ( nMu.Momentum()   < 5.0 ) continue;
//            if ( pTk.transverse() < 1.0 ) continue;
//            if ( nTk.transverse() < 1.0 ) continue;
//            if ( pTk.Momentum()   < 1.0 ) continue;
//            if ( nTk.Momentum()   < 1.0 ) continue;
//
//            if ( fourTk.transverse()< 12. ) continue;
//            if ( fourTk.Momentum()  < 12. ) continue;
//
//
//            const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
//            if ( _vtx == nullptr ) continue;
//            double fdSig = usefulFuncs::getFlightDistanceSignificance ( cand, &bs );
//            if ( fdSig < 3.0 ) continue;
//            double cos2d = usefulFuncs::getCosa2d( cand, &bs );
//            double vtxprob = TMath::Prob( _vtx->chi2(), _vtx->ndof() );
//            if ( cos2d < 0.99 ) continue;
//            if ( vtxprob < 0.15 ) continue;
//
//            candsSorted.emplace_back( vtxprob, &cand );
//        }
//        if ( candsSorted.size() == 0 ) return;
//
//        std::vector< std::pair<double, const pat::CompositeCandidate*> >::const_iterator iter = candsSorted.begin();
//        std::vector< std::pair<double, const pat::CompositeCandidate*> >::const_iterator iend = candsSorted.end  ();
//
//        while ( iter != iend )
//        {
//            myData.Clear();
//            const pat::CompositeCandidate& cand = *(iter++->second);
//
//            // first one is proton and second one is kaon ( consider bigger momentum with heavier particle )
//            const GlobalVector* dPTR[2] = {nullptr};
//            dPTR[0] = cand.userData<GlobalVector>("TkTk/Proton.fitMom");
//            dPTR[1] = cand.userData<GlobalVector>("TkTk/Kaon.fitMom");
//
//            const GlobalVector* muPTR[2] = {nullptr};
//            muPTR[0] = cand.userData<GlobalVector>("JPsi/MuPos.fitMom");
//            muPTR[1] = cand.userData<GlobalVector>("JPsi/MuNeg.fitMom");
//
//            fourMom pMu ( muPTR[0]->x(), muPTR[0]->y(), muPTR[0]->z() );
//            fourMom nMu ( muPTR[1]->x(), muPTR[1]->y(), muPTR[1]->z() );
//            pMu.setMass( 0.1056583745 );
//            nMu.setMass( 0.1056583745 );
//            fourMom pTk ( dPTR[0]->x(), dPTR[0]->y(), dPTR[0]->z() );
//            fourMom nTk ( dPTR[1]->x(), dPTR[1]->y(), dPTR[1]->z() );
//            fourMom tktk;
//            fourMom fourTk;
//
//            // search for pentaQuark
//            // jpsip = jpsi + proton, jpsiP = jpsi + anti-proton
//            fourMom jpsip, jpsipBar;
//            pTk.setMass( BPHParticleMasses::protonMass );
//            nTk.setMass( BPHParticleMasses::protonMass );
//            jpsip = pMu + nMu + pTk;
//            jpsipBar = pMu + nMu + nTk;
//
//            // search for lam0
//            pTk.setMass( BPHParticleMasses::protonMass );
//            nTk.setMass( BPHParticleMasses::  pionMass );
//            tktk = pTk+nTk;
//            fourTk = pMu + nMu + tktk;
//
//            myData.myData.dataD[dataRecord::dataRecord::fake_Lam0Mass] = tktk.Mag();
//            myData.myData.dataD[dataRecord::dataRecord::fake_LbL0Mass] = fourTk.Mag();
//
//            // search for k short
//            pTk.setMass( BPHParticleMasses::pionMass );
//            nTk.setMass( BPHParticleMasses::pionMass );
//            tktk = pTk+nTk;
//            fourTk = pMu + nMu + tktk;
//
//            myData.myData.dataD[dataRecord::dataRecord::fake_KshortMass] = tktk.Mag();
//            myData.myData.dataD[dataRecord::dataRecord::fake_mumupipiMass] = fourTk.Mag();
//
//            // search for k(892)
//            pTk.setMass( BPHParticleMasses::kaonMass );
//            nTk.setMass( BPHParticleMasses::pionMass );
//            tktk = pTk+nTk;
//            fourTk = pMu + nMu + tktk;
//
//            myData.myData.dataD[dataRecord::dataRecord::fake_KstarMass] = tktk.Mag();
//            myData.myData.dataD[dataRecord::dataRecord::fake_BdMass] = fourTk.Mag();
//
//            // search for phi(1020)
//            pTk.setMass( BPHParticleMasses::kaonMass );
//            nTk.setMass( BPHParticleMasses::kaonMass );
//            tktk = pTk+nTk;
//            fourTk = pMu + nMu + tktk;
//
//            myData.myData.dataD[dataRecord::dataRecord::fake_PhiMass] = tktk.Mag();
//            myData.myData.dataD[dataRecord::dataRecord::fake_BsMass] = fourTk.Mag();
//
//
//            const pat::CompositeCandidate* jpsi = usefulFuncs::getByRef<pat::CompositeCandidate>( cand, "refToJPsi" );
//            //const reco::Vertex* pv = usefulFuncs::get<reco::Vertex>( *jpsi, "fitVertex" );
//
//            myData.myData.dataD[dataRecord::dataRecord::lbtkMass] = cand.userFloat( "fitMass" );
//            myData.myData.dataD[dataRecord::dataRecord::lbtkFlightDistance2d] = usefulFuncs::getFlightDistance ( cand, &bs );
//            myData.myData.dataD[dataRecord::dataRecord::lbtkFlightDistanceSig]= usefulFuncs::getFlightDistanceSignificance ( cand, &bs );
//            const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
//            myData.myData.dataD[dataRecord::dataRecord::lbtkVtxprob] = TMath::Prob( _vtx->chi2(), _vtx->ndof() );
//            myData.myData.dataD[dataRecord::dataRecord::lbtkCosa2d] = usefulFuncs::getCosa2d(cand,&bs);
//
//            myData.myData.dataD[dataRecord::dataRecord::targetJpsiP_mass] = jpsip.Mag();
//            myData.myData.dataD[dataRecord::dataRecord::targetJpsiP_pt] = jpsip.transverse();
//            myData.myData.dataD[dataRecord::dataRecord::targetJpsiPBar_mass] = jpsipBar.Mag();
//            myData.myData.dataD[dataRecord::dataRecord::targetJpsiPBar_pt] = jpsipBar.transverse();
//
//            myData.myData.dataD[dataRecord::dataRecord::lbtkMom]= fourTk.Momentum();
//            myData.myData.dataD[dataRecord::dataRecord::lbtkPt] = fourTk.transverse();
//            myData.myData.dataD[dataRecord::dataRecord::tktkPt] = tktk.transverse();
//            myData.myData.dataD[dataRecord::dataRecord::tktkMom]= tktk.Momentum();
//            myData.myData.dataD[dataRecord::dataRecord::ptkPt]  = pTk.transverse();
//            myData.myData.dataD[dataRecord::dataRecord::ntkPt]  = nTk.transverse();
//            myData.myData.dataD[dataRecord::dataRecord::ptkMom] = pTk.Momentum();
//            myData.myData.dataD[dataRecord::dataRecord::ntkMom] = nTk.Momentum();
//            myData.myData.dataD[dataRecord::dataRecord::ptkIPt] = cand.userFloat("TkTk/Proton.IPt");
//            myData.myData.dataD[dataRecord::dataRecord::ntkIPt] = cand.userFloat("TkTk/Kaon.IPt");
//            myData.myData.dataD[dataRecord::dataRecord::ptkIPtErr] = cand.userFloat("TkTk/Proton.IPt.Error");
//            myData.myData.dataD[dataRecord::dataRecord::ntkIPtErr] = cand.userFloat("TkTk/Kaon.IPt.Error");
//            if ( cand.hasUserFloat("TkTk/Proton.dEdx.pixelHrm") )
//                myData.myData.dataD[dataRecord::dataRecord::ptkDEDX_pixelHrm] = cand.userFloat( "TkTk/Proton.dEdx.pixelHrm" );
//            if ( cand.hasUserFloat(  "TkTk/Kaon.dEdx.pixelHrm") )
//                myData.myData.dataD[dataRecord::dataRecord::ntkDEDX_pixelHrm] = cand.userFloat( "TkTk/Kaon.dEdx.pixelHrm" );
//            if ( cand.hasUserFloat("TkTk/Proton.dEdx.Harmonic") )
//                myData.myData.dataD[dataRecord::dataRecord::ptkDEDX_Harmonic] = cand.userFloat( "TkTk/Proton.dEdx.Harmonic" );
//            if ( cand.hasUserFloat(  "TkTk/Kaon.dEdx.Harmonic") )
//                myData.myData.dataD[dataRecord::dataRecord::ntkDEDX_Harmonic] = cand.userFloat(   "TkTk/Kaon.dEdx.Harmonic" );
//            myData.thisTree()->Fill();
//        }
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
std::vector< std::pair< std::pair<double,const pat::CompositeCandidate*>, std::pair<double,const pat::CompositeCandidate*> > > treeCreatingSpecificDecay::pairpnCands
( std::vector<std::pair<double,const pat::CompositeCandidate*> >& pCands, std::vector<std::pair<double, const pat::CompositeCandidate*> >& nCands )
{
    unsigned NpCands = pCands.size();
    unsigned NnCands = nCands.size();

    std::vector< std::pair< std::pair<double,const pat::CompositeCandidate*>, std::pair<double,const pat::CompositeCandidate*> > > pairpnCandList;
    pairpnCandList.reserve( NpCands );
    std::vector< unsigned > idxPairHistory;
    idxPairHistory.reserve( NpCands );
    for ( unsigned i = 0; i < NpCands; ++i )
    {
        const pat::CompositeCandidate* pcompcand = pCands[i].second;

        //double logDeltaR = 999.;
        unsigned reserveJ = 999;
        

        for ( unsigned j = 0; j < NnCands; ++j )
        {
            const pat::CompositeCandidate* ncompcand = nCands[j].second;
            if ( theSameCompCand(pcompcand, ncompcand) )
                reserveJ = j;
        }
        if ( reserveJ == 999 ) continue;

        // check whether it is repeated to find the same index
        for ( const auto idx : idxPairHistory )
            if ( reserveJ == idx )
                printf( "\nWarning!!! in pairpnCands function: there are j idx repeating paired: (i,j) = (%d,%d)\n", i,reserveJ );

        pairpnCandList.push_back( std::make_pair(pCands[i], nCands[reserveJ]) );
        idxPairHistory.push_back( reserveJ );
    }
    return pairpnCandList;
}
bool treeCreatingSpecificDecay::theSameCompCand( const pat::CompositeCandidate* cand1, const pat::CompositeCandidate* cand2 )
{
    if ( cand1->numberOfDaughters() != cand2->numberOfDaughters() ) return false;
    const reco::Candidate* dau10 = cand1->daughter(0);
    const reco::Candidate* dau11 = cand1->daughter(1);
    const reco::Candidate* dau12 = cand1->daughter(2);
    const reco::Candidate* dau13 = cand1->daughter(3);
    const reco::Candidate* dau20 = cand2->daughter(0);
    const reco::Candidate* dau21 = cand2->daughter(1);
    const reco::Candidate* dau22 = cand2->daughter(2);
    const reco::Candidate* dau23 = cand2->daughter(3);
    bool dR00 = theSameDaugs( dau10, dau20, 0.00001 );
    bool dR01 = theSameDaugs( dau10, dau21, 0.00001 );
    bool dR10 = theSameDaugs( dau11, dau20, 0.00001 );
    bool dR11 = theSameDaugs( dau11, dau21, 0.00001 );
    bool dR22 = theSameDaugs( dau12, dau22, 0.00001 );
    bool dR23 = theSameDaugs( dau12, dau23, 0.00001 );
    bool dR32 = theSameDaugs( dau13, dau22, 0.00001 );
    bool dR33 = theSameDaugs( dau13, dau23, 0.00001 );

    short result = 2;
    if ( (dR00 && dR11) || (dR10&&dR01) ) --result;
    if ( (dR22 && dR33) || (dR23&&dR32) ) --result;
    return result == 0 ? true : false;
}

#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE( treeCreatingSpecificDecay );
