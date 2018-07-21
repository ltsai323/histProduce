#ifndef histProduce_histProduce_treeCreatingSpecificDecay_h
#define histProduce_histProduce_treeCreatingSpecificDecay_h

// this code should use BPHAnalysis and histProduce, so if there is compiling error, please download BPHAnalysis package.
// git clone https://github.com/johnson20050/BPHAnalysis
#include "BPHAnalysis/RecoDecay/interface/BPHAnalyzerTokenWrapper.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/Ref.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include <string>

#include "histProduce/histProduce/interface/tmainLbTk.h"

class TH1F;
class TVector3;

namespace reco {
  class Candidate;
  class Vertex;
}

// use the tree structure of treeMain, not to use the Process function.
typedef treeMain_LbTk dataRecord;
class treeCreatingSpecificDecay:
      public BPHAnalyzerWrapper<BPHModuleWrapper::one_analyzer> {

 public:

  explicit treeCreatingSpecificDecay( const edm::ParameterSet& ps );
  virtual ~treeCreatingSpecificDecay();

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

  std::string oniaCandsLabel;
  std::string tktkCandsLabel;
  std::string LbTkCandsLabel;
  std::string   bsPointLabel;
  BPHTokenWrapper< std::vector<pat::CompositeCandidate> > oniaCandsToken;
  BPHTokenWrapper< std::vector<pat::CompositeCandidate> > tktkCandsToken;
  BPHTokenWrapper< std::vector<pat::CompositeCandidate> > LbTkCandsToken;
  BPHTokenWrapper< reco::BeamSpot                       >   bsPointToken;
  bool useOnia;
  bool useTkTk;
  bool useLbTk;
  bool useBS;

  edm::Service<TFileService> fs;
  TFileDirectory dir;

  std::map<std::string,TH1F*> histoMap;

  dataRecord myData;

  void fillHisto   ( const std::string& name,
                     const pat::CompositeCandidate& cand );
  void fillHisto   ( const std::string& name, float x );
  void createHisto ( const std::string& name,
                     int nbin, float hmin, float hmax );

};

#endif
