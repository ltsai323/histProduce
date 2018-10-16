#!/usr/bin/env cmsRun
import FWCore.ParameterSet.Config as cms

process = cms.Process("bphAnalysis")

#process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(3000) )
process.load("Configuration.Geometry.GeometryRecoDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.load("TrackingTools/TransientTrack/TransientTrackBuilder_cfi")


process.MessageLogger.cerr.FwkReport.reportEvery = 1000
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )
process.options.allowUnscheduled = cms.untracked.bool(True)

process.source = cms.Source("PoolSource",fileNames = cms.untracked.vstring(
"file:///home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPH-RunIISpring16DR80-00058.20.root",
"file:///home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPH-RunIISpring16DR80-00058.21.root",
"file:///home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPH-RunIISpring16DR80-00058.22.root",
"file:///home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPH-RunIISpring16DR80-00058.23.root",
"file:///home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPH-RunIISpring16DR80-00058.24.root",
"file:///home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPH-RunIISpring16DR80-00058.25.root",
"file:///home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPH-RunIISpring16DR80-00058.26.root",
"file:///home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPH-RunIISpring16DR80-00058.27.root",
"file:///home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPH-RunIISpring16DR80-00058.28.root",
"file:///home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPH-RunIISpring16DR80-00058.29.root",
"file:///home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPH-RunIISpring16DR80-00058_00.root",
"file:///home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPH-RunIISpring16DR80-00058_01.root",
"file:///home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPH-RunIISpring16DR80-00058_02.root",
"file:///home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPH-RunIISpring16DR80-00058_03.root",
"file:///home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPH-RunIISpring16DR80-00058_04.root",
"file:///home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPH-RunIISpring16DR80-00058_05.root",
"file:///home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPH-RunIISpring16DR80-00058_06.root",
"file:///home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPH-RunIISpring16DR80-00058_07.root",
"file:///home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPH-RunIISpring16DR80-00058_08.root",
"file:///home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPH-RunIISpring16DR80-00058_09.root",
"file:///home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPH-RunIISpring16DR80-00058_10.root",
"file:///home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPH-RunIISpring16DR80-00058_11.root",
"file:///home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPH-RunIISpring16DR80-00058_12.root",
"file:///home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPH-RunIISpring16DR80-00058_13.root",
"file:///home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPH-RunIISpring16DR80-00058_14.root",
"file:///home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPH-RunIISpring16DR80-00058_15.root",
"file:///home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPH-RunIISpring16DR80-00058_16.root",
"file:///home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPH-RunIISpring16DR80-00058_17.root",
"file:///home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPH-RunIISpring16DR80-00058_18.root",
"file:///home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPH-RunIISpring16DR80-00058_19.root",
),
        duplicateCheckMode = cms.untracked.string('noDuplicateCheck')
)
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
#process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')
process.GlobalTag = GlobalTag(process.GlobalTag, '80X_dataRun2_2016LegacyRepro_v3', '')
#process.GlobalTag = GlobalTag(process.GlobalTag, '94X_dataRun2_ReReco_EOY17_v1', '')

#HLTName='HLT_DoubleMu4_JpsiTrk_Displaced_v*'
HLTName='HLT_DoubleMu4_3_Jpsi_Displaced_v*'
from HLTrigger.HLTfilters.hltHighLevel_cfi import hltHighLevel
process.hltHighLevel= hltHighLevel.clone(HLTPaths = cms.vstring(HLTName))

# preselect pat muon and pat tracks choose for CMSSW version
process.load("BPHAnalysis.PreselectFilter.FilterConf9_cfi")
#process.load("BPHAnalysis.PreselectFilter.FilterConf8_cfi")

# remove MC dependence
from PhysicsTools.PatAlgos.tools.coreTools import *
removeMCMatching(process, names=['All'], outputModules=[] )

from BPHAnalysis.SpecificDecay.LbrecoSelectForWrite_cfi import recoSelect
process.lbWriteSpecificDecay = cms.EDProducer('lbWriteSpecificDecay',

# the label used in calling data
    bsPointLabel = cms.string('offlineBeamSpot::RECO'),
    pVertexLabel = cms.string('offlinePrimaryVertices::RECO'),
    gpCandsLabel = cms.string('selectedTracks'),
    patMuonLabel = cms.string('selectedMuons'),
    dedxHrmLabel = cms.string('dedxHarmonic2::RECO'),

# the label of output product
    oniaName      = cms.string('oniaFitted'),
    pTksName      = cms.string('pTksFitted'),
    pL0BName      = cms.string('pL0BFitted'),
    nTksName      = cms.string('nTksFitted'),
    nL0BName      = cms.string('nL0BFitted'),

    Lam0Name      = cms.string('Lam0Fitted'),
    LbL0Name      = cms.string('LbL0Fitted'),
    LamoName      = cms.string('LamoFitted'),
    LbLoName      = cms.string('LbLoFitted'),
    writeVertex   = cms.bool( True ),
    writeMomentum = cms.bool( True ),
    recoSelect    = cms.VPSet(recoSelect)
)


# used for EDAnalyzer output 
# cms.outputModules is disabled for delete the output of EDProducer
process.TFileService = cms.Service('TFileService',
  fileName = cms.string('treelbSpecificDecay_LbL0_LbTk.root'),
  closeFileFast = cms.untracked.bool(True)
)

process.lbSpecificDecay = cms.EDAnalyzer('treeCreatingSpecificDecay',
    oniaCandsLabel = cms.string("lbWriteSpecificDecay:oniaFitted:bphAnalysis"),
    pTksCandsLabel = cms.string("lbWriteSpecificDecay:pTksFitted:bphAnalysis"),
    pL0BCandsLabel = cms.string("lbWriteSpecificDecay:pL0BFitted:bphAnalysis"),
    nTksCandsLabel = cms.string("lbWriteSpecificDecay:nTksFitted:bphAnalysis"),
    nL0BCandsLabel = cms.string("lbWriteSpecificDecay:nL0BFitted:bphAnalysis"),

    Lam0CandsLabel = cms.string("lbWriteSpecificDecay:Lam0Fitted:bphAnalysis"),
    LbL0CandsLabel = cms.string("lbWriteSpecificDecay:LbL0Fitted:bphAnalysis"),
    LamoCandsLabel = cms.string("lbWriteSpecificDecay:LamoFitted:bphAnalysis"),
    LbLoCandsLabel = cms.string("lbWriteSpecificDecay:LbLoFitted:bphAnalysis"),
      bsPointLabel = cms.string("offlineBeamSpot::RECO")
)

process.myfilterpath = cms.Path(
      process.hltHighLevel

    # for CMSSW9
    * process.myMuonsSequence
    * process.myTrackSequence

    # for CMSSW8
    # * process.selectedMuons
    # * process.selectedTracks
    * process.lbWriteSpecificDecay
    * process.lbSpecificDecay
)

