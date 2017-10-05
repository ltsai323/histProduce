import FWCore.ParameterSet.Config as cms
process = cms.PSet()


process.generalSet = cms.PSet(
    defaultTree= cms.string('lbSpecificDecay'),
    mergedPlots= cms.vstring(
        'massLbTk',
        'massTkTk',
        'massFakeBd',
        'massFakeBs',
        'massFakeBd_withCuts',
        'massFakeBs_withCuts',
        'massFakePhi1020',
        'massFakeK892',
        'massFakePiPi',
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
        FontSize= cms.int32( 18 ),
)

process.plotSet = cms.VPSet(
    cms.PSet( PreName   = cms.string( 'vtxprobFirst1' ),
              TitleName = cms.string( 'event choose 1 event' ), # used in TLegend
              SetNormalize= cms.bool( False ),
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 1 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet( PreName   = cms.string( 'vtxprobFirst2' ),
              TitleName = cms.string( 'event choose 2 event' ), # used in TLegend
              SetNormalize= cms.bool( False ),
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 2 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet( PreName   = cms.string( 'vtxprobFirst3' ),
              TitleName = cms.string( 'event choose 3 event' ), # used in TLegend
              SetNormalize= cms.bool( False ),
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 3 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet( PreName   = cms.string( 'vtxprobFirst4' ),
              TitleName = cms.string( 'event choose 4 event' ), # used in TLegend
              SetNormalize= cms.bool( False ),
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 4 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet( PreName   = cms.string( 'vtxprobFirst5' ),
              TitleName = cms.string( 'event choose 5 event' ), # used in TLegend
              SetNormalize= cms.bool( False ),
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 5 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet( PreName   = cms.string( 'vtxprobFirst6' ),
              TitleName = cms.string( 'event choose 6 event' ), # used in TLegend
              SetNormalize= cms.bool( False ),
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 6 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
)



