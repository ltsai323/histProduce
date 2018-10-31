#ifndef histProduce_histProduce_lam0Loader_h
#define histProduce_histProduce_lam0Loader_h

// this code should use BPHAnalysis and histProduce, so if there is compiling error, please download BPHAnalysis package.
// git clone https://github.com/johnson20050/BPHAnalysis
#include "BPHAnalysis/RecoDecay/interface/BPHAnalyzerTokenWrapper.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "DataFormats/Candidate/interface/VertexCompositeCandidate.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/Common/interface/Ref.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include <string>

#include "histProduce/histProduce/interface/formatTreeLbTknew.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"

class TH1F;
class TVector3;

namespace reco {
  class Candidate;
  class Vertex;
}

// use the tree structure of treeMain, not to use the Process function.
typedef formatTree_newLbTk LbTkRecord;
class lam0Loader:
      public BPHAnalyzerWrapper<BPHModuleWrapper::one_analyzer> {

 public:

  explicit lam0Loader( const edm::ParameterSet& ps );
  virtual ~lam0Loader();

  static void fillDescriptions( edm::ConfigurationDescriptions& descriptions );

  virtual void beginJob();
  virtual void analyze( const edm::Event& ev, const edm::EventSetup& es );
  virtual void endJob();

  class CandidateSelect {
   public:
    virtual ~CandidateSelect() {}
    virtual bool accept( const pat::CompositeCandidate& cand,
                         const reco::Vertex* pv = 0 ) const = 0 ;
  };

 private:

  std::string Lam0CandsLabel;
  std::string MyL0CandsLabel;
  std::string   bsPointLabel;
  BPHTokenWrapper< std::vector<reco::VertexCompositeCandidate> > Lam0CandsToken;
  BPHTokenWrapper< std::vector<pat::CompositeCandidate>        > MyL0CandsToken;
  BPHTokenWrapper< reco::BeamSpot                              >   bsPointToken;
  bool useLam0;
  bool useMyL0;
  bool useBS;

  edm::Service<TFileService> fs;
  //TFileDirectory dir;

  std::map<std::string,TH1F*> histoMap;



  void fillHisto   ( const std::string& name, float x );
  void createHisto ( const std::string& name,
                     int nbin, float hmin, float hmax );

  bool sameVal( double val1, double val2, double minValue = 0.0001 )
  { if ( fabs(val1-val2) < minValue ) return true; return false; }
  bool theSameDaugs( const reco::Candidate* daug1, const reco::Candidate* daug2, double minValue )
  { if ( sameVal(daug1->eta(), daug2->eta(), minValue) && sameVal(daug1->phi(), daug2->phi(), minValue) ) return true; return false; }
};

#endif
