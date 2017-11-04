#include "histProduce/histProduce/interface/hMain.h"

inline void histMain::createHisto( const std::string& name, int nbin, double min, double max )
{
    if ( hMap.find( name ) != hMap.end() )
    {
        printf ( "histogram recreated! please check the createHisto function\n" );
        return;
    }
    hMap[name] = dir->make<TH1D>( name.c_str(), name.c_str(), nbin, min, max );
    return;
}
inline void histMain::createHisto( const std::string& name, int nbin, double min, double max, int nbin2, double min2, double max2 )
{
    if ( hMap2D.find( name ) != hMap2D.end() )
    {
        printf ( "2D-histogram recreated! please check the createHisto function\n" );
        return;
    }
    hMap2D[name] = dir->make<TH2D>( name.c_str(), name.c_str(), nbin, min, max, nbin2, min2, max2 );
    return;
}
inline void histMain::fillHisto( const std::string& name, double value )
{
    std::map<std::string,TH1D*>::iterator iter = hMap.find( name );
    std::map<std::string,TH1D*>::iterator iend = hMap.end();
    if ( iter == iend ) return;
    iter->second->Fill( value );
    return;
}
inline void histMain::fillHisto( const std::string& name, double valueX, double valueY )
{
    std::map<std::string,TH2D*>::iterator iter = hMap2D.find( name );
    std::map<std::string,TH2D*>::iterator iend = hMap2D.end();
    if ( iter == iend ) return;
    iter->second->Fill( valueX, valueY );
    return;
}
void histMain::clearHisto()
{
    for ( const auto& histoPair : hMap )
        histoPair.second->Reset();
    for ( const auto& histo2DPair : hMap2D )
        histo2DPair.second->Reset();
    return;
}


// initialization of static member
std::map<std::string, TH1D*> histMain::hMap;
std::map<std::string, TH2D*> histMain::hMap2D;
std::vector<myCut::generalCutList*>* histMain::_cutLists = NULL;
