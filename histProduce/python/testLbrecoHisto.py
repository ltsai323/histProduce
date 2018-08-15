#!/usr/bin/env cmsRun
import FWCore.ParameterSet.Config as cms

process = cms.Process("treeCreatingSDecay")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

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

process.MessageLogger.cerr.FwkReport.reportEvery = 100

process.source = cms.Source("PoolSource",fileNames = cms.untracked.vstring(
    'file:recoWriteSDecay.2017testData_13TeV.root'

))

from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
#process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')
#process.GlobalTag = GlobalTag(process.GlobalTag, '80X_dataRun2_2016LegacyRepro_v3', '')
process.GlobalTag = GlobalTag(process.GlobalTag, '94X_dataRun2_ReReco_EOY17_v1', '')

process.TFileService = cms.Service('TFileService',
  fileName = cms.string('treeCreatingSpecificDecay_2017Data.root'),
  closeFileFast = cms.untracked.bool(True)
)

process.treeCreatingSpecificDecay = cms.EDAnalyzer('treeCreatingSpecificDecay',
    oniaCandsLabel = cms.string("lbWriteSpecificDecay:oniaFitted:bphAnalysis"   ),
    tktkCandsLabel = cms.string("lbWriteSpecificDecay:TkTkFitted:bphAnalysis"   ),
    LbTkCandsLabel = cms.string("lbWriteSpecificDecay:LbToTkTkFitted:bphAnalysis"),
      bsPointLabel = cms.string("offlineBeamSpot::RECO")
)

process.p = cms.Path(
    process.treeCreatingSpecificDecay
)


