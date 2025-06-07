/**
* Filename: minigame_roulette.h
* Author: Owsap
* Description: Mini-Game Roulette (Late Summer Event)
**/

#if !defined(__INC_MINI_GAME_ROULETTE_H__) && defined(__SUMMER_EVENT_ROULETTE__)
#define __INC_MINI_GAME_ROULETTE_H__

#include "text_file_loader.h"
#include "questlua.h"

class CRouletteManager : public singleton<CRouletteManager>
{
public:
	CRouletteManager() {};
	~CRouletteManager() {};

	typedef struct SMapper
	{
		std::string strGroupName;
		BYTE bType;
		int iPrice;
		int iSoulCount;
	} TMapper;
	typedef std::map<BYTE, TMapper> TMapperMap;

	typedef struct SItem
	{
		DWORD dwVnum;
		BYTE bCount;
		DWORD dwWeight;
	} TItem;
	typedef std::vector<TItem> TItemVec;
	typedef std::map<BYTE, TItemVec> TItemMap;

	bool ReadRouletteTableFile(const char* c_pszFileName);

	BYTE GetRandomMapperIndex(BYTE bType) const;
	BYTE GetRandomItemIndex(BYTE bMapperIndex) const;

	const TMapper* GetMapper(BYTE bRowIndex) const;
	const TItem* GetItem(BYTE bMapperIndex, BYTE bRowIndex) const;
	const TItemVec* GetItemVec(BYTE bMapperIndex) const;

protected:
	bool __LoadRouletteMapper(CTextFileLoader& TextFileLoader);
	bool __LoadRouletteItem(CTextFileLoader& TextFileLoader);

private:
	TMapperMap m_MapperMap;
	TItemMap m_ItemMap;
};

class CMiniGameRoulette
{
public:
	enum EMiniGameRoulette
	{
		TYPE_NORMAL,
		TYPE_SPECIAL
	};

	enum EMiniGameRouletteMisc
	{
		EVENT_NPC = 33009,
		PRICE = 250000,
		SOUL_PRICE = 50,
		SOUL_MAX_NUM = 4000,

		BUFF_SOUL_COUNT = 20,
		PREMIUM_BUFF_SOUL_COUNT = 150,

		SPECIAL_ROULETTE_EXPIRE_TIME = 3600,
	};

public:
	CMiniGameRoulette(LPCHARACTER pNPC, LPCHARACTER pChar, BYTE bType);
	~CMiniGameRoulette();

	static void Open(LPCHARACTER pNPC, LPCHARACTER pChar, BYTE bType);
	static bool IsActiveEvent();
	static bool IsSpecialRoulette(LPCHARACTER pChar);
	static void SpawnEventNPC(bool bSpawn);
	static void UpdateQuestFlag(LPCHARACTER pChar);

	static void GetScoreTable(lua_State* L);
	static DWORD GetMyScoreValue(lua_State* L, DWORD dwPID);

public:
	void Initialize();
	void Destroy();

	void Start();
	void Request();
	void End();
	void Close(bool bForce = false);
	void CloseError(const char* pszMsg, ...);

	void Recover();
	void SendPacket(BYTE bSubHeader);
	void SetSpecialRouletteEvent(LPEVENT pEvent);
	void UpdateMyScore(DWORD dwPID);

	LPCHARACTER GetCharacter() const { return m_pChar; }
	LPCHARACTER GetRouletteCharacter() const { return m_pNPC; }

private:
	BYTE m_bType;
	BYTE m_bMapperIndex;
	BYTE m_bItemIndex;

	int m_iPrice;
	int m_iSoulCount;

	LPCHARACTER m_pChar;
	LPCHARACTER m_pNPC;

	bool m_bIsSpinning;
	BYTE m_bSpinCount;

	LPEVENT m_pSpecialRouletteEvent;
};
#endif // __SUMMER_EVENT_ROULETTE__
