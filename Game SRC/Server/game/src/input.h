#ifndef __INC_INPUT_PROCESSOR_H__
#define __INC_INPUT_PROCESSOR_H__

#include "packet_info.h"

enum
{
	INPROC_CLOSE,
	INPROC_HANDSHAKE,
	INPROC_LOGIN,
	INPROC_MAIN,
	INPROC_DEAD,
	INPROC_DB,
	INPROC_UDP,
	INPROC_P2P,
	INPROC_AUTH,
};

void LoginFailure(LPDESC d, const char* c_pszStatus);

class CInputProcessor
{
public:
	CInputProcessor();
	virtual ~CInputProcessor() {};

	virtual bool Process(LPDESC d, const void* c_pvOrig, int iBytes, int& r_iBytesProceed);
	virtual BYTE GetType() = 0;

	void BindPacketInfo(CPacketInfo* pPacketInfo);
	void Pong(LPDESC d);
	void Handshake(LPDESC d, const char* c_pData);
	void Version(LPCHARACTER ch, const char* c_pData);

protected:
	virtual int Analyze(LPDESC d, BYTE bHeader, const char* c_pData) = 0;

	CPacketInfo* m_pPacketInfo;
	int m_iBufferLeft;

	CPacketInfoCG m_packetInfoCG;
};

class CInputClose : public CInputProcessor
{
public:
	virtual BYTE GetType() { return INPROC_CLOSE; }

protected:
	virtual int Analyze(LPDESC d, BYTE bHeader, const char* c_pData) { return m_iBufferLeft; }
};

class CInputHandshake : public CInputProcessor
{
public:
	CInputHandshake();
	virtual ~CInputHandshake();

	virtual BYTE GetType() { return INPROC_HANDSHAKE; }

protected:
	virtual int Analyze(LPDESC d, BYTE bHeader, const char* c_pData);

protected:
	void GuildMarkLogin(LPDESC d, const char* c_pData);

	CPacketInfo* m_pMainPacketInfo;
};

class CInputLogin : public CInputProcessor
{
public:
	virtual BYTE GetType() { return INPROC_LOGIN; }

protected:
	virtual int Analyze(LPDESC d, BYTE bHeader, const char* c_pData);

protected:
	void Login(LPDESC d, const char* data);
	void LoginByKey(LPDESC d, const char* data);

	void CharacterSelect(LPDESC d, const char* data);
	void CharacterCreate(LPDESC d, const char* data);
	void CharacterDelete(LPDESC d, const char* data);

	void Entergame(LPDESC d, const char* data);
	void Empire(LPDESC d, const char* c_pData);
	void GuildMarkCRCList(LPDESC d, const char* c_pData);
	// MARK_BUG_FIX
	void GuildMarkIDXList(LPDESC d, const char* c_pData);
	// END_OF_MARK_BUG_FIX
	void GuildMarkUpload(LPDESC d, const char* c_pData);
	int GuildSymbolUpload(LPDESC d, const char* c_pData, size_t uiBytes);
	void GuildSymbolCRC(LPDESC d, const char* c_pData);
	void ChangeName(LPDESC d, const char* data);
};

class CInputMain : public CInputProcessor
{
public:
	virtual BYTE GetType() { return INPROC_MAIN; }

protected:
	virtual int Analyze(LPDESC d, BYTE bHeader, const char* c_pData);

protected:
	void Attack(LPCHARACTER ch, const BYTE header, const char* data);

	int Whisper(LPCHARACTER ch, const char* data, size_t uiBytes);
	int Chat(LPCHARACTER ch, const char* data, size_t uiBytes);
	void ItemUse(LPCHARACTER ch, const char* data);
	void ItemDrop(LPCHARACTER ch, const char* data);
	void ItemDrop2(LPCHARACTER ch, const char* data);
#if defined(__NEW_DROP_DIALOG__)
	void ItemDestroy(LPCHARACTER ch, const char* data);
#endif
	void ItemMove(LPCHARACTER ch, const char* data);
	void ItemPickup(LPCHARACTER ch, const char* data);
	void ItemToItem(LPCHARACTER ch, const char* pcData);
	void QuickslotAdd(LPCHARACTER ch, const char* data);
	void QuickslotDelete(LPCHARACTER ch, const char* data);
	void QuickslotSwap(LPCHARACTER ch, const char* data);
	int Shop(LPCHARACTER ch, const char* data, size_t uiBytes);
	void OnClick(LPCHARACTER ch, const char* data);
	void Exchange(LPCHARACTER ch, const char* data);
	void Position(LPCHARACTER ch, const char* data);
	void Move(LPCHARACTER ch, const char* data);
	int SyncPosition(LPCHARACTER ch, const char* data, size_t uiBytes);
	void FlyTarget(LPCHARACTER ch, const char* pcData, BYTE bHeader);
	void UseSkill(LPCHARACTER ch, const char* pcData);

