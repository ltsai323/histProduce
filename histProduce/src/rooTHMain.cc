#include "histProduce/histProduce/interface/rooTHMain.h"
#include <iostream>

root_TreeHistoMain::root_TreeHistoMain( TFileDirectory* d,  const std::string& pName ) : dir(d), preName( pName )
{
    if ( d == nullptr )
    {
        setOutputAlive( false );
        return;
    }
    setOutputAlive( true );
    if ( preName.empty() ) return;
    regName( preName );
    outputTree = dir->make<TTree>( preName.c_str(), preName.c_str() );
}
root_TreeHistoMain::~root_TreeHistoMain()
{
    // do something
}

void root_TreeHistoMain::LoopEvents( unsigned& maxEvents )
{
    if ( NoOutput() ) return;
    unsigned i = 0;
    unsigned N = readTree()->GetEntries();
    std::cout << " this tree owns " << N << " events\n";
    while ( i != N )
    {
        //if ( maxEvents-- == 0 ) return;
        Process( i++ );
    }
    return;
}

inline bool root_TreeHistoMain::NoOutput() const
{ return noOutputFile; }
void root_TreeHistoMain::ResetInputTreeName( const std::string& name )
{ inputTreeName = name; return; }


inline void root_TreeHistoMain::createHisto( const std::string& name, int nbin, double min, double max )
{
    if ( hMap.find( name ) != hMap.end() )
    {
        printf ( "histogram recreated! please check the createHisto function\n" );
        return;
    }
    hMap[name] = dir->make<TH1D>( getFullName(name).c_str(), name.c_str(), nbin, min, max );
    printf( "appened histogram name : %s\n", getFullName(name).c_str() );
    return;
}
inline void root_TreeHistoMain::createHisto( const std::string& name, int nbin, double min, double max, int nbin2, double min2, double max2 )
{
    if ( hMap2D.find( name ) != hMap2D.end() )
    {
        printf ( "2D-histogram recreated! please check the createHisto function\n" );
        return;
    }
    hMap2D[name] = dir->make<TH2D>( getFullName(name).c_str(), name.c_str(), nbin, min, max, nbin2, min2, max2 );
    return;
}
inline void root_TreeHistoMain::fillHisto( const std::string& name, double value )
{
    std::map<std::string,TH1D*>::iterator iter = hMap.find( name );
    std::map<std::string,TH1D*>::iterator iend = hMap.end();
    if ( iter == iend ) return;
    iter->second->Fill( value );
    return;
}
inline void root_TreeHistoMain::fillHisto( const std::string& name, double valueX, double valueY )
{
    std::map<std::string,TH2D*>::iterator iter = hMap2D.find( name );
    std::map<std::string,TH2D*>::iterator iend = hMap2D.end();
    if ( iter == iend ) return;
    iter->second->Fill( valueX, valueY );
    return;
}
const std::map<std::string,TH1D*>& root_TreeHistoMain::getHistos() const
{ return hMap; }
const std::map<std::string,TH2D*>& root_TreeHistoMain::getHistos2D() const
{ return hMap2D; }

std::string root_TreeHistoMain::getFullName( const std::string& name ) const
{ return (preName+"_"+name); }
void root_TreeHistoMain::regName( const std::string& _preName )
{
    std::vector< std::string >::const_iterator iter = nameReg.begin();
    std::vector< std::string >::const_iterator iend = nameReg.end  ();
    bool tag = true;
    while ( iter != iend )
    {
        const std::string& storedpName = *iter++;
        if ( storedpName == _preName )
            tag = false;
    }
    if ( tag )
        nameReg.push_back( _preName );
    else
    { std::cerr << "warning! there are two plots with the same pre name: " << _preName << std::endl; throw; }

    return;
}
bool root_TreeHistoMain::SetInputFile( TFile* inputFile )
{
    if ( !inputFile ) return false;
    if ( inputFile->IsZombie() ) return false;
    inputTree = nullptr;
    inputTree = (TTree*) inputFile->Get( inputTreeName.c_str() );
    if ( inputTree == nullptr )
        return false;

    LoadSourceBranch();
    return true;
}
//bool root_TreeHistoMain::SetInputFile( const std::vector<const char*>
void root_TreeHistoMain::setInputTreeName( const std::string& name )
{ inputTreeName = name; return; }
void root_TreeHistoMain::setOutputAlive( bool tag )
{ noOutputFile = !tag; }




// initialization of static member
TFile* inputFile = nullptr;
std::vector< std::string > root_TreeHistoMain::nameReg;
std::vector<myCut::generalCutList*>* root_TreeHistoMain::_cutLists = nullptr;
