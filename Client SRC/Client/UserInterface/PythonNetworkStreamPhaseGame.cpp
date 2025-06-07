#include "StdAfx.h"
#include "PythonNetworkStream.h"
#include "Packet.h"

#include "PythonGuild.h"
#include "PythonCharacterManager.h"
#include "PythonPlayer.h"
#include "PythonBackground.h"
#include "PythonMiniMap.h"
#include "PythonTextTail.h"
#include "PythonItem.h"
#include "PythonChat.h"
#include "PythonShop.h"
#include "PythonExchange.h"
#include "PythonQuest.h"
#include "PythonEventManager.h"
#include "PythonMessenger.h"
#include "PythonApplication.h"
#if defined(ENABLE_MAILBOX)
#	include "PythonMailBox.h"
#endif

#include "../EterPack/EterPackManager.h"
#include "../GameLib/ItemManager.h"

#include "AbstractApplication.h"
#include "AbstractCharacterManager.h"
#include "InstanceBase.h"

#include "ProcessCRC.h"
#include "../GameLib/ItemData.h"

#if defined(ENABLE_DISCORD_RPC)
#include "discord_rpc.h"
#endif

BOOL gs_bEmpireLanuageEnable = TRUE;

static int64_t StartTime;
static constexpr auto DiscordClientID = "732229869118685217"; // Change

#if defined(ENABLE_DISCORD_RPC)
void CPythonNetworkStream::Discord_Start()
{
	StartTime = time(0);
	DiscordEventHandlers handlers;
	memset(&handlers, 0, sizeof(handlers));
	Discord_Initialize(DiscordClientID, &handlers, 1, nullptr);
	Discord_Update(false /* bGameState */);
}

void CPythonNetworkStream::Discord_Update(const bool bGameState)
{
	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));
	if (bGameState)
	{
		discordPresence.details = CPythonPlayer::Instance().GetName();
		discordPresence.state = CPythonBackground::Instance().GetWarpMapName();

		discordPresence.startTimestamp = StartTime;

		discordPresence.largeImageKey = "logo";
		discordPresence.largeImageText = "Farques Development";

		switch (CPythonPlayer::Instance().GetRace())
		{
			case MAIN_RACE_WARRIOR_M:
				discordPresence.smallImageKey = "warrior_m";
				discordPresence.smallImageText = "Savasci Erkek";
				break;

			case MAIN_RACE_WARRIOR_W:
				discordPresence.smallImageKey = "warrior_w";
				discordPresence.smallImageText = "Savasci Kadin";
				break;

			case MAIN_RACE_ASSASSIN_M:
				discordPresence.smallImageKey = "assassin_m";
				discordPresence.smallImageText = "Ninja Erkek";
				break;

			case MAIN_RACE_ASSASSIN_W:
				discordPresence.smallImageKey = "assassin_w";
				discordPresence.smallImageText = "Ninja Kadin";
				break;

			case MAIN_RACE_SURA_M:
				discordPresence.smallImageKey = "sura_m";
				discordPresence.smallImageText = "Sura Erkek";
				break;

			case MAIN_RACE_SURA_W:
				discordPresence.smallImageKey = "sura_w";
				discordPresence.smallImageText = "Sura Kadin";
				break;

			case MAIN_RACE_SHAMAN_M:
				discordPresence.smallImageKey = "shaman_m";
				discordPresence.smallImageText = "Saman Erkek";
				break;

			case MAIN_RACE_SHAMAN_W:
				discordPresence.smallImageKey = "shaman_w";
				discordPresence.smallImageText = "Saman Kadin";
				break;

			case MAIN_RACE_WOLFMAN_M:
				discordPresence.smallImageKey = "wolfman_w";
				discordPresence.smallImageText = "Wolfman";
				break;

		}
	}
	discordPresence.buttonLabel = "Farques Discord";
	discordPresence.buttonURL = "https://discord.com/invite/v36Ru9zpJ3";
	Discord_UpdatePresence(&discordPresence);
}

void CPythonNetworkStream::Discord_Close()
{
	Discord_Shutdown();
}
#endif

void CPythonNetworkStream::__RefreshAlignmentWindow()
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshAlignment", Py_BuildValue("()"));
}

void CPythonNetworkStream::__RefreshTargetBoardByVID(DWORD dwVID)
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshTargetBoardByVID", Py_BuildValue("(i)", dwVID));
}

void CPythonNetworkStream::__RefreshTargetBoardByName(const char* c_szName)
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshTargetBoardByName", Py_BuildValue("(s)", c_szName));
}

void CPythonNetworkStream::__RefreshTargetBoard()
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshTargetBoard", Py_BuildValue("()"));
}

void CPythonNetworkStream::__RefreshGuildWindowGradePage()
{
	m_isRefreshGuildWndGradePage = true;
}

void CPythonNetworkStream::__RefreshGuildWindowSkillPage()
{
	m_isRefreshGuildWndSkillPage = true;
}

void CPythonNetworkStream::__RefreshGuildWindowMemberPageGradeComboBox()
{
	m_isRefreshGuildWndMemberPageGradeComboBox = true;
}

void CPythonNetworkStream::__RefreshGuildWindowMemberPage()
{
	m_isRefreshGuildWndMemberPage = true;
}

void CPythonNetworkStream::__RefreshGuildWindowBoardPage()
{
	m_isRefreshGuildWndBoardPage = true;
}

void CPythonNetworkStream::__RefreshGuildWindowInfoPage()
{
	m_isRefreshGuildWndInfoPage = true;
}

void CPythonNetworkStream::__RefreshMessengerWindow()
{
	m_isRefreshMessengerWnd = true;
}

void CPythonNetworkStream::__RefreshSafeboxWindow()
{
	m_isRefreshSafeboxWnd = true;
}

void CPythonNetworkStream::__RefreshMallWindow()
{
	m_isRefreshMallWnd = true;
}

void CPythonNetworkStream::__RefreshSkillWindow()
{
	m_isRefreshSkillWnd = true;
}

void CPythonNetworkStream::__RefreshExchangeWindow()
{
	m_isRefreshExchangeWnd = true;
}

void CPythonNetworkStream::__RefreshStatus()
{
	m_isRefreshStatus = true;
}

void CPythonNetworkStream::__RefreshCharacterWindow()
{
	m_isRefreshCharacterWnd = true;
}

void CPythonNetworkStream::__RefreshInventoryWindow()
{
	m_isRefreshInventoryWnd = true;
}

void CPythonNetworkStream::__RefreshEquipmentWindow()
{
	m_isRefreshEquipmentWnd = true;
}

void CPythonNetworkStream::__SetGuildID(DWORD id)
{
	if (m_dwGuildID != id)
	{
		m_dwGuildID = id;
		IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();

		for (int i = 0; i < PLAYER_PER_ACCOUNT4; ++i)
		{
			if (!strncmp(m_akSimplePlayerInfo[i].szName, rkPlayer.GetName(), CHARACTER_NAME_MAX_LEN))
			{
				m_adwGuildID[i] = id;

				std::string guildName;
				if (CPythonGuild::Instance().GetGuildName(id, &guildName))
				{
					m_astrGuildName[i] = guildName;
				}
				else
				{
					m_astrGuildName[i] = "";
				}
			}
		}
	}
}

struct PERF_PacketInfo
{
	DWORD dwCount;
	DWORD dwTime;

	PERF_PacketInfo()
	{
		dwCount = 0;
		dwTime = 0;
	}
};

#ifdef __PERFORMANCE_CHECK__

class PERF_PacketTimeAnalyzer
{
public:
	~PERF_PacketTimeAnalyzer()
	{
		FILE* fp = fopen("perf_dispatch_packet_result.txt", "w");

		for (std::map<DWORD, PERF_PacketInfo>::iterator i = m_kMap_kPacketInfo.begin(); i != m_kMap_kPacketInfo.end(); ++i)
		{
			if (i->second.dwTime > 0)
				fprintf(fp, "header %d: count %d, time %d, tpc %d\n", i->first, i->second.dwCount, i->second.dwTime, i->second.dwTime / i->second.dwCount);
		}
		fclose(fp);
	}

public:
	std::map<DWORD, PERF_PacketInfo> m_kMap_kPacketInfo;
};

PERF_PacketTimeAnalyzer gs_kPacketTimeAnalyzer;

#endif

// Game Phase ---------------------------------------------------------------------------
void CPythonNetworkStream::GamePhase()
{
	if (!m_kQue_stHack.empty())
	{
		__SendHack(m_kQue_stHack.front().c_str());
		m_kQue_stHack.pop_front();
	}

	TPacketHeader header = 0;
	bool ret = true;

#ifdef __PERFORMANCE_CHECK__
	DWORD timeBeginDispatch = timeGetTime();

	static std::map<DWORD, PERF_PacketInfo> kMap_kPacketInfo;
	kMap_kPacketInfo.clear();
#endif

#ifdef __DOUBLE_RECV_BUFFER__
	const DWORD MAX_RECV_COUNT = 4 * 2;
	const DWORD SAFE_RECV_BUFSIZE = 8192 * 2;
#else
	const DWORD MAX_RECV_COUNT = 4;
	const DWORD SAFE_RECV_BUFSIZE = 8192;
#endif
	DWORD dwRecvCount = 0;

	while (ret)
	{
		if (dwRecvCount++ >= MAX_RECV_COUNT - 1 && GetRecvBufferSize() < SAFE_RECV_BUFSIZE
			&& m_strPhase == "Game") // phase_game 이 아니어도 여기로 들어오는 경우가 있다.
			break;

		if (!CheckPacket(&header))
			break;

#ifdef __PERFORMANCE_CHECK__
		DWORD timeBeginPacket = timeGetTime();
#endif

		switch (header)
		{
			case HEADER_GC_WARP:
				ret = RecvWarpPacket();
				break;

			case HEADER_GC_PHASE:
				ret = RecvPhasePacket();
				return; // 도중에 Phase 가 바뀌면 일단 무조건 GamePhase 탈출 - [levites]

			case HEADER_GC_PVP:
				ret = RecvPVPPacket();
				break;

			case HEADER_GC_DUEL_START:
				ret = RecvDuelStartPacket();
				break;

			case HEADER_GC_CHARACTER_ADD:
				ret = RecvCharacterAppendPacket();
				break;

			case HEADER_GC_CHAR_ADDITIONAL_INFO:
				ret = RecvCharacterAdditionalInfo();
				break;

			case HEADER_GC_CHARACTER_UPDATE:
				ret = RecvCharacterUpdatePacket();
				break;

			case HEADER_GC_CHARACTER_DEL:
				ret = RecvCharacterDeletePacket();
				break;

			case HEADER_GC_CHAT:
				ret = RecvChatPacket();
				break;

			case HEADER_GC_SYNC_POSITION:
				ret = RecvSyncPositionPacket();
				break;

			case HEADER_GC_OWNERSHIP:
				ret = RecvOwnerShipPacket();
				break;

			case HEADER_GC_WHISPER:
				ret = RecvWhisperPacket();
				break;

			case HEADER_GC_CHARACTER_MOVE:
				ret = RecvCharacterMovePacket();
				break;

				// Position
			case HEADER_GC_CHARACTER_POSITION:
				ret = RecvCharacterPositionPacket();
				break;

				// Battle Packet
			case HEADER_GC_STUN:
				ret = RecvStunPacket();
				break;

			case HEADER_GC_DEAD:
				ret = RecvDeadPacket();
				break;

			case HEADER_GC_PLAYER_POINT_CHANGE:
				ret = RecvPointChange();
				break;

				// Item packet
			case HEADER_GC_ITEM_SET_EMPTY:
				ret = RecvItemSetEmptyPacket();
				break;

			case HEADER_GC_ITEM_SET:
				ret = RecvItemSetPacket();
				break;

			case HEADER_GC_ITEM_USE:
				ret = RecvItemUsePacket();
				break;

			case HEADER_GC_ITEM_UPDATE:
				ret = RecvItemUpdatePacket();
				break;

			case HEADER_GC_ITEM_GROUND_ADD:
				ret = RecvItemGroundAddPacket();
				break;

			case HEADER_GC_ITEM_GROUND_DEL:
				ret = RecvItemGroundDelPacket();
				break;

			case HEADER_GC_ITEM_OWNERSHIP:
				ret = RecvItemOwnership();
				break;

			case HEADER_GC_QUICKSLOT_ADD:
				ret = RecvQuickSlotAddPacket();
				break;

			case HEADER_GC_QUICKSLOT_DEL:
				ret = RecvQuickSlotDelPacket();
				break;

			case HEADER_GC_QUICKSLOT_SWAP:
				ret = RecvQuickSlotMovePacket();
				break;

			case HEADER_GC_MOTION:
				ret = RecvMotionPacket();
				break;

			case HEADER_GC_EMOTE:
				ret = RecvEmote();
				break;

			case HEADER_GC_SHOP:
				ret = RecvShopPacket();
				break;

			case HEADER_GC_SHOP_SIGN:
				ret = RecvShopSignPacket();
				break;

			case HEADER_GC_EXCHANGE:
				ret = RecvExchangePacket();
				break;

			case HEADER_GC_QUEST_INFO:
				ret = RecvQuestInfoPacket();
				break;

			case HEADER_GC_REQUEST_MAKE_GUILD:
				ret = RecvRequestMakeGuild();
				break;

			case HEADER_GC_PING:
				ret = RecvPingPacket();
				break;

			case HEADER_GC_SCRIPT:
				ret = RecvScriptPacket();
				break;

			case HEADER_GC_QUEST_CONFIRM:
				ret = RecvQuestConfirmPacket();
				break;

			case HEADER_GC_TARGET:
				ret = RecvTargetPacket();
				break;

			case HEADER_GC_DAMAGE_INFO:
				ret = RecvDamageInfoPacket();
				break;

			case HEADER_GC_MOUNT:
				ret = RecvMountPacket();
				break;

			case HEADER_GC_CHANGE_SPEED:
				ret = RecvChangeSpeedPacket();
				break;

			case HEADER_GC_PLAYER_POINTS:
				ret = __RecvPlayerPoints();
				break;

			case HEADER_GC_CREATE_FLY:
				ret = RecvCreateFlyPacket();
				break;

			case HEADER_GC_FLY_TARGETING:
				ret = RecvFlyTargetingPacket();
				break;

			case HEADER_GC_ADD_FLY_TARGETING:
				ret = RecvAddFlyTargetingPacket();
				break;

			case HEADER_GC_SKILL_LEVEL:
				ret = RecvSkillLevel();
				break;

			case HEADER_GC_SKILL_LEVEL_NEW:
				ret = RecvSkillLevelNew();
				break;

			case HEADER_GC_MESSENGER:
				ret = RecvMessenger();
				break;

			case HEADER_GC_GUILD:
				ret = RecvGuild();
				break;

			case HEADER_GC_PARTY_INVITE:
				ret = RecvPartyInvite();
				break;

			case HEADER_GC_PARTY_ADD:
				ret = RecvPartyAdd();
				break;

			case HEADER_GC_PARTY_UPDATE:
				ret = RecvPartyUpdate();
				break;

			case HEADER_GC_PARTY_REMOVE:
				ret = RecvPartyRemove();
				break;

			case HEADER_GC_PARTY_LINK:
				ret = RecvPartyLink();
				break;

			case HEADER_GC_PARTY_UNLINK:
				ret = RecvPartyUnlink();
				break;

			case HEADER_GC_PARTY_PARAMETER:
				ret = RecvPartyParameter();
				break;

			case HEADER_GC_SAFEBOX_SET:
				ret = RecvSafeBoxSetPacket();
				break;

			case HEADER_GC_SAFEBOX_DEL:
				ret = RecvSafeBoxDelPacket();
				break;

			case HEADER_GC_SAFEBOX_WRONG_PASSWORD:
				ret = RecvSafeBoxWrongPasswordPacket();
				break;

			case HEADER_GC_SAFEBOX_SIZE:
				ret = RecvSafeBoxSizePacket();
				break;

			case HEADER_GC_FISHING:
				ret = RecvFishing();
				break;

			case HEADER_GC_DUNGEON:
				ret = RecvDungeon();
				break;

			case HEADER_GC_TIME:
				ret = RecvTimePacket();
				break;

			case HEADER_GC_WALK_MODE:
				ret = RecvWalkModePacket();
				break;

			case HEADER_GC_CHANGE_SKILL_GROUP:
				ret = RecvChangeSkillGroupPacket();
				break;

			case HEADER_GC_REFINE_INFORMATION:
				ret = RecvRefineInformationPacket();
				break;

			case HEADER_GC_REFINE_INFORMATION_NEW:
				ret = RecvRefineInformationPacketNew();
				break;

			case HEADER_GC_SEPCIAL_EFFECT:
				ret = RecvSpecialEffect();
				break;

			case HEADER_GC_NPC_POSITION:
				ret = RecvNPCList();
				break;

			case HEADER_GC_CHANNEL:
				ret = RecvChannelPacket();
				break;

			case HEADER_GC_VIEW_EQUIP:
				ret = RecvViewEquipPacket();
				break;

			case HEADER_GC_LAND_LIST:
				ret = RecvLandPacket();
				break;

				//case HEADER_GC_TARGET_CREATE:
				//	ret = RecvTargetCreatePacket();
				//	break;

			case HEADER_GC_TARGET_CREATE_NEW:
				ret = RecvTargetCreatePacketNew();
				break;

			case HEADER_GC_TARGET_UPDATE:
				ret = RecvTargetUpdatePacket();
				break;

			case HEADER_GC_TARGET_DELETE:
				ret = RecvTargetDeletePacket();
				break;

			case HEADER_GC_AFFECT_ADD:
				ret = RecvAffectAddPacket();
				break;

			case HEADER_GC_AFFECT_REMOVE:
				ret = RecvAffectRemovePacket();
				break;

			case HEADER_GC_MALL_OPEN:
				ret = RecvMallOpenPacket();
				break;

			case HEADER_GC_MALL_SET:
				ret = RecvMallItemSetPacket();
				break;

			case HEADER_GC_MALL_DEL:
				ret = RecvMallItemDelPacket();
				break;

			case HEADER_GC_LOVER_INFO:
				ret = RecvLoverInfoPacket();
				break;

			case HEADER_GC_LOVE_POINT_UPDATE:
				ret = RecvLovePointUpdatePacket();
				break;

			case HEADER_GC_DIG_MOTION:
				ret = RecvDigMotionPacket();
				break;

			case HEADER_GC_HANDSHAKE:
				RecvHandshakePacket();
				return;

			case HEADER_GC_HANDSHAKE_OK:
				RecvHandshakeOKPacket();
				return;

			case HEADER_GC_HYBRIDCRYPT_KEYS:
				RecvHybridCryptKeyPacket();
				return;

			case HEADER_GC_HYBRIDCRYPT_SDB:
				RecvHybridCryptSDBPacket();
				return;

#if defined(__IMPROVED_PACKET_ENCRYPTION__)
			case HEADER_GC_KEY_AGREEMENT:
				RecvKeyAgreementPacket();
				return;

			case HEADER_GC_KEY_AGREEMENT_COMPLETED:
				RecvKeyAgreementCompletedPacket();
				return;
#endif

			case HEADER_GC_SPECIFIC_EFFECT:
				ret = RecvSpecificEffect();
				break;

#if defined(ENABLE_DRAGON_SOUL_SYSTEM)
			case HEADER_GC_DRAGON_SOUL_REFINE:
				ret = RecvDragonSoulRefine();
				break;
#endif

#if defined(ENABLE_SEND_TARGET_INFO)
			case HEADER_GC_TARGET_INFO:
				ret = RecvTargetInfoPacket();
				break;
#endif

#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
			case HEADER_GC_ACCE_REFINE:
				ret = RecvAcceRefinePacket();
				break;
#endif

#if defined(ENABLE_AURA_COSTUME_SYSTEM)
			case HEADER_GC_AURA_REFINE:
				ret = RecvAuraRefinePacket();
				break;
#endif

#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
			case HEADER_GC_CHANGE_LOOK_SET:
				ret = RecvChangeLookItemSetPacket();
				break;

			case HEADER_GC_CHANGE_LOOK_DEL:
				ret = RecvChangeLookItemDelPacket();
				break;

			case HEADER_GC_CHANGE_LOOK_FREE_SET:
				ret = RecvChangeLookFreeItemSetPacket();
				break;

			case HEADER_GC_CHANGE_LOOK_FREE_DEL:
				ret = RecvChangeLookFreeItemDelPacket();
				break;
#endif

#if defined(ENABLE_MINI_GAME_CATCH_KING)
			case HEADER_GC_MINI_GAME_CATCH_KING:
				ret = RecvMiniGameCatchKingPacket();
				break;
#endif

#if defined(ENABLE_PRIVATESHOP_SEARCH_SYSTEM)
			case HEADER_GC_PRIVATESHOP_SEARCH:
				ret = RecvPrivateShopSearch();
				break;
			case HEADER_GC_PRIVATESHOP_SEARCH_OPEN:
				ret = RecvPrivateShopSearchOpen();
				break;
#endif

#if defined(ENABLE_MAILBOX)
			case HEADER_GC_MAILBOX_PROCESS:
				ret = RecvMailboxProcess();
				break;

			case HEADER_GC_MAILBOX:
				ret = RecvMailbox();
				break;

			case HEADER_GC_MAILBOX_ADD_DATA:
				ret = RecvMailboxAddData();
				break;

			case HEADER_GC_MAILBOX_ALL:
				ret = RecvMailboxAll();
				break;

			case HEADER_GC_MAILBOX_UNREAD:
				ret = RecvMailboxUnread();
				break;
#endif

#if defined(ENABLE_RANKING_SYSTEM)
			case HEADER_GC_RANKING:
				ret = RecvRanking();
				break;
#endif

#if defined(ENABLE_CHANGED_ATTR)
			case HEADER_GC_ITEM_SELECT_ATTR:
				ret = RecvSelectAttr();
				break;
#endif

#if defined(ENABLE_LOOTING_SYSTEM)
			case HEADER_GC_LOOT_FILTER:
				ret = RecvLootFilterPacket();
				break;
#endif

#if defined(ENABLE_MINI_GAME_RUMI)
			case HEADER_GC_MINI_GAME_RUMI:
				ret = RecvMiniGameRumi();
				break;
#endif

#if defined(ENABLE_MINI_GAME_YUTNORI)
			case HEADER_GC_MINI_GAME_YUTNORI:
				ret = RecvMiniGameYutnori();
				break;
#endif

#if defined(ENABLE_LUCKY_BOX)
			case HEADER_GC_LUCKY_BOX:
				ret = RecvLuckyBoxPacket();
				break;
#endif

#if defined(ENABLE_FISHING_GAME)
			case HEADER_GC_FISHING_GAME:
				ret = RecvFishingGamePacket();
				break;
#endif

#if defined(ENABLE_GEM_SYSTEM)
			case HEADER_GC_GEM_SHOP:
				ret = RecvGemShop();
				break;

			case HEADER_GC_GEM_SHOP_PROCESS:
				ret = RecvGemShopProcess();
				break;
#endif

#if defined(ENABLE_EXTEND_INVEN_SYSTEM)
			case HEADER_GC_EXTEND_INVEN:
				ret = RecvExtendInven();
				break;

			case HEADER_GC_EXTEND_INVEN_ITEM_USE:
				ret = RecvExtendInvenItemUse();
				break;
#endif

#if defined(ENABLE_CLIENT_TIMER)
			case HEADER_GC_CLIENT_TIMER:
				ret = RecvClientTimer();
				break;
#endif

#if defined(ENABLE_CUBE_RENEWAL)
			case HEADER_GC_CUBE:
				ret = RecvCubePacket();
				break;
#endif

#if defined(ENABLE_SNOWFLAKE_STICK_EVENT)
			case HEADER_GC_SNOWFLAKE_STICK_EVENT:
				ret = RecvSnowflakeStickEventPacket();
				break;
#endif

#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
			case HEADER_GC_REFINE_ELEMENT:
				ret = RecvRefineElementPacket();
				break;
#endif

#if defined(ENABLE_GUILD_DRAGONLAIR_SYSTEM)
			case HEADER_GC_GUILD_DRAGONLAIR:
				ret = RecvGuildDragonLairPacket();
				break;
#endif

#if defined(ENABLE_SUMMER_EVENT_ROULETTE)
			case HEADER_GC_MINI_GAME_ROULETTE:
				ret = RecvMiniGameRoulettePacket();
				break;
#endif

#if defined(ENABLE_FLOWER_EVENT)
			case HEADER_GC_FLOWER_EVENT:
				ret = RecvFlowerEventPacket();
				break;
#endif

			default:
				ret = RecvDefaultPacket(header);
				break;
		}

#ifdef __PERFORMANCE_CHECK__
		DWORD timeEndPacket = timeGetTime();

		{
			PERF_PacketInfo& rkPacketInfo = kMap_kPacketInfo[header];
			rkPacketInfo.dwCount++;
			rkPacketInfo.dwTime += timeEndPacket - timeBeginPacket;
		}

		{
			PERF_PacketInfo& rkPacketInfo = gs_kPacketTimeAnalyzer.m_kMap_kPacketInfo[header];
			rkPacketInfo.dwCount++;
			rkPacketInfo.dwTime += timeEndPacket - timeBeginPacket;
		}
#endif
	}

#ifdef __PERFORMANCE_CHECK__
	DWORD timeEndDispatch = timeGetTime();

	if (timeEndDispatch - timeBeginDispatch > 2)
	{
		static FILE* fp = fopen("perf_dispatch_packet.txt", "w");

		fprintf(fp, "delay %d\n", timeEndDispatch - timeBeginDispatch);
		for (std::map<DWORD, PERF_PacketInfo>::iterator i = kMap_kPacketInfo.begin(); i != kMap_kPacketInfo.end(); ++i)
		{
			if (i->second.dwTime > 0)
				fprintf(fp, "header %d: count %d, time %d\n", i->first, i->second.dwCount, i->second.dwTime);
		}
		fputs("=====================================================\n", fp);
		fflush(fp);
	}
#endif

	if (!ret)
		RecvErrorPacket(header);

	static DWORD s_nextRefreshTime = ELTimer_GetMSec();

	DWORD curTime = ELTimer_GetMSec();
	if (s_nextRefreshTime > curTime)
		return;

	if (m_isRefreshCharacterWnd)
	{
		m_isRefreshCharacterWnd = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshCharacter", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshEquipmentWnd)
	{
		m_isRefreshEquipmentWnd = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshEquipment", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshInventoryWnd)
	{
		m_isRefreshInventoryWnd = false;

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshInventory", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshExchangeWnd)
	{
		m_isRefreshExchangeWnd = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshExchange", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshSkillWnd)
	{
		m_isRefreshSkillWnd = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshSkill", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshSafeboxWnd)
	{
		m_isRefreshSafeboxWnd = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshSafebox", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshMallWnd)
	{
		m_isRefreshMallWnd = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshMall", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshStatus)
	{
		m_isRefreshStatus = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshStatus", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshMessengerWnd)
	{
		m_isRefreshMessengerWnd = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshMessenger", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndInfoPage)
	{
		m_isRefreshGuildWndInfoPage = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildInfoPage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndBoardPage)
	{
		m_isRefreshGuildWndBoardPage = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildBoardPage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndMemberPage)
	{
		m_isRefreshGuildWndMemberPage = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildMemberPage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndMemberPageGradeComboBox)
	{
		m_isRefreshGuildWndMemberPageGradeComboBox = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildMemberPageGradeComboBox", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndSkillPage)
	{
		m_isRefreshGuildWndSkillPage = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildSkillPage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndGradePage)
	{
		m_isRefreshGuildWndGradePage = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildGradePage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}
}

void CPythonNetworkStream::__InitializeGamePhase()
{
	__ServerTimeSync_Initialize();

	m_isRefreshStatus = false;
	m_isRefreshCharacterWnd = false;
	m_isRefreshEquipmentWnd = false;
	m_isRefreshInventoryWnd = false;
	m_isRefreshExchangeWnd = false;
	m_isRefreshSkillWnd = false;
	m_isRefreshSafeboxWnd = false;
	m_isRefreshMallWnd = false;
	m_isRefreshMessengerWnd = false;
	m_isRefreshGuildWndInfoPage = false;
	m_isRefreshGuildWndBoardPage = false;
	m_isRefreshGuildWndMemberPage = false;
	m_isRefreshGuildWndMemberPageGradeComboBox = false;
	m_isRefreshGuildWndSkillPage = false;
	m_isRefreshGuildWndGradePage = false;

	m_EmoticonTypeVector.clear();

	m_pInstTarget = NULL;
}

void CPythonNetworkStream::Warp(LONG lGlobalX, LONG lGlobalY)
{
	CPythonBackground& rkBgMgr = CPythonBackground::Instance();
	rkBgMgr.SetShadowLevel(CPythonBackground::SHADOW_NONE);
	rkBgMgr.Destroy();

	rkBgMgr.Create();
	rkBgMgr.Warp(lGlobalX, lGlobalY);
	rkBgMgr.RefreshShadowLevel();

	// NOTE : Warp 했을때 CenterPosition의 Height가 0이기 때문에 카메라가 땅바닥에 박혀있게 됨
	// 움직일때마다 Height가 갱신 되기 때문이므로 맵을 이동하면 Position을 강제로 한번
	// 셋팅해준다 - [levites]
	LONG lLocalX = lGlobalX;
	LONG lLocalY = lGlobalY;
	__GlobalPositionToLocalPosition(lLocalX, lLocalY);
	float fHeight = CPythonBackground::Instance().GetHeight(float(lLocalX), float(lLocalY));

	IAbstractApplication& rkApp = IAbstractApplication::GetSingleton();
	rkApp.SetCenterPosition(float(lLocalX), float(lLocalY), fHeight);

	__ShowMapName(lLocalX, lLocalY);
}

void CPythonNetworkStream::__ShowMapName(LONG lLocalX, LONG lLocalY)
{
	const std::string& c_rstrMapFileName = CPythonBackground::Instance().GetWarpMapName();
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ShowMapName", Py_BuildValue("(sii)", c_rstrMapFileName.c_str(), lLocalX, lLocalY));
}

void CPythonNetworkStream::__LeaveGamePhase()
{
	CInstanceBase::ClearPVPKeySystem();

	__ClearNetworkActorManager();

	m_bComboSkillFlag = FALSE;

	IAbstractCharacterManager& rkChrMgr = IAbstractCharacterManager::GetSingleton();
	rkChrMgr.Destroy();

	CPythonItem& rkItemMgr = CPythonItem::Instance();
	rkItemMgr.Destroy();
}

void CPythonNetworkStream::SetGamePhase()
{
	if ("Game" != m_strPhase)
		m_phaseLeaveFunc.Run();

	Tracen("");
	Tracen("## Network - Game Phase ##");
	Tracen("");

	m_strPhase = "Game";

	m_dwChangingPhaseTime = ELTimer_GetMSec();
	m_phaseProcessFunc.Set(this, &CPythonNetworkStream::GamePhase);
	m_phaseLeaveFunc.Set(this, &CPythonNetworkStream::__LeaveGamePhase);

	// Main Character 등록O

	IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
	rkPlayer.SetMainCharacterIndex(GetMainActorVID());

	__RefreshStatus();
}

#if defined(ENABLE_SEND_TARGET_INFO)
bool CPythonNetworkStream::SendTargetInfoPacket(DWORD dwVID)
{
	TPacketCGTargetInfo TargetInfoPacket;
	TargetInfoPacket.bHeader = HEADER_CG_TARGET_INFO;
	TargetInfoPacket.dwVID = dwVID;

	if (!Send(sizeof(TargetInfoPacket), &TargetInfoPacket))
		return false;

	return SendSequence();
}
#endif

bool CPythonNetworkStream::RecvWarpPacket()
{
	TPacketGCWarp kWarpPacket;

	if (!Recv(sizeof(kWarpPacket), &kWarpPacket))
		return false;

	__DirectEnterMode_Set(m_dwSelectedCharacterIndex);
	CNetworkStream::Connect((DWORD)kWarpPacket.lAddr, kWarpPacket.wPort);

	return true;
}

bool CPythonNetworkStream::RecvDuelStartPacket()
{
	TPacketGCDuelStart kDuelStartPacket;
	if (!Recv(sizeof(kDuelStartPacket), &kDuelStartPacket))
		return false;

	DWORD count = (kDuelStartPacket.wSize - sizeof(kDuelStartPacket)) / sizeof(DWORD);

	CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();

	CInstanceBase* pkInstMain = rkChrMgr.GetMainInstancePtr();
	if (!pkInstMain)
	{
		TraceError("CPythonNetworkStream::RecvDuelStartPacket - MainCharacter is NULL");
		return false;
	}
	DWORD dwVIDSrc = pkInstMain->GetVirtualID();
	DWORD dwVIDDest;

	for (DWORD i = 0; i < count; i++)
	{
		Recv(sizeof(dwVIDDest), &dwVIDDest);
		CInstanceBase::InsertDUELKey(dwVIDSrc, dwVIDDest);
	}

	if (count == 0)
		pkInstMain->SetDuelMode(CInstanceBase::DUEL_CANNOTATTACK);
	else
		pkInstMain->SetDuelMode(CInstanceBase::DUEL_START);

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "CloseTargetBoard", Py_BuildValue("()"));

	rkChrMgr.RefreshAllPCTextTail();

	return true;
}

bool CPythonNetworkStream::RecvPVPPacket()
{
	TPacketGCPVP kPVPPacket;
	if (!Recv(sizeof(kPVPPacket), &kPVPPacket))
		return false;

	CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();

	switch (kPVPPacket.bMode)
	{
		case PVP_MODE_AGREE:
			rkChrMgr.RemovePVPKey(kPVPPacket.dwVIDSrc, kPVPPacket.dwVIDDst);

			// 상대가 나(Dst)에게 동의를 구했을때
			if (rkPlayer.IsMainCharacterIndex(kPVPPacket.dwVIDDst))
				rkPlayer.RememberChallengeInstance(kPVPPacket.dwVIDSrc);

			// 상대에게 동의를 구한 동안에는 대결 불능
			if (rkPlayer.IsMainCharacterIndex(kPVPPacket.dwVIDSrc))
				rkPlayer.RememberCantFightInstance(kPVPPacket.dwVIDDst);
			break;
		case PVP_MODE_REVENGE:
		{
			rkChrMgr.RemovePVPKey(kPVPPacket.dwVIDSrc, kPVPPacket.dwVIDDst);

			DWORD dwKiller = kPVPPacket.dwVIDSrc;
			DWORD dwVictim = kPVPPacket.dwVIDDst;

			// 내(victim)가 상대에게 복수할 수 있을때
			if (rkPlayer.IsMainCharacterIndex(dwVictim))
				rkPlayer.RememberRevengeInstance(dwKiller);

			// 상대(victim)가 나에게 복수하는 동안에는 대결 불능
			if (rkPlayer.IsMainCharacterIndex(dwKiller))
				rkPlayer.RememberCantFightInstance(dwVictim);
			break;
		}

		case PVP_MODE_FIGHT:
			rkChrMgr.InsertPVPKey(kPVPPacket.dwVIDSrc, kPVPPacket.dwVIDDst);
			rkPlayer.ForgetInstance(kPVPPacket.dwVIDSrc);
			rkPlayer.ForgetInstance(kPVPPacket.dwVIDDst);
			break;
		case PVP_MODE_NONE:
			rkChrMgr.RemovePVPKey(kPVPPacket.dwVIDSrc, kPVPPacket.dwVIDDst);
			rkPlayer.ForgetInstance(kPVPPacket.dwVIDSrc);
			rkPlayer.ForgetInstance(kPVPPacket.dwVIDDst);
			break;
	}

	// NOTE : PVP 토글시 TargetBoard 를 업데이트 합니다.
	__RefreshTargetBoardByVID(kPVPPacket.dwVIDSrc);
	__RefreshTargetBoardByVID(kPVPPacket.dwVIDDst);

	return true;
}

// DELETEME
/*
void CPythonNetworkStream::__SendWarpPacket()
{
	TPacketCGWarp kWarpPacket;
	kWarpPacket.bHeader=HEADER_GC_WARP;
	if (!Send(sizeof(kWarpPacket), &kWarpPacket))
	{
		return;
	}
}
*/
void CPythonNetworkStream::NotifyHack(const char* c_szMsg)
{
	if (!m_kQue_stHack.empty())
		if (c_szMsg == m_kQue_stHack.back())
			return;

	m_kQue_stHack.push_back(c_szMsg);
}

bool CPythonNetworkStream::__SendHack(const char* c_szMsg)
{
	Tracen(c_szMsg);

	TPacketCGHack kPacketHack;
	kPacketHack.bHeader = HEADER_CG_HACK;
	strncpy(kPacketHack.szBuf, c_szMsg, sizeof(kPacketHack.szBuf) - 1);

	if (!Send(sizeof(kPacketHack), &kPacketHack))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendCharacterStatePacket(const TPixelPosition& c_rkPPosDst, float fDstRot, UINT eFunc, UINT uArg)
{
	NANOBEGIN
		if (!__CanActMainInstance())
			return true;

	if (fDstRot < 0.0f)
		fDstRot = 360 + fDstRot;
	else if (fDstRot > 360.0f)
		fDstRot = fmodf(fDstRot, 360.0f);

	// TODO: 나중에 패킷이름을 바꾸자
	TPacketCGMove kStatePacket;
	kStatePacket.bHeader = HEADER_CG_CHARACTER_MOVE;
	kStatePacket.bFunc = eFunc;
	kStatePacket.bArg = uArg;
	kStatePacket.bRot = fDstRot / 5.0f;
	kStatePacket.lX = long(c_rkPPosDst.x);
	kStatePacket.lY = long(c_rkPPosDst.y);
	kStatePacket.dwTime = ELTimer_GetServerMSec();

	assert(kStatePacket.lX >= 0 && kStatePacket.lX < 204800);

	__LocalPositionToGlobalPosition(kStatePacket.lX, kStatePacket.lY);

	if (!Send(sizeof(kStatePacket), &kStatePacket))
	{
		Tracenf("CPythonNetworkStream::SendCharacterStatePacket(dwCmdTime=%u, fDstPos=(%f, %f), fDstRot=%f, eFunc=%d uArg=%d) - PACKET SEND ERROR",
			kStatePacket.dwTime,
			float(kStatePacket.lX),
			float(kStatePacket.lY),
			fDstRot,
			kStatePacket.bFunc,
			kStatePacket.bArg);
		return false;
	}
	NANOEND
		return SendSequence();
}

// NOTE : SlotIndex는 임시
bool CPythonNetworkStream::SendUseSkillPacket(DWORD dwSkillIndex, DWORD dwTargetVID)
{
	TPacketCGUseSkill UseSkillPacket;
	UseSkillPacket.bHeader = HEADER_CG_USE_SKILL;
	UseSkillPacket.dwVnum = dwSkillIndex;
	UseSkillPacket.dwTargetVID = dwTargetVID;

	if (!Send(sizeof(TPacketCGUseSkill), &UseSkillPacket))
	{
		Tracen("CPythonNetworkStream::SendUseSkillPacket - SEND PACKET ERROR");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendChatPacket(const char* c_szChat, BYTE byType)
{
	if (m_strPhase != "Game")
		return true;

	if (strlen(c_szChat) == 0)
		return true;

	if (strlen(c_szChat) >= 512)
		return true;

	if (c_szChat[0] == '/')
	{
		if (1 == strlen(c_szChat))
		{
			if (!m_strLastCommand.empty())
				c_szChat = m_strLastCommand.c_str();
		}
		else
		{
			m_strLastCommand = c_szChat;
		}
	}

	if (ClientCommand(c_szChat))
		return true;

	int iTextLen = strlen(c_szChat) + 1;
	TPacketCGChat ChatPacket;
	ChatPacket.header = HEADER_CG_CHAT;
	ChatPacket.length = sizeof(ChatPacket) + iTextLen;
	ChatPacket.type = byType;

	if (!Send(sizeof(ChatPacket), &ChatPacket))
		return false;

	if (!Send(iTextLen, c_szChat))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendCommandPacket(const char* c_szChat)
{
	return SendChatPacket(c_szChat, CHAT_TYPE_COMMAND);
}

//////////////////////////////////////////////////////////////////////////
// Emoticon
void CPythonNetworkStream::RegisterEmoticonType(const BYTE bEmoticonType)
{
	if (m_EmoticonTypeVector.size() >= CInstanceBase::EMOTICON_NUM)
	{
		TraceError("Can't register emoticon string... vector is full (size:%d)", m_EmoticonTypeVector.size());
		return;
	}
	m_EmoticonTypeVector.push_back(bEmoticonType);
}

bool CPythonNetworkStream::ParseEmoticon(const BYTE bEmoticonType, DWORD* pdwEmoticon)
{
	for (DWORD dwEmoticonIndex = 0; dwEmoticonIndex < m_EmoticonTypeVector.size(); ++dwEmoticonIndex)
	{
		if (m_EmoticonTypeVector[dwEmoticonIndex] == bEmoticonType)
		{
			*pdwEmoticon = dwEmoticonIndex;
			return true;
		}
	}

	return false;
}
// Emoticon
//////////////////////////////////////////////////////////////////////////

void CPythonNetworkStream::__ConvertEmpireText(DWORD dwEmpireID, char* szText)
{
	if (dwEmpireID < 1 || dwEmpireID>3)
		return;

	UINT uHanPos;

	STextConvertTable& rkTextConvTable = m_aTextConvTable[dwEmpireID - 1];

	BYTE* pbText = (BYTE*)szText;
	while (*pbText)
	{
		if (*pbText & 0x80)
		{
			if (pbText[0] >= 0xb0 && pbText[0] <= 0xc8 && pbText[1] >= 0xa1 && pbText[1] <= 0xfe)
			{
				uHanPos = (pbText[0] - 0xb0) * (0xfe - 0xa1 + 1) + (pbText[1] - 0xa1);
				pbText[0] = rkTextConvTable.aacHan[uHanPos][0];
				pbText[1] = rkTextConvTable.aacHan[uHanPos][1];
			}
			pbText += 2;
		}
		else
		{
			if (*pbText >= 'a' && *pbText <= 'z')
			{
				*pbText = rkTextConvTable.acLower[*pbText - 'a'];
			}
			else if (*pbText >= 'A' && *pbText <= 'Z')
			{
				*pbText = rkTextConvTable.acUpper[*pbText - 'A'];
			}
			pbText++;
		}
	}
}

bool CPythonNetworkStream::RecvChatPacket()
{
	TPacketGCChat kChat;
	char buf[1024 + 1];
	char line[1024 + 1];

	if (!Recv(sizeof(kChat), &kChat))
		return false;

	UINT uChatSize = kChat.size - sizeof(kChat);

	if (!Recv(uChatSize, buf))
		return false;

	buf[uChatSize] = '\0';

#if defined(ENABLE_LOCALE_CLIENT)
	TTokenVector vecMultiLine;
	std::string sFormat = { buf };
	if (kChat.bCanFormat && CHAT_TYPE_SHOUT != kChat.type)
	{
		CPythonLocale::Instance().FormatString(sFormat);
		CPythonLocale::Instance().MultiLineSplit(sFormat, vecMultiLine);
	}
	else
	{
		CPythonLocale::Instance().FilterHyperlinkName(sFormat);
	}

	strncpy(buf, sFormat.c_str(), sizeof(buf));
	buf[sizeof(buf) - 1] = '\0';
#endif

	// 유럽 아랍 버전 처리
	// "이름: 내용" 입력을 "내용: 이름" 순서로 출력하기 위해 탭(0x08)을 넣음
	// 탭을 아랍어 기호로 처리해 (영어1) : (영어2) 로 입력되어도 (영어2) : (영어1) 로 출력하게 만든다
	if (LocaleService_IsEUROPE() && GetDefaultCodePage() == 1256)
	{
		char* p = strchr(buf, ':');
		if (p && p[1] == ' ')
			p[1] = 0x08;
	}

	if (kChat.type >= CHAT_TYPE_MAX_NUM)
		return true;

	if (CHAT_TYPE_COMMAND == kChat.type)
	{
		ServerCommand(buf);
		return true;
	}
	else if (CHAT_TYPE_MISSION == kChat.type)
	{
		if (uChatSize)
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetMissionMessage", Py_BuildValue("(s)", buf));
		else
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_CleanMissionMessage", Py_BuildValue("()"));

		return true;
	}
	else if (CHAT_TYPE_SUB_MISSION == kChat.type)
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetSubMissionMessage", Py_BuildValue("(s)", buf));
		return true;
	}

	if (kChat.dwVID != 0)
	{
		CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
		CInstanceBase* pkInstChatter = rkChrMgr.GetInstancePtr(kChat.dwVID);
		if (NULL == pkInstChatter)
			return true;

		switch (kChat.type)
		{
			case CHAT_TYPE_TALKING: /* 그냥 채팅 */
			case CHAT_TYPE_PARTY: /* 파티말 */
			case CHAT_TYPE_GUILD: /* 길드말 */
			case CHAT_TYPE_SHOUT: /* 외치기 */
			case CHAT_TYPE_WHISPER: // 서버와는 연동되지 않는 Only Client Enum
			{
				char* p = strchr(buf, ':');

				if (p)
					p += 2;
				else
					p = buf;

				if (gs_bEmpireLanuageEnable)
				{
					CInstanceBase* pkInstMain = rkChrMgr.GetMainInstancePtr();
					if (pkInstMain)
						if (!pkInstMain->IsSameEmpire(*pkInstChatter))
							__ConvertEmpireText(pkInstChatter->GetEmpireID(), p);
				}

				if (m_isEnableChatInsultFilter)
				{
					if (false == pkInstChatter->IsNPC() && false == pkInstChatter->IsEnemy())
					{
						__FilterInsult(p, strlen(p));
					}
				}

				_snprintf(line, sizeof(line), "%s", p);
			}
			break;

			case CHAT_TYPE_COMMAND: /* 명령 */
			case CHAT_TYPE_INFO: /* 정보 (아이템을 집었다, 경험치를 얻었다. 등) */
			case CHAT_TYPE_NOTICE: /* 공지사항 */
			case CHAT_TYPE_BIG_NOTICE:
#if defined(ENABLE_OX_RENEWAL)
			case CHAT_TYPE_BIG_CONTROL_NOTICE:
#endif
#if defined(ENABLE_DICE_SYSTEM)
			case CHAT_TYPE_DICE_INFO:
#endif
#if defined(ENABLE_CHATTING_WINDOW_RENEWAL)
			case CHAT_TYPE_EXP_INFO:
			case CHAT_TYPE_ITEM_INFO:
			case CHAT_TYPE_MONEY_INFO:
#endif
			case CHAT_TYPE_MAX_NUM:
			default:
				_snprintf(line, sizeof(line), "%s", buf);
				break;
		}

		if (CHAT_TYPE_SHOUT != kChat.type)
		{
			CPythonTextTail::Instance().RegisterChatTail(kChat.dwVID, line);
		}

		if (pkInstChatter->IsPC())
		{
#if defined(ENABLE_MESSENGER_BLOCK)
			if (CPythonMessenger::Instance().IsBlockFriendByName(pkInstChatter->GetNameString()))
				return true;
#endif

#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
			CPythonChat::Instance().AppendChat(kChat.type, buf, IAbstractChat::ESpecialColorType::CHAT_SPECIAL_COLOR_NORMAL, kChat.szCountry);
#else
			CPythonChat::Instance().AppendChat(kChat.type, buf);
#endif
		}
	}
	else
	{
		if (CHAT_TYPE_NOTICE == kChat.type)
		{
#if defined(ENABLE_LOCALE_CLIENT)
			if (!vecMultiLine.empty())
			{
				for (std::string& sMessage : vecMultiLine)
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetTipMessage", Py_BuildValue("(s)", sMessage.c_str()));
			}
			else
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetTipMessage", Py_BuildValue("(s)", buf));
			}
#else
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetTipMessage", Py_BuildValue("(s)", buf));
#endif
		}
		else if (CHAT_TYPE_BIG_NOTICE == kChat.type)
		{
#if defined(ENABLE_LOCALE_CLIENT)
			if (!vecMultiLine.empty())
			{
				for (const std::string& sMessage : vecMultiLine)
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetBigMessage", Py_BuildValue("(s)", sMessage.c_str()));
			}
			else
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetBigMessage", Py_BuildValue("(s)", buf));
			}
#else
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetBigMessage", Py_BuildValue("(s)", buf));
#endif
		}
#if defined(ENABLE_OX_RENEWAL)
		else if (CHAT_TYPE_BIG_CONTROL_NOTICE == kChat.type)
		{
#if defined(ENABLE_LOCALE_CLIENT)
			if (strstr(buf, "#start") || strstr(buf, "#send") || strstr(buf, "#end"))
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetBigControlMessage", Py_BuildValue("(s)", buf));
			}
			else
			{
				if (!vecMultiLine.empty())
				{
					for (const std::string& sMessage : vecMultiLine)
						PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetBigControlMessage", Py_BuildValue("(s)", sMessage.c_str()));
				}
				else
				{
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetBigControlMessage", Py_BuildValue("(s)", buf));
				}
			}
#else
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetBigControlMessage", Py_BuildValue("(s)", buf));
#endif
		}
#endif
		else if (CHAT_TYPE_SHOUT == kChat.type)
		{
			char* p = strchr(buf, ':');

			if (p)
			{
				if (m_isEnableChatInsultFilter)
					__FilterInsult(p, strlen(p));
			}
		}

#if defined(ENABLE_LOCALE_CLIENT)
		if (!vecMultiLine.empty())
		{
			for (const std::string& sMessage : vecMultiLine)
				CPythonChat::Instance().AppendChat(kChat.type, sMessage.c_str());
		}
		else
		{
			if (buf[0] == '@')
				CPythonChat::Instance().AppendChat(kChat.type, &buf[1], IAbstractChat::ESpecialColorType::CHAT_SPECIAL_COLOR_DICE_0);
			else if (buf[0] == '$')
				CPythonChat::Instance().AppendChat(kChat.type, &buf[1], IAbstractChat::ESpecialColorType::CHAT_SPECIAL_COLOR_DICE_1);
			else
			{
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
				CPythonChat::Instance().AppendChat(kChat.type, buf, IAbstractChat::ESpecialColorType::CHAT_SPECIAL_COLOR_NORMAL, kChat.szCountry);
#else
				CPythonChat::Instance().AppendChat(kChat.type, buf);
#endif
			}
		}
#else
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
		CPythonChat::Instance().AppendChat(kChat.type, buf, kChat.szCountry);
#else
		CPythonChat::Instance().AppendChat(kChat.type, buf);
#endif
#endif
	}

	return true;
}

bool CPythonNetworkStream::RecvWhisperPacket()
{
	TPacketGCWhisper whisperPacket;
	char buf[512 + 1];

	if (!Recv(sizeof(whisperPacket), &whisperPacket))
		return false;

	assert(whisperPacket.wSize - sizeof(whisperPacket) < 512);

	if (!Recv(whisperPacket.wSize - sizeof(whisperPacket), &buf))
		return false;

	buf[whisperPacket.wSize - sizeof(whisperPacket)] = '\0';

#if defined(ENABLE_LOCALE_CLIENT)
	TTokenVector vecMultiLine;
	if (whisperPacket.bCanFormat)
	{
		CPythonLocale::Instance().FormatString(buf, sizeof(buf));
		CPythonLocale::Instance().FormatString(whisperPacket.szNameFrom, sizeof(whisperPacket.szNameFrom));
	}
#endif

	static char line[256];
	if (CPythonChat::WHISPER_TYPE_CHAT == whisperPacket.bType || CPythonChat::WHISPER_TYPE_GM == whisperPacket.bType)
	{
		if (GetDefaultCodePage() == CP_ARABIC)
			_snprintf(line, sizeof(line), "%s", buf);
		else
			_snprintf(line, sizeof(line), "%s : %s", whisperPacket.szNameFrom, buf);

#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnRecvWhisper", Py_BuildValue("(isss)", (int)whisperPacket.bType, whisperPacket.szNameFrom, line, whisperPacket.szCountry));
#else
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnRecvWhisper", Py_BuildValue("(iss)", (int)whisperPacket.bType, whisperPacket.szNameFrom, line));
#endif
	}
	else if (CPythonChat::WHISPER_TYPE_SYSTEM == whisperPacket.bType || CPythonChat::WHISPER_TYPE_ERROR == whisperPacket.bType)
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnRecvWhisperSystemMessage", Py_BuildValue("(iss)", (int)whisperPacket.bType, whisperPacket.szNameFrom, buf));
	}
	else
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnRecvWhisperError", Py_BuildValue("(iss)", (int)whisperPacket.bType, whisperPacket.szNameFrom, buf));
	}

	return true;
}

bool CPythonNetworkStream::SendWhisperPacket(const char* name, const char* c_szChat)
{
	if (strlen(c_szChat) >= 255)
		return true;

	int iTextLen = strlen(c_szChat) + 1;
	TPacketCGWhisper WhisperPacket;
	WhisperPacket.bHeader = HEADER_CG_WHISPER;
	WhisperPacket.wSize = sizeof(WhisperPacket) + iTextLen;

	strncpy(WhisperPacket.szNameTo, name, sizeof(WhisperPacket.szNameTo) - 1);

	if (!Send(sizeof(WhisperPacket), &WhisperPacket))
		return false;

	if (!Send(iTextLen, c_szChat))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvPointChange()
{
	TPacketGCPointChange PointChange;

	if (!Recv(sizeof(TPacketGCPointChange), &PointChange))
	{
		Tracen("Recv Point Change Packet Error");
		return false;
	}

	CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
	rkChrMgr.ShowPointEffect(PointChange.wType, PointChange.dwVID);

	CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetMainInstancePtr();

	// 자신의 Point가 변경되었을 경우..
	if (pInstance)
	{
		if (PointChange.dwVID == pInstance->GetVirtualID())
		{
			CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
			rkPlayer.SetStatus(PointChange.wType, PointChange.lValue);

			switch (PointChange.wType)
			{
				case POINT_STAT_RESET_COUNT:
					__RefreshStatus();
					break;

#if defined(ENABLE_IMPROVED_LOGOUT_POINTS)
				case POINT_PLAYTIME:
					m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].dwPlayMinutes = PointChange.lValue;
					break;
#endif

				case POINT_LEVEL:
#if defined(ENABLE_IMPROVED_LOGOUT_POINTS)
				{
					m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].byLevel = PointChange.lValue;
					__RefreshStatus();
					__RefreshSkillWindow();
				}
				break;
#endif

				case POINT_ST:
#if defined(ENABLE_IMPROVED_LOGOUT_POINTS)
				{
					m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].byST = PointChange.lValue;
					__RefreshStatus();
					__RefreshSkillWindow();
				}
				break;
#endif

				case POINT_DX:
#if defined(ENABLE_IMPROVED_LOGOUT_POINTS)
				{
					m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].byDX = PointChange.lValue;
					__RefreshStatus();
					__RefreshSkillWindow();
				}
				break;
#endif

				case POINT_HT:
#if defined(ENABLE_IMPROVED_LOGOUT_POINTS)
				{
					m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].byHT = PointChange.lValue;
					__RefreshStatus();
					__RefreshSkillWindow();
				}
				break;
#endif

				case POINT_IQ:
#if defined(ENABLE_IMPROVED_LOGOUT_POINTS)
				{
					m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].byIQ = PointChange.lValue;
					__RefreshStatus();
					__RefreshSkillWindow();
				}
				break;
#endif

#if defined(ENABLE_CONQUEROR_LEVEL)
				case POINT_CONQUEROR_LEVEL:
				{
					pInstance->UpdateTextTailConquerorLevel(PointChange.lValue);
#if defined(ENABLE_IMPROVED_LOGOUT_POINTS)
					m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].byConquerorLevel = PointChange.lValue;
					__RefreshStatus();
					__RefreshSkillWindow();
#endif
				}
				break;

				case POINT_SUNGMA_STR:
#if defined(ENABLE_IMPROVED_LOGOUT_POINTS)
				{
					m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].bySungmaStr = PointChange.lValue;
					__RefreshStatus();
					__RefreshSkillWindow();
				}
				break;
#endif

				case POINT_SUNGMA_HP:
#if defined(ENABLE_IMPROVED_LOGOUT_POINTS)
				{
					m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].bySungmaHp = PointChange.lValue;
					__RefreshStatus();
					__RefreshSkillWindow();
				}
				break;
#endif

				case POINT_SUNGMA_MOVE:
#if defined(ENABLE_IMPROVED_LOGOUT_POINTS)
				{
					m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].bySungmaMove = PointChange.lValue;
					__RefreshStatus();
					__RefreshSkillWindow();
				}
				break;
#endif

				case POINT_SUNGMA_IMMUNE:
#if defined(ENABLE_IMPROVED_LOGOUT_POINTS)
				{
					m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].bySungmaImmune = PointChange.lValue;
					__RefreshStatus();
					__RefreshSkillWindow();
				}
				break;
#endif
#endif

				case POINT_SKILL:
				case POINT_SUB_SKILL:
				case POINT_HORSE_SKILL:
					__RefreshSkillWindow();
					break;

				case POINT_ENERGY:
				{
					if (PointChange.lValue == 0)
					{
						rkPlayer.SetStatus(POINT_ENERGY_END_TIME, 0);
					}
					__RefreshStatus();
				}
				break;

				default:
					__RefreshStatus();
					break;
			}

			if (POINT_GOLD == PointChange.wType)
			{
				if (PointChange.lAmount > 0)
				{
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnPickMoney", Py_BuildValue("(i)", PointChange.lAmount));
				}
			}

