#include "histProduce/histProduce/interface/hMainPV.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "math.h"


histMain_PV::histMain_PV( TFileDirectory* d ) :
    histMain( d, histMain::Label( "offlinePrimaryVertices", "", "RECO" ) )
{
    createHisto( "parPV_PVdistribution",  80, -0.8, 0.8,  80, -0.8, 0.8 );
}
void histMain_PV::Process( fwlite::Event* ev )
{
    try 
    {
        if ( !ev->isValid() ) return;
        fwlite::Handle< std::vector<reco::Vertex> > pvHandle;
        pvHandle.getByLabel( *ev, _label.module.c_str(), _label.label.c_str(), _label.process.c_str() );

        // pv not recorded, use BS
        fwlite::Handle< reco::BeamSpot > beamSpotHandle;
        beamSpotHandle.getByLabel( *ev,"offlineBeamSpot", "", "RECO"  );
        if ( !beamSpotHandle.isValid() ) return;
        if ( pvHandle->size()  == 0 ) return;
        const reco::Vertex bs( (*beamSpotHandle).position(), (*beamSpotHandle).covariance3D() );
        std::vector<reco::Vertex>::const_iterator iter = pvHandle->begin();
        std::vector<reco::Vertex>::const_iterator iend = pvHandle->end  ();
        while ( iter != iend )
        {
            const reco::Vertex& _pv = *iter++;
            fillHisto(  "parPV_PVdistribution", ( _pv.x()-bs.x() ), ( _pv.y()-bs.y() )  );
        }

    
    } catch (...) {}
}

void histMain_PV::Clear()
{
}
