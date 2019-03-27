#!/usr/bin/env cmsRun
import FWCore.ParameterSet.Config as cms

process = cms.Process("analyzer")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.Services_cff')

process.MessageLogger.cerr.FwkReport.reportEvery = 1000

from vertexProducer.generalFileList.mcStep3 import readFiles

process.source = cms.Source("PoolSource",fileNames = readFiles,
#process.source = cms.Source("PoolSource",fileNames = cms.untracked.vstring(
#'file:///home/ltsai/Data/mcStep3_LbToPcK_13TeV_withoutPileUp_190219/step3_1.root'
#),
        duplicateCheckMode = cms.untracked.string('noDuplicateCheck')
)



process.TFileService = cms.Service('TFileService',
  fileName = cms.string('tree_VCCAnalyzer_forTest.root'),
  closeFileFast = cms.untracked.bool(False)
)

process.TrackGenMatchResult = cms.EDAnalyzer('TrackGenMatchResult',
        tracksrc=cms.InputTag('generalTracks'),
        mcmatchsrc=cms.InputTag('genParticles::HLT'),
)


process.p = cms.Path(
      process.TrackGenMatchResult
)
