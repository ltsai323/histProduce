#include <memory>
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"


#include "DataFormats/PatCandidates/interface/GenericParticle.h"
#include <vector>
#include "CommonTools/CandUtils/interface/AddFourMomenta.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "vertexProducer/MCMatchTools/interface/familyRelationShipLbToPcK.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include <TTree.h>
#include <TFile.h>



//typedef pat::GenericParticle myTrack;
typedef reco::Track myTrack;
typedef std::vector < myTrack > myTrackList;
typedef reco::GenParticle MCparticle;
typedef std::vector<reco::GenParticle> MCparticleList;

#define MAX_TRACKS 2048

//------------------------------------------------------------------------------
//   Class Definition
//------------------------------------------------------------------------------
class TrackGenMatchResult:public edm::EDAnalyzer
{
public:
    explicit TrackGenMatchResult(const edm::ParameterSet &);
    ~TrackGenMatchResult();
    static void fillDescriptions(edm::ConfigurationDescriptions & descriptions);

	virtual void beginJob() override;
    virtual void analyze( const edm::Event&, const edm::EventSetup& ) override;
	virtual void endJob() override;
private:

    // Data Members
    const edm::EDGetTokenT < myTrackList >  _tkToken;
    const edm::EDGetTokenT<MCparticleList>  _mcMatchToken;

    edm::Handle < myTrackList > _tkHandle;
    edm::Handle<MCparticleList> _mcMatchHandle;

    familyRelationShip* _mcDaugDetail;

    TTree* _deltaRTree;
    double** _dataD;
    int**    _dataI;

    enum recordDataD { _dRVal, _dRSig, _ptRatio,_totNumD };
    enum recordDataI { _charge, _totNumI };
    int _nTracks;

	edm::Service<TFileService> _fs;
};

//------------------------------------------------------------------------------
//   Constructor and destructor
//------------------------------------------------------------------------------

TrackGenMatchResult::TrackGenMatchResult(const edm::ParameterSet & iConfig):
    _tkToken(consumes < myTrackList > (iConfig.getParameter < edm::InputTag > ("tracksrc"))),
    _mcMatchToken(consumes<MCparticleList>(iConfig.getParameter<edm::InputTag>("mcmatchsrc")))
{
    //return;
    _mcDaugDetail = new familyRelationShipLbToPcK();
    _dataD = new double*[_totNumD];
    _dataI = new int*[_totNumI];
    for ( int i=0; i < _totNumD; ++i )
        _dataD[i] = new double[MAX_TRACKS];
    for ( int i=0; i < _totNumI; ++i )
        _dataI[i] = new int   [MAX_TRACKS];

    _deltaRTree = _fs->make<TTree>("deltaRRec", "deltaRRec");

    _deltaRTree->Branch("nTracks", &_nTracks, "nTracks/I");
    _deltaRTree->Branch("dRVal", _dataD[_dRVal], "dRVal[nTracks]/D");
    _deltaRTree->Branch("dRSig", _dataD[_dRSig], "dRSig[nTracks]/D");
    _deltaRTree->Branch("ptRatio", _dataD[_ptRatio], "ptRatio[nTracks]/D");
    _deltaRTree->Branch("charge", _dataI[_charge], "charge[nTracks]/I");
    return;
}

TrackGenMatchResult::~TrackGenMatchResult()
{
    //return;
    for ( int i=0; i<_totNumD; ++i )
        delete[] _dataD[i];
    for ( int i=0; i<_totNumI; ++i )
        delete[] _dataI[i];
    delete[] _dataD;
    delete[] _dataI;
    delete _mcDaugDetail;
    return;
}

void TrackGenMatchResult::beginJob()
{
    return;
}

//------------------------------------------------------------------------------
//   Main Control Flow
//------------------------------------------------------------------------------
void TrackGenMatchResult::analyze( const edm::Event& iEvent, const edm::EventSetup& iSetup )
{
    //return;
    iEvent.getByToken(_tkToken, _tkHandle);
    iEvent.getByToken(_mcMatchToken, _mcMatchHandle);

    _nTracks = _tkHandle->size();
    



    // Object selection
    for ( unsigned idx = 0; idx < _tkHandle->size(); ++idx )
    {
        const myTrack& tk = _tkHandle.product()->at(idx);
        
        _dataI[_charge][idx] = tk.charge();
        _dataD[_dRVal][idx] = 999.; 
        _dataD[_dRSig][idx] = 999.; 
        _dataD[_ptRatio][idx] = 999.;


        double& dRVal = _dataD[_dRVal][idx];
        double& dRSig = _dataD[_dRSig][idx];
        double& ptRatio = _dataD[_ptRatio][idx];
        for ( const MCparticle& mc : *(_mcMatchHandle.product()) )
        {
            if ( fabs(mc.pdgId()) != 5122 ) continue;
            if ( !_mcDaugDetail->isTargetGenParticleInDecayChannel(mc) ) continue;
            
            const reco::Candidate* daugPtr = &mc;
            for ( int layerIdx = 0; layerIdx < _mcDaugDetail->daugLayer(2); ++layerIdx )
                if ( daugPtr->status() != 1 )
                    daugPtr = daugPtr->daughter( _mcDaugDetail->getDaughterIdxOnLayer(2,layerIdx) );
            if ( daugPtr )
                if ( daugPtr->charge() == tk.charge() )
                    if ( daugPtr->status() == 1 )
            {
                dRVal = _mcDaugDetail->deltaR_Value(tk, *daugPtr);
                dRSig = _mcDaugDetail->deltaR_Significance(tk, *daugPtr);
                ptRatio = fabs(tk.pt()-daugPtr->pt())/daugPtr->pt();
            }
            daugPtr = &mc;
            for ( int layerIdx = 0; layerIdx < _mcDaugDetail->daugLayer(3); ++layerIdx )
                if ( daugPtr->status() != 1 )
                    daugPtr = daugPtr->daughter( _mcDaugDetail->getDaughterIdxOnLayer(3,layerIdx) );
            if ( daugPtr )
                if ( daugPtr->charge() == tk.charge() )
                    if ( daugPtr->status() == 1 )
            {
                dRVal = _mcDaugDetail->deltaR_Value(tk, *daugPtr);
                dRSig = _mcDaugDetail->deltaR_Significance(tk, *daugPtr);
                ptRatio = fabs(tk.pt()-daugPtr->pt())/daugPtr->pt();
            }
            break;
        }

    }

    
    _deltaRTree->Fill();
    return;
}

void TrackGenMatchResult::endJob()
{
    return;
}


//------------------------------------------------------------------------------
//   EDM Plugin requirements
//------------------------------------------------------------------------------
void TrackGenMatchResult::fillDescriptions(edm::ConfigurationDescriptions & descriptions)
{
    //The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}
//define this as a plug-in
#include "FWCore/PluginManager/interface/ModuleDef.h"
DEFINE_FWK_MODULE(TrackGenMatchResult);
