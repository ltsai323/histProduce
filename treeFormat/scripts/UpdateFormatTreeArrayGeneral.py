#!/usr/bin/env python2
# read enum like format file and make formatTreeArray.cc & .h
# usage :
#    ./UpdateFormatTreeArray.py --cppType=double --listOfVariables=../data/treeContent_LbTk.asc --outName=LbTk --forceWrite
#    ./UpdateFormatTreeArray.py -C double -L ../data/treeContent_LbTk.asc -o LbTk -f

REPOSITORY='histProduce'
SUBREPOSITORY='treeFormat'


import argparse
import os.path
import re
templateH = '''
#ifndef __UPDATED_{fName}_formatTreeArray{varType}__
#define __UPDATED_{fName}_formatTreeArray{varType}__
#include "{repository}/{subRepository}/interface/formatTreeArray.hpp"
struct {fName}_formatTreeArray{varType} : public formatTreeArray<{varClass}>
{{
public:
    enum readVar
    {{
        {vars}
        comNum
    }};
    virtual void RegFormatTree(TTree* t) override;
    // LoadFormatSourceBranch and LoadEvtContent are two methods to load data. choose one.
    virtual void LoadFormatSourceBranch(TTree* t) override;
    virtual void LoadEvtContent(TreeReader& evtInfo) override;

    explicit {fName}_formatTreeArray{varType}( int totNum=comNum, int maxCSize=128 ) : formatTreeArray ( totNum, maxCSize ) {{
    }}
    virtual ~{fName}_formatTreeArray{varType}() {{
    }}
}};

#endif
'''

templateCC = '''
#include "{repository}/{subRepository}/interface/{fName}_formatTreeArray{varType}.h"
#include "{repository}/{subRepository}/interface/untuplizer.h"
#include <stdio.h>
#include <stdlib.h>

void {fName}_formatTreeArray{varType}::RegFormatTree(TTree* t)
{{
    if ( t == nullptr )
    {{
        printf("{fName}_formatTreeArray{varType}:        ERROR   no input tree, program cannot register tree to record data!\\n");
        exit(1);
    }}
    this->ReadyForOutputTreeContent();

    t->Branch( "{sizeVar}", &candSize, "{sizeVar}/I" );

    {varBranch}

    return;
}}

void {fName}_formatTreeArray{varType}::LoadFormatSourceBranch(TTree* t)
{{
    this->ReadyForInputTreeContent();
    return;

    // t->SetBranchAddress( "{sizeVar}", &candSize ); // this line need to be modified.

    {varBranchAddr}
    return;
}}
void {fName}_formatTreeArray{varType}::LoadEvtContent(TreeReader& evtInfo)
{{
    if ( evtInfo.isEmpty() )
        throw std::invalid_argument("{fName}_formatTreeArray{varType}::LoadEvtContent() : imported TTreeReader is empty\\n");
    // candSize = evtInfo.GetInt("{sizeVar}"); // this line need to be modified.
    return;
    {varGetPtr}

    return;
}}
'''

def listVarsFromFile(inFile):
    # load file which has content like enum format
    lists = []
    if not inFile:
        return lists
    with open(inFile, mode='r') as iFile:
        for line in iFile:
            cleanline=line.strip()
            if ',' in cleanline:
               for var in cleanline.split(','):
                  outStr = var.strip()
                  if outStr != '':
                     lists.append(outStr)
            else:
               lists.append(cleanline)
    return lists

def listEnumFromFile(varList):
   return [ var+',' for var in varList ]


def extractFromEnumList(enumList):
    # extract str with format 'dsfi, wenc, oeirjl, erwuhbnejriv, slkdf387, '
    # to 'dsfi', 'wenc', 'oeirjl', 'erwuhbnejriv', 'slkdf387'
    lists = []
    for eachLine in enumList:
        if ',' in eachLine:
            for var in eachLine.split(','):
               outStr = var.lstrip().rstrip()
               if outStr != '':
                  lists.append(outStr)
        else:
           lists.append(eachLine)
    return lists


