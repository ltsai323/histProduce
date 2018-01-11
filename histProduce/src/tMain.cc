#include "histProduce/histProduce/interface/tMain.h"


inline std::string treeMain::getFullName( const std::string& name )
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

// initialization of static member
std::vector< std::string > treeMain::nameReg;
std::vector<myCut::generalCutList*>* treeMain::_cutLists = nullptr;
