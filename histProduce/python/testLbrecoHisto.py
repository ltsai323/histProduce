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
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_10.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_100.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1000.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1001.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1002.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1003.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1004.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1005.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1006.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1007.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1008.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1009.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_101.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1010.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1011.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1012.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1013.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1014.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1015.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1016.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1017.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1018.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1019.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_102.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1020.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1021.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1022.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1023.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1024.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1025.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1026.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1027.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1028.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1029.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_103.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1030.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1031.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1032.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1033.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1034.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1035.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1036.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1037.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1038.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1039.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_104.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1040.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_9_Mar_2018_180309_2016RunH_07Apr/recoWriteSpecificDecay_1041.root",
))

from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
#process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')
process.GlobalTag = GlobalTag(process.GlobalTag, '80X_dataRun2_2016LegacyRepro_v3', '')

process.TFileService = cms.Service('TFileService',
  fileName = cms.string('his_myWriteSDecay.root'),
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


