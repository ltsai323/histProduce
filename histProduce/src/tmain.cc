#include "histProduce/histProduce/interface/tmain.h"
#include <iostream>

treeMain::treeMain( TFileDirectory* d, Label l, const std::string& pName ) : _label( l ), dir( d ), preName( pName )
{
    regName( preName );
    _storageTree = dir->make<TTree>( preName.c_str(), preName.c_str() );
}
treeMain::~treeMain()
{
    // do something
}

std::string treeMain::getFullName( const std::string& name )
{ return (preName+"_"+name); }
void treeMain::regName( const std::string& _preName )
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
void treeMain::getByLabel_Cand(fwlite::Event* ev)
{ _handle.getByLabel( *ev, _label.module.c_str(), _label.label.c_str(), _label.process.c_str() ); return; }

TH1D* treeMain::createHisto( const std::string& name, int nbin, double xmin, double xmax )
{ return dir->make<TH1D>( name.c_str(), name.c_str(), nbin, xmin, xmax ); }
TH2D* treeMain::createHisto( const std::string& name, int nbin, double xmin, double xmax, int mbin, double ymin, double ymax )
{ return dir->make<TH2D>( name.c_str(), name.c_str(), nbin, xmin, xmax, mbin, ymin, ymax ); }

// initialization of static member
std::vector< std::string > treeMain::nameReg;
std::vector<myCut::generalCutList*>* treeMain::_cutLists = nullptr;
