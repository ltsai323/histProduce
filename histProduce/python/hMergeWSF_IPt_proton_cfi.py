import FWCore.ParameterSet.Config as cms
process = cms.PSet()


process.generalSet = cms.PSet(
    defaultTree= cms.string('lbSpecificDecay'),
    plotTitle  = cms.string('impact parameter from proton in  #Lambda^{0}'),
    XaxisTitle = cms.string( '' ),
    XaxisMin   = cms.double( 0. ),
    XaxisMax   = cms.double( 0.2 ),
    YaxisTitle = cms.string( 'Normalized to background shape' ),
    YaxisMin   = cms.int32( 0 ), # if -1 is set, find the value automatically.
    YaxisMax   = cms.int32( 1 ), # if -1 is set, find the value automatically.
    isSideBand = cms.bool(False),
    isLogScale = cms.bool(False),
    outFormat  = cms.string('.png')
)
process.LegendSet = cms.PSet(
        Title = cms.string('Type:'),
        xLeft = cms.double( 0.70 ),
        xRight= cms.double( 0.88 ),
        yLeft = cms.double( 0.65 ),
        yRight= cms.double( 0.85 ),
        SetTransparent=cms.bool(True),
        )

process.plotSet = cms.VPSet(
    cms.PSet( NameInTree    = cms.string( 'fd180parSignalIPt_proton_inLam0' ),
              TitleName= cms.string( 'signal proton' ), # used in TLegend
              #ScaleFactor   = cms.double( 1./4565. ), # if -1 set, normalize automatically
              ScaleFactor   = cms.double( 1./190. ), # if -1 set, normalize automatically
              SetFill  = cms.bool( False ),
              LineWidth= cms.int32( 2 ),
              LineColor= cms.int32( 3 ),
              FillStyle= cms.int32( 0 ),
              FillColor= cms.int32( 0 ),
              ),
    cms.PSet( NameInTree    = cms.string( 'fd180parBackgroundIPt_proton_inLam0' ),
              TitleName= cms.string( 'background' ), # used in TLegend
              #ScaleFactor   = cms.double( 1./36599. ),
              ScaleFactor   = cms.double( 1./1851. ),
              SetFill  = cms.bool( False ),
              LineWidth= cms.int32( 2 ),
              LineColor= cms.int32( 2 ),
              FillStyle= cms.int32( 0 ),
              FillColor= cms.int32( 0 ),
              ),
)



