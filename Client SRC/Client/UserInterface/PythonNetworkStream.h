#pragma once

#include "../EterLib/FuncObject.h"
#include "../Eterlib/NetStream.h"
#include "../EterLib/NetPacketHeaderMap.h"

#include "InsultChecker.h"

#include "Packet.h"

class CInstanceBase;
class CNetworkActorManager;
struct SNetworkActorData;
struct SNetworkUpdateActorData;

class CPythonNetworkStream : public CNetworkStream, public CSingleton<CPythonNetworkStream>
{
public:
	enum
	{
		SERVER_COMMAND_LOG_OUT = 0,
		SERVER_COMMAND_RETURN_TO_SELECT_CHARACTER = 1,
		SERVER_COMMAND_QUIT = 2,

		MAX_ACCOUNT_PLAYER
	};

	enum
	{
		ERROR_NONE,
		ERROR_UNKNOWN,
		ERROR_CONNECT_MARK_SERVER,
		ERROR_LOAD_MARK,
		ERROR_MARK_WIDTH,
		ERROR_MARK_HEIGHT,

		// MARK_BUG_FIX
		ERROR_MARK_UPLOAD_NEED_RECONNECT,
		ERROR_MARK_CHECK_NEED_RECONNECT,
		// END_OF_MARK_BUG_FIX
	};

	enum
	{
		ACCOUNT_CHARACTER_SLOT_ID,
		ACCOUNT_CHARACTER_SLOT_NAME,
		ACCOUNT_CHARACTER_SLOT_RACE,
		ACCOUNT_CHARACTER_SLOT_LEVEL,
		ACCOUNT_CHARACTER_SLOT_STR,
		ACCOUNT_CHARACTER_SLOT_DEX,
		ACCOUNT_CHARACTER_SLOT_HTH,
		ACCOUNT_CHARACTER_SLOT_INT,
		ACCOUNT_CHARACTER_SLOT_PLAYTIME,
		ACCOUNT_CHARACTER_SLOT_FORM,
		ACCOUNT_CHARACTER_SLOT_ADDR,
		ACCOUNT_CHARACTER_SLOT_PORT,
		ACCOUNT_CHARACTER_SLOT_GUILD_ID,
		ACCOUNT_CHARACTER_SLOT_GUILD_NAME,
		ACCOUNT_CHARACTER_SLOT_CHANGE_NAME_FLAG,
		ACCOUNT_CHARACTER_SLOT_HAIR,
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
		ACCOUNT_CHARACTER_SLOT_ACCE,
#endif
		ACCOUNT_CHARACTER_SLOT_LAST_PLAYTIME,
#if defined(ENABLE_CONQUEROR_LEVEL)
		ACCOUNT_CHARACTER_SLOT_CONQUEROR_LEVEL,
		ACCOUNT_CHARACTER_SLOT_SUNGMA_STR,
		ACCOUNT_CHARACTER_SLOT_SUNGMA_HP,
		ACCOUNT_CHARACTER_SLOT_SUNGMA_MOVE,
		ACCOUNT_CHARACTER_SLOT_SUNGMA_IMMUNE,
#endif
	};

	enum
	{
		PHASE_WINDOW_LOGO,
		PHASE_WINDOW_LOGIN,
		PHASE_WINDOW_SELECT,
		PHASE_WINDOW_CREATE,
		PHASE_WINDOW_LOAD,
		PHASE_WINDOW_GAME,
		PHASE_WINDOW_EMPIRE,
		PHASE_WINDOW_NUM,
	};

public:
	CPythonNetworkStream();
	virtual ~CPythonNetworkStream();

	bool SendSpecial(int nLen, void* pvBuf);

	void StartGame();
	void Warp(LONG lGlobalX, LONG lGlobalY);

	void NotifyHack(const char* c_szMsg);
	void SetWaitFlag();

#if defined(ENABLE_DISCORD_RPC)
	void Discord_Start();
	void Discord_Close();
	void Discord_Update(const bool bGameState);
#endif

	void SendEmoticon(UINT eEmoticon);

	void ExitApplication();
	void ExitGame();
#if defined(ENABLE_LOCALE_CLIENT)
	void ExitGameLanguageChange();
#endif
	void LogOutGame();
	void AbsoluteExitGame();
	void AbsoluteExitApplication();

	void EnableChatInsultFilter(bool isEnable);
	bool IsChatInsultIn(const char* c_szMsg);
	bool IsInsultIn(const char* c_szMsg);

	DWORD GetGuildID();

	UINT UploadMark(const char* c_szImageFileName);
	UINT UploadSymbol(const char* c_szImageFileName);

	void RefreshGuildMark();

	bool LoadInsultList(const char* c_szInsultListFileName);
	bool LoadConvertTable(DWORD dwEmpireID, const char* c_szFileName);

public:
	UINT GetAccountCharacterSlotDatau(UINT iSlot, UINT eType);
	const char* GetAccountCharacterSlotDataz(UINT iSlot, UINT eType);

	// SUPPORT_BGM
	const char* GetFieldMusicFileName();
	float GetFieldMusicVolume();
	// END_OF_SUPPORT_BGM

