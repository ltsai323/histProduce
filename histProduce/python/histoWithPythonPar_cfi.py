import FWCore.ParameterSet.Config as cms


process = cms.PSet()

process.GlobalSet = cms.PSet(
    ControlPlot = cms.string('h1_Mgg'),
    XaxisName  = cms.string( 'GeV' ),
    XaxisRange = cms.vdouble( 5.0, 6.0 ),
    YaxisRange = cms.vint32( 0, -1 ),
    isSideBand = cms.bool(False),
    isLogScale = cms.bool(True)
)

process.plotSet = cms.VPSet(
    cms.PSet( HistoName = cms.string( 'hOniaMass' ),
              TitleName = cms.string( 'J/#Psi Mass' ),
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 0 ),
              SetVTLine = cms.VPSet(
                  cms.PSet( VLine = cms.double() ),
                  cms.PSet( VLine = cms.double() ),
                  ),
              ),
    cms.PSet( HistoName = cms.string( 'hLam0Mass' ),
              TitleName = cms.string( '#Lambda^{0} Mass' ),
              SetFill   = cms.bool( False ),
              ),
    cms.PSet( HistoName = cms.string( 'hTkTkMass' ),
              TitleName = cms.string( 'tktkMass' ),
              SetFill   = cms.bool( False ),
              ),
    cms.PSet( HistoName = cms.string( 'hLbL0Mass' ),
              TitleName = cms.string( '#Lambda^{0}_{b} To J/#Psi+#Lambda^{0} Mass' ),
              SetFill   = cms.bool( False ),
              ),
    cms.PSet( HistoName = cms.string( 'hLbTkMass' ),
              TitleName = cms.string( '#Lambda^{0}_{b} To J/#Psi+TkTk Mass' ),
              SetFill   = cms.bool( False ),
              ),
)

#process.SigMCSamples = cms.PSet(
#
#    mH = cms.uint32(125),
#    VBFFile = cms.string(inFilePath + 'output_VBFHToGG_M125_13TeV_amcatnlo_pythia8.root'),
#    GGFFile = cms.string(inFilePath + 'output_GluGluHToGG_M125_13TeV_amcatnloFXFX_pythia8.root')
#)
#
#process.BkgOption = cms.PSet(
#    useGJ     = cms.bool(True),
#    useDipho  = cms.bool(True),
#    useQCD    = cms.bool(True),
#    useDYjets = cms.bool(False)
#)
process.cutSet = cms.PSet(
        pvCut = cms.vint32( 0, 1 ),
        )



