import FWCore.ParameterSet.Config as cms
process = cms.PSet()


process.generalSet = cms.PSet(
    defaultTree= cms.string('lbSpecificDecay'),
    plotTitle  = cms.string('fake B^{0}_{s} impact parameter'),
    XaxisName  = cms.string( 'mm' ),
    XaxisMin   = cms.double( -0.2 ),
    XaxisMax   = cms.double(  0.2 ),
    YaxisName  = cms.string( 'Normalized to first bin' ),
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
        SetTransparent=cms.bool(True),
        )

process.plotSet = cms.VPSet(
    cms.PSet( NameInTree    = cms.string( 'par_ptk_IPFakeBd' ),
              TitleName     = cms.string( 'positive track in Bs' ), # used in TLegend
              ScaleFactor   = cms.double( 1./2754. ), # if -1 set, normalize automatically
              SetFill       = cms.bool( False ),
              LineWidth     = cms.int32( 2 ),
              LineColor     = cms.int32( 3 ),
              FillStyle     = cms.int32( 0 ),
              FillColor     = cms.int32( 0 ),
              ),
    cms.PSet( NameInTree    = cms.string( 'par_ntk_IPFakeBd' ),
              TitleName     = cms.string( 'negtive track in fakeBs' ), # used in TLegend
              ScaleFactor   = cms.double( 1./2757. ),
              SetFill       = cms.bool( False ),
              LineWidth     = cms.int32( 2 ),
              LineColor     = cms.int32( 2 ),
              FillStyle     = cms.int32( 0 ),
              FillColor     = cms.int32( 0 ),
              ),
    cms.PSet( NameInTree    = cms.string( 'par_ALL_IPFakeBd' ),
              TitleName     = cms.string( 'all tracks' ), # used in TLegend
              ScaleFactor   = cms.double( 1./25942. ),
              SetFill       = cms.bool( False ),
              LineWidth     = cms.int32( 2 ),
              LineColor     = cms.int32( 1 ),
              FillStyle     = cms.int32( 0 ),
              FillColor     = cms.int32( 0 ),
              ),
)