	bool IsSelectedEmpire();

	void ToggleGameDebugInfo();

	void SetMarkServer(const char* c_szAddr, UINT uPort);
	void ConnectLoginServer(const char* c_szAddr, UINT uPort);
	void ConnectGameServer(UINT iChrSlot);

	void SetLoginInfo(const char* c_szID, const char* c_szPassword);
	std::string GetLoginId();
	void SetLoginKey(DWORD dwLoginKey);
	void ClearLoginInfo(void);

	void SetHandler(PyObject* poHandler);
	void SetPhaseWindow(UINT ePhaseWnd, PyObject* poPhaseWnd);
	void ClearPhaseWindow(UINT ePhaseWnd, PyObject* poPhaseWnd);
	void SetServerCommandParserWindow(PyObject* poPhaseWnd);

	bool SendSyncPositionElementPacket(DWORD dwVictimVID, DWORD dwVictimX, DWORD dwVictimY);

	bool SendAttackPacket(UINT uMotAttack, DWORD dwVIDVictim);
	bool SendCharacterStatePacket(const TPixelPosition& c_rkPPosDst, float fDstRot, UINT eFunc, UINT uArg);
	bool SendUseSkillPacket(DWORD dwSkillIndex, DWORD dwTargetVID = 0);

	bool SendTargetPacket(DWORD dwVID);

	// OLDCODE:
	bool SendCharacterStartWalkingPacket(float fRotation, long lx, long ly);
	bool SendCharacterEndWalkingPacket(float fRotation, long lx, long ly);
	bool SendCharacterCheckWalkingPacket(float fRotation, long lx, long ly);

	bool SendCharacterPositionPacket(BYTE iPosition);

	bool SendItemUsePacket(TItemPos pos);
	bool SendItemUseToItemPacket(TItemPos source_pos, TItemPos target_pos);
#if defined(ENABLE_CHEQUE_SYSTEM)
	bool SendItemDropPacket(TItemPos pos, DWORD elk, DWORD cheque);
	bool SendItemDropPacketNew(TItemPos pos, DWORD elk, DWORD cheque, DWORD count);
#else
	bool SendItemDropPacket(TItemPos pos, DWORD elk);
	bool SendItemDropPacketNew(TItemPos pos, DWORD elk, DWORD count);
#endif
#if defined(ENABLE_NEW_DROP_DIALOG)
	bool SendItemDestroyPacket(TItemPos pos);
#endif
	bool SendItemMovePacket(TItemPos pos, TItemPos change_pos, WORD num);
	bool SendItemPickUpPacket(DWORD vid);

	bool SendQuickSlotAddPacket(BYTE wpos, BYTE type, int pos);
	bool SendQuickSlotDelPacket(BYTE wpos);
	bool SendQuickSlotMovePacket(BYTE wpos, BYTE change_pos);

	// PointReset 개 임시
	bool SendPointResetPacket();

	// Shop
	bool SendShopEndPacket();
	bool SendShopBuyPacket(BYTE byCount);
	bool SendShopSellPacket(BYTE bySlot);
	bool SendShopSellPacketNew(WORD wSlot, WORD wCount, BYTE byType);

#if defined(ENABLE_MYSHOP_DECO)
	// MyShop Decoration
	bool SendMyShopDecoState(BYTE bState);
	bool SendMyShopDecoSet(BYTE bType, DWORD dwPolyVnum);
#endif

	// Exchange
	bool SendExchangeStartPacket(DWORD vid);
	bool SendExchangeItemAddPacket(TItemPos ItemPos, BYTE byDisplayPos);
	bool SendExchangeElkAddPacket(DWORD elk);
#if defined(ENABLE_CHEQUE_SYSTEM)
	bool SendExchangeChequeAddPacket(DWORD cheque);
#endif
	bool SendExchangeItemDelPacket(BYTE pos);
	bool SendExchangeAcceptPacket();
	bool SendExchangeExitPacket();

	// Quest
	bool SendScriptAnswerPacket(int iAnswer);
	bool SendScriptButtonPacket(unsigned int iIndex);
	bool SendAnswerMakeGuildPacket(const char* c_szName);
	bool SendQuestInputStringPacket(const char* c_szString);
#if defined(ENABLE_OX_RENEWAL)
	bool SendQuestInputStringLongPacket(const char* c_szString);
#endif
	bool SendQuestConfirmPacket(BYTE byAnswer, DWORD dwPID);

	// Event
	bool SendOnClickPacket(DWORD vid);

	// Fly
	bool SendFlyTargetingPacket(DWORD dwTargetVID, const TPixelPosition& kPPosTarget);
	bool SendAddFlyTargetingPacket(DWORD dwTargetVID, const TPixelPosition& kPPosTarget);
	bool SendShootPacket(UINT uSkill);

	// Command
	bool ClientCommand(const char* c_szCommand);
	void ServerCommand(char* c_szCommand);

	// Emoticon
	void RegisterEmoticonType(const BYTE bEmoticonType);

	// Emote
	bool RecvEmote();

