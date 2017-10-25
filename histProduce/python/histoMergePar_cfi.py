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
process.aaMergedPlots = cms.VPSet(
    cms.PSet(
        MergedName = cms.string( 'massLbTk' ),
        SignalRegionMin = cms.double( 5.58 ),
        SignalRegionMax = cms.double( 5.64 ),
        XaxisName       = cms.string( 'GeV' ),
        XaxisMin        = cms.double( 5.0 ),
        XaxisMax        = cms.double( 6.0 ),
        YaxisName       = cms.string( 'Number' ),
        YaxisMin        = cms.double( 0. ),
        YaxisMax        = cms.double( -1.),
        ),
    cms.PSet(
        MergedName = cms.string( 'massTkTk' ),
        SignalRegionMin = cms.double( -1.),
        SignalRegionMax = cms.double( -1.),
        XaxisName       = cms.string( 'GeV' ),
        XaxisMin        = cms.double( 1.3 ),
        XaxisMax        = cms.double( 2.2 ),
        YaxisName       = cms.string( 'Number' ),
        YaxisMin        = cms.double( 0. ),
        YaxisMax        = cms.double( -1.),
        ),
    cms.PSet(
        MergedName = cms.string( 'massFakeBs_withCuts' ),
        SignalRegionMin = cms.double( 5.3 ),
        SignalRegionMax = cms.double( 5.44),
        XaxisName       = cms.string( 'GeV' ),
        XaxisMin        = cms.double( 5.0 ),
        XaxisMax        = cms.double( 6.0 ),
        YaxisName       = cms.string( 'Number' ),
        YaxisMin        = cms.double( 0. ),
        YaxisMax        = cms.double( -1.),
        ),
    cms.PSet(
        MergedName = cms.string( 'massFakeBd_withCuts' ),
        SignalRegionMin = cms.double( 5.22),
        SignalRegionMax = cms.double( 5.32),
        XaxisName       = cms.string( 'GeV' ),
        XaxisMin        = cms.double( 5.0 ),
        XaxisMax        = cms.double( 6.0 ),
        YaxisName       = cms.string( 'Number' ),
        YaxisMin        = cms.double( 0. ),
        YaxisMax        = cms.double( -1.),
        ),
    cms.PSet(
        MergedName = cms.string( 'massFakeBs' ),
        SignalRegionMin = cms.double( 5.3 ),
        SignalRegionMax = cms.double( 5.44),
        XaxisName       = cms.string( 'GeV' ),
        XaxisMin        = cms.double( 5.0 ),
        XaxisMax        = cms.double( 6.0 ),
        YaxisName       = cms.string( 'Number' ),
        YaxisMin        = cms.double( 0. ),
        YaxisMax        = cms.double( -1.),
        ),
    cms.PSet(
        MergedName = cms.string( 'massFakeBd' ),
        SignalRegionMin = cms.double( 5.22),
        SignalRegionMax = cms.double( 5.32),
        XaxisName       = cms.string( 'GeV' ),
        XaxisMin        = cms.double( 5.0 ),
        XaxisMax        = cms.double( 6.0 ),
        YaxisName       = cms.string( 'Number' ),
        YaxisMin        = cms.double( 0. ),
        YaxisMax        = cms.double( -1.),
        ),
    cms.PSet(
        MergedName = cms.string( 'massFakePhi1020' ),
        SignalRegionMin = cms.double( 1.01 ),
        SignalRegionMax = cms.double( 1.035),
        XaxisName       = cms.string( 'GeV' ),
        XaxisMin        = cms.double( 0.90 ),
        XaxisMax        = cms.double( 1.10 ),
        YaxisName       = cms.string( 'Number' ),
        YaxisMin        = cms.double( 0. ),
        YaxisMax        = cms.double( -1.),
        ),
    cms.PSet(
        MergedName = cms.string( 'massFakeK892' ),
        SignalRegionMin = cms.double( 0.85 ),
        SignalRegionMax = cms.double( 0.95 ),
        XaxisName       = cms.string( 'GeV' ),
        XaxisMin        = cms.double( 0.8 ),
        XaxisMax        = cms.double( 1.0 ),
        YaxisName       = cms.string( 'Number' ),
        YaxisMin        = cms.double( 0. ),
        YaxisMax        = cms.double( -1.),
        ),
    cms.PSet(
        MergedName = cms.string( 'massFakePiPi' ),
        SignalRegionMin = cms.double( -1. ),
        SignalRegionMax = cms.double( -1. ),
        XaxisName       = cms.string( 'GeV' ),
        XaxisMin        = cms.double( 0.2 ),
        XaxisMax        = cms.double( 1.3 ),
        YaxisName       = cms.string( 'Number' ),
        YaxisMin        = cms.double( 0. ),
        YaxisMax        = cms.double( -1.),
        ),
    )

process.SecondaryPlotSetting = cms.VPSet(
    cms.PSet( PreName   = cms.string( 'fd1S' ),
              TitleName = cms.string( 'fd > 1 #sigma' ), # used in TLegend
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 1 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet( PreName   = cms.string( 'fd2S' ),
              TitleName = cms.string( 'fd > 2 #sigma' ), # used in TLegend
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 2 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet( PreName   = cms.string( 'fd3S' ),
              TitleName = cms.string( 'fd > 3 #sigma' ), # used in TLegend
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 3 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet( PreName   = cms.string( 'fd4S' ),
              TitleName = cms.string( 'fd > 4 #sigma' ), # used in TLegend
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 4 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet( PreName   = cms.string( 'fd5S' ),
              TitleName = cms.string( 'fd > 5 #sigma' ), # used in TLegend
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 6 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
)



