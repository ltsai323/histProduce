#ifndef __formatTreeArray_H__
#define __formatTreeArray_H__
#include "TTree.h"
#include "TLorentzVector.h"
#include <iostream>
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

struct formatTreeArray
{
public:
	double** dataD;
	int**    dataI;
	double** readD;
	int**    readI;
    int      candSize;

	formatTreeArray( int nD, int nI ) : totNumD(nD), totNumI(nI), readMode(false)
	{
		dataD = new double*[totNumD];
		dataI = new int*   [totNumI];
        for ( int i=0; i<totNumD; ++i )
            dataD[i] = new double[MAX_CAND_NUM];
        for ( int i=0; i<totNumI; ++i )
            dataI[i] = new int   [MAX_CAND_NUM];

		readD = nullptr;
		readI = nullptr;
		return;
	}
	virtual ~formatTreeArray()
	{
		if ( readMode )
		{
			delete[] readD;
			delete[] readI;

			readD = nullptr;
			readI = nullptr;
		}
        else
        {
            for ( int i=0; i<totNumD; ++i )
                delete[] dataD[i];
            for ( int i=0; i<totNumI; ++i )
                delete[] dataI[i];
            delete[] dataD;
            delete[] dataI;
            dataD = nullptr;
            dataI = nullptr;
        }
		return;
	}

	// used in tree creating, every entry you need to get a clean contain.
	virtual void Clear() final
	{
        if ( readMode )
        {
            printf("formatTreeArray::Clear() : error when clear! Clear data is not allowed when you read something\n");
            exit(1);
        }
        for ( int i=0; i<totNumD; ++i )
            if ( dataD[i] )
                memset( dataD[i], 0x00, MAX_CAND_NUM*sizeof( double ) );
        for ( int i=0; i<totNumI; ++i )
            if ( dataI[i] )
                memset( dataI[i], 0x00, MAX_CAND_NUM*sizeof( int    ) );
        candSize = 0;

		return;
	}

	// create new tree structure
	virtual void RegFormatTree(TTree* t)   = 0;

	// load tree structure from old tree
	virtual void LoadFormatSourceBranch(TTree* t) = 0;

	void SetReadMode()
	{
        candIdx = -1;
		readMode = true;
		readD = new double*[totNumD];
		readI = new int*   [totNumI];

        for ( int i=0; i<totNumD; ++i )
            delete[] dataD[i];
        for ( int i=0; i<totNumI; ++i )
            delete[] dataI[i];
        delete[] dataD;
        delete[] dataI;
        dataD = nullptr;
        dataI = nullptr;
		return;
	}

    // functions for reading
    void atCand(int i) { candIdx = i; return; }
    double getD(int idx) { if ( candIdx == -1 ) {printf("set cand Idx before you get().\n"); exit(1);} return readD[idx][candIdx]; }
    int    getI(int idx) { if ( candIdx == -1 ) {printf("set cand Idx before you get().\n"); exit(1);} return readI[idx][candIdx]; }
private:
	const int totNumD;
	const int totNumI;
	bool readMode;
    int candIdx;
};
#endif