	// Party
	bool SendPartyInvitePacket(DWORD dwVID);
	bool SendPartyInviteAnswerPacket(DWORD dwLeaderVID, BYTE byAccept);
	bool SendPartyRemovePacket(DWORD dwPID);
	bool SendPartySetStatePacket(DWORD dwVID, BYTE byState, BYTE byFlag);
	bool SendPartyUseSkillPacket(BYTE bySkillIndex, DWORD dwVID);
	bool SendPartyParameterPacket(BYTE byDistributeMode);

	// SafeBox
	bool SendSafeBoxMoneyPacket(BYTE byState, DWORD dwMoney);
	bool SendSafeBoxCheckinPacket(TItemPos InventoryPos, BYTE bySafeBoxPos);
	bool SendSafeBoxCheckoutPacket(BYTE bySafeBoxPos, TItemPos InventoryPos);
	bool SendSafeBoxItemMovePacket(BYTE bySourcePos, BYTE byTargetPos, WORD wCount);

	// Mall
	bool SendMallCheckoutPacket(BYTE byMallPos, TItemPos InventoryPos);

	// Guild
	bool SendGuildAddMemberPacket(DWORD dwVID);
	bool SendGuildRemoveMemberPacket(DWORD dwPID);
	bool SendGuildChangeGradeNamePacket(BYTE byGradeNumber, const char* c_szName);
	bool SendGuildChangeGradeAuthorityPacket(BYTE byGradeNumber, BYTE byAuthority);
	bool SendGuildOfferPacket(DWORD dwExperience);
	bool SendGuildPostCommentPacket(const char* c_szMessage);
	bool SendGuildDeleteCommentPacket(DWORD dwIndex);
	bool SendGuildRefreshCommentsPacket(DWORD dwHighestIndex);
	bool SendGuildChangeMemberGradePacket(DWORD dwPID, BYTE byGrade);
	bool SendGuildUseSkillPacket(DWORD dwSkillID, DWORD dwTargetVID);
	bool SendGuildChangeMemberGeneralPacket(DWORD dwPID, BYTE byFlag);
	bool SendGuildInvitePacket(DWORD dwVID);
	bool SendGuildInviteAnswerPacket(DWORD dwGuildID, BYTE byAnswer);
	bool SendGuildChargeGSPPacket(DWORD dwMoney);
	bool SendGuildDepositMoneyPacket(DWORD dwMoney);
	bool SendGuildWithdrawMoneyPacket(DWORD dwMoney);

	// Mall
	bool RecvMallOpenPacket();
	bool RecvMallItemSetPacket();
	bool RecvMallItemDelPacket();

	// Lover
	bool RecvLoverInfoPacket();
	bool RecvLovePointUpdatePacket();

	// Dig
	bool RecvDigMotionPacket();

	// Fishing
	bool SendFishingPacket(int iRotation);

#if defined(ENABLE_GEM_SYSTEM)
	// Gem Shop
	bool RecvGemShop();
	bool RecvGemShopProcess();

	bool SendGemShopCommand(const BYTE c_bSubheader, const BYTE c_bSlotIndex = GEM_SHOP_SLOT_COUNT);
#endif

#if defined(ENABLE_FISHING_GAME)
	// Fishing Game
	bool RecvFishingGamePacket();
	bool SendFishingGamePacket(BYTE bSubHeader, BYTE bGoals = 0);
#endif

	bool SendGiveItemPacket(DWORD dwTargetVID, TItemPos ItemPos, int iItemCount);

#if defined(ENABLE_ATTR_6TH_7TH)
	// 6th and 7th Attribute
	bool SendAttr67AddPacket(const TAttr67AddData* pkAttr67AddData, bool bAddPacket);
	bool SendAttr67ClosePacket();
#endif

#if defined(ENABLE_LUCKY_BOX)
	// Lucky Box
	bool SendLuckyBoxActionPacket(BYTE bAction);
	bool RecvLuckyBoxPacket();
#endif

#if defined(ENABLE_RANKING_SYSTEM)
	// Ranking
	bool RecvRanking();
#endif

#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	// Acce Refine
public:
	bool SendAcceRefineCheckIn(TItemPos AttachedPos, TItemPos SelectedPos, BYTE bType);
	bool SendAcceRefineCheckOut(TItemPos SelectedPos, BYTE bType);
	bool SendAcceRefineAccept(BYTE bType);
	bool SendAcceRefineCancel();
protected:
	bool RecvAcceRefinePacket();
#endif

#if defined(ENABLE_AURA_COSTUME_SYSTEM)
	// Aura System
public:
	bool SendAuraRefineCheckIn(TItemPos AttachedPos, TItemPos SelectedPos, BYTE bType);
	bool SendAuraRefineCheckOut(TItemPos SelectedPos, BYTE bType);
	bool SendAuraRefineAccept(BYTE bType);
	bool SendAuraRefineCancel();
protected:
	bool RecvAuraRefinePacket();
#endif

#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
	// Change Look
public:
	bool SendChangeLookCheckInPacket(const TItemPos& c_rItemPos, const BYTE c_bSlotIndex);
	bool SendChangeLookCheckOutPacket(const BYTE c_bSlotIndex);
	bool SendChangeLookCheckInFreeYangItemPacket(const TItemPos& c_rItemPos);
	bool SendChangeLookCheckOutFreeYangItemPacket();
	bool SendChangeLookAcceptPacket();
	bool SendChangeLookCancelPacket();
protected:
	bool RecvChangeLookItemSetPacket();
	bool RecvChangeLookItemDelPacket();
	bool RecvChangeLookFreeItemSetPacket();
	bool RecvChangeLookFreeItemDelPacket();
#endif

