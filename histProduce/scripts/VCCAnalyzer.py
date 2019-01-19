#!/usr/bin/env cmsRun
import FWCore.ParameterSet.Config as cms

process = cms.Process("analyzer")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
#process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(3000) )

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
#from histProduce.histProduce.data_2016RunG_LbL0_cfi import files
#process.source = cms.Source("PoolSource",fileNames = files,
process.source = cms.Source("PoolSource",fileNames = cms.untracked.vstring(
#'/store/user/ltsai/vertexProducer/20181228revision/Charmonium/2016RunC_vertexProducer/181228_101222/0000/vertexProducer_BdRemoved_9.root'
'file:a.root',
#'file:///home/ltsai/ReceivedFile/tmp/vertexProducer_BdRemoved_1-10.root'
),
        duplicateCheckMode = cms.untracked.string('noDuplicateCheck')
)

from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
#process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')
process.GlobalTag = GlobalTag(process.GlobalTag, '80X_dataRun2_2016LegacyRepro_v3', '')
#process.GlobalTag = GlobalTag(process.GlobalTag, '94X_dataRun2_ReReco_EOY17_v1', '')


process.TFileService = cms.Service('TFileService',
  fileName = cms.string('tree_VCCAnalyzer.root'),
  closeFileFast = cms.untracked.bool(True)
)

process.VertexCompCandAnalyzer = cms.EDAnalyzer('VertexCompCandAnalyzer',
    pL0BCandsLabel = cms.string("fourTracksFromVCCProducer:pL0B:myVertexingProcedure"),
    nL0BCandsLabel = cms.string("fourTracksFromVCCProducer:nL0B:myVertexingProcedure"),
    LbL0CandsLabel = cms.string("fourTracksFromVCCProducer:LbL0:myVertexingProcedure"),
    LbLoCandsLabel = cms.string("fourTracksFromVCCProducer:LbLo:myVertexingProcedure"),
    HLTRecordLabel = cms.string("TriggerResults::HLT"),
      bsPointLabel = cms.string("offlineBeamSpot::RECO")
)


process.p = cms.Path(
      process.treeCreatingSpecificDecay
)
