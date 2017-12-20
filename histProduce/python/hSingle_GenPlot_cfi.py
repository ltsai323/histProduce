import FWCore.ParameterSet.Config as cms

process = cms.PSet()

process.generalSet = cms.PSet(
    defaultTree= cms.string('lbSpecificDecay'),
    isSideBand = cms.bool(False),
    isLogScale = cms.bool(False),
    outFormat  = cms.string('.pdf')
)
process.plotSet = cms.VPSet(
    cms.PSet(
              NameInTree = cms.string( 'parJPsiGen_momentumDiffBetweenPairs' ),
              TitleName  = cms.string( 'mom diff of Muon' ), # new title of the plot
              SetFill    = cms.bool( True ),
              LineWidth  = cms.int32( 2 ),
              LineColor  = cms.int32( 2 ),
              FillStyle  = cms.int32( 0 ),
              FillColor  = cms.int32( 0 ),
              XaxisTitle = cms.string( 'GeV' ),
              XaxisMin   = cms.double( -5.0 ),
              XaxisMax   = cms.double(  5.0 ),
              YaxisTitle = cms.string( 'Number' ),
              YaxisMin   = cms.int32(  0 ), # if -1 is set, find the value automatically.
              YaxisMax   = cms.int32( -1 ), # if -1 is set, find the value automatically.
              LeftLineX  = cms.double( -999. ),
              RightLineX = cms.double( -999. ),
          ),
    cms.PSet(
              NameInTree = cms.string( 'parTkTkGen_momentumDiffBetweenPairs' ),
              TitleName  = cms.string( 'mom diff of All tracks' ), # new title of the plot
              SetFill    = cms.bool( True ),
              LineWidth  = cms.int32( 2 ),
              LineColor  = cms.int32( 2 ),
              FillStyle  = cms.int32( 0 ),
              FillColor  = cms.int32( 0 ),
              XaxisTitle = cms.string( 'GeV' ),
              XaxisMin   = cms.double( -5.0 ),
              XaxisMax   = cms.double(  5.0 ),
              YaxisTitle = cms.string( 'Number' ),
              YaxisMin   = cms.int32(  0 ), # if -1 is set, find the value automatically.
              YaxisMax   = cms.int32( -1 ), # if -1 is set, find the value automatically.
              LeftLineX  = cms.double( -999. ),
              RightLineX = cms.double( -999. ),
          ),
    cms.PSet(
              NameInTree = cms.string( 'parTkTkGen_momentumDiffProton' ),
              TitleName  = cms.string( 'mom diff of PROTON' ), # new title of the plot
              SetFill    = cms.bool( True ),
              LineWidth  = cms.int32( 2 ),
              LineColor  = cms.int32( 2 ),
              FillStyle  = cms.int32( 0 ),
              FillColor  = cms.int32( 0 ),
              XaxisTitle = cms.string( 'GeV' ),
              XaxisMin   = cms.double( -5.0 ),
              XaxisMax   = cms.double(  5.0 ),
              YaxisTitle = cms.string( 'Number' ),
              YaxisMin   = cms.int32(  0 ), # if -1 is set, find the value automatically.
              YaxisMax   = cms.int32( -1 ), # if -1 is set, find the value automatically.
              LeftLineX  = cms.double( -999. ),
              RightLineX = cms.double( -999. ),
          ),
    cms.PSet(
              NameInTree = cms.string( 'parTkTkGen_momentumDiffKaon' ),
              TitleName  = cms.string( 'mom diff of KAON' ), # new title of the plot
              SetFill    = cms.bool( True ),
              LineWidth  = cms.int32( 2 ),
              LineColor  = cms.int32( 2 ),
              FillStyle  = cms.int32( 0 ),
              FillColor  = cms.int32( 0 ),
              XaxisTitle = cms.string( 'GeV' ),
              XaxisMin   = cms.double( -5.0 ),
              XaxisMax   = cms.double(  5.0 ),
              YaxisTitle = cms.string( 'Number' ),
              YaxisMin   = cms.int32(  0 ), # if -1 is set, find the value automatically.
              YaxisMax   = cms.int32( -1 ), # if -1 is set, find the value automatically.
              LeftLineX  = cms.double( -999. ),
              RightLineX = cms.double( -999. ),
          ),
    cms.PSet(
              NameInTree = cms.string( 'parTkTkGen_minDeltaR' ),
              TitleName  = cms.string( 'min value of #Delta R in each event' ), # new title of the plot
              SetFill    = cms.bool( False ),
              LineWidth  = cms.int32( 2 ),
              LineColor  = cms.int32( 2 ),
              FillStyle  = cms.int32( 0 ),
              FillColor  = cms.int32( 0 ),
              XaxisTitle = cms.string( '' ),
              XaxisMin   = cms.double( 0.0 ),
              XaxisMax   = cms.double( 1.0 ),
              YaxisTitle = cms.string( 'Number' ),
              YaxisMin   = cms.int32(  0 ), # if -1 is set, find the value automatically.
              YaxisMax   = cms.int32( -1 ), # if -1 is set, find the value automatically.
              LeftLineX  = cms.double( -999. ),
              RightLineX = cms.double( -999. ),
          ),

    )
