#!/usr/bin/env cmsRun
import FWCore.ParameterSet.Config as cms

process = cms.Process("treeCreatingSDecay")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
#process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100000) )

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

#from histProduce.histProduce.data_bphOrig_cfi import files
from histProduce.histProduce.data_2016RunG_LbL0_cfi import files
process.source = cms.Source("PoolSource",fileNames = files,
#process.source = cms.Source("PoolSource",fileNames = cms.untracked.vstring(
##'file:recoBPHanalysis_withFilter.root'
#'file:///home/ltsai/ReceivedFile/rocheseReco.root'
#),
        duplicateCheckMode = cms.untracked.string('noDuplicateCheck')
)

from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
#process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')
process.GlobalTag = GlobalTag(process.GlobalTag, '80X_dataRun2_2016LegacyRepro_v3', '')
#process.GlobalTag = GlobalTag(process.GlobalTag, '94X_dataRun2_ReReco_EOY17_v1', '')

#HLTName='HLT_DoubleMu4_JpsiTrk_Displaced_v*'
#HLTName='HLT_DoubleMu4_3_Jpsi_Displaced_v*'
HLTName='HLT_Dimuon16_Jpsi_v*'
from HLTrigger.HLTfilters.hltHighLevel_cfi import hltHighLevel
process.hltHighLevel= hltHighLevel.clone(HLTPaths = cms.vstring(HLTName))

process.TFileService = cms.Service('TFileService',
  fileName = cms.string('treeCreatingSpecificDecay_2016RunG_LbL0_LbTk.root'),
  closeFileFast = cms.untracked.bool(True)
)

process.treeCreatingSpecificDecay = cms.EDAnalyzer('treeCreatingSpecificDecay',
    oniaCandsLabel = cms.string("lbWriteSpecificDecay:oniaFitted:bphAnalysis"),
    pTksCandsLabel = cms.string("lbWriteSpecificDecay:pTksFitted:bphAnalysis"),
    pL0BCandsLabel = cms.string("lbWriteSpecificDecay:pL0BFitted:bphAnalysis"),
    nTksCandsLabel = cms.string("lbWriteSpecificDecay:nTksFitted:bphAnalysis"),
    nL0BCandsLabel = cms.string("lbWriteSpecificDecay:nL0BFitted:bphAnalysis"),

    Lam0CandsLabel = cms.string("lbWriteSpecificDecay:Lam0Fitted:bphAnalysis"),
    LbL0CandsLabel = cms.string("lbWriteSpecificDecay:LbL0Fitted:bphAnalysis"),
    #Lam0CandsLabel = cms.string('bphWriteSpecificDecay:l0Fitted:bphAnalysis'),
    #LbL0CandsLabel = cms.string('bphWriteSpecificDecay:lbFitted:bphAnalysis'),
    LamoCandsLabel = cms.string("lbWriteSpecificDecay:LamoFitted:bphAnalysis"),
    LbLoCandsLabel = cms.string("lbWriteSpecificDecay:LbLoFitted:bphAnalysis"),
      bsPointLabel = cms.string("offlineBeamSpot::RECO")
)

process.p = cms.Path(
    # process.hltHighLevel
      process.treeCreatingSpecificDecay
)