	void ScriptAnswer(LPCHARACTER ch, const void* pvData);
	void ScriptButton(LPCHARACTER ch, const void* pvData);
	void ScriptSelectItem(LPCHARACTER ch, const void* pvData);
#if defined(__GEM_SYSTEM__)
	void SelectItemEx(LPCHARACTER c_lpCh, const void* c_pvData);
#endif
#if defined(__QUEST_REQUEST_EVENT__)
	void RequestEventQuest(LPCHARACTER pChar, const void* c_pvData);
#endif

	void QuestInputString(LPCHARACTER ch, const void* pvData);
#if defined(__OX_RENEWAL__)
	void QuestInputLongString(LPCHARACTER ch, const void* pvData);
#endif
	void QuestConfirm(LPCHARACTER ch, const void* pvData);
	void Target(LPCHARACTER ch, const char* pcData);
	void Warp(LPCHARACTER ch, const char* pcData);
	void SafeboxCheckin(LPCHARACTER ch, const char* c_pData);
	void SafeboxCheckout(LPCHARACTER ch, const char* c_pData, bool bMall);
	void SafeboxItemMove(LPCHARACTER ch, const char* data);
	int Messenger(const LPCHARACTER c_lpChar, const char* c_pData, std::size_t uiBytes);

	void PartyInvite(LPCHARACTER ch, const char* c_pData);
	void PartyInviteAnswer(LPCHARACTER ch, const char* c_pData);
	void PartyRemove(LPCHARACTER ch, const char* c_pData);
	void PartySetState(LPCHARACTER ch, const char* c_pData);
	void PartyUseSkill(LPCHARACTER ch, const char* c_pData);
	void PartyParameter(LPCHARACTER ch, const char* c_pData);

	int Guild(LPCHARACTER ch, const char* data, size_t uiBytes);
	void AnswerMakeGuild(LPCHARACTER ch, const char* c_pData);

	void Fishing(LPCHARACTER ch, const char* c_pData);

#if defined(__FISHING_GAME__)
	// Fishing Game
	void FishingGame(const LPCHARACTER c_lpChar, const char* c_pszData);
#endif

	void ItemGive(LPCHARACTER ch, const char* c_pData);
	void Hack(LPCHARACTER ch, const char* c_pData);
	int MyShop(LPCHARACTER ch, const char* c_pData, size_t uiBytes);

#if defined(__MYSHOP_DECO__)
	void MyShopDecoState(LPCHARACTER ch, const char* c_pData);
	void MyShopDecoAdd(LPCHARACTER ch, const char* c_pData);
#endif

