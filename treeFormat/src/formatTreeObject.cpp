#ifndef __FormatTreeObject_CPP__
#define __FormatTreeObject_CPP__

#include "histProduce/treeFormat/interface/formatTreeObject.hpp"
#include <stdexcept>
#include <exception>

template<typename T>
formatTreeObject<T>::formatTreeObject( int nVar ): formatTreeArray<T>( nVar, 1 ), _exdata(nullptr), cIdx(-1)
{ return; }

template<typename T>
formatTreeObject<T>::formatTreeObject( formatTreeArray<T>* exObj , int candIdx): formatTreeArray<T>( 0 ), _exdata(exObj), cIdx(candIdx)
{}

template<typename T>
formatTreeObject<T>::~formatTreeObject()
{}
template<typename T>
void formatTreeObject<T>::RegFormatTree(TTree* t)
{ throw std::bad_alloc("formatTreeObject::ReadyForInputTreeContent() : Error ! This object should not connect to a tree\n"); return; }
template<typename T>
void formatTreeObject<T>::LoadFormatSourceBranch(TTree* t)
{ throw std::bad_alloc("formatTreeObject::ReadyForOutputTreeContent() : Error ! This object should not connect to a tree\n"); return; }
template<typename T>
int formatTreeObject<T>::size() const { return 1; }
template<typename T>
T formatTreeObject<T>::var(int varEnum) const
{
    if ( cIdx == -1 ) { this->checkValidIndex(); return read[varEnum][this->currentCand()]; }
    return _exdata->read[varEnum][cIdx];
}


#endif
