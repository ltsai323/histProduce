import FWCore.ParameterSet.Config as cms
process = cms.PSet()


process.generalSet = cms.PSet(
    defaultTree= cms.string('lbSpecificDecay'),
    mergedPlots = cms.VPSet(
        cms.PSet(
            MergedName = cms.string( 'massLbTk' ),
            SRegionMin = cms.double( 5.58 ),
            SRegionMax = cms.double( 5.64 ),
            ),
        cms.PSet(
            MergedName = cms.string( 'massTkTk' ),
            SRegionMin = cms.double( -1.),
            SRegionMax = cms.double( -1.),
            ),
        cms.PSet(
            MergedName = cms.string( 'massFakeBs_withCuts' ),
            SRegionMin = cms.double( 5.3 ),
            SRegionMax = cms.double( 5.44),
            ),
        cms.PSet(
            MergedName = cms.string( 'massFakeBd_withCuts' ),
            SRegionMin = cms.double( 5.22),
            SRegionMax = cms.double( 5.32),
            ),
        cms.PSet(
            MergedName = cms.string( 'massFakeBs' ),
            SRegionMin = cms.double( 5.3 ),
            SRegionMax = cms.double( 5.44),
            ),
        cms.PSet(
            MergedName = cms.string( 'massFakeBd' ),
            SRegionMin = cms.double( 5.22),
            SRegionMax = cms.double( 5.32),
            ),
        cms.PSet(
            MergedName = cms.string( 'massFakePhi1020' ),
            SRegionMin = cms.double( 1.01 ),
            SRegionMax = cms.double( 1.035),
            ),
        cms.PSet(
            MergedName = cms.string( 'massFakeK892' ),
            SRegionMin = cms.double( 0.85 ),
            SRegionMax = cms.double( 0.95 ),
            ),
        cms.PSet(
            MergedName = cms.string( 'massFakePiPi' ),
            SRegionMin = cms.double( -1. ),
            SRegionMax = cms.double( -1. ),
            ),
        ),
    XaxisTitle = cms.string( 'GeV' ),
    XaxisMin   = cms.double( 5.0 ),
    XaxisMax   = cms.double( 6.0 ),
    YaxisTitle = cms.string( 'number' ),
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



