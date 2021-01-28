# histProduce
histProduce name is needed to be modified.
fwlite for lbWriteSpecificDecay, produce histograms


## CMSSWTreeMgr
Mainly FWLite and EDAnalyzer. CMSSW dependency is needed. Use 'scram b' to build.

## RootTreeMgr
Load root TTree and convert to ntuples or plots. No CMSSW dependency but it can be built in CMSSW. makefile is main building metho.

## treeFormat
Record the tree format like Branch, SetBranchAddress and variables declare in root file.

# ToDo
* makefile : can it execute other makefile in current makefile?
* completion of CMSSWTreeMgr
* change the header path in each file due to name changing sub-directiory.

## treeFormat
* for loop in makefile. How to build all of things in makefile in one line.

## RootTreeMgr
* build c++ code as a python module. And use python to control the whole procedure.
* how to build boost library in g++
