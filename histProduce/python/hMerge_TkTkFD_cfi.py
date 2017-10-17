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
        MergedName = cms.string( 'massLam0_Lam0' ),
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
        MergedName = cms.string( 'massTkTk_FakeLam0' ),
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
        MergedName = cms.string( 'massTkTk_FakeKshort' ),
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
    cms.PSet( PreName   = cms.string( 'fd030' ),
              TitleName = cms.string( 'fd > 0.3 cm' ), # used in TLegend
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 1 ),
              FillStyle = cms.int32( 3122 ),
              FillColor = cms.int32( 3 ),
              ),
    cms.PSet( PreName   = cms.string( 'fd060' ),
              TitleName = cms.string( 'fd > 0.6 cm' ), # used in TLegend
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 2 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet( PreName   = cms.string( 'fd039' ),
              TitleName = cms.string( 'fd > 0.9 cm' ), # used in TLegend
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 3 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet( PreName   = cms.string( 'fd120' ),
              TitleName = cms.string( 'fd > 1.2 cm' ), # used in TLegend
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 4 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet( PreName   = cms.string( 'fd150' ),
              TitleName = cms.string( 'fd > 1.5 cm' ), # used in TLegend
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 5 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet( PreName   = cms.string( 'fd180' ),
              TitleName = cms.string( 'fd > 1.8 cm' ), # used in TLegend
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 6 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet( PreName   = cms.string( 'fd210' ),
              TitleName = cms.string( 'fd > 2.1 cm' ), # used in TLegend
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 7 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet( PreName   = cms.string( 'fd240' ),
              TitleName = cms.string( 'fd > 2.4 cm' ), # used in TLegend
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 8 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
)



