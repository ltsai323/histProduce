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


treeCreatingSpecificDecay::treeCreatingSpecificDecay( const edm::ParameterSet& ps ) 
{

  useOnia = ( SET_LABEL( oniaCandsLabel, ps ) != "" );
  usepTks = ( SET_LABEL( pTksCandsLabel, ps ) != "" );
  usepL0B = ( SET_LABEL( pL0BCandsLabel, ps ) != "" );
  usenTks = ( SET_LABEL( nTksCandsLabel, ps ) != "" );
  usenL0B = ( SET_LABEL( nL0BCandsLabel, ps ) != "" );
  useLam0 = ( SET_LABEL( Lam0CandsLabel, ps ) != "" );
  useLbL0 = ( SET_LABEL( LbL0CandsLabel, ps ) != "" );
  useLamo = ( SET_LABEL( LamoCandsLabel, ps ) != "" );
  useLbLo = ( SET_LABEL( LbLoCandsLabel, ps ) != "" );
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
  if ( useLam0 ) consume< vector<pat::CompositeCandidate> >( Lam0CandsToken,
                                                             Lam0CandsLabel );
  if ( useLbL0 ) consume< vector<pat::CompositeCandidate> >( LbL0CandsToken,
                                                             LbL0CandsLabel );
  if ( useLamo ) consume< vector<pat::CompositeCandidate> >( LamoCandsToken,
                                                             LamoCandsLabel );
  if ( useLbLo ) consume< vector<pat::CompositeCandidate> >( LbLoCandsToken,
                                                             LbLoCandsLabel );
  if ( useBS )   consume< reco::BeamSpot                  >( bsPointToken,
                                                             bsPointLabel );

    pL0BTree = fs->make<TTree>( "pLbTk", "pLbTk" );
    nL0BTree = fs->make<TTree>( "nLbTk", "nLbTk" );
    LbL0Tree = fs->make<TTree>( "pLbL0", "pLbL0" );
    LbLoTree = fs->make<TTree>( "nLbL0", "nLbL0" );
    pL0B.RegFormatTree( pL0BTree );
    nL0B.RegFormatTree( nL0BTree );
    LbL0.RegFormatTree( LbL0Tree );
    LbLo.RegFormatTree( LbLoTree );
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
   desc.add<string>( "Lam0CandsLabel", "" );
   desc.add<string>( "LbL0CandsLabel", "" );
   desc.add<string>( "LamoCandsLabel", "" );
   desc.add<string>( "LbLoCandsLabel", "" );
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

  if ( usepL0B ) // Lb->JPsi p K {{{
  {
    edm::Handle< vector<pat::CompositeCandidate> > pL0BCands;
    pL0BCandsToken.get( ev, pL0BCands );
    std::vector<pat::CompositeCandidate>::const_iterator handleIter;
    std::vector<pat::CompositeCandidate>::const_iterator handleIend;
    // preselection {{{
    if ( !pL0BCands.isValid() ) return;
    if ( pL0BCands->size() == 0 ) return;
  
    std::vector< std::pair< double, const pat::CompositeCandidate*> > selectedCandList;
    selectedCandList.clear();
    selectedCandList.reserve( pL0BCands->size() );
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
        if ( pMu.transverse() < 4.0 ) continue;
        if ( nMu.transverse() < 4.0 ) continue;
        if ( pMu.Momentum()   < 4.0 ) continue;
        if ( nMu.Momentum()   < 4.0 ) continue;
        if ( pTk.transverse() < 0.8 ) continue;
        //if ( nTk.transverse() < 1.0 ) continue;
        if ( pTk.Momentum()   < 0.8 ) continue;
        //if ( nTk.Momentum()   < 1.0 ) continue;
  
        if ( fourTk.transverse()< 10. ) continue;
        if ( fourTk.Momentum()  < 10. ) continue;
  
  
        const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
        if ( _vtx == nullptr ) continue;
        double fdSig = usefulFuncs::getFlightDistanceSignificance ( cand, &bs );
        if ( fdSig < 3.0 ) continue;
        double cos2d = usefulFuncs::getCosa2d( cand, &bs );
        double vtxprob = TMath::Prob( _vtx->chi2(), _vtx->ndof() );
        if ( cos2d < 0.95 ) continue;
        if ( vtxprob < 0.03 ) continue;
  
        selectedCandList.emplace_back( vtxprob, &cand );
    }
    // preselection end }}}
  
    unsigned N = selectedCandList.size();
    for ( unsigned i = 0; i < N; ++i )
    {
        pL0B.Clear();
        const double candVtxprob = selectedCandList[i].first;
        const pat::CompositeCandidate& selCand = *(selectedCandList[i].second);
        const pat::CompositeCandidate& tktkCand = *(usefulFuncs::getByRef<pat::CompositeCandidate>( selCand, "refToTkTk" ));
        //const pat::CompositeCandidate& jpsiCand = *(usefulFuncs::getByRef<pat::CompositeCandidate>( selCand, "refToJPsi" ));
  
        const GlobalVector* dPTR[2] = {nullptr};
        dPTR[0] = selCand.userData<GlobalVector>("TkTk/Proton.fitMom");
        dPTR[1] = selCand.userData<GlobalVector>("TkTk/Kaon.fitMom");
  
        const GlobalVector* muPTR[2] = {nullptr};
        muPTR[0] = selCand.userData<GlobalVector>("JPsi/MuPos.fitMom");
        muPTR[1] = selCand.userData<GlobalVector>("JPsi/MuNeg.fitMom");
        const GlobalVector* fourTk = selCand.userData<GlobalVector>("fitMomentum");
        TLorentzVector fourTkMom( fourTk->x(), fourTk->y(), fourTk->z(), sqrt(fourTk->x()*fourTk->x()+fourTk->y()*fourTk->y()+fourTk->z()*fourTk->z()+selCand.userFloat("fitMass")*selCand.userFloat("fitMass")) );
        const GlobalVector* twoTk  = tktkCand.userData<GlobalVector>("fitMomentum");
        TLorentzVector twoTkMom( twoTk->x(), twoTk->y(), twoTk->z(), sqrt(twoTk->x()*twoTk->x()+twoTk->y()*twoTk->y()+twoTk->z()*twoTk->z()+tktkCand.userFloat("fitMass")*tktkCand.userFloat("fitMass")) );
        
        pL0B.dataD[LbTkRecord::lbtkMass] = selCand.userFloat( "fitMass" );
        pL0B.dataD[LbTkRecord::lbtkPt] = fourTkMom.Pt();
        pL0B.dataD[LbTkRecord::lbtkEta] = fourTkMom.Eta();
        pL0B.dataD[LbTkRecord::lbtkY] = fourTkMom.Rapidity();
        pL0B.dataD[LbTkRecord::lbtkPhi] = fourTk->phi();
        pL0B.dataD[LbTkRecord::lbtkFlightDistance2d] = usefulFuncs::getFlightDistance ( selCand, &bs );
        pL0B.dataD[LbTkRecord::lbtkFlightDistanceSig]= usefulFuncs::getFlightDistanceSignificance ( selCand, &bs );
        const reco::Vertex* candVtx = usefulFuncs::get<reco::Vertex>( selCand, "fitVertex" );
        pL0B.dataD[LbTkRecord::lbtkCosa2d] = usefulFuncs::getCosa2d(selCand,&bs);
        pL0B.dataD[LbTkRecord::lbtkVtxprob] = TMath::Prob( candVtx->chi2(), candVtx->ndof() );
        pL0B.dataD[LbTkRecord::lbtknChi2] = candVtx->chi2() / candVtx->ndof();

        pL0B.dataD[LbTkRecord::tktkMass] = tktkCand.userFloat( "fitMass" );
        pL0B.dataD[LbTkRecord::tktkPt] = twoTkMom.Pt();
        pL0B.dataD[LbTkRecord::tktkEta] = twoTkMom.Eta();
        pL0B.dataD[LbTkRecord::tktkY] = twoTkMom.Rapidity();
        pL0B.dataD[LbTkRecord::tktkPhi] = twoTk->phi();
        pL0B.dataD[LbTkRecord::tktkFlightDistance2d] = usefulFuncs::getFlightDistance ( tktkCand, &bs );
        pL0B.dataD[LbTkRecord::tktkFlightDistanceSig]= usefulFuncs::getFlightDistanceSignificance ( tktkCand, &bs );
        const reco::Vertex* tktkCandVtx = usefulFuncs::get<reco::Vertex>( tktkCand, "fitVertex" );
        pL0B.dataD[LbTkRecord::tktkCosa2d] = usefulFuncs::getCosa2d(tktkCand,&bs);
        pL0B.dataD[LbTkRecord::tktkVtxprob] = TMath::Prob( tktkCandVtx->chi2(), tktkCandVtx->ndof() );
        pL0B.dataD[LbTkRecord::tktknChi2] = tktkCandVtx->chi2() / tktkCandVtx->ndof();
  
        pL0B.dataD[LbTkRecord::pmuPt] = muPTR[0]->perp();
        pL0B.dataD[LbTkRecord::pmuP0] = sqrt(muPTR[0]->mag2()+0.105658*0.105658);
        pL0B.dataD[LbTkRecord::pmuP1] = muPTR[0]->x();
        pL0B.dataD[LbTkRecord::pmuP2] = muPTR[0]->y();
        pL0B.dataD[LbTkRecord::pmuP3] = muPTR[0]->z();
        pL0B.dataD[LbTkRecord::nmuPt] = muPTR[1]->perp();
        pL0B.dataD[LbTkRecord::nmuP0] = sqrt(muPTR[1]->mag2()+0.105658*0.105658);
        pL0B.dataD[LbTkRecord::nmuP1] = muPTR[1]->x();
        pL0B.dataD[LbTkRecord::nmuP2] = muPTR[1]->y();
        pL0B.dataD[LbTkRecord::nmuP3] = muPTR[1]->z();

        pL0B.dataD[LbTkRecord::tk1Pt] = dPTR[0]->perp();
        pL0B.dataD[LbTkRecord::tk1P0] = sqrt(dPTR[0]->mag2()+0.105658*0.105658);
        pL0B.dataD[LbTkRecord::tk1P1] = dPTR[0]->x();
        pL0B.dataD[LbTkRecord::tk1P2] = dPTR[0]->y();
        pL0B.dataD[LbTkRecord::tk1P3] = dPTR[0]->z();
        pL0B.dataD[LbTkRecord::tk2Pt] = dPTR[1]->perp();
        pL0B.dataD[LbTkRecord::tk2P0] = sqrt(dPTR[1]->mag2()+0.105658*0.105658);
        pL0B.dataD[LbTkRecord::tk2P1] = dPTR[1]->x();
        pL0B.dataD[LbTkRecord::tk2P2] = dPTR[1]->y();
        pL0B.dataD[LbTkRecord::tk2P3] = dPTR[1]->z();

        pL0B.dataD[LbTkRecord::tk1IPt] = selCand.userFloat("TkTk/Proton.IPt");
        pL0B.dataD[LbTkRecord::tk2IPt] = selCand.userFloat("TkTk/Kaon.IPt");
        pL0B.dataD[LbTkRecord::tk1IPtErr] = selCand.userFloat("TkTk/Proton.IPt.Error");
        pL0B.dataD[LbTkRecord::tk2IPtErr] = selCand.userFloat("TkTk/Kaon.IPt.Error");
        if ( selCand.hasUserFloat("TkTk/Proton.dEdx.pixelHrm") )
            pL0B.dataD[LbTkRecord::tk1DEDX_pixelHrm] = selCand.userFloat( "TkTk/Proton.dEdx.pixelHrm" );
        if ( selCand.hasUserFloat(  "TkTk/Kaon.dEdx.pixelHrm") )
            pL0B.dataD[LbTkRecord::tk2DEDX_pixelHrm] = selCand.userFloat( "TkTk/Kaon.dEdx.pixelHrm" );
        if ( selCand.hasUserFloat("TkTk/Proton.dEdx.Harmonic") )
            pL0B.dataD[LbTkRecord::tk1DEDX_Harmonic] = selCand.userFloat( "TkTk/Proton.dEdx.Harmonic" );
        if ( selCand.hasUserFloat(  "TkTk/Kaon.dEdx.Harmonic") )
            pL0B.dataD[LbTkRecord::tk2DEDX_Harmonic] = selCand.userFloat(   "TkTk/Kaon.dEdx.Harmonic" );
  
  
  
        pL0BTree->Fill();
    }
  } // Lb->Jpsi p K end }}}
  if ( usenL0B ) // Lb->JPsi P k {{{
  {
    edm::Handle< vector<pat::CompositeCandidate> > nL0BCands;
    nL0BCandsToken.get( ev, nL0BCands );
    std::vector<pat::CompositeCandidate>::const_iterator handleIter;
    std::vector<pat::CompositeCandidate>::const_iterator handleIend;
    // preselection {{{
    if ( !nL0BCands.isValid() ) return;
    if ( nL0BCands->size() == 0 ) return;
  
    std::vector< std::pair< double, const pat::CompositeCandidate*> > selectedCandList;
    selectedCandList.clear();
    selectedCandList.reserve( nL0BCands->size() );
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
        if ( pMu.transverse() < 4.0 ) continue;
        if ( nMu.transverse() < 4.0 ) continue;
        if ( pMu.Momentum()   < 4.0 ) continue;
        if ( nMu.Momentum()   < 4.0 ) continue;
        if ( pTk.transverse() < 0.8 ) continue;
        //if ( nTk.transverse() < 1.0 ) continue;
        if ( pTk.Momentum()   < 0.8 ) continue;
        //if ( nTk.Momentum()   < 1.0 ) continue;
  
        if ( fourTk.transverse()< 10. ) continue;
        if ( fourTk.Momentum()  < 10. ) continue;
  
  
        const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
        if ( _vtx == nullptr ) continue;
        double fdSig = usefulFuncs::getFlightDistanceSignificance ( cand, &bs );
        if ( fdSig < 3.0 ) continue;
        double cos2d = usefulFuncs::getCosa2d( cand, &bs );
        double vtxprob = TMath::Prob( _vtx->chi2(), _vtx->ndof() );
        if ( cos2d < 0.95 ) continue;
        if ( vtxprob < 0.03 ) continue;
  
        selectedCandList.emplace_back( vtxprob, &cand );
    }
    // preselection end }}}
  
    unsigned N = selectedCandList.size();
    for ( unsigned i = 0; i < N; ++i )
    {
        nL0B.Clear();
        const double candVtxprob = selectedCandList[i].first;
        const pat::CompositeCandidate& selCand = *(selectedCandList[i].second);
        const pat::CompositeCandidate& tktkCand = *(usefulFuncs::getByRef<pat::CompositeCandidate>( selCand, "refToTkTk" ));
        //const pat::CompositeCandidate& jpsiCand = *(usefulFuncs::getByRef<pat::CompositeCandidate>( selCand, "refToJPsi" ));
  
        const GlobalVector* dPTR[2] = {nullptr};
        dPTR[0] = selCand.userData<GlobalVector>("TkTk/Proton.fitMom");
        dPTR[1] = selCand.userData<GlobalVector>("TkTk/Kaon.fitMom");
  
        const GlobalVector* muPTR[2] = {nullptr};
        muPTR[0] = selCand.userData<GlobalVector>("JPsi/MuPos.fitMom");
        muPTR[1] = selCand.userData<GlobalVector>("JPsi/MuNeg.fitMom");
        const GlobalVector* fourTk = selCand.userData<GlobalVector>("fitMomentum");
        TLorentzVector fourTkMom( fourTk->x(), fourTk->y(), fourTk->z(), sqrt(fourTk->x()*fourTk->x()+fourTk->y()*fourTk->y()+fourTk->z()*fourTk->z()+selCand.userFloat("fitMass")*selCand.userFloat("fitMass")) );
        const GlobalVector* twoTk  = tktkCand.userData<GlobalVector>("fitMomentum");
        TLorentzVector twoTkMom( twoTk->x(), twoTk->y(), twoTk->z(), sqrt(twoTk->x()*twoTk->x()+twoTk->y()*twoTk->y()+twoTk->z()*twoTk->z()+tktkCand.userFloat("fitMass")*tktkCand.userFloat("fitMass")) );
        
        nL0B.dataD[LbTkRecord::lbtkMass] = selCand.userFloat( "fitMass" );
        nL0B.dataD[LbTkRecord::lbtkPt] = fourTkMom.Pt();
        nL0B.dataD[LbTkRecord::lbtkEta] = fourTkMom.Eta();
        nL0B.dataD[LbTkRecord::lbtkY] = fourTkMom.Rapidity();
        nL0B.dataD[LbTkRecord::lbtkPhi] = fourTk->phi();
        nL0B.dataD[LbTkRecord::lbtkFlightDistance2d] = usefulFuncs::getFlightDistance ( selCand, &bs );
        nL0B.dataD[LbTkRecord::lbtkFlightDistanceSig]= usefulFuncs::getFlightDistanceSignificance ( selCand, &bs );
        const reco::Vertex* candVtx = usefulFuncs::get<reco::Vertex>( selCand, "fitVertex" );
        nL0B.dataD[LbTkRecord::lbtkCosa2d] = usefulFuncs::getCosa2d(selCand,&bs);
        nL0B.dataD[LbTkRecord::lbtkVtxprob] = TMath::Prob( candVtx->chi2(), candVtx->ndof() );
        nL0B.dataD[LbTkRecord::lbtknChi2] = candVtx->chi2() / candVtx->ndof();

        nL0B.dataD[LbTkRecord::tktkMass] = tktkCand.userFloat( "fitMass" );
        nL0B.dataD[LbTkRecord::tktkPt] = twoTkMom.Pt();
        nL0B.dataD[LbTkRecord::tktkEta] = twoTkMom.Eta();
        nL0B.dataD[LbTkRecord::tktkY] = twoTkMom.Rapidity();
        nL0B.dataD[LbTkRecord::tktkPhi] = twoTk->phi();
        nL0B.dataD[LbTkRecord::tktkFlightDistance2d] = usefulFuncs::getFlightDistance ( tktkCand, &bs );
        nL0B.dataD[LbTkRecord::tktkFlightDistanceSig]= usefulFuncs::getFlightDistanceSignificance ( tktkCand, &bs );
        const reco::Vertex* tktkCandVtx = usefulFuncs::get<reco::Vertex>( tktkCand, "fitVertex" );
        nL0B.dataD[LbTkRecord::tktkCosa2d] = usefulFuncs::getCosa2d(tktkCand,&bs);
        nL0B.dataD[LbTkRecord::tktkVtxprob] = TMath::Prob( tktkCandVtx->chi2(), tktkCandVtx->ndof() );
        nL0B.dataD[LbTkRecord::tktknChi2] = tktkCandVtx->chi2() / tktkCandVtx->ndof();
  
        nL0B.dataD[LbTkRecord::pmuPt] = muPTR[0]->perp();
        nL0B.dataD[LbTkRecord::pmuP0] = sqrt(muPTR[0]->mag2()+0.105658*0.105658);
        nL0B.dataD[LbTkRecord::pmuP1] = muPTR[0]->x();
        nL0B.dataD[LbTkRecord::pmuP2] = muPTR[0]->y();
        nL0B.dataD[LbTkRecord::pmuP3] = muPTR[0]->z();
        nL0B.dataD[LbTkRecord::nmuPt] = muPTR[1]->perp();
        nL0B.dataD[LbTkRecord::nmuP0] = sqrt(muPTR[1]->mag2()+0.105658*0.105658);
        nL0B.dataD[LbTkRecord::nmuP1] = muPTR[1]->x();
        nL0B.dataD[LbTkRecord::nmuP2] = muPTR[1]->y();
        nL0B.dataD[LbTkRecord::nmuP3] = muPTR[1]->z();

        nL0B.dataD[LbTkRecord::tk1Pt] = dPTR[0]->perp();
        nL0B.dataD[LbTkRecord::tk1P0] = sqrt(dPTR[0]->mag2()+0.105658*0.105658);
        nL0B.dataD[LbTkRecord::tk1P1] = dPTR[0]->x();
        nL0B.dataD[LbTkRecord::tk1P2] = dPTR[0]->y();
        nL0B.dataD[LbTkRecord::tk1P3] = dPTR[0]->z();
        nL0B.dataD[LbTkRecord::tk2Pt] = dPTR[1]->perp();
        nL0B.dataD[LbTkRecord::tk2P0] = sqrt(dPTR[1]->mag2()+0.105658*0.105658);
        nL0B.dataD[LbTkRecord::tk2P1] = dPTR[1]->x();
        nL0B.dataD[LbTkRecord::tk2P2] = dPTR[1]->y();
        nL0B.dataD[LbTkRecord::tk2P3] = dPTR[1]->z();

        nL0B.dataD[LbTkRecord::tk1IPt] = selCand.userFloat("TkTk/Proton.IPt");
        nL0B.dataD[LbTkRecord::tk2IPt] = selCand.userFloat("TkTk/Kaon.IPt");
        nL0B.dataD[LbTkRecord::tk1IPtErr] = selCand.userFloat("TkTk/Proton.IPt.Error");
        nL0B.dataD[LbTkRecord::tk2IPtErr] = selCand.userFloat("TkTk/Kaon.IPt.Error");
        if ( selCand.hasUserFloat("TkTk/Proton.dEdx.pixelHrm") )
            nL0B.dataD[LbTkRecord::tk1DEDX_pixelHrm] = selCand.userFloat( "TkTk/Proton.dEdx.pixelHrm" );
        if ( selCand.hasUserFloat(  "TkTk/Kaon.dEdx.pixelHrm") )
            nL0B.dataD[LbTkRecord::tk2DEDX_pixelHrm] = selCand.userFloat( "TkTk/Kaon.dEdx.pixelHrm" );
        if ( selCand.hasUserFloat("TkTk/Proton.dEdx.Harmonic") )
            nL0B.dataD[LbTkRecord::tk1DEDX_Harmonic] = selCand.userFloat( "TkTk/Proton.dEdx.Harmonic" );
        if ( selCand.hasUserFloat(  "TkTk/Kaon.dEdx.Harmonic") )
            nL0B.dataD[LbTkRecord::tk2DEDX_Harmonic] = selCand.userFloat(   "TkTk/Kaon.dEdx.Harmonic" );
  
  
  
        nL0BTree->Fill();
    }
  } // Lb->Jpsi P k end }}}


  //////////// LbL0 ////////////
  if ( useLbL0 ) // Lb->JPsi Lam0 {{{
  {
    edm::Handle< vector<pat::CompositeCandidate> > LbL0Cands;
    LbL0CandsToken.get( ev, LbL0Cands );
    std::vector<pat::CompositeCandidate>::const_iterator handleIter;
    std::vector<pat::CompositeCandidate>::const_iterator handleIend;
    // preselection {{{
    if ( !LbL0Cands.isValid() ) return;
    if ( LbL0Cands->size() == 0 ) return;
  
    std::vector< std::pair< double, const pat::CompositeCandidate*> > selectedCandList;
    selectedCandList.clear();
    selectedCandList.reserve( LbL0Cands->size() );
    handleIter = LbL0Cands->begin();
    handleIend = LbL0Cands->end  ();
    while( handleIter != handleIend )
    {
        const pat::CompositeCandidate& cand = *handleIter++;
        if ( !cand.hasUserFloat("fitMass") ) continue;
        if ( !cand.hasUserData("TkTk/Proton.fitMom") ) continue;
        if ( !cand.hasUserData(  "TkTk/Pion.fitMom") ) continue;
        if ( !cand.hasUserFloat("TkTk/Proton.IPt") ) continue;
        if ( !cand.hasUserFloat(  "TkTk/Pion.IPt") ) continue;
        if ( !cand.hasUserData( "fitVertex" ) ) continue;
        if ( !cand.hasUserData( "refToJPsi" ) ) continue;
  
        const GlobalVector* dPTR[2] = {nullptr};
        dPTR[0] = cand.userData<GlobalVector>("TkTk/Proton.fitMom");
        dPTR[1] = cand.userData<GlobalVector>("TkTk/Pion.fitMom");
  
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
        if ( pMu.transverse() < 4.0 ) continue;
        if ( nMu.transverse() < 4.0 ) continue;
        if ( pMu.Momentum()   < 4.0 ) continue;
        if ( nMu.Momentum()   < 4.0 ) continue;
        if ( pTk.transverse() < 0.8 ) continue;
        //if ( nTk.transverse() < 1.0 ) continue;
        if ( pTk.Momentum()   < 0.8 ) continue;
        //if ( nTk.Momentum()   < 1.0 ) continue;
  
        if ( fourTk.transverse()< 10. ) continue;
        if ( fourTk.Momentum()  < 10. ) continue;
  
  
        const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
        if ( _vtx == nullptr ) continue;
        double fdSig = usefulFuncs::getFlightDistanceSignificance ( cand, &bs );
        if ( fdSig < 3.0 ) continue;
        double cos2d = usefulFuncs::getCosa2d( cand, &bs );
        double vtxprob = TMath::Prob( _vtx->chi2(), _vtx->ndof() );
        if ( cos2d < 0.95 ) continue;
        if ( vtxprob < 0.03 ) continue;
  
        selectedCandList.emplace_back( vtxprob, &cand );
    }
    // preselection end }}}
  
    unsigned N = selectedCandList.size();
    for ( unsigned i = 0; i < N; ++i )
    {
        LbL0.Clear();
        const double candVtxprob = selectedCandList[i].first;
        const pat::CompositeCandidate& selCand = *(selectedCandList[i].second);
        const pat::CompositeCandidate& tktkCand = *(usefulFuncs::getByRef<pat::CompositeCandidate>( selCand, "refToTkTk" ));
        //const pat::CompositeCandidate& jpsiCand = *(usefulFuncs::getByRef<pat::CompositeCandidate>( selCand, "refToJPsi" ));
  
        const GlobalVector* dPTR[2] = {nullptr};
        dPTR[0] = selCand.userData<GlobalVector>("TkTk/Proton.fitMom");
        dPTR[1] = selCand.userData<GlobalVector>("TkTk/Pion.fitMom");
  
        const GlobalVector* muPTR[2] = {nullptr};
        muPTR[0] = selCand.userData<GlobalVector>("JPsi/MuPos.fitMom");
        muPTR[1] = selCand.userData<GlobalVector>("JPsi/MuNeg.fitMom");
        const GlobalVector* fourTk = selCand.userData<GlobalVector>("fitMomentum");
        TLorentzVector fourTkMom( fourTk->x(), fourTk->y(), fourTk->z(), sqrt(fourTk->x()*fourTk->x()+fourTk->y()*fourTk->y()+fourTk->z()*fourTk->z()+selCand.userFloat("fitMass")*selCand.userFloat("fitMass")) );
        const GlobalVector* twoTk  = tktkCand.userData<GlobalVector>("fitMomentum");
        TLorentzVector twoTkMom( twoTk->x(), twoTk->y(), twoTk->z(), sqrt(twoTk->x()*twoTk->x()+twoTk->y()*twoTk->y()+twoTk->z()*twoTk->z()+tktkCand.userFloat("fitMass")*tktkCand.userFloat("fitMass")) );
        
        LbL0.dataD[LbTkRecord::lbtkMass] = selCand.userFloat( "fitMass" );
        LbL0.dataD[LbTkRecord::lbtkPt] = fourTkMom.Pt();
        LbL0.dataD[LbTkRecord::lbtkEta] = fourTkMom.Eta();
        LbL0.dataD[LbTkRecord::lbtkY] = fourTkMom.Rapidity();
        LbL0.dataD[LbTkRecord::lbtkPhi] = fourTk->phi();
        LbL0.dataD[LbTkRecord::lbtkFlightDistance2d] = usefulFuncs::getFlightDistance ( selCand, &bs );
        LbL0.dataD[LbTkRecord::lbtkFlightDistanceSig]= usefulFuncs::getFlightDistanceSignificance ( selCand, &bs );
        const reco::Vertex* candVtx = usefulFuncs::get<reco::Vertex>( selCand, "fitVertex" );
        LbL0.dataD[LbTkRecord::lbtkCosa2d] = usefulFuncs::getCosa2d(selCand,&bs);
        LbL0.dataD[LbTkRecord::lbtkVtxprob] = TMath::Prob( candVtx->chi2(), candVtx->ndof() );
        LbL0.dataD[LbTkRecord::lbtknChi2] = candVtx->chi2() / candVtx->ndof();

        LbL0.dataD[LbTkRecord::tktkMass] = tktkCand.userFloat( "fitMass" );
        LbL0.dataD[LbTkRecord::tktkPt] = twoTkMom.Pt();
        LbL0.dataD[LbTkRecord::tktkEta] = twoTkMom.Eta();
        LbL0.dataD[LbTkRecord::tktkY] = twoTkMom.Rapidity();
        LbL0.dataD[LbTkRecord::tktkPhi] = twoTk->phi();
        LbL0.dataD[LbTkRecord::tktkFlightDistance2d] = usefulFuncs::getFlightDistance ( tktkCand, &bs );
        LbL0.dataD[LbTkRecord::tktkFlightDistanceSig]= usefulFuncs::getFlightDistanceSignificance ( tktkCand, &bs );
        const reco::Vertex* tktkCandVtx = usefulFuncs::get<reco::Vertex>( tktkCand, "fitVertex" );
        LbL0.dataD[LbTkRecord::tktkCosa2d] = usefulFuncs::getCosa2d(tktkCand,&bs);
        LbL0.dataD[LbTkRecord::tktkVtxprob] = TMath::Prob( tktkCandVtx->chi2(), tktkCandVtx->ndof() );
        LbL0.dataD[LbTkRecord::tktknChi2] = tktkCandVtx->chi2() / tktkCandVtx->ndof();
  
        LbL0.dataD[LbTkRecord::pmuPt] = muPTR[0]->perp();
        LbL0.dataD[LbTkRecord::pmuP0] = sqrt(muPTR[0]->mag2()+0.105658*0.105658);
        LbL0.dataD[LbTkRecord::pmuP1] = muPTR[0]->x();
        LbL0.dataD[LbTkRecord::pmuP2] = muPTR[0]->y();
        LbL0.dataD[LbTkRecord::pmuP3] = muPTR[0]->z();
        LbL0.dataD[LbTkRecord::nmuPt] = muPTR[1]->perp();
        LbL0.dataD[LbTkRecord::nmuP0] = sqrt(muPTR[1]->mag2()+0.105658*0.105658);
        LbL0.dataD[LbTkRecord::nmuP1] = muPTR[1]->x();
        LbL0.dataD[LbTkRecord::nmuP2] = muPTR[1]->y();
        LbL0.dataD[LbTkRecord::nmuP3] = muPTR[1]->z();

        LbL0.dataD[LbTkRecord::tk1Pt] = dPTR[0]->perp();
        LbL0.dataD[LbTkRecord::tk1P0] = sqrt(dPTR[0]->mag2()+0.105658*0.105658);
        LbL0.dataD[LbTkRecord::tk1P1] = dPTR[0]->x();
        LbL0.dataD[LbTkRecord::tk1P2] = dPTR[0]->y();
        LbL0.dataD[LbTkRecord::tk1P3] = dPTR[0]->z();
        LbL0.dataD[LbTkRecord::tk2Pt] = dPTR[1]->perp();
        LbL0.dataD[LbTkRecord::tk2P0] = sqrt(dPTR[1]->mag2()+0.105658*0.105658);
        LbL0.dataD[LbTkRecord::tk2P1] = dPTR[1]->x();
        LbL0.dataD[LbTkRecord::tk2P2] = dPTR[1]->y();
        LbL0.dataD[LbTkRecord::tk2P3] = dPTR[1]->z();

        LbL0.dataD[LbTkRecord::tk1IPt] = selCand.userFloat("TkTk/Proton.IPt");
        LbL0.dataD[LbTkRecord::tk2IPt] = selCand.userFloat("TkTk/Pion.IPt");
        LbL0.dataD[LbTkRecord::tk1IPtErr] = selCand.userFloat("TkTk/Proton.IPt.Error");
        LbL0.dataD[LbTkRecord::tk2IPtErr] = selCand.userFloat("TkTk/Pion.IPt.Error");
        if ( selCand.hasUserFloat("TkTk/Proton.dEdx.pixelHrm") )
            LbL0.dataD[LbTkRecord::tk1DEDX_pixelHrm] = selCand.userFloat( "TkTk/Proton.dEdx.pixelHrm" );
        if ( selCand.hasUserFloat(  "TkTk/Pion.dEdx.pixelHrm") )
            LbL0.dataD[LbTkRecord::tk2DEDX_pixelHrm] = selCand.userFloat( "TkTk/Pion.dEdx.pixelHrm" );
        if ( selCand.hasUserFloat("TkTk/Proton.dEdx.Harmonic") )
            LbL0.dataD[LbTkRecord::tk1DEDX_Harmonic] = selCand.userFloat( "TkTk/Proton.dEdx.Harmonic" );
        if ( selCand.hasUserFloat(  "TkTk/Pion.dEdx.Harmonic") )
            LbL0.dataD[LbTkRecord::tk2DEDX_Harmonic] = selCand.userFloat(   "TkTk/Pion.dEdx.Harmonic" );
  
  
  
        LbL0Tree->Fill();
    }
  } // Lb->Jpsi Lam0 end }}}
  if ( useLbLo ) // Lb->JPsi Lamo {{{
  {
    edm::Handle< vector<pat::CompositeCandidate> > LbLoCands;
    LbLoCandsToken.get( ev, LbLoCands );
    std::vector<pat::CompositeCandidate>::const_iterator handleIter;
    std::vector<pat::CompositeCandidate>::const_iterator handleIend;
    // preselection {{{
    if ( !LbLoCands.isValid() ) return;
    if ( LbLoCands->size() == 0 ) return;
  
    std::vector< std::pair< double, const pat::CompositeCandidate*> > selectedCandList;
    selectedCandList.clear();
    selectedCandList.reserve( LbLoCands->size() );
    handleIter = LbLoCands->begin();
    handleIend = LbLoCands->end  ();
    while( handleIter != handleIend )
    {
        const pat::CompositeCandidate& cand = *handleIter++;
        if ( !cand.hasUserFloat("fitMass") ) continue;
        if ( !cand.hasUserData("TkTk/Proton.fitMom") ) continue;
        if ( !cand.hasUserData(  "TkTk/Pion.fitMom") ) continue;
        if ( !cand.hasUserFloat("TkTk/Proton.IPt") ) continue;
        if ( !cand.hasUserFloat(  "TkTk/Pion.IPt") ) continue;
        if ( !cand.hasUserData( "fitVertex" ) ) continue;
        if ( !cand.hasUserData( "refToJPsi" ) ) continue;
  
        const GlobalVector* dPTR[2] = {nullptr};
        dPTR[0] = cand.userData<GlobalVector>("TkTk/Proton.fitMom");
        dPTR[1] = cand.userData<GlobalVector>("TkTk/Pion.fitMom");
  
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
        if ( pMu.transverse() < 4.0 ) continue;
        if ( nMu.transverse() < 4.0 ) continue;
        if ( pMu.Momentum()   < 4.0 ) continue;
        if ( nMu.Momentum()   < 4.0 ) continue;
        if ( pTk.transverse() < 0.8 ) continue;
        //if ( nTk.transverse() < 1.0 ) continue;
        if ( pTk.Momentum()   < 0.8 ) continue;
        //if ( nTk.Momentum()   < 1.0 ) continue;
  
        if ( fourTk.transverse()< 10. ) continue;
        if ( fourTk.Momentum()  < 10. ) continue;
  
  
        const reco::Vertex* _vtx = usefulFuncs::get<reco::Vertex>( cand, "fitVertex" );
        if ( _vtx == nullptr ) continue;
        double fdSig = usefulFuncs::getFlightDistanceSignificance ( cand, &bs );
        if ( fdSig < 3.0 ) continue;
        double cos2d = usefulFuncs::getCosa2d( cand, &bs );
        double vtxprob = TMath::Prob( _vtx->chi2(), _vtx->ndof() );
        if ( cos2d < 0.95 ) continue;
        if ( vtxprob < 0.03 ) continue;
  
        selectedCandList.emplace_back( vtxprob, &cand );
    }
    // preselection end }}}
  
    unsigned N = selectedCandList.size();
    for ( unsigned i = 0; i < N; ++i )
    {
        LbLo.Clear();
        const double candVtxprob = selectedCandList[i].first;
        const pat::CompositeCandidate& selCand = *(selectedCandList[i].second);
        const pat::CompositeCandidate& tktkCand = *(usefulFuncs::getByRef<pat::CompositeCandidate>( selCand, "refToTkTk" ));
        //const pat::CompositeCandidate& jpsiCand = *(usefulFuncs::getByRef<pat::CompositeCandidate>( selCand, "refToJPsi" ));
  
        const GlobalVector* dPTR[2] = {nullptr};
        dPTR[0] = selCand.userData<GlobalVector>("TkTk/Proton.fitMom");
        dPTR[1] = selCand.userData<GlobalVector>("TkTk/Pion.fitMom");
  
        const GlobalVector* muPTR[2] = {nullptr};
        muPTR[0] = selCand.userData<GlobalVector>("JPsi/MuPos.fitMom");
        muPTR[1] = selCand.userData<GlobalVector>("JPsi/MuNeg.fitMom");
        const GlobalVector* fourTk = selCand.userData<GlobalVector>("fitMomentum");
        TLorentzVector fourTkMom( fourTk->x(), fourTk->y(), fourTk->z(), sqrt(fourTk->x()*fourTk->x()+fourTk->y()*fourTk->y()+fourTk->z()*fourTk->z()+selCand.userFloat("fitMass")*selCand.userFloat("fitMass")) );
        const GlobalVector* twoTk  = tktkCand.userData<GlobalVector>("fitMomentum");
        TLorentzVector twoTkMom( twoTk->x(), twoTk->y(), twoTk->z(), sqrt(twoTk->x()*twoTk->x()+twoTk->y()*twoTk->y()+twoTk->z()*twoTk->z()+tktkCand.userFloat("fitMass")*tktkCand.userFloat("fitMass")) );
        
        LbLo.dataD[LbTkRecord::lbtkMass] = selCand.userFloat( "fitMass" );
        LbLo.dataD[LbTkRecord::lbtkPt] = fourTkMom.Pt();
        LbLo.dataD[LbTkRecord::lbtkEta] = fourTkMom.Eta();
        LbLo.dataD[LbTkRecord::lbtkY] = fourTkMom.Rapidity();
        LbLo.dataD[LbTkRecord::lbtkPhi] = fourTk->phi();
        LbLo.dataD[LbTkRecord::lbtkFlightDistance2d] = usefulFuncs::getFlightDistance ( selCand, &bs );
        LbLo.dataD[LbTkRecord::lbtkFlightDistanceSig]= usefulFuncs::getFlightDistanceSignificance ( selCand, &bs );
        const reco::Vertex* candVtx = usefulFuncs::get<reco::Vertex>( selCand, "fitVertex" );
        LbLo.dataD[LbTkRecord::lbtkCosa2d] = usefulFuncs::getCosa2d(selCand,&bs);
        LbLo.dataD[LbTkRecord::lbtkVtxprob] = TMath::Prob( candVtx->chi2(), candVtx->ndof() );
        LbLo.dataD[LbTkRecord::lbtknChi2] = candVtx->chi2() / candVtx->ndof();

        LbLo.dataD[LbTkRecord::tktkMass] = tktkCand.userFloat( "fitMass" );
        LbLo.dataD[LbTkRecord::tktkPt] = twoTkMom.Pt();
        LbLo.dataD[LbTkRecord::tktkEta] = twoTkMom.Eta();
        LbLo.dataD[LbTkRecord::tktkY] = twoTkMom.Rapidity();
        LbLo.dataD[LbTkRecord::tktkPhi] = twoTk->phi();
        LbLo.dataD[LbTkRecord::tktkFlightDistance2d] = usefulFuncs::getFlightDistance ( tktkCand, &bs );
        LbLo.dataD[LbTkRecord::tktkFlightDistanceSig]= usefulFuncs::getFlightDistanceSignificance ( tktkCand, &bs );
        const reco::Vertex* tktkCandVtx = usefulFuncs::get<reco::Vertex>( tktkCand, "fitVertex" );
        LbLo.dataD[LbTkRecord::tktkCosa2d] = usefulFuncs::getCosa2d(tktkCand,&bs);
        LbLo.dataD[LbTkRecord::tktkVtxprob] = TMath::Prob( tktkCandVtx->chi2(), tktkCandVtx->ndof() );
        LbLo.dataD[LbTkRecord::tktknChi2] = tktkCandVtx->chi2() / tktkCandVtx->ndof();
  
        LbLo.dataD[LbTkRecord::pmuPt] = muPTR[0]->perp();
        LbLo.dataD[LbTkRecord::pmuP0] = sqrt(muPTR[0]->mag2()+0.105658*0.105658);
        LbLo.dataD[LbTkRecord::pmuP1] = muPTR[0]->x();
        LbLo.dataD[LbTkRecord::pmuP2] = muPTR[0]->y();
        LbLo.dataD[LbTkRecord::pmuP3] = muPTR[0]->z();
        LbLo.dataD[LbTkRecord::nmuPt] = muPTR[1]->perp();
        LbLo.dataD[LbTkRecord::nmuP0] = sqrt(muPTR[1]->mag2()+0.105658*0.105658);
        LbLo.dataD[LbTkRecord::nmuP1] = muPTR[1]->x();
        LbLo.dataD[LbTkRecord::nmuP2] = muPTR[1]->y();
        LbLo.dataD[LbTkRecord::nmuP3] = muPTR[1]->z();

        LbLo.dataD[LbTkRecord::tk1Pt] = dPTR[0]->perp();
        LbLo.dataD[LbTkRecord::tk1P0] = sqrt(dPTR[0]->mag2()+0.105658*0.105658);
        LbLo.dataD[LbTkRecord::tk1P1] = dPTR[0]->x();
        LbLo.dataD[LbTkRecord::tk1P2] = dPTR[0]->y();
        LbLo.dataD[LbTkRecord::tk1P3] = dPTR[0]->z();
        LbLo.dataD[LbTkRecord::tk2Pt] = dPTR[1]->perp();
        LbLo.dataD[LbTkRecord::tk2P0] = sqrt(dPTR[1]->mag2()+0.105658*0.105658);
        LbLo.dataD[LbTkRecord::tk2P1] = dPTR[1]->x();
        LbLo.dataD[LbTkRecord::tk2P2] = dPTR[1]->y();
        LbLo.dataD[LbTkRecord::tk2P3] = dPTR[1]->z();

        LbLo.dataD[LbTkRecord::tk1IPt] = selCand.userFloat("TkTk/Proton.IPt");
        LbLo.dataD[LbTkRecord::tk2IPt] = selCand.userFloat("TkTk/Pion.IPt");
        LbLo.dataD[LbTkRecord::tk1IPtErr] = selCand.userFloat("TkTk/Proton.IPt.Error");
        LbLo.dataD[LbTkRecord::tk2IPtErr] = selCand.userFloat("TkTk/Pion.IPt.Error");
        if ( selCand.hasUserFloat("TkTk/Proton.dEdx.pixelHrm") )
            LbLo.dataD[LbTkRecord::tk1DEDX_pixelHrm] = selCand.userFloat( "TkTk/Proton.dEdx.pixelHrm" );
        if ( selCand.hasUserFloat(  "TkTk/Pion.dEdx.pixelHrm") )
            LbLo.dataD[LbTkRecord::tk2DEDX_pixelHrm] = selCand.userFloat( "TkTk/Pion.dEdx.pixelHrm" );
        if ( selCand.hasUserFloat("TkTk/Proton.dEdx.Harmonic") )
            LbLo.dataD[LbTkRecord::tk1DEDX_Harmonic] = selCand.userFloat( "TkTk/Proton.dEdx.Harmonic" );
        if ( selCand.hasUserFloat(  "TkTk/Pion.dEdx.Harmonic") )
            LbLo.dataD[LbTkRecord::tk2DEDX_Harmonic] = selCand.userFloat(   "TkTk/Pion.dEdx.Harmonic" );
  
  
  
        LbLoTree->Fill();
    }
  } // Lb->Jpsi Lamo end }}}

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