#if defined(ENABLE_CHEQUE_SYSTEM)
			if (POINT_CHEQUE == PointChange.wType)
			{
				if (PointChange.lAmount > 0)
				{
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnPickCheque", Py_BuildValue("(i)", PointChange.lAmount));
				}
			}
#endif

#if defined(ENABLE_GEM_SYSTEM)
			if (POINT_GEM == PointChange.wType)
			{
				if (PointChange.lAmount > 0)
				{
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnPickGem", Py_BuildValue("(i)", PointChange.lAmount));
				}
			}
#endif
		}
		else if (POINT_LEVEL == PointChange.wType)
		{
			CInstanceBase* pOtherInstance = CPythonCharacterManager::Instance().GetInstancePtr(PointChange.dwVID);
			if (pOtherInstance)
				pOtherInstance->UpdateTextTailLevel(PointChange.lValue);
		}
#if defined(ENABLE_CONQUEROR_LEVEL)
		else if (POINT_CONQUEROR_LEVEL == PointChange.wType)
		{
			CInstanceBase* pOtherInstance = CPythonCharacterManager::Instance().GetInstancePtr(PointChange.dwVID);
			if (pOtherInstance)
				pOtherInstance->UpdateTextTailConquerorLevel(PointChange.lValue);
		}
#endif
	}

	return true;
}

bool CPythonNetworkStream::RecvStunPacket()
{
	TPacketGCStun StunPacket;

	if (!Recv(sizeof(StunPacket), &StunPacket))
	{
		Tracen("CPythonNetworkStream::RecvStunPacket Error");
		return false;
	}

	//Tracef("RecvStunPacket %d\n", StunPacket.vid);

	CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
	CInstanceBase* pkInstSel = rkChrMgr.GetInstancePtr(StunPacket.vid);

	if (pkInstSel)
	{
		if (CPythonCharacterManager::Instance().GetMainInstancePtr() == pkInstSel)
			pkInstSel->Die();
		else
			pkInstSel->Stun();
	}

	return true;
}

bool CPythonNetworkStream::RecvDeadPacket()
{
	TPacketGCDead DeadPacket;
	if (!Recv(sizeof(DeadPacket), &DeadPacket))
	{
		Tracen("CPythonNetworkStream::RecvDeadPacket Error");
		return false;
	}

	CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
	CInstanceBase* pkChrInstSel = rkChrMgr.GetInstancePtr(DeadPacket.vid);
	if (pkChrInstSel)
	{
		CInstanceBase* pkInstMain = rkChrMgr.GetMainInstancePtr();
		if (pkInstMain == pkChrInstSel)
		{
			Tracenf("주인공 사망");

			if (false == pkInstMain->GetDuelMode())
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnGameOver", Py_BuildValue("(ii)", DeadPacket.map_index, DeadPacket.dialog_type));

			CPythonPlayer::Instance().NotifyDeadMainCharacter();
#if defined(ENABLE_SKILL_COOLTIME_UPDATE)
			CPythonPlayer::Instance().ResetSkillCoolTimes();
#endif
		}

		pkChrInstSel->Die();
	}

	return true;
}

bool CPythonNetworkStream::SendCharacterPositionPacket(BYTE iPosition)
{
	TPacketCGPosition PositionPacket;

	PositionPacket.header = HEADER_CG_CHARACTER_POSITION;
	PositionPacket.position = iPosition;

	if (!Send(sizeof(TPacketCGPosition), &PositionPacket))
	{
		Tracen("Send Character Position Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendOnClickPacket(DWORD vid)
{
	TPacketCGOnClick OnClickPacket;
	OnClickPacket.header = HEADER_CG_ON_CLICK;
	OnClickPacket.vid = vid;

	if (!Send(sizeof(OnClickPacket), &OnClickPacket))
	{
		Tracen("Send On_Click Packet Error");
		return false;
	}

	Tracef("SendOnClickPacket\n");
	return SendSequence();
}

bool CPythonNetworkStream::RecvCharacterPositionPacket()
{
	TPacketGCPosition PositionPacket;

	if (!Recv(sizeof(TPacketGCPosition), &PositionPacket))
		return false;

	CInstanceBase* pChrInstance = CPythonCharacterManager::Instance().GetInstancePtr(PositionPacket.vid);

	if (!pChrInstance)
		return true;

	//pChrInstance->UpdatePosition(PositionPacket.position);

	return true;
}

bool CPythonNetworkStream::RecvMotionPacket()
{
	TPacketGCMotion MotionPacket;

	if (!Recv(sizeof(TPacketGCMotion), &MotionPacket))
		return false;

	CInstanceBase* pMainInstance = CPythonCharacterManager::Instance().GetInstancePtr(MotionPacket.vid);
	CInstanceBase* pVictimInstance = NULL;

	if (0 != MotionPacket.victim_vid)
		pVictimInstance = CPythonCharacterManager::Instance().GetInstancePtr(MotionPacket.victim_vid);

	if (!pMainInstance)
		return false;

	return true;
}

bool CPythonNetworkStream::RecvShopPacket()
{
	std::vector<char> vecBuffer;
	vecBuffer.clear();

	TPacketGCShop packet_shop;
	if (!Recv(sizeof(packet_shop), &packet_shop))
		return false;

	int iSize = packet_shop.size - sizeof(packet_shop);
	if (iSize > 0)
	{
		vecBuffer.resize(iSize);
		if (!Recv(iSize, &vecBuffer[0]))
			return false;
	}

	switch (packet_shop.subheader)
	{
		case SHOP_SUBHEADER_GC_START:
		{
			CPythonShop::Instance().Clear();

			DWORD dwVID = *(DWORD*)&vecBuffer[0];

			TPacketGCShopStart* pShopStartPacket = (TPacketGCShopStart*)&vecBuffer[4];
#if defined(ENABLE_MYSHOP_DECO)
			BYTE shop_tab_count = pShopStartPacket->shop_tab_count;
			CPythonShop::Instance().SetTabCount(shop_tab_count);
#endif

#if defined(ENABLE_MYSHOP_EXPANSION)
			for (BYTE iItemIndex = 0; iItemIndex < 80; ++iItemIndex)
#else
			for (BYTE iItemIndex = 0; iItemIndex < SHOP_HOST_ITEM_MAX_NUM; ++iItemIndex)
#endif
				CPythonShop::Instance().SetItemData(iItemIndex, pShopStartPacket->items[iItemIndex]);

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "StartShop", Py_BuildValue("(i)", dwVID));
		}
		break;

		case SHOP_SUBHEADER_GC_END:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "EndShop", Py_BuildValue("()"));
			break;

		case SHOP_SUBHEADER_GC_UPDATE_ITEM:
		{
			TPacketGCShopUpdateItem* pShopUpdateItemPacket = (TPacketGCShopUpdateItem*)&vecBuffer[0];
			CPythonShop::Instance().SetItemData(pShopUpdateItemPacket->pos, pShopUpdateItemPacket->item);
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshShop", Py_BuildValue("()"));
		}
		break;

		case SHOP_SUBHEADER_GC_UPDATE_PRICE:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetShopSellingPrice", Py_BuildValue("(i)", *(int*)&vecBuffer[0]));
			break;

		case SHOP_SUBHEADER_GC_OK:
			break;

		case SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_MONEY"));
			break;

		case SHOP_SUBHEADER_GC_SOLDOUT:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "SOLDOUT"));
			break;

		case SHOP_SUBHEADER_GC_INVENTORY_FULL:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "INVENTORY_FULL"));
			break;

		case SHOP_SUBHEADER_GC_INVALID_POS:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "INVALID_POS"));
			break;

		case SHOP_SUBHEADER_GC_SOLD_OUT:
			break;


		case SHOP_SUBHEADER_GC_START_EX:
		{
			CPythonShop::Instance().Clear();

			TPacketGCShopStartEx* pShopStartPacket = (TPacketGCShopStartEx*)&vecBuffer[0];
			size_t read_point = sizeof(TPacketGCShopStartEx);

			DWORD dwVID = pShopStartPacket->owner_vid;
			BYTE shop_tab_count = pShopStartPacket->shop_tab_count;

			CPythonShop::Instance().SetTabCount(shop_tab_count);

			for (BYTE i = 0; i < shop_tab_count; i++)
			{
				TPacketGCShopStartEx::TSubPacketShopTab* pPackTab = (TPacketGCShopStartEx::TSubPacketShopTab*)&vecBuffer[read_point];
				read_point += sizeof(TPacketGCShopStartEx::TSubPacketShopTab);

				CPythonShop::Instance().SetTabCoinType(i, pPackTab->coin_type);
				CPythonShop::Instance().SetTabName(i, pPackTab->name);

				struct packet_shop_item* item = &pPackTab->items[0];

				for (BYTE j = 0; j < SHOP_HOST_ITEM_MAX_NUM; j++)
				{
					TShopItemData* itemData = (item + j);
					if (itemData)
						CPythonShop::Instance().SetItemData(i, j, *itemData);
				}
			}

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "StartShop", Py_BuildValue("(i)", dwVID));
		}
		break;

		case SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY_EX:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_MONEY_EX"));
			break;

		case SHOP_SUBHEADER_GC_NOT_ENOUGH_BP:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_BP"));
			break;

		case SHOP_SUBHEADER_GC_EXCEED_LIMIT_TODAY:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "EXCEED_LIMIT_TODAY"));
			break;

