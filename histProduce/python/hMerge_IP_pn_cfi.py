import FWCore.ParameterSet.Config as cms
process = cms.PSet()


process.generalSet = cms.PSet(
    defaultTree= cms.string('lbSpecificDecay'),
    mergedPlots= cms.vstring(
        #"IPFakeBs",
        "IPFakeBd",
        ),
    XaxisTitle = cms.string( 'GeV' ),
    XaxisMin   = cms.double( 6.0 ),
    XaxisMax   = cms.double( 6.0 ),
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
    cms.PSet( PreName   = cms.string( 'par_ptk_' ),
              TitleName = cms.string( 'pTrack' ), # used in TLegend
              ScaleFactor= cms.double( 1 ),
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 1 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet( PreName   = cms.string( 'par_ntk_' ),
              TitleName = cms.string( 'nTrack' ), # used in TLegend
              ScaleFactor= cms.double( 1 ),
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 2 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
    cms.PSet( PreName   = cms.string( 'par_ALL_' ),
              TitleName = cms.string( 'ALL' ), # used in TLegend
              ScaleFactor= cms.double( 4271./20832.),
              SetFill   = cms.bool( False ),
              LineWidth = cms.int32( 2 ),
              LineColor = cms.int32( 3 ),
              FillStyle = cms.int32( 0 ),
              FillColor = cms.int32( 0 ),
              ),
)