	// Private Shop
public:
	bool SendBuildPrivateShopPacket(const char* c_szName, const std::vector<TShopItemTable>& c_rSellingItemStock);
#if defined(ENABLE_PRIVATESHOP_SEARCH_SYSTEM)
	// Private Shop Search
	bool SendPrivateShopSearchClose();
	bool SendPrivateShopSearchBuyItem(int iIndex);
	void PrivateShopSearchChangePage(int iPage);
	bool SendPrivateShopSearchInfoPacket(BYTE, BYTE, int, int, int, int, int, int, int, const char*
#if defined(ENABLE_CHEQUE_SYSTEM)
		, int, int
#endif
	);
#endif

	// Refine
	bool SendRefinePacket(BYTE byPos, BYTE byType);
	bool SendSelectItemPacket(DWORD dwItemPos);
#if defined(ENABLE_GEM_SYSTEM)
	bool SendSelectMetinStone(DWORD dwItemPos);
#endif

#if defined(ENABLE_MOVE_COSTUME_ATTR)
	// Item Combination
	bool SendItemCombinationPacket(const int iMediumIndex, const int iBaseIndex, const int iMaterialIndex);
	bool SendItemCombinationPacketCancel();
#endif

#if defined(ENABLE_CHANGED_ATTR)
	// Select Attribute
	bool RecvSelectAttr();
	bool SendChagedItemAttributePacket(const bool bNew, const TItemPos& pos);
#endif

#if defined(ENABLE_LOOTING_SYSTEM)
	// Loot Filter
	bool SendLootFilterPacket(BYTE(&settings)[ELootFilter::LOOT_FILTER_SETTINGS_MAX]);
	bool RecvLootFilterPacket();
#endif

#if defined(ENABLE_CUBE_RENEWAL)
	// Cube
	bool SendCubeClosePacket();
	bool SendCubeMakePacket(const UINT iCubeIndex, const UINT iQuantity = 1, const INT iImproveItemPos = -1);
	bool RecvCubePacket();
#endif

#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	// Refine Element System
	bool SendRefineElementPacket(BYTE bSubHeader, WORD wElementType);
	bool RecvRefineElementPacket();
#endif

#if defined(ENABLE_GUILD_DRAGONLAIR_SYSTEM)
	// Guild DragonLair
	bool RecvGuildDragonLairPacket();
#endif

#if defined(ENABLE_SUMMER_EVENT_ROULETTE)
	// Summer Event Roulette
	bool SendMiniGameRoulettePacket(BYTE bSubHeader);
	bool RecvMiniGameRoulettePacket();
#endif

#if defined(ENABLE_FLOWER_EVENT)
	// Flower Event
	bool SendFlowerEventPacket(BYTE bSubHeader, BYTE bShootType = SHOOT_TYPE_MAX, BYTE bExchangeKey = 0);
	bool RecvFlowerEventPacket();
#endif

	// Client Version
	bool SendClientVersionPacket();

	// CRC Report
	bool __SendCRCReportPacket();

#if defined(ENABLE_DRAGON_SOUL_SYSTEM)
	// 용홍석 강화
	bool SendDragonSoulRefinePacket(BYTE bRefineType, TItemPos* pos);
#endif

	// Handshake
	bool RecvHandshakePacket();
	bool RecvHandshakeOKPacket();

	bool RecvHybridCryptKeyPacket();
	bool RecvHybridCryptSDBPacket();
#if defined(__IMPROVED_PACKET_ENCRYPTION__)
	bool RecvKeyAgreementPacket();
	bool RecvKeyAgreementCompletedPacket();
#endif

	// ETC
	DWORD GetMainActorVID();
	DWORD GetMainActorRace();
	DWORD GetMainActorEmpire();
	DWORD GetMainActorSkillGroup();
	void SetEmpireID(DWORD dwEmpireID);
	DWORD GetEmpireID();
	void __TEST_SetSkillGroupFake(int iIndex);

#if defined(ENABLE_MAILBOX)
	// MailBox
public:
	bool RecvMailboxProcess();
	bool RecvMailbox();
	bool RecvMailboxAddData();
	bool RecvMailboxAll();
	bool RecvMailboxUnread();

	bool SendPostDelete(const BYTE Index);
	bool SendPostGetItems(const BYTE Index);
	bool SendPostWriteConfirm(const char* szName);
	bool SendPostWrite(const char* szName, const char* szTitle, const char* szMessage, const TItemPos& pos, const int iYang, const int iWon);
	bool SendMailBoxClose();
	bool SendPostAllDelete();
	bool SendPostAllGetItems();
	bool RequestPostAddData(const BYTE ButtonIndex, const BYTE DataIndex);
#endif

#if defined(ENABLE_MOVE_CHANNEL)
	// Move Channel
public:
	void SetMapIndex(int iMapIndex);
	int GetMapIndex();

