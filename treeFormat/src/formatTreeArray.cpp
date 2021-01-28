#ifndef FORMATTREEARRAY_CC
#define FORMATTREEARRAY_CC

#include "histProduce/treeFormat/interface/formatTreeArray.hpp"
#include <stdexcept>
#include <exception>

template<typename T>
formatTreeArray<T>::formatTreeArray( int nVar, int maxCSize ):
    data(nullptr), read(nullptr), candSize(0),
    _rSwitch(false),_wSwitch(false) ,
    _allVar(nVar),
    _maxCandSize(maxCSize)
{ return; }

template<typename T>
formatTreeArray<T>::~formatTreeArray()
{
    if ( _rSwitch )
    { delete[] read; read=nullptr; }
    if ( _wSwitch )
    { for ( int i=0; i<_allVar; ++i ) delete[] data[i]; delete[] data; data = nullptr; }
    return;
}
template<typename T>
void formatTreeArray<T>::Clear()
{
    if ( _wSwitch )
    {
        candSize = 0;
        for ( int i=0; i<_allVar; ++i )
            if ( data[i] )
                memset( data[i], 0x00, _maxCandSize*sizeof(T) );
    }

    return;
}
template<typename T>
void formatTreeArray<T>::LoadFormatSourceBranch(TTree* t)
{ throw std::bad_function_call(); return; }
//{ throw std::bad_function_call("formatTreeArray::LoadFormatSourceBranch() : no function definition. This function needs to be overriden."); return; }
template<typename T>
void formatTreeArray<T>::LoadEvtContent(TreeReader& evtInfo)
{ throw std::bad_function_call(); return; }
//{ throw std::bad_function_call("formatTreeArray::LoadEvt() : no function definition. This function needs to be overriden."); return; }
template<typename T>
void formatTreeArray<T>::ReadyForInputTreeContent()
{
    //if ( _wSwitch ) throw std::logic_error("formatTreeArray::ReadForInputTreeContent() : Error! You have setuped the object in WRITE mode, you cannot set this in write and read mode at the same time\n");
    _candIdx = -1;
    if ( _rSwitch ) return;
    _rSwitch = true;
    read = new T*[_allVar];
    return;
}
template<typename T>
void formatTreeArray<T>::ReadyForOutputTreeContent()
{
    _candIdx = -1;
    //if ( _rSwitch ) throw std::logic_error("formatTreeArray::ReadForOutputTreeContent() : Error! You have setuped the object in READ mode, you cannot set this in write and read mode at the same time\n");
    if ( _wSwitch ) return;
    _wSwitch = true;
    data = new T*[_allVar];
    for ( int i=0; i<_allVar; ++i )
        data[i] = new T[_maxCandSize];
    return;
}
template<typename T>
formatTreeArray<T>* formatTreeArray<T>::atCand(int i)
{
    if (_rSwitch)
        if (i>=candSize)
            throw std::out_of_range("formatTreeArray::atCand() : Error! Only "+std::to_string(candSize)+" candidate in this event, input index exceeds this value\n");
    _candIdx = i; return this;
}
template<typename T>
int  formatTreeArray<T>::currentCand() const { return _candIdx; }
template<typename T>
int formatTreeArray<T>::size() const { return candSize; }
template<typename T>
void formatTreeArray<T>::checkValidIndex() const
{
    if ( _candIdx<0 )
        throw std::out_of_range("formatTreeArray::var() : _candIdx haven't been set. Set candidate index before accessing data.\n");
    if ( _candIdx>=candSize )
        throw std::out_of_range("formatTreeArray::var() : _candIdx exceeds the size of candidate in this event.\n");
    return;
}
template<typename T>
T formatTreeArray<T>::var(int varEnum) const
{ this->checkValidIndex(); return read[varEnum][_candIdx]; }

#endif
