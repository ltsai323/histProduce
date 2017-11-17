import FWCore.ParameterSet.Config as cms
process = cms.PSet()


process.generalSet = cms.PSet(
    defaultTree= cms.string('lbSpecificDecay'),
    plotTitle  = cms.string('#Lambda^{0} candidate with different reconstruction way'),
    XaxisTitle = cms.string( 'GeV' ),
    XaxisMin   = cms.double(  1.1 ),
    XaxisMax   = cms.double(  1.15),
    YaxisTitle = cms.string( 'Normalized to background shape' ),
    YaxisMin   = cms.int32( 0 ), # if -1 is set, find the value automatically.
    YaxisMax   = cms.int32( 6 ), # if -1 is set, find the value automatically.
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
    cms.PSet( NameInTree    = cms.string( 'massLam0_Lam0' ),
              TitleName= cms.string( 'form #Lambda^{0} vertex then find out mass directly' ), # used in TLegend
              ScaleFactor   = cms.double( 1./284. ), # if -1 set, normalize automatically
              SetFill  = cms.bool( False ),
              LineWidth= cms.int32( 4 ),
              LineColor= cms.int32( 1 ),
              FillStyle= cms.int32( 0 ),
              FillColor= cms.int32( 0 ),
              ),
    cms.PSet( NameInTree    = cms.string( 'massTkTk_FakeLam0' ),
              TitleName= cms.string( 'form pK vertex then reassign p #pi mass' ), # used in TLegend
              ScaleFactor   = cms.double( 1./135. ), # if -1 set, normalize automatically
              SetFill  = cms.bool( False ),
              LineWidth= cms.int32( 2 ),
              LineColor= cms.int32( 2 ),
              FillStyle= cms.int32( 0 ),
              FillColor= cms.int32( 0 ),
              ),
    cms.PSet( NameInTree    = cms.string( 'massLam0_FakeLam0' ),
              TitleName= cms.string( 'form #Lambda^{0} then reassign p #pi mass' ), # used in TLegend
              ScaleFactor   = cms.double( 1./284. ), # if -1 set, normalize automatically
              SetFill  = cms.bool( False ),
              LineWidth= cms.int32( 2 ),
              LineColor= cms.int32( 3 ),
              FillStyle= cms.int32( 0 ),
              FillColor= cms.int32( 0 ),
              ),
    cms.PSet( NameInTree    = cms.string( 'massKshort_FakeLam0' ),
              TitleName= cms.string( 'form K^{0}_{s} vertex then reassign p #pi mass' ), # used in TLegend
              ScaleFactor   = cms.double( 1./64. ), # if -1 set, normalize automatically
              SetFill  = cms.bool( False ),
              LineWidth= cms.int32( 2 ),
              LineColor= cms.int32( 4 ),
              FillStyle= cms.int32( 0 ),
              FillColor= cms.int32( 0 ),
              ),
)



