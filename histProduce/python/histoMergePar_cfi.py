import FWCore.ParameterSet.Config as cms
process = cms.PSet()


process.generalSet = cms.PSet(
    defaultTree= cms.string('lbSpecificDecay'),
    mergedPlots= cms.vstring(
        'massLbTk',
        'massFakeBd_withCuts',
        'massFakeBs_withCuts',
        'massFakePhi1020',
        'massFakeK892',
        ),
    TitleName  = cms.string('merged plot'),
    XaxisName  = cms.string( 'GeV' ),
    XaxisMin   = cms.double( 5.0 ),
    XaxisMax   = cms.double( 6.0 ),
    YaxisMin   = cms.int32( 0 ), # if -1 is set, find the value automatically.
    YaxisMax   = cms.int32( -1 ), # if -1 is set, find the value automatically.
    isSideBand = cms.bool(False),
    isLogScale = cms.bool(False),
    outFormat  = cms.string('.png')
)
process.LegendSet = cms.PSet(
        Title = cms.string('Cut:'),
        xLeft = cms.double( 0.70 ),
        xRight= cms.double( 0.88 ),
        yLeft = cms.double( 0.65 ),
        yRight= cms.double( 0.85 ),
        )

process.plotSet = cms.VPSet(
    cms.PSet( PreName   = cms.string( 'fd020' ),
              TitleName = cms.string( 'fd > 0.2  mm' ), # used in TLegend
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 1 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet( PreName   = cms.string( 'fd025' ),
              TitleName = cms.string( 'fd > 0.25 mm' ), # used in TLegend
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 2 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet( PreName   = cms.string( 'fd030' ),
              TitleName = cms.string( 'fd > 0.3  mm' ), # used in TLegend
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 3 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet( PreName   = cms.string( 'fd035' ),
              TitleName = cms.string( 'fd > 0.35 mm' ), # used in TLegend
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 4 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet( PreName   = cms.string( 'fd040' ),
              TitleName = cms.string( 'fd > #sigma ' ), # used in TLegend
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 6 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
)



