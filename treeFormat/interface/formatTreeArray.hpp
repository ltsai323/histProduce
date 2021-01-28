#ifndef __formatTreeArray_HPP__
#define __formatTreeArray_HPP__
#include "TTree.h"
#include <iostream>
#include "histProduce/treeFormat/interface/untuplizer.h"
#define MAX_CAND_NUM 128

// this file is to create data structure in TTree when you run EDanalyzer
// And formatTreeArray is a virtual class.
// The only way to use this file is to inheritate this class and create
// two 'enum' : readVarD and readVarI to define the variables.
//
// And formatTreeArray store array in event.
// Ex:
//	tree->Branch( "trigError", dataI[trigError], "trigError[candSize]/I" );
//	tree->SetBranchAddress( "tk2IPtErr", readD[tk2IPtErr] );

template <class T>
struct formatTreeArray
{
public:
    T** data;
    T** read;
    int candSize;

	explicit formatTreeArray( int nVar, int maxCSize=MAX_CAND_NUM );
	virtual ~formatTreeArray();

	// used in tree creating, every entry you need to get a clean contain.
	virtual void Clear() final;

	// create new tree structure
	virtual void RegFormatTree(TTree* t) = 0;
	// load tree structure from old tree. (select one of them)
	virtual void LoadFormatSourceBranch(TTree* t);
    virtual void LoadEvtContent(TreeReader& evtInfo);

    void ReadyForInputTreeContent();
    void ReadyForOutputTreeContent();

    // functions for reading
    virtual formatTreeArray<T>* atCand(int i);
    virtual int  currentCand() const;
    virtual int size() const;
    virtual T var(int varEnum) const;
    virtual void checkValidIndex() const final;

private:
	bool _rSwitch, _wSwitch;
    int _candIdx;

    const int _allVar;
    const int _maxCandSize;
};
#include "histProduce/treeFormat/src/formatTreeArray.cpp"
#endif
// asdf I can add a treeFormatObject class to be the sub structure of this.
