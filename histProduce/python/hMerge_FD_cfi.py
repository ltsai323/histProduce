import FWCore.ParameterSet.Config as cms
process = cms.PSet()


process.GeneralSet = cms.PSet(
    defaultTree= cms.string('lbSpecificDecay'),
    isSideBand = cms.bool(False),
    isLogScale = cms.bool(False),
    outFormat  = cms.string('.eps')
)
process.LegendSet = cms.PSet(
        Title = cms.string('Cut:'),
        xLeft = cms.double( 0.70 ),
        xRight= cms.double( 0.88 ),
        yLeft = cms.double( 0.65 ),
        yRight= cms.double( 0.85 ),
        SetTransparent=cms.bool( True ),
        )
process.MergedPlots = cms.VPSet(
    cms.PSet(
        MergedName = cms.string( 'massFakeLam0' ),
        SignalRegionMin = cms.double( 1.114),
        SignalRegionMax = cms.double( 1.118),
        XaxisName       = cms.string( 'GeV' ),
        XaxisMin        = cms.double( 1.1 ),
        XaxisMax        = cms.double( 1.15),
        YaxisName       = cms.string( 'Number' ),
        YaxisMin        = cms.double( 0. ),
        YaxisMax        = cms.double( -1.),
        ),
    cms.PSet(
        MergedName = cms.string( 'massFakeLbL0' ),
        SignalRegionMin = cms.double( 5.58),
        SignalRegionMax = cms.double( 5.63),
        XaxisName       = cms.string( 'GeV' ),
        XaxisMin        = cms.double( 5.4 ),
        XaxisMax        = cms.double( 5.8 ),
        YaxisName       = cms.string( 'Number' ),
        YaxisMin        = cms.double( 0. ),
        YaxisMax        = cms.double( -1.),
        ),
    cms.PSet(
        MergedName = cms.string( 'massFakeKs' ),
        SignalRegionMin = cms.double( 0.490 ),
        SignalRegionMax = cms.double( 0.505 ),
        XaxisName       = cms.string( 'GeV' ),
        XaxisMin        = cms.double( 0.3 ),
        XaxisMax        = cms.double( 0.8 ),
        YaxisName       = cms.string( 'Number' ),
        YaxisMin        = cms.double( 0. ),
        YaxisMax        = cms.double( -1.),
        ),
    )

process.SecondaryPlotSetting = cms.VPSet(
    cms.PSet( PreName   = cms.string( 'fd05' ),
              TitleName = cms.string( 'fd > 0.5 cm' ), # used in TLegend
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 1 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet( PreName   = cms.string( 'fd10' ),
              TitleName = cms.string( 'fd > 1.0 cm' ), # used in TLegend
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 2 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet( PreName   = cms.string( 'fd15' ),
              TitleName = cms.string( 'fd > 1.5 cm' ), # used in TLegend
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 3 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet( PreName   = cms.string( 'fd20' ),
              TitleName = cms.string( 'fd > 2.0 cm' ), # used in TLegend
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 4 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet( PreName   = cms.string( 'fd25' ),
              TitleName = cms.string( 'fd > 2.5 cm' ), # used in TLegend
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 5 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
)



