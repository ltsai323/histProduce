#include "histProduce/histProduce/interface/tmainGenList.h"
#include "histProduce/histProduce/interface/generalCutList.h"
#include "histProduce/histProduce/interface/fourMom.h"
#include "histProduce/histProduce/interface/usefulFuncs.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "math.h"


treeMainGen_List::treeMainGen_List( TFileDirectory* d ) :
    treeMainGen( d, treeMain::Label("lbWriteSpecificDecay", "Lam0Fitted", "bphAnalysis"), "GenList" ),
    protonMass( 0.9382720813 ), pionMass( 0.13957061 )
{
    RegTree();
}
void treeMainGen_List::Process( fwlite::Event* ev )
{
    try
    {
        if ( !ev->isValid() ) return;
        GetByLabel( ev );


        if ( _genHandle->size() == 0 ) return;

        std::vector<reco::GenParticle>::const_iterator iter = _genHandle->begin();
        std::vector<reco::GenParticle>::const_iterator iend = _genHandle->end  ();

        while ( iter != iend )
        {
            Clear();
            const reco::GenParticle gParticle = *iter++;

            // choose a fragiled particle.
            if ( gParticle.status() != 2 ) continue;
            int NumDaus = gParticle.numberOfDaughters();
            if ( NumDaus < 2 ) continue;

            const reco::Candidate* daug[NumDaus] = {nullptr};
            bool daugCutTag = false;
            for ( int i=0; i<NumDaus; ++i )
            {
                daug[i] = gParticle.daughter(i);

                // choose particles not fragiled.
                if ( daug[i]->status() != 1 )
                {
                    daugCutTag = true;
                    break;
                }
            }
            if ( daugCutTag ) continue;


            dataI[motherPID] = gParticle.pdgId();
            dataI[motherNumDaughter] = NumDaus;
            dataD[motherPt]  = gParticle.pt();
            dataD[motherMass]= gParticle.mass();

            dataI[d1PID] = daug[0]->pdgId();
            dataD[d1Pt]  = daug[0]->pt();
            dataD[d1Mom] = daug[0]->p();

            dataI[d2PID] = daug[1]->pdgId();
            dataD[d2Pt]  = daug[1]->pt();
            dataD[d2Mom] = daug[1]->p();

            if ( NumDaus > 2 )
            {
                dataI[d3PID] = daug[2]->pdgId();
                dataD[d3Pt]  = daug[2]->pt();
                dataD[d3Mom] = daug[2]->p();
            }

            thisTree()->Fill();
        }
    } catch (...) {}
}

void treeMainGen_List::Clear()
{
    memset( dataD, 0x00, totNumD*sizeof( double ) );
    memset( dataI, 0x00, totNumI*sizeof( int ) );
}
void treeMainGen_List::RegTree()
{
    thisTree()->Branch( "motherPt", &dataD[motherPt], "motherPt/D" );
    thisTree()->Branch( "motherMass", &dataD[motherMass], "motherMass/D" );

    thisTree()->Branch( "d1Pt" , &dataD[d1Pt] , "d1Pt/D" );
    thisTree()->Branch( "d1Mom", &dataD[d1Mom], "d1Mom/D" );
    thisTree()->Branch( "d2Pt" , &dataD[d2Pt] , "d2Pt/D" );
    thisTree()->Branch( "d2Mom", &dataD[d2Mom], "d2Mom/D" );
    thisTree()->Branch( "d3Pt" , &dataD[d3Pt] , "d3Pt/D" );
    thisTree()->Branch( "d3Mom", &dataD[d3Mom], "d3Mom/D" );

    thisTree()->Branch( "motherNumDaughter", &dataI[motherNumDaughter], "motherNumDaughter/I" );
    thisTree()->Branch( "motherPID", &dataI[motherPID], "motherPID/I" );
    thisTree()->Branch( "d1PID", &dataI[d1PID], "d1PID/I" );
    thisTree()->Branch( "d2PID", &dataI[d2PID], "d2PID/I" );
    thisTree()->Branch( "d3PID", &dataI[d3PID], "d3PID/I" );
}
void treeMainGen_List::GetByLabel( fwlite::Event* ev )
{
    getByLabel_genParticle( ev );
}