def formatBranch(varList, type, candsize):
    #   t->Branch( "lbtkMass", &dataD[lbtkMass], "lbtkMass/D" );
    branchFormat = '''t->Branch( "{tVar}", data[{tVar}], "{tVar}[{size}]/{varType}" );'''
    return [branchFormat.format(tVar=var, varType=type, size=candsize) for var in varList]


def formatSetBranchAddress(varList):
    #   t->SetBranchAddress( "lbtkMass", &readD[lbtkMass] );
    branchAddFormat = '''t->SetBranchAddress( "{0}", read[{0}] );'''
    return [branchAddFormat.format(var) for var in varList]
def formatGetPtr(varList, varClass):
    print varClass
    getPtrContent = '''read[{0}] = evtInfo.GetPtr%s("{0}");''' % (varClass.capitalize())
    return [getPtrContent.format(var) for var in varList]
def formatStrFromList(vars):
    if vars:
        return '\n    '.join(vars)
    raise ValueError('formatStrFromList() : input variable list is not able to be recognized.\n -- {0}'.format(vars))

if __name__ == '__main__':
    parser = argparse.ArgumentParser('Options for create formatTreeArray')
    parser.add_argument('-L', '--listOfVariables', type=str, default=None,
                        help='input variable list, a "," is the separater to divide each variable.')
    parser.add_argument('-o', '--outName', type=str, default=None,
                        help='output filename and class name ( this name will be formated )')
    parser.add_argument('-f', '--forceWrite', action='store_true',
                        help='if output file existed, overwritten it!')
    parser.add_argument('-C', '--cppType', type=str, default=None,
                        help='input variable type in c++. Currently allowed value is "double", "float", "int" and "unsigned". This option must be offered.')
    opt = parser.parse_args()

    if not opt.cppType or not opt.listOfVariables:
        raise ValueError('You need to input variable list and their type as the argument. [-L] and [-V].')
    if not opt.cppType:
        raise ValueError('You need to assign the variable type in c++ code. [-C]/[--cppType]')

    # if no filename assigned, use treeFormatArrayTmp.* and use Tmp to be the class name.
    outputFileName = 'Tmp'
    briefVarType=opt.cppType[0].upper()
    if opt.outName:
        outputFileName = opt.outName
        if os.path.isfile('../interface/{1}_formatTreeArray{0}.h'.format(briefVarType,outputFileName)):
            if not opt.forceWrite:
                print '\n\noutput directory already owns the same file. If you want to overrite it. Please add [-f][--forceWrite] option'
                exit()
    candSize=outputFileName+'CandSize'


    vars = listVarsFromFile(opt.listOfVariables)
    varEnumList = listEnumFromFile(vars)
    varBranch = formatBranch(vars, briefVarType, candSize)
    varBranchAdd = formatSetBranchAddress(vars)
    varGetPtr = formatGetPtr(vars, opt.cppType)

    with open('../interface/{1}_formatTreeArray{0}.h'.format(briefVarType,outputFileName), mode='w') as headerFile:
        enum = ''
        if len(varEnumList):
            enum = '\n        '.join(varEnumList)

        headerFile.write(templateH.format(
            fName=outputFileName,
            sizeVar=candSize,
            vars=enum,
            varType=briefVarType,
            varClass=opt.cppType,
            repository=REPOSITORY,
            subRepository=SUBREPOSITORY))
    with open('../src/{1}_formatTreeArray{0}.cc'.format(briefVarType,outputFileName), mode='w') as ccFile:
        ccFile.write(templateCC.format(
            fName=outputFileName,
            sizeVar=candSize,
            varBranch=formatStrFromList(varBranch),
            varBranchAddr=formatStrFromList(varBranchAdd),
            varGetPtr=formatStrFromList(varGetPtr),
            varType=briefVarType,
            repository=REPOSITORY,
            subRepository=SUBREPOSITORY))

    print '\n\nyour file is created, please check interface/{1}_formatTreeArray{0}.h and {1}_formatTreeArray{0} .cc'.format(briefVarType,outputFileName)
