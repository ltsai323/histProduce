#ifndef __FORMATTREE_H__
#define __FORMATTREE_H__
#include "TTree.h"

struct formatTree
{
    public:
        double* dataD;
        int*    dataI;
        double* readD;
        int*    readI;

        formatTree( int nD, int nI ) : oTree(nullptr), nTree(nullptr), totNumD(nD), totNumI(nI)
        {
            dataD = new double(nD);
            dataI = new int   (nI);
            return;
        }
        virtual ~formatTree()
        { delete dataD; delete dataI; return; }

    // used in tree creating, every entry you need to get a clean contain.
    virtual void Clear() final
    {
        memset( dataD, 0x00, totNumD*sizeof( double ) );
        memset( dataI, 0x00, totNumI*sizeof( int    ) );
        return;
    }
    // create new tree structure
    virtual void RegFormatTree()   = 0;
    // load tree structure from old tree
    virtual void LoadFormatSourceBranch() = 0;

    void SetOldFormatTree( TTree* t ) { oTree = t; return; }
    void SetNewFormatTree( TTree* t ) { nTree = t; return; }
    TTree* getOldFormatTree() { return oTree; }
    TTree* getNewFormatTree() { return nTree; }
    private:
        TTree* oTree;
        TTree* nTree;
        int totNumD, totNumI;

};
#endif