#if defined(ENABLE_MYSHOP_DECO)
		case SHOP_SUBHEADER_GC_MYPRIV_SHOP_OPEN:
		{
			const TPacketGCMyPrivShopOpen* pMyPrivShopOpenPacket = (TPacketGCMyPrivShopOpen*)&vecBuffer[0];
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MyPrivShopOpen", Py_BuildValue("(bi)", pMyPrivShopOpenPacket->bCashItem, pMyPrivShopOpenPacket->bTabCount));
		}
		break;
#endif

		case SHOP_SUBHEADER_GC_NOT_ENOUGH_10TH_COIN:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_10TH_COIN"));
			break;

		case SHOP_SUBHEADER_GC_LIMITED_PURCHASE_OVER:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "LIMITED_PURCHASE_OVER"));
			break;

		case SHOP_SUBHEADER_GC_LIMITED_DATA_LOADING:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "LIMITED_DATA_LOADING"));
			break;

			// 12ZI RELATED
		case SHOP_SUBHEADER_GC_UNK_19:
			break;
		case SHOP_SUBHEADER_GC_UNK_20:
			break;
		case SHOP_SUBHEADER_GC_UNK_21:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshShopItemToolTip", Py_BuildValue("()"));
			break;
			// END_OF_12ZI_RELATED

		case SHOP_SUBHEADER_GC_NOT_ENOUGH_GUILD_LEVEL:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_GUILD_LEVEL"));
			break;

		case SHOP_SUBHEADER_GC_NOT_ENOUGH_MEDAL_OF_HONOR:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_MEDAL_OF_HONOR"));
			break;

		case SHOP_SUBHEADER_GC_NOT_ENOUGH_BNW:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_BNW"));
			break;

#if defined(ENABLE_SHOPEX_RENEWAL)
		case SHOP_SUBHEADER_GC_NOT_ENOUGH_ITEM:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_ITEM"));
			break;
#endif

		default:
			TraceError("CPythonNetworkStream::RecvShopPacket: Unknown subheader\n");
			break;
	}

	return true;
}

bool CPythonNetworkStream::RecvExchangePacket()
{
	TPacketGCExchange exchange_packet;

	if (!Recv(sizeof(exchange_packet), &exchange_packet))
		return false;

	switch (exchange_packet.subheader)
	{
		case EXCHANGE_SUBHEADER_GC_START:
		{
			CPythonExchange::Instance().Clear();
			CPythonExchange::Instance().Start();
			CPythonExchange::Instance().SetSelfName(CPythonPlayer::Instance().GetName());

			{
				CInstanceBase* pCharacterInstance = CPythonCharacterManager::Instance().GetInstancePtr(exchange_packet.arg1);

				if (pCharacterInstance)
				{
					CPythonExchange::Instance().SetTargetName(pCharacterInstance->GetNameString());
					CPythonExchange::Instance().SetLevelToTarget(pCharacterInstance->GetLevel());
				}
			}

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "StartExchange", Py_BuildValue("()"));
		}
		break;

		case EXCHANGE_SUBHEADER_GC_ITEM_ADD:
		{
			if (exchange_packet.is_me)
			{
				int iSlotIndex = exchange_packet.arg2.cell;
				CPythonExchange::Instance().SetItemToSelf(iSlotIndex, exchange_packet.arg1, (WORD)exchange_packet.arg3);

				for (BYTE bSlotIdx = 0; bSlotIdx < ITEM_SOCKET_SLOT_MAX_NUM; ++bSlotIdx)
					CPythonExchange::Instance().SetItemMetinSocketToSelf(iSlotIndex, bSlotIdx, exchange_packet.alValues[bSlotIdx]);

				for (BYTE bSlotIdx = 0; bSlotIdx < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++bSlotIdx)
					CPythonExchange::Instance().SetItemAttributeToSelf(iSlotIndex, bSlotIdx,
						exchange_packet.aAttr[bSlotIdx].wType, exchange_packet.aAttr[bSlotIdx].lValue);

#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
				CPythonExchange::Instance().SetItemChangeLookVnumToSelf(iSlotIndex, exchange_packet.dwTransmutationVnum);
#endif

#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
				CPythonExchange::Instance().SetItemRefineElementToSelf(iSlotIndex, exchange_packet.RefineElement);
#endif

#if defined(ENABLE_APPLY_RANDOM)
				for (BYTE bSlotIdx = 0; bSlotIdx < ITEM_APPLY_RANDOM_SLOT_MAX_NUM; ++bSlotIdx)
					CPythonExchange::Instance().SetItemApplyRandomToSelf(iSlotIndex, bSlotIdx,
						exchange_packet.aApplyRandom[bSlotIdx].wType, exchange_packet.aApplyRandom[bSlotIdx].lValue);
#endif

#if defined(ENABLE_SET_ITEM)
				CPythonExchange::Instance().SetItemSetValueToSelf(iSlotIndex, exchange_packet.bSetValue);
#endif

#if defined(WJ_ENABLE_TRADABLE_ICON)
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AddExchangeItemSlotIndex", Py_BuildValue("(i)", exchange_packet.arg4.cell));
#endif
			}
			else
			{
				int iSlotIndex = exchange_packet.arg2.cell;
				CPythonExchange::Instance().SetItemToTarget(iSlotIndex, exchange_packet.arg1, (WORD)exchange_packet.arg3);

				for (BYTE bSlotIdx = 0; bSlotIdx < ITEM_SOCKET_SLOT_MAX_NUM; ++bSlotIdx)
					CPythonExchange::Instance().SetItemMetinSocketToTarget(iSlotIndex, bSlotIdx, exchange_packet.alValues[bSlotIdx]);

				for (BYTE bSlotIdx = 0; bSlotIdx < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++bSlotIdx)
					CPythonExchange::Instance().SetItemAttributeToTarget(iSlotIndex, bSlotIdx,
						exchange_packet.aAttr[bSlotIdx].wType, exchange_packet.aAttr[bSlotIdx].lValue);

#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
				CPythonExchange::Instance().SetItemChangeLookVnumToTarget(iSlotIndex, exchange_packet.dwTransmutationVnum);
#endif

#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
				CPythonExchange::Instance().SetItemRefineElementToTarget(iSlotIndex, exchange_packet.RefineElement);
#endif

#if defined(ENABLE_APPLY_RANDOM)
				for (BYTE bSlotIdx = 0; bSlotIdx < ITEM_APPLY_RANDOM_SLOT_MAX_NUM; ++bSlotIdx)
					CPythonExchange::Instance().SetItemApplyRandomToTarget(iSlotIndex, bSlotIdx,
						exchange_packet.aApplyRandom[bSlotIdx].wType, exchange_packet.aApplyRandom[bSlotIdx].lValue);
#endif

#if defined(ENABLE_SET_ITEM)
				CPythonExchange::Instance().SetItemSetValueToTarget(iSlotIndex, exchange_packet.bSetValue);
#endif
			}

			__RefreshExchangeWindow();
			__RefreshInventoryWindow();
		}
		break;

		case EXCHANGE_SUBHEADER_GC_ITEM_DEL:
		{
			if (exchange_packet.is_me)
			{
				CPythonExchange::Instance().DelItemOfSelf((WORD)exchange_packet.arg1);
			}
			else
			{
				CPythonExchange::Instance().DelItemOfTarget((WORD)exchange_packet.arg1);
			}
			__RefreshExchangeWindow();
			__RefreshInventoryWindow();
		}
		break;

		case EXCHANGE_SUBHEADER_GC_ELK_ADD:
		{
			if (exchange_packet.is_me)
				CPythonExchange::Instance().SetElkToSelf(exchange_packet.arg1);
			else
				CPythonExchange::Instance().SetElkToTarget(exchange_packet.arg1);

			__RefreshExchangeWindow();
		}
		break;

#if defined(ENABLE_CHEQUE_SYSTEM)
		case EXCHANGE_SUBHEADER_GC_CHEQUE_ADD:
		{
			if (exchange_packet.is_me)
				CPythonExchange::Instance().SetChequeToSelf(exchange_packet.arg1);
			else
				CPythonExchange::Instance().SetChequeToTarget(exchange_packet.arg1);

			__RefreshExchangeWindow();
		}
		break;
#endif

		case EXCHANGE_SUBHEADER_GC_ACCEPT:
		{
			if (exchange_packet.is_me)
			{
				CPythonExchange::Instance().SetAcceptToSelf((BYTE)exchange_packet.arg1);
			}
			else
			{
				CPythonExchange::Instance().SetAcceptToTarget((BYTE)exchange_packet.arg1);
			}
			__RefreshExchangeWindow();
		}
		break;

		case EXCHANGE_SUBHEADER_GC_END:
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "EndExchange", Py_BuildValue("()"));
			__RefreshInventoryWindow();
			CPythonExchange::Instance().End();
		}
		break;

		case EXCHANGE_SUBHEADER_GC_ALREADY:
			Tracef("trade_already");
			break;

		case EXCHANGE_SUBHEADER_GC_LESS_ELK:
			Tracef("trade_less_elk");
			break;
	};

	return true;
}

bool CPythonNetworkStream::RecvQuestInfoPacket()
{
	TPacketGCQuestInfo QuestInfo;

	if (!Peek(sizeof(TPacketGCQuestInfo), &QuestInfo))
	{
		Tracen("Recv Quest Info Packet Error #1");
		return false;
	}

	if (!Peek(QuestInfo.size))
	{
		Tracen("Recv Quest Info Packet Error #2");
		return false;
	}

	Recv(sizeof(TPacketGCQuestInfo));

	const BYTE& c_rFlag = QuestInfo.flag;

	enum
	{
		QUEST_PACKET_TYPE_NONE,
		QUEST_PACKET_TYPE_BEGIN,
		QUEST_PACKET_TYPE_UPDATE,
		QUEST_PACKET_TYPE_END,
	};

	enum EQuestLen
	{
#if defined(ENABLE_QUEST_RENEWAL)
		QUEST_TITLE_NAME_LEN = 50,
#else
		QUEST_TITLE_NAME_LEN = 30,
#endif
		QUEST_CLOCK_NAME_LEN = 16,
		QUEST_COUNTER_NAME_LEN = 16,
		QUEST_ICON_FILE_NAME_LEN = 24,
	};

	BYTE byQuestPacketType = QUEST_PACKET_TYPE_NONE;

	if (0 != (c_rFlag & QUEST_SEND_IS_BEGIN))
	{
		BYTE isBegin;
		if (!Recv(sizeof(isBegin), &isBegin))
			return false;

		if (isBegin)
			byQuestPacketType = QUEST_PACKET_TYPE_BEGIN;
		else
			byQuestPacketType = QUEST_PACKET_TYPE_END;
	}
	else
	{
		byQuestPacketType = QUEST_PACKET_TYPE_UPDATE;
	}

	// Recv Data Start
	char szTitle[QUEST_TITLE_NAME_LEN + 1] = "";
	char szClockName[QUEST_CLOCK_NAME_LEN + 1] = "";
	int iClockValue = 0;
	char szCounterName[QUEST_COUNTER_NAME_LEN + 1] = "";
	int iCounterValue = 0;
	char szIconFileName[QUEST_ICON_FILE_NAME_LEN + 1] = "";

	if (0 != (c_rFlag & QUEST_SEND_TITLE))
	{
		if (!Recv(sizeof(szTitle), &szTitle))
			return false;

		szTitle[QUEST_TITLE_NAME_LEN] = '\0';
#if defined(ENABLE_LOCALE_CLIENT)
		CPythonLocale::Instance().FormatString(szTitle, sizeof(szTitle));
#endif
	}

	if (0 != (c_rFlag & QUEST_SEND_CLOCK_NAME))
	{
		if (!Recv(sizeof(szClockName), &szClockName))
			return false;

		szClockName[QUEST_CLOCK_NAME_LEN] = '\0';
#if defined(ENABLE_LOCALE_CLIENT)
		CPythonLocale::Instance().FormatString(szClockName, sizeof(szClockName));
#endif
	}

	if (0 != (c_rFlag & QUEST_SEND_CLOCK_VALUE))
	{
		if (!Recv(sizeof(iClockValue), &iClockValue))
			return false;
	}

	if (0 != (c_rFlag & QUEST_SEND_COUNTER_NAME))
	{
		if (!Recv(sizeof(szCounterName), &szCounterName))
			return false;

		szCounterName[QUEST_COUNTER_NAME_LEN] = '\0';
#if defined(ENABLE_LOCALE_CLIENT)
		CPythonLocale::Instance().FormatString(szCounterName, sizeof(szCounterName));
#endif
	}

	if (0 != (c_rFlag & QUEST_SEND_COUNTER_VALUE))
	{
		if (!Recv(sizeof(iCounterValue), &iCounterValue))
			return false;
	}

	if (0 != (c_rFlag & QUEST_SEND_ICON_FILE))
	{
		if (!Recv(sizeof(szIconFileName), &szIconFileName))
			return false;

		szIconFileName[QUEST_ICON_FILE_NAME_LEN] = '\0';
	}
	// Recv Data End

	CPythonQuest& rkQuest = CPythonQuest::Instance();

	// Process Start
	if (QUEST_PACKET_TYPE_END == byQuestPacketType)
	{
		rkQuest.DeleteQuestInstance(QuestInfo.index);
#if defined(ENABLE_QUEST_RENEWAL)
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "DeleteQuest", Py_BuildValue("(ii)", QuestInfo.type, QuestInfo.index));
#endif
	}
	else if (QUEST_PACKET_TYPE_UPDATE == byQuestPacketType)
	{
		if (!rkQuest.IsQuest(QuestInfo.index))
		{
#if defined(ENABLE_QUEST_RENEWAL)
			rkQuest.MakeQuest(QuestInfo.index, QuestInfo.type, QuestInfo.is_confirmed);
#else
			rkQuest.MakeQuest(QuestInfo.index);
#endif
		}

		if (strlen(szTitle) > 0)
			rkQuest.SetQuestTitle(QuestInfo.index, szTitle);
		if (strlen(szClockName) > 0)
			rkQuest.SetQuestClockName(QuestInfo.index, szClockName);
		if (strlen(szCounterName) > 0)
			rkQuest.SetQuestCounterName(QuestInfo.index, szCounterName);
		if (strlen(szIconFileName) > 0)
			rkQuest.SetQuestIconFileName(QuestInfo.index, szIconFileName);

		if (c_rFlag & QUEST_SEND_CLOCK_VALUE)
			rkQuest.SetQuestClockValue(QuestInfo.index, iClockValue);
		if (c_rFlag & QUEST_SEND_COUNTER_VALUE)
			rkQuest.SetQuestCounterValue(QuestInfo.index, iCounterValue);

#if defined(ENABLE_QUEST_RENEWAL)
		rkQuest.SetQuestIsConfirmed(QuestInfo.index, QuestInfo.is_confirmed);
#endif
	}
	else if (QUEST_PACKET_TYPE_BEGIN == byQuestPacketType)
	{
		CPythonQuest::SQuestInstance QuestInstance;
		QuestInstance.dwIndex = QuestInfo.index;
		QuestInstance.strTitle = szTitle;
		QuestInstance.strClockName = szClockName;
		QuestInstance.iClockValue = iClockValue;
		QuestInstance.strCounterName = szCounterName;
		QuestInstance.iCounterValue = iCounterValue;
		QuestInstance.strIconFileName = szIconFileName;
		//CPythonQuest::Instance().RegisterQuestInstance(QuestInstance);
		rkQuest.RegisterQuestInstance(QuestInstance);
	}
	// Process Start End

#if defined(ENABLE_QUEST_RENEWAL)
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshQuest", Py_BuildValue("(ii)", QuestInfo.type, QuestInfo.index));
#else
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshQuest", Py_BuildValue("()"));
#endif
	return true;
}

bool CPythonNetworkStream::RecvQuestConfirmPacket()
{
	TPacketGCQuestConfirm kQuestConfirmPacket;
	if (!Recv(sizeof(kQuestConfirmPacket), &kQuestConfirmPacket))
	{
		Tracen("RecvQuestConfirmPacket Error");
		return false;
	}

#if defined(ENABLE_LOCALE_CLIENT)
	CPythonLocale::Instance().FormatString(kQuestConfirmPacket.msg, sizeof(kQuestConfirmPacket.msg));
#endif

	PyObject* poArg = Py_BuildValue("(sii)", kQuestConfirmPacket.msg, kQuestConfirmPacket.timeout, kQuestConfirmPacket.requestPID);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OnQuestConfirm", poArg);
	return true;
}

bool CPythonNetworkStream::RecvRequestMakeGuild()
{
	TPacketGCBlank blank;
	if (!Recv(sizeof(blank), &blank))
	{
		Tracen("RecvRequestMakeGuild Packet Error");
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AskGuildName", Py_BuildValue("()"));

	return true;
}

void CPythonNetworkStream::ToggleGameDebugInfo()
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ToggleDebugInfo", Py_BuildValue("()"));
}

bool CPythonNetworkStream::SendExchangeStartPacket(DWORD vid)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGExchange	packet;

	packet.header = HEADER_CG_EXCHANGE;
	packet.subheader = EXCHANGE_SUBHEADER_CG_START;
	packet.arg1 = vid;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_start_packet Error\n");
		return false;
	}

	Tracef("send_trade_start_packet vid %d \n", vid);
	return SendSequence();
}

bool CPythonNetworkStream::SendExchangeElkAddPacket(DWORD elk)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGExchange	packet;

	packet.header = HEADER_CG_EXCHANGE;
	packet.subheader = EXCHANGE_SUBHEADER_CG_ELK_ADD;
	packet.arg1 = elk;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_elk_add_packet Error\n");
		return false;
	}

	return SendSequence();
}

#if defined(ENABLE_CHEQUE_SYSTEM)
bool CPythonNetworkStream::SendExchangeChequeAddPacket(DWORD cheque)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGExchange	packet;

	packet.header = HEADER_CG_EXCHANGE;
	packet.subheader = EXCHANGE_SUBHEADER_CG_CHEQUE_ADD;
	packet.arg1 = cheque;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_cheque_add_packet Error\n");
		return false;
	}

	return SendSequence();
}
#endif

bool CPythonNetworkStream::SendExchangeItemAddPacket(TItemPos ItemPos, BYTE byDisplayPos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGExchange	packet;

	packet.header = HEADER_CG_EXCHANGE;
	packet.subheader = EXCHANGE_SUBHEADER_CG_ITEM_ADD;
	packet.Pos = ItemPos;
	packet.arg2 = byDisplayPos;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_item_add_packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendExchangeItemDelPacket(BYTE pos)
{
	assert(!"Can't be called function - CPythonNetworkStream::SendExchangeItemDelPacket");
	return true;

	if (!__CanActMainInstance())
		return true;

	TPacketCGExchange	packet;

	packet.header = HEADER_CG_EXCHANGE;
	packet.subheader = EXCHANGE_SUBHEADER_CG_ITEM_DEL;
	packet.arg1 = pos;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_item_del_packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendExchangeAcceptPacket()
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGExchange	packet;

	packet.header = HEADER_CG_EXCHANGE;
	packet.subheader = EXCHANGE_SUBHEADER_CG_ACCEPT;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_accept_packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendExchangeExitPacket()
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGExchange	packet;

	packet.header = HEADER_CG_EXCHANGE;
	packet.subheader = EXCHANGE_SUBHEADER_CG_CANCEL;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_exit_packet Error\n");
		return false;
	}

	return SendSequence();
}

// PointReset 개임시
bool CPythonNetworkStream::SendPointResetPacket()
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "StartPointReset", Py_BuildValue("()"));
	return true;
}

bool CPythonNetworkStream::__IsPlayerAttacking()
{
	CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
	CInstanceBase* pkInstMain = rkChrMgr.GetMainInstancePtr();

	if (!pkInstMain)
		return false;

	if (!pkInstMain->IsAttacking())
		return false;

	return true;
}

bool CPythonNetworkStream::RecvScriptPacket()
{
	TPacketGCScript ScriptPacket;

	if (!Recv(sizeof(TPacketGCScript), &ScriptPacket))
	{
		TraceError("RecvScriptPacket_RecvError");
		return false;
	}

	if (ScriptPacket.size < sizeof(TPacketGCScript))
	{
		TraceError("RecvScriptPacket_SizeError");
		return false;
	}

	ScriptPacket.size -= sizeof(TPacketGCScript);

	static std::string str;
	str = "";
	str.resize(ScriptPacket.size + 1);

	if (!Recv(ScriptPacket.size, &str[0]))
		return false;

	str[str.size() - 1] = '\0';
#if defined(ENABLE_LOCALE_CLIENT)
	CPythonLocale::Instance().FormatString(str);
#endif

	int iIndex = CPythonEventManager::Instance().RegisterEventSetFromString(str);

	if (-1 != iIndex)
	{
		CPythonEventManager::Instance().SetVisibleLineCount(iIndex, CPythonEventManager::QUEST_VISIBLE_LINE_COUNT);
		CPythonNetworkStream::Instance().OnScriptEventStart(ScriptPacket.skin, iIndex);
	}

	return true;
}

bool CPythonNetworkStream::SendScriptAnswerPacket(int iAnswer)
{
	TPacketCGScriptAnswer ScriptAnswer;

	ScriptAnswer.header = HEADER_CG_SCRIPT_ANSWER;
	ScriptAnswer.answer = (BYTE)iAnswer;
	if (!Send(sizeof(TPacketCGScriptAnswer), &ScriptAnswer))
	{
		Tracen("Send Script Answer Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendScriptButtonPacket(unsigned int iIndex)
{
	TPacketCGScriptButton ScriptButton;

	ScriptButton.header = HEADER_CG_SCRIPT_BUTTON;
	ScriptButton.idx = iIndex;
	if (!Send(sizeof(TPacketCGScriptButton), &ScriptButton))
	{
		Tracen("Send Script Button Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendAnswerMakeGuildPacket(const char* c_szName)
{
	TPacketCGAnswerMakeGuild Packet;

	Packet.header = HEADER_CG_ANSWER_MAKE_GUILD;
	strncpy(Packet.guild_name, c_szName, GUILD_NAME_MAX_LEN);
	Packet.guild_name[GUILD_NAME_MAX_LEN] = '\0';

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracen("SendAnswerMakeGuild Packet Error");
		return false;
	}

	//Tracef("SendAnswerMakeGuildPacket : %s", c_szName);
	return SendSequence();
}

bool CPythonNetworkStream::SendQuestInputStringPacket(const char* c_szString)
{
	TPacketCGQuestInputString Packet;
	Packet.bHeader = HEADER_CG_QUEST_INPUT_STRING;
	strncpy(Packet.szString, c_szString, QUEST_INPUT_STRING_MAX_NUM);

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracen("SendQuestInputStringPacket Error");
		return false;
	}

	return SendSequence();
}

#if defined(ENABLE_OX_RENEWAL)
bool CPythonNetworkStream::SendQuestInputStringLongPacket(const char* c_szString)
{
	TPacketCGQuestInputLongString Packet;
	Packet.bHeader = HEADER_CG_QUEST_INPUT_LONG_STRING;
	strncpy(Packet.szString, c_szString, QUEST_INPUT_LONG_STRING_MAX_NUM);

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracen("SendQuestInputLongStringPacket Error");
		return false;
	}

	return SendSequence();
}
#endif

bool CPythonNetworkStream::SendQuestConfirmPacket(BYTE byAnswer, DWORD dwPID)
{
	TPacketCGQuestConfirm kPacket;
	kPacket.header = HEADER_CG_QUEST_CONFIRM;
	kPacket.answer = byAnswer;
	kPacket.requestPID = dwPID;

	if (!Send(sizeof(kPacket), &kPacket))
	{
		Tracen("SendQuestConfirmPacket Error");
		return false;
	}

	Tracenf("SendQuestConfirmPacket : %d, %d", byAnswer, dwPID);
	return SendSequence();
}

bool CPythonNetworkStream::RecvSkillLevel()
{
	assert(!"CPythonNetworkStream::RecvSkillLevel - 사용하지 않는 함수");
	TPacketGCSkillLevel packet;
	if (!Recv(sizeof(TPacketGCSkillLevel), &packet))
	{
		Tracen("CPythonNetworkStream::RecvSkillLevel - RecvError");
		return false;
	}

	DWORD dwSlotIndex;

	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
	for (int i = 0; i < SKILL_MAX_NUM; ++i)
	{
		if (rkPlayer.GetSkillSlotIndex(i, &dwSlotIndex))
			rkPlayer.SetSkillLevel(dwSlotIndex, packet.abSkillLevels[i]);
	}

	__RefreshSkillWindow();
	__RefreshStatus();
	Tracef(" >> RecvSkillLevel\n");
	return true;
}

bool CPythonNetworkStream::RecvSkillLevelNew()
{
	TPacketGCSkillLevelNew packet;

	if (!Recv(sizeof(TPacketGCSkillLevelNew), &packet))
	{
		Tracen("CPythonNetworkStream::RecvSkillLevelNew - RecvError");
		return false;
	}

	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();

	rkPlayer.SetSkill(7, 0);
	rkPlayer.SetSkill(8, 0);
#if defined(ENABLE_CONQUEROR_LEVEL)
	if (!rkPlayer.GetStatus(POINT_CONQUEROR_LEVEL))
		rkPlayer.SetSkill(9, 0);
#endif

	for (int i = 0; i < SKILL_MAX_NUM; ++i)
	{
		const TPlayerSkill& rPlayerSkill = packet.skills[i];

		if (i >= 112 && i <= 115 && rPlayerSkill.bLevel)
			rkPlayer.SetSkill(7, i);

		if (i >= 116 && i <= 119 && rPlayerSkill.bLevel)
			rkPlayer.SetSkill(8, i);

#if defined(ENABLE_678TH_SKILL)
		if (i >= 221 && i <= 229 && rPlayerSkill.bLevel)
			rkPlayer.SetSkill(7, i);

		if (i >= 236 && i <= 244 && rPlayerSkill.bLevel)
			rkPlayer.SetSkill(8, i);
#endif

		rkPlayer.SetSkillLevel_(i, rPlayerSkill.bMasterType, rPlayerSkill.bLevel);
	}

	__RefreshSkillWindow();
	__RefreshStatus();
	//Tracef(" >> RecvSkillLevelNew\n");
	return true;
}

bool CPythonNetworkStream::RecvDamageInfoPacket()
{
	TPacketGCDamageInfo DamageInfoPacket;

	if (!Recv(sizeof(TPacketGCDamageInfo), &DamageInfoPacket))
	{
		Tracen("Recv Target Packet Error");
		return false;
	}

	CInstanceBase* pInstTarget = CPythonCharacterManager::Instance().GetInstancePtr(DamageInfoPacket.dwVID);
	bool bSelf = (pInstTarget == CPythonCharacterManager::Instance().GetMainInstancePtr());
	bool bTarget = (pInstTarget == m_pInstTarget);
	if (pInstTarget)
	{
		if (DamageInfoPacket.damage >= 0)
			pInstTarget->AddDamageEffect(DamageInfoPacket.damage, DamageInfoPacket.flag, bSelf, bTarget);
		else
			TraceError("Damage is equal or below 0.");
	}

	return true;
}

bool CPythonNetworkStream::RecvTargetPacket()
{
	TPacketGCTarget TargetPacket;

	if (!Recv(sizeof(TPacketGCTarget), &TargetPacket))
	{
		Tracen("Recv Target Packet Error");
		return false;
	}

#if defined(ENABLE_DEFENSE_WAVE)
	if (TargetPacket.bAlliance)
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetHPAllianceTargetBoard",
			Py_BuildValue("(iii)", TargetPacket.dwVID, TargetPacket.iMinHP, TargetPacket.iMaxHP));
		return true;
	}
#endif

	CInstanceBase* pInstPlayer = CPythonCharacterManager::Instance().GetMainInstancePtr();
	CInstanceBase* pInstTarget = CPythonCharacterManager::Instance().GetInstancePtr(TargetPacket.dwVID);
	if (pInstPlayer && pInstTarget)
	{
		if (!pInstTarget->IsDead())
		{
#if defined(ENABLE_VIEW_TARGET_PLAYER_HP)
			if (pInstTarget->IsBuilding())
#else
			if (pInstTarget->IsPC() || pInstTarget->IsBuilding())
#endif
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "CloseTargetBoardIfDifferent", Py_BuildValue("(i)", TargetPacket.dwVID));
			}
			else if (pInstPlayer->CanViewTargetHP(*pInstTarget))
			{
#if defined(ENABLE_VIEW_TARGET_DECIMAL_HP)
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetHPTargetBoard", Py_BuildValue("(iiii)", TargetPacket.dwVID, TargetPacket.bHPPercent, TargetPacket.iMinHP, TargetPacket.iMaxHP));
#else
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetHPTargetBoard", Py_BuildValue("(ii)", TargetPacket.dwVID, TargetPacket.bHPPercent));
#endif

#if defined(ENABLE_ELEMENT_ADD)
				PyObject* poDict = PyDict_New();
				for (BYTE bElementIndex = 0; bElementIndex < CPythonNonPlayer::MOB_ELEMENT_MAX_NUM; ++bElementIndex)
				{
					PyDict_SetItem(poDict,
						Py_BuildValue("i", bElementIndex),
						Py_BuildValue("i", TargetPacket.bElement[bElementIndex]));
				}
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ShowTargetElementEnchant", Py_BuildValue("(O)", poDict));
				Py_DECREF(poDict);
#endif
			}
			else
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "CloseTargetBoard", Py_BuildValue("()"));
			}

			m_pInstTarget = pInstTarget;
		}
	}
	else
	{
		CPythonPlayer::Instance().SetTarget(0);
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "CloseTargetBoard", Py_BuildValue("()"));
	}

	return true;
}

#if defined(ENABLE_SEND_TARGET_INFO)
bool CPythonNetworkStream::RecvTargetInfoPacket()
{
	TPacketGCTargetInfo TargetInfoPacket;
	if (!Recv(sizeof(TPacketGCTargetInfo), &TargetInfoPacket))
	{
		Tracen("Recv TargetInfo Packet Error");
		return false;
	}

	CPythonNonPlayer::Instance().ClearItemDrop();

	CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(TargetInfoPacket.dwVID);
	if (pInstance && (pInstance->IsEnemy() || pInstance->IsStone() && !pInstance->IsDead()))
	{
		UINT uiPacketSize = (TargetInfoPacket.wSize - sizeof(TPacketGCTargetInfo));
		for (; uiPacketSize > 0; uiPacketSize -= sizeof(TPacketGCTargetDropInfo))
		{
			TPacketGCTargetDropInfo TargetDropInfoPacket;
			if (!Recv(sizeof(TargetDropInfoPacket), &TargetDropInfoPacket))
				return false;

			CPythonNonPlayer::Instance().AddItemDrop(
				TargetInfoPacket.dwRaceVnum,
				TargetDropInfoPacket.dwVnum,
				TargetDropInfoPacket.bCount
			);
		}

		CPythonNonPlayer::Instance().SetDropMetinStone(TargetInfoPacket.bDropMetinStone);
	}
	else
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "CloseTargetBoard", Py_BuildValue("()"));

	return true;
}
#endif

bool CPythonNetworkStream::RecvMountPacket()
{
	TPacketGCMount MountPacket;

	if (!Recv(sizeof(TPacketGCMount), &MountPacket))
	{
		Tracen("Recv Mount Packet Error");
		return false;
	}

	CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(MountPacket.vid);

	if (pInstance)
	{
		// Mount
		if (0 != MountPacket.mount_vid)
		{
			//pInstance->Ride(MountPacket.pos, MountPacket.mount_vid);
		}
		// Unmount
		else
		{
			//pInstance->Unride(MountPacket.pos, MountPacket.x, MountPacket.y);
		}
	}

	if (CPythonPlayer::Instance().IsMainCharacterIndex(MountPacket.vid))
	{
		//CPythonPlayer::Instance().SetRidingVehicleIndex(MountPacket.mount_vid);
	}

	return true;
}

