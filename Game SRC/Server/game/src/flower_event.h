/**
* Filename: flower_event.h
* Author: Owsap
**/

#if !defined(__INC_FLOWER_EVENT_H__) && defined(__FLOWER_EVENT__)
#define __INC_FLOWER_EVENT_H__

class CFlowerEvent
{
public:
	enum EFlowerAffect
	{
		AFFECT_UPGRADE_SUCCESS_RATE = 15,
		AFFECT_CHANGE_SUCCESS_RATE = 30,
		AFFECT_ADD_SUCCESS_RATE = 50,
		AFFECT_MAX_LEVEL = 5,
	};

	typedef struct SShootEventFlag
	{
		BYTE bShootType;
		const char* szQuestFlag;
		DWORD dwShootVNum;
	} TShootEventFlag;

public:
	CFlowerEvent() = default;
	~CFlowerEvent() = default;

	static void Reward(LPCHARACTER pChar);
	static void RequestAllInfo(LPCHARACTER pChar);
	static void Process(LPCHARACTER pChar, BYTE bSubHeader, BYTE bChatType = FLOWER_EVENT_CHAT_TYPE_MAX, BYTE bShootType = SHOOT_TYPE_MAX, int iShootCount = 0);

	static bool Exchange(LPCHARACTER pChar, BYTE bShooType, BYTE bExchangeKey);
	static bool GiveRandomShoot(LPCHARACTER pChar, int iExchangeCount);

	static bool UseFlower(LPCHARACTER pChar, LPITEM pItem);
};
#endif // __FLOWER_EVENT__
