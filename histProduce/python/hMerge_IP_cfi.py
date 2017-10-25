import FWCore.ParameterSet.Config as cms
process = cms.PSet()


process.generalSet = cms.PSet(
    defaultTree= cms.string('lbSpecificDecay'),
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
            MergedName = cms.string( 'massFakeBs' ),
            SignalRegionMin = cms.double( 5.3 ),
            SignalRegionMax = cms.double( 5.44),
            ),
        cms.PSet(
            MergedName = cms.string( 'massFakeBd' ),
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
        Title = cms.string('Cut:'),
        xLeft = cms.double( 0.70 ),
        xRight= cms.double( 0.88 ),
        yLeft = cms.double( 0.65 ),
        yRight= cms.double( 0.85 ),
)

process.plotSet = cms.VPSet(
    cms.PSet( PreName   = cms.string( 'IPt0S_both_' ),
              TitleName = cms.string( 'IP > 0 #sigma' ), # used in TLegend
              SetNormalize= cms.bool( False ),
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 1 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet( PreName   = cms.string( 'IPt1S_both_' ),
              TitleName = cms.string( 'IP > 1 #sigma' ), # used in TLegend
              SetNormalize= cms.bool( False ),
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 2 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet( PreName   = cms.string( 'IPt2S_both_' ),
              TitleName = cms.string( 'IP > 2 #sigma' ), # used in TLegend
              SetNormalize= cms.bool( False ),
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 3 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet( PreName   = cms.string( 'IPt3S_both_' ),
              TitleName = cms.string( 'IP > 3 #sigma' ), # used in TLegend
              SetNormalize= cms.bool( False ),
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 4 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet( PreName   = cms.string( 'IPt4S_both_' ),
              TitleName = cms.string( 'IP > 4 #sigma' ), # used in TLegend
              SetNormalize= cms.bool( False ),
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 6 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
)



