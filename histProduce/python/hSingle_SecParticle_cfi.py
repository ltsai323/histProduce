import FWCore.ParameterSet.Config as cms

process = cms.PSet()

process.generalSet = cms.PSet(
    defaultTree= cms.string('lbSpecificDecay'),
    isSideBand = cms.bool(False),
    isLogScale = cms.bool(False),
    outFormat  = cms.string('.eps')
)
process.plotSet = cms.VPSet(
    cms.PSet(
              NameInTree = cms.string( 'massFakeBd' ),
              TitleName  = cms.string( 'fake B_{d} -> J/#psi + K^{+} + p^{-}' ), # new title of the plot
              SetFill    = cms.bool( True ),
              LineWidth  = cms.int32( 2 ),
              LineColor  = cms.int32( 2 ),
              FillStyle  = cms.int32( 0 ),
              FillColor  = cms.int32( 0 ),
              XaxisTitle = cms.string( 'GeV' ),
              XaxisMin   = cms.double( 5.0 ),
              XaxisMax   = cms.double( 6.0 ),
              YaxisTitle = cms.string( 'Number' ),
              YaxisMin   = cms.int32(  0 ), # if -1 is set, find the value automatically.
              YaxisMax   = cms.int32( -1 ), # if -1 is set, find the value automatically.
              LeftLineX  = cms.double(5.279 - 0.04),
              RightLineX = cms.double(5.279 + 0.04),
          ),

    )
