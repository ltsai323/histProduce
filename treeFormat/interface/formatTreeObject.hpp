#ifndef __formatTreeReadObject_HPP__
#define __formatTreeReadObject_HPP__
#include "histProduce/treeFormat/interface/formatTreeArray.hpp"
// this class are separated as two parts : const and non const.
// non const part is used as

template <typename T>
struct formatTreeObject : public formatTreeArray<T>
{
public:

    //explicit formatTreeReadObject( int nVar );
    formatTreeObject( int nVar );
    formatTreeObject( formatTreeArray<T>* exobj, int candIdx ); // used for formatTreeReadObject
    virtual ~formatTreeObject();
    int GetNVars();


    // create new tree structure
    virtual void RegFormatTree(TTree* t) override;
    // load tree structure from old tree
    virtual void LoadFormatSourceBranch(TTree* t) override;
    virtual void GetEntry(Long64_t evtIdx) override;

    virtual int  size() const override;
    virtual T var(int varEnum) const override;

private:
    const formatTreeArray<T>* _exdata;
    const int cIdx;


};
// make a alias to const reader.
template<typename T>
using formatTreeReadObject = const formatTreeObject<T>;
#include "histProduce/treeFormat/src/formatTreeObject.cpp"
#endif
