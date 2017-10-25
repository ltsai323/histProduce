import FWCore.ParameterSet.Config as cms
process = cms.PSet()


process.generalSet = cms.PSet(
    defaultTree= cms.string('lbSpecificDecay'),
    ComplementTitle = cms.string( ' using vtxprob sorting' ),
    mergedPlots = cms.VPSet(
        cms.PSet(
            MergedName = cms.string( 'massLbTk' ),
            SignalRegionMin = cms.double( 5.58 ),
            SignalRegionMax = cms.double( 5.64 ),
            ),
        cms.PSet(
            MergedName = cms.string( 'massTkTk' ),
            SignalRegionMin = cms.double( -1.),
            SignalRegionMax = cms.double( -1.),
            ),
        cms.PSet(
            MergedName = cms.string( 'massFakeBs_withCuts' ),
            SignalRegionMin = cms.double( 5.3 ),
            SignalRegionMax = cms.double( 5.44),
            ),
        cms.PSet(
            MergedName = cms.string( 'massFakeBd_withCuts' ),
            SignalRegionMin = cms.double( 5.22),
            SignalRegionMax = cms.double( 5.32),
            ),
        cms.PSet(
            MergedName = cms.string( 'massFakePhi1020' ),
            SignalRegionMin = cms.double( 1.01 ),
            SignalRegionMax = cms.double( 1.035),
            ),
        cms.PSet(
            MergedName = cms.string( 'massFakeK892' ),
            SignalRegionMin = cms.double( 0.85 ),
            SignalRegionMax = cms.double( 0.95 ),
            ),
        cms.PSet(
            MergedName = cms.string( 'massFakePiPi' ),
            SignalRegionMin = cms.double( -1. ),
            SignalRegionMax = cms.double( -1. ),
            ),
        ),
    XaxisName  = cms.string( 'GeV' ),
    XaxisMin   = cms.double( 5.0 ),
    XaxisMax   = cms.double( 6.0 ),
    YaxisName  = cms.string( 'number' ),
    YaxisMin   = cms.int32( 0 ), # if -1 is set, find the value automatically.
    YaxisMax   = cms.int32( -1 ), # if -1 is set, find the value automatically.
    isSideBand = cms.bool(False),
    isLogScale = cms.bool(False),
    outFormat  = cms.string('.eps')
)
process.LegendSet = cms.PSet(
        Title = cms.string('compare to each event chooses 6 candidates:'),
        xLeft = cms.double( 0.60 ),
        xRight= cms.double( 0.99 ),
        yLeft = cms.double( 0.60 ),
        yRight= cms.double( 0.89 ),
        SetTransparent = cms.bool( True ),
        )

process.plotSet = cms.VPSet(
    cms.PSet(
              PreNameTar= cms.string( 'vtxprobFirst1' ),
              PreNameObj= cms.string( 'vtxprobFirst6' ),
              TitleName = cms.string( 'each event chooses 1 candidate' ), # used in TLegend
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 1 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet(
              PreNameTar= cms.string( 'vtxprobFirst2' ),
              PreNameObj= cms.string( 'vtxprobFirst6' ),
              TitleName = cms.string( 'each event chooses 2 candidate' ), # used in TLegend
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 2 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet(
              PreNameTar= cms.string( 'vtxprobFirst3' ),
              PreNameObj= cms.string( 'vtxprobFirst6' ),
              TitleName = cms.string( 'each event chooses 3 candidate' ), # used in TLegend
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 3 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet(
              PreNameTar= cms.string( 'vtxprobFirst4' ),
              PreNameObj= cms.string( 'vtxprobFirst6' ),
              TitleName = cms.string( 'each event chooses 4 candidate' ), # used in TLegend
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 4 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet(
              PreNameTar= cms.string( 'vtxprobFirst5' ),
              PreNameObj= cms.string( 'vtxprobFirst6' ),
              TitleName = cms.string( 'each event chooses 5 candidate' ), # used in TLegend
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 5 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
)



