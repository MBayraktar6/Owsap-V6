#ifndef __INC_ITEM_AWARD_H__
#define __INC_ITEM_AWARD_H__

#include <map>
#include <set>
#include "Peer.h"

enum
{
	ITEM_SKILLBOOK_VNUM = 50300,
	ITEM_SKILLFORGET_VNUM = 70037,
#if defined(__EXTENDED_ITEM_AWARD__)
	ITEM_ATTRIBUTE_NONE = -1,
#endif
};

typedef struct SItemAward
{
	DWORD dwID;
	char szLogin[LOGIN_MAX_LEN + 1];
	DWORD dwVnum;
	DWORD dwCount;
	DWORD dwSocket0;
	DWORD dwSocket1;
	DWORD dwSocket2;
#if defined(__ITEM_SOCKET6__)
	DWORD dwSocket3;
	DWORD dwSocket4;
	DWORD dwSocket5;
#endif
	char szWhy[ITEM_AWARD_WHY_MAX_LEN + 1];
	bool bTaken;
	bool bMall;
#if defined(__EXTENDED_ITEM_AWARD__)
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
#endif
} TItemAward;

typedef std::set<TItemAward*> ItemAwardSet;
typedef std::map<DWORD, TItemAward*> SetAwardMap;
typedef std::map<std::string, ItemAwardSet> kSetAwardMap;

class ItemAwardManager : public singleton<ItemAwardManager>
{
public:
	ItemAwardManager();
	virtual ~ItemAwardManager();

	void RequestLoad();
	void Load(SQLMsg* pMsg);
	ItemAwardSet* GetByLogin(const char* c_pszLogin);

	void Taken(DWORD dwAwardID, DWORD dwItemID);

#if defined(__EXTENDED_ITEM_AWARD__)
	void CheckItemAttributes(TItemAward& pkItemAward, const TItemTable& pkItemTable, const std::vector<TItemAttrTable> vec_itemAttrTable);
	const int8_t GetItemAttributeSetIndex(const uint8_t bItemType, const uint8_t bItemSubType);

	void CheckItemAddonType(TItemAward& pkItemAward, const TItemTable& pkItemTable);
	void CheckItemCount(TItemAward& pkItemAward, const TItemTable& pkItemTable);
	void CheckItemBlend(TItemAward& pkItemAward, const TItemTable& pkItemTable);
	void CheckItemSkillBook(TItemAward& pkItemAward, const std::vector<TSkillTable> vec_skillTable);
#endif

	// gift notify
	std::map<DWORD, TItemAward*>& GetMapAward();
	std::map<std::string, ItemAwardSet>& GetMapkSetAwardByLogin();
private:
	// ID, ItemAward pair
	SetAwardMap m_map_award;
	// PID, ItemAward pair
	kSetAwardMap m_map_kSetAwardByLogin;
};

#endif // __INC_ITEM_AWARD_H__