bool CPythonNetworkStream::RecvChangeSpeedPacket()
{
	TPacketGCChangeSpeed SpeedPacket;

	if (!Recv(sizeof(TPacketGCChangeSpeed), &SpeedPacket))
	{
		Tracen("Recv Speed Packet Error");
		return false;
	}

	CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(SpeedPacket.vid);

	if (!pInstance)
		return true;

	//pInstance->SetWalkSpeed(SpeedPacket.walking_speed);
	//pInstance->SetRunSpeed(SpeedPacket.running_speed);
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Recv
bool CPythonNetworkStream::SendAttackPacket(UINT uMotAttack, DWORD dwVIDVictim)
{
	if (!__CanActMainInstance())
		return true;

#ifdef ATTACK_TIME_LOG
	static DWORD prevTime = timeGetTime();
	DWORD curTime = timeGetTime();
	TraceError("TIME: %.4f(%.4f) ATTACK_PACKET: %d TARGET: %d", curTime / 1000.0f, (curTime - prevTime) / 1000.0f, uMotAttack, dwVIDVictim);
	prevTime = curTime;
#endif

	TPacketCGAttack kPacketAtk;
	kPacketAtk.header = HEADER_CG_ATTACK;
	kPacketAtk.bType = uMotAttack;
	kPacketAtk.dwVictimVID = dwVIDVictim;

	if (!SendSpecial(sizeof(kPacketAtk), &kPacketAtk))
	{
		Tracen("Send Battle Attack Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendSpecial(int nLen, void* pvBuf)
{
	BYTE bHeader = *(BYTE*)pvBuf;

	switch (bHeader)
	{
		case HEADER_CG_ATTACK:
		{
			TPacketCGAttack* pkPacketAtk = (TPacketCGAttack*)pvBuf;
			pkPacketAtk->bCRCMagicCubeProcPiece = GetProcessCRCMagicCubePiece();
			pkPacketAtk->bCRCMagicCubeFilePiece = GetProcessCRCMagicCubePiece();
			return Send(nLen, pvBuf);
		}
		break;
	}

	return Send(nLen, pvBuf);
}

bool CPythonNetworkStream::RecvAddFlyTargetingPacket()
{
	TPacketGCFlyTargeting kPacket;
	if (!Recv(sizeof(kPacket), &kPacket))
		return false;

	__GlobalPositionToLocalPosition(kPacket.lX, kPacket.lY);

	Tracef("VID [%d]가 타겟을 추가 설정\n", kPacket.dwShooterVID);

	CPythonCharacterManager& rpcm = CPythonCharacterManager::Instance();

	CInstanceBase* pShooter = rpcm.GetInstancePtr(kPacket.dwShooterVID);

	if (!pShooter)
	{
#ifndef _DEBUG
		TraceError("CPythonNetworkStream::RecvFlyTargetingPacket() - dwShooterVID[%d] NOT EXIST", kPacket.dwShooterVID);
#endif
		return true;
	}

	CInstanceBase* pTarget = rpcm.GetInstancePtr(kPacket.dwTargetVID);

	if (kPacket.dwTargetVID && pTarget)
	{
		pShooter->GetGraphicThingInstancePtr()->AddFlyTarget(pTarget->GetGraphicThingInstancePtr());
	}
	else
	{
		float h = CPythonBackground::Instance().GetHeight(kPacket.lX, kPacket.lY) + 60.0f; // TEMPORARY HEIGHT
		pShooter->GetGraphicThingInstancePtr()->AddFlyTarget(D3DXVECTOR3(kPacket.lX, kPacket.lY, h));
		//pShooter->GetGraphicThingInstancePtr()->SetFlyTarget(kPacket.kPPosTarget.x,kPacket.kPPosTarget.y,);
	}

	return true;
}

bool CPythonNetworkStream::RecvFlyTargetingPacket()
{
	TPacketGCFlyTargeting kPacket;
	if (!Recv(sizeof(kPacket), &kPacket))
		return false;

	__GlobalPositionToLocalPosition(kPacket.lX, kPacket.lY);

	//Tracef("CPythonNetworkStream::RecvFlyTargetingPacket - VID [%d]\n",kPacket.dwShooterVID);

	CPythonCharacterManager& rpcm = CPythonCharacterManager::Instance();

	CInstanceBase* pShooter = rpcm.GetInstancePtr(kPacket.dwShooterVID);

	if (!pShooter)
	{
#ifdef _DEBUG
		TraceError("CPythonNetworkStream::RecvFlyTargetingPacket() - dwShooterVID[%d] NOT EXIST", kPacket.dwShooterVID);
#endif
		return true;
	}

	CInstanceBase* pTarget = rpcm.GetInstancePtr(kPacket.dwTargetVID);

	if (kPacket.dwTargetVID && pTarget)
	{
		pShooter->GetGraphicThingInstancePtr()->SetFlyTarget(pTarget->GetGraphicThingInstancePtr());
	}
	else
	{
		float h = CPythonBackground::Instance().GetHeight(kPacket.lX, kPacket.lY) + 60.0f; // TEMPORARY HEIGHT
		pShooter->GetGraphicThingInstancePtr()->SetFlyTarget(D3DXVECTOR3(kPacket.lX, kPacket.lY, h));
		//pShooter->GetGraphicThingInstancePtr()->SetFlyTarget(kPacket.kPPosTarget.x,kPacket.kPPosTarget.y,);
	}

	return true;
}

bool CPythonNetworkStream::SendShootPacket(UINT uSkill)
{
	TPacketCGShoot kPacketShoot;
	kPacketShoot.bHeader = HEADER_CG_SHOOT;
	kPacketShoot.bType = uSkill;

	if (!Send(sizeof(kPacketShoot), &kPacketShoot))
	{
		Tracen("SendShootPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendAddFlyTargetingPacket(DWORD dwTargetVID, const TPixelPosition& kPPosTarget)
{
	TPacketCGFlyTargeting packet;

	//CPythonCharacterManager & rpcm = CPythonCharacterManager::Instance();

	packet.bHeader = HEADER_CG_ADD_FLY_TARGETING;
	packet.dwTargetVID = dwTargetVID;
	packet.lX = kPPosTarget.x;
	packet.lY = kPPosTarget.y;

	__LocalPositionToGlobalPosition(packet.lX, packet.lY);

	if (!Send(sizeof(packet), &packet))
	{
		Tracen("Send FlyTargeting Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendFlyTargetingPacket(DWORD dwTargetVID, const TPixelPosition& kPPosTarget)
{
	TPacketCGFlyTargeting packet;

	//CPythonCharacterManager & rpcm = CPythonCharacterManager::Instance();

	packet.bHeader = HEADER_CG_FLY_TARGETING;
	packet.dwTargetVID = dwTargetVID;
	packet.lX = kPPosTarget.x;
	packet.lY = kPPosTarget.y;

	__LocalPositionToGlobalPosition(packet.lX, packet.lY);

	if (!Send(sizeof(packet), &packet))
	{
		Tracen("Send FlyTargeting Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvCreateFlyPacket()
{
	TPacketGCCreateFly kPacket;
	if (!Recv(sizeof(TPacketGCCreateFly), &kPacket))
		return false;

	CFlyingManager& rkFlyMgr = CFlyingManager::Instance();
	CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();

	CInstanceBase* pkStartInst = rkChrMgr.GetInstancePtr(kPacket.dwStartVID);
	CInstanceBase* pkEndInst = rkChrMgr.GetInstancePtr(kPacket.dwEndVID);
	if (!pkStartInst || !pkEndInst)
		return true;

	rkFlyMgr.CreateIndexedFly(kPacket.bType, pkStartInst->GetGraphicThingInstancePtr(), pkEndInst->GetGraphicThingInstancePtr());

	return true;
}

bool CPythonNetworkStream::SendTargetPacket(DWORD dwVID)
{
	TPacketCGTarget packet;
	packet.header = HEADER_CG_TARGET;
	packet.dwVID = dwVID;

	if (!Send(sizeof(packet), &packet))
	{
		Tracen("Send Target Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendSyncPositionElementPacket(DWORD dwVictimVID, DWORD dwVictimX, DWORD dwVictimY)
{
	TPacketCGSyncPositionElement kSyncPos;
	kSyncPos.dwVID = dwVictimVID;
	kSyncPos.lX = dwVictimX;
	kSyncPos.lY = dwVictimY;

	__LocalPositionToGlobalPosition(kSyncPos.lX, kSyncPos.lY);

	if (!Send(sizeof(kSyncPos), &kSyncPos))
	{
		Tracen("CPythonNetworkStream::SendSyncPositionElementPacket - ERROR");
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// Messenger

bool CPythonNetworkStream::SendMessengerAddByVIDPacket(const DWORD c_dwVID)
{
	TPacketCGMessenger Packet;
	Packet.bHeader = HEADER_CG_MESSENGER;
	Packet.bSubHeader = MESSENGER_SUBHEADER_CG_ADD_BY_VID;
	if (!Send(sizeof(Packet), &Packet))
		return false;

	if (!Send(sizeof(c_dwVID), &c_dwVID))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendMessengerAddByNamePacket(const char* c_szName)
{
	TPacketCGMessenger Packet;
	Packet.bHeader = HEADER_CG_MESSENGER;
	Packet.bSubHeader = MESSENGER_SUBHEADER_CG_ADD_BY_NAME;
	if (!Send(sizeof(Packet), &Packet))
		return false;

	char szName[CHARACTER_NAME_MAX_LEN] = {};
	strncpy(szName, c_szName, CHARACTER_NAME_MAX_LEN - 1);
	szName[CHARACTER_NAME_MAX_LEN - 1] = '\0'; // #720: 메신저 이름 관련 버퍼 오버플로우 버그 수정

	if (!Send(sizeof(szName), &szName))
		return false;

	Tracef(" SendMessengerAddByNamePacket : %s\n", c_szName);
	return SendSequence();
}

bool CPythonNetworkStream::SendMessengerRemovePacket(const char* c_szKey, const char* c_szName)
{
	TPacketCGMessenger Packet;
	Packet.bHeader = HEADER_CG_MESSENGER;
	Packet.bSubHeader = MESSENGER_SUBHEADER_CG_REMOVE;
	if (!Send(sizeof(Packet), &Packet))
		return false;

	char szKey[CHARACTER_NAME_MAX_LEN] = {};
	strncpy(szKey, c_szKey, CHARACTER_NAME_MAX_LEN - 1);
	if (!Send(sizeof(szKey), &szKey))
		return false;

	__RefreshTargetBoardByName(c_szName);
	return SendSequence();
}

#if defined(ENABLE_MESSENGER_BLOCK)
bool CPythonNetworkStream::SendMessengerBlockAddByVIDPacket(const DWORD c_dwVID)
{
	TPacketCGMessenger Packet;
	Packet.bHeader = HEADER_CG_MESSENGER;
	Packet.bSubHeader = MESSENGER_SUBHEADER_CG_BLOCK_ADD_BY_VID;
	if (!Send(sizeof(Packet), &Packet))
		return false;

	if (!Send(sizeof(c_dwVID), &c_dwVID))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendMessengerBlockAddByNamePacket(const char* c_szName)
{
	TPacketCGMessenger Packet;
	Packet.bHeader = HEADER_CG_MESSENGER;
	Packet.bSubHeader = MESSENGER_SUBHEADER_CG_BLOCK_ADD_BY_NAME;
	if (!Send(sizeof(Packet), &Packet))
		return false;

	char szName[CHARACTER_NAME_MAX_LEN] = {};
	strncpy(szName, c_szName, CHARACTER_NAME_MAX_LEN - 1);
	szName[CHARACTER_NAME_MAX_LEN - 1] = '\0';

	if (!Send(sizeof(szName), &szName))
		return false;

	Tracef(" SendMessengerBlockAddByNamePacket : %s\n", c_szName);
	return SendSequence();
}

bool CPythonNetworkStream::SendMessengerBlockRemovePacket(const char* c_szKey, const char* c_szName)
{
	TPacketCGMessenger Packet;
	Packet.bHeader = HEADER_CG_MESSENGER;
	Packet.bSubHeader = MESSENGER_SUBHEADER_CG_BLOCK_REMOVE;
	if (!Send(sizeof(Packet), &Packet))
		return false;

	char szKey[CHARACTER_NAME_MAX_LEN];
	strncpy(szKey, c_szKey, CHARACTER_NAME_MAX_LEN - 1);
	if (!Send(sizeof(szKey), &szKey))
		return false;

	CPythonMessenger::Instance().RemoveBlock(szKey);
	__RefreshTargetBoardByName(c_szName);
	return SendSequence();
}

bool CPythonNetworkStream::SendMessengerBlockRemoveByVIDPacket(DWORD c_dwVID)
{
	TPacketCGMessenger Packet;
	Packet.bHeader = HEADER_CG_MESSENGER;
	Packet.bSubHeader = MESSENGER_SUBHEADER_CG_BLOCK_REMOVE;
	if (!Send(sizeof(Packet), &Packet))
		return false;

	CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(c_dwVID);

	char szKey[CHARACTER_NAME_MAX_LEN];
	strncpy(szKey, pInstance->GetNameString(), CHARACTER_NAME_MAX_LEN - 1);
	if (!Send(sizeof(szKey), &szKey))
		return false;

	CPythonMessenger::Instance().RemoveBlock(szKey);
	__RefreshTargetBoardByName(pInstance->GetNameString());
	return SendSequence();
}
#endif

bool CPythonNetworkStream::RecvMessenger()
{
	TPacketGCMessenger Packet;
	if (!Recv(sizeof(Packet), &Packet))
		return false;

	UINT uiPacketSize = (Packet.wSize - sizeof(TPacketGCMessenger));
	for (; uiPacketSize > 0; uiPacketSize -= sizeof(TPacketGCMessengerList))
	{
		TPacketGCMessengerList ListPacket;
		if (!Recv(sizeof(ListPacket), &ListPacket))
			return false;

		switch (Packet.bSubHeader)
		{
			case MESSENGER_SUBHEADER_GC_LIST:
			{
				if (ListPacket.bConnected & MESSENGER_CONNECTED_STATE_ONLINE)
				{
#if defined(ENABLE_MESSENGER_DETAILS)
					CPythonMessenger::Instance().OnFriendLogin(ListPacket.szName
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
						, ListPacket.szCountry
#endif
					);
#else
					CPythonMessenger::Instance().OnFriendLogin(ListPacket.szName);
#endif
				}
				else
				{
#if defined(ENABLE_MESSENGER_DETAILS)
					CPythonMessenger::Instance().OnFriendLogout(ListPacket.szName, ListPacket.dwLastPlayTime
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
						, ListPacket.szCountry
#endif
					);
#else
					CPythonMessenger::Instance().OnFriendLogout(ListPacket.szName);
#endif
				}
			}
			break;

			case MESSENGER_SUBHEADER_GC_LOGIN:
			{
#if defined(ENABLE_MESSENGER_DETAILS)
				CPythonMessenger::Instance().OnFriendLogin(ListPacket.szName
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
					, ListPacket.szCountry
#endif
				);
#else
				CPythonMessenger::Instance().OnFriendLogin(ListPacket.szName);
#endif
				__RefreshTargetBoardByName(ListPacket.szName);
			}
			break;

			case MESSENGER_SUBHEADER_GC_LOGOUT:
#if defined(ENABLE_MESSENGER_DETAILS)
				CPythonMessenger::Instance().OnFriendLogout(ListPacket.szName, ListPacket.dwLastPlayTime
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
					, ListPacket.szCountry
#endif
				);
#else
				CPythonMessenger::Instance().OnFriendLogout(ListPacket.szName);
#endif
				break;

#if defined(ENABLE_MESSENGER_GM)
			case MESSENGER_SUBHEADER_GC_GM_LIST:
			{
				if (ListPacket.bConnected & MESSENGER_CONNECTED_STATE_ONLINE)
					CPythonMessenger::Instance().OnGMLogin(ListPacket.szName
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
						, ListPacket.szCountry
#endif
					);
				else
					CPythonMessenger::Instance().OnGMLogout(ListPacket.szName, ListPacket.dwLastPlayTime
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
						, ListPacket.szCountry
#endif
					);
			}
			break;

			case MESSENGER_SUBHEADER_GC_GM_LOGIN:
			{
				CPythonMessenger::Instance().OnGMLogin(ListPacket.szName
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
					, ListPacket.szCountry
#endif
				);
				__RefreshTargetBoardByName(ListPacket.szName);
			}
			break;

			case MESSENGER_SUBHEADER_GC_GM_LOGOUT:
				CPythonMessenger::Instance().OnGMLogout(ListPacket.szName, ListPacket.dwLastPlayTime
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
					, ListPacket.szCountry
#endif
				);
				break;
#endif

#if defined(ENABLE_MESSENGER_BLOCK)
			case MESSENGER_SUBHEADER_GC_BLOCK_LIST:
			{
				if (ListPacket.bConnected & MESSENGER_CONNECTED_STATE_ONLINE)
					CPythonMessenger::Instance().OnBlockLogin(ListPacket.szName);
				else
					CPythonMessenger::Instance().OnBlockLogout(ListPacket.szName);
			}
			break;

			case MESSENGER_SUBHEADER_GC_BLOCK_LOGIN:
			{
				CPythonMessenger::Instance().OnBlockLogin(ListPacket.szName);
				__RefreshTargetBoardByName(ListPacket.szName);
			}
			break;

			case MESSENGER_SUBHEADER_GC_BLOCK_LOGOUT:
				CPythonMessenger::Instance().OnBlockLogout(ListPacket.szName);
				break;
#endif
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// Party

bool CPythonNetworkStream::SendPartyInvitePacket(DWORD dwVID)
{
	TPacketCGPartyInvite kPartyInvitePacket;
	kPartyInvitePacket.header = HEADER_CG_PARTY_INVITE;
	kPartyInvitePacket.vid = dwVID;

	if (!Send(sizeof(kPartyInvitePacket), &kPartyInvitePacket))
	{
		Tracenf("CPythonNetworkStream::SendPartyInvitePacket [%ud] - PACKET SEND ERROR", dwVID);
		return false;
	}

	Tracef(" << SendPartyInvitePacket : %d\n", dwVID);
	return SendSequence();
}

bool CPythonNetworkStream::SendPartyInviteAnswerPacket(DWORD dwLeaderVID, BYTE byAnswer)
{
	TPacketCGPartyInviteAnswer kPartyInviteAnswerPacket;
	kPartyInviteAnswerPacket.header = HEADER_CG_PARTY_INVITE_ANSWER;
	kPartyInviteAnswerPacket.leader_pid = dwLeaderVID;
	kPartyInviteAnswerPacket.accept = byAnswer;

	if (!Send(sizeof(kPartyInviteAnswerPacket), &kPartyInviteAnswerPacket))
	{
		Tracenf("CPythonNetworkStream::SendPartyInviteAnswerPacket [%ud %ud] - PACKET SEND ERROR", dwLeaderVID, byAnswer);
		return false;
	}

	Tracef(" << SendPartyInviteAnswerPacket : %d, %d\n", dwLeaderVID, byAnswer);
	return SendSequence();
}

bool CPythonNetworkStream::SendPartyRemovePacket(DWORD dwPID)
{
	TPacketCGPartyRemove kPartyInviteRemove;
	kPartyInviteRemove.header = HEADER_CG_PARTY_REMOVE;
	kPartyInviteRemove.pid = dwPID;

	if (!Send(sizeof(kPartyInviteRemove), &kPartyInviteRemove))
	{
		Tracenf("CPythonNetworkStream::SendPartyRemovePacket [%ud] - PACKET SEND ERROR", dwPID);
		return false;
	}

	Tracef(" << SendPartyRemovePacket : %d\n", dwPID);
	return SendSequence();
}

bool CPythonNetworkStream::SendPartySetStatePacket(DWORD dwVID, BYTE byState, BYTE byFlag)
{
	TPacketCGPartySetState kPartySetState;
	kPartySetState.byHeader = HEADER_CG_PARTY_SET_STATE;
	kPartySetState.dwVID = dwVID;
	kPartySetState.byState = byState;
	kPartySetState.byFlag = byFlag;

	if (!Send(sizeof(kPartySetState), &kPartySetState))
	{
		Tracenf("CPythonNetworkStream::SendPartySetStatePacket(%ud, %ud) - PACKET SEND ERROR", dwVID, byState);
		return false;
	}

	Tracef(" << SendPartySetStatePacket : %d, %d, %d\n", dwVID, byState, byFlag);
	return SendSequence();
}

bool CPythonNetworkStream::SendPartyUseSkillPacket(BYTE bySkillIndex, DWORD dwVID)
{
	TPacketCGPartyUseSkill kPartyUseSkill;
	kPartyUseSkill.byHeader = HEADER_CG_PARTY_USE_SKILL;
	kPartyUseSkill.bySkillIndex = bySkillIndex;
	kPartyUseSkill.dwTargetVID = dwVID;

	if (!Send(sizeof(kPartyUseSkill), &kPartyUseSkill))
	{
		Tracenf("CPythonNetworkStream::SendPartyUseSkillPacket(%ud, %ud) - PACKET SEND ERROR", bySkillIndex, dwVID);
		return false;
	}

	Tracef(" << SendPartyUseSkillPacket : %d, %d\n", bySkillIndex, dwVID);
	return SendSequence();
}

bool CPythonNetworkStream::SendPartyParameterPacket(BYTE byDistributeMode)
{
	TPacketCGPartyParameter kPartyParameter;
	kPartyParameter.bHeader = HEADER_CG_PARTY_PARAMETER;
	kPartyParameter.bDistributeMode = byDistributeMode;

	if (!Send(sizeof(kPartyParameter), &kPartyParameter))
	{
		Tracenf("CPythonNetworkStream::SendPartyParameterPacket(%d) - PACKET SEND ERROR", byDistributeMode);
		return false;
	}

	Tracef(" << SendPartyParameterPacket : %d\n", byDistributeMode);
	return SendSequence();
}

bool CPythonNetworkStream::RecvPartyInvite()
{
	TPacketGCPartyInvite kPartyInvitePacket;
	if (!Recv(sizeof(kPartyInvitePacket), &kPartyInvitePacket))
		return false;

	CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(kPartyInvitePacket.leader_pid);
	if (!pInstance)
	{
		TraceError(" CPythonNetworkStream::RecvPartyInvite - Failed to find leader instance [%d]\n", kPartyInvitePacket.leader_pid);
		return true;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RecvPartyInviteQuestion", Py_BuildValue("(is)", kPartyInvitePacket.leader_pid, pInstance->GetNameString()));
	Tracef(" >> RecvPartyInvite : %d, %s\n", kPartyInvitePacket.leader_pid, pInstance->GetNameString());

	return true;
}

bool CPythonNetworkStream::RecvPartyAdd()
{
	TPacketGCPartyAdd kPartyAddPacket;
	if (!Recv(sizeof(kPartyAddPacket), &kPartyAddPacket))
		return false;

	CPythonPlayer::Instance().AppendPartyMember(kPartyAddPacket.pid, kPartyAddPacket.name);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AddPartyMember",
		Py_BuildValue("(is"
#if defined(WJ_SHOW_PARTY_ON_MINIMAP)
			"i"
#endif
#if defined(ENABLE_PARTY_CHANNEL_FIX)
			"i"
#endif
			")",
			kPartyAddPacket.pid, kPartyAddPacket.name
#if defined(WJ_SHOW_PARTY_ON_MINIMAP)
			, kPartyAddPacket.mapIdx
#endif
#if defined(ENABLE_PARTY_CHANNEL_FIX)
			, kPartyAddPacket.channel
#endif
		));

	Tracef(" >> RecvPartyAdd : %d, %s"
#if defined(WJ_SHOW_PARTY_ON_MINIMAP)
		", %d"
#endif
#if defined(ENABLE_PARTY_CHANNEL_FIX)
		", %d"
#endif
		"\n", kPartyAddPacket.pid, kPartyAddPacket.name
#if defined(WJ_SHOW_PARTY_ON_MINIMAP)
		, kPartyAddPacket.mapIdx
#endif
#if defined(ENABLE_PARTY_CHANNEL_FIX)
		, kPartyAddPacket.channel
#endif
	);

	return true;
}

bool CPythonNetworkStream::RecvPartyUpdate()
{
	TPacketGCPartyUpdate kPartyUpdatePacket;
	if (!Recv(sizeof(kPartyUpdatePacket), &kPartyUpdatePacket))
		return false;

	CPythonPlayer::TPartyMemberInfo* pPartyMemberInfo;
	if (!CPythonPlayer::Instance().GetPartyMemberPtr(kPartyUpdatePacket.pid, &pPartyMemberInfo))
		return true;

	BYTE byOldState = pPartyMemberInfo->byState;

	CPythonPlayer::Instance().UpdatePartyMemberInfo(kPartyUpdatePacket.pid, kPartyUpdatePacket.state, kPartyUpdatePacket.percent_hp);
	for (int i = 0; i < PARTY_AFFECT_SLOT_MAX_NUM; ++i)
	{
		CPythonPlayer::Instance().UpdatePartyMemberAffect(kPartyUpdatePacket.pid, i, kPartyUpdatePacket.affects[i]);
	}

#if defined(WJ_SHOW_PARTY_ON_MINIMAP)
	if (kPartyUpdatePacket.x || kPartyUpdatePacket.y)
		__GlobalPositionToLocalPosition(kPartyUpdatePacket.x, kPartyUpdatePacket.y);

	CPythonPlayer::Instance().UpdatePartyMemberPosition(kPartyUpdatePacket.pid, kPartyUpdatePacket.x, kPartyUpdatePacket.y);
#endif

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "UpdatePartyMemberInfo", Py_BuildValue("(i)", kPartyUpdatePacket.pid));

	// 만약 리더가 바뀌었다면, TargetBoard 의 버튼을 업데이트 한다.
	DWORD dwVID;
	if (CPythonPlayer::Instance().PartyMemberPIDToVID(kPartyUpdatePacket.pid, &dwVID))
		if (byOldState != kPartyUpdatePacket.state)
		{
			__RefreshTargetBoardByVID(dwVID);
		}

	//Tracef(" >> RecvPartyUpdate : %d, %d, %d\n", kPartyUpdatePacket.pid, kPartyUpdatePacket.state, kPartyUpdatePacket.percent_hp);

	return true;
}

bool CPythonNetworkStream::RecvPartyRemove()
{
	TPacketGCPartyRemove kPartyRemovePacket;
	if (!Recv(sizeof(kPartyRemovePacket), &kPartyRemovePacket))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RemovePartyMember", Py_BuildValue("(i)", kPartyRemovePacket.pid));
	Tracef(" >> RecvPartyRemove : %d\n", kPartyRemovePacket.pid);

	return true;
}

bool CPythonNetworkStream::RecvPartyLink()
{
	TPacketGCPartyLink kPartyLinkPacket;
	if (!Recv(sizeof(kPartyLinkPacket), &kPartyLinkPacket))
		return false;

	CPythonPlayer::Instance().LinkPartyMember(kPartyLinkPacket.pid, kPartyLinkPacket.vid);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "LinkPartyMember",
		Py_BuildValue("(ii"
#if defined(WJ_SHOW_PARTY_ON_MINIMAP)
			"i"
#endif
#if defined(ENABLE_PARTY_CHANNEL_FIX)
			"i"
#endif
			")",
			kPartyLinkPacket.pid, kPartyLinkPacket.vid
#if defined(WJ_SHOW_PARTY_ON_MINIMAP)
			, kPartyLinkPacket.mapIdx
#endif
#if defined(ENABLE_PARTY_CHANNEL_FIX)
			, kPartyLinkPacket.channel
#endif
		));

	Tracef(" >> RecvPartyLink : %d, %d"
#if defined(WJ_SHOW_PARTY_ON_MINIMAP)
		", %d"
#endif
#if defined(ENABLE_PARTY_CHANNEL_FIX)
		", %d"
#endif
		"\n", kPartyLinkPacket.pid, kPartyLinkPacket.vid
#if defined(WJ_SHOW_PARTY_ON_MINIMAP)
		, kPartyLinkPacket.mapIdx
#endif
#if defined(ENABLE_PARTY_CHANNEL_FIX)
		, kPartyLinkPacket.channel
#endif
	);

	return true;
}

bool CPythonNetworkStream::RecvPartyUnlink()
{
	TPacketGCPartyLink kPartyUnlinkPacket;
	if (!Recv(sizeof(kPartyUnlinkPacket), &kPartyUnlinkPacket))
		return false;

	CPythonPlayer::Instance().UnlinkPartyMember(kPartyUnlinkPacket.pid);

	if (CPythonPlayer::Instance().IsMainCharacterIndex(kPartyUnlinkPacket.vid))
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "UnlinkAllPartyMember", Py_BuildValue("()"));
	}
	else
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "UnlinkPartyMember", Py_BuildValue("(i)", kPartyUnlinkPacket.pid));
	}

	Tracef(" >> RecvPartyUnlink : %d, %d\n", kPartyUnlinkPacket.pid, kPartyUnlinkPacket.vid);

	return true;
}

bool CPythonNetworkStream::RecvPartyParameter()
{
	TPacketGCPartyParameter kPartyParameterPacket;
	if (!Recv(sizeof(kPartyParameterPacket), &kPartyParameterPacket))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ChangePartyParameter", Py_BuildValue("(i)", kPartyParameterPacket.bDistributeMode));
	Tracef(" >> RecvPartyParameter : %d\n", kPartyParameterPacket.bDistributeMode);

	return true;
}
// Party
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Guild
bool CPythonNetworkStream::SendGuildAddMemberPacket(DWORD dwVID)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_ADD_MEMBER;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(dwVID), &dwVID))
		return false;

	Tracef(" SendGuildAddMemberPacket\n", dwVID);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildRemoveMemberPacket(DWORD dwPID)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_REMOVE_MEMBER;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(dwPID), &dwPID))
		return false;

	Tracef(" SendGuildRemoveMemberPacket %d\n", dwPID);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildChangeGradeNamePacket(BYTE byGradeNumber, const char* c_szName)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_CHANGE_GRADE_NAME;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(byGradeNumber), &byGradeNumber))
		return false;

	char szName[GUILD_GRADE_NAME_MAX_LEN + 1];
	strncpy(szName, c_szName, GUILD_GRADE_NAME_MAX_LEN);
	szName[GUILD_GRADE_NAME_MAX_LEN] = '\0';

	if (!Send(sizeof(szName), &szName))
		return false;

	Tracef(" SendGuildChangeGradeNamePacket %d, %s\n", byGradeNumber, c_szName);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildChangeGradeAuthorityPacket(BYTE byGradeNumber, BYTE byAuthority)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_CHANGE_GRADE_AUTHORITY;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(byGradeNumber), &byGradeNumber))
		return false;
	if (!Send(sizeof(byAuthority), &byAuthority))
		return false;

	Tracef(" SendGuildChangeGradeAuthorityPacket %d, %d\n", byGradeNumber, byAuthority);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildOfferPacket(DWORD dwExperience)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_OFFER;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(dwExperience), &dwExperience))
		return false;

	Tracef(" SendGuildOfferPacket %d\n", dwExperience);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildPostCommentPacket(const char* c_szMessage)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_POST_COMMENT;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	BYTE bySize = BYTE(strlen(c_szMessage)) + 1;
	if (!Send(sizeof(bySize), &bySize))
		return false;
	if (!Send(bySize, c_szMessage))
		return false;

	Tracef(" SendGuildPostCommentPacket %d, %s\n", bySize, c_szMessage);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildDeleteCommentPacket(DWORD dwIndex)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_DELETE_COMMENT;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	if (!Send(sizeof(dwIndex), &dwIndex))
		return false;

	Tracef(" SendGuildDeleteCommentPacket %d\n", dwIndex);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildRefreshCommentsPacket(DWORD dwHighestIndex)
{
	static DWORD s_LastTime = timeGetTime() - 1001;

	if (timeGetTime() - s_LastTime < 1000)
		return true;
	s_LastTime = timeGetTime();

	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_REFRESH_COMMENT;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	Tracef(" SendGuildRefreshCommentPacket %d\n", dwHighestIndex);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildChangeMemberGradePacket(DWORD dwPID, BYTE byGrade)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_CHANGE_MEMBER_GRADE;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	if (!Send(sizeof(dwPID), &dwPID))
		return false;
	if (!Send(sizeof(byGrade), &byGrade))
		return false;

	Tracef(" SendGuildChangeMemberGradePacket %d, %d\n", dwPID, byGrade);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildUseSkillPacket(DWORD dwSkillID, DWORD dwTargetVID)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_USE_SKILL;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	if (!Send(sizeof(dwSkillID), &dwSkillID))
		return false;
	if (!Send(sizeof(dwTargetVID), &dwTargetVID))
		return false;

	Tracef(" SendGuildUseSkillPacket %d, %d\n", dwSkillID, dwTargetVID);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildChangeMemberGeneralPacket(DWORD dwPID, BYTE byFlag)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_CHANGE_MEMBER_GENERAL;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	if (!Send(sizeof(dwPID), &dwPID))
		return false;
	if (!Send(sizeof(byFlag), &byFlag))
		return false;

	Tracef(" SendGuildChangeMemberGeneralFlagPacket %d, %d\n", dwPID, byFlag);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildInviteAnswerPacket(DWORD dwGuildID, BYTE byAnswer)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	if (!Send(sizeof(dwGuildID), &dwGuildID))
		return false;
	if (!Send(sizeof(byAnswer), &byAnswer))
		return false;

	Tracef(" SendGuildInviteAnswerPacket %d, %d\n", dwGuildID, byAnswer);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildChargeGSPPacket(DWORD dwMoney)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_CHARGE_GSP;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	if (!Send(sizeof(dwMoney), &dwMoney))
		return false;

	Tracef(" SendGuildChargeGSPPacket %d\n", dwMoney);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildDepositMoneyPacket(DWORD dwMoney)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_DEPOSIT_MONEY;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(dwMoney), &dwMoney))
		return false;

	Tracef(" SendGuildDepositMoneyPacket %d\n", dwMoney);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildWithdrawMoneyPacket(DWORD dwMoney)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_WITHDRAW_MONEY;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(dwMoney), &dwMoney))
		return false;

	Tracef(" SendGuildWithdrawMoneyPacket %d\n", dwMoney);
	return SendSequence();
}

bool CPythonNetworkStream::RecvGuild()
{
	TPacketGCGuild GuildPacket;
	if (!Recv(sizeof(GuildPacket), &GuildPacket))
		return false;

	switch (GuildPacket.subheader)
	{
		case GUILD_SUBHEADER_GC_LOGIN:
		{
			DWORD dwPID;
			if (!Recv(sizeof(DWORD), &dwPID))
				return false;

			// Messenger
			CPythonGuild::TGuildMemberData* pGuildMemberData;
			if (CPythonGuild::Instance().GetMemberDataPtrByPID(dwPID, &pGuildMemberData))
				if (0 != pGuildMemberData->strName.compare(CPythonPlayer::Instance().GetName()))
					CPythonMessenger::Instance().LoginGuildMember(pGuildMemberData->strName.c_str());

			//Tracef(" <Login> %d\n", dwPID);
			break;
		}
		case GUILD_SUBHEADER_GC_LOGOUT:
		{
			DWORD dwPID;
			if (!Recv(sizeof(DWORD), &dwPID))
				return false;

			// Messenger
			CPythonGuild::TGuildMemberData* pGuildMemberData;
			if (CPythonGuild::Instance().GetMemberDataPtrByPID(dwPID, &pGuildMemberData))
				if (0 != pGuildMemberData->strName.compare(CPythonPlayer::Instance().GetName()))
					CPythonMessenger::Instance().LogoutGuildMember(pGuildMemberData->strName.c_str());

			//Tracef(" <Logout> %d\n", dwPID);
			break;
		}
		case GUILD_SUBHEADER_GC_REMOVE:
		{
			DWORD dwPID;
			if (!Recv(sizeof(dwPID), &dwPID))
				return false;

			// Main Player 일 경우 DeleteGuild
			if (CPythonGuild::Instance().IsMainPlayer(dwPID))
			{
				CPythonGuild::Instance().Destroy();
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "DeleteGuild", Py_BuildValue("()"));
				CPythonMessenger::Instance().RemoveAllGuildMember();
				__SetGuildID(0);
				__RefreshMessengerWindow();
				__RefreshTargetBoard();
				__RefreshCharacterWindow();
			}
			else
			{
				// Get Member Name
				std::string strMemberName = "";
				CPythonGuild::TGuildMemberData* pData;
				if (CPythonGuild::Instance().GetMemberDataPtrByPID(dwPID, &pData))
				{
					strMemberName = pData->strName;
					CPythonMessenger::Instance().RemoveGuildMember(pData->strName.c_str());
				}

				CPythonGuild::Instance().RemoveMember(dwPID);

				// Refresh
				__RefreshTargetBoardByName(strMemberName.c_str());
				__RefreshGuildWindowMemberPage();
			}

			Tracef(" <Remove> %d\n", dwPID);
			break;
		}
		case GUILD_SUBHEADER_GC_LIST:
		{
			int iPacketSize = int(GuildPacket.size) - sizeof(GuildPacket);

			for (; iPacketSize > 0;)
			{
				TPacketGCGuildSubMember memberPacket;
				if (!Recv(sizeof(memberPacket), &memberPacket))
					return false;

				char szName[CHARACTER_NAME_MAX_LEN + 1] = "";
				if (memberPacket.byNameFlag)
				{
					if (!Recv(sizeof(szName), &szName))
						return false;

					iPacketSize -= CHARACTER_NAME_MAX_LEN + 1;
				}
				else
				{
					CPythonGuild::TGuildMemberData* pMemberData;
					if (CPythonGuild::Instance().GetMemberDataPtrByPID(memberPacket.pid, &pMemberData))
					{
						strncpy(szName, pMemberData->strName.c_str(), CHARACTER_NAME_MAX_LEN);
					}
				}

				//Tracef(" <List> %d : %s, %d (%d, %d, %d)\n", memberPacket.pid, szName, memberPacket.byGrade, memberPacket.byJob, memberPacket.byLevel, memberPacket.dwOffer);

				CPythonGuild::SGuildMemberData GuildMemberData;
				GuildMemberData.dwPID = memberPacket.pid;
				GuildMemberData.byGrade = memberPacket.byGrade;
				GuildMemberData.strName = szName;
				GuildMemberData.byJob = memberPacket.byJob;
				GuildMemberData.byLevel = memberPacket.byLevel;
				GuildMemberData.dwOffer = memberPacket.dwOffer;
				GuildMemberData.byGeneralFlag = memberPacket.byIsGeneral;
				CPythonGuild::Instance().RegisterMember(GuildMemberData);

				// Messenger
				if (strcmp(szName, CPythonPlayer::Instance().GetName()))
					CPythonMessenger::Instance().AppendGuildMember(szName);

				__RefreshTargetBoardByName(szName);

				iPacketSize -= sizeof(memberPacket);
			}

			__RefreshGuildWindowInfoPage();
			__RefreshGuildWindowMemberPage();
			__RefreshMessengerWindow();
			__RefreshCharacterWindow();
			break;
		}
		case GUILD_SUBHEADER_GC_GRADE:
		{
			BYTE byCount;
			if (!Recv(sizeof(byCount), &byCount))
				return false;

			for (BYTE i = 0; i < byCount; ++i)
			{
				BYTE byIndex;
				if (!Recv(sizeof(byCount), &byIndex))
					return false;

				TPacketGCGuildSubGrade GradePacket;
				if (!Recv(sizeof(GradePacket), &GradePacket))
					return false;

#if defined(ENABLE_LOCALE_CLIENT)
				CPythonLocale::Instance().FormatString(GradePacket.grade_name, sizeof(GradePacket.grade_name));
#endif

				auto aGradeData = CPythonGuild::SGuildGradeData(GradePacket.auth_flag, GradePacket.grade_name);
				CPythonGuild::Instance().SetGradeData(byIndex, aGradeData);
				//Tracef(" <Grade> [%d/%d] : %s, %d\n", byIndex, byCount, GradePacket.grade_name, GradePacket.auth_flag);
			}
			__RefreshGuildWindowGradePage();
			__RefreshGuildWindowMemberPageGradeComboBox();
			break;
		}
		case GUILD_SUBHEADER_GC_GRADE_NAME:
		{
			BYTE byGradeNumber;
			if (!Recv(sizeof(byGradeNumber), &byGradeNumber))
				return false;

			char szGradeName[GUILD_GRADE_NAME_MAX_LEN + 1] = "";
			if (!Recv(sizeof(szGradeName), &szGradeName))
				return false;

#if defined(ENABLE_LOCALE_CLIENT)
			CPythonLocale::Instance().FormatString(szGradeName, sizeof(szGradeName));
#endif

			CPythonGuild::Instance().SetGradeName(byGradeNumber, szGradeName);
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildGrade", Py_BuildValue("()"));

			Tracef(" <Change Grade Name> %d, %s\n", byGradeNumber, szGradeName);
			__RefreshGuildWindowGradePage();
			__RefreshGuildWindowMemberPageGradeComboBox();
			break;
		}
		case GUILD_SUBHEADER_GC_GRADE_AUTH:
		{
			BYTE byGradeNumber;
			if (!Recv(sizeof(byGradeNumber), &byGradeNumber))
				return false;
			BYTE byAuthorityFlag;
			if (!Recv(sizeof(byAuthorityFlag), &byAuthorityFlag))
				return false;

			CPythonGuild::Instance().SetGradeAuthority(byGradeNumber, byAuthorityFlag);
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildGrade", Py_BuildValue("()"));

			Tracef(" <Change Grade Authority> %d, %d\n", byGradeNumber, byAuthorityFlag);
			__RefreshGuildWindowGradePage();
			break;
		}
		case GUILD_SUBHEADER_GC_INFO:
		{
			TPacketGCGuildInfo GuildInfo;
			if (!Recv(sizeof(GuildInfo), &GuildInfo))
				return false;

			CPythonGuild::Instance().EnableGuild();
			CPythonGuild::TGuildInfo& rGuildInfo = CPythonGuild::Instance().GetGuildInfoRef();
			strncpy(rGuildInfo.szGuildName, GuildInfo.name, GUILD_NAME_MAX_LEN);
			rGuildInfo.szGuildName[GUILD_NAME_MAX_LEN] = '\0';

			rGuildInfo.dwGuildID = GuildInfo.guild_id;
			rGuildInfo.dwMasterPID = GuildInfo.master_pid;
			rGuildInfo.dwGuildLevel = GuildInfo.level;
			rGuildInfo.dwCurrentExperience = GuildInfo.exp;
			rGuildInfo.dwCurrentMemberCount = GuildInfo.member_count;
			rGuildInfo.dwMaxMemberCount = GuildInfo.max_member_count;
			rGuildInfo.dwGuildMoney = GuildInfo.gold;
			rGuildInfo.bHasLand = GuildInfo.hasLand;

			//Tracef(" <Info> %s, %d, %d : %d\n", GuildInfo.name, GuildInfo.master_pid, GuildInfo.level, rGuildInfo.bHasLand);
			__RefreshGuildWindowInfoPage();
			break;
		}
		case GUILD_SUBHEADER_GC_COMMENTS:
		{
			BYTE byCount;
			if (!Recv(sizeof(byCount), &byCount))
				return false;

			CPythonGuild::Instance().ClearComment();
			//Tracef(" >>> Comments Count : %d\n", byCount);

			for (BYTE i = 0; i < byCount; ++i)
			{
				DWORD dwCommentID;
				if (!Recv(sizeof(dwCommentID), &dwCommentID))
					return false;

				char szName[CHARACTER_NAME_MAX_LEN + 1] = "";
				if (!Recv(sizeof(szName), &szName))
					return false;

				char szComment[GULID_COMMENT_MAX_LEN + 1] = "";
				if (!Recv(sizeof(szComment), &szComment))
					return false;

				//Tracef(" [Comment-%d] : %s, %s\n", dwCommentID, szName, szComment);
				CPythonGuild::Instance().RegisterComment(dwCommentID, szName, szComment);
			}

			__RefreshGuildWindowBoardPage();
			break;
		}
		case GUILD_SUBHEADER_GC_CHANGE_EXP:
		{
			BYTE byLevel;
			if (!Recv(sizeof(byLevel), &byLevel))
				return false;
			DWORD dwEXP;
			if (!Recv(sizeof(dwEXP), &dwEXP))
				return false;
			CPythonGuild::Instance().SetGuildEXP(byLevel, dwEXP);
			Tracef(" <ChangeEXP> %d, %d\n", byLevel, dwEXP);
			__RefreshGuildWindowInfoPage();
			break;
		}
		case GUILD_SUBHEADER_GC_CHANGE_MEMBER_GRADE:
		{
			DWORD dwPID;
			if (!Recv(sizeof(dwPID), &dwPID))
				return false;
			BYTE byGrade;
			if (!Recv(sizeof(byGrade), &byGrade))
				return false;
			CPythonGuild::Instance().ChangeGuildMemberGrade(dwPID, byGrade);
			Tracef(" <ChangeMemberGrade> %d, %d\n", dwPID, byGrade);
			__RefreshGuildWindowMemberPage();
			break;
		}
		case GUILD_SUBHEADER_GC_SKILL_INFO:
		{
			CPythonGuild::TGuildSkillData& rSkillData = CPythonGuild::Instance().GetGuildSkillDataRef();
			if (!Recv(sizeof(rSkillData.bySkillPoint), &rSkillData.bySkillPoint))
				return false;
			if (!Recv(sizeof(rSkillData.bySkillLevel), rSkillData.bySkillLevel))
				return false;
			if (!Recv(sizeof(rSkillData.wGuildPoint), &rSkillData.wGuildPoint))
				return false;
			if (!Recv(sizeof(rSkillData.wMaxGuildPoint), &rSkillData.wMaxGuildPoint))
				return false;

			Tracef(" <SkillInfo> %d / %d, %d\n", rSkillData.bySkillPoint, rSkillData.wGuildPoint, rSkillData.wMaxGuildPoint);
			__RefreshGuildWindowSkillPage();
			break;
		}
		case GUILD_SUBHEADER_GC_CHANGE_MEMBER_GENERAL:
		{
			DWORD dwPID;
			if (!Recv(sizeof(dwPID), &dwPID))
				return false;
			BYTE byFlag;
			if (!Recv(sizeof(byFlag), &byFlag))
				return false;

			CPythonGuild::Instance().ChangeGuildMemberGeneralFlag(dwPID, byFlag);
			Tracef(" <ChangeMemberGeneralFlag> %d, %d\n", dwPID, byFlag);
			__RefreshGuildWindowMemberPage();
			break;
		}
		case GUILD_SUBHEADER_GC_GUILD_INVITE:
		{
			DWORD dwGuildID;
			if (!Recv(sizeof(dwGuildID), &dwGuildID))
				return false;
			char szGuildName[GUILD_NAME_MAX_LEN + 1];
			if (!Recv(GUILD_NAME_MAX_LEN, &szGuildName))
				return false;

			szGuildName[GUILD_NAME_MAX_LEN] = 0;

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RecvGuildInviteQuestion", Py_BuildValue("(is)", dwGuildID, szGuildName));
			Tracef(" <Guild Invite> %d, %s\n", dwGuildID, szGuildName);
			break;
		}
		case GUILD_SUBHEADER_GC_WAR:
		{
			TPacketGCGuildWar kGuildWar;
			if (!Recv(sizeof(kGuildWar), &kGuildWar))
				return false;

			switch (kGuildWar.bWarState)
			{
				case GUILD_WAR_SEND_DECLARE:
					Tracef(" >> GUILD_SUBHEADER_GC_WAR : GUILD_WAR_SEND_DECLARE\n");
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
						"BINARY_GuildWar_OnSendDeclare",
						Py_BuildValue("(i)", kGuildWar.dwGuildOpp)
					);
					break;
				case GUILD_WAR_RECV_DECLARE:
					Tracef(" >> GUILD_SUBHEADER_GC_WAR : GUILD_WAR_RECV_DECLARE\n");
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
						"BINARY_GuildWar_OnRecvDeclare",
						Py_BuildValue("(ii)", kGuildWar.dwGuildOpp, kGuildWar.bType)
					);
					break;
				case GUILD_WAR_ON_WAR:
					Tracef(" >> GUILD_SUBHEADER_GC_WAR : GUILD_WAR_ON_WAR : %d, %d\n", kGuildWar.dwGuildSelf, kGuildWar.dwGuildOpp);
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
						"BINARY_GuildWar_OnStart",
						Py_BuildValue("(ii)", kGuildWar.dwGuildSelf, kGuildWar.dwGuildOpp)
					);
					CPythonGuild::Instance().StartGuildWar(kGuildWar.dwGuildOpp);
					break;
				case GUILD_WAR_END:
					Tracef(" >> GUILD_SUBHEADER_GC_WAR : GUILD_WAR_END\n");
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
						"BINARY_GuildWar_OnEnd",
						Py_BuildValue("(ii)", kGuildWar.dwGuildSelf, kGuildWar.dwGuildOpp)
					);
					CPythonGuild::Instance().EndGuildWar(kGuildWar.dwGuildOpp);
					break;
			}
			break;
		}
		case GUILD_SUBHEADER_GC_GUILD_NAME:
		{
			DWORD dwID;
			char szGuildName[GUILD_NAME_MAX_LEN + 1];

			int iPacketSize = int(GuildPacket.size) - sizeof(GuildPacket);

			int nItemSize = sizeof(dwID) + GUILD_NAME_MAX_LEN;

			assert(iPacketSize % nItemSize == 0 && "GUILD_SUBHEADER_GC_GUILD_NAME");

			for (; iPacketSize > 0;)
			{
				if (!Recv(sizeof(dwID), &dwID))
					return false;

				if (!Recv(GUILD_NAME_MAX_LEN, &szGuildName))
					return false;

				szGuildName[GUILD_NAME_MAX_LEN] = 0;

				//Tracef(" >> GulidName [%d : %s]\n", dwID, szGuildName);
				CPythonGuild::Instance().RegisterGuildName(dwID, szGuildName);
				iPacketSize -= nItemSize;
			}
			break;
		}
		case GUILD_SUBHEADER_GC_GUILD_WAR_LIST:
		{
			DWORD dwSrcGuildID;
			DWORD dwDstGuildID;

			int iPacketSize = int(GuildPacket.size) - sizeof(GuildPacket);
			int nItemSize = sizeof(dwSrcGuildID) + sizeof(dwDstGuildID);

			assert(iPacketSize % nItemSize == 0 && "GUILD_SUBHEADER_GC_GUILD_WAR_LIST");

			for (; iPacketSize > 0;)
			{
				if (!Recv(sizeof(dwSrcGuildID), &dwSrcGuildID))
					return false;

				if (!Recv(sizeof(dwDstGuildID), &dwDstGuildID))
					return false;

				Tracef(" >> GulidWarList [%d vs %d]\n", dwSrcGuildID, dwDstGuildID);
				CInstanceBase::InsertGVGKey(dwSrcGuildID, dwDstGuildID);
				CPythonCharacterManager::Instance().ChangeGVG(dwSrcGuildID, dwDstGuildID);
				iPacketSize -= nItemSize;
			}
			break;
		}
		case GUILD_SUBHEADER_GC_GUILD_WAR_END_LIST:
		{
			DWORD dwSrcGuildID;
			DWORD dwDstGuildID;

			int iPacketSize = int(GuildPacket.size) - sizeof(GuildPacket);
			int nItemSize = sizeof(dwSrcGuildID) + sizeof(dwDstGuildID);

			assert(iPacketSize % nItemSize == 0 && "GUILD_SUBHEADER_GC_GUILD_WAR_END_LIST");

			for (; iPacketSize > 0;)
			{
				if (!Recv(sizeof(dwSrcGuildID), &dwSrcGuildID))
					return false;

				if (!Recv(sizeof(dwDstGuildID), &dwDstGuildID))
					return false;

				Tracef(" >> GulidWarEndList [%d vs %d]\n", dwSrcGuildID, dwDstGuildID);
				CInstanceBase::RemoveGVGKey(dwSrcGuildID, dwDstGuildID);
				CPythonCharacterManager::Instance().ChangeGVG(dwSrcGuildID, dwDstGuildID);
				iPacketSize -= nItemSize;
			}
			break;
		}
		case GUILD_SUBHEADER_GC_WAR_POINT:
		{
			TPacketGuildWarPoint GuildWarPoint;
			if (!Recv(sizeof(GuildWarPoint), &GuildWarPoint))
				return false;

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
				"BINARY_GuildWar_OnRecvPoint",
				Py_BuildValue("(iii)", GuildWarPoint.dwGainGuildID, GuildWarPoint.dwOpponentGuildID, GuildWarPoint.lPoint)
			);
			break;
		}
		case GUILD_SUBHEADER_GC_MONEY_CHANGE:
		{
			DWORD dwMoney;
			if (!Recv(sizeof(dwMoney), &dwMoney))
				return false;

			CPythonGuild::Instance().SetGuildMoney(dwMoney);

			__RefreshGuildWindowInfoPage();
			Tracef(" >> Guild Money Change : %d\n", dwMoney);
			break;
		}
	}

	return true;
}
// Guild
//////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// Fishing
bool CPythonNetworkStream::SendFishingPacket(int iRotation)
{
	BYTE byHeader = HEADER_CG_FISHING;
	if (!Send(sizeof(byHeader), &byHeader))
		return false;
	BYTE byPacketRotation = iRotation / 5;
	if (!Send(sizeof(BYTE), &byPacketRotation))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendGiveItemPacket(DWORD dwTargetVID, TItemPos ItemPos, int iItemCount)
{
	TPacketCGGiveItem GiveItemPacket;
	GiveItemPacket.byHeader = HEADER_CG_GIVE_ITEM;
	GiveItemPacket.dwTargetVID = dwTargetVID;
	GiveItemPacket.ItemPos = ItemPos;
	GiveItemPacket.byItemCount = iItemCount;

	if (!Send(sizeof(GiveItemPacket), &GiveItemPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvFishing()
{
	TPacketGCFishing FishingPacket;
	if (!Recv(sizeof(FishingPacket), &FishingPacket))
		return false;

	CInstanceBase* pFishingInstance = NULL;
	if (FISHING_SUBHEADER_GC_FISH != FishingPacket.subheader)
	{
		pFishingInstance = CPythonCharacterManager::Instance().GetInstancePtr(FishingPacket.info);
		if (!pFishingInstance)
			return true;
	}

	switch (FishingPacket.subheader)
	{
		case FISHING_SUBHEADER_GC_START:
			pFishingInstance->StartFishing(float(FishingPacket.dir) * 5.0f);
			break;

		case FISHING_SUBHEADER_GC_STOP:
			if (pFishingInstance->IsFishing())
				pFishingInstance->StopFishing();
			break;

		case FISHING_SUBHEADER_GC_REACT:
			if (pFishingInstance->IsFishing())
			{
#if !defined(ENABLE_FISHING_GAME)
				pFishingInstance->SetFishEmoticon(); // Fish Emoticon
#endif
				pFishingInstance->ReactFishing();
			}
			break;

		case FISHING_SUBHEADER_GC_SUCCESS:
			pFishingInstance->CatchSuccess();
#if defined(ENABLE_FISHING_GAME)
			pFishingInstance->SetFishEmoticon();
#endif
			break;

		case FISHING_SUBHEADER_GC_FAIL:
			pFishingInstance->CatchFail();
			if (pFishingInstance == CPythonCharacterManager::Instance().GetMainInstancePtr())
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingFailure", Py_BuildValue("()"));
#if defined(ENABLE_FISHING_GAME)
			pFishingInstance->SetEmoticon(CInstanceBase::EMOTICON_NO_FISH);
#endif
			break;

		case FISHING_SUBHEADER_GC_FISH:
		{
			DWORD dwFishID = FishingPacket.info;

			if (0 == FishingPacket.info)
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingNotifyUnknown", Py_BuildValue("()"));
				return true;
			}

			CItemData* pItemData;
			if (!CItemManager::Instance().GetItemDataPointer(dwFishID, &pItemData))
				return true;

			CInstanceBase* pMainInstance = CPythonCharacterManager::Instance().GetMainInstancePtr();
			if (!pMainInstance)
				return true;

			if (pMainInstance->IsFishing())
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingNotify", Py_BuildValue("(is)", CItemData::ITEM_TYPE_FISH == pItemData->GetType(), pItemData->GetName()));
			else
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingSuccess", Py_BuildValue("(is)", CItemData::ITEM_TYPE_FISH == pItemData->GetType(), pItemData->GetName()));
			break;
		}
	}

	return true;
}

#if defined(ENABLE_FISHING_GAME)
bool CPythonNetworkStream::RecvFishingGamePacket()
{
	TPacketGCFishingGame Packet;
	if (!Recv(sizeof(Packet), &Packet))
		return false;

	CPythonPlayer& rkPlayer = CPythonPlayer::instance();
	switch (Packet.bSubHeader)
	{
		case FISHING_GAME_SUBHEADER_OPEN:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingGameStart", Py_BuildValue("(i)", Packet.bLevel));
			rkPlayer.SetFishingGameWindowOpen(true);
			break;

		case FISHING_GAME_SUBHEADER_QUIT:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingStopGame", Py_BuildValue("()"));
			rkPlayer.SetFishingGameWindowOpen(false);
			break;
	}

	return true;
}

bool CPythonNetworkStream::SendFishingGamePacket(BYTE bSubHeader, BYTE bGoals)
{
	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
	if (!rkPlayer.GetFishingGameWindowOpen())
		return false;

	TPacketCGFishingGame Packet;
	Packet.bHeader = HEADER_CG_FISHING_GAME;
	Packet.bSubHeader = bSubHeader;
	Packet.bGoals = bGoals;
	if (!Send(sizeof(Packet), &Packet))
		return false;

	return SendSequence();
}
#endif
// Fishing
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// Dungeon
bool CPythonNetworkStream::RecvDungeon()
{
	TPacketGCDungeon DungeonPacket;
	if (!Recv(sizeof(DungeonPacket), &DungeonPacket))
		return false;

	switch (DungeonPacket.subheader)
	{
		case DUNGEON_SUBHEADER_GC_TIME_ATTACK_START:
		{
			break;
		}
		case DUNGEON_SUBHEADER_GC_DESTINATION_POSITION:
		{
			unsigned long ulx, uly;
			if (!Recv(sizeof(ulx), &ulx))
				return false;
			if (!Recv(sizeof(uly), &uly))
				return false;

			CPythonPlayer::Instance().SetDungeonDestinationPosition(ulx, uly);
			break;
		}
	}

	return true;
}
// Dungeon
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// MyShop
bool CPythonNetworkStream::SendBuildPrivateShopPacket(const char* c_szName, const std::vector<TShopItemTable>& c_rSellingItemStock)
{
	TPacketCGMyShop packet;
	packet.bHeader = HEADER_CG_MYSHOP;
	strncpy(packet.szSign, c_szName, SHOP_SIGN_MAX_LEN);
	packet.bCount = static_cast<BYTE>(c_rSellingItemStock.size());

	if (!Send(sizeof(packet), &packet))
		return false;

	for (std::vector<TShopItemTable>::const_iterator itor = c_rSellingItemStock.begin(); itor < c_rSellingItemStock.end(); ++itor)
	{
		const TShopItemTable& c_rItem = *itor;
		if (!Send(sizeof(c_rItem), &c_rItem))
			return false;
	}

	return SendSequence();
}

#if defined(ENABLE_MYSHOP_DECO)
bool CPythonNetworkStream::SendMyShopDecoState(BYTE bState)
{
	TPacketCGMyShopDeco Packet;
	Packet.bHeader = HEADER_CG_MYSHOP_DECO;
	Packet.bState = bState;

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracef("TPacketCGMyShopDeco Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendMyShopDecoSet(BYTE bType, DWORD dwPolyVnum)
{
	TPacketCGMyShopDecoAdd Packet;
	Packet.bHeader = HEADER_CG_MYSHOP_DECO_ADD;
	Packet.bType = bType;
	Packet.dwPolyVnum = dwPolyVnum;

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracef("TPacketCGMyShopDecoAdd Error\n");
		return false;
	}

	return SendSequence();
}
#endif

bool CPythonNetworkStream::RecvShopSignPacket()
{
	TPacketGCShopSign p;
	if (!Recv(sizeof(TPacketGCShopSign), &p))
		return false;

	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();

#if defined(ENABLE_GRAPHIC_ON_OFF)
	CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(p.dwVID);
	if (!pInstance)
		return true;
#endif

	if (0 == strlen(p.szSign))
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_PrivateShop_Disappear", Py_BuildValue("(i)", p.dwVID));

		if (rkPlayer.IsMainCharacterIndex(p.dwVID))
			rkPlayer.ClosePrivateShop();

#if defined(ENABLE_GRAPHIC_ON_OFF)
		pInstance->GetGraphicThingInstancePtr()->RemovePrivateShopSign();
#endif
	}
	else
	{
#if defined(ENABLE_MYSHOP_DECO)
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_PrivateShop_Appear", Py_BuildValue("(isi)", p.dwVID, p.szSign, p.bType));
#else
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_PrivateShop_Appear", Py_BuildValue("(is)", p.dwVID, p.szSign));
#endif

		if (rkPlayer.IsMainCharacterIndex(p.dwVID))
			rkPlayer.OpenPrivateShop();

#if defined(ENABLE_GRAPHIC_ON_OFF)
		pInstance->GetGraphicThingInstancePtr()->AddPrivateShopSign();
#endif
	}

	return true;
}
/////////////////////////////////////////////////////////////////////////

bool CPythonNetworkStream::RecvTimePacket()
{
	TPacketGCTime TimePacket;
	if (!Recv(sizeof(TimePacket), &TimePacket))
		return false;

	IAbstractApplication& rkApp = IAbstractApplication::GetSingleton();
	rkApp.SetServerTime(TimePacket.time);

	return true;
}

bool CPythonNetworkStream::RecvWalkModePacket()
{
	TPacketGCWalkMode WalkModePacket;
	if (!Recv(sizeof(WalkModePacket), &WalkModePacket))
		return false;

	CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(WalkModePacket.vid);
	if (pInstance)
	{
		if (WALKMODE_RUN == WalkModePacket.mode)
		{
			pInstance->SetRunMode();
		}
		else
		{
			pInstance->SetWalkMode();
		}
	}

	return true;
}

bool CPythonNetworkStream::RecvChangeSkillGroupPacket()
{
	TPacketGCChangeSkillGroup ChangeSkillGroup;
	if (!Recv(sizeof(ChangeSkillGroup), &ChangeSkillGroup))
		return false;

	m_dwMainActorSkillGroup = ChangeSkillGroup.skill_group;

	CPythonPlayer::Instance().NEW_ClearSkillData();
	__RefreshCharacterWindow();
	return true;
}

void CPythonNetworkStream::__TEST_SetSkillGroupFake(int iIndex)
{
	m_dwMainActorSkillGroup = DWORD(iIndex);

	CPythonPlayer::Instance().NEW_ClearSkillData();
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshCharacter", Py_BuildValue("()"));
}

bool CPythonNetworkStream::SendRefinePacket(BYTE byPos, BYTE byType)
{
	TPacketCGRefine kRefinePacket;
	kRefinePacket.header = HEADER_CG_REFINE;
	kRefinePacket.pos = byPos;
	kRefinePacket.type = byType;

	if (!Send(sizeof(kRefinePacket), &kRefinePacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendSelectItemPacket(DWORD dwItemPos)
{
	TPacketCGScriptSelectItem kScriptSelectItem;
	kScriptSelectItem.header = HEADER_CG_SCRIPT_SELECT_ITEM;
	kScriptSelectItem.selection = dwItemPos;

	if (!Send(sizeof(kScriptSelectItem), &kScriptSelectItem))
		return false;

	return SendSequence();
}

#if defined(ENABLE_GEM_SYSTEM)
bool CPythonNetworkStream::SendSelectMetinStone(DWORD dwItemPos)
{
	TPacketCGSelectItemEx command_select_item_ex(dwItemPos);
	if (!Send(sizeof(command_select_item_ex), &command_select_item_ex))
		return false;
	return SendSequence();
}
#endif

bool CPythonNetworkStream::RecvRefineInformationPacket()
{
	TPacketGCRefineInformation kRefineInfoPacket;
	if (!Recv(sizeof(kRefineInfoPacket), &kRefineInfoPacket))
		return false;

	TRefineTable& rkRefineTable = kRefineInfoPacket.refine_table;
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
		"OpenRefineDialog",
		Py_BuildValue("(iiii)",
			kRefineInfoPacket.pos,
			kRefineInfoPacket.refine_table.result_vnum,
			rkRefineTable.cost,
			rkRefineTable.prob));

	for (int i = 0; i < rkRefineTable.material_count; ++i)
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AppendMaterialToRefineDialog", Py_BuildValue("(ii)", rkRefineTable.materials[i].vnum, rkRefineTable.materials[i].count));
	}

#ifdef _DEBUG
	Tracef(" >> RecvRefineInformationPacket(pos=%d, result_vnum=%d, cost=%d, prob=%d)\n",
		kRefineInfoPacket.pos,
		kRefineInfoPacket.refine_table.result_vnum,
		rkRefineTable.cost,
		rkRefineTable.prob);
#endif

	return true;
}

bool CPythonNetworkStream::RecvRefineInformationPacketNew()
{
	TPacketGCRefineInformationNew kRefineInfoPacket;
	if (!Recv(sizeof(kRefineInfoPacket), &kRefineInfoPacket))
		return false;

	TRefineTable& rkRefineTable = kRefineInfoPacket.refine_table;

#if defined(ENABLE_APPLY_RANDOM)
	PyObject* poList = PyList_New(0);
	if (!poList)
		return false;

	for (BYTE bApplySlot = 0; bApplySlot < ITEM_APPLY_RANDOM_SLOT_MAX_NUM; ++bApplySlot)
	{
		PyObject* poItem = Py_BuildValue("(ii)", kRefineInfoPacket.aApplyRandom[bApplySlot].wType, kRefineInfoPacket.aApplyRandom[bApplySlot].lValue);
		if (!poItem)
		{
			Py_DECREF(poList);
			return false;
		}

		PyList_Append(poList, poItem);
		Py_DECREF(poItem);
	}
#endif

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
		"OpenRefineDialog",
#if defined(ENABLE_APPLY_RANDOM)
		Py_BuildValue("(iiiiiOi)",
			kRefineInfoPacket.pos,
			kRefineInfoPacket.refine_table.result_vnum,
			rkRefineTable.cost,
			rkRefineTable.prob,
			kRefineInfoPacket.type,
			poList,
			rkRefineTable.src_vnum)
#else
		Py_BuildValue("(iiiii)",
			kRefineInfoPacket.pos,
			kRefineInfoPacket.refine_table.result_vnum,
			rkRefineTable.cost,
			rkRefineTable.prob,
			kRefineInfoPacket.type)
#endif
	);

#if defined(ENABLE_APPLY_RANDOM)
	Py_DECREF(poList);
#endif

	for (int i = 0; i < rkRefineTable.material_count; ++i)
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AppendMaterialToRefineDialog", Py_BuildValue("(ii)", rkRefineTable.materials[i].vnum, rkRefineTable.materials[i].count));

#ifdef _DEBUG
	Tracef(" >> RecvRefineInformationPacketNew(pos=%d, result_vnum=%d, cost=%d, prob=%d, type=%d, src_vnum=%d)\n",
		kRefineInfoPacket.pos,
		kRefineInfoPacket.refine_table.result_vnum,
		rkRefineTable.cost,
		rkRefineTable.prob,
		kRefineInfoPacket.type
	);
#endif

	return true;
}

bool CPythonNetworkStream::RecvNPCList()
{
	TPacketGCNPCPosition kNPCPosition;
	if (!Recv(sizeof(kNPCPosition), &kNPCPosition))
		return false;

	assert(int(kNPCPosition.size) - sizeof(kNPCPosition) == kNPCPosition.count * sizeof(TNPCPosition) && "HEADER_GC_NPC_POSITION");

	CPythonMiniMap::Instance().ClearAtlasMarkInfo();

	for (int i = 0; i < kNPCPosition.count; ++i)
	{
		TNPCPosition NPCPosition;
		if (!Recv(sizeof(TNPCPosition), &NPCPosition))
			return false;

		CPythonMiniMap::Instance().RegisterAtlasMark(NPCPosition.bType, NPCPosition.name, NPCPosition.x, NPCPosition.y);
	}

	return true;
}

bool CPythonNetworkStream::__SendCRCReportPacket()
{
	/*
	DWORD dwProcessCRC = 0;
	DWORD dwFileCRC = 0;
	CFilename exeFileName;
	//LPCVOID c_pvBaseAddress = NULL;

	GetExeCRC(dwProcessCRC, dwFileCRC);

	CFilename strRootPackFileName = CEterPackManager::Instance().GetRootPacketFileName();
	strRootPackFileName.ChangeDosPath();

	TPacketCGCRCReport kReportPacket;

	kReportPacket.header = HEADER_CG_CRC_REPORT;
	kReportPacket.byPackMode = CEterPackManager::Instance().GetSearchMode();
	kReportPacket.dwBinaryCRC32 = dwFileCRC;
	kReportPacket.dwProcessCRC32 = dwProcessCRC;
	kReportPacket.dwRootPackCRC32 = GetFileCRC32(strRootPackFileName.c_str());

	if (!Send(sizeof(kReportPacket), &kReportPacket))
		Tracef("SendClientReportPacket Error");

	return SendSequence();
	*/
	return true;
}

bool CPythonNetworkStream::SendClientVersionPacket()
{
	std::string filename;

	GetExcutedFileName(filename);

	filename = CFileNameHelper::NoPath(filename);
	CFileNameHelper::ChangeDosPath(filename);

	if (LocaleService_IsEUROPE() && false == LocaleService_IsYMIR())
	{
		TPacketCGClientVersion2 kVersionPacket;
		kVersionPacket.header = HEADER_CG_CLIENT_VERSION2;
		strncpy(kVersionPacket.filename, filename.c_str(), sizeof(kVersionPacket.filename) - 1);
		strncpy(kVersionPacket.timestamp, "1689090990", sizeof(kVersionPacket.timestamp) - 1);

		if (!Send(sizeof(kVersionPacket), &kVersionPacket))
			Tracef("SendClientReportPacket Error");
	}
	else
	{
		TPacketCGClientVersion kVersionPacket;
		kVersionPacket.header = HEADER_CG_CLIENT_VERSION;
		strncpy(kVersionPacket.filename, filename.c_str(), sizeof(kVersionPacket.filename) - 1);
		strncpy(kVersionPacket.timestamp, __TIMESTAMP__, sizeof(kVersionPacket.timestamp) - 1);

		if (!Send(sizeof(kVersionPacket), &kVersionPacket))
			Tracef("SendClientReportPacket Error");
	}
	return SendSequence();
}

bool CPythonNetworkStream::RecvAffectAddPacket()
{
	TPacketGCAffectAdd kAffectAdd;
	if (!Recv(sizeof(kAffectAdd), &kAffectAdd))
		return false;

	TPacketAffectElement& rkElement = kAffectAdd.elem;
	if (rkElement.wApplyOn == POINT_ENERGY)
	{
		CPythonPlayer::Instance().SetStatus(POINT_ENERGY_END_TIME, CPythonApplication::Instance().GetServerTimeStamp() + rkElement.lDuration);
		__RefreshStatus();
	}

	CPythonPlayer::Instance().AddAffect(rkElement.dwType, kAffectAdd.elem);
#if defined(ENABLE_AFFECT_RENEWAL)
	if (rkElement.bUpdate)
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_NEW_UpdateAffect", Py_BuildValue("(iiii)", rkElement.dwType, rkElement.wApplyOn, rkElement.lApplyValue, rkElement.lDuration));
	else
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_NEW_AddAffect", Py_BuildValue("(iiii)", rkElement.dwType, rkElement.wApplyOn, rkElement.lApplyValue, rkElement.lDuration));
#else
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_NEW_AddAffect", Py_BuildValue("(iiii)", rkElement.dwType, rkElement.wApplyOn, rkElement.lApplyValue, rkElement.lDuration));
#endif

	return true;
}

bool CPythonNetworkStream::RecvAffectRemovePacket()
{
	TPacketGCAffectRemove kAffectRemove;
	if (!Recv(sizeof(kAffectRemove), &kAffectRemove))
		return false;

	CPythonPlayer::Instance().RemoveAffect(kAffectRemove.dwType, kAffectRemove.wApplyOn);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_NEW_RemoveAffect", Py_BuildValue("(ii)", kAffectRemove.dwType, kAffectRemove.wApplyOn));

	return true;
}

bool CPythonNetworkStream::RecvChannelPacket()
{
	TPacketGCChannel kChannelPacket;
	if (!Recv(sizeof(kChannelPacket), &kChannelPacket))
		return false;

	// Tracef(" >> CPythonNetworkStream::RecvChannelPacket(channel=%d)\n", kChannelPacket.channel);

	return true;
}

bool CPythonNetworkStream::RecvViewEquipPacket()
{
	TPacketGCViewEquip kViewEquipPacket;
	if (!Recv(sizeof(kViewEquipPacket), &kViewEquipPacket))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenEquipmentDialog", Py_BuildValue("(i)", kViewEquipPacket.dwVID));

	for (BYTE bSlotIdx = 0; bSlotIdx < WEAR_MAX_NUM; ++bSlotIdx)
	{
		TEquipmentItemSet& rItemSet = kViewEquipPacket.Equips[bSlotIdx];
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetEquipmentDialogItem", Py_BuildValue(
			"("
			"i" // window
			"i" // vid
			"i" // slot_index
			"i" // vnum
			"i" // count
#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
			"i" // change_look_vnum
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
			"(iiOO)" // refine_element
#endif
#if defined(ENABLE_APPLY_RANDOM)
			"(OOOO)" // apply_random_list
#endif
#if defined(ENABLE_SET_ITEM)
			"i" // set_value
#endif
			")"
			, EQUIPMENT
			, kViewEquipPacket.dwVID
			, bSlotIdx
			, rItemSet.dwVnum
			, rItemSet.bCount
#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
			, rItemSet.dwTransmutationVnum
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
			, rItemSet.RefineElement.wApplyType
			, rItemSet.RefineElement.bGrade
			, Py_BuildValue("iii",
				rItemSet.RefineElement.abValue[0],
				rItemSet.RefineElement.abValue[1],
				rItemSet.RefineElement.abValue[2])
			, Py_BuildValue("iii",
				rItemSet.RefineElement.abBonusValue[0],
				rItemSet.RefineElement.abBonusValue[1],
				rItemSet.RefineElement.abBonusValue[2])
#endif
#if defined(ENABLE_APPLY_RANDOM)
			, Py_BuildValue("ii", rItemSet.aApplyRandom[0].wType, rItemSet.aApplyRandom[0].lValue)
			, Py_BuildValue("ii", rItemSet.aApplyRandom[1].wType, rItemSet.aApplyRandom[1].lValue)
			, Py_BuildValue("ii", rItemSet.aApplyRandom[2].wType, rItemSet.aApplyRandom[2].lValue)
			, Py_BuildValue("ii", rItemSet.aApplyRandom[3].wType, rItemSet.aApplyRandom[3].lValue)
#endif
#if defined(ENABLE_SET_ITEM)
			, rItemSet.bSetValue
#endif
		));

		for (BYTE bSocketIdx = 0; bSocketIdx < ITEM_SOCKET_SLOT_MAX_NUM; ++bSocketIdx)
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetEquipmentDialogSocket", Py_BuildValue("(iiiii)",
				EQUIPMENT, kViewEquipPacket.dwVID, bSlotIdx, bSocketIdx, rItemSet.alSockets[bSocketIdx]));

		for (BYTE bAttrIdx = 0; bAttrIdx < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++bAttrIdx)
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetEquipmentDialogAttr", Py_BuildValue("(iiiiii)",
				EQUIPMENT, kViewEquipPacket.dwVID, bSlotIdx, bAttrIdx, rItemSet.aAttr[bAttrIdx].wType, rItemSet.aAttr[bAttrIdx].lValue));
	}

	return true;
}

