
import FWCore.ParameterSet.Config as cms

files = cms.vstring()
files.extend( [
"/home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPHSDecay/recoWriteSDecay.mcLbToJPsipK_13TeV_noPU.00.root",
"/home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPHSDecay/recoWriteSDecay.mcLbToJPsipK_13TeV_noPU.01.root",
"/home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPHSDecay/recoWriteSDecay.mcLbToJPsipK_13TeV_noPU.02.root",
"/home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPHSDecay/recoWriteSDecay.mcLbToJPsipK_13TeV_noPU.03.root",
"/home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPHSDecay/recoWriteSDecay.mcLbToJPsipK_13TeV_noPU.04.root",
"/home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPHSDecay/recoWriteSDecay.mcLbToJPsipK_13TeV_noPU.05.root",
"/home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPHSDecay/recoWriteSDecay.mcLbToJPsipK_13TeV_noPU.06.root",
"/home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPHSDecay/recoWriteSDecay.mcLbToJPsipK_13TeV_noPU.07.root",
"/home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPHSDecay/recoWriteSDecay.mcLbToJPsipK_13TeV_noPU.08.root",
"/home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPHSDecay/recoWriteSDecay.mcLbToJPsipK_13TeV_noPU.09.root",
"/home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPHSDecay/recoWriteSDecay.mcLbToJPsipK_13TeV_noPU.10.root",
"/home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPHSDecay/recoWriteSDecay.mcLbToJPsipK_13TeV_noPU.11.root",
"/home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPHSDecay/recoWriteSDecay.mcLbToJPsipK_13TeV_noPU.12.root",
"/home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPHSDecay/recoWriteSDecay.mcLbToJPsipK_13TeV_noPU.13.root",
"/home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPHSDecay/recoWriteSDecay.mcLbToJPsipK_13TeV_noPU.14.root",
"/home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPHSDecay/recoWriteSDecay.mcLbToJPsipK_13TeV_noPU.15.root",
"/home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPHSDecay/recoWriteSDecay.mcLbToJPsipK_13TeV_noPU.16.root",
"/home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPHSDecay/recoWriteSDecay.mcLbToJPsipK_13TeV_noPU.17.root",
"/home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPHSDecay/recoWriteSDecay.mcLbToJPsipK_13TeV_noPU.18.root",
"/home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPHSDecay/recoWriteSDecay.mcLbToJPsipK_13TeV_noPU.19.root",
"/home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPHSDecay/recoWriteSDecay.mcLbToJPsipK_13TeV_noPU.20.root",
"/home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPHSDecay/recoWriteSDecay.mcLbToJPsipK_13TeV_noPU.21.root",
"/home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPHSDecay/recoWriteSDecay.mcLbToJPsipK_13TeV_noPU.22.root",
"/home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPHSDecay/recoWriteSDecay.mcLbToJPsipK_13TeV_noPU.23.root",
"/home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPHSDecay/recoWriteSDecay.mcLbToJPsipK_13TeV_noPU.24.root",
"/home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPHSDecay/recoWriteSDecay.mcLbToJPsipK_13TeV_noPU.25.root",
"/home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPHSDecay/recoWriteSDecay.mcLbToJPsipK_13TeV_noPU.26.root",
"/home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPHSDecay/recoWriteSDecay.mcLbToJPsipK_13TeV_noPU.27.root",
"/home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPHSDecay/recoWriteSDecay.mcLbToJPsipK_13TeV_noPU.28.root",
"/home/ltsai/Data/mcStep3_LbTJPsipK_13TeV_withoutPileUp_180524/BPHSDecay/recoWriteSDecay.mcLbToJPsipK_13TeV_noPU.29.root"
 ] )

process = cms.PSet()

process.runSetting = cms.PSet(
        maxEvents = cms.int32(-1),
        outEvery  = cms.uint32(0),
        outName   = cms.string('tree_LbToJPsipK_MC_13TeV_noPU_noPreSelection.root'),
        )
process.inputFiles = cms.PSet( fileNames  = files )