	void Refine(LPCHARACTER ch, const char* c_pData);

#if defined(__CUBE_RENEWAL__)
	// Cube
	void Cube(const LPCHARACTER pChar, const char* pData);
#endif

#if defined(__SEND_TARGET_INFO__)
	void TargetInfo(LPCHARACTER pChar, const char* c_pszData);
#endif

#if defined(__MOVE_COSTUME_ATTR__)
	// Item Combination
	void ItemCombination(LPCHARACTER ch, const char* c_pData);
	void ItemCombinationCancel(LPCHARACTER ch, const char* c_pData);
#endif

#if defined(__CHANGED_ATTR__)
	// Select Attribute
	void ItemSelectAttr(LPCHARACTER ch, const char* c_pData);
#endif

#if defined(__ACCE_COSTUME_SYSTEM__)
	// Acce Refine
	int AcceRefine(const LPCHARACTER pChar, const char* pszData, size_t uiBytes);
#endif

#if defined(__AURA_COSTUME_SYSTEM__)
	// Aura Refine
	int AuraRefine(const LPCHARACTER pChar, const char* pszData, size_t uiBytes);
#endif

#if defined(__CHANGE_LOOK_SYSTEM__)
	// Change Look
	void ChangeLook(LPCHARACTER lpCh, const char* c_pszData);
#endif

#if defined(__PRIVATESHOP_SEARCH_SYSTEM__)
	// Private Shop Search
	void PrivateShopSearch(LPCHARACTER ch, const char* data);
	void PrivateShopSearchClose(LPCHARACTER ch, const char* data);
	void PrivateShopSearchBuyItem(LPCHARACTER ch, const char* data);
#endif

#if defined(__SKILLBOOK_COMB_SYSTEM__)
	// Skill Book Combination
	bool SkillBookCombination(LPCHARACTER ch, TItemPos(&CombItemGrid)[SKILLBOOK_COMB_SLOT_MAX], BYTE bAction);
#endif

#if defined(__MAILBOX__)
	// Mail Box
	void MailboxWrite(LPCHARACTER ch, const char* data);
	void MailboxConfirm(LPCHARACTER ch, const char* data);
	void MailboxProcess(LPCHARACTER ch, const char* c_pData);
#endif

#if defined(__LOOT_FILTER_SYSTEM__)
	// Loot Filter
	void LootFilter(LPCHARACTER ch, const char* c_pData);
#endif

#if defined(__MINI_GAME_RUMI__)
	// Mini-Game Rumi
	void MiniGameRumi(LPCHARACTER pChar, const char* pszData);
#endif

#if defined(__MINI_GAME_YUTNORI__)
	// Mini-Game Yutnori
	void MiniGameYutnori(LPCHARACTER pChar, const char* pszData);
#endif

#if defined(__ATTR_6TH_7TH__)
	// 6th and 7th Attribute
	void Attr67Add(LPCHARACTER ch, const char* c_pData);
#endif

#if defined(__LUCKY_BOX__)
	// Lucky Box
	void LuckyBox(LPCHARACTER ch, const char* c_pData);
#endif

#if defined(__GEM_SHOP__)
	// Gem Shop
	void GemShop(LPCHARACTER c_lpCh, const char* c_pszData);
#endif

#if defined(__EXTEND_INVEN_SYSTEM__)
	// Extend Inven
	void ExtendInven(LPCHARACTER pChar, const char* c_pszData);
#endif

#if defined(__SNOWFLAKE_STICK_EVENT__)
	// Snowflake Stick Event
	void SnowflakeStickEvent(LPCHARACTER pChar, const char* c_pszData);
#endif

#if defined(__REFINE_ELEMENT_SYSTEM__)
	// Refine Element
	void RefineElement(LPCHARACTER pChar, const char* c_pszData);
#endif

#if defined(__LEFT_SEAT__)
	// Left Seat
	void LeftSeat(LPCHARACTER pChar, const char* c_pszData);
#endif

#if defined(__SUMMER_EVENT_ROULETTE__)
	// Mini-Game Roulette (Late Summer Event)
	void MiniGameRoulette(LPCHARACTER pChar, const char* pszData);
#endif

#if defined(__FLOWER_EVENT__)
	// Flower Event
	void FlowerEvent(LPCHARACTER pChar, const char* pszData);
#endif
};

class CInputDead : public CInputMain
{
public:
	virtual BYTE GetType() { return INPROC_DEAD; }

protected:
	virtual int Analyze(LPDESC d, BYTE bHeader, const char* c_pData);
};

class CInputDB : public CInputProcessor
{
public:
	virtual bool Process(LPDESC d, const void* c_pvOrig, int iBytes, int& r_iBytesProceed);
	virtual BYTE GetType() { return INPROC_DB; }

protected:
	virtual int Analyze(LPDESC d, BYTE bHeader, const char* c_pData);

protected:
	void MapLocations(const char* c_pData);
	void LoginSuccess(DWORD dwHandle, const char* data);
	void PlayerCreateFailure(LPDESC d, BYTE bType); // 0 = 일반 실패 1 = 이미 있음
	void PlayerDeleteSuccess(LPDESC d, const char* data);
	void PlayerDeleteFail(LPDESC d
#if defined(__DELETE_FAILURE_TYPE__)
		, const char* c_pszData
#endif
	);
	void PlayerLoad(LPDESC d, const char* data);
	void PlayerCreateSuccess(LPDESC d, const char* data);
	void Boot(const char* data);
	void QuestLoad(LPDESC d, const char* c_pData);
	void SafeboxLoad(LPDESC d, const char* c_pData);
	void SafeboxChangeSize(LPDESC d, const char* c_pData);
	void SafeboxWrongPassword(LPDESC d);
	void SafeboxChangePasswordAnswer(LPDESC d, const char* c_pData);
	void MallLoad(LPDESC d, const char* c_pData);
	void EmpireSelect(LPDESC d, const char* c_pData);
	void P2P(const char* c_pData);
	void ItemLoad(LPDESC d, const char* c_pData);
	void AffectLoad(LPDESC d, const char* c_pData);