bool CPythonNetworkStream::RecvLandPacket()
{
	TPacketGCLandList kLandList;
	if (!Recv(sizeof(kLandList), &kLandList))
		return false;

	std::vector<DWORD> kVec_dwGuildID;

	CPythonMiniMap& rkMiniMap = CPythonMiniMap::Instance();
	CPythonBackground& rkBG = CPythonBackground::Instance();
	CInstanceBase* pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();

	rkMiniMap.ClearGuildArea();
	rkBG.ClearGuildArea();

	int iPacketSize = (kLandList.size - sizeof(TPacketGCLandList));
	for (; iPacketSize > 0; iPacketSize -= sizeof(TLandPacketElement))
	{
		TLandPacketElement kElement;
		if (!Recv(sizeof(TLandPacketElement), &kElement))
			return false;

		rkMiniMap.RegisterGuildArea(kElement.dwID,
			kElement.dwGuildID,
			kElement.x,
			kElement.y,
			kElement.width,
			kElement.height);

		if (pMainInstance)
			if (kElement.dwGuildID == pMainInstance->GetGuildID())
			{
				rkBG.RegisterGuildArea(kElement.x,
					kElement.y,
					kElement.x + kElement.width,
					kElement.y + kElement.height);
			}

		if (0 != kElement.dwGuildID)
			kVec_dwGuildID.push_back(kElement.dwGuildID);
	}

	if (kVec_dwGuildID.size() > 0)
		__DownloadSymbol(kVec_dwGuildID);

	return true;
}