	void SetChannelNumber(BYTE bChannelNumber);
	BYTE GetChannelNumber();

protected:
	BYTE m_bChannelNumber;
	int m_iMapIndex;
#endif

#if defined(ENABLE_QUEST_REQUEST_EVENT)
	// Request Event Quest
public:
	bool SendRequestEventQuest(const char* c_pszEventQuest);
#endif

#if defined(ENABLE_LEFT_SEAT)
	// Left Seat
public:
	bool SendLeftSeatWaitTimeIndexPacket(BYTE bIndex);
	bool SendLeftSeatOffPacket(BYTE bIndex);
	bool DisableLeftSeatLogOutState();
#endif

#if defined(ENABLE_EXTEND_INVEN_SYSTEM)
	// Extended Inventory
public:
	bool SendExtendInven(bool bExtend, BYTE bIndex = 0);

	bool RecvExtendInven();
	bool RecvExtendInvenItemUse();
#endif

#if defined(ENABLE_CLIENT_TIMER)
public:
	bool RecvClientTimer();
#endif

#if defined(ENABLE_MINI_GAME_RUMI)
	// Mini-Game Rumi
public:
	bool RecvMiniGameRumi();

	bool SendMiniGameRumiExit();
	bool SendMiniGameRumiStart();
	bool SendMiniGameRumiDeckCardClick();
	bool SendMiniGameRumiHandCardClick(bool bUse, BYTE bIndex);
	bool SendMiniGameRumiFieldCardClick(BYTE bIndex);
#if defined(ENABLE_OKEY_EVENT_FLAG_RENEWAL)
	bool SendMiniGameRumiRequestQuestFlag();
#endif
#endif

#if defined(ENABLE_MINI_GAME_YUTNORI)
	// Mini-Game Yutnori
public:
	bool RecvMiniGameYutnori();

	bool SendMiniGameYutnoriStart();
	bool SendMiniGameYutnoriGiveup();
	bool SendMiniGameYutnoriProb(BYTE bProbIndex);
	bool SendMiniGameYutnoriCharClick(BYTE bPlayerIndex);
	bool SendMiniGameYutnoriThrow(BYTE bPC);
	bool SendMiniGameYutnoriMove(BYTE bPlayerIndex);
	bool SendMiniGameYutnoriReward();
	bool SendMiniGameYutnoriRequestComAction();
#if defined(ENABLE_YUTNORI_EVENT_FLAG_RENEWAL)
	bool SendMiniGameYutnoriRequestQuestFlag();
#endif
#endif

#if defined(ENABLE_MINI_GAME_CATCH_KING)
	// MiniGame Catch King
public:
	bool RecvMiniGameCatchKingPacket();

	bool SendMiniGameCatchKing(BYTE bSubHeader, BYTE bSubArgument);
#if defined(ENABLE_CATCH_KING_EVENT_FLAG_RENEWAL)
	bool SendMiniGameCatchKingRequestQuestFlag();
#endif
#endif

#if defined(ENABLE_SNOWFLAKE_STICK_EVENT)
	// Snowflake Stick Event
public:
	bool SendSnowflakeStickEventPacket(BYTE bSubHeader);
	bool RecvSnowflakeStickEventPacket();
#endif

	//////////////////////////////////////////////////////////////////////////
	// Phase 관련
	//////////////////////////////////////////////////////////////////////////
public:
	void SetOffLinePhase();
	void SetHandShakePhase();
	void SetLoginPhase();
	void SetSelectPhase();
	void SetLoadingPhase();
	void SetGamePhase();
	void ClosePhase();

	// Login Phase
	bool SendLoginPacket(const char* c_szName, const char* c_szPassword);
	bool SendLoginPacketNew(const char* c_szName, const char* c_szPassword);
	//bool SendDirectEnterPacket(const char * c_szName, const char * c_szPassword, UINT uChrSlot);

	bool SendEnterGame();

	// Select Phase
	bool SendSelectEmpirePacket(DWORD dwEmpireID);
	bool SendSelectCharacterPacket(BYTE index);
	bool SendChangeNamePacket(BYTE index, const char* name);
	bool SendCreateCharacterPacket(BYTE index, const char* name, BYTE job, BYTE shape, BYTE byStat1, BYTE byStat2, BYTE byStat3, BYTE byStat4);
	bool SendDestroyCharacterPacket(BYTE index, const char* szPrivateCode);

	// Main Game Phase
	bool SendC2CPacket(DWORD dwSize, void* pData);
	bool SendChatPacket(const char* c_szChat, BYTE byType = CHAT_TYPE_TALKING);
	bool SendWhisperPacket(const char* name, const char* c_szChat);
	bool SendCommandPacket(const char* c_szChat);

