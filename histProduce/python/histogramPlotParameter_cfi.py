import FWCore.ParameterSet.Config as cms

inFilePath = '/wk_cms/youying/CMSSW_8_0_26_patch1/src/VBFTag/'

process = cms.PSet()

process.GlobalSet = cms.PSet(
    ControlPlot = cms.string('h1_Mgg'),
    isSideBand = cms.bool(False),
    isLogScale = cms.bool(True)
)

process.cutSet = cms.VPSet(
    cms.PSet( HistoName = cms.string( 'hOniaMass' ),
              TitleName = cms.string( 'J/#Psi Mass' ),
              XaxisName = cms.string( 'GeV' ),
              XaxisRange= cms.vdouble( 2.8, 3.2 ),
              YaxisRange= cms.vint32( 0, -1 ),
              SetFill   = cms.bool( False )
              ),
    cms.PSet( HistoName = cms.string( 'hLam0Mass' ),
              TitleName = cms.string( '#Lambda^{0} Mass' ),
              XaxisName = cms.string( 'GeV' ),
              XaxisRange= cms.vdouble( 1.1, 1.15),
              YaxisRange= cms.vint32( 0, -1 ),
              SetFill   = cms.bool( False )
              ),
    cms.PSet( HistoName = cms.string( 'hTkTkMass' ),
              TitleName = cms.string( 'tktkMass' ),
              XaxisName = cms.string( 'GeV' ),
              XaxisRange= cms.vdouble( 1.0, 2.50),
              YaxisRange= cms.vint32( 0, -1 ),
              SetFill   = cms.bool( False )
              ),
    cms.PSet( HistoName = cms.string( 'hLbL0Mass' ),
              TitleName = cms.string( '#Lambda^{0}_{b} To J/#Psi+#Lambda^{0} Mass' ),
              XaxisName = cms.string( 'GeV' ),
              XaxisRange= cms.vdouble( 5.0, 6.0 ),
              YaxisRange= cms.vint32( 0, -1 ),
              SetFill   = cms.bool( False )
              ),
    cms.PSet( HistoName = cms.string( 'hLbTkMass' ),
              TitleName = cms.string( '#Lambda^{0}_{b} To J/#Psi+TkTk Mass' ),
              XaxisName = cms.string( 'GeV' ),
              XaxisRange= cms.vdouble( 5.0, 6.0 ),
              YaxisRange= cms.vint32( 0, -1 ),
              SetFill   = cms.bool( False )
              ),
)

process.SigMCSamples = cms.PSet(

    mH = cms.uint32(125),
    VBFFile = cms.string(inFilePath + 'output_VBFHToGG_M125_13TeV_amcatnlo_pythia8.root'),
    GGFFile = cms.string(inFilePath + 'output_GluGluHToGG_M125_13TeV_amcatnloFXFX_pythia8.root')
        
)

process.BkgOption = cms.PSet(
    useGJ     = cms.bool(True),
    useDipho  = cms.bool(True),
    useQCD    = cms.bool(True),
    useDYjets = cms.bool(False)
 
)