	void GuildLoad(const char* c_pData);
	void GuildSkillUpdate(const char* c_pData);
	void GuildSkillRecharge();
	void GuildExpUpdate(const char* c_pData);
	void GuildAddMember(const char* c_pData);
	void GuildRemoveMember(const char* c_pData);
	void GuildChangeGrade(const char* c_pData);
	void GuildChangeMemberData(const char* c_pData);
	void GuildDisband(const char* c_pData);
	void GuildLadder(const char* c_pData);
	void GuildWar(const char* c_pData);
	void GuildWarScore(const char* c_pData);
	void GuildSkillUsableChange(const char* c_pData);
	void GuildMoneyChange(const char* c_pData);
	void GuildWithdrawMoney(const char* c_pData);
	void GuildWarReserveAdd(TGuildWarReserve* p);
	void GuildWarReserveUpdate(TGuildWarReserve* p);
	void GuildWarReserveDelete(DWORD dwID);
	void GuildWarBet(TPacketGDGuildWarBet* p);
	void GuildChangeMaster(TPacketChangeGuildMaster* p);

	void LoginAlready(LPDESC d, const char* c_pData);

	void PartyCreate(const char* c_pData);
	void PartyDelete(const char* c_pData);
	void PartyAdd(const char* c_pData);
	void PartyRemove(const char* c_pData);
	void PartyStateChange(const char* c_pData);
	void PartySetMemberLevel(const char* c_pData);

	void Time(const char* c_pData);

	void ReloadProto(const char* c_pData);
	void ChangeName(LPDESC d, const char* data);

	void AuthLogin(LPDESC d, const char* c_pData);
	void ItemAward(const char* c_pData);

	void ChangeEmpirePriv(const char* c_pData);
	void ChangeGuildPriv(const char* c_pData);
	void ChangeCharacterPriv(const char* c_pData);

	void MoneyLog(const char* c_pData);

	void SetEventFlag(const char* c_pData);

	void CreateObject(const char* c_pData);
	void DeleteObject(const char* c_pData);
	void UpdateLand(const char* c_pData);

	void Notice(const char* c_pData);

	void MarriageAdd(TPacketMarriageAdd* p);
	void MarriageUpdate(TPacketMarriageUpdate* p);
	void MarriageRemove(TPacketMarriageRemove* p);

	void WeddingRequest(TPacketWeddingRequest* p);
	void WeddingReady(TPacketWeddingReady* p);
	void WeddingStart(TPacketWeddingStart* p);
	void WeddingEnd(TPacketWeddingEnd* p);

	void TakeMonarchMoney(LPDESC d, const char* data);
	void AddMonarchMoney(LPDESC d, const char* data);
	void DecMonarchMoney(LPDESC d, const char* data);
	void SetMonarch(LPDESC d, const char* data);

	void ChangeMonarchLord(TPacketChangeMonarchLordACK* data);
	void UpdateMonarchInfo(TMonarchInfo* data);
	void AddBlockCountryIp(TPacketBlockCountryIp* data);
	void BlockException(TPacketBlockException* data);

	// MYSHOP_PRICE_LIST
	/// 아이템 가격정보 리스트 요청에 대한 응답 패킷(HEADER_DG_MYSHOP_PRICELIST_RES) 처리함수
	/**
	* @param d 아이템 가격정보 리스트를 요청한 플레이어의 descriptor
	* @param p 패킷데이터의 포인터
	*/
	void MyshopPricelistRes(LPDESC d, const TPacketMyshopPricelistHeader* p);
	// END_OF_MYSHOP_PRICE_LIST

	//RELOAD_ADMIN
	void ReloadAdmin(const char* c_pData);
	//END_RELOAD_ADMIN