	// Messenger
	bool SendMessengerAddByVIDPacket(const DWORD c_dwVID);
	bool SendMessengerAddByNamePacket(const char* c_szName);
	bool SendMessengerRemovePacket(const char* c_szKey, const char* c_szName);
#if defined(ENABLE_MESSENGER_BLOCK)
	bool SendMessengerBlockAddByVIDPacket(const DWORD c_dwVID);
	bool SendMessengerBlockAddByNamePacket(const char* c_szName);
	bool SendMessengerBlockRemovePacket(const char* c_szKey, const char* c_szName);
	bool SendMessengerBlockRemoveByVIDPacket(const DWORD c_dwVID);
#endif

protected:
	bool OnProcess(); // State들을 실제로 실행한다.
	void OffLinePhase();
	void HandShakePhase();
	void LoginPhase();
	void SelectPhase();
	void LoadingPhase();
	void GamePhase();

	bool __IsNotPing();

	void __DownloadMark();
	void __DownloadSymbol(const std::vector<DWORD>& c_rkVec_dwGuildID);

	void __PlayInventoryItemUseSound(TItemPos uSlotPos);
	void __PlayInventoryItemDropSound(TItemPos uSlotPos);
	//void __PlayShopItemDropSound(UINT uSlotPos);
	void __PlaySafeBoxItemDropSound(UINT uSlotPos);
	void __PlayMallItemDropSound(UINT uSlotPos);

	bool __CanActMainInstance();

	enum REFRESH_WINDOW_TYPE
	{
		RefreshStatus = (1 << 0),
		RefreshAlignmentWindow = (1 << 1),
		RefreshCharacterWindow = (1 << 2),
		RefreshEquipmentWindow = (1 << 3),
		RefreshInventoryWindow = (1 << 4),
		RefreshExchangeWindow = (1 << 5),
		RefreshSkillWindow = (1 << 6),
		RefreshSafeboxWindow = (1 << 7),
		RefreshMessengerWindow = (1 << 8),
		RefreshGuildWindowInfoPage = (1 << 9),
		RefreshGuildWindowBoardPage = (1 << 10),
		RefreshGuildWindowMemberPage = (1 << 11),
		RefreshGuildWindowMemberPageGradeComboBox = (1 << 12),
		RefreshGuildWindowSkillPage = (1 << 13),
		RefreshGuildWindowGradePage = (1 << 14),
		RefreshTargetBoard = (1 << 15),
		RefreshMallWindow = (1 << 16),
	};

	void __RefreshStatus();
	void __RefreshAlignmentWindow();
	void __RefreshCharacterWindow();
	void __RefreshEquipmentWindow();
	void __RefreshInventoryWindow();
	void __RefreshExchangeWindow();
	void __RefreshSkillWindow();
	void __RefreshSafeboxWindow();
	void __RefreshMessengerWindow();
	void __RefreshGuildWindowInfoPage();
	void __RefreshGuildWindowBoardPage();
	void __RefreshGuildWindowMemberPage();
	void __RefreshGuildWindowMemberPageGradeComboBox();
	void __RefreshGuildWindowSkillPage();
	void __RefreshGuildWindowGradePage();
	void __RefreshTargetBoardByVID(DWORD dwVID);
	void __RefreshTargetBoardByName(const char* c_szName);
	void __RefreshTargetBoard();
	void __RefreshMallWindow();

	bool __SendHack(const char* c_szMsg);

protected:
	// Common
	bool RecvErrorPacket(int header);
	bool RecvPingPacket();
	bool RecvDefaultPacket(int header);
	bool RecvPhasePacket();

	// Login Phase
	bool __RecvLoginSuccessPacket3();
	bool __RecvLoginSuccessPacket4();
	bool __RecvLoginFailurePacket();
	bool __RecvEmpirePacket();
	bool __RecvLoginKeyPacket();

	// Select Phase
	bool __RecvPlayerCreateSuccessPacket();
	bool __RecvPlayerCreateFailurePacket();
	bool __RecvPlayerDestroySuccessPacket();
	bool __RecvPlayerDestroyFailurePacket();
	bool __RecvPreserveItemPacket();
	bool __RecvPlayerPoints();
	bool __RecvChangeName();

	// Loading Phase
	bool RecvMainCharacter();
	bool RecvMainCharacter2_EMPIRE();
	bool RecvMainCharacter3_BGM();
	bool RecvMainCharacter4_BGM_VOL();

	void __SetFieldMusicFileName(const char* musicName);
	void __SetFieldMusicFileInfo(const char* musicName, float vol);
	// END_OF_SUPPORT_BGM

	// Main Game Phase
	bool RecvWarpPacket();
	bool RecvPVPPacket();
	bool RecvDuelStartPacket();
	bool RecvGlobalTimePacket();
	bool RecvCharacterAppendPacket();
	bool RecvCharacterAdditionalInfo();
	bool RecvCharacterUpdatePacket();
	bool RecvCharacterDeletePacket();
	bool RecvChatPacket();
	bool RecvOwnerShipPacket();
	bool RecvSyncPositionPacket();
	bool RecvWhisperPacket();
	bool RecvPointChange(); // Alarm to python
	bool RecvChangeSpeedPacket();

	bool RecvStunPacket();
	bool RecvDeadPacket();
	bool RecvCharacterMovePacket();

