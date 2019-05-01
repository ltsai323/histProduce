#!/usr/bin/env python2
# read enum like format file and make formatTreeArray.cc & .h
# usage :
#    /UpdateFormatTreeArray.py -I ../data/treeContent_MC_fourDaughterI.asc -D ../data/treeContent_MC_fourDaughterD.asc -o MC_fourDaughter --forceWrite


import argparse
import os.path
import re
templateH = '''
# ifndef __UPDATED_formatTreeArray_{0}__
# define __UPDATED_formatTreeArray_{0}__
# include "histProduce/histProduce/interface/formatTreeArray.h"
struct formatTreeArray_{0} : public formatTreeArray
{{
public:
	enum readVarD
	{{
        {1}
		comNumD
	}};
	enum readVarI
    {{
        {2}
		comNumI
	}};
	virtual void RegFormatTree(TTree* t) override;
	virtual void LoadFormatSourceBranch(TTree* t) override;

	formatTreeArray_{0}( int totNumD=comNumD, int totNumI=comNumI ) : formatTreeArray ( totNumD, totNumI ) {{
	}}
	~formatTreeArray_{0}() {{
	}}
}};

# endif
'''

templateCC = '''
# include "histProduce/histProduce/interface/formatTreeArray{0}.h"
# include <stdio.h>
# include <stdlib.h>

void formatTreeArray_{0}::RegFormatTree(TTree* t)
{{
    if ( t == nullptr )
    {{
        printf("formatTreeArray {0}:        ERROR   no input tree, program cannot register tree to record data!\\n");
        exit(1);
    }}
    t->Branch( "candSize", &candSize, "candSize/I" );

    {1}

    {2}

    return;
}}

void formatTreeArray_{0}::LoadFormatSourceBranch(TTree* t)
{{
    if ( t == nullptr )
    {{
        printf("formatTreeArray {0}:        ERROR   no input tree, program cannot load tree from data!\\n");
        exit(1);
    }}
    SetReadMode();
    t->SetBranchAddress( "candSize", &candSize );

    {3}

    {4}

    return;
}}
'''


def listEnumFromFile(inFile):
    # load file which has content like enum format
    lists = []
    if not inFile:
        return lists
    with open(inFile, mode='r') as iFile:
        for line in iFile:
            if line.endswith((',\n', ', \n')):
                # remove space before and after the string
                # output str will like 'asdf, asdf2, asdf3,'
                outStr = line.lstrip().rstrip()
                if outStr != '':
                    lists.append(outStr)
    return lists


def extractFromEnumList(enumList):
    # extract str with format 'dsfi, wenc, oeirjl, erwuhbnejriv, slkdf387, '
    # to 'dsfi', 'wenc', 'oeirjl', 'erwuhbnejriv', 'slkdf387'
    lists = []
    for eachLine in enumList:
        for var in eachLine.split(','):
            outStr = var.lstrip().rstrip()
            if outStr != '':
                lists.append(outStr)
    return lists


def formatBranch(varList, type):
    #   t->Branch( "lbtkMass", &dataD[lbtkMass], "lbtkMass/D" );
    branchFormat = '''t->Branch( "{0}", data{1}[{0}], "{0}[candSize]/{1}" );'''
    return [branchFormat.format(var, type) for var in varList]


def formatSetBranchAddress(varList, type):
    #   t->SetBranchAddress( "lbtkMass", &readD[lbtkMass] );
    branchAddFormat = '''t->SetBranchAddress( "{0}", read{1}[{0}] );'''
    return [branchAddFormat.format(var, type) for var in varList]


if __name__ == '__main__':
    parser = argparse.ArgumentParser('Options for create formatTreeArray')
    parser.add_argument('-I', '--intVarsList',
                        help='input list of INT variables, separated with ","', type=str, default=None)
    parser.add_argument('-D', '--doubleVarsList',
                        help='input list of DOUBLE variables, separated with ","', type=str, default=None)
    parser.add_argument(
        '-o', '--outName', help='output filename and class name ( this name will be formated )', type=str, default=None)
    parser.add_argument(
        '-f', '--forceWrite', help='if output file existed, overwritten it!', action='store_true')
    opt = parser.parse_args()
    if not opt.doubleVarsList:
        print 'you need to input variable list (double type).'
        parser.print_help()
        exit()

    intVarEnumList = listEnumFromFile(opt.intVarsList)
    intVars = extractFromEnumList(intVarEnumList)
    intVarBranch = formatBranch(intVars, 'I')
    intVarBranchAdd = formatSetBranchAddress(intVars, 'I')

    doubleVarEnumList = listEnumFromFile(opt.doubleVarsList)
    doubleVars = extractFromEnumList(doubleVarEnumList)
    doubleVarBranch = formatBranch(doubleVars, 'D')
    doubleVarBranchAdd = formatSetBranchAddress(doubleVars, 'D')

    # if no filename assigned, use treeFormatArrayTmp.* and use Tmp to be the class name.
    outputFileName = 'Tmp'
    if opt.outName:
        outputFileName = opt.outName
        if os.path.isfile('../interface/formatTreeArray{}.h'.format(outputFileName)):
            if not opt.forceWrite:
                print '\n\noutput directory already owns the same file. If you want to overrite it. Please add [-f][--forceWrite] option'
                exit()
    with open('../interface/formatTreeArray{}.h'.format(outputFileName), mode='w') as headerFile:
        enumI = ''
        enumD = ''
        if len(doubleVarEnumList):
            enumD = '\n\t\t'.join(doubleVarEnumList)
        if len(intVarEnumList):
            enumI = '\n\t\t'.join(intVarEnumList)

        headerFile.write(templateH.format(outputFileName, enumD, enumI))
    with open('../src/formatTreeArray{}.cc'.format(outputFileName), mode='w') as ccFile:
        branchD = ''
        branchI = ''
        branchAddD = ''
        branchAddI = ''
        if doubleVarBranch:
            branchD = '\n\t'.join(doubleVarBranch)
        if intVarBranch:
            branchI = '\n\t'.join(intVarBranch)
        if doubleVarBranchAdd:
            branchAddD = '\n\t'.join(doubleVarBranchAdd)
        if intVarBranchAdd:
            branchAddI = '\n\t'.join(intVarBranchAdd)

        ccFile.write(templateCC.format(outputFileName,
                                       branchD, branchI, branchAddD, branchAddI))

    print '\n\nyour file is created, please check formatTreeArray{0}.h and formatTreeArray{0}.cc'.format(outputFileName)
