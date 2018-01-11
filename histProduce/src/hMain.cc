#include "histProduce/histProduce/interface/hMain.h"

inline void histMain::createHisto( const std::string& name, int nbin, double min, double max )
{
    if ( hMap.find( name ) != hMap.end() )
    {
        printf ( "histogram recreated! please check the createHisto function\n" );
        return;
    }
    hMap[name] = dir->make<TH1D>( getFullName(name).c_str(), name.c_str(), nbin, min, max );
    std::cout << "appened name :" << getFullName( name ) << std::endl;
    return;
}
inline void histMain::createHisto( const std::string& name, int nbin, double min, double max, int nbin2, double min2, double max2 )
{
    if ( hMap2D.find( name ) != hMap2D.end() )
    {
        printf ( "2D-histogram recreated! please check the createHisto function\n" );
        return;
    }
    hMap2D[name] = dir->make<TH2D>( getFullName(name).c_str(), name.c_str(), nbin, min, max, nbin2, min2, max2 );
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

inline std::string histMain::getFullName( const std::string& name )
{ return (preName+"_"+name); }
void histMain::regName( const std::string& _preName )
{
    std::vector< std::string >::const_iterator iter = nameReg.begin();
    std::vector< std::string >::const_iterator iend = nameReg.end  ();
    bool tag = true;
    while ( iter != iend )
    {
        const std::string& storedName = *iter++;
        if ( storedName == _preName )
            tag = false;
    }
    if ( tag )
        nameReg.push_back( _preName );
    else
    { std::cerr << "warning! there are two plots with the same pre name: " << _preName << std::endl; throw; }

    return;

}

// initialization of static member
std::vector< std::string > histMain::nameReg;
std::vector<myCut::generalCutList*>* histMain::_cutLists = nullptr;