bool CPythonNetworkStream::RecvTargetCreatePacket()
{
	TPacketGCTargetCreate kTargetCreate;
	if (!Recv(sizeof(kTargetCreate), &kTargetCreate))
		return false;

#if defined(ENABLE_LOCALE_CLIENT)
	CPythonLocale::Instance().FormatString(kTargetCreate.szTargetName, sizeof(kTargetCreate.szTargetName));
#endif

	CPythonMiniMap& rkpyMiniMap = CPythonMiniMap::Instance();
	rkpyMiniMap.CreateTarget(kTargetCreate.lID, kTargetCreate.szTargetName);

	//#ifdef _DEBUG
	//	char szBuf[256+1];
	//	_snprintf(szBuf, sizeof(szBuf), "타겟이 생성 되었습니다 [%d:%s]", kTargetCreate.lID, kTargetCreate.szTargetName);
	//	CPythonChat::Instance().AppendChat(CHAT_TYPE_NOTICE, szBuf);
	//	Tracef(" >> RecvTargetCreatePacket %d : %s\n", kTargetCreate.lID, kTargetCreate.szTargetName);
	//#endif

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OpenAtlasWindow", Py_BuildValue("()"));
	return true;
}

bool CPythonNetworkStream::RecvTargetCreatePacketNew()
{
	TPacketGCTargetCreateNew kTargetCreate;
	if (!Recv(sizeof(kTargetCreate), &kTargetCreate))
		return false;

#if defined(ENABLE_LOCALE_CLIENT)
	CPythonLocale::Instance().FormatString(kTargetCreate.szTargetName, sizeof(kTargetCreate.szTargetName));
#endif

	CPythonMiniMap& rkpyMiniMap = CPythonMiniMap::Instance();
	CPythonBackground& rkpyBG = CPythonBackground::Instance();
	if (CREATE_TARGET_TYPE_LOCATION == kTargetCreate.byType)
	{
		rkpyMiniMap.CreateTarget(kTargetCreate.lID, kTargetCreate.szTargetName);
	}
#if defined(ENABLE_PRIVATESHOP_SEARCH_SYSTEM)
	else if (CREATE_TARGET_TYPE_SHOP_SEARCH == kTargetCreate.byType)
	{
		rkpyMiniMap.CreateTarget(kTargetCreate.lID, kTargetCreate.szTargetName, kTargetCreate.dwVID);
		rkpyBG.CreatePrivateShopPos(kTargetCreate.lID, kTargetCreate.dwVID);
	}
#endif
	else
	{
		rkpyMiniMap.CreateTarget(kTargetCreate.lID, kTargetCreate.szTargetName, kTargetCreate.dwVID);
		rkpyBG.CreateTargetEffect(kTargetCreate.lID, kTargetCreate.dwVID);
	}

	//#ifdef _DEBUG
	//	char szBuf[256+1];
	//	_snprintf(szBuf, sizeof(szBuf), "캐릭터 타겟이 생성 되었습니다 [%d:%s:%d]", kTargetCreate.lID, kTargetCreate.szTargetName, kTargetCreate.dwVID);
	//	CPythonChat::Instance().AppendChat(CHAT_TYPE_NOTICE, szBuf);
	//	Tracef(" >> RecvTargetCreatePacketNew %d : %d/%d\n", kTargetCreate.lID, kTargetCreate.byType, kTargetCreate.dwVID);
	//#endif

	//PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OpenAtlasWindow", Py_BuildValue("()"));
	return true;
}

bool CPythonNetworkStream::RecvTargetUpdatePacket()
{
	TPacketGCTargetUpdate kTargetUpdate;
	if (!Recv(sizeof(kTargetUpdate), &kTargetUpdate))
		return false;

	CPythonMiniMap& rkpyMiniMap = CPythonMiniMap::Instance();
	rkpyMiniMap.UpdateTarget(kTargetUpdate.lID, kTargetUpdate.lX, kTargetUpdate.lY);

	CPythonBackground& rkpyBG = CPythonBackground::Instance();
#if defined(ENABLE_PRIVATESHOP_SEARCH_SYSTEM)
	//if (kTargetUpdate.bIsShopSearch == true)
	//	rkpyBG.CreateShopSearchTargetEffect(kTargetUpdate.lID, kTargetUpdate.lX, kTargetUpdate.lY);
	//else
	rkpyBG.CreateTargetEffect(kTargetUpdate.lID, kTargetUpdate.lX, kTargetUpdate.lY);
#else
	rkpyBG.CreateTargetEffect(kTargetUpdate.lID, kTargetUpdate.lX, kTargetUpdate.lY);
#endif

	//#ifdef _DEBUG
	//	char szBuf[256+1];
	//	_snprintf(szBuf, sizeof(szBuf), "타겟의 위치가 갱신 되었습니다 [%d:%d/%d]", kTargetUpdate.lID, kTargetUpdate.lX, kTargetUpdate.lY);
	//	CPythonChat::Instance().AppendChat(CHAT_TYPE_NOTICE, szBuf);
	//	Tracef(" >> RecvTargetUpdatePacket %d : %d, %d\n", kTargetUpdate.lID, kTargetUpdate.lX, kTargetUpdate.lY);
	//#endif

	return true;
}

bool CPythonNetworkStream::RecvTargetDeletePacket()
{
	TPacketGCTargetDelete kTargetDelete;
	if (!Recv(sizeof(kTargetDelete), &kTargetDelete))
		return false;

	CPythonMiniMap& rkpyMiniMap = CPythonMiniMap::Instance();
	rkpyMiniMap.DeleteTarget(kTargetDelete.lID);

	CPythonBackground& rkpyBG = CPythonBackground::Instance();
	rkpyBG.DeleteTargetEffect(kTargetDelete.lID);

	//#ifdef _DEBUG
	//	Tracef(" >> RecvTargetDeletePacket %d\n", kTargetDelete.lID);
	//#endif

	return true;
}

bool CPythonNetworkStream::RecvLoverInfoPacket()
{
	TPacketGCLoverInfo kLoverInfo;
	if (!Recv(sizeof(kLoverInfo), &kLoverInfo))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_LoverInfo", Py_BuildValue("(si)", kLoverInfo.szName, kLoverInfo.byLovePoint));
#ifdef _DEBUG
	Tracef("RECV LOVER INFO : %s, %d\n", kLoverInfo.szName, kLoverInfo.byLovePoint);
#endif
	return true;
}

bool CPythonNetworkStream::RecvLovePointUpdatePacket()
{
	TPacketGCLovePointUpdate kLovePointUpdate;
	if (!Recv(sizeof(kLovePointUpdate), &kLovePointUpdate))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_UpdateLovePoint", Py_BuildValue("(i)", kLovePointUpdate.byLovePoint));
#ifdef _DEBUG
	Tracef("RECV LOVE POINT UPDATE : %d\n", kLovePointUpdate.byLovePoint);
#endif
	return true;
}

bool CPythonNetworkStream::RecvDigMotionPacket()
{
	TPacketGCDigMotion kDigMotion;
	if (!Recv(sizeof(kDigMotion), &kDigMotion))
		return false;

#ifdef _DEBUG
	Tracef(" Dig Motion [%d/%d]\n", kDigMotion.vid, kDigMotion.count);
#endif

	IAbstractCharacterManager& rkChrMgr = IAbstractCharacterManager::GetSingleton();
	CInstanceBase* pkInstMain = rkChrMgr.GetInstancePtr(kDigMotion.vid);
	CInstanceBase* pkInstTarget = rkChrMgr.GetInstancePtr(kDigMotion.target_vid);
	if (NULL == pkInstMain)
		return true;

	if (pkInstTarget)
		pkInstMain->NEW_LookAtDestInstance(*pkInstTarget);

	for (int i = 0; i < kDigMotion.count; ++i)
		pkInstMain->PushOnceMotion(CRaceMotionData::NAME_DIG);

	return true;
}

#if defined(ENABLE_DRAGON_SOUL_SYSTEM)
// 용혼석 강화
bool CPythonNetworkStream::SendDragonSoulRefinePacket(BYTE bRefineType, TItemPos* pos)
{
	TPacketCGDragonSoulRefine pk;
	pk.header = HEADER_CG_DRAGON_SOUL_REFINE;
	pk.bSubType = bRefineType;
	memcpy(pk.ItemGrid, pos, sizeof(TItemPos) * DS_REFINE_WINDOW_MAX_NUM);
	if (!Send(sizeof(pk), &pk))
	{
		return false;
	}
	return true;
}
#endif

#if defined(ENABLE_SKILLBOOK_COMB_SYSTEM)
bool CPythonNetworkStream::SendSkillBookCombinationPacket(TItemPos* pPos, BYTE bAction)
{
	TPacketCGSkillBookCombination SkillBookCombCGPacket;
	SkillBookCombCGPacket.bHeader = HEADER_CG_SKILLBOOK_COMB;
	SkillBookCombCGPacket.bAction = bAction;
	memcpy(SkillBookCombCGPacket.CombItemGrid, pPos, sizeof(TItemPos) * SKILLBOOK_COMB_SLOT_MAX);

	if (!Send(sizeof(SkillBookCombCGPacket), &SkillBookCombCGPacket))
		return false;

	return true;
}
#endif

#if defined(ENABLE_MINI_GAME_CATCH_KING)
bool CPythonNetworkStream::SendMiniGameCatchKing(BYTE bSubHeader, BYTE bSubArgument)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGMiniGameCatchKing packet;
	packet.bHeader = HEADER_CG_MINI_GAME_CATCH_KING;
	packet.bSubHeader = bSubHeader;
	packet.bSubArgument = bSubArgument;

	if (!Send(sizeof(TPacketCGMiniGameCatchKing), &packet))
	{
		Tracef("SendMiniGameCatchKing Send Packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvMiniGameCatchKingPacket()
{
	std::vector<char> vPacketBuffer;
	vPacketBuffer.clear();

	TPacketGCMiniGameCatchKing Packet;
	if (!Recv(sizeof(Packet), &Packet))
		return false;

	unsigned int uiPacketSize = Packet.wSize - sizeof(Packet);
	if (uiPacketSize > 0)
	{
		vPacketBuffer.resize(uiPacketSize);
		if (!Recv(uiPacketSize, &vPacketBuffer[0]))
			return false;
	}

	switch (Packet.bSubHeader)
	{
		case CATCHKING_GC_START:
		{
			DWORD dwBigScore = *(DWORD*)&vPacketBuffer[0];
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MiniGameCatchKingEventStart", Py_BuildValue("(i)", dwBigScore));
		}
		break;

		case CATCHKING_GC_SET_CARD:
		{
			BYTE bCardNumber = *(BYTE*)&vPacketBuffer[0];
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MiniGameCatchKingSetHandCard", Py_BuildValue("(i)", bCardNumber));
		}
		break;

		case CATCHKING_GC_RESULT_FIELD:
		{
			TPacketGCMiniGameCatchKingResult* pkData = (TPacketGCMiniGameCatchKingResult*)&vPacketBuffer[0];
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MiniGameCatchKingResultField", Py_BuildValue("(iiiibbbb)",
				pkData->dwPoints, pkData->bRowType, pkData->bCardPos, pkData->bCardValue,
				pkData->bKeepFieldCard, pkData->bDestroyHandCard, pkData->bGetReward, pkData->bIsFiveNearBy));
		}
		break;

		case CATCHKING_GC_SET_END_CARD:
		{
			TPacketGCMiniGameCatchKingSetEndCard* pkData = (TPacketGCMiniGameCatchKingSetEndCard*)&vPacketBuffer[0];
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MiniGameCatchKingSetEndCard", Py_BuildValue("(ii)",
				pkData->bCardPos, pkData->bCardValue));
		}
		break;

		case CATCHKING_GC_REWARD:
		{
			BYTE bReturnCode = *(BYTE*)&vPacketBuffer[0];
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MiniGameCatchKingReward", Py_BuildValue("(i)", bReturnCode));
		}
		break;

#if defined(ENABLE_CATCH_KING_EVENT_FLAG_RENEWAL)
		case CATCHKING_GC_SET_CARD_PIECE_FLAG:
		case CATCHKING_GC_SET_CARD_FLAG:
		case CATCHKING_GC_SET_QUEST_FLAG:
		case CATCHKING_GC_NO_MORE_GAIN:
		{
			TPacketGCMiniGameCatchKingQuestFlag* pkData = (TPacketGCMiniGameCatchKingQuestFlag*)&vPacketBuffer[0];
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "CatchKingFlagProcess", Py_BuildValue("(i(ii))",
				Packet.bSubHeader, pkData->wPieceCount, pkData->wPackCount));
		}
		break;
#endif

		default:
			TraceError("CPythonNetworkStream::RecvMiniGameCatchKingPacket: Unknown SubHeader %d\n", Packet.bSubHeader);
			break;
	}

	return true;
}

#if defined(ENABLE_CATCH_KING_EVENT_FLAG_RENEWAL)
bool CPythonNetworkStream::SendMiniGameCatchKingRequestQuestFlag()
{
	TPacketCGMiniGameCatchKing Packet;
	Packet.bHeader = HEADER_CG_MINI_GAME_CATCH_KING;
	Packet.bSubHeader = CATCHKING_CG_REQUEST_QUEST_FLAG;

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracef("SendMiniGameCatchKingRequestQuestFlag Error\n");
		return false;
	}

	return SendSequence();
}
#endif
#endif

#if defined(ENABLE_PRIVATESHOP_SEARCH_SYSTEM)
bool CPythonNetworkStream::RecvPrivateShopSearch()
{
	TPacketGCPrivateShopSearch p;
	if (!Recv(sizeof(p), &p))
		return false;

	CPythonShop::Instance().ClearShopSearchData();
	unsigned int iPacketSize = (p.size - sizeof(TPacketGCPrivateShopSearch));
	for (; iPacketSize > 0; iPacketSize -= sizeof(TPacketGCPrivateShopSearchItem))
	{
		TPacketGCPrivateShopSearchItem Item;
		if (!Recv(sizeof(TPacketGCPrivateShopSearchItem), &Item))
			return false;

		ShopSearchData* SShopSearch = new ShopSearchData();
		SShopSearch->item = Item.item;
		SShopSearch->name.assign(Item.szSellerName);
		SShopSearch->dwShopPID = Item.dwShopPID;
		SShopSearch->dwShopVID = Item.dwShopVID;
		CPythonShop::Instance().SetShopSearchItemData(SShopSearch);
	}

	CPythonShop::Instance().SortShopSearchData();
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshPShopSearchDialog", Py_BuildValue("()"));
	return true;
}

bool CPythonNetworkStream::RecvPrivateShopSearchOpen()
{
	TPacketGCPrivateShopSearchOpen p;
	if (!Recv(sizeof(p), &p))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenPShopSearchDialogCash", Py_BuildValue("()"));
	return true;
}

void CPythonNetworkStream::PrivateShopSearchChangePage(int iPage)
{
	CPythonShop::Instance().ShopSearchChangePage(iPage);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshPShopSearchDialog", Py_BuildValue("()"));
}

