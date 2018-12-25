
#ifndef __TriggerBooking_h__
#define __TriggerBooking_h__
#include <string>
namespace HLTList
{
	const std::string trigName[] = {
		"HLT_DoubleMu4_3_Jpsi_Displaced_v*",
		"HLT_DoubleMu4_JpsiTrkTrk_Displaced_v*",
		"HLT_DoubleMu4_JpsiTrk_Displaced_v*",
		"HLT_DoubleMu4_Jpsi_Displaced_v*",
		"HLT_Dimuon10_Jpsi_Barrel_v*",
		"HLT_Dimuon20_Jpsi_Barrel_Seagulls_v*",
		"HLT_Dimuon16_Jpsi_v*",
		"HLT_Dimuon20_Jpsi_v*",
		"HLT_Dimuon25_Jpsi_v*"
	};

	enum trigNum {
		HLT_DoubleMu4_3_Jpsi_Displaced_v=0,
		HLT_DoubleMu4_JpsiTrkTrk_Displaced_v=1,
		HLT_DoubleMu4_JpsiTrk_Displaced_v=2,
		HLT_DoubleMu4_Jpsi_Displaced_v=3,
		HLT_Dimuon10_Jpsi_Barrel_v=4,
		HLT_Dimuon20_Jpsi_Barrel_Seagulls_v=5,
		HLT_Dimuon16_Jpsi_v=6,
		HLT_Dimuon20_Jpsi_v=7,
		HLT_Dimuon25_Jpsi_v=8,
		totNum=9
	};
	int encodeHLT( int hltNum )
	{
		return 1<<hltNum;
	}

	bool decodeHLT( int encodeBoolInt, int num )
	{
		return (encodeBoolInt>>num)%2;
	}
}
#endif
