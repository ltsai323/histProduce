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

process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.source = cms.Source("PoolSource",fileNames = cms.untracked.vstring(
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-1.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-10.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-11.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-12.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-13.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-14.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-15.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-16.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-17.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-18.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-19.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-2.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-20.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-21.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-22.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-23.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-24.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-25.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-26.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-27.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-28.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-29.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-3.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-30.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-31.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-32.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-33.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-34.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-35.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-36.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-37.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-38.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-4.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-5.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-6.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-7.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-8.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1-9.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_1.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_10.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_11.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_12.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_13.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_14.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_15.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_16.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_17.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_18.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_19.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_2.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_20.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_21.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_22.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_23.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_24.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_25.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_26.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_27.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_28.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_29.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_3.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_30.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_31.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_33.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_34.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_35.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_36.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_37.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_38.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_39.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_4.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_40.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_41.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_42.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_43.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_44.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_45.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_46.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_47.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_48.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_49.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_5.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_50.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_51.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_52.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_53.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_54.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_55.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_56.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_57.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_58.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_59.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_6.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_60.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_61.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_62.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_63.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_64.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_65.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_66.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_67.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_68.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_69.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_7.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_70.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_71.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_72.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_73.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_74.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_75.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_76.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_77.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_78.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_79.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_8.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_80.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_81.root",
"file:///home/ltsai/Data/CRABdata/CRABdata_20_Aug_2018_2017RunD/recoBPHanalysis_withFilter_9.root",

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
    oniaCandsLabel = cms.string("lbWriteSpecificDecay:oniaFitted:bphAnalysis"),
    pTksCandsLabel = cms.string("lbWriteSpecificDecay:pTksFitted:bphAnalysis"),
    pL0BCandsLabel = cms.string("lbWriteSpecificDecay:pL0BFitted:bphAnalysis"),
    nTksCandsLabel = cms.string("lbWriteSpecificDecay:nTksFitted:bphAnalysis"),
    nL0BCandsLabel = cms.string("lbWriteSpecificDecay:nL0BFitted:bphAnalysis"),
      bsPointLabel = cms.string("offlineBeamSpot::RECO")
)

process.p = cms.Path(
    process.treeCreatingSpecificDecay
)


