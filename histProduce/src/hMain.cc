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
inline void histMain::fillHisto( const std::string& name, double value )
{
    std::map<std::string,TH1D*>::iterator iter = hMap.find( name );
    std::map<std::string,TH1D*>::iterator iend = hMap.end();
    if ( iter == iend ) return;
    iter->second->Fill( value );
    return;
}
void histMain::clearHisto()
{
    std::map<std::string,TH1D*>::iterator iter = hMap.begin();
    std::map<std::string,TH1D*>::iterator iend = hMap.end()  ;
    while ( iter != iend )
        iter++->second->Reset();
    return;
}


// initialization of static member
std::map<std::string, TH1D*> histMain::hMap;
std::vector<myCut::generalCutList*>* histMain::_cutLists = NULL;
