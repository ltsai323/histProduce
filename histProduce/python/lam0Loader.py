#!/usr/bin/env cmsRun
import FWCore.ParameterSet.Config as cms

process = cms.Process("myLam0Loader")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10000) )

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

process.source = cms.Source("PoolSource",fileNames = cms.untracked.vstring(
    'file:producer_LbL0_LbTk_testPreselection.root',
    #'file:producer.root',
#'file:///home/ltsai/Work/ReceivedFile/2016RunC/0C4F845D-DE9B-E711-B872-001E67A3AEB8.root',
#'file:///home/ltsai/Work/ReceivedFile/2016RunC/6C8E640D-E19B-E711-9CF3-001E67D8A423.root',
#'file:///home/ltsai/Work/ReceivedFile/2016RunC/B8F7A9DA-E19B-E711-9663-A4BF0102A5F4.root',
),
        duplicateCheckMode = cms.untracked.string('noDuplicateCheck')
)

from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
#process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')
process.GlobalTag = GlobalTag(process.GlobalTag, '80X_dataRun2_2016LegacyRepro_v3', '')
#process.GlobalTag = GlobalTag(process.GlobalTag, '94X_dataRun2_ReReco_EOY17_v1', '')

process.TFileService = cms.Service('TFileService',
  #fileName = cms.string('lam0Loader.root'),
  fileName = cms.string('lam0Loader_Test.root'),
  closeFileFast = cms.untracked.bool(True)
)

process.lam0Loader = cms.EDAnalyzer('lam0Loader',
    #Lam0CandsLabel = cms.string("generalV0Candidates:Lambda:RECO"),
    MyL0CandsLabel = cms.string("lbWriteSpecificDecay:Lam0Fitted:bphAnalysis"),
      bsPointLabel = cms.string("offlineBeamSpot::RECO")
)

process.p = cms.Path(
    process.lam0Loader
)