	void DetailLog(const TPacketNeedLoginLogInfo* info);
	// 독일 선물 기능 테스트
	void ItemAwardInformer(TPacketItemAwardInfromer* data);

	void RespondChannelStatus(LPDESC desc, const char* pcData);

#if defined(__MOVE_CHANNEL__)
	void MoveChannel(LPDESC desc, const char* pcData);
#endif

#if defined(__EXPRESSING_EMOTIONS__)
	// Emotions
	void EmoteLoad(LPDESC pDesc, const char* c_pData);
	void EmoteGet(LPDESC pDesc, const char* c_pData);
#endif

#if defined(__MAILBOX__)
	void MailBoxRespondLoad(LPDESC d, const char* c_pData);
	void MailBoxRespondName(LPDESC d, const char* c_pData);
	void MailBoxRespondUnreadData(LPDESC d, const char* c_pData);
#endif

#if defined(__GEM_SHOP__)
	// Gem Shop
	void GemShopLoad(LPDESC lpDesc, const char* c_pszData);
	void GemShopUpdate(LPDESC lpDesc, const char* c_pszData);
#endif

#if defined(__GUILD_EVENT_FLAG__) //&& defined(__GUILD_RENEWAL__)
	void GuildSetEventFlag(const char* c_pData);
#endif

protected:
	DWORD m_dwHandle;
};

class CInputUDP : public CInputProcessor
{
public:
	CInputUDP();
	virtual bool Process(LPDESC d, const void* c_pvOrig, int iBytes, int& r_iBytesProceed);

	virtual BYTE GetType() { return INPROC_UDP; }
	void SetSockAddr(struct sockaddr_in& rSockAddr) { m_SockAddr = rSockAddr; };

protected:
	virtual int Analyze(LPDESC d, BYTE bHeader, const char* c_pData);

protected:
	void Handshake(LPDESC lpDesc, const char* c_pData);
	void StateChecker(const char* c_pData);

protected:
	struct sockaddr_in m_SockAddr;
	CPacketInfoUDP m_packetInfoUDP;
};

class CInputP2P : public CInputProcessor
{
public:
	CInputP2P();
	virtual BYTE GetType() { return INPROC_P2P; }

protected:
	virtual int Analyze(LPDESC d, BYTE bHeader, const char* c_pData);

public:
	void Setup(LPDESC d, const char* c_pData);
	void Login(LPDESC d, const char* c_pData);
	void Logout(LPDESC d, const char* c_pData);
	int Relay(LPDESC d, const char* c_pData, size_t uiBytes);
	int Notice(LPDESC d, const char* c_pData, size_t uiBytes);
	int MonarchNotice(LPDESC d, const char* c_pData, size_t uiBytes);
	int MonarchTransfer(LPDESC d, const char* c_pData);
	int Guild(LPDESC d, const char* c_pData, size_t uiBytes);
	void Shout(const char* c_pData);
	void Disconnect(const char* c_pData);
	void MessengerAdd(const char* c_pData);
	void MessengerRemove(const char* c_pData);
#if defined(__MESSENGER_BLOCK_SYSTEM__)
	void MessengerBlockAdd(const char* c_pData);
	void MessengerBlockRemove(const char* c_pData);
#endif
	void FindPosition(LPDESC d, const char* c_pData);
	void WarpCharacter(const char* c_pData);
	void GuildWarZoneMapIndex(const char* c_pData);
	void Transfer(const char* c_pData);
#if defined(__XMAS_EVENT_2008__)
	void XmasWarpSanta(const char* c_pData);
	void XmasWarpSantaReply(const char* c_pData);
#endif
	void LoginPing(LPDESC d, const char* c_pData);
	void BlockChat(const char* c_pData);
	void PCBangUpdate(const char* c_pData);
	void IamAwake(LPDESC d, const char* c_pData);

protected:
	CPacketInfoGG m_packetInfoGG;
};

class CInputAuth : public CInputProcessor
{
public:
	CInputAuth();
	virtual BYTE GetType() { return INPROC_AUTH; }

protected:
	virtual int Analyze(LPDESC d, BYTE bHeader, const char* c_pData);

public:
	void Login(LPDESC d, const char* c_pData);

};
#endif // __INC_INPUT_PROCESSOR_H__
