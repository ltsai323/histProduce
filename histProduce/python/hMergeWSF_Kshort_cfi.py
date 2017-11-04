import FWCore.ParameterSet.Config as cms
process = cms.PSet()


process.generalSet = cms.PSet(
    defaultTree= cms.string('lbSpecificDecay'),
    plotTitle  = cms.string('Kshort candidate with different reconstruction way'),
    XaxisName  = cms.string( 'GeV' ),
    XaxisMin   = cms.double(  0.4 ),
    XaxisMax   = cms.double(  0.6 ),
    YaxisName  = cms.string( 'Normalized to background shape' ),
    YaxisMin   = cms.int32( 0 ), # if -1 is set, find the value automatically.
    YaxisMax   = cms.int32( 3 ), # if -1 is set, find the value automatically.
    isSideBand = cms.bool(False),
    isLogScale = cms.bool(False),
    outFormat  = cms.string('.eps')
)
process.LegendSet = cms.PSet(
        Title = cms.string('the reconstruction ways:'),
        xLeft = cms.double( 0.58 ),
        xRight= cms.double( 0.88 ),
        yLeft = cms.double( 0.55 ),
        yRight= cms.double( 0.85 ),
        SetTransparent=cms.bool(True),
        )

process.plotSet = cms.VPSet(
    cms.PSet( NameInTree    = cms.string( 'massKshort_Kshort' ),
              TitleName     = cms.string( 'form K^{0}_{s} vertex then find out mass directly' ), # used in TLegend
              ScaleFactor   = cms.double( 1./7017. ), # if -1 set, normalize automatically
              SetFill       = cms.bool( False ),
              LineWidth     = cms.int32( 4 ),
              LineColor     = cms.int32( 1 ),
              FillStyle     = cms.int32( 0 ),
              FillColor     = cms.int32( 0 ),
              ),
    cms.PSet( NameInTree    = cms.string( 'massTkTk_FakeKshort' ),
              TitleName     = cms.string( 'form pK vertex then reassign #pi #pi mass' ), # used in TLegend
              ScaleFactor   = cms.double( 1./8174. ), # if -1 set, normalize automatically
              SetFill       = cms.bool( False ),
              LineWidth     = cms.int32( 2 ),
              LineColor     = cms.int32( 2 ),
              FillStyle     = cms.int32( 0 ),
              FillColor     = cms.int32( 0 ),
              ),
    cms.PSet( NameInTree    = cms.string( 'massLam0_FakeKshort' ),
              TitleName     = cms.string( 'form #Lambda^{0} then reassign #pi #pi mass' ), # used in TLegend
              ScaleFactor   = cms.double( 1./1897. ), # if -1 set, normalize automatically
              SetFill       = cms.bool( False ),
              LineWidth     = cms.int32( 2 ),
              LineColor     = cms.int32( 3 ),
              FillStyle     = cms.int32( 0 ),
              FillColor     = cms.int32( 0 ),
              ),
    cms.PSet( NameInTree    = cms.string( 'massKshort_FakeKshort' ),
              TitleName     = cms.string( 'form K^{0}_{s} vertex then reassign #pi #pi mass' ), # used in TLegend
              ScaleFactor   = cms.double( 1./6941. ), # if -1 set, normalize automatically
              SetFill       = cms.bool( False ),
              LineWidth     = cms.int32( 2 ),
              LineColor     = cms.int32( 4 ),
              FillStyle     = cms.int32( 0 ),
              FillColor     = cms.int32( 0 ),
              ),
)



