
#include "histProduce/treeFormat/interface/ggPhoton_formatTreeArrayI.h"
#include "histProduce/treeFormat/interface/untuplizer.h"
#include <stdio.h>
#include <stdlib.h>

void ggPhoton_formatTreeArrayI::RegFormatTree(TTree* t)
{
    if ( t == nullptr )
    {
        printf("ggPhoton_formatTreeArrayI:        ERROR   no input tree, program cannot register tree to record data!\n");
        exit(1);
    }
    this->ReadyForOutputTreeContent();

    t->Branch( "ggPhotonCandSize", &candSize, "ggPhotonCandSize/I" );

    t->Branch( "phohasPixelSeed", data[phohasPixelSeed], "phohasPixelSeed[ggPhotonCandSize]/I" );
    t->Branch( "phoEleVeto", data[phoEleVeto], "phoEleVeto[ggPhotonCandSize]/I" );

    return;
}

void ggPhoton_formatTreeArrayI::LoadFormatSourceBranch(TTree* t)
{
    this->ReadyForInputTreeContent();
    return;

    // t->SetBranchAddress( "ggPhotonCandSize", &candSize ); // this line need to be modified.

    t->SetBranchAddress( "phohasPixelSeed", read[phohasPixelSeed] );
    t->SetBranchAddress( "phoEleVeto", read[phoEleVeto] );
    return;
}
void ggPhoton_formatTreeArrayI::LoadEvtContent(TreeReader& evtInfo)
{
    if ( evtInfo.isEmpty() )
        throw std::invalid_argument("ggPhoton_formatTreeArrayI::LoadEvtContent() : imported TTreeReader is empty\n");
    candSize = evtInfo.GetInt("nPho");
    read[phohasPixelSeed] = evtInfo.GetPtrInt("phohasPixelSeed");
    read[phoEleVeto] = evtInfo.GetPtrInt("phoEleVeto");

    return;
}
