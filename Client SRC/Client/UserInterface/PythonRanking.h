/**
* Filename: PythonRanking.h
* Author: Owsap
**/

#pragma once

#if defined(ENABLE_RANKING_SYSTEM)
#include "Packet.h"

class CPythonRanking : public CSingleton<CPythonRanking>
{
public:
	CPythonRanking();
	virtual ~CPythonRanking();

	void Add(const BYTE c_bType, const BYTE c_bCategory, SRankingData& rTable);
	void Clear();

	INT GetMySoloRankingPosition(const BYTE c_bCategory) const;
	TRankingData* GetHighRankingInfoSolo(const BYTE c_bCategory, const INT c_iLine) const;
	std::size_t GetRankingInfoSoloSize(const BYTE c_bCategory) const;

	INT GetMyPartyRankingPosition(const BYTE c_bCategory) const;
	TRankingData* GetHighRankingInfoParty(const BYTE c_bCategory, const INT c_iLine) const;

public:
	enum ERankingType : BYTE
	{
		TYPE_RK_SOLO,
		TYPE_RK_PARTY,
		TYPE_RK_MAX
	};

	enum ESoloRanking : BYTE
	{
		SOLO_RK_CATEGORY_BF_WEAK,
		SOLO_RK_CATEGORY_BF_TOTAL,
		SOLO_RK_CATEGORY_MD_RED,
		SOLO_RK_CATEGORY_MD_BLUE,
		SOLO_RK_CATEGORY_UNKOWN4,
		SOLO_RK_CATEGORY_UNKOWN5,
		SOLO_RK_CATEGORY_BNW,
		SOLO_RK_CATEGORY_BATTLE_ROYALE,
		SOLO_RK_CATEGORY_WORLD_BOSS,
		SOLO_RK_CATEGORY_MAX
	};

	enum EPartyRanking : BYTE
	{
#if defined(ENABLE_GUILD_DRAGONLAIR_PARTY_SYSTEM)
		PARTY_RK_CATEGORY_GUILD_DRAGONLAIR_RED_ALL,
		PARTY_RK_CATEGORY_GUILD_DRAGONLAIR_RED_NOW_WEEK,
		PARTY_RK_CATEGORY_GUILD_DRAGONLAIR_RED_PAST_WEEK,
#endif
		PARTY_RK_CATEGORY_CZ_MOUSE,
		PARTY_RK_CATEGORY_CZ_COW,
		PARTY_RK_CATEGORY_CZ_TIGER,
		PARTY_RK_CATEGORY_CZ_RABBIT,
		PARTY_RK_CATEGORY_CZ_DRAGON,
		PARTY_RK_CATEGORY_CZ_SNAKE,
		PARTY_RK_CATEGORY_CZ_HORSE,
		PARTY_RK_CATEGORY_CZ_SHEEP,
		PARTY_RK_CATEGORY_CZ_MONKEY,
		PARTY_RK_CATEGORY_CZ_CHICKEN,
		PARTY_RK_CATEGORY_CZ_DOG,
		PARTY_RK_CATEGORY_CZ_PIG,
#if defined(ENABLE_DEFENSE_WAVE)
		PARTY_RK_CATEGORY_DEFENSE_WAVE,
#endif
		PARTY_RK_CATEGORY_MAX,
	};

private:
	using RankingDataVector = std::vector<std::shared_ptr<TRankingData>>;
	using RankingCategoryMap = std::map<BYTE, RankingDataVector>;
	RankingCategoryMap m_map_vecRankingData[TYPE_RK_MAX];
};
#endif
