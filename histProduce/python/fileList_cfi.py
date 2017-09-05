import FWCore.ParameterSet.Config as cms

process = cms.PSet()

process.runSetting = cms.PSet(
        maxEvents = cms.int32(-1),
        outEvery  = cms.uint32(1000),
        outName   = cms.string('myTestOutputHistogram.root')
        )
process.inputFiles = cms.PSet(
    fileNames   = cms.vstring('root://eoscms//eos/cms/store/relval/CMSSW_7_1_0_pre1/RelValProdTTbar/GEN-SIM-RECO/START70_V5-v1/00000/14842A6B-2086-E311-B5CB-02163E00E8DA.root')
)