bool CPythonNetworkStream::SendPrivateShopSearchInfoPacket(BYTE bJob, BYTE bMaskType, int iMaskSub, int iMinRefine, int iMaxRefine, int iMinLevel, int iMaxLevel, int iMinGold, int iMaxGold, const char* szItemName
#if defined(ENABLE_CHEQUE_SYSTEM)
	, int iMinCheque, int iMaxCheque
#endif
)
{
	TPacketCGPrivateShopSearch packet;
	packet.header = HEADER_CG_PRIVATESHOP_SEARCH;
	packet.bJob = bJob;
	packet.bMaskType = bMaskType;
	packet.iMaskSub = iMaskSub;
	packet.iMinRefine = iMinRefine;
	packet.iMaxRefine = iMaxRefine;
	packet.iMinLevel = iMinLevel;
	packet.iMaxLevel = iMaxLevel;
	packet.iMinGold = iMinGold;
	packet.iMaxGold = iMaxGold;
	std::strcpy(packet.szItemName, szItemName);
#if defined(ENABLE_CHEQUE_SYSTEM)
	packet.iMinCheque = iMinCheque;
	packet.iMaxCheque = iMaxCheque;
#endif

	if (!Send(sizeof(packet), &packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendPrivateShopSearchClose()
{
	TPacketCGPrivateShopSearchClose packet;
	packet.header = HEADER_CG_PRIVATESHOP_SEARCH_CLOSE;
	if (!Send(sizeof(packet), &packet))
		return false;

	CPythonShop::Instance().ClearShopSearchData();
	return SendSequence();
}

bool CPythonNetworkStream::SendPrivateShopSearchBuyItem(int iIndex)
{
	auto Item = CPythonShop::Instance().GetShopSearchItemData(iIndex);
	if (Item == nullptr)
		return false;

	TPacketCGPrivateShopSearchBuyItem packet;
	packet.header = HEADER_CG_PRIVATESHOP_SEARCH_BUY_ITEM;
	packet.pos = Item->item.bDisplayPos;
	packet.dwShopPID = Item->dwShopPID;
	packet.dwShopVID = Item->dwShopVID;
	if (!Send(sizeof(packet), &packet))
		return false;

	return SendSequence();
}
#endif

#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
bool CPythonNetworkStream::RecvAcceRefinePacket()
{
	TPacketGCAcceRefine Packet;
	if (!Recv(sizeof(TPacketGCAcceRefine), &Packet))
	{
		Tracef("CPythonNetworkStream::RecvAcceRefinePacket Error\n");
		return false;
	}

	UINT uiPacketSize = (Packet.wSize - sizeof(TPacketGCAcceRefine));
	switch (Packet.bSubHeader)
	{
		case ACCE_REFINE_SUBHEADER_GC_CLOSE:
		case ACCE_REFINE_SUBHEADER_GC_OPEN:
		{
			if (uiPacketSize > 0)
			{
				TSubPacketGCAcceRefineOpenClose SubPacket;
				assert(uiPacketSize % sizeof(TSubPacketGCAcceRefineOpenClose) == 0 && "ACCE_REFINE_SUBHEADER_GC_CLOSE | ACCE_REFINE_SUBHEADER_GC_OPEN");
				if (!Recv(sizeof(TSubPacketGCAcceRefineOpenClose), &SubPacket))
					return false;

				if (Packet.bSubHeader == ACCE_REFINE_SUBHEADER_GC_CLOSE)
				{
					if (SubPacket.bServerClose)
						PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "CloseAcceWindow", Py_BuildValue("()"));

					for (BYTE bSlotIndex = ACCE_SLOT_LEFT; bSlotIndex < ACCE_SLOT_MAX; ++bSlotIndex)
						CPythonPlayer::Instance().DelAcceItemData(bSlotIndex);
				}
				else if (Packet.bSubHeader == ACCE_REFINE_SUBHEADER_GC_OPEN)
				{
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenAcceWindow", Py_BuildValue("(i)", SubPacket.bType));
				}

				CPythonPlayer::Instance().SetAcceRefineWindowType(SubPacket.bType);
			}
			else
				TraceError("CPythonNetworkStream::RecvAcceRefinePacket - Empty Packet Size SubHeader %d", Packet.bSubHeader);
		}
		break;

		case ACCE_REFINE_SUBHEADER_GC_SET_ITEM:
		{
			if (uiPacketSize > 0)
			{
				TSubPacketGCAcceRefineSetItem SubPacket;
				assert(uiPacketSize % sizeof(TSubPacketGCAcceRefineSetItem) == 0 && "ACCE_REFINE_SUBHEADER_GC_SET_ITEM");
				if (!Recv(sizeof(TSubPacketGCAcceRefineSetItem), &SubPacket))
					return false;

				TItemData ItemData = {};
				ItemData.dwVnum = SubPacket.Item.dwVnum;
				ItemData.dwCount = SubPacket.Item.dwCount;
				for (BYTE bSocketIndex = 0; bSocketIndex < ITEM_SOCKET_SLOT_MAX_NUM; ++bSocketIndex)
					ItemData.alSockets[bSocketIndex] = SubPacket.Item.alSockets[bSocketIndex];
				for (BYTE bAttrIndex = 0; bAttrIndex < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++bAttrIndex)
					ItemData.aAttr[bAttrIndex] = SubPacket.Item.aAttr[bAttrIndex];
#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
				ItemData.dwTransmutationVnum = SubPacket.Item.dwTransmutationVnum;
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
				ItemData.RefineElement = SubPacket.Item.RefineElement;
#endif
#if defined(ENABLE_APPLY_RANDOM)
				for (BYTE bApplyIndex = 0; bApplyIndex < ITEM_APPLY_RANDOM_SLOT_MAX_NUM; ++bApplyIndex)
					ItemData.aApplyRandom[bApplyIndex] = SubPacket.Item.aApplyRandom[bApplyIndex];
#endif
#if defined(ENABLE_SET_ITEM)
				ItemData.bSetValue = SubPacket.Item.bSetValue;
#endif

				if (SubPacket.AttachedPos.IsValidCell() && !SubPacket.AttachedPos.IsEquipCell())
					CPythonPlayer::Instance().SetActivatedAcceSlot(static_cast<BYTE>(SubPacket.SelectedPos.cell), SubPacket.AttachedPos);

				CPythonPlayer::Instance().SetAcceItemData(static_cast<BYTE>(SubPacket.SelectedPos.cell), ItemData);
			}
			else
				TraceError("CPythonNetworkStream::RecvAcceRefinePacket - Empty Packet Size SubHeader %d", Packet.bSubHeader);
		}
		break;

		case ACCE_REFINE_SUBHEADER_GC_CLEAR_SLOT:
		{
			if (uiPacketSize > 0)
			{
				TSubPacketGCAcceRefineClearSlot SubPacket;
				assert(uiPacketSize % sizeof(TSubPacketGCAcceRefineClearSlot) == 0 && "ACCE_REFINE_SUBHEADER_GC_CLEAR_SLOT");
				if (!Recv(sizeof(TSubPacketGCAcceRefineClearSlot), &SubPacket))
					return false;

				CPythonPlayer::Instance().DelAcceItemData(SubPacket.bSlotIndex);
			}
			else
				TraceError("CPythonNetworkStream::RecvAcceRefinePacket - Empty Packet Size SubHeader %d", Packet.bSubHeader);
		}
		break;

		case ACCE_REFINE_SUBHEADER_GC_CLEAR_ALL:
		{
			for (BYTE bSlotIndex = ACCE_SLOT_LEFT; bSlotIndex < ACCE_SLOT_MAX; ++bSlotIndex)
				CPythonPlayer::Instance().DelAcceItemData(bSlotIndex);
		}
		break;
	}

	__RefreshInventoryWindow();

	return true;
}

bool CPythonNetworkStream::SendAcceRefineCheckIn(TItemPos AttachedPos, TItemPos SelectedPos, BYTE bType)
{
	__PlayInventoryItemDropSound(AttachedPos);

	TPacketCGAcceRefine Packet;
	Packet.wSize = sizeof(TPacketCGAcceRefine) + sizeof(TSubPacketCGAcceRefineCheckIn);
	Packet.bSubHeader = ACCE_REFINE_SUBHEADER_CG_CHECKIN;

	TSubPacketCGAcceRefineCheckIn SubPacket;
	SubPacket.AttachedPos = AttachedPos;
	SubPacket.SelectedPos = SelectedPos;
	SubPacket.bType = bType;

	if (!Send(sizeof(TPacketCGAcceRefine), &Packet))
		return false;

	if (!Send(sizeof(TSubPacketCGAcceRefineCheckIn), &SubPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendAcceRefineCheckOut(TItemPos SelectedPos, BYTE bType)
{
	TPacketCGAcceRefine Packet;
	Packet.wSize = sizeof(TPacketCGAcceRefine) + sizeof(TSubPacketCGAcceRefineCheckOut);
	Packet.bSubHeader = ACCE_REFINE_SUBHEADER_CG_CHECKOUT;

	TSubPacketCGAcceRefineCheckOut SubPacket;
	SubPacket.SelectedPos = SelectedPos;
	SubPacket.bType = bType;

	if (!Send(sizeof(TPacketCGAcceRefine), &Packet))
		return false;

	if (!Send(sizeof(TSubPacketCGAcceRefineCheckOut), &SubPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendAcceRefineAccept(BYTE bType)
{
	TPacketCGAcceRefine Packet;
	Packet.wSize = sizeof(TPacketCGAcceRefine) + sizeof(TSubPacketCGAcceRefineAccept);
	Packet.bSubHeader = ACCE_REFINE_SUBHEADER_CG_ACCEPT;

	TSubPacketCGAcceRefineAccept SubPacket;
	SubPacket.bType = bType;

	if (!Send(sizeof(TPacketCGAcceRefine), &Packet))
		return false;

	if (!Send(sizeof(TSubPacketCGAcceRefineAccept), &SubPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendAcceRefineCancel()
{
	TPacketCGAcceRefine Packet;
	Packet.wSize = sizeof(TPacketCGAcceRefine);
	Packet.bSubHeader = ACCE_REFINE_SUBHEADER_CG_CANCEL;

	if (!Send(sizeof(TPacketCGAcceRefine), &Packet))
		return false;

	return SendSequence();
}
#endif

#if defined(ENABLE_AURA_COSTUME_SYSTEM)
bool CPythonNetworkStream::RecvAuraRefinePacket()
{
	TPacketGCAuraRefine Packet;
	if (!Recv(sizeof(TPacketGCAuraRefine), &Packet))
	{
		Tracef("CPythonNetworkStream::RecvAuraRefinePacket Error\n");
		return false;
	}

	UINT uiPacketSize = (Packet.wSize - sizeof(TPacketGCAuraRefine));
	switch (Packet.bSubHeader)
	{
		case AURA_REFINE_SUBHEADER_GC_CLOSE:
		case AURA_REFINE_SUBHEADER_GC_OPEN:
		{
			if (uiPacketSize > 0)
			{
				TSubPacketGCAuraRefineOpenClose SubPacket;
				assert(uiPacketSize % sizeof(TSubPacketGCAuraRefineOpenClose) == 0 && "AURA_REFINE_SUBHEADER_CG_CLOSE | AURA_REFINE_SUBHEADER_CG_OPEN");
				if (!Recv(sizeof(TSubPacketGCAuraRefineOpenClose), &SubPacket))
					return false;

				if (Packet.bSubHeader == AURA_REFINE_SUBHEADER_GC_CLOSE)
				{
					if (SubPacket.bServerClose)
						PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "CloseAuraWindow", Py_BuildValue("()"));

					for (BYTE bSlotIndex = AURA_SLOT_MAIN; bSlotIndex < AURA_SLOT_MAX; ++bSlotIndex)
						CPythonPlayer::Instance().DelAuraItemData(bSlotIndex);
				}
				else if (Packet.bSubHeader == AURA_REFINE_SUBHEADER_GC_OPEN)
				{
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenAuraWindow", Py_BuildValue("(i)", SubPacket.bType));
				}

				CPythonPlayer::Instance().SetAuraWindowType(SubPacket.bType);
			}
			else
				TraceError("CPythonNetworkStream::RecvAuraRefinePacket - Empty Packet Size SubHeader %d", Packet.bSubHeader);
		}
		break;

		case AURA_REFINE_SUBHEADER_GC_SET_ITEM:
		{
			if (uiPacketSize > 0)
			{
				TSubPacketGCAuraRefineSetItem SubPacket;
				assert(uiPacketSize % sizeof(TSubPacketGCAuraRefineSetItem) == 0 && "AURA_REFINE_SUBHEADER_GC_SET_ITEM");
				if (!Recv(sizeof(TSubPacketGCAuraRefineSetItem), &SubPacket))
					return false;

				TItemData ItemData = {};
				ItemData.dwVnum = SubPacket.Item.dwVnum;
				ItemData.dwCount = SubPacket.Item.dwCount;
				for (BYTE bSocketIndex = 0; bSocketIndex < ITEM_SOCKET_SLOT_MAX_NUM; ++bSocketIndex)
					ItemData.alSockets[bSocketIndex] = SubPacket.Item.alSockets[bSocketIndex];
				for (BYTE bAttrIndex = 0; bAttrIndex < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++bAttrIndex)
					ItemData.aAttr[bAttrIndex] = SubPacket.Item.aAttr[bAttrIndex];
#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
				ItemData.dwTransmutationVnum = SubPacket.Item.dwTransmutationVnum;
#endif
#if defined(ENABLE_APPLY_RANDOM)
				for (BYTE bApplyIndex = 0; bApplyIndex < ITEM_APPLY_RANDOM_SLOT_MAX_NUM; ++bApplyIndex)
					ItemData.aApplyRandom[bApplyIndex] = SubPacket.Item.aApplyRandom[bApplyIndex];
#endif
#if defined(ENABLE_SET_ITEM)
				ItemData.bSetValue = SubPacket.Item.bSetValue;
#endif

				if (SubPacket.AttachedPos.IsValidCell() && !SubPacket.AttachedPos.IsEquipCell())
					CPythonPlayer::Instance().SetActivatedAuraSlot(static_cast<BYTE>(SubPacket.SelectedPos.cell), SubPacket.AttachedPos);

				CPythonPlayer::Instance().SetAuraItemData(static_cast<BYTE>(SubPacket.SelectedPos.cell), ItemData);
			}
			else
				TraceError("CPythonNetworkStream::RecvAuraRefinePacket - Empty Packet Size SubHeader %d", Packet.bSubHeader);
		}
		break;

		case AURA_REFINE_SUBHEADER_GC_CLEAR_SLOT:
		{
			if (uiPacketSize > 0)
			{
				TSubPacketGCAuraRefineClearSlot SubPacket;
				assert(uiPacketSize % sizeof(TSubPacketGCAuraRefineClearSlot) == 0 && "AURA_REFINE_SUBHEADER_GC_CLEAR_SLOT");
				if (!Recv(sizeof(TSubPacketGCAuraRefineClearSlot), &SubPacket))
					return false;

				CPythonPlayer::Instance().DelAuraItemData(SubPacket.bSlotIndex);
			}
			else
				TraceError("CPythonNetworkStream::RecvAuraRefinePacket - Empty Packet Size SubHeader %d", Packet.bSubHeader);
		}
		break;

		case AURA_REFINE_SUBHEADER_GC_CLEAR_ALL:
		{
			for (BYTE bSlotIndex = AURA_SLOT_MAIN; bSlotIndex < AURA_SLOT_MAX; ++bSlotIndex)
				CPythonPlayer::Instance().DelAuraItemData(bSlotIndex);
		}
		break;

		case AURA_REFINE_SUBHEADER_GC_INFO:
		{
			if (uiPacketSize > 0)
			{
				for (UINT uiPacketOffset = 0; uiPacketSize > 0; ++uiPacketOffset)
				{
					assert(uiPacketSize % sizeof(TSubPacketGCAuraRefineInfo) == 0 && "AURA_REFINE_SUBHEADER_GC_INFO");
					TSubPacketGCAuraRefineInfo SubPacket;
					if (!Recv(sizeof(TSubPacketGCAuraRefineInfo), &SubPacket))
						return false;

					CPythonPlayer::Instance().SetAuraRefineInfo(SubPacket.bInfoType, SubPacket.bInfoLevel, SubPacket.bInfoExpPercent);
					uiPacketSize -= sizeof(TSubPacketGCAuraRefineInfo);
				}
			}
			else
				TraceError("CPythonNetworkStream::RecvAuraRefinePacket - Empty Packet Size SubHeader %d", Packet.bSubHeader);
		}
		break;
	}

	__RefreshInventoryWindow();

	return true;
}

bool CPythonNetworkStream::SendAuraRefineCheckIn(TItemPos AttachedPos, TItemPos SelectedPos, BYTE bType)
{
	__PlayInventoryItemDropSound(AttachedPos);

	TPacketCGAuraRefine Packet;
	Packet.wSize = sizeof(TPacketCGAuraRefine) + sizeof(TSubPacketCGAuraRefineCheckIn);
	Packet.bSubHeader = AURA_REFINE_SUBHEADER_CG_CHECKIN;

	TSubPacketCGAuraRefineCheckIn SubPacket;
	SubPacket.AttachedPos = AttachedPos;
	SubPacket.SelectedPos = SelectedPos;
	SubPacket.bType = bType;

	if (!Send(sizeof(TPacketCGAuraRefine), &Packet))
		return false;

	if (!Send(sizeof(TSubPacketCGAuraRefineCheckIn), &SubPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendAuraRefineCheckOut(TItemPos SelectedPos, BYTE bType)
{
	TPacketCGAuraRefine Packet;
	Packet.wSize = sizeof(TPacketCGAuraRefine) + sizeof(TSubPacketCGAuraRefineCheckOut);
	Packet.bSubHeader = AURA_REFINE_SUBHEADER_CG_CHECKOUT;

	TSubPacketCGAuraRefineCheckOut SubPacket;
	SubPacket.SelectedPos = SelectedPos;
	SubPacket.bType = bType;

	if (!Send(sizeof(TPacketCGAuraRefine), &Packet))
		return false;

	if (!Send(sizeof(TSubPacketCGAuraRefineCheckOut), &SubPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendAuraRefineAccept(BYTE bType)
{
	TPacketCGAuraRefine Packet;
	Packet.wSize = sizeof(TPacketCGAuraRefine) + sizeof(TSubPacketCGAuraRefineAccept);
	Packet.bSubHeader = AURA_REFINE_SUBHEADER_CG_ACCEPT;

	TSubPacketCGAuraRefineAccept SubPacket;
	SubPacket.bType = bType;

	if (!Send(sizeof(TPacketCGAuraRefine), &Packet))
		return false;

	if (!Send(sizeof(TSubPacketCGAuraRefineAccept), &SubPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendAuraRefineCancel()
{
	TPacketCGAuraRefine Packet;
	Packet.wSize = sizeof(TPacketCGAuraRefine);
	Packet.bSubHeader = AURA_REFINE_SUBHEADER_CG_CANCEL;

	if (!Send(sizeof(TPacketCGAuraRefine), &Packet))
		return false;

	return SendSequence();
}
#endif

#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
#define ChangeLookPacket(x) TPacketCGChangeLook Packet(static_cast<std::underlying_type_t<EPacketCGChangeLookSubHeader>>(EPacketCGChangeLookSubHeader::x))

bool CPythonNetworkStream::SendChangeLookCheckInPacket(const TItemPos& c_rItemPos, const BYTE c_bSlotIndex)
{
	ChangeLookPacket(ITEM_CHECK_IN);
	Packet.bSlotIndex = c_bSlotIndex;
	Packet.ItemPos = c_rItemPos;
	if (!Send(sizeof(Packet), &Packet))
	{
		Tracef("CPythonNetworkStream::SendChangeLookCheckInPacket Error\n");
		return false;
	}
	return SendSequence();
}

bool CPythonNetworkStream::SendChangeLookCheckOutPacket(const BYTE c_bSlotIndex)
{
	ChangeLookPacket(ITEM_CHECK_OUT);
	Packet.bSlotIndex = c_bSlotIndex;
	if (!Send(sizeof(Packet), &Packet))
	{
		Tracef("CPythonNetworkStream::SendChangeLookCheckOutPacket Error\n");
		return false;
	}
	return SendSequence();
}

bool CPythonNetworkStream::SendChangeLookCheckInFreeYangItemPacket(const TItemPos& c_rItemPos)
{
	ChangeLookPacket(FREE_ITEM_CHECK_IN);
	Packet.ItemPos = c_rItemPos;
	if (!Send(sizeof(Packet), &Packet))
	{
		Tracef("CPythonNetworkStream::SendChangeLookCheckInFreeYangItemPacket Error\n");
		return false;
	}
	return SendSequence();
}

bool CPythonNetworkStream::SendChangeLookCheckOutFreeYangItemPacket()
{
	ChangeLookPacket(FREE_ITEM_CHECK_OUT);
	if (!Send(sizeof(Packet), &Packet))
	{
		Tracef("CPythonNetworkStream::SendChangeLookCheckOutFreeYangItemPacket Error\n");
		return false;
	}
	return SendSequence();
}

bool CPythonNetworkStream::SendChangeLookAcceptPacket()
{
	ChangeLookPacket(ACCEPT);
	if (!Send(sizeof(Packet), &Packet))
	{
		Tracef("CPythonNetworkStream::SendChangeLookAcceptPacket Error\n");
		return false;
	}
	return SendSequence();
}

bool CPythonNetworkStream::SendChangeLookCancelPacket()
{
	ChangeLookPacket(CANCEL);
	if (!Send(sizeof(Packet), &Packet))
	{
		Tracef("CPythonNetworkStream::SendChangeLookCancelPacket Error\n");
		return false;
	}
	return SendSequence();
}

bool CPythonNetworkStream::RecvChangeLookItemSetPacket()
{
	TPacketGCChangeLookSet kItemSet;
	if (!Recv(sizeof(kItemSet), &kItemSet))
		return false;

	if (kItemSet.bSlotIndex >= static_cast<BYTE>(EChangeLookSlots::CHANGE_LOOK_SLOT_MAX))
		return true;

	CPythonPlayer::Instance().SetChangeLookItemData(kItemSet);
	__RefreshInventoryWindow();

	return true;
}

bool CPythonNetworkStream::RecvChangeLookItemDelPacket()
{
	TPacketGCChangeLookDel kItemDel;
	if (!Recv(sizeof(kItemDel), &kItemDel))
		return false;

	CPythonPlayer::Instance().DelChangeLookItemData(kItemDel.bSlotIndex, true);
	__RefreshInventoryWindow();

	return true;
}

bool CPythonNetworkStream::RecvChangeLookFreeItemSetPacket()
{
	TPacketGCChangeLookSet kItemSet;
	if (!Recv(sizeof(kItemSet), &kItemSet))
		return false;

	CPythonPlayer::Instance().SetChangeLookFreeItemData(kItemSet);
	__RefreshInventoryWindow();

	return true;
}

bool CPythonNetworkStream::RecvChangeLookFreeItemDelPacket()
{
	TPacketGCChangeLookDel kItemDel;
	if (!Recv(sizeof(kItemDel), &kItemDel))
		return false;

	CPythonPlayer::Instance().DelChangeLookFreeItemData(true);
	__RefreshInventoryWindow();

	return true;
}
#endif

#if defined(ENABLE_MAILBOX)
bool CPythonNetworkStream::RecvMailboxProcess()
{
	TPacketMailboxProcess p;

	if (!Recv(sizeof(p), &p))
		return false;

	switch (p.bSubHeader)
	{
		case CPythonMailBox::EMAILBOX_GC::MAILBOX_GC_CLOSE:
			CPythonMailBox::Instance().Destroy();
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MailBoxProcess", Py_BuildValue("(ii)", p.bSubHeader, p.bArg1));
			break;
		case CPythonMailBox::EMAILBOX_GC::MAILBOX_GC_OPEN:
		case CPythonMailBox::EMAILBOX_GC::MAILBOX_GC_POST_WRITE_CONFIRM:
		case CPythonMailBox::EMAILBOX_GC::MAILBOX_GC_POST_WRITE:
		case CPythonMailBox::EMAILBOX_GC::MAILBOX_GC_POST_ALL_DELETE:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MailBoxProcess", Py_BuildValue("(ii)", p.bSubHeader, p.bArg1));
			break;
		case CPythonMailBox::EMAILBOX_GC::MAILBOX_GC_ADD_DATA:
		case CPythonMailBox::EMAILBOX_GC::MAILBOX_GC_POST_DELETE:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MailBoxProcess", Py_BuildValue("(i(ii))", p.bSubHeader, p.bArg1, p.bArg2));
			break;
		case CPythonMailBox::EMAILBOX_GC::MAILBOX_GC_POST_GET_ITEMS:
			CPythonMailBox::Instance().ResetAddData(p.bArg1);
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MailBoxProcess", Py_BuildValue("(i(ii))", p.bSubHeader, p.bArg1, p.bArg2));
			break;
		default:
			TraceError("CPythonNetworkStream::RecvMailboxProcess: Unknown subheader: %d\n", p.bSubHeader);
			break;
	}

	return true;
}

bool CPythonNetworkStream::RecvMailbox()
{
	TPacketGCMailBox p;
	if (!Recv(sizeof(p), &p))
		return false;

	CPythonMailBox::Instance().Destroy();

	unsigned int iPacketSize = (p.wSize - sizeof(TPacketGCMailBox));
	for (; iPacketSize > 0; iPacketSize -= sizeof(TPacketGCMailBoxMessage))
	{
		TPacketGCMailBoxMessage Message;
		if (!Recv(sizeof(Message), &Message))
			return false;

#if defined(ENABLE_LOCALE_CLIENT)
		if (Message.bIsGMPost)
			CPythonLocale::Instance().FormatString(Message.szTitle, sizeof(Message.szTitle));
#endif

		CPythonMailBox::Instance().AddMail(new CPythonMailBox::SMailBox(Message.SendTime, Message.DeleteTime, Message.szTitle, Message.bIsGMPost, Message.bIsItemExist, Message.bIsConfirm));
	}

	return true;
}

bool CPythonNetworkStream::RecvMailboxAddData()
{
	TPacketGCMailBoxAddData p;

	if (!Recv(sizeof(p), &p))
		return false;

	CPythonMailBox::SMailBox* _Data = CPythonMailBox::Instance().GetMail(p.Index);
	if (_Data == nullptr)
	{
		Tracef("RecvMailboxAddData Error: SMailBox is null.\n");
		return true;
	}

	CPythonMailBox::SMailBoxAddData*& _AddData = _Data->AddData;
	if (_AddData != nullptr)
	{
		Tracef("RecvMailboxAddData Error: SMailBoxAddData is not null.\n");
		return true;
	}

	_Data->bIsConfirm = true;
	_AddData = new CPythonMailBox::SMailBoxAddData(p.szFrom, p.szMessage, p.iYang, p.iWon, p.dwItemVnum, p.dwItemCount, p.alSockets, p.aAttr
#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
		, p.dwChangeLookVnum
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
		, &p.RefineElement
#endif
#if defined(ENABLE_APPLY_RANDOM)
		, p.aApplyRandom
#endif
#if defined(ENABLE_SET_ITEM)
		, p.bSetValue
#endif
	);
	return true;
}

bool CPythonNetworkStream::RecvMailboxAll()
{
	TPacketGCMailBox p;
	if (!Recv(sizeof(p), &p))
		return false;

	PyObject* list = PyList_New(0);

	unsigned int iPacketSize = (p.wSize - sizeof(TPacketGCMailBox));
	for (; iPacketSize > 0; iPacketSize -= sizeof(TPacketGCMailboxProcessAll))
	{
		TPacketGCMailboxProcessAll Index;
		if (!Recv(sizeof(Index), &Index))
			return false;

		CPythonMailBox::Instance().ResetAddData(Index.Index);
		PyList_Append(list, Py_BuildValue("i", Index.Index));
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MailBoxProcess", Py_BuildValue("i(iO)",
		CPythonMailBox::EMAILBOX_GC::MAILBOX_GC_POST_ALL_GET_ITEMS, CPythonMailBox::EMAILBOX_POST_ALL_GET_ITEMS::POST_ALL_GET_ITEMS_OK, list));

	Py_DECREF(list);
	return true;
}

bool CPythonNetworkStream::RecvMailboxUnread()
{
	TMailBoxRespondUnreadData p;

	if (!Recv(sizeof(p), &p))
		return false;

	const bool bFlash = p.bItemMessageCount > 0;
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MailBoxProcess", Py_BuildValue("i(iiiii)", CPythonMailBox::EMAILBOX_GC::MAILBOX_GC_UNREAD_DATA,
		bFlash, (p.bItemMessageCount + p.bCommonMessageCount), p.bItemMessageCount, p.bCommonMessageCount, p.bGMVisible));

	return true;
}

bool CPythonNetworkStream::SendPostWriteConfirm(const char* szName)
{
	TPacketCGMailboxWriteConfirm p;
	p.bHeader = HEADER_CG_MAILBOX_WRITE_CONFIRM;
	std::strcpy(p.szName, szName);

	if (!Send(sizeof(p), &p))
	{
		Tracef("SendPostWriteConfirm Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendMailBoxClose()
{
	TPacketMailboxProcess p;
	p.bHeader = HEADER_CG_MAILBOX_PROCESS;
	p.bSubHeader = CPythonMailBox::EMAILBOX_CG::MAILBOX_CG_CLOSE;

	if (!Send(sizeof(p), &p))
	{
		Tracef("SendMailBoxClose Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendPostDelete(const BYTE Index)
{
	TPacketMailboxProcess p;
	p.bHeader = HEADER_CG_MAILBOX_PROCESS;
	p.bSubHeader = CPythonMailBox::EMAILBOX_CG::MAILBOX_CG_DELETE;
	p.bArg1 = Index;

	if (!Send(sizeof(p), &p))
	{
		Tracef("SendPostDelete Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendPostAllDelete()
{
	TPacketMailboxProcess p;
	p.bHeader = HEADER_CG_MAILBOX_PROCESS;
	p.bSubHeader = CPythonMailBox::EMAILBOX_CG::MAILBOX_CG_ALL_DELETE;

	if (!Send(sizeof(p), &p))
	{
		Tracef("SendPostAllDelete Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendPostGetItems(const BYTE Index)
{
	TPacketMailboxProcess p;
	p.bHeader = HEADER_CG_MAILBOX_PROCESS;
	p.bSubHeader = CPythonMailBox::EMAILBOX_CG::MAILBOX_CG_GET_ITEMS;
	p.bArg1 = Index;

	if (!Send(sizeof(p), &p))
	{
		Tracef("SendPostGetItems Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendPostAllGetItems()
{
	TPacketMailboxProcess p;
	p.bHeader = HEADER_CG_MAILBOX_PROCESS;
	p.bSubHeader = CPythonMailBox::EMAILBOX_CG::MAILBOX_CG_ALL_GET_ITEMS;

	if (!Send(sizeof(p), &p))
	{
		Tracef("SendPostAllGetItems Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RequestPostAddData(const BYTE ButtonIndex, const BYTE DataIndex)
{
	TPacketMailboxProcess p;
	p.bHeader = HEADER_CG_MAILBOX_PROCESS;
	p.bSubHeader = CPythonMailBox::EMAILBOX_CG::MAILBOX_CG_ADD_DATA;
	p.bArg1 = ButtonIndex;
	p.bArg2 = DataIndex;

	if (!Send(sizeof(p), &p))
	{
		Tracef("RequestPostAddData Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendPostWrite(const char* szName, const char* szTitle, const char* szMessage, const TItemPos& pos, const int iYang, const int iWon)
{
	TPacketCGMailboxWrite p;
	p.bHeader = HEADER_CG_MAILBOX_WRITE;
	std::strcpy(p.szName, szName);
	std::strcpy(p.szTitle, szTitle);
	std::strcpy(p.szMessage, szMessage);
	p.pos = pos;
	p.iYang = iYang;
	p.iWon = iWon;

	if (!Send(sizeof(p), &p))
	{
		Tracef("SendPostWrite Error\n");
		return false;
	}

	return SendSequence();
}
#endif

#if defined(ENABLE_RANKING_SYSTEM)
#include "PythonRanking.h"
bool CPythonNetworkStream::RecvRanking()
{
	TPacketGCRanking Packet;
	if (!Recv(sizeof(Packet), &Packet))
		return false;

	CPythonRanking::Instance().Clear();

	for (unsigned int iPacketSize = Packet.wSize - sizeof(Packet); iPacketSize > 0; iPacketSize -= sizeof(SRankingData))
	{
		SRankingData RKData;
		if (Recv(sizeof(RKData), &RKData))
			CPythonRanking::Instance().Add(Packet.bType, Packet.bCategory, RKData);
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenRankingBoard", Py_BuildValue("(ii)", Packet.bType, Packet.bCategory));

	return true;
}
#endif

#if defined(ENABLE_MOVE_COSTUME_ATTR)
bool CPythonNetworkStream::SendItemCombinationPacket(const int iMediumIndex, const int iBaseIndex, const int iMaterialIndex)
{
	TPacketCGItemCombination p;
	p.Header = HEADER_CG_ITEM_COMBINATION;
	p.MediumIndex = iMediumIndex;
	p.BaseIndex = iBaseIndex;
	p.MaterialIndex = iMaterialIndex;

	if (!Send(sizeof(p), &p))
	{
		Tracef("SendItemCombinationPacket Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendItemCombinationPacketCancel()
{
	TPacketCGItemCombinationCancel p;
	p.Header = HEADER_CG_ITEM_COMBINATION_CANCEL;

	if (!Send(sizeof(p), &p))
	{
		Tracef("SendItemCombinationPacketCancel Error\n");
		return false;
	}

	return SendSequence();
}
#endif

#if defined(ENABLE_CHANGED_ATTR)
bool CPythonNetworkStream::RecvSelectAttr()
{
	TPacketGCItemSelectAttr p;

	if (!Recv(sizeof(p), &p))
		return false;

	CPythonPlayer::Instance().SetSelectAttr(p.aAttr);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenSelectAttrDialog", Py_BuildValue("(ii)", p.pItemPos.window_type, p.pItemPos.cell));

	return true;
}

bool CPythonNetworkStream::SendChagedItemAttributePacket(const bool bNew, const TItemPos& pos)
{
	TPacketCGItemSelectAttr p;
	p.bHeader = HEADER_CG_ITEM_SELECT_ATTR;
	p.bNew = bNew;
	p.pItemPos = pos;

	if (!Send(sizeof(p), &p))
	{
		Tracef("SendChagedItemAttributePacket Error\n");
		return false;
	}

	return SendSequence();
}
#endif

#if defined(ENABLE_LOOTING_SYSTEM)
bool CPythonNetworkStream::SendLootFilterPacket(BYTE(&settings)[ELootFilter::LOOT_FILTER_SETTINGS_MAX])
{
	TPacketCGLootFilter p;
	p.header = HEADER_CG_LOOT_FILTER;
	memcpy(p.settings, settings, sizeof(p.settings));

	if (!Send(sizeof(p), &p))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvLootFilterPacket()
{
	TPacketGCLootFilter p;
	if (!Recv(sizeof(p), &p))
		return false;

	if (p.enable)
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "LootingSystemProcess", Py_BuildValue("()"));
	else
		CPythonItem::Instance().InsertLootFilteredItem(p.vid);

	return true;
}
#endif

// MARK_BUG_FIX
void CPythonNetworkStream::RefreshGuildMark()
{
	__RefreshGuildWindowInfoPage();
}
// END_OF_MARK_BUG_FIX

#if defined(ENABLE_MINI_GAME_RUMI)
bool CPythonNetworkStream::RecvMiniGameRumi()
{
	std::vector<char> vPacketBuffer;
	vPacketBuffer.clear();

	TPacketGCMiniGameRumi Packet;
	if (!Recv(sizeof(Packet), &Packet))
		return false;

	unsigned int uiPacketSize = Packet.wSize - sizeof(Packet);
	if (uiPacketSize > 0)
	{
		vPacketBuffer.resize(uiPacketSize);
		if (!Recv(uiPacketSize, &vPacketBuffer[0]))
			return false;
	}

	switch (Packet.bSubHeader)
	{
		case RUMI_GC_SUBHEADER_END:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MiniGameRumiEnd", Py_BuildValue("()"));
			break;

		case RUMI_GC_SUBHEADER_START:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MiniGameRumiStart", Py_BuildValue("()"));
			break;

		case RUMI_GC_SUBHEADER_SET_DECK:
		{
			TPacketGCMiniGameRumiSetDeck* pkData = (TPacketGCMiniGameRumiSetDeck*)&vPacketBuffer[0];
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MiniGameRumiSetDeckCount", Py_BuildValue("(i)", pkData->bDeckCount));
		}
		break;

		case RUMI_GC_SUBHEADER_SET_SCORE:
		{
			TPacketGCMiniGameRumiSetScore* pkData = (TPacketGCMiniGameRumiSetScore*)&vPacketBuffer[0];
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MiniGameRumiIncreaseScore", Py_BuildValue("(ii)",
				pkData->wScore, pkData->wTotalScore));
		}
		break;

		case RUMI_GC_SUBHEADER_MOVE_CARD:
		{
			TPacketGCMiniGameRumiMoveCard* pkData = (TPacketGCMiniGameRumiMoveCard*)&vPacketBuffer[0];
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MiniGameRumiMoveCard", Py_BuildValue("(iiiiiiii)",
				pkData->bSrcPos, pkData->bSrcIndex, pkData->bSrcColor, pkData->bSrcNumber,
				pkData->bDstPos, pkData->bDstIndex, pkData->bDstColor, pkData->bDstNumber));
		}
		break;

#if defined(ENABLE_OKEY_EVENT_FLAG_RENEWAL)
		case RUMI_GC_SUBHEADER_SET_CARD_PIECE_FLAG:
		case RUMI_GC_SUBHEADER_SET_CARD_FLAG:
		case RUMI_GC_SUBHEADER_SET_QUEST_FLAG:
		case RUMI_GC_SUBHEADER_NO_MORE_GAIN:
		{
			TPacketGCMiniGameRumiQuestFlag* pkData = (TPacketGCMiniGameRumiQuestFlag*)&vPacketBuffer[0];
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MiniGameRumiFlagProcess", Py_BuildValue("(i(ii))",
				Packet.bSubHeader, pkData->wCardPieceCount, pkData->wCardCount));
		}
		break;
#endif
	}

	return true;
}

bool CPythonNetworkStream::SendMiniGameRumiExit()
{
	TPacketCGMiniGameRumi Packet;
	Packet.bHeader = HEADER_CG_MINI_GAME_RUMI;
	Packet.bSubHeader = RUMI_CG_SUBHEADER_END;
	if (!Send(sizeof(Packet), &Packet))
	{
		Tracef("SendMiniGameRumiExit Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendMiniGameRumiStart()
{
	TPacketCGMiniGameRumi Packet;
	Packet.bHeader = HEADER_CG_MINI_GAME_RUMI;
	Packet.bSubHeader = RUMI_CG_SUBHEADER_START;

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracef("SendMiniGameRumiStart Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendMiniGameRumiDeckCardClick()
{
	TPacketCGMiniGameRumi Packet;
	Packet.bHeader = HEADER_CG_MINI_GAME_RUMI;
	Packet.bSubHeader = RUMI_CG_SUBHEADER_DECK_CARD_CLICK;

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracef("SendMiniGameRumiDeckCardClick Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendMiniGameRumiHandCardClick(bool bUse, BYTE bIndex)
{
	TPacketCGMiniGameRumi Packet;
	Packet.bHeader = HEADER_CG_MINI_GAME_RUMI;
	Packet.bSubHeader = RUMI_CG_SUBHEADER_HAND_CARD_CLICK;
	Packet.bUseCard = bUse;
	Packet.bIndex = bIndex;

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracef("SendMiniGameRumiHandCardClick Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendMiniGameRumiFieldCardClick(const BYTE c_bIndex)
{
	TPacketCGMiniGameRumi Packet;
	Packet.bHeader = HEADER_CG_MINI_GAME_RUMI;
	Packet.bSubHeader = RUMI_CG_SUBHEADER_FIELD_CARD_CLICK;
	Packet.bIndex = c_bIndex;

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracef("SendMiniGameRumiFieldCardClick Error\n");
		return false;
	}

	return SendSequence();
}

#if defined(ENABLE_OKEY_EVENT_FLAG_RENEWAL)
bool CPythonNetworkStream::SendMiniGameRumiRequestQuestFlag()
{
	TPacketCGMiniGameRumi Packet;
	Packet.bHeader = HEADER_CG_MINI_GAME_RUMI;
	Packet.bSubHeader = RUMI_CG_SUBHEADER_REQUEST_QUEST_FLAG;

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracef("SendMiniGameRumiRequestQuestFlag Error\n");
		return false;
	}

	return SendSequence();
}
#endif
#endif

#if defined(ENABLE_GEM_SYSTEM)
bool CPythonNetworkStream::RecvGemShop()
{
	TPacketGCGemShop Packet;
	if (!Recv(sizeof(Packet), &Packet))
		return false;

	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
	rkPlayer.ClearGemShop();

	unsigned int uiPacketSize = (Packet.wSize - sizeof(TPacketGCGemShop));
	for (; uiPacketSize > 0; uiPacketSize -= sizeof(TGemShopTable))
	{
		TGemShopTable GemShopTable;
		if (!Recv(sizeof(GemShopTable), &GemShopTable))
			return false;

		rkPlayer.GemShop(GemShopTable);
	}

	return true;
}

bool CPythonNetworkStream::RecvGemShopProcess()
{
	TPacketGCGemShopProcess Packet;
	if (!Recv(sizeof(Packet), &Packet))
		return false;

	switch (Packet.bSubHeader)
	{
		case SUBHEADER_GEM_SHOP_CLOSE:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "CloseGemShop", Py_BuildValue("()"));
			break;
		case SUBHEADER_GEM_SHOP_OPEN:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenGemShop", Py_BuildValue("()"));
			break;
		case SUBHEADER_GEM_SHOP_BUY:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "GemShopSlotBuy", Py_BuildValue("(ib)", Packet.bSlotIndex, Packet.bEnable));
			break;
		case SUBHEADER_GEM_SHOP_SLOT_ADD:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "GemShopSlotAdd", Py_BuildValue("(ib)", Packet.bSlotIndex, Packet.bEnable));
			break;
		case SUBHEADER_GEM_SHOP_REFRESH:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGemShopWindow", Py_BuildValue("()"));
			break;
		default:
			TraceError("CPythonNetworkStream::RecvGemShopProcess: Unknown SubHeader: %d\n", Packet.bSubHeader);
			break;
	}

	return true;
}

bool CPythonNetworkStream::SendGemShopCommand(const BYTE c_bSubHeader, const BYTE c_bSlotIndex)
{
	if (c_bSubHeader > SUBHEADER_GEM_SHOP_MAX)
		return false;

	if (c_bSlotIndex > GEM_SHOP_SLOT_COUNT)
		return false;

	TPacketCGGemShop Packet(c_bSubHeader, c_bSlotIndex);
	if (!Send(sizeof(Packet), &Packet))
		return false;

	return SendSequence();
}
#endif

#if defined(ENABLE_CONQUEROR_LEVEL)
void CPythonNetworkStream::__SungMaAttrInfoProcess() const
{
	CPythonBackground& rkBg = CPythonBackground::Instance();
	if (!rkBg.HasSungMaAttr())
		return;

	const CMapBase::SSungmaAttr& data = rkBg.GetSungMaAttrData();
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SungMaAttr", Py_BuildValue("(iiii)", data.str, data.hp, data.move, data.immune));
}
#endif

#if defined(ENABLE_EXTEND_INVEN_SYSTEM)
bool CPythonNetworkStream::SendExtendInven(bool bUpgrade, BYTE bIndex)
{
	TPacketCGExtendInven Packet;
	Packet.bHeader = HEADER_CG_EXTEND_INVEN;
	Packet.bUpgrade = bUpgrade;
	Packet.bIndex = bIndex;
	if (!Send(sizeof(TPacketCGExtendInven), &Packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvExtendInven()
{
	TPacketGCExtendInven Packet;
	if (!Recv(sizeof(Packet), &Packet))
		return false;

	CPythonPlayer::Instance().SetExtendInvenStage(Packet.bStage);
	CPythonPlayer::Instance().SetExtendInvenMax(Packet.wMaxNum);

	__RefreshInventoryWindow();

	return true;
}

bool CPythonNetworkStream::RecvExtendInvenItemUse()
{
	TPacketGCExtendInvenItemUse Packet;
	if (!Recv(sizeof(Packet), &Packet))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ExInvenItemUseMsg",
		Py_BuildValue("(iii)", 0 /* unused item vnum */, Packet.bMsgResult, Packet.bEnoughCount));

	__RefreshStatus();
	__RefreshInventoryWindow();

	return true;
}
#endif

#if defined(ENABLE_CLIENT_TIMER)
bool CPythonNetworkStream::RecvClientTimer()
{
	TPacketGCClientTimer Packet;
	if (!Recv(sizeof(TPacketGCClientTimer), &Packet))
		return false;

	switch (Packet.bSubHeader)
	{
		case CLIENT_TIMER_SUBHEADER_GC_SET:
		{
			PyObject* poDict = PyDict_New();
			if (poDict == nullptr)
				return false;

			for (BYTE bDataIndex = 0; bDataIndex < ECLIENT_TIMER_MAX; ++bDataIndex)
				PyDict_SetItem(poDict, Py_BuildValue("i", bDataIndex), Py_BuildValue("i", Packet.dwData[bDataIndex]));

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ClientTimerProcess",
				Py_BuildValue("iO", CLIENT_TIMER_SUBHEADER_GC_SET, poDict));

			Py_DECREF(poDict);
		}
		break;

		case CLIENT_TIMER_SUBHEADER_GC_DELETE:
		default:
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ClientTimerProcess",
				Py_BuildValue("ii", CLIENT_TIMER_SUBHEADER_GC_DELETE, 0));
		}
		break;
	}

	return true;
}
#endif

bool CPythonNetworkStream::RecvEmote()
{
	TPacketGCEmote Packet;
	if (!Recv(sizeof(TPacketGCEmote), &Packet))
		return false;

	switch (Packet.bSubHeader)
	{
		case SUBHEADER_EMOTE_ADD:
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AddSpecialActionSlot",
				Py_BuildValue("ii", Packet.dwEmoteVnum, Packet.dwDuration));
		}
		break;

#if defined(ENABLE_EXPRESSING_EMOTION)
		case SUBHEADER_EMOTE_CLEAR:
		{
			for (int iIndex = SPECIAL_ACTION_START_INDEX; iIndex < EMOTION_NUM; ++iIndex)
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RemoveSpecialActionSlot", Py_BuildValue("(i)", iIndex));
		}
		break;
#endif

		case SUBHEADER_EMOTE_MOTION:
		{
			IAbstractCharacterManager& rkChrMgr = IAbstractCharacterManager::GetSingleton();
			CInstanceBase* pkInstMain = rkChrMgr.GetInstancePtr(Packet.dwMainVID);
			CInstanceBase* pkInstTarget = rkChrMgr.GetInstancePtr(Packet.dwTargetVID);
			if (pkInstMain == nullptr)
				return false;

			switch (Packet.dwEmoteVnum)
			{
				case EMOTION_CLAP:
					pkInstMain->ActEmotion(CRaceMotionData::NAME_CLAP);
					break;
				case EMOTION_CONGRATULATION:
					pkInstMain->ActEmotion(CRaceMotionData::NAME_CONGRATULATION);
					break;
				case EMOTION_FORGIVE:
					pkInstMain->ActEmotion(CRaceMotionData::NAME_FORGIVE);
					break;
				case EMOTION_ANGRY:
					pkInstMain->ActEmotion(CRaceMotionData::NAME_ANGRY);
					break;
				case EMOTION_ATTRACTIVE:
					pkInstMain->ActEmotion(CRaceMotionData::NAME_ATTRACTIVE);
					break;
				case EMOTION_SAD:
					pkInstMain->ActEmotion(CRaceMotionData::NAME_SAD);
					break;
				case EMOTION_SHY:
					pkInstMain->ActEmotion(CRaceMotionData::NAME_SHY);
					break;
				case EMOTION_CHEERUP:
					pkInstMain->ActEmotion(CRaceMotionData::NAME_CHEERUP);
					break;
				case EMOTION_BANTER:
					pkInstMain->ActEmotion(CRaceMotionData::NAME_BANTER);
					break;
				case EMOTION_JOY:
					pkInstMain->ActEmotion(CRaceMotionData::NAME_JOY);
					break;
				case EMOTION_CHEERS_1:
					pkInstMain->ActEmotion(CRaceMotionData::NAME_CHEERS_1);
					break;
				case EMOTION_CHEERS_2:
					pkInstMain->ActEmotion(CRaceMotionData::NAME_CHEERS_2);
					break;
				case EMOTION_DANCE_1:
					pkInstMain->ActEmotion(CRaceMotionData::NAME_DANCE_1);
					break;
				case EMOTION_DANCE_2:
					pkInstMain->ActEmotion(CRaceMotionData::NAME_DANCE_2);
					break;
				case EMOTION_DANCE_3:
					pkInstMain->ActEmotion(CRaceMotionData::NAME_DANCE_3);
					break;
				case EMOTION_DANCE_4:
					pkInstMain->ActEmotion(CRaceMotionData::NAME_DANCE_4);
					break;
				case EMOTION_DANCE_5:
					pkInstMain->ActEmotion(CRaceMotionData::NAME_DANCE_5);
					break;
				case EMOTION_DANCE_6:
					pkInstMain->ActEmotion(CRaceMotionData::NAME_DANCE_6);
					break;

				case EMOTION_KISS:
					if (pkInstTarget)
						pkInstMain->ActDualEmotion(*pkInstTarget, CRaceMotionData::NAME_KISS_START, CRaceMotionData::NAME_KISS_START);
					break;
				case EMOTION_FRENCH_KISS:
					if (pkInstTarget)
						pkInstMain->ActDualEmotion(*pkInstTarget, CRaceMotionData::NAME_FRENCH_KISS_START, CRaceMotionData::NAME_FRENCH_KISS_START);
					break;
				case EMOTION_SLAP:
					if (pkInstTarget)
						pkInstMain->ActDualEmotion(*pkInstTarget, CRaceMotionData::NAME_SLAP_HURT_START, CRaceMotionData::NAME_SLAP_HIT_START);
					break;

#if defined(ENABLE_EXPRESSING_EMOTION)
				case EMOTION_PUSH_UP:
					pkInstMain->ActEmotion(CRaceMotionData::NAME_PUSH_UP);
					break;
				case EMOTION_DANCE_7:
					pkInstMain->ActEmotion(CRaceMotionData::NAME_DANCE_7);
					break;
				case EMOTION_EXERCISE:
					pkInstMain->ActEmotion(CRaceMotionData::NAME_EXERCISE);
					break;
				case EMOTION_DOZE:
					pkInstMain->ActEmotion(CRaceMotionData::NAME_DOZE);
					break;
				case EMOTION_SELFIE:
					pkInstMain->ActEmotion(CRaceMotionData::NAME_SELFIE);
					break;

				case EMOTION_CHARGING:
				case EMOTION_NOSAY:
				case EMOTION_WEATHER_1:
				case EMOTION_WEATHER_2:
				case EMOTION_WEATHER_3:
				case EMOTION_HUNGRY:
				case EMOTION_SIREN:
				case EMOTION_LETTER:
				case EMOTION_CALL:
				case EMOTION_CELEBRATION:
				case EMOTION_ALCOHOL:
				case EMOTION_BUSY:
				case EMOTION_WHIRL:
				{
					DWORD dwEmoticon;
					if (ParseEmoticon(Packet.dwEmoteVnum, &dwEmoticon))
						pkInstMain->SetEmoticon(dwEmoticon);
				}
				break;
#endif
			}
		}
		break;

		case SUBHEADER_EMOTE_ICON:
		{
			IAbstractCharacterManager& rkChrMgr = IAbstractCharacterManager::GetSingleton();
			CInstanceBase* pkInstMain = rkChrMgr.GetInstancePtr(Packet.dwMainVID);
			CInstanceBase* pkInstTarget = rkChrMgr.GetInstancePtr(Packet.dwTargetVID);
			if (pkInstMain == nullptr)
				return false;

			DWORD dwEmoticon;
			if (ParseEmoticon(Packet.dwEmoteVnum, &dwEmoticon))
				pkInstMain->SetEmoticon(dwEmoticon);
		}
		break;
	}

	return true;
}

#if defined(ENABLE_CUBE_RENEWAL)
bool CPythonNetworkStream::SendCubeClosePacket()
{
	TPacketCGCube Packet;
	Packet.bHeader = HEADER_CG_CUBE;
	Packet.bSubHeader = SUBHEADER_CG_CUBE_CLOSE;
	Packet.dwFileCrc = CPythonCubeManager::Instance().GetFileCrc();
	if (!Send(sizeof(TPacketCGCube), &Packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendCubeMakePacket(const UINT iCubeIndex, const UINT iQuantity, const INT iImproveItemPos)
{
	TPacketCGCube Packet;
	Packet.bHeader = HEADER_CG_CUBE;
	Packet.bSubHeader = SUBHEADER_CG_CUBE_MAKE;
	Packet.iCubeIndex = iCubeIndex;
	Packet.iQuantity = iQuantity;
	Packet.iImproveItemPos = iImproveItemPos;
	Packet.dwFileCrc = CPythonCubeManager::Instance().GetFileCrc();
	if (!Send(sizeof(TPacketCGCube), &Packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvCubePacket()
{
	TPacketGCCube Packet;
	if (!Recv(sizeof(TPacketGCCube), &Packet))
		return false;

	if (Packet.bSubHeader != SUBHEADER_GC_CUBE_CLOSE && Packet.dwFileCrc != CPythonCubeManager::Instance().GetFileCrc())
	{
		Tracef("CPythonNetworkStream::RecvCubePacket() - File Mismatch");
		return false;
	}

	switch (Packet.bSubHeader)
	{
		case SUBHEADER_GC_CUBE_OPEN:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Cube_Open", Py_BuildValue("(i)", Packet.dwNPCVnum));
			break;
		case SUBHEADER_GC_CUBE_CLOSE:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Cube_Close", Py_BuildValue("()"));
			break;
		case SUBHEADER_GC_CUBE_RESULT:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Cube_Result", Py_BuildValue("(b)", Packet.bSuccess));
			break;
	}

	return true;
}
#endif

#if defined(ENABLE_SNOWFLAKE_STICK_EVENT)
bool CPythonNetworkStream::SendSnowflakeStickEventPacket(BYTE bSubHeader)
{
	TPacketCGSnowflakeStickEvent Packet(bSubHeader);
	if (!Send(sizeof(TPacketCGSnowflakeStickEvent), &Packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvSnowflakeStickEventPacket()
{
	TPacketGCSnowflakeStickEvent Packet;
	if (!Recv(sizeof(TPacketGCSnowflakeStickEvent), &Packet))
		return false;

	switch (Packet.bSubHeader)
	{
		case SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_EVENT_INFO:
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SnowflakeStickEventProcess",
				Py_BuildValue("(i(iiiiii))", Packet.bSubHeader,
					Packet.dwValue[SNOWFLAKE_STICK_EVENT_QUEST_FLAG_SNOW_BALL],
					Packet.dwValue[SNOWFLAKE_STICK_EVENT_QUEST_FLAG_TREE_BRANCH],
					Packet.dwValue[SNOWFLAKE_STICK_EVENT_QUEST_FLAG_EXCHANGE_STICK],
					Packet.dwValue[SNOWFLAKE_STICK_EVENT_QUEST_FLAG_EXCHANGE_STICK_COOLDOWN],
					Packet.dwValue[SNOWFLAKE_STICK_EVENT_QUEST_FLAG_EXCHANGE_PET],
					Packet.dwValue[SNOWFLAKE_STICK_EVENT_QUEST_FLAG_EXCHANGE_MOUNT]));
		}
		break;

		case SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_ENABLE:
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SnowflakeStickEventProcess",
				Py_BuildValue("(ii)", Packet.bSubHeader, Packet.dwValue[0]));
		}
		break;

		case SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_MESSAGE_USE_STICK_FAILED:
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SnowflakeStickEventProcess",
				Py_BuildValue("(ii)", Packet.bSubHeader, Packet.dwValue[0]));
		}
		break;

		default:
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SnowflakeStickEventProcess",
				Py_BuildValue("(i())", Packet.bSubHeader));
		}
		break;
	}

	return true;
}
#endif

#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
bool CPythonNetworkStream::SendRefineElementPacket(BYTE bSubHeader, WORD wElementType)
{
	TPacketCGRefineElement kPacket(bSubHeader, wElementType);
	if (!Send(sizeof(TPacketCGRefineElement), &kPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvRefineElementPacket()
{
	TPacketGCRefineElement kPacket;
	if (!Recv(sizeof(TPacketGCRefineElement), &kPacket))
		return false;

	switch (kPacket.bSubHeader)
	{
		case REFINE_ELEMENT_GC_OPEN:
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefineElementProcess",
				Py_BuildValue("(ii(ii))", kPacket.bSubHeader, kPacket.bRefineType,
					kPacket.SrcPos.cell, kPacket.DestPos.cell
				));
		}
		break;
		case REFINE_ELEMENT_GC_RESULT:
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefineElementProcess",
				Py_BuildValue("(iib)", kPacket.bSubHeader, kPacket.bRefineType, kPacket.bResult));
		}
		break;
	}

	return true;
}
#endif

#if defined(ENABLE_MINI_GAME_YUTNORI)
bool CPythonNetworkStream::RecvMiniGameYutnori()
{
	std::vector<char> vPacketBuffer;
	vPacketBuffer.clear();

	TPacketGCMiniGameYutnori Packet;
	if (!Recv(sizeof(Packet), &Packet))
		return false;

	unsigned int uiPacketSize = Packet.wSize - sizeof(Packet);
	if (uiPacketSize > 0)
	{
		vPacketBuffer.resize(uiPacketSize);
		if (!Recv(uiPacketSize, &vPacketBuffer[0]))
			return false;
	}

	switch (Packet.bSubHeader)
	{
		case YUTNORI_GC_SUBHEADER_START:
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "YutnoriProcess", Py_BuildValue("(ii)",
				Packet.bSubHeader, 0));
		}
		break;

		case YUTNORI_GC_SUBHEADER_STOP:
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "YutnoriProcess", Py_BuildValue("(ii)",
				Packet.bSubHeader, 0));
		}
		break;

		case YUTNORI_GC_SUBHEADER_SET_PROB:
		{
			TPacketGCMiniGameYutnoriSetProb* pPacket = (TPacketGCMiniGameYutnoriSetProb*)&vPacketBuffer[0];
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "YutnoriProcess", Py_BuildValue("(ii)",
				Packet.bSubHeader, pPacket->bProbIndex));
		}
		break;

		case YUTNORI_GC_SUBHEADER_THROW:
		{
			TPacketGCMiniGameYutnoriThrowYut* pPacket = (TPacketGCMiniGameYutnoriThrowYut*)&vPacketBuffer[0];
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "YutnoriProcess", Py_BuildValue("(i(ii))",
				Packet.bSubHeader, pPacket->bPC, pPacket->bYut));
		}
		break;

		case YUTNORI_GC_SUBHEADER_MOVE:
		{
			TPacketGCMiniGameYutnoriMoveYut* pPacket = (TPacketGCMiniGameYutnoriMoveYut*)&vPacketBuffer[0];
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "YutnoriProcess", Py_BuildValue("(i(iiiii))",
				Packet.bSubHeader, pPacket->bPC, pPacket->bUnitIndex, pPacket->bIsCatch, pPacket->bStartIndex, pPacket->bDestIndex));
		}
		break;

		case YUTNORI_GC_SUBHEADER_AVAILABLE_AREA:
		{
			TPacketGCMiniGameYutnoriAvailableArea* pPacket = (TPacketGCMiniGameYutnoriAvailableArea*)&vPacketBuffer[0];
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "YutnoriProcess", Py_BuildValue("(i(ii))",
				Packet.bSubHeader, pPacket->bPlayerIndex, pPacket->bAvailableIndex));
		}
		break;

		case YUTNORI_GC_SUBHEADER_PUSH_CATCH_YUT:
		{
			TPacketGCMiniGameYutnoriPushCatchYut* pPacket = (TPacketGCMiniGameYutnoriPushCatchYut*)&vPacketBuffer[0];
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "YutnoriProcess", Py_BuildValue("(i(ii))",
				Packet.bSubHeader, pPacket->bPC, pPacket->bUnitIndex));
		}
		break;

		case YUTNORI_GC_SUBHEADER_SET_SCORE:
		{
			TPacketGCMiniGameYutnoriSetScore* pPacket = (TPacketGCMiniGameYutnoriSetScore*)&vPacketBuffer[0];
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "YutnoriProcess", Py_BuildValue("(ii)",
				Packet.bSubHeader, pPacket->wScore));
		}
		break;

		case YUTNORI_GC_SUBHEADER_SET_REMAIN_COUNT:
		{
			TPacketGCMiniGameYutnoriSetRemainCount* pPacket = (TPacketGCMiniGameYutnoriSetRemainCount*)&vPacketBuffer[0];
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "YutnoriProcess", Py_BuildValue("(ii)",
				Packet.bSubHeader, pPacket->bRemainCount));
		}
		break;

		case YUTNORI_GC_SUBHEADER_PUSH_NEXT_TURN:
		{
			TPacketGCMiniGameYutnoriPushNextTurn* pPacket = (TPacketGCMiniGameYutnoriPushNextTurn*)&vPacketBuffer[0];
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "YutnoriProcess", Py_BuildValue("(i(ii))",
				Packet.bSubHeader, pPacket->bPC, pPacket->bState));
		}
		break;

#if defined(ENABLE_YUTNORI_EVENT_FLAG_RENEWAL)
		case YUTNORI_GC_SUBHEADER_SET_YUT_PIECE_FLAG:
		case YUTNORI_GC_SUBHEADER_SET_YUT_BOARD_FLAG:
		case YUTNORI_GC_SUBHEADER_SET_QUEST_FLAG:
		case YUTNORI_GC_SUBHEADER_NO_MORE_GAIN:
		{
			TPacketGCMiniGameYutnoriQuestFlag* pPacket = (TPacketGCMiniGameYutnoriQuestFlag*)&vPacketBuffer[0];
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "YutnoriFlagProcess", Py_BuildValue("(i(ii))",
				Packet.bSubHeader, pPacket->wYutPieceCount, pPacket->wYutBoardCount));
		}
		break;
#endif
	}

	return true;
}

bool CPythonNetworkStream::SendMiniGameYutnoriStart()
{
	TPacketCGMiniGameYutnori Packet;
	Packet.bHeader = HEADER_CG_MINI_GAME_YUTNORI;
	Packet.bSubHeader = YUTNORI_CG_SUBHEADER_START;

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracef("SendMiniGameYutnoriStart Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendMiniGameYutnoriGiveup()
{
	TPacketCGMiniGameYutnori Packet;
	Packet.bHeader = HEADER_CG_MINI_GAME_YUTNORI;
	Packet.bSubHeader = YUTNORI_CG_SUBHEADER_GIVEUP;

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracef("SendMiniGameYutnoriGiveup Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendMiniGameYutnoriProb(BYTE bProbIndex)
{
	TPacketCGMiniGameYutnori Packet;
	Packet.bHeader = HEADER_CG_MINI_GAME_YUTNORI;
	Packet.bSubHeader = YUTNORI_CG_SUBHEADER_SET_PROB;
	Packet.bArgument = bProbIndex;

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracef("SendMiniGameYutnoriProb Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendMiniGameYutnoriCharClick(BYTE bPlayerIndex)
{
	TPacketCGMiniGameYutnori Packet;
	Packet.bHeader = HEADER_CG_MINI_GAME_YUTNORI;
	Packet.bSubHeader = YUTNORI_CG_SUBHEADER_CLICK_CHAR;
	Packet.bArgument = bPlayerIndex;

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracef("SendMiniGameYutnoriCharClick Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendMiniGameYutnoriThrow(BYTE bPC)
{
	TPacketCGMiniGameYutnori Packet;
	Packet.bHeader = HEADER_CG_MINI_GAME_YUTNORI;
	Packet.bSubHeader = YUTNORI_CG_SUBHEADER_THROW;
	Packet.bArgument = bPC;

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracef("SendMiniGameYutnoriThrow Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendMiniGameYutnoriMove(BYTE bPlayerIndex)
{
	TPacketCGMiniGameYutnori Packet;
	Packet.bHeader = HEADER_CG_MINI_GAME_YUTNORI;
	Packet.bSubHeader = YUTNORI_CG_SUBHEADER_MOVE;
	Packet.bArgument = bPlayerIndex;

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracef("SendMiniGameYutnoriMove Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendMiniGameYutnoriReward()
{
	TPacketCGMiniGameYutnori Packet;
	Packet.bHeader = HEADER_CG_MINI_GAME_YUTNORI;
	Packet.bSubHeader = YUTNORI_CG_SUBHEADER_REWARD;

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracef("SendMiniGameYutnoriReward Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendMiniGameYutnoriRequestComAction()
{
	TPacketCGMiniGameYutnori Packet;
	Packet.bHeader = HEADER_CG_MINI_GAME_YUTNORI;
	Packet.bSubHeader = YUTNORI_CG_SUBHEADER_REQUEST_COM_ACTION;

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracef("SendMiniGameYutnoriRequestComAction Error\n");
		return false;
	}

	return SendSequence();
}

#if defined(ENABLE_YUTNORI_EVENT_FLAG_RENEWAL)
bool CPythonNetworkStream::SendMiniGameYutnoriRequestQuestFlag()
{
	TPacketCGMiniGameYutnori Packet;
	Packet.bHeader = HEADER_CG_MINI_GAME_YUTNORI;
	Packet.bSubHeader = YUTNORI_CG_SUBHEADER_REQUEST_QUEST_FLAG;

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracef("SendMiniGameYutnoriRequestQuestFlag Error\n");
		return false;
	}

	return SendSequence();
}
#endif
#endif

#if defined(ENABLE_QUEST_REQUEST_EVENT)
bool CPythonNetworkStream::SendRequestEventQuest(const char* pszEventQuest)
{
	TPacketCGRequestEventQuest Packet;
	Packet.bHeader = HEADER_CG_REQUEST_EVENT_QUEST;
	std::strcpy(Packet.szEventQuest, pszEventQuest);

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracef("SendRequestEventQuest Error\n");
		return false;
	}

	return SendSequence();
}
#endif

#if defined(ENABLE_LEFT_SEAT)
bool CPythonNetworkStream::SendLeftSeatWaitTimeIndexPacket(BYTE bIndex)
{
	TPacketCGLeftSeat Packet;
	Packet.bHeader = HEADER_CG_LEFT_SEAT;
	Packet.bSubHeader = LEFT_SEAT_SET_WAIT_TIME_INDEX;
	Packet.bIndex = bIndex;

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracef("SendLeftSeatWaitTimeIndexPacket Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendLeftSeatOffPacket(BYTE bIndex)
{
	TPacketCGLeftSeat Packet;
	Packet.bHeader = HEADER_CG_LEFT_SEAT;
	Packet.bSubHeader = LEFT_SEAT_SET_LOGOUT_TIME_INDEX;
	Packet.bIndex = bIndex;

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracef("SendLeftSeatOffPacket Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::DisableLeftSeatLogOutState()
{
	TPacketCGLeftSeat Packet;
	Packet.bHeader = HEADER_CG_LEFT_SEAT;
	Packet.bSubHeader = LEFT_SEAT_DISABLE_LOGOUT_STATE;
	Packet.bIndex = 0;

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracef("DisableLeftSeatLogOutState Error\n");
		return false;
	}

	return SendSequence();
}
#endif

#if defined(ENABLE_GUILD_DRAGONLAIR_SYSTEM)
bool CPythonNetworkStream::RecvGuildDragonLairPacket()
{
	TPacketGCGuildDragonLair GuildDragonLairPacket;
	if (!Recv(sizeof(TPacketGCGuildDragonLair), &GuildDragonLairPacket))
		return false;

	switch (GuildDragonLairPacket.bSubHeader)
	{
		case GUILD_DRAGONLAIR_GC_SUBHEADER_RANKING:
		{
			CPythonGuild::Instance().ClearDragonLairRanking(GuildDragonLairPacket.bType);

			unsigned int iPacketSize = (GuildDragonLairPacket.wSize - sizeof(TPacketGCGuildDragonLair));
			for (; iPacketSize > 0; iPacketSize -= sizeof(TPacketGCGuildDragonLairRanking))
			{
				TPacketGCGuildDragonLairRanking GuildDragonLairRankingPacket;
				if (!Recv(sizeof(TPacketGCGuildDragonLairRanking), &GuildDragonLairRankingPacket))
					return false;

				CPythonGuild::Instance().RegisterDragonLairRanking(
					GuildDragonLairRankingPacket.bType,
					GuildDragonLairRankingPacket.dwGuildID,
					GuildDragonLairRankingPacket.szGuildName,
					GuildDragonLairRankingPacket.bMemberCount,
					GuildDragonLairRankingPacket.dwTime
				);
			}

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenGuildDragonLairRanking", Py_BuildValue("(i)", GuildDragonLairPacket.bType));
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildDragonLairRanking", Py_BuildValue("(i)", GuildDragonLairPacket.bType));
		}
		break;

#if defined(ENABLE_GUILD_DRAGONLAIR_PARTY_SYSTEM)
		case GUILD_DRAGONLAIR_GC_SUBHEADER_1ST_GUILD_TEXT:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetGuildDragonLairFistGuildText", Py_BuildValue("(i)", GuildDragonLairPacket.dwSeconds));
			break;

		case GUILD_DRAGONLAIR_GC_SUBHEADER_START:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetGuildDragonLairStart", Py_BuildValue("()"));
			break;

		case GUILD_DRAGONLAIR_GC_SUBHEADER_SUCCESS:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetGuildDragonLairSuccess", Py_BuildValue("()"));
			break;
#endif
	}

	return true;
}
#endif

#if defined(ENABLE_SUMMER_EVENT_ROULETTE)
bool CPythonNetworkStream::SendMiniGameRoulettePacket(BYTE bSubHeader)
{
	TPacketCGMiniGameRoulette MiniGameRoulettePacket;
	MiniGameRoulettePacket.bHeader = HEADER_CG_MINI_GAME_ROULETTE;
	MiniGameRoulettePacket.bSubHeader = bSubHeader;

	if (!Send(sizeof(MiniGameRoulettePacket), &MiniGameRoulettePacket))
	{
		Tracef("SendMiniGameRoulettePacket Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvMiniGameRoulettePacket()
{
	TPacketGCMiniGameRoulette MiniGameRoulettePacket;
	if (!Recv(sizeof(TPacketGCMiniGameRoulette), &MiniGameRoulettePacket))
		return false;

	switch (MiniGameRoulettePacket.bSubHeader)
	{
		case ROULETTE_GC_OPEN:
		{
			PyObject* poTuple = PyTuple_New(ROULETTE_ITEM_MAX);
			if (poTuple == NULL)
				return false;

			for (BYTE bSlotIdx = 0; bSlotIdx < ROULETTE_ITEM_MAX; bSlotIdx++)
			{
				PyObject* poItemTuple = PyTuple_New(2);
				if (poItemTuple == NULL)
					break;

				PyTuple_SetItem(poItemTuple, 0, PyInt_FromLong(MiniGameRoulettePacket.ItemData[bSlotIdx].dwVnum));
				PyTuple_SetItem(poItemTuple, 1, PyInt_FromLong(MiniGameRoulettePacket.ItemData[bSlotIdx].bCount));
				PyTuple_SetItem(poTuple, bSlotIdx, poItemTuple);
			}

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RouletteProcess",
				Py_BuildValue("(i(iO))", ROULETTE_GC_OPEN, MiniGameRoulettePacket.dwExpireTime, poTuple));
		}
		break;

		case ROULETTE_GC_START:
		case ROULETTE_GC_END:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RouletteProcess",
				Py_BuildValue("(ii)", MiniGameRoulettePacket.bSubHeader, MiniGameRoulettePacket.bResult));
			break;

		case ROULETTE_GC_CLOSE:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RouletteProcess",
				Py_BuildValue("(i)", ROULETTE_GC_CLOSE));
			break;
	}

	return true;
}
#endif

#if defined(ENABLE_FLOWER_EVENT)
bool CPythonNetworkStream::SendFlowerEventPacket(BYTE bSubHeader, BYTE bShootType, BYTE bExchangeKey)
{
	if (CPythonPlayer::Instance().GetFlowerEventEnable() == 0)
		return false;

	TPacketCGFlowerEvent FlowerEventPacket;
	FlowerEventPacket.bHeader = HEADER_CG_FLOWER_EVENT;
	FlowerEventPacket.bSubHeader = bSubHeader;
	FlowerEventPacket.bShootType = bShootType;
	FlowerEventPacket.bExchangeKey = bExchangeKey;

	if (!Send(sizeof(FlowerEventPacket), &FlowerEventPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvFlowerEventPacket()
{
	TPacketGCFlowerEvent FlowerEventPacket;
	if (!Recv(sizeof(TPacketGCFlowerEvent), &FlowerEventPacket))
		return false;

	switch (FlowerEventPacket.bSubHeader)
	{
		case FLOWER_EVENT_SUBHEADER_GC_INFO_ALL:
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "FlowerEventProcess",
				Py_BuildValue("(i(iiiiii))", FLOWER_EVENT_SUBHEADER_GC_INFO_ALL
					, FlowerEventPacket.aiShootCount[SHOOT_ENVELOPE]
					, FlowerEventPacket.aiShootCount[SHOOT_CHRYSANTHEMUM]
					, FlowerEventPacket.aiShootCount[SHOOT_MAY_BELL]
					, FlowerEventPacket.aiShootCount[SHOOT_DAFFODIL]
					, FlowerEventPacket.aiShootCount[SHOOT_LILY]
					, FlowerEventPacket.aiShootCount[SHOOT_SUNFLOWER]
				));
		}
		break;

		case FLOWER_EVENT_SUBHEADER_GC_GET_INFO:
		{
			if (FlowerEventPacket.bShootType == SHOOT_TYPE_MAX)
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "FlowerEventProcess",
					Py_BuildValue("ii", FLOWER_EVENT_SUBHEADER_GC_GET_INFO, FlowerEventPacket.bChatType));
			}
			else
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "FlowerEventProcess",
					Py_BuildValue("i(ii)", FLOWER_EVENT_SUBHEADER_GC_GET_INFO,
						FlowerEventPacket.bShootType, FlowerEventPacket.aiShootCount[FlowerEventPacket.bShootType]));
			}
		}
		break;

		case FLOWER_EVENT_SUBHEADER_GC_UPDATE_INFO:
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "FlowerEventProcess",
				Py_BuildValue("i(ii)", FLOWER_EVENT_SUBHEADER_GC_UPDATE_INFO,
					FlowerEventPacket.bShootType, FlowerEventPacket.aiShootCount[FlowerEventPacket.bShootType]));
		}
		break;
	}

	return true;
}
#endif
