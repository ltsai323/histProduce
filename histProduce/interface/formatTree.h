#ifndef __FORMATTREE_H__
#define __FORMATTREE_H__
#include "TTree.h"
#include <iostream>

struct formatTree
{
    public:
        double* dataD;
        int*    dataI;
        double* readD;
        int*    readI;

        formatTree( int nD, int nI ) : totNumD(nD), totNumI(nI), readMode(false)
        {
            dataD = new double[nD];
            dataI = new int   [nI];
            readD = nullptr;
            readI = nullptr;
            return;
        }
        virtual ~formatTree()
        { 
            delete[] dataD; delete[] dataI;
            if ( readMode )
            {
                delete[] readD;
                delete[] readI;
                readD = nullptr;
                readI = nullptr;
            }
            dataD = nullptr;
            dataI = nullptr;
            return; 
        }

        // used in tree creating, every entry you need to get a clean contain.
        virtual void Clear() final
        {
            if ( dataD )
                memset( dataD, 0x00, totNumD*sizeof( double ) );
            if ( dataI )
                memset( dataI, 0x00, totNumI*sizeof( int    ) );

            return;
        }

        // create new tree structure
        virtual void RegFormatTree(TTree* t)   = 0;
        // load tree structure from old tree
        virtual void LoadFormatSourceBranch(TTree* t) = 0;
        void SetReadMode()
        { 
            readMode = true;
            readD = new double[totNumD];
            readI = new int   [totNumI];
            return;
        }
        
    private:
        const int totNumD;
        const int totNumI;
        bool readMode;

};
#endif