	bool RecvItemSetEmptyPacket(); // Alarm to python
	bool RecvItemSetPacket(); // Alarm to python
	bool RecvItemUsePacket(); // Alarm to python
	bool RecvItemUpdatePacket(); // Alarm to python
	bool RecvItemGroundAddPacket();
	bool RecvItemGroundDelPacket();
	bool RecvItemOwnership();

	bool RecvQuickSlotAddPacket(); // Alarm to python
	bool RecvQuickSlotDelPacket(); // Alarm to python
	bool RecvQuickSlotMovePacket(); // Alarm to python

	bool RecvCharacterPositionPacket();
	bool RecvMotionPacket();

	bool RecvShopPacket();
	bool RecvShopSignPacket();
#if defined(ENABLE_PRIVATESHOP_SEARCH_SYSTEM)
	bool RecvPrivateShopSearch();
	bool RecvPrivateShopSearchOpen();
#endif

	bool RecvExchangePacket();

	// Quest
	bool RecvScriptPacket();
	bool RecvQuestInfoPacket();
	bool RecvQuestConfirmPacket();
	bool RecvRequestMakeGuild();

	// Skill
	bool RecvSkillLevel();
	bool RecvSkillLevelNew();

	// Target
	bool RecvTargetPacket();
	bool RecvViewEquipPacket();
	bool RecvDamageInfoPacket();

#if defined(ENABLE_SEND_TARGET_INFO)
	bool RecvTargetInfoPacket();

public:
	bool SendTargetInfoPacket(DWORD dwVID);

protected:
#endif

	// Mount
	bool RecvMountPacket();

	// Fly
	bool RecvCreateFlyPacket();
	bool RecvFlyTargetingPacket();
	bool RecvAddFlyTargetingPacket();

	// Messenger
	bool RecvMessenger();

	// Guild
	bool RecvGuild();

	// Party
	bool RecvPartyInvite();
	bool RecvPartyAdd();
	bool RecvPartyUpdate();
	bool RecvPartyRemove();
	bool RecvPartyLink();
	bool RecvPartyUnlink();
	bool RecvPartyParameter();

	// SafeBox
	bool RecvSafeBoxSetPacket();
	bool RecvSafeBoxDelPacket();
	bool RecvSafeBoxWrongPasswordPacket();
	bool RecvSafeBoxSizePacket();

	// Fishing
	bool RecvFishing();

	// Dungeon
	bool RecvDungeon();

	// Time
	bool RecvTimePacket();

	// WalkMode
	bool RecvWalkModePacket();

	// ChangeSkillGroup
	bool RecvChangeSkillGroupPacket();

	// Refine
	bool RecvRefineInformationPacket();
	bool RecvRefineInformationPacketNew();

	// Use Potion
	bool RecvSpecialEffect();

	// 서버에서 지정한 이팩트 발동 패킷.
	bool RecvSpecificEffect();

#if defined(ENABLE_DRAGON_SOUL_SYSTEM)
	// 용혼석 관련
	bool RecvDragonSoulRefine();
#endif

	// MiniMap Info
	bool RecvNPCList();
	bool RecvLandPacket();
	bool RecvTargetCreatePacket();
	bool RecvTargetCreatePacketNew();
	bool RecvTargetUpdatePacket();
	bool RecvTargetDeletePacket();

	// Affect
	bool RecvAffectAddPacket();
	bool RecvAffectRemovePacket();

	// Channel
	bool RecvChannelPacket();

protected:
	// 이모티콘
	bool ParseEmoticon(const BYTE bEmoticonType, DWORD* pdwEmoticon);

	// 파이썬으로 보내는 콜들
	void OnConnectFailure();
	void OnScriptEventStart(int iSkin, int iIndex);

	void OnRemoteDisconnect();
	void OnDisconnect();

	void SetGameOnline();
	void SetGameOffline();
	BOOL IsGameOnline();

#if defined(ENABLE_SKILLBOOK_COMB_SYSTEM)
public:
	bool SendSkillBookCombinationPacket(TItemPos* pPos, BYTE bAction);
#endif

protected:
	bool CheckPacket(TPacketHeader* pRetHeader);

	void __InitializeGamePhase();
	void __InitializeMarkAuth();
	void __GlobalPositionToLocalPosition(LONG& rGlobalX, LONG& rGlobalY);
	void __LocalPositionToGlobalPosition(LONG& rLocalX, LONG& rLocalY);

	bool __IsPlayerAttacking();
	bool __IsEquipItemInSlot(TItemPos Cell);

	void __ShowMapName(LONG lLocalX, LONG lLocalY);

	void __LeaveOfflinePhase() {}
	void __LeaveHandshakePhase() {}
	void __LeaveLoginPhase() {}
	void __LeaveSelectPhase() {}
	void __LeaveLoadingPhase() {}
	void __LeaveGamePhase();

	void __ClearNetworkActorManager();

	void __ClearSelectCharacterData();

	// DELETEME
	//void __SendWarpPacket();

	void __ConvertEmpireText(DWORD dwEmpireID, char* szText);

	void __RecvCharacterAppendPacket(SNetworkActorData* pkNetActorData);
	void __RecvCharacterUpdatePacket(SNetworkUpdateActorData* pkNetUpdateActorData);

