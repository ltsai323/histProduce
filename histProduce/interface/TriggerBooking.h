
#ifndef __UPDATED_TriggerBooking_h__
#define __UPDATED_TriggerBooking_h__
#include <string>
namespace HLTList
{
    const std::string trigName[] = {
        "HLT_DoubleMu4_3_Jpsi_Displaced",
		"HLT_DoubleMu4_JpsiTrkTrk_Displaced",
		"HLT_DoubleMu4_JpsiTrk_Displaced",
		"HLT_DoubleMu4_Jpsi_Displaced",
		"HLT_Dimuon10_Jpsi_Barrel",
		"HLT_Dimuon20_Jpsi_Barrel_Seagulls",
		"HLT_Dimuon16_Jpsi",
		"HLT_Dimuon20_Jpsi",
		"HLT_Dimuon25_Jpsi"
    };

    enum trigNum {
        HLT_DoubleMu4_3_Jpsi_Displaced=0,
		HLT_DoubleMu4_JpsiTrkTrk_Displaced=1,
		HLT_DoubleMu4_JpsiTrk_Displaced=2,
		HLT_DoubleMu4_Jpsi_Displaced=3,
		HLT_Dimuon10_Jpsi_Barrel=4,
		HLT_Dimuon20_Jpsi_Barrel_Seagulls=5,
		HLT_Dimuon16_Jpsi=6,
		HLT_Dimuon20_Jpsi=7,
		HLT_Dimuon25_Jpsi=8,
        totNum
    };
	int encodeHLT( int hltNum )
	{
		return 1<<hltNum;
	}

	bool hasHLT( int encodeBoolInt, int num )
	{
		return (encodeBoolInt>>num)%2;
	}
}
#endif
