#!/usr/bin/env cmsRun
# usage:
#   cmsRun ./a.py readSource=/wk_cms/ltsai/LbFrame/CMSSW_9_4_0/src/vertexProducer/generalFileList/python/mcPosLb_LbToJPsikP_fileList_cfi.py  outputTag=posLBtoJPsipK_noPreselection
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

from FWCore.ParameterSet.VarParsing import VarParsing
options=VarParsing('analysis')
options.register('readSource', None                 , VarParsing.multiplicity.singleton, VarParsing.varType.string,'input source contains file list to load')
options.register('outputTag' , 'VCCAnalyzer_forTest', VarParsing.multiplicity.singleton, VarParsing.varType.string,'tag to create outputFileName, "tag" inputed will goes to "tag_(inputFileName)_cfi.py" ')
options.parseArguments()
if not options.readSource:
    print 'select input file list : use [readSource] option'
    exit(1)

# handle input file path to file name. /a/b/c_cfi.py -> c_cfi
sepIFName=options.readSource.split('/')
sepedIFName=sepIFName[ len(sepIFName)-1 ]
finalIFName=sepedIFName.split('.') [0]

import imp
myImport=imp.load_source('readFiles', options.readSource)
#from histProduce.histProduce.data_bphOrig_cfi import readFiles
process.source = cms.Source("PoolSource",
        fileNames = myImport.readFiles,
        duplicateCheckMode = cms.untracked.string('noDuplicateCheck')
)

from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')
#process.GlobalTag = GlobalTag(process.GlobalTag, '80X_dataRun2_2016LegacyRepro_v3', '')
#process.GlobalTag = GlobalTag(process.GlobalTag, '94X_dataRun2_ReReco_EOY17_v1', '')


process.TFileService = cms.Service('TFileService',
  fileName = cms.string('tree_{}_{}.root'.format(options.outputTag, finalIFName)),
  #fileName = cms.string('tree_%s.root' % options.outputTag),
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
      process.VertexCompCandAnalyzer
)