	void __FilterInsult(char* szLine, UINT uLineLen);

	void __SetGuildID(DWORD id);

#if defined(ENABLE_CONQUEROR_LEVEL)
	void __SungMaAttrInfoProcess() const;
#endif

protected:
	TPacketGCHandshake m_HandshakeData;
	DWORD m_dwChangingPhaseTime;
	DWORD m_dwBindupRetryCount;
	DWORD m_dwMainActorVID;
	DWORD m_dwMainActorRace;
	DWORD m_dwMainActorEmpire;
	DWORD m_dwMainActorSkillGroup;
	BOOL m_isGameOnline;
	BOOL m_isStartGame;

	DWORD m_dwGuildID;
	DWORD m_dwEmpireID;
#if defined(ENABLE_NEW_USER_CARE)
	bool m_bEmpireSelect;
#endif

	struct SServerTimeSync
	{
		DWORD m_dwChangeServerTime;
		DWORD m_dwChangeClientTime;
	} m_kServerTimeSync;

	void __ServerTimeSync_Initialize();
	//DWORD m_dwBaseServerTime;
	//DWORD m_dwBaseClientTime;

	DWORD m_dwLastGamePingTime;

	std::string m_stID;
	std::string m_stPassword;
	std::string m_strLastCommand;
	std::string m_strPhase;
	DWORD m_dwLoginKey;
	BOOL m_isWaitLoginKey;

	std::string m_stMarkIP;

	CFuncObject<CPythonNetworkStream> m_phaseProcessFunc;
	CFuncObject<CPythonNetworkStream> m_phaseLeaveFunc;

	PyObject* m_poHandler;
	PyObject* m_apoPhaseWnd[PHASE_WINDOW_NUM];
	PyObject* m_poSerCommandParserWnd;

	TSimplePlayerInformation m_akSimplePlayerInfo[PLAYER_PER_ACCOUNT4];
	DWORD m_adwGuildID[PLAYER_PER_ACCOUNT4];
	std::string m_astrGuildName[PLAYER_PER_ACCOUNT4];
	bool m_bSimplePlayerInfo;

	CRef<CNetworkActorManager> m_rokNetActorMgr;

	bool m_isRefreshStatus;
	bool m_isRefreshCharacterWnd;
	bool m_isRefreshEquipmentWnd;
	bool m_isRefreshInventoryWnd;
	bool m_isRefreshExchangeWnd;
	bool m_isRefreshSkillWnd;
	bool m_isRefreshSafeboxWnd;
	bool m_isRefreshMallWnd;
	bool m_isRefreshMessengerWnd;
	bool m_isRefreshGuildWndInfoPage;
	bool m_isRefreshGuildWndBoardPage;
	bool m_isRefreshGuildWndMemberPage;
	bool m_isRefreshGuildWndMemberPageGradeComboBox;
	bool m_isRefreshGuildWndSkillPage;
	bool m_isRefreshGuildWndGradePage;

	// Emoticon
	std::vector<BYTE> m_EmoticonTypeVector;

	struct STextConvertTable
	{
		char acUpper[26];
		char acLower[26];
		BYTE aacHan[5000][2];
	} m_aTextConvTable[3];

	struct SMarkAuth
	{
		CNetworkAddress m_kNetAddr;
		DWORD m_dwHandle;
		DWORD m_dwRandomKey;
	} m_kMarkAuth;

	DWORD m_dwSelectedCharacterIndex;

	CInsultChecker m_kInsultChecker;

	bool m_isEnableChatInsultFilter;
	bool m_bComboSkillFlag;

	std::deque<std::string> m_kQue_stHack;

#if defined(ENABLE_LEFT_SEAT)
	// Left Seat
public:
	void SetLeftSeatLogOutState(bool bLeftSeatLogOutState) { m_bIsLeftSeatLogOutState = bLeftSeatLogOutState; }
	bool IsLeftSeatLogOutState() const { return m_bIsLeftSeatLogOutState; }

protected:
	bool m_bIsLeftSeatLogOutState;
#endif

private:
	struct SDirectEnterMode
	{
		bool m_isSet;
		DWORD m_dwChrSlotIndex;
	} m_kDirectEnterMode;

	void __DirectEnterMode_Initialize();
	void __DirectEnterMode_Set(UINT uChrSlotIndex);
	bool __DirectEnterMode_IsSet();

public:
	DWORD EXPORT_GetBettingGuildWarValue(const char* c_szValueName);

private:
	struct SBettingGuildWar
	{
		DWORD m_dwBettingMoney;
		DWORD m_dwObserverCount;
	} m_kBettingGuildWar;

	CInstanceBase* m_pInstTarget;

	void __BettingGuildWar_Initialize();
	void __BettingGuildWar_SetObserverCount(UINT uObserverCount);
	void __BettingGuildWar_SetBettingMoney(UINT uBettingMoney);

#if defined(_DEBUG) && defined(__IMGUI__)
public:
	PyObject* GetPhaseWindow(BYTE bPhaseWnd) const { return m_apoPhaseWnd[bPhaseWnd]; }
#endif
};
