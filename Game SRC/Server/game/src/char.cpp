#include "stdafx.h"

#include "../../common/VnumHelper.h"

#include "char.h"
#include "config.h"
#include "utils.h"
#include "crc32.h"
#include "char_manager.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "buffer_manager.h"
#include "item_manager.h"
#include "motion.h"
#include "vector.h"
#include "packet.h"
#include "cmd.h"
#include "fishing.h"
#include "exchange.h"
#include "battle.h"
#include "affect.h"
#include "shop.h"
#include "shop_manager.h"
#include "safebox.h"
#include "regen.h"
#include "pvp.h"
#include "party.h"
#include "start_position.h"
#include "questmanager.h"
#include "log.h"
#include "p2p.h"
#include "guild.h"
#include "guild_manager.h"
#include "dungeon.h"
#include "messenger_manager.h"
#include "unique_item.h"
#include "unique_mob.h"
#include "priv_manager.h"
#include "war_map.h"
#include "xmas_event.h"
#include "banword.h"
#include "target.h"
#include "wedding.h"
#include "mob_manager.h"
#include "mining.h"
#include "monarch.h"
#include "castle.h"
#include "arena.h"
#include "dev_log.h"
#include "horsename_manager.h"
#include "pcbang.h"
#include "gm.h"
#include "map_location.h"
#include "BlueDragon_Binder.h"
#include "skill_power.h"
#include "buff_on_attributes.h"
#include "threeway_war.h"

#if defined(__PET_SYSTEM__)
#	include "PetSystem.h"
#endif
#include "DragonSoul.h"
#ifdef ENABLE_QUEEN_NETHIS
#include "SnakeLair.h"
#endif
#if defined(__LOOT_FILTER_SYSTEM__)
#	include "LootFilter.h"
#endif

#include "BlueDragon.h"
#if defined(__GUILD_DRAGONLAIR_SYSTEM__)
#	include "guild_dragonlair.h"
#endif
#if defined(__DEFENSE_WAVE__)
#	include "defense_wave.h"
#endif

extern const POINT_TYPE g_aBuffOnAttrPoints;
extern bool RaceToJob(BYTE race, BYTE* ret_job);

extern int g_nPortalLimitTime;
extern int test_server;

bool CAN_ENTER_ZONE(const LPCHARACTER& ch, int map_index)
{
	switch (map_index)
	{
		case MAP_CAPEDRAGONHEAD:
		case MAP_DAWNMISTWOOD:
		case MAP_BAYBLACKSAND:
		case MAP_MT_THUNDER:
			if (ch->GetLevel() < 90)
				return false;
	}
	return true;
}

// <Factor> DynamicCharacterPtr member function definitions
LPCHARACTER DynamicCharacterPtr::Get() const
{
	LPCHARACTER p = NULL;
	if (is_pc)
		p = CHARACTER_MANAGER::instance().FindByPID(id);
	else
		p = CHARACTER_MANAGER::instance().Find(id);

	return p;
}

DynamicCharacterPtr& DynamicCharacterPtr::operator=(LPCHARACTER character)
{
	if (character == NULL)
	{
		Reset();
		return *this;
	}

	if (character->IsPC())
	{
		is_pc = true;
		id = character->GetPlayerID();
	}
	else
	{
		is_pc = false;
		id = character->GetVID();
	}

	return *this;
}

CHARACTER::CHARACTER()
{
	m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateIdle, &CHARACTER::EndStateEmpty);
	m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateMove, &CHARACTER::EndStateEmpty);
	m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateBattle, &CHARACTER::EndStateEmpty);

	Initialize();
}

CHARACTER::~CHARACTER()
{
	Destroy();
}

void CHARACTER::Initialize()
{
	m_stName = "";

	CEntity::Initialize(ENTITY_CHARACTER);

#if defined(__LEFT_SEAT__)
	m_bLeftSeat = false;

	m_dwLeftSeatWaitTime = false;
	m_pLeftSeatWaitTimerEvent = NULL;

	m_dwLeftSeatLogoutTime = false;
	m_pLeftSeatLogoutTimerEvent = NULL;

	m_dwLastRequestTime = 0;
#endif

#if defined(__REFINE_ELEMENT_SYSTEM__)
	SetUnderRefineElement(false);
#endif

#if defined(__EXTEND_INVEN_SYSTEM__)
	m_bExtendInvenStage = 0;
#endif

#if defined(__ACCE_COSTUME_SYSTEM__)
	m_bAcceRefineWindowType = ACCE_SLOT_TYPE_MAX;
	m_bAcceRefineWindowOpen = false;
	for (BYTE bSlotIndex = ACCE_SLOT_LEFT; bSlotIndex < ACCE_SLOT_MAX; bSlotIndex++)
		m_pAcceRefineWindowItemSlot[bSlotIndex] = NPOS;
#endif

#if defined(__AURA_COSTUME_SYSTEM__)
	m_bAuraRefineWindowType = AURA_WINDOW_TYPE_MAX;
	m_bAuraRefineWindowOpen = false;
	for (BYTE i = AURA_SLOT_MAIN; i < AURA_SLOT_MAX; i++)
		m_pAuraRefineWindowItemSlot[i] = NPOS;

	std::memset(&m_bAuraRefineInfo, 0, AURA_REFINE_INFO_SLOT_MAX * sizeof(TAuraRefineInfo));
#endif

#if defined(__FISHING_GAME__)
	SetFishingGameGoals(0);
#endif

#if defined(__ATTR_6TH_7TH__)
	m_bIsOpenAttr67Add = false;
#endif

#if defined(__GEM_SHOP__)
	m_pGemShop = nullptr;
	m_bGemShopLoading = false;
#endif

#if defined(__MAILBOX__)
	m_pkMailBox = nullptr;
	bMailBoxLoading = false;
	m_iMyMailBoxTime = 0;
#endif

#if defined(__MINI_GAME_RUMI__)
	m_pkMiniGameRumi = nullptr;
#endif

#if defined(__MINI_GAME_YUTNORI__)
	m_pkMiniGameYutnori = nullptr;
#endif

#if defined(__SUMMER_EVENT_ROULETTE__)
	m_pMiniGameRoulette = nullptr;
	m_bMiniGameRoulette_RewardMapperNum = 0;
#endif

	m_bNoOpenedShop = true;

	m_bOpeningSafebox = false;

	m_fSyncTime = get_float_time() - 3;
	m_dwPlayerID = 0;
	m_dwKillerPID = 0;

	m_iMoveCount = 0;

	m_pkRegen = NULL;
	regen_id_ = 0;
	m_posRegen.x = m_posRegen.y = m_posRegen.z = 0;
	m_posStart.x = m_posStart.y = 0;
	m_posDest.x = m_posDest.y = 0;
	m_fRegenAngle = 0.0f;

	m_pkMobData = NULL;
	m_pkMobInst = NULL;

	m_pkShop = NULL;
	m_pkChrShopOwner = NULL;
	m_pkMyShop = NULL;
	m_pkExchange = NULL;
	m_pkParty = NULL;
	m_pkPartyRequestEvent = NULL;

	m_pGuild = NULL;

	m_pkChrTarget = NULL;

#if defined(__CHANGE_LOOK_SYSTEM__)
	m_pkChangeLook = nullptr;
#endif

	m_pkMuyeongEvent = NULL;
#ifdef ENABLE_QUEEN_NETHIS
	m_pkSnakeSkillEvent = NULL;
#endif
#if defined(__PVP_BALANCE_IMPROVING__)
	m_pkGyeongGongEvent = NULL;
#endif
#if defined(__9TH_SKILL__)
	m_pkCheonunEvent = NULL;
#endif
#if defined(__ATTR_6TH_7TH__)
	m_pHitBuffEvent = NULL;
#endif
#if defined(__ELEMENTAL_DUNGEON__)
	m_pElementalCurseEvent = NULL;
	m_dwAccumulatedDamage = 0;
#endif

	m_pkWarpNPCEvent = NULL;
	m_pkDeadEvent = NULL;
	m_pkStunEvent = NULL;
	m_pkSaveEvent = NULL;
	m_pkRecoveryEvent = NULL;
	m_pkTimedEvent = NULL;
	m_pkFishingEvent = NULL;
	m_pkWarpEvent = NULL;

	// MINING
	m_pkMiningEvent = NULL;
	// END_OF_MINING

	m_pkPoisonEvent = NULL;
	m_pkBleedingEvent = NULL;
	m_pkFireEvent = NULL;
#if defined(__DAWNMIST_DUNGEON__)
	m_pkHealEvent = NULL;
#endif

	m_pkCheckSpeedHackEvent = NULL;
	m_speed_hack_count = 0;

	m_pkAffectEvent = NULL;
	m_afAffectFlag = TAffectFlag(0, 0);

	m_pkDestroyWhenIdleEvent = NULL;

	m_pkChrSyncOwner = NULL;

	memset(&m_points, 0, sizeof(m_points));
	memset(&m_pointsInstant, 0, sizeof(m_pointsInstant));
	memset(&m_quickslot, 0, sizeof(m_quickslot));

	m_bCharType = CHAR_TYPE_MONSTER;

	SetPosition(POS_STANDING);

	m_dwPlayStartTime = m_dwLastMoveTime = get_dword_time();
#if defined(__LEFT_SEAT__)
	m_dwLastRequestTime = get_dword_time();
#endif

	GotoState(m_stateIdle);
	m_dwStateDuration = 1;

	m_dwLastAttackTime = get_dword_time() - 20000;

	m_bAddChrState = 0;

	m_pkChrStone = NULL;

	m_pkSafebox = NULL;
	m_iSafeboxSize = -1;
	m_iSafeboxLoadTime = 0;

	m_pkMall = NULL;
	m_iMallLoadTime = 0;

	m_posWarp.x = m_posWarp.y = m_posWarp.z = 0;
	m_lWarpMapIndex = 0;

	m_posExit.x = m_posExit.y = m_posExit.z = 0;
	m_lExitMapIndex = 0;

	m_pSkillLevels = NULL;

	m_dwMoveStartTime = 0;
	m_dwMoveDuration = 0;

	m_dwFlyTargetID = 0;

	m_dwNextStatePulse = 0;

	m_dwLastDeadTime = get_dword_time() - 180000;

	m_bSkipSave = false;

	m_bItemLoaded = false;

	m_bHasPoisoned = false;
	m_bHasBled = false;

	m_pkDungeon = NULL;
	m_iEventAttr = 0;

#if defined(__GUILD_DRAGONLAIR_SYSTEM__)
	m_pGuildDragonLair = NULL;
#endif

#if defined(__DEFENSE_WAVE__)
	m_pDefenseWave = NULL;
#endif

	m_kAttackLog.dwVID = 0;
	m_kAttackLog.dwTime = 0;

	m_bNowWalking = m_bWalking = false;
	ResetChangeAttackPositionTime();

	m_bDetailLog = false;
	m_bMonsterLog = false;

	m_bDisableCooltime = false;

	m_iAlignment = 0;
	m_iRealAlignment = 0;

	m_iKillerModePulse = 0;
	m_bPKMode = PK_MODE_PEACE;

	m_dwQuestNPCVID = 0;
	m_dwQuestByVnum = 0;
	m_pQuestItem = NULL;

	m_dwUnderGuildWarInfoMessageTime = get_dword_time() - 60000;

	m_bUnderRefine = false;

	// REFINE_NPC
	m_dwRefineNPCVID = 0;
	// END_OF_REFINE_NPC

	m_dwPolymorphRace = 0;
#if defined(__RACE_SWAP__)
	m_dwEventRaceNum = 0;
#endif

	m_bStaminaConsume = false;

	ResetChainLightningIndex();

#if defined(__PRIVATESHOP_SEARCH_SYSTEM__)
	bPrivateShopSearchState = SHOP_SEARCH_OFF;
#endif

	m_dwMountVnum = 0;
	m_chHorse = NULL;
	m_chRider = NULL;

	m_pWarMap = NULL;
	m_pWeddingMap = NULL;
	m_bChatCounter = 0;
	m_bWhisperCounter = 0;

	ResetStopTime();

	m_dwLastVictimSetTime = get_dword_time() - 3000;
	m_iMaxAggro = -100;

	m_bSendHorseLevel = 0;
	m_bSendHorseHealthGrade = 0;
	m_bSendHorseStaminaGrade = 0;

	m_dwLoginPlayTime = 0;

	m_pkChrMarried = NULL;

	m_posSafeboxOpen.x = -1000;
	m_posSafeboxOpen.y = -1000;

	// EQUIP_LAST_SKILL_DELAY
	m_dwLastSkillTime = get_dword_time();
	// END_OF_EQUIP_LAST_SKILL_DELAY

	// MOB_SKILL_COOLTIME
	memset(m_adwMobSkillCooltime, 0, sizeof(m_adwMobSkillCooltime));
	// END_OF_MOB_SKILL_COOLTIME

	m_isinPCBang = false;

	// ARENA
	m_pArena = NULL;
	m_nPotionLimit = quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count");
	// END_ARENA

	// PREVENT_TRADE_WINDOW
	m_isOpenSafebox = 0;
	// END_PREVENT_TRADE_WINDOW

	// PREVENT_REFINE_HACK
	m_iRefineTime = 0;
	// END_PREVENT_REFINE_HACK

	// RESTRICT_USE_SEED_OR_MOONBOTTLE
	m_iSeedTime = 0;
	// END_RESTRICT_USE_SEED_OR_MOONBOTTLE
	// PREVENT_PORTAL_AFTER_EXCHANGE
	m_iExchangeTime = 0;
	// END_PREVENT_PORTAL_AFTER_EXCHANGE

	m_iSafeboxLoadTime = 0;

	m_iMyShopTime = 0;

#if defined(__MYSHOP_DECO__)
	m_bMyShopDecoType = 0;
	m_bMyShopDecoState = 0;
	m_bMyShopDecoPolyVnum = 30000;

	m_bMyPrivShopTabCount = 1;
	m_bMyPrivShopIsCashItem = false;
#endif

#if defined(__FLOWER_EVENT__)
	m_dwLastFlowerEventExchangePulse = 0;
#endif

	InitMC();

	m_deposit_pulse = 0;

	SET_OVER_TIME(this, OT_NONE);

	m_strNewName = "";

	m_known_guild.clear();

	m_dwLogOffInterval = 0;
	m_dwLastPlay = 0;

	m_bComboSequence = 0;
	m_dwLastComboTime = 0;
	m_bComboIndex = 0;
	m_iComboHackCount = 0;
	m_dwSkipComboAttackByTime = 0;

	m_dwMountTime = 0;

	m_dwLastGoldDropTime = 0;
#if defined(__CHEQUE_SYSTEM__)
	m_dwLastChequeDropTime = 0;
#endif

	m_bIsLoadedAffect = false;
	cannot_dead = false;

	m_newSummonInterval = 0;
	m_lastSummonTime = 0;

#if defined(__PET_SYSTEM__)
	m_petSystem = 0;
	m_bIsPet = false;
#endif

#if defined(__LOOT_FILTER_SYSTEM__)
	m_pLootFilter = nullptr;
#endif

	m_fAttMul = 1.0f;
	m_fDamMul = 1.0f;

	m_pointsInstant.iDragonSoulActiveDeck = -1;

	memset(&m_tvLastSyncTime, 0, sizeof(m_tvLastSyncTime));
	m_iSyncHackCount = 0;

#if defined(__CHANGED_ATTR__)
	memset(&m_ItemSelectAttr, 0, sizeof(m_ItemSelectAttr));
#endif

#if defined(__MINI_GAME_CATCH_KING__)
	m_vecCatchKingFieldCards.clear();
	bCatchKingHandCard = 0;
	bCatchKingHandCardLeft = 0;
	bCatchKingBetSetNumber = 0;
	dwCatchKingTotalScore = 0;
	dwCatchKingGameStatus = false;
#endif

#if defined(__LUCKY_BOX__)
	ResetLuckyBoxData();
#endif

#if defined(__GAME_OPTION_ESCAPE__)
	m_dwEscapeCooltime = 0;
#endif

#if defined(__HIDE_COSTUME_SYSTEM__)
	m_dwHideCostumePulse = 0;
	memset(&m_bHiddenCostumePart, 0, sizeof(m_bHiddenCostumePart));
#endif
}

void CHARACTER::Create(const char* c_pszName, DWORD vid, bool isPC)
{
	static int s_crc = 172814;

	char crc_string[128 + 1];
	snprintf(crc_string, sizeof(crc_string), "%s%p%d", c_pszName, this, ++s_crc);
	m_vid = VID(vid, GetCRC32(crc_string, strlen(crc_string)));

	if (isPC)
		m_stName = c_pszName;
}

void CHARACTER::Destroy()
{
	CloseMyShop();

	if (m_pkRegen)
	{
		if (m_pkDungeon)
		{
			// Dungeon regen may not be valid at this point
			if (m_pkDungeon->IsValidRegen(m_pkRegen, regen_id_))
			{
				--m_pkRegen->count;
			}
		}
		else
		{
			// Is this really safe?
			--m_pkRegen->count;
		}
		m_pkRegen = NULL;
	}

	if (m_pkDungeon)
		SetDungeon(NULL);

#if defined(__GUILD_DRAGONLAIR_SYSTEM__)
	if (m_pGuildDragonLair)
		SetGuildDragonLair(NULL);
#endif

#if defined(__DEFENSE_WAVE__)
	if (m_pDefenseWave)
		SetDefenseWave(NULL);
#endif

#if defined(__PET_SYSTEM__)
	if (m_petSystem)
	{
		m_petSystem->Destroy();
		delete m_petSystem;

		m_petSystem = 0;
	}
#endif

#if defined(__LOOT_FILTER_SYSTEM__)
	ClearLootFilter();
#endif

#if defined(__CHANGE_LOOK_SYSTEM__)
	SetChangeLook(nullptr);
#endif

#if defined(__MINI_GAME_RUMI__)
	SetMiniGameRumi(nullptr);
#endif

#if defined(__MINI_GAME_YUTNORI__)
	SetMiniGameYutnori(nullptr);
#endif

#if defined(__MAILBOX__)
	SetMailBox(nullptr);
#endif

#if defined(__GEM_SHOP__)
	SetGemShop(nullptr);
#endif

#if defined(__SUMMER_EVENT_ROULETTE__)
	SetMiniGameRoulette(nullptr);
#endif

	HorseSummon(false);

	if (GetRider())
		GetRider()->ClearHorseInfo();

	if (GetDesc())
	{
		GetDesc()->BindCharacter(NULL);
		//BindDesc(NULL);
	}

	if (m_pkExchange)
		m_pkExchange->Cancel();

	SetVictim(NULL);

	if (GetShop())
	{
		GetShop()->RemoveGuest(this);
		SetShop(NULL);
	}

	ClearStone();
	ClearSync();
	ClearTarget();

	if (NULL == m_pkMobData)
	{
#if defined(__DRAGON_SOUL_SYSTEM__)
		DragonSoul_CleanUp();
#endif
		ClearItem();
	}

	// <Factor> m_pkParty becomes NULL after CParty destructor call!
	LPPARTY party = m_pkParty;
	if (party)
	{
		if (party->GetLeaderPID() == GetVID() && !IsPC())
		{
			M2_DELETE(party);
		}
		else
		{
			party->Unlink(this);

			if (!IsPC())
				party->Quit(GetVID());
		}

		SetParty(NULL); // 안해도 되지만 안전하게.
	}

	if (m_pkMobInst)
	{
		M2_DELETE(m_pkMobInst);
		m_pkMobInst = NULL;
	}

	m_pkMobData = NULL;

	if (m_pkSafebox)
	{
		M2_DELETE(m_pkSafebox);
		m_pkSafebox = NULL;
	}

	if (m_pkMall)
	{
		M2_DELETE(m_pkMall);
		m_pkMall = NULL;
	}

	m_set_pkChrSpawnedBy.clear();

	StopMuyeongEvent();
#if defined(__9TH_SKILL__)
	StopCheonunEvent();
#endif
#if defined(__ATTR_6TH_7TH__)
	StopHitBuffEvent();
#endif
#if defined(__ELEMENTAL_DUNGEON__)
	StopElementalCurseEvent();
#endif

	event_cancel(&m_pkWarpNPCEvent);
	event_cancel(&m_pkRecoveryEvent);
	event_cancel(&m_pkDeadEvent);
	event_cancel(&m_pkSaveEvent);
	event_cancel(&m_pkTimedEvent);
	event_cancel(&m_pkStunEvent);
	event_cancel(&m_pkFishingEvent);
	event_cancel(&m_pkPoisonEvent);
	event_cancel(&m_pkBleedingEvent);
	event_cancel(&m_pkFireEvent);
#if defined(__DAWNMIST_DUNGEON__)
	event_cancel(&m_pkHealEvent);
#endif
	event_cancel(&m_pkPartyRequestEvent);
	// DELAYED_WARP
	event_cancel(&m_pkWarpEvent);
	event_cancel(&m_pkCheckSpeedHackEvent);
	// END_DELAYED_WARP

	// RECALL_DELAY
	//event_cancel(&m_pkRecallEvent);
	// END_OF_RECALL_DELAY

	// MINING
	event_cancel(&m_pkMiningEvent);
	// END_OF_MINING

#if defined(__LEFT_SEAT__)
	event_cancel(&m_pLeftSeatWaitTimerEvent);
	event_cancel(&m_pLeftSeatLogoutTimerEvent);
#endif

	for (MobSkillEventMap::iterator it = m_mapMobSkillEvent.begin();
		it != m_mapMobSkillEvent.end(); ++it)
	{
		LPEVENT pkEvent = it->second;
		event_cancel(&pkEvent);
	}
	m_mapMobSkillEvent.clear();

	//event_cancel(&m_pkAffectEvent);
	ClearAffect();

	for (TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.begin();
		it != m_map_buff_on_attrs.end(); ++it)
	{
		if (NULL != it->second)
		{
			M2_DELETE(it->second);
		}
	}
	m_map_buff_on_attrs.clear();

	event_cancel(&m_pkDestroyWhenIdleEvent);

	if (m_pSkillLevels)
	{
		M2_DELETE_ARRAY(m_pSkillLevels);
		m_pSkillLevels = NULL;
	}

	CEntity::Destroy();

	if (GetSectree())
		GetSectree()->RemoveEntity(this);

	if (m_bMonsterLog)
		CHARACTER_MANAGER::instance().UnregisterForMonsterLog(this);
}

const char* CHARACTER::GetName() const
{
	return m_stName.empty() ? (m_pkMobData ? m_pkMobData->m_table.szLocaleName : "") : m_stName.c_str();
}

void CHARACTER::OpenMyShop(const char* c_pszSign, TShopItemTable* pTable, BYTE bItemCount)
{
	if (!CanHandleItem())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("다른 거래중(창고,교환,상점)에는 개인상점을 사용할 수 없습니다."));
		return;
	}

	if (GetPart(PART_MAIN) > 2)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("갑옷을 벗어야 개인 상점을 열 수 있습니다."));
		return;
	}

	if (GetMyShop()) // 이미 샵이 열려 있으면 닫는다.
	{
		CloseMyShop();
		return;
	}

	// 진행중인 퀘스트가 있으면 상점을 열 수 없다.
	quest::PC* pPC = quest::CQuestManager::instance().GetPCForce(GetPlayerID());

	// GetPCForce는 NULL일 수 없으므로 따로 확인하지 않음
	if (pPC->IsRunning())
		return;

	if (bItemCount == 0)
		return;

	int64_t nTotalMoney = 0;
#if defined(__CHEQUE_SYSTEM__)
	int64_t nTotalCheque = 0;
#endif

	for (int n = 0; n < bItemCount; ++n)
	{
		nTotalMoney += static_cast<int64_t>((pTable + n)->price);
#if defined(__CHEQUE_SYSTEM__)
		nTotalCheque += static_cast<int64_t>((pTable + n)->cheque);
#endif
	}

	nTotalMoney += static_cast<int64_t>(GetGold());
#if defined(__CHEQUE_SYSTEM__)
	nTotalCheque += static_cast<int64_t>(GetCheque());
#endif

	if (GOLD_MAX <= nTotalMoney)
	{
		sys_err("[OVERFLOW_GOLD] Overflow (GOLD_MAX) id %u name %s", GetPlayerID(), GetName());
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("20억 냥을 초과하여 상점을 열수가 없습니다"));
		return;
	}

#if defined(__CHEQUE_SYSTEM__)
	if (CHEQUE_MAX < nTotalCheque)
	{
		sys_err("[OVERFLOW_CHEQUE] Overflow (CHEQUE_MAX) id %u name %s", GetPlayerID(), GetName());
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("You cannot trade, because the maximum number of %d Won was exceeded.", CHEQUE_MAX));
		return;
	}
#endif

	char szSign[SHOP_SIGN_MAX_LEN + 1];
	strlcpy(szSign, c_pszSign, sizeof(szSign));

	m_stShopSign = szSign;

	if (m_stShopSign.length() == 0)
		return;

	if (CBanwordManager::instance().CheckString(m_stShopSign.c_str(), m_stShopSign.length()))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("비속어나 은어가 포함된 상점 이름으로 상점을 열 수 없습니다."));
		return;
	}

	// MYSHOP_PRICE_LIST
#if defined(__CHEQUE_SYSTEM__)
	std::map<DWORD, TItemPriceInfo> itemkind;
#else
	std::map<DWORD, DWORD> itemkind; // 아이템 종류별 가격, first: vnum, second: 단일 수량 가격
#endif
	// END_OF_MYSHOP_PRICE_LIST

	std::set<TItemPos> cont;
	for (BYTE i = 0; i < bItemCount; ++i)
	{
		if (cont.find((pTable + i)->pos) != cont.end())
		{
			sys_err("MYSHOP: duplicate shop item detected! (name: %s)", GetName());
			return;
		}

		// ANTI_GIVE, ANTI_MYSHOP check
		LPITEM pkItem = GetItem((pTable + i)->pos);

		if (pkItem)
		{
			const TItemTable* item_table = pkItem->GetProto();

			if (item_table && (IS_SET(item_table->ullAntiFlags, ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_MYSHOP)))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_STRING("유료화 아이템은 개인상점에서 판매할 수 없습니다."));
				return;
			}

			if (pkItem->IsEquipped() == true)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_STRING("장비중인 아이템은 개인상점에서 판매할 수 없습니다."));
				return;
			}

			if (true == pkItem->isLocked())
			{
				ChatPacket(CHAT_TYPE_INFO, LC_STRING("사용중인 아이템은 개인상점에서 판매할 수 없습니다."));
				return;
			}

#if defined(__SOUL_BIND_SYSTEM__)
			if (pkItem->IsSealed())
			{
				ChatPacket(CHAT_TYPE_INFO, LC_STRING("You cannot sell soulbound items in a private shop."));
				return;
			}
#endif

			// MYSHOP_PRICE_LIST
#if defined(__CHEQUE_SYSTEM__)
			TItemPriceInfo priceinfo;
			priceinfo.dwVnum = pkItem->GetVnum();
			priceinfo.dwPrice = (pTable + i)->price / pkItem->GetCount();
			priceinfo.dwCheque = (pTable + i)->cheque / pkItem->GetCount();
			itemkind[pkItem->GetVnum()] = priceinfo;
#else
			itemkind[pkItem->GetVnum()] = (pTable + i)->price / pkItem->GetCount();
#endif
			// END_OF_MYSHOP_PRICE_LIST
		}

		cont.insert((pTable + i)->pos);
	}

	// MYSHOP_PRICE_LIST
	// 보따리 개수를 감소시킨다.
#if defined(__MYSHOP_DECO__)
	if (m_bMyPrivShopIsCashItem) // 비단 보따리는 없애지 않고 가격정보를 저장한다.
#else
	if (CountSpecifyItem(71049)) // 비단 보따리는 없애지 않고 가격정보를 저장한다.
#endif
	{
		//
		// 아이템 가격정보를 저장하기 위해 아이템 가격정보 패킷을 만들어 DB 캐시에 보낸다.
		//
		TItemPriceListTable header;
		memset(&header, 0, sizeof(TItemPriceListTable));

		header.dwOwnerID = GetPlayerID();
		header.byCount = static_cast<BYTE>(itemkind.size());

		size_t idx = 0;

		for (auto it = itemkind.begin(); it != itemkind.end(); ++it)
		{
			header.aPriceInfo[idx].dwVnum = it->first;
#if defined(__CHEQUE_SYSTEM__)
			header.aPriceInfo[idx].dwPrice = it->second.dwPrice;
			header.aPriceInfo[idx].dwCheque = it->second.dwCheque;
#else
			header.aPriceInfo[idx].dwPrice = it->second;
#endif

			idx++;
		}

		db_clientdesc->DBPacket(HEADER_GD_MYSHOP_PRICELIST_UPDATE, GetDesc()->GetHandle(), &header, sizeof(TItemPriceListTable));
	}
	// END_OF_MYSHOP_PRICE_LIST
	else if (CountSpecifyItem(50200))
		RemoveSpecifyItem(50200, 1);
	else
		return; // 보따리가 없으면 중단.

	if (m_pkExchange)
		m_pkExchange->Cancel();

	TPacketGCShopSign p;

	p.bHeader = HEADER_GC_SHOP_SIGN;
	p.dwVID = GetVID();
	strlcpy(p.szSign, c_pszSign, sizeof(p.szSign));
#if defined(__MYSHOP_DECO__)
	p.bType = GetMyShopDecoType();
#endif

	PacketAround(&p, sizeof(TPacketGCShopSign));

	m_pkMyShop = CShopManager::instance().CreatePCShop(this, pTable, bItemCount);

	if (IsPolymorphed() == true)
	{
		RemoveAffect(AFFECT_POLYMORPH);
	}

	if (GetHorse())
	{
		HorseSummon(false, true);
	}
	// new mount 이용 중에, 개인 상점 열면 자동 unmount
	// StopRiding으로 뉴마운트까지 처리하면 좋은데 왜 그렇게 안해놨는지 알 수 없다.
	else if (GetMountVnum())
	{
		RemoveAffect(AFFECT_MOUNT);
		RemoveAffect(AFFECT_MOUNT_BONUS);
	}

#if defined(__MYSHOP_DECO__)
	SetPolymorph(GetMyShopDecoPolyVnum(), true);
#else
	SetPolymorph(30000, true);
#endif
}

void CHARACTER::CloseMyShop()
{
	if (GetMyShop())
	{
		m_stShopSign.clear();
		CShopManager::instance().DestroyPCShop(this);
		m_pkMyShop = NULL;

#if defined(__MYSHOP_DECO__)
		SetMyShopDecoType(0);
		SetMyShopDecoPolyVnum(30000);
		SetMyPrivShopTabCount(1);
#endif

		TPacketGCShopSign p;

		p.bHeader = HEADER_GC_SHOP_SIGN;
		p.dwVID = GetVID();
		p.szSign[0] = '\0';
#if defined(__MYSHOP_DECO__)
		p.bType = 0;
#endif

		PacketAround(&p, sizeof(p));

		SetPolymorph(m_points.bJob, true);
#if defined(__PRIVATESHOP_SEARCH_SYSTEM__)
		CTargetManager::instance().DeleteShopSearchTarget(static_cast<DWORD>(GetVID()));
#endif
	}
}

void EncodeMovePacket(TPacketGCMove& pack, DWORD dwVID, BYTE bFunc, BYTE bArg, DWORD x, DWORD y, DWORD dwDuration, DWORD dwTime, BYTE bRot)
{
	pack.bHeader = HEADER_GC_MOVE;
	pack.bFunc = bFunc;
	pack.bArg = bArg;
	pack.dwVID = dwVID;
	pack.dwTime = dwTime ? dwTime : get_dword_time();
	pack.bRot = bRot;
	pack.lX = x;
	pack.lY = y;
	pack.dwDuration = dwDuration;
}

void CHARACTER::Restart(BYTE bSubCMD)
{
	if (false == IsDead())
	{
		ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");
		StartRecoveryEvent();
		return;
	}

	if (NULL == m_pkDeadEvent)
		return;

	int iTimeToDead = (event_time(m_pkDeadEvent) / passes_per_sec);

	if (bSubCMD != SCMD_RESTART_TOWN && (!GetWarMap() || GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG))
	{
		if (!test_server)
		{
			if (IsHack())
			{
				// 성지 맵일경우에는 체크 하지 않는다.
				if (false == CThreeWayWar::instance().IsSungZiMapIndex(GetMapIndex()))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_STRING("아직 재시작 할 수 없습니다. (%d초 남음)", iTimeToDead - (180 - g_nPortalLimitTime)));
					return;
				}
			}

			if (iTimeToDead > 170)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_STRING("아직 재시작 할 수 없습니다. (%d초 남음)", iTimeToDead - 170));
				return;
			}
		}
	}

	// PREVENT_HACK
	// DESC : 창고, 교환 창 후 포탈을 사용하는 버그에 이용될수 있어서
	// 쿨타임을 추가
	if (bSubCMD == SCMD_RESTART_TOWN)
	{
		if (IsHack())
		{
			// 길드맵, 성지맵에서는 체크 하지 않는다.
			if ((!GetWarMap() || GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG) ||
				false == CThreeWayWar::instance().IsSungZiMapIndex(GetMapIndex()))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_STRING("아직 재시작 할 수 없습니다. (%d초 남음)", iTimeToDead - (180 - g_nPortalLimitTime)));
				return;
			}
		}

		if (iTimeToDead > 173)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("아직 마을에서 재시작 할 수 없습니다. (%d 초 남음)", iTimeToDead - 173));
			return;
		}
	}
	//END_PREVENT_HACK

	ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");

	GetDesc()->SetPhase(PHASE_GAME);
	SetPosition(POS_STANDING);
	StartRecoveryEvent();

	// FORKED_LOAD
	// DESC: 삼거리 전투시 부활을 할경우 맵의 입구가 아닌 삼거리 전투의 시작지점으로 이동하게 된다.
	if (1 == quest::CQuestManager::instance().GetEventFlag("threeway_war"))
	{
		if (bSubCMD == SCMD_RESTART_TOWN || bSubCMD == SCMD_RESTART_HERE)
		{
			if (true == CThreeWayWar::instance().IsThreeWayWarMapIndex(GetMapIndex()) &&
				false == CThreeWayWar::instance().IsSungZiMapIndex(GetMapIndex()))
			{
				WarpSet(EMPIRE_START_X(GetEmpire()), EMPIRE_START_Y(GetEmpire()));

				PointChange(POINT_HP, GetMaxHP() - GetHP());
				PointChange(POINT_SP, GetMaxSP() - GetSP());

				ComputePoints();
				ReviveInvisible(5);

				return;
			}

			// 성지
			if (true == CThreeWayWar::instance().IsSungZiMapIndex(GetMapIndex()))
			{
				if (CThreeWayWar::instance().GetReviveTokenForPlayer(GetPlayerID()) <= 0)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_STRING("성지에서 부활 기회를 모두 잃었습니다! 마을로 이동합니다!"));
					WarpSet(EMPIRE_START_X(GetEmpire()), EMPIRE_START_Y(GetEmpire()));
				}
				else
				{
					Show(GetMapIndex(), GetSungziStartX(GetEmpire()), GetSungziStartY(GetEmpire()));
				}

				PointChange(POINT_HP, GetMaxHP() - GetHP());
				PointChange(POINT_SP, GetMaxSP() - GetSP());

				ComputePoints();
				ReviveInvisible(5);

				return;
			}
		}
	}
	// END_FORKED_LOAD

	if (GetDungeon())
		GetDungeon()->UseRevive(this);

	if (GetWarMap() && !IsObserverMode())
	{
		CWarMap* pMap = GetWarMap();
		DWORD dwGuildOpponent = pMap ? pMap->GetGuildOpponent(this) : 0;

		if (dwGuildOpponent)
		{
			switch (bSubCMD)
			{
				case SCMD_RESTART_TOWN:
				{
					sys_log(0, "do_restart: restart town");

					PIXEL_POSITION pos;
					if (CWarMapManager::instance().GetStartPosition(GetMapIndex(), GetGuild()->GetID() < dwGuildOpponent ? 0 : 1, pos))
						Show(GetMapIndex(), pos.x, pos.y);
					else
						ExitToSavedLocation();

					PointChange(POINT_HP, GetMaxHP() - GetHP());
					PointChange(POINT_SP, GetMaxSP() - GetSP());

					ComputePoints();
					ReviveInvisible(5);
				}
				break;

				case SCMD_RESTART_HERE:
				{
					sys_log(0, "do_restart: restart here");

					RestartAtSamePos();
					//Show(GetMapIndex(), GetX(), GetY());

					PointChange(POINT_HP, GetMaxHP() - GetHP());
					PointChange(POINT_SP, GetMaxSP() - GetSP());

					ComputePoints();
					ReviveInvisible(5);
				}
				break;
			}

			return;
		}
	}

	if (IS_MAZE_DUNGEON(GetMapIndex()))
	{
		if (WarpSet(1147800, 532600))
			return;
	}

#if defined(__ELEMENTAL_DUNGEON__)
	if (IS_ELEMENTAL_DUNGEON(GetMapIndex()) && !IsGM())
		bSubCMD = SCMD_RESTART_GIVEUP;
#endif

	switch (bSubCMD)
	{
		case SCMD_RESTART_TOWN:
		{
			sys_log(0, "do_restart: restart town");

#if defined(__GUILD_DRAGONLAIR_SYSTEM__)
			if (GetGuildDragonLair())
			{
				const LPSECTREE_MAP pSectree = SECTREE_MANAGER::instance().GetMap(MAP_N_FLAME_DRAGON);
				if (pSectree != NULL)
				{
					Show(GetMapIndex(),
						pSectree->m_setting.iBaseX + GuildDragonLair_GetFactor(3, "PixelPosition", "InsideWatcher", "x") * 100,
						pSectree->m_setting.iBaseY + GuildDragonLair_GetFactor(3, "PixelPosition", "InsideWatcher", "y") * 100, 0);
					Stop();
				}
				else
					CGuildDragonLairManager::Instance().Exit(this);

				PointChange(POINT_HP, 50 - GetHP());
				DeathPenalty(1);
				return;
			}
#endif

			PIXEL_POSITION pos;
			if (SECTREE_MANAGER::instance().GetRecallPositionByEmpire(GetMapIndex(), GetEmpire(), pos))
				WarpSet(pos.x, pos.y);
			else
				WarpSet(EMPIRE_START_X(GetEmpire()), EMPIRE_START_Y(GetEmpire()));

			PointChange(POINT_HP, 50 - GetHP());
			DeathPenalty(1);
		}
		break;

		case SCMD_RESTART_HERE:
		{
			sys_log(0, "do_restart: restart here");

			RestartAtSamePos();
			//Show(GetMapIndex(), GetX(), GetY());

			DeathPenalty(0);
			PointChange(POINT_HP, 50 - GetHP());

			ComputePoints();
			ReviveInvisible(5);
		}
		break;

		case SCMD_RESTART_IMMEDIATE:
			break;

		case SCMD_RESTART_GIVEUP:
		{
#if defined(__DEFENSE_WAVE__)
			if (GetDefenseWave())
				GetDefenseWave()->Exit(this);
#endif

#if defined(__ELEMENTAL_DUNGEON__)
			if (IS_ELEMENTAL_DUNGEON(GetMapIndex()))
			{
				PIXEL_POSITION WarpPos;
				if (SECTREE_MANAGER::instance().GetRecallPositionByEmpire(MAP_DEFENSEWAVE_PORT, GetEmpire(), WarpPos))
					WarpSet(WarpPos.x, WarpPos.y);
				else
					WarpSet(EMPIRE_START_X(GetEmpire()), EMPIRE_START_Y(GetEmpire()));
			}
#endif
		}
	}
}

void CHARACTER::RestartAtSamePos()
{
	if (m_bIsObserver)
		return;

#if defined(__QUEST_EVENT_RESTART_HERE__)
	quest::CQuestManager::instance().RestartHere(GetPlayerID());
#endif

#if defined(__SET_ITEM__)
	// NOTE : Due to the classic pet disappearing when the character dies,
	// we have to call this function again to check if the pet is spawned.
	RefreshItemSetBonus();
	// NOTE : If the player is cruel, he is likely to drop some equipment,
	// it is necessary to re-check the set bonus.
	RefreshItemSetBonusByValue();
#endif

	EncodeRemovePacket(this);
	EncodeInsertPacket(this);

	ENTITY_MAP::iterator it = m_map_view.begin();

	while (it != m_map_view.end())
	{
		LPENTITY entity = (it++)->first;

		EncodeRemovePacket(entity);
		if (!m_bIsObserver)
			EncodeInsertPacket(entity);

		if (entity->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER lpChar = (LPCHARACTER)entity;
			if (lpChar->IsPC() || lpChar->IsNPC() || lpChar->IsMonster())
			{
				if (!entity->IsObserverMode())
					entity->EncodeInsertPacket(this);
			}
		}
		else
		{
			if (!entity->IsObserverMode())
			{
				entity->EncodeInsertPacket(this);
			}
		}
	}
}

// Entity에 내가 나타났다고 패킷을 보낸다.
void CHARACTER::EncodeInsertPacket(LPENTITY entity)
{
	LPDESC d;

	if (!(d = entity->GetDesc()))
		return;

	// 길드이름 버그 수정 코드
	LPCHARACTER ch = (LPCHARACTER)entity;
	ch->SendGuildName(GetGuild());
	// 길드이름 버그 수정 코드

	TPacketGCCharacterAdd pack;

	pack.header = HEADER_GC_CHARACTER_ADD;
	pack.dwVID = m_vid;
#if defined(__WJ_SHOW_MOB_INFO__)
	if (IsMonster() || IsStone())
	{
		pack.dwLevel = GetLevel();
		pack.dwAIFlag = IsMonster() ? GetAIFlag() : 0;
	}
	else
	{
		pack.dwLevel = 0;
		pack.dwAIFlag = 0;
	}
#endif
	pack.bType = GetCharType();
	pack.angle = GetRotation();
	pack.x = GetX();
	pack.y = GetY();
	pack.z = GetZ();
	pack.wRaceNum = GetRaceNum();
#if defined(__RACE_SWAP__)
	pack.dwEventRaceNum = GetEventRaceNum();
#endif

#if defined(__PET_SYSTEM__)
	if (IsPet())
	{
		pack.bMovingSpeed = 150;
	}
	else
#endif
	{
		pack.bMovingSpeed = GetLimitPoint(POINT_MOV_SPEED);
	}
	pack.bAttackSpeed = GetLimitPoint(POINT_ATT_SPEED);
	pack.dwAffectFlag[0] = m_afAffectFlag.bits[0];
	pack.dwAffectFlag[1] = m_afAffectFlag.bits[1];
	pack.dwAffectFlag[2] = m_afAffectFlag.bits[2];

	pack.bStateFlag = m_bAddChrState;

	int iDur = 0;

	if (m_posDest.x != pack.x || m_posDest.y != pack.y)
	{
		iDur = (m_dwMoveStartTime + m_dwMoveDuration) - get_dword_time();

		if (iDur <= 0)
		{
			pack.x = m_posDest.x;
			pack.y = m_posDest.y;
		}
	}

	d->Packet(&pack, sizeof(pack));

	if (IsPC() || m_bCharType == CHAR_TYPE_NPC
		|| m_bCharType == CHAR_TYPE_HORSE
		|| IsHorse()
#if defined(__PET_SYSTEM__)
		|| m_bCharType == CHAR_TYPE_PET_PAY
		|| IsPet()
#endif
		//#if defined(__GROWTH_PET_SYSTEM__)
		|| m_bCharType == CHAR_TYPE_PET
		//#endif
		)
	{
		TPacketGCCharacterAdditionalInfo addPacket;
		memset(&addPacket, 0, sizeof(TPacketGCCharacterAdditionalInfo));

		addPacket.header = HEADER_GC_CHAR_ADDITIONAL_INFO;
		addPacket.dwVID = m_vid;

		addPacket.adwPart[CHR_EQUIPPART_ARMOR] = GetPart(PART_MAIN);
		addPacket.adwPart[CHR_EQUIPPART_WEAPON] = GetPart(PART_WEAPON);
		addPacket.adwPart[CHR_EQUIPPART_HEAD] = GetPart(PART_HEAD);
		addPacket.adwPart[CHR_EQUIPPART_HAIR] = GetPart(PART_HAIR);
#if defined(__ACCE_COSTUME_SYSTEM__)
		addPacket.adwPart[CHR_EQUIPPART_ACCE] = GetPart(PART_ACCE);
#endif
#if defined(__AURA_COSTUME_SYSTEM__)
		addPacket.adwPart[CHR_EQUIPPART_AURA] = GetPart(PART_AURA);
#endif
#if defined(__QUIVER_SYSTEM__)
		addPacket.dwArrow = (IsPC() && GetWear(WEAR_ARROW) != NULL) ? GetWear(WEAR_ARROW)->GetOriginalVnum() : 0;
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
		addPacket.wRefineElementAffectType = GetRefineElementEffect();
#endif

		addPacket.bPKMode = m_bPKMode;
		addPacket.dwMountVnum = GetMountVnum();
		addPacket.bEmpire = m_bEmpire;
		addPacket.dwLevel = 0;
#if defined(__CONQUEROR_LEVEL__)
		addPacket.dwConquerorLevel = IsPC() ? GetConquerorLevel() : 0;
#endif
		addPacket.dwGuildID = 0;
#if defined(__GUILD_LEADER_GRADE_NAME__)
		addPacket.bGuildLeaderGrade = 0;
#endif
#if defined(__MULTI_LANGUAGE_SYSTEM__)
		strlcpy(addPacket.szCountry, GetCountry(), sizeof(addPacket.szCountry));
#endif

		strlcpy(addPacket.name, GetName(), sizeof(addPacket.name));

		if (IsPC())
		{
			addPacket.dwLevel = GetLevel();
		}

		if (GetGuild() != NULL)
		{
			addPacket.dwGuildID = GetGuild()->GetID();
#if defined(__GUILD_LEADER_GRADE_NAME__)
			CGuild* pGuild = this->GetGuild();
			if (pGuild->GetMasterPID() == GetPlayerID())
				addPacket.bGuildLeaderGrade = 3;
			else if (pGuild->GetGeneralPID(GetPlayerID()) != 0)
				addPacket.bGuildLeaderGrade = 2;
			else
				addPacket.bGuildLeaderGrade = 1;
#endif
		}

		addPacket.sAlignment = m_iAlignment / 10;

		d->Packet(&addPacket, sizeof(TPacketGCCharacterAdditionalInfo));
	}

	if (iDur)
	{
		TPacketGCMove pack;
		EncodeMovePacket(pack, GetVID(), FUNC_MOVE, 0, m_posDest.x, m_posDest.y, iDur, 0, (BYTE)(GetRotation() / 5));
		d->Packet(&pack, sizeof(pack));

		TPacketGCWalkMode p;
		p.vid = GetVID();
		p.header = HEADER_GC_WALK_MODE;
		p.mode = m_bNowWalking ? WALKMODE_WALK : WALKMODE_RUN;

		d->Packet(&p, sizeof(p));
	}

	if (entity->IsType(ENTITY_CHARACTER) && GetDesc())
	{
		if (ch->IsWalking())
		{
			TPacketGCWalkMode p;
			p.vid = ch->GetVID();
			p.header = HEADER_GC_WALK_MODE;
			p.mode = ch->m_bNowWalking ? WALKMODE_WALK : WALKMODE_RUN;
			GetDesc()->Packet(&p, sizeof(p));
		}

#if defined(__ACCE_COSTUME_SYSTEM__)
		LPITEM pAcceItem = GetWear(WEAR_COSTUME_ACCE);
		if (pAcceItem && pAcceItem->GetSocket(ITEM_SOCKET_ACCE_DRAIN_VALUE) > 18)
		{
			TPacketGCSpecialEffect p;
			p.bHeader = HEADER_GC_SEPCIAL_EFFECT;
			p.bEffectNum = SE_ACCE_BACK;
			p.dwVID = GetVID();
			ch->GetDesc()->Packet(&p, sizeof(p));
			//EffectPacket(SE_ACCE_BACK);
		}
#endif
	}

	if (GetMyShop())
	{
		TPacketGCShopSign p;
		p.bHeader = HEADER_GC_SHOP_SIGN;
		p.dwVID = GetVID();
		strlcpy(p.szSign, m_stShopSign.c_str(), sizeof(p.szSign));
#if defined(__MYSHOP_DECO__)
		p.bType = GetMyShopDecoType();
#endif
		d->Packet(&p, sizeof(TPacketGCShopSign));
	}

#if defined(__GUILD_DRAGONLAIR_SYSTEM__)
	if (CGuildDragonLairManager::Instance().IsRedDragonLair(GetMapIndex()))
	{
		CGuildDragonLair* pGuildDragonLair = GetGuildDragonLair();
		if (pGuildDragonLair != NULL)
		{
			CAffect* pAffect = ch->FindAffect(AFFECT_RED_DRAGONLAIR_STONE);
			if (pAffect && pAffect->lApplyValue)
				ch->EffectPacket(pAffect->lApplyValue);
		}
	}
#endif

#if defined(__SUMMER_EVENT_ROULETTE__)
	if (GetRaceNum() == CMiniGameRoulette::EVENT_NPC)
	{
		if (CMiniGameRoulette::IsActiveEvent() && CMiniGameRoulette::IsSpecialRoulette(ch))
		{
			TPacketGCSpecialEffect p;
			p.bHeader = HEADER_GC_SEPCIAL_EFFECT;
			p.bEffectNum = SE_SPECIAL_ROULETTE;
			p.dwVID = GetVID();
			d->Packet(&p, sizeof(p));
		}
	}
#endif

	if (entity->IsType(ENTITY_CHARACTER))
	{
		sys_log(3, "EntityInsert %s (RaceNum %d) (%d %d) TO %s",
			GetName(), GetRaceNum(), GetX() / SECTREE_SIZE, GetY() / SECTREE_SIZE, ((LPCHARACTER)entity)->GetName());
	}
}

void CHARACTER::EncodeRemovePacket(LPENTITY entity)
{
	if (entity->GetType() != ENTITY_CHARACTER)
		return;

	LPDESC d;

	if (!(d = entity->GetDesc()))
		return;

	TPacketGCCharacterDelete pack;

	pack.header = HEADER_GC_CHARACTER_DEL;
	pack.id = m_vid;

	d->Packet(&pack, sizeof(TPacketGCCharacterDelete));

	if (entity->IsType(ENTITY_CHARACTER))
		sys_log(3, "EntityRemove %s(%d) FROM %s", GetName(), (DWORD)m_vid, ((LPCHARACTER)entity)->GetName());
}

void CHARACTER::UpdatePacket()
{
	if (IsPC() && (!GetDesc() || !GetDesc()->GetCharacter()))
		return;

	if (GetSectree() == NULL)
		return;

	TPacketGCCharacterUpdate pack;
	TPacketGCCharacterUpdate pack2;

	pack.header = HEADER_GC_CHARACTER_UPDATE;
	pack.dwVID = m_vid;

	pack.adwPart[CHR_EQUIPPART_ARMOR] = GetPart(PART_MAIN);
	pack.adwPart[CHR_EQUIPPART_WEAPON] = GetPart(PART_WEAPON);
	pack.adwPart[CHR_EQUIPPART_HEAD] = GetPart(PART_HEAD);
	pack.adwPart[CHR_EQUIPPART_HAIR] = GetPart(PART_HAIR);
#if defined(__ACCE_COSTUME_SYSTEM__)
	pack.adwPart[CHR_EQUIPPART_ACCE] = GetPart(PART_ACCE);
#endif
#if defined(__AURA_COSTUME_SYSTEM__)
	pack.adwPart[CHR_EQUIPPART_AURA] = GetPart(PART_AURA);
#endif
#if defined(__QUIVER_SYSTEM__)
	pack.dwArrow = GetWear(WEAR_ARROW) != NULL ? GetWear(WEAR_ARROW)->GetOriginalVnum() : 0;
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
	pack.wRefineElementAffectType = GetRefineElementEffect();
#endif

	pack.bMovingSpeed = GetLimitPoint(POINT_MOV_SPEED);
	pack.bAttackSpeed = GetLimitPoint(POINT_ATT_SPEED);
	pack.bStateFlag = m_bAddChrState;
	pack.dwAffectFlag[0] = m_afAffectFlag.bits[0];
	pack.dwAffectFlag[1] = m_afAffectFlag.bits[1];
	pack.dwAffectFlag[2] = m_afAffectFlag.bits[2];
	pack.dwGuildID = GetGuild() ? GetGuild()->GetID() : 0;
	pack.sAlignment = m_iAlignment / 10;
	//pack.dwLevel = IsPC() ? GetLevel() : 0;
	pack.dwLevel = GetLevel();
#if defined(__CONQUEROR_LEVEL__)
	pack.dwConquerorLevel = IsPC() ? GetConquerorLevel() : 0;
#endif
	pack.bPKMode = m_bPKMode;
	pack.dwMountVnum = GetMountVnum();
#if defined(__LEFT_SEAT__)
	pack.bLeftSeat = IsPC() ? LeftSeat() : false;
#endif

#if defined(__GUILD_LEADER_GRADE_NAME__)
	CGuild* pGuild = this->GetGuild();
	if (pGuild)
	{
		if (pGuild->GetMasterPID() == GetPlayerID())
			pack.bGuildLeaderGrade = 3;
		else if (pGuild->GetGeneralPID(GetPlayerID()) != 0)
			pack.bGuildLeaderGrade = 2;
		else
			pack.bGuildLeaderGrade = 1;
	}
	else
	{
		pack.bGuildLeaderGrade = 0;
	}
#endif

	pack2 = pack;
	pack2.dwGuildID = 0;
	pack2.sAlignment = 0;

	if (false)
	{
		if (m_bIsObserver != true)
		{
			for (ENTITY_MAP::iterator iter = m_map_view.begin(); iter != m_map_view.end(); iter++)
			{
				LPENTITY pEntity = iter->first;

				if (pEntity != NULL)
				{
					if (pEntity->IsType(ENTITY_CHARACTER) == true)
					{
						if (pEntity->GetDesc() != NULL)
						{
							LPCHARACTER pChar = (LPCHARACTER)pEntity;

							if (GetEmpire() == pChar->GetEmpire() || pChar->GetGMLevel() > GM_PLAYER)
							{
								pEntity->GetDesc()->Packet(&pack, sizeof(pack));
							}
							else
							{
								pEntity->GetDesc()->Packet(&pack2, sizeof(pack2));
							}
						}
					}
					else
					{
						if (pEntity->GetDesc() != NULL)
						{
							pEntity->GetDesc()->Packet(&pack, sizeof(pack));
						}
					}
				}
			}
		}

		if (GetDesc() != NULL)
		{
			GetDesc()->Packet(&pack, sizeof(pack));
		}
	}
	else
	{
		PacketAround(&pack, sizeof(pack));
	}
}

LPCHARACTER CHARACTER::FindCharacterInView(const char* c_pszName, bool bFindPCOnly)
{
	ENTITY_MAP::iterator it = m_map_view.begin();

	for (; it != m_map_view.end(); ++it)
	{
		if (!it->first->IsType(ENTITY_CHARACTER))
			continue;

		LPCHARACTER tch = (LPCHARACTER)it->first;

		if (bFindPCOnly && tch->IsNPC())
			continue;

		if (!strcasecmp(tch->GetName(), c_pszName))
			return (tch);
	}

	return NULL;
}

void CHARACTER::SetPosition(int pos)
{
	if (pos == POS_STANDING)
	{
		REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_DEAD);
		REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_STUN);

		event_cancel(&m_pkDeadEvent);
		event_cancel(&m_pkStunEvent);
	}
	else if (pos == POS_DEAD)
		SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_DEAD);

	if (!IsStone())
	{
		switch (pos)
		{
			case POS_FIGHTING:
				if (!IsState(m_stateBattle))
					MonsterLog("[BATTLE] 싸우는 상태");

				GotoState(m_stateBattle);
				break;

			default:
				if (!IsState(m_stateIdle))
					MonsterLog("[IDLE] 쉬는 상태");

				GotoState(m_stateIdle);
				break;
		}
	}

	m_pointsInstant.position = pos;
}

void CHARACTER::Save()
{
	if (!m_bSkipSave)
		CHARACTER_MANAGER::instance().DelayedSave(this);
}

void CHARACTER::CreatePlayerProto(TPlayerTable& tab)
{
	memset(&tab, 0, sizeof(TPlayerTable));

	if (GetNewName().empty())
	{
		strlcpy(tab.name, GetName(), sizeof(tab.name));
	}
	else
	{
		strlcpy(tab.name, GetNewName().c_str(), sizeof(tab.name));
	}

	strlcpy(tab.ip, GetDesc()->GetHostName(), sizeof(tab.ip));

	tab.id = m_dwPlayerID;
	tab.voice = GetPoint(POINT_VOICE);
	tab.level = GetLevel();
	tab.level_step = GetPoint(POINT_LEVEL_STEP);
	tab.exp = GetExp();
#if defined(__EXTEND_INVEN_SYSTEM__)
	tab.inven_stage = GetExtendInvenStage();
#endif
	tab.gold = GetGold();
#if defined(__CHEQUE_SYSTEM__)
	tab.cheque = GetCheque();
#endif
#if defined(__GEM_SYSTEM__)
	tab.gem = GetGem();
#endif
	tab.job = m_points.bJob;
	tab.part_base = m_pointsInstant.bBasePart;
	tab.skill_group = m_points.bSkillGroup;

	DWORD dwPlayedTime = (get_dword_time() - m_dwPlayStartTime);

	if (dwPlayedTime > 60000)
	{
		if (GetSectree() && !GetSectree()->IsAttr(GetX(), GetY(), ATTR_BANPK))
		{
			if (GetRealAlignment() < 0)
			{
				if (IsEquipUniqueItem(UNIQUE_ITEM_FASTER_ALIGNMENT_UP_BY_TIME))
					UpdateAlignment(120 * (dwPlayedTime / 60000));
				else
					UpdateAlignment(60 * (dwPlayedTime / 60000));
			}
			else
				UpdateAlignment(5 * (dwPlayedTime / 60000));
		}

		SetRealPoint(POINT_PLAYTIME, GetRealPoint(POINT_PLAYTIME) + dwPlayedTime / 60000);
		ResetPlayTime(dwPlayedTime % 60000);
	}

	tab.playtime = GetRealPoint(POINT_PLAYTIME);
	tab.lAlignment = m_iRealAlignment;

	if (m_posWarp.x != 0 || m_posWarp.y != 0)
	{
		tab.x = m_posWarp.x;
		tab.y = m_posWarp.y;
		tab.z = 0;
		tab.lMapIndex = m_lWarpMapIndex;
	}
	else
	{
		tab.x = GetX();
		tab.y = GetY();
		tab.z = GetZ();
		tab.lMapIndex = GetMapIndex();
	}

	if (m_lExitMapIndex == 0)
	{
		tab.lExitMapIndex = tab.lMapIndex;
		tab.lExitX = tab.x;
		tab.lExitY = tab.y;
	}
	else
	{
		tab.lExitMapIndex = m_lExitMapIndex;
		tab.lExitX = m_posExit.x;
		tab.lExitY = m_posExit.y;
	}

	sys_log(0, "SAVE: %s %dx%d", GetName(), tab.x, tab.y);

	tab.st = GetRealPoint(POINT_ST);
	tab.ht = GetRealPoint(POINT_HT);
	tab.dx = GetRealPoint(POINT_DX);
	tab.iq = GetRealPoint(POINT_IQ);

	tab.stat_point = GetPoint(POINT_STAT);
	tab.skill_point = GetPoint(POINT_SKILL);
	tab.sub_skill_point = GetPoint(POINT_SUB_SKILL);
	tab.horse_skill_point = GetPoint(POINT_HORSE_SKILL);

	tab.stat_reset_count = GetPoint(POINT_STAT_RESET_COUNT);

	tab.hp = GetHP();
	tab.sp = GetSP();

	tab.stamina = GetStamina();

	tab.sRandomHP = m_points.iRandomHP;
	tab.sRandomSP = m_points.iRandomSP;

	for (int i = 0; i < QUICKSLOT_MAX_NUM; ++i)
		tab.quickslot[i] = m_quickslot[i];

	thecore_memcpy(tab.adwParts, m_pointsInstant.adwParts, sizeof(tab.adwParts));

	// REMOVE_REAL_SKILL_LEVLES
	thecore_memcpy(tab.skills, m_pSkillLevels, sizeof(TPlayerSkill) * SKILL_MAX_NUM);
	// END_OF_REMOVE_REAL_SKILL_LEVLES

	tab.horse = GetHorseData();

#if defined(__CONQUEROR_LEVEL__)
	tab.conqueror_level = GetConquerorLevel();
	tab.conqueror_level_step = GetPoint(POINT_CONQUEROR_LEVEL_STEP);
	tab.conqueror_exp = GetConquerorExp();

	tab.sungma_str = GetRealPoint(POINT_SUNGMA_STR);
	tab.sungma_hp = GetRealPoint(POINT_SUNGMA_HP);
	tab.sungma_move = GetRealPoint(POINT_SUNGMA_MOVE);
	tab.sungma_immune = GetRealPoint(POINT_SUNGMA_IMMUNE);

	tab.conqueror_point = GetPoint(POINT_CONQUEROR_POINT);
#endif
}

void CHARACTER::SaveReal()
{
	if (m_bSkipSave)
		return;

	if (!GetDesc())
	{
		sys_err("Character::Save : no descriptor when saving (name: %s)", GetName());
		return;
	}

	TPlayerTable table;
	CreatePlayerProto(table);

	db_clientdesc->DBPacket(HEADER_GD_PLAYER_SAVE, GetDesc()->GetHandle(), &table, sizeof(TPlayerTable));

	quest::PC* pkQuestPC = quest::CQuestManager::instance().GetPCForce(GetPlayerID());

	if (!pkQuestPC)
		sys_err("CHARACTER::Save : null quest::PC pointer! (name %s)", GetName());
	else
	{
		pkQuestPC->Save();
	}

	marriage::TMarriage* pMarriage = marriage::CManager::instance().Get(GetPlayerID());
	if (pMarriage)
		pMarriage->Save();
}

void CHARACTER::FlushDelayedSaveItem()
{
	int i = 0;

	// 저장 안된 소지품을 전부 저장시킨다.
	LPITEM item = nullptr;

	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		if ((item = GetInventoryItem(i)))
			ITEM_MANAGER::Instance().FlushDelayedSave(item);
	}

	for (int i = 0; i < EQUIPMENT_MAX_NUM; ++i)
	{
		if ((item = GetEquipmentItem(i)))
			ITEM_MANAGER::Instance().FlushDelayedSave(item);
	}

#if defined(__DRAGON_SOUL_SYSTEM__)
	for (int i = 0; i < DRAGON_SOUL_INVENTORY_MAX_NUM; ++i)
	{
		if ((item = GetDragonSoulInventoryItem(i)))
			ITEM_MANAGER::Instance().FlushDelayedSave(item);
	}
#endif

	for (int i = 0; i < BELT_INVENTORY_SLOT_COUNT; ++i)
	{
		if ((item = GetBeltInventoryItem(i)))
			ITEM_MANAGER::Instance().FlushDelayedSave(item);
	}
}

void CHARACTER::Disconnect(const char* c_pszReason)
{
	assert(GetDesc() != NULL);

	sys_log(0, "DISCONNECT: %s (%s)", GetName(), c_pszReason ? c_pszReason : "unset");

	if (GetShop())
	{
		GetShop()->RemoveGuest(this);
		SetShop(NULL);
	}

	if (GetArena() != NULL)
	{
		GetArena()->OnDisconnect(GetPlayerID());
	}

	if (GetParty() != NULL)
	{
		GetParty()->UpdateOfflineState(GetPlayerID());
	}

	marriage::CManager::instance().Logout(this);

#if defined(__LUCKY_BOX__)
	LuckyBoxReceive();
#endif

	// P2P Logout
	TPacketGGLogout p;
	p.bHeader = HEADER_GG_LOGOUT;
	strlcpy(p.szName, GetName(), sizeof(p.szName));
	P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGLogout));
	char buf[51];
#if defined(__CHEQUE_SYSTEM__) && defined(__GEM_SYSTEM__)
	snprintf(buf, sizeof(buf), "%s %d %d %d %d %ld %d",
		inet_ntoa(GetDesc()->GetAddr().sin_addr), GetGold(), GetCheque(), GetGem(), g_bChannel, GetMapIndex(), GetAlignment());
#elif defined(__CHEQUE_SYSTEM__)
	snprintf(buf, sizeof(buf), "%s %d %d %d %ld %d",
		inet_ntoa(GetDesc()->GetAddr().sin_addr), GetGold(), GetCheque(), g_bChannel, GetMapIndex(), GetAlignment());
#else
	snprintf(buf, sizeof(buf), "%s %d %d %ld %d",
		inet_ntoa(GetDesc()->GetAddr().sin_addr), GetGold(), g_bChannel, GetMapIndex(), GetAlignment());
#endif
	LogManager::instance().CharLog(this, 0, "LOGOUT", buf);

	if (LC_IsYMIR() || LC_IsKorea() || LC_IsBrazil())
	{
		long playTime = GetRealPoint(POINT_PLAYTIME) - m_dwLoginPlayTime;
		LogManager::instance().LoginLog(false, GetDesc()->GetAccountTable().id, GetPlayerID(), GetLevel(), GetJob(), playTime);

		if (LC_IsBrazil() != true)
			CPCBangManager::instance().Log(GetDesc()->GetHostName(), GetPlayerID(), playTime);
	}

	if (m_pWarMap)
		SetWarMap(NULL);

	if (m_pWeddingMap)
	{
		SetWeddingMap(NULL);
	}

	if (GetGuild())
		GetGuild()->LogoutMember(this);

	quest::CQuestManager::instance().LogoutPC(this);

	if (GetParty())
		GetParty()->Unlink(this);

	// 죽었을 때 접속끊으면 경험치 줄게 하기
	if (IsStun() || IsDead())
	{
		DeathPenalty(0);
		PointChange(POINT_HP, 50 - GetHP());
	}

	if (!CHARACTER_MANAGER::instance().FlushDelayedSave(this))
	{
		SaveReal();
	}

	FlushDelayedSaveItem();

	SaveAffect();
	m_bIsLoadedAffect = false;

	m_bSkipSave = true; // 이 이후에는 더이상 저장하면 안된다.

	quest::CQuestManager::instance().DisconnectPC(this);

	CloseSafebox();
	CloseMall();

#if defined(__ACCE_COSTUME_SYSTEM__)
	if (IsAcceRefineWindowOpen())
		AcceRefineWindowClose(true);
#endif

#if defined(__AURA_COSTUME_SYSTEM__)
	if (IsAuraRefineWindowOpen())
		AuraRefineWindowClose(true);
#endif

	CPVPManager::instance().Disconnect(this);

	CTargetManager::instance().Logout(GetPlayerID());

	CMessengerManager::instance().Logout(GetName());

	if (GetDesc())
	{
#if defined(__IMPROVED_LOGOUT_POINTS__)
		packet_point_change pack;
		pack.bHeader = HEADER_GC_CHARACTER_POINT_CHANGE;
		pack.dwVID = m_vid;
		pack.wType = POINT_PLAYTIME;
		pack.lValue = GetRealPoint(POINT_PLAYTIME) + (get_dword_time() - m_dwPlayStartTime) / 60000;
		pack.lAmount = 0;
		GetDesc()->Packet(&pack, sizeof(struct packet_point_change));
#endif
		GetDesc()->BindCharacter(NULL);
		// BindDesc(NULL);
	}

	M2_DESTROY_CHARACTER(this);
}

bool CHARACTER::Show(long lMapIndex, long x, long y, long z, bool bShowSpawnMotion/* = false */
#if defined(__WJ_SHOW_MOB_INFO__)
	, bool bAggressive
#endif
)
{
	LPSECTREE sectree = SECTREE_MANAGER::instance().Get(lMapIndex, x, y);
	if (!sectree)
	{
		sys_log(0, "cannot find sectree by %dx%d mapindex %d", x, y, lMapIndex);
		return false;
	}

	SetMapIndex(lMapIndex);

	bool bChangeTree = false;

	if (!GetSectree() || GetSectree() != sectree)
		bChangeTree = true;

	if (bChangeTree)
	{
		if (GetSectree())
			GetSectree()->RemoveEntity(this);

		ViewCleanup();
	}

	if (!IsNPC())
	{
		sys_log(0, "SHOW: %s %dx%dx%d", GetName(), x, y, z);
		if (GetStamina() < GetMaxStamina())
			StartAffectEvent();
	}
	else if (m_pkMobData)
	{
		m_pkMobInst->m_posLastAttacked.x = x;
		m_pkMobInst->m_posLastAttacked.y = y;
		m_pkMobInst->m_posLastAttacked.z = z;
	}

	if (bShowSpawnMotion)
	{
		SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);
		m_afAffectFlag.Set(AFF_SPAWN);
	}

#if defined(__WJ_SHOW_MOB_INFO__)
	if (bAggressive)
		SetAggressive();
#endif

	SetXYZ(x, y, z);

	m_posDest.x = x;
	m_posDest.y = y;
	m_posDest.z = z;

	m_posStart.x = x;
	m_posStart.y = y;
	m_posStart.z = z;

	if (bChangeTree)
	{
		EncodeInsertPacket(this);
		sectree->InsertEntity(this);

		UpdateSectree();
	}
	else
	{
		ViewReencode();
		sys_log(0, "      in same sectree");
	}

	REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);

	SetValidComboInterval(0);
	return true;
}

// BGM_INFO
struct BGMInfo
{
	std::string name;
	float vol;
};

typedef std::map<unsigned, BGMInfo> BGMInfoMap;

static BGMInfoMap gs_bgmInfoMap;
static bool gs_bgmVolEnable = false;

void CHARACTER_SetBGMVolumeEnable()
{
	gs_bgmVolEnable = true;
	sys_log(0, "bgm_info.set_bgm_volume_enable");
}

void CHARACTER_AddBGMInfo(unsigned mapIndex, const char* name, float vol)
{
	BGMInfo newInfo;
	newInfo.name = name;
	newInfo.vol = vol;

	gs_bgmInfoMap[mapIndex] = newInfo;

	sys_log(0, "bgm_info.add_info(%d, '%s', %f)", mapIndex, name, vol);
}

const BGMInfo& CHARACTER_GetBGMInfo(unsigned mapIndex)
{
	BGMInfoMap::iterator f = gs_bgmInfoMap.find(mapIndex);
	if (gs_bgmInfoMap.end() == f)
	{
		static BGMInfo s_empty = { "", 0.0f };
		return s_empty;
	}
	return f->second;
}

bool CHARACTER_IsBGMVolumeEnable()
{
	return gs_bgmVolEnable;
}
// END_OF_BGM_INFO

void CHARACTER::MainCharacterPacket()
{
	const unsigned mapIndex = GetMapIndex();
	const BGMInfo& bgmInfo = CHARACTER_GetBGMInfo(mapIndex);

	// SUPPORT_BGM
	if (!bgmInfo.name.empty())
	{
		if (CHARACTER_IsBGMVolumeEnable())
		{
			sys_log(1, "bgm_info.play_bgm_vol(%d, name='%s', vol=%f)", mapIndex, bgmInfo.name.c_str(), bgmInfo.vol);
			TPacketGCMainCharacter4_BGM_VOL mainChrPacket;
			mainChrPacket.header = HEADER_GC_MAIN_CHARACTER4_BGM_VOL;
			mainChrPacket.dwVID = m_vid;
			mainChrPacket.wRaceNum = GetRaceNum();
			mainChrPacket.lx = GetX();
			mainChrPacket.ly = GetY();
			mainChrPacket.lz = GetZ();
			mainChrPacket.empire = GetDesc()->GetEmpire();
			mainChrPacket.skill_group = GetSkillGroup();
			strlcpy(mainChrPacket.szChrName, GetName(), sizeof(mainChrPacket.szChrName));

			mainChrPacket.fBGMVol = bgmInfo.vol;
			strlcpy(mainChrPacket.szBGMName, bgmInfo.name.c_str(), sizeof(mainChrPacket.szBGMName));
			GetDesc()->Packet(&mainChrPacket, sizeof(TPacketGCMainCharacter4_BGM_VOL));
		}
		else
		{
			sys_log(1, "bgm_info.play(%d, '%s')", mapIndex, bgmInfo.name.c_str());
			TPacketGCMainCharacter3_BGM mainChrPacket;
			mainChrPacket.header = HEADER_GC_MAIN_CHARACTER3_BGM;
			mainChrPacket.dwVID = m_vid;
			mainChrPacket.wRaceNum = GetRaceNum();
			mainChrPacket.lx = GetX();
			mainChrPacket.ly = GetY();
			mainChrPacket.lz = GetZ();
			mainChrPacket.empire = GetDesc()->GetEmpire();
			mainChrPacket.skill_group = GetSkillGroup();
			strlcpy(mainChrPacket.szChrName, GetName(), sizeof(mainChrPacket.szChrName));
			strlcpy(mainChrPacket.szBGMName, bgmInfo.name.c_str(), sizeof(mainChrPacket.szBGMName));
			GetDesc()->Packet(&mainChrPacket, sizeof(TPacketGCMainCharacter3_BGM));
		}
	}
	// END_OF_SUPPORT_BGM
	else
	{
		sys_log(0, "bgm_info.play(%d, DEFAULT_BGM_NAME)", mapIndex);

		TPacketGCMainCharacter pack;
		pack.header = HEADER_GC_MAIN_CHARACTER;
		pack.dwVID = m_vid;
		pack.wRaceNum = GetRaceNum();
		pack.lx = GetX();
		pack.ly = GetY();
		pack.lz = GetZ();
		pack.empire = GetDesc()->GetEmpire();
		pack.skill_group = GetSkillGroup();
		strlcpy(pack.szName, GetName(), sizeof(pack.szName));
		GetDesc()->Packet(&pack, sizeof(TPacketGCMainCharacter));
	}
}

void CHARACTER::PointsPacket()
{
	if (!GetDesc())
		return;

	TPacketGCPoints Packet;
	Packet.bHeader = HEADER_GC_CHARACTER_POINTS;
	Packet.lPoints[POINT_LEVEL] = GetLevel();
	Packet.lPoints[POINT_EXP] = GetExp();
	Packet.lPoints[POINT_NEXT_EXP] = GetNextExp();
	Packet.lPoints[POINT_HP] = GetHP();
	Packet.lPoints[POINT_MAX_HP] = GetMaxHP();
	Packet.lPoints[POINT_SP] = GetSP();
	Packet.lPoints[POINT_MAX_SP] = GetMaxSP();
	Packet.lPoints[POINT_GOLD] = GetGold();
	Packet.lPoints[POINT_STAMINA] = GetStamina();
	Packet.lPoints[POINT_MAX_STAMINA] = GetMaxStamina();

	for (WORD wPointType = POINT_ST; wPointType < POINT_MAX_NUM; ++wPointType)
		Packet.lPoints[wPointType] = GetPoint(wPointType);

#if defined(__CONQUEROR_LEVEL__)
	Packet.lPoints[POINT_CONQUEROR_LEVEL] = GetConquerorLevel();
	Packet.lPoints[POINT_CONQUEROR_EXP] = GetConquerorExp();
	Packet.lPoints[POINT_CONQUEROR_NEXT_EXP] = GetNextConquerorExp();

	Packet.lPoints[POINT_MOV_SPEED] = GetLimitPoint(POINT_MOV_SPEED);
#endif

#if defined(__CHEQUE_SYSTEM__)
	Packet.lPoints[POINT_CHEQUE] = GetCheque();
#endif
#if defined(__GEM_SYSTEM__)
	Packet.lPoints[POINT_GEM] = GetGem();
#endif

	GetDesc()->Packet(&Packet, sizeof(Packet));
}

void CHARACTER::UpdatePointsPacket(POINT_TYPE wPointType, POINT_VALUE lPointValue, POINT_VALUE lPointAmount, bool bAmount, bool bBroadcast)
{
	if (GetDesc())
	{
		TPacketGCPointChange Packet;
		Packet.bHeader = HEADER_GC_CHARACTER_POINT_CHANGE;
		Packet.dwVID = m_vid;
		Packet.wType = wPointType;
		Packet.lValue = lPointValue;
		Packet.lAmount = bAmount ? lPointAmount : 0;

		if (!bBroadcast)
			GetDesc()->Packet(&Packet, sizeof(Packet));
		else
			PacketAround(&Packet, sizeof(Packet));
	}
}

bool CHARACTER::ChangeSex()
{
	int src_race = GetRaceNum();

	switch (src_race)
	{
		case MAIN_RACE_WARRIOR_M:
			m_points.bJob = MAIN_RACE_WARRIOR_W;
			break;

		case MAIN_RACE_WARRIOR_W:
			m_points.bJob = MAIN_RACE_WARRIOR_M;
			break;

		case MAIN_RACE_ASSASSIN_M:
			m_points.bJob = MAIN_RACE_ASSASSIN_W;
			break;

		case MAIN_RACE_ASSASSIN_W:
			m_points.bJob = MAIN_RACE_ASSASSIN_M;
			break;

		case MAIN_RACE_SURA_M:
			m_points.bJob = MAIN_RACE_SURA_W;
			break;

		case MAIN_RACE_SURA_W:
			m_points.bJob = MAIN_RACE_SURA_M;
			break;

		case MAIN_RACE_SHAMAN_M:
			m_points.bJob = MAIN_RACE_SHAMAN_W;
			break;

		case MAIN_RACE_SHAMAN_W:
			m_points.bJob = MAIN_RACE_SHAMAN_M;
			break;

		case MAIN_RACE_WOLFMAN_M:
			m_points.bJob = MAIN_RACE_WOLFMAN_M;
			break;

		default:
			sys_err("CHANGE_SEX: %s unknown race %d", GetName(), src_race);
			return false;
	}

	sys_log(0, "CHANGE_SEX: %s (%d -> %d)", GetName(), src_race, m_points.bJob);
	return true;
}

bool CHARACTER::IsMale() const
{
	switch (GetRaceNum())
	{
		case MAIN_RACE_WARRIOR_M:
		case MAIN_RACE_SURA_M:
		case MAIN_RACE_ASSASSIN_M:
		case MAIN_RACE_SHAMAN_M:
		case MAIN_RACE_WOLFMAN_M:
			return true;
	}
	return false;
}

bool CHARACTER::IsFemale() const
{
	switch (GetRaceNum())
	{
		case MAIN_RACE_ASSASSIN_W:
		case MAIN_RACE_SHAMAN_W:
		case MAIN_RACE_WARRIOR_W:
		case MAIN_RACE_SURA_W:
			return true;
	}
	return false;
}

WORD CHARACTER::GetRaceNum() const
{
	if (m_dwPolymorphRace)
		return m_dwPolymorphRace;

	if (m_pkMobData)
		return m_pkMobData->m_table.dwVnum;

	return m_points.bJob;
}

void CHARACTER::SetRace(BYTE race)
{
	if (race >= MAIN_RACE_MAX_NUM)
	{
		sys_err("CHARACTER::SetRace(name=%s, race=%d).OUT_OF_RACE_RANGE", GetName(), race);
		return;
	}

	m_points.bJob = race;
}

BYTE CHARACTER::GetJob() const
{
	BYTE race = m_points.bJob;
	BYTE job;

	if (RaceToJob(race, &job))
		return job;

	sys_err("CHARACTER::GetJob(name=%s, race=%d).OUT_OF_RACE_RANGE", GetName(), race);
	return JOB_WARRIOR;
}

void CHARACTER::SetLevel(BYTE bValue)
{
	m_points.bLevel = bValue;

	if (IsPC())
	{
		if (bValue < PK_PROTECT_LEVEL)
			SetPKMode(PK_MODE_PROTECT);
		else if (GetGMLevel() != GM_PLAYER)
			SetPKMode(PK_MODE_PROTECT);
		else if (m_bPKMode == PK_MODE_PROTECT)
			SetPKMode(PK_MODE_PEACE);
	}
}

void CHARACTER::SetEmpire(BYTE bEmpire)
{
	m_bEmpire = bEmpire;
}

void CHARACTER::SetPlayerProto(const TPlayerTable* t)
{
	if (!GetDesc() || !*GetDesc()->GetHostName())
		sys_err("cannot get desc or hostname");
	else
		SetGMLevel();

	m_bCharType = CHAR_TYPE_PC;

	m_dwPlayerID = t->id;

	m_iAlignment = t->lAlignment;
	m_iRealAlignment = t->lAlignment;

	m_points.bVoice = t->voice;

	m_points.bSkillGroup = t->skill_group;

	m_pointsInstant.bBasePart = t->part_base;
	SetPart(PART_HAIR, t->adwParts[PART_HAIR]);
#if defined(__ACCE_COSTUME_SYSTEM__)
	SetPart(PART_ACCE, t->adwParts[PART_ACCE]);
#endif
#if defined(__AURA_COSTUME_SYSTEM__)
	SetPart(PART_AURA, t->adwParts[PART_AURA]);
#endif

	m_points.iRandomHP = t->sRandomHP;
	m_points.iRandomSP = t->sRandomSP;

	// REMOVE_REAL_SKILL_LEVLES
	if (m_pSkillLevels)
		M2_DELETE_ARRAY(m_pSkillLevels);

	m_pSkillLevels = M2_NEW TPlayerSkill[SKILL_MAX_NUM];
	thecore_memcpy(m_pSkillLevels, t->skills, sizeof(TPlayerSkill) * SKILL_MAX_NUM);
	// END_OF_REMOVE_REAL_SKILL_LEVLES

	if (t->lMapIndex >= 10000)
	{
		m_posWarp.x = t->lExitX;
		m_posWarp.y = t->lExitY;
		m_lWarpMapIndex = t->lExitMapIndex;
	}

	SetRealPoint(POINT_PLAYTIME, t->playtime);
	m_dwLoginPlayTime = t->playtime;
	SetRealPoint(POINT_ST, t->st);
	SetRealPoint(POINT_HT, t->ht);
	SetRealPoint(POINT_DX, t->dx);
	SetRealPoint(POINT_IQ, t->iq);

	SetPoint(POINT_ST, t->st);
	SetPoint(POINT_HT, t->ht);
	SetPoint(POINT_DX, t->dx);
	SetPoint(POINT_IQ, t->iq);

	SetPoint(POINT_STAT, t->stat_point);
	SetPoint(POINT_SKILL, t->skill_point);
	SetPoint(POINT_SUB_SKILL, t->sub_skill_point);
	SetPoint(POINT_HORSE_SKILL, t->horse_skill_point);

	SetPoint(POINT_STAT_RESET_COUNT, t->stat_reset_count);

	SetPoint(POINT_LEVEL_STEP, t->level_step);
	SetRealPoint(POINT_LEVEL_STEP, t->level_step);

	SetRace(t->job);

#if defined(__EXTEND_INVEN_SYSTEM__)
	SetExtendInvenStage(t->inven_stage);
#endif

	SetLevel(t->level);
	SetExp(t->exp);
	SetGold(t->gold);
#if defined(__CHEQUE_SYSTEM__)
	SetCheque(t->cheque);
#endif
#if defined(__GEM_SYSTEM__)
	SetGem(t->gem);
#endif

#if defined(__CONQUEROR_LEVEL__)
	SetRealPoint(POINT_SUNGMA_STR, t->sungma_str);
	SetRealPoint(POINT_SUNGMA_HP, t->sungma_hp);
	SetRealPoint(POINT_SUNGMA_MOVE, t->sungma_move);
	SetRealPoint(POINT_SUNGMA_IMMUNE, t->sungma_immune);

	SetPoint(POINT_SUNGMA_STR, t->sungma_str);
	SetPoint(POINT_SUNGMA_HP, t->sungma_hp);
	SetPoint(POINT_SUNGMA_MOVE, t->sungma_move);
	SetPoint(POINT_SUNGMA_IMMUNE, t->sungma_immune);

	SetPoint(POINT_CONQUEROR_POINT, t->conqueror_point);

	SetPoint(POINT_CONQUEROR_LEVEL_STEP, t->conqueror_level_step);
	SetRealPoint(POINT_CONQUEROR_LEVEL_STEP, t->conqueror_level_step);

	SetConquerorLevel(t->conqueror_level);
	SetConquerorExp(t->conqueror_exp);
#endif

	SetMapIndex(t->lMapIndex);
	SetXYZ(t->x, t->y, t->z);

	ComputePoints();

	SetHP(t->hp);
	SetSP(t->sp);
	SetStamina(t->stamina);

	// GM일때 보호모드
	if (GetGMLevel() > GM_LOW_WIZARD)
		m_afAffectFlag.Set(AFF_YMIR);

	if (GetLevel() < PK_PROTECT_LEVEL)
		m_bPKMode = PK_MODE_PROTECT;

	SetHorseData(t->horse);

	if (GetHorseLevel() > 0)
		UpdateHorseDataByLogoff(t->logoff_interval);

	thecore_memcpy(m_aiPremiumTimes, t->aiPremiumTimes, sizeof(t->aiPremiumTimes));

	m_dwLogOffInterval = t->logoff_interval;
	m_dwLastAttackTime = t->last_play;

	sys_log(0, "PLAYER_LOAD: %s PREMIUM %d %d, LOGGOFF_INTERVAL %u PTR: %p", t->name, m_aiPremiumTimes[0], m_aiPremiumTimes[1], t->logoff_interval, this);

	if (GetGMLevel() != GM_PLAYER)
	{
		LogManager::instance().CharLog(this, GetGMLevel(), "GM_LOGIN", "");
		sys_log(0, "GM_LOGIN(gmlevel=%d, name=%s(%d), pos=(%d, %d)", GetGMLevel(), GetName(), GetPlayerID(), GetX(), GetY());
	}

#if defined(__PET_SYSTEM__)
	// NOTE: 일단 캐릭터가 PC인 경우에만 PetSystem을 갖도록 함. 유럽 머신당 메모리 사용률때문에 NPC까지 하긴 좀..
	if (m_petSystem)
	{
		m_petSystem->Destroy();
		delete m_petSystem;
	}

	m_petSystem = M2_NEW CPetSystem(this);
#endif
}

EVENTFUNC(kill_ore_load_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>(event->info);
	if (info == NULL)
	{
		sys_err("kill_ore_load_even> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER ch = info->ch;

	if (ch == NULL) // <Factor>
		return 0;

	ch->m_pkMiningEvent = NULL;
	M2_DESTROY_CHARACTER(ch);
	return 0;
}

void CHARACTER::SetProto(const CMob* pkMob)
{
	if (m_pkMobInst)
		M2_DELETE(m_pkMobInst);

	m_pkMobData = pkMob;
	m_pkMobInst = M2_NEW CMobInstance;

	m_bPKMode = PK_MODE_FREE;

	const TMobTable* t = &m_pkMobData->m_table;

	m_bCharType = t->bType;

	SetLevel(t->bLevel);
	SetEmpire(t->bEmpire);

	SetExp(t->dwExp);
	SetRealPoint(POINT_ST, t->bStr);
	SetRealPoint(POINT_DX, t->bDex);
	SetRealPoint(POINT_HT, t->bCon);
	SetRealPoint(POINT_IQ, t->bInt);

#if defined(__CONQUEROR_LEVEL__)
	SetConquerorExp(t->dwSungMaExp);
	SetRealPoint(POINT_SUNGMA_STR, t->bSungMaStr);
	SetRealPoint(POINT_SUNGMA_HP, t->bSungMaDex);
	SetRealPoint(POINT_SUNGMA_MOVE, t->bSungMaCon);
	SetRealPoint(POINT_SUNGMA_IMMUNE, t->bSungMaInt);
#endif

	ComputePoints();

	SetHP(GetMaxHP());
	SetSP(GetMaxSP());

	////////////////////
	m_pointsInstant.dwAIFlag = t->dwAIFlag;
	SetImmuneFlag(t->dwImmuneFlag);

	AssignTriggers(t);

	ApplyMobAttribute(t);

	if (IsStone())
	{
		DetermineDropMetinStone();
	}

	if (IsWarp() || IsGoto())
	{
		StartWarpNPCEvent();
	}

	CHARACTER_MANAGER::instance().RegisterRaceNumMap(this);

#if defined(__XMAS_EVENT_2008__)
	// XXX X-mas santa hardcoding
	if (GetRaceNum() == xmas::MOB_XMAS_SANTA_VNUM)
	{
		SetPoint(POINT_ATT_GRADE_BONUS, 10);
		if (g_iUseLocale)
			SetPoint(POINT_DEF_GRADE_BONUS, 6);
		else
			SetPoint(POINT_DEF_GRADE_BONUS, 15);

		// 산타용
		//m_dwPlayStartTime = get_dword_time() + 10 * 60 * 1000;
		// 신선자 노해
		m_dwPlayStartTime = get_dword_time() + 30 * 1000;
		if (test_server)
			m_dwPlayStartTime = get_dword_time() + 30 * 1000;
	}
#endif

	// XXX CTF GuildWar hardcoding
	if (warmap::IsWarFlag(GetRaceNum()))
	{
		m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlag, &CHARACTER::EndStateEmpty);
		m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlag, &CHARACTER::EndStateEmpty);
		m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlag, &CHARACTER::EndStateEmpty);
	}

	if (warmap::IsWarFlagBase(GetRaceNum()))
	{
		m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlagBase, &CHARACTER::EndStateEmpty);
		m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlagBase, &CHARACTER::EndStateEmpty);
		m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlagBase, &CHARACTER::EndStateEmpty);
	}

	if (IsHorse() ||
		GetRaceNum() == 20101 ||
		GetRaceNum() == 20102 ||
		GetRaceNum() == 20103 ||
		GetRaceNum() == 20104 ||
		GetRaceNum() == 20105 ||
		GetRaceNum() == 20106 ||
		GetRaceNum() == 20107 ||
		GetRaceNum() == 20108 ||
		GetRaceNum() == 20109
		)
	{
		m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateHorse, &CHARACTER::EndStateEmpty);
		m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateMove, &CHARACTER::EndStateEmpty);
		m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateHorse, &CHARACTER::EndStateEmpty);
	}

	// MINING
	if (mining::IsVeinOfOre(GetRaceNum()))
	{
		char_event_info* info = AllocEventInfo<char_event_info>();

		info->ch = this;

		m_pkMiningEvent = event_create(kill_ore_load_event, info, PASSES_PER_SEC(number(7 * 60, 15 * 60)));
	}
	// END_OF_MINING
}

const TMobTable& CHARACTER::GetMobTable() const
{
	return m_pkMobData->m_table;
}

BYTE CHARACTER::GetMobRank() const
{
	if (!m_pkMobData)
		return MOB_RANK_KNIGHT; // PC일 경우 KNIGHT급

	return m_pkMobData->m_table.bRank;
}

BYTE CHARACTER::GetMobType() const
{
	if (!m_pkMobData)
		return CHAR_TYPE_MONSTER;

	return m_pkMobData->m_table.bType;
}

BYTE CHARACTER::GetMobBattleType() const
{
	if (!m_pkMobData)
		return BATTLE_TYPE_MELEE;

	return m_pkMobData->m_table.bBattleType;
}

BYTE CHARACTER::GetMobSize() const
{
	if (!m_pkMobData)
		return MOBSIZE_MEDIUM;

	return m_pkMobData->m_table.bSize;
}

DWORD CHARACTER::GetMobDamageMin() const
{
	return m_pkMobData ? m_pkMobData->m_table.dwDamageRange[0] : 0;
}

DWORD CHARACTER::GetMobDamageMax() const
{
	return m_pkMobData ? m_pkMobData->m_table.dwDamageRange[1] : 0;
}

WORD CHARACTER::GetMobAttackRange() const
{
	WORD wAttackRange = m_pkMobData ? m_pkMobData->m_table.wAttackRange : 0;
	float fHitRange = m_pkMobData ? m_pkMobData->m_table.fHitRange : 0;

	switch (GetMobBattleType())
	{
		case BATTLE_TYPE_RANGE:
		case BATTLE_TYPE_MAGIC:
		{
#if defined(__DEFENSE_WAVE__)
			if (m_pkMobData && CDefenseWaveManager::Instance().IsHydra(GetRaceNum()) && GetDefenseWave())
				return wAttackRange + GetPoint(POINT_BOW_DISTANCE) + fHitRange + 2000;
#endif
			return wAttackRange + GetPoint(POINT_BOW_DISTANCE) + fHitRange;
		}
		default:
		{
#if defined(__DEFENSE_WAVE__)
			if (m_pkMobData && GetDefenseWave())
				return wAttackRange + fHitRange + 300;
#endif
			return wAttackRange + fHitRange;
		}
	}
}

DWORD CHARACTER::GetMobDropItemVnum() const
{
	return m_pkMobData ? m_pkMobData->m_table.dwDropItemVnum : 0;
}

float CHARACTER::GetMobDamageMultiply() const
{
	float fDamMultiply = GetMobTable().fDamMultiply;

	if (IsBerserk())
		fDamMultiply = fDamMultiply * 2.0f; // BALANCE: 광폭화 시 두배

	return fDamMultiply;
}

#if defined(__ELEMENT_SYSTEM__)
int CHARACTER::GetMobElement(BYTE bElement) const
{
	if (m_pkMobData && bElement >= MOB_ELEMENT_ELECT && bElement < MOB_ELEMENT_MAX_NUM)
		return m_pkMobData->m_table.cElements[bElement];
	return 0;
}
#endif

float CHARACTER::GetMonsterHitRange() const
{
	if (!m_pkMobData)
		return 70.0f;

	if (m_pkMobData->m_table.fHitRange)
		return m_pkMobData->m_table.fHitRange;

	return 100.0f;
}

bool CHARACTER::IsRaceFlag(DWORD dwBit) const
{
	return m_pkMobData ? IS_SET(m_pkMobData->m_table.dwRaceFlag, dwBit) : 0;
}

bool CHARACTER::IsSummonMonster() const
{
	return GetSummonVnum() != 0;
}

DWORD CHARACTER::GetSummonVnum() const
{
	return m_pkMobData ? m_pkMobData->m_table.dwSummonVnum : 0;
}

bool CHARACTER::CanSummonMonster() const
{
	if (!IsSummonMonster())
		return false;

	return (thecore_pulse() > m_lastSummonTime + PASSES_PER_SEC(m_newSummonInterval));
}

void CHARACTER::MarkSummonedMonster()
{
	m_lastSummonTime = thecore_pulse();
	m_newSummonInterval = number(g_npcGroupRespawnRange[0], g_npcGroupRespawnRange[1]);
}

DWORD CHARACTER::GetPolymorphItemVnum() const
{
	return m_pkMobData ? m_pkMobData->m_table.dwPolymorphItemVnum : 0;
}

DWORD CHARACTER::GetMonsterDrainSPPoint() const
{
	return m_pkMobData ? m_pkMobData->m_table.dwDrainSP : 0;
}

void CHARACTER::ComputeBattlePoints()
{
	if (IsPolymorphed())
	{
		DWORD dwMobVnum = GetPolymorphVnum();
		const CMob* pMob = CMobManager::instance().Get(dwMobVnum);
		int iAtt = 0;
		int iDef = 0;

		if (pMob)
		{
			iAtt = GetLevel() * 2 + GetPolymorphPoint(POINT_ST) * 2;
			// lev + con
			iDef = GetLevel() + GetPolymorphPoint(POINT_HT) + pMob->m_table.wDef;
		}

		SetPoint(POINT_ATT_GRADE, iAtt);
		SetPoint(POINT_DEF_GRADE, iDef);
		SetPoint(POINT_MAGIC_ATT_GRADE, GetPoint(POINT_ATT_GRADE));
		SetPoint(POINT_MAGIC_DEF_GRADE, GetPoint(POINT_DEF_GRADE));
	}
	else if (IsPC())
	{
		SetPoint(POINT_ATT_GRADE, 0);
		SetPoint(POINT_DEF_GRADE, 0);
		SetPoint(POINT_CLIENT_DEF_GRADE, 0);
		SetPoint(POINT_MAGIC_ATT_GRADE, GetPoint(POINT_ATT_GRADE));
		SetPoint(POINT_MAGIC_DEF_GRADE, GetPoint(POINT_DEF_GRADE));

		//
		// 기본 ATK = 2lev + 2str, 직업에 마다 2str은 바뀔 수 있음
		//
		int iAtk = GetLevel() * 2;
		int iStatAtk = 0;

		switch (GetJob())
		{
			case JOB_WARRIOR:
			case JOB_SURA:
				iStatAtk = (4 * GetPoint(POINT_ST) + 2 * GetPoint(POINT_DX)) / 3;
				break;

			case JOB_ASSASSIN:
				iStatAtk = (5 * GetPoint(POINT_DX) + 1 * GetPoint(POINT_ST)) / 3;
				break;

			case JOB_SHAMAN:
				iStatAtk = (5 * GetPoint(POINT_IQ) + 1 * GetPoint(POINT_DX)) / 3;
				break;

			case JOB_WOLFMAN:
				iStatAtk = (7 * GetPoint(POINT_DX) + 2 * GetPoint(POINT_HT)) / 3;
				break;

			default:
				sys_err("invalid job %d", GetJob());
				iStatAtk = (2 * GetPoint(POINT_ST));
				break;
		}

		// 말을 타고 있고, 스탯으로 인한 공격력이 ST*2 보다 낮으면 ST*2로 한다.
		// 스탯을 잘못 찍은 사람 공격력이 더 낮지 않게 하기 위해서다.
		if (GetMountVnum() && iStatAtk < 2 * GetPoint(POINT_ST))
			iStatAtk = (2 * GetPoint(POINT_ST));

		iAtk += iStatAtk;

		// 승마(말) : 검수라 데미지 감소
		if (GetMountVnum())
		{
			if (GetJob() == JOB_SURA && GetSkillGroup() == 1)
			{
				iAtk += (iAtk * GetHorseLevel()) / 60;
			}
			else
			{
				iAtk += (iAtk * GetHorseLevel()) / 30;
			}
		}

		//
		// ATK Setting
		//
		iAtk += GetPoint(POINT_ATT_GRADE_BONUS);

		PointChange(POINT_ATT_GRADE, iAtk);

		// DEF = LEV + CON + ARMOR
		int iShowDef = GetLevel() + GetPoint(POINT_HT); // For Ymir(천마)
		int iDef = GetLevel() + static_cast<int>((GetPoint(POINT_HT) / 1.25)); // For Other
		int iArmor = 0;

		LPITEM pItem = nullptr;
		for (BYTE bWearIndex = 0; bWearIndex < WEAR_MAX_NUM; ++bWearIndex)
		{
			if ((pItem = GetWear(bWearIndex)) && pItem->GetType() == ITEM_ARMOR)
			{
				if (pItem->GetSubType() == ARMOR_BODY
					|| pItem->GetSubType() == ARMOR_HEAD
					|| pItem->GetSubType() == ARMOR_FOOTS
					|| pItem->GetSubType() == ARMOR_SHIELD
#if defined(__PENDANT_SYSTEM__)
					|| pItem->GetSubType() == ARMOR_PENDANT
#endif
#if defined(__GLOVE_SYSTEM__)
					|| pItem->GetSubType() == ARMOR_GLOVE
#endif
					)
				{
#if defined(__ITEM_APPLY_RANDOM__) && defined(__ITEM_VALUE10__)
					DWORD dwArmorVal = pItem->GetValue(1);

					const DWORD dwMinMax = pItem->GetSocket(ITEM_SOCKET_DEF_MINMAX_RANDOM);
					if (dwMinMax != 0)
						dwArmorVal = dwMinMax / ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE;

					iArmor += dwArmorVal;
#else
					iArmor += pItem->GetValue(1);
#endif
					iArmor += (2 * pItem->GetValue(5));
				}
			}
#if defined(__ITEM_APPLY_RANDOM__) && defined(__ITEM_VALUE10__)
			else if (pItem && pItem->GetType() == ITEM_BELT)
			{
				DWORD dwArmorVal = pItem->GetValue(1);

				const DWORD dwMinMax = pItem->GetSocket(ITEM_SOCKET_DEF_MINMAX_RANDOM);
				if (dwMinMax != 0)
					dwArmorVal = dwMinMax / ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE;

				iArmor += dwArmorVal;
				iArmor += (2 * pItem->GetValue(5));
			}
#endif
#if defined(__AURA_COSTUME_SYSTEM__)
			else if (pItem && pItem->GetType() == ITEM_COSTUME && pItem->GetSubType() == COSTUME_AURA)
			{
				const DWORD dwLevelSocket = pItem->GetSocket(ITEM_SOCKET_AURA_LEVEL_VALUE);
				const DWORD dwDrainSocket = pItem->GetSocket(ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM);

				BYTE bCurLevel = (dwLevelSocket / 100000) - 1000;
				float fAuraDrainPer = (1.0f * bCurLevel / 10.0f) / 100.0f;

				TItemTable* pDrainedItem = nullptr;
				if (dwDrainSocket != 0)
					pDrainedItem = ITEM_MANAGER::instance().GetTable(dwDrainSocket);

				if (pDrainedItem != nullptr && pDrainedItem->bType == ITEM_ARMOR && pDrainedItem->bSubType == ARMOR_SHIELD)
				{
#if defined(__ITEM_APPLY_RANDOM__) && defined(__ITEM_VALUE10__)
					DWORD dwArmorVal = pDrainedItem->GetValue(1);

					const DWORD dwMinMax = pItem->GetSocket(ITEM_SOCKET_DEF_MINMAX_RANDOM);
					if (dwMinMax != 0)
						dwArmorVal = dwMinMax / ITEM_VALUE_MINMAX_RANDOM_DIVISION_VALUE;

					float fValue = (dwArmorVal + (2 * pDrainedItem->GetValue(5))) * fAuraDrainPer;
#else
					float fValue = (pDrainedItem->GetValue(1) + (2 * pDrainedItem->GetValue(5))) * fAuraDrainPer;
#endif
					iArmor += static_cast<int>((fValue < 1.0f) ? ceilf(fValue) : truncf(fValue));
				}
			}
#endif
		}

		// 말 타고 있을 때 방어력이 말의 기준 방어력보다 낮으면 기준 방어력으로 설정
		if (IsHorseRiding())
		{
			if (iArmor < GetHorseArmor())
				iArmor = GetHorseArmor();

			const char* pHorseName = CHorseNameManager::instance().GetHorseName(GetPlayerID());
			if (pHorseName != NULL && strlen(pHorseName))
			{
				iArmor += 20;
			}
		}

		iArmor += GetPoint(POINT_DEF_GRADE_BONUS);
		iArmor += GetPoint(POINT_PARTY_DEFENDER_BONUS);

		PointChange(POINT_DEF_GRADE, iDef + iArmor);
		PointChange(POINT_CLIENT_DEF_GRADE, (iShowDef + iArmor) - GetPoint(POINT_DEF_GRADE));
		PointChange(POINT_MAGIC_ATT_GRADE, GetLevel() * 2 + GetPoint(POINT_IQ) * 2 + GetPoint(POINT_MAGIC_ATT_GRADE_BONUS));
		PointChange(POINT_MAGIC_DEF_GRADE, GetLevel() + (GetPoint(POINT_IQ) * 3 + GetPoint(POINT_HT)) / 3 + iArmor / 2 + GetPoint(POINT_MAGIC_DEF_GRADE_BONUS));
	}
	else
	{
		// 2lev + str * 2
		int iAtt = GetLevel() * 2 + GetPoint(POINT_ST) * 2;
		// lev + con
		int iDef = GetLevel() + GetPoint(POINT_HT) + GetMobTable().wDef;

		SetPoint(POINT_ATT_GRADE, iAtt);
		SetPoint(POINT_DEF_GRADE, iDef);
		SetPoint(POINT_MAGIC_ATT_GRADE, GetPoint(POINT_ATT_GRADE));
		SetPoint(POINT_MAGIC_DEF_GRADE, GetPoint(POINT_DEF_GRADE));
	}
}

void CHARACTER::ComputePoints()
{
	long lStat = GetPoint(POINT_STAT);
	long lStatResetCount = GetPoint(POINT_STAT_RESET_COUNT);
	long lSkillActive = GetPoint(POINT_SKILL);
	long lSkillSub = GetPoint(POINT_SUB_SKILL);
	long lSkillHorse = GetPoint(POINT_HORSE_SKILL);
	long lLevelStep = GetPoint(POINT_LEVEL_STEP);

	long lAttackerBonus = GetPoint(POINT_PARTY_ATTACKER_BONUS);
	long lTankerBonus = GetPoint(POINT_PARTY_TANKER_BONUS);
	long lBufferBonus = GetPoint(POINT_PARTY_BUFFER_BONUS);
	long lSkillMasterBonus = GetPoint(POINT_PARTY_SKILL_MASTER_BONUS);
	long lHasteBonus = GetPoint(POINT_PARTY_HASTE_BONUS);
	long lDefenderBonus = GetPoint(POINT_PARTY_DEFENDER_BONUS);

	long lHPRecovery = GetPoint(POINT_HP_RECOVERY);
	long lSPRecovery = GetPoint(POINT_SP_RECOVERY);

#if defined(__CONQUEROR_LEVEL__)
	long lConquerorPoint = GetPoint(POINT_CONQUEROR_POINT);
	long lConquerorLevelStep = GetPoint(POINT_CONQUEROR_LEVEL_STEP);

	long lSungmaStr = GetRealPoint(POINT_SUNGMA_STR);
	long lSungmaHP = GetRealPoint(POINT_SUNGMA_HP);
	long lSungmaMove = GetRealPoint(POINT_SUNGMA_MOVE);
	long lSungmaImmune = GetRealPoint(POINT_SUNGMA_IMMUNE);
#endif

	memset(m_pointsInstant.points, 0, sizeof(m_pointsInstant.points));
	BuffOnAttr_ClearAll();
	m_SkillDamageBonus.clear();

	SetPoint(POINT_STAT, lStat);
	SetPoint(POINT_SKILL, lSkillActive);
	SetPoint(POINT_SUB_SKILL, lSkillSub);
	SetPoint(POINT_HORSE_SKILL, lSkillHorse);
	SetPoint(POINT_LEVEL_STEP, lLevelStep);
	SetPoint(POINT_STAT_RESET_COUNT, lStatResetCount);

	SetPoint(POINT_ST, GetRealPoint(POINT_ST));
	SetPoint(POINT_HT, GetRealPoint(POINT_HT));
	SetPoint(POINT_DX, GetRealPoint(POINT_DX));
	SetPoint(POINT_IQ, GetRealPoint(POINT_IQ));

#if defined(__CONQUEROR_LEVEL__)
	SetPoint(POINT_CONQUEROR_POINT, lConquerorPoint);
	SetPoint(POINT_CONQUEROR_LEVEL_STEP, lConquerorLevelStep);

	SetPoint(POINT_SUNGMA_STR, lSungmaStr);
	SetPoint(POINT_SUNGMA_HP, lSungmaHP),
	SetPoint(POINT_SUNGMA_MOVE, lSungmaMove);
	SetPoint(POINT_SUNGMA_IMMUNE, lSungmaImmune);
#endif

	SetPart(PART_MAIN, GetOriginalPart(PART_MAIN));
	SetPart(PART_WEAPON, GetOriginalPart(PART_WEAPON));
	SetPart(PART_HEAD, GetOriginalPart(PART_HEAD));
	SetPart(PART_HAIR, GetOriginalPart(PART_HAIR));
#if defined(__ACCE_COSTUME_SYSTEM__)
	SetPart(PART_ACCE, GetOriginalPart(PART_ACCE));
#endif
#if defined(__AURA_COSTUME_SYSTEM__)
	SetPart(PART_AURA, GetOriginalPart(PART_AURA));
#endif

	SetPoint(POINT_PARTY_ATTACKER_BONUS, lAttackerBonus);
	SetPoint(POINT_PARTY_TANKER_BONUS, lTankerBonus);
	SetPoint(POINT_PARTY_BUFFER_BONUS, lBufferBonus);
	SetPoint(POINT_PARTY_SKILL_MASTER_BONUS, lSkillMasterBonus);
	SetPoint(POINT_PARTY_HASTE_BONUS, lHasteBonus);
	SetPoint(POINT_PARTY_DEFENDER_BONUS, lDefenderBonus);

	SetPoint(POINT_HP_RECOVERY, lHPRecovery);
	SetPoint(POINT_SP_RECOVERY, lSPRecovery);

	// PC_BANG_ITEM_ADD
	SetPoint(POINT_PC_BANG_EXP_BONUS, 0);
	SetPoint(POINT_PC_BANG_DROP_BONUS, 0);
	// END_PC_BANG_ITEM_ADD

	int iMaxHP, iMaxSP;
	int iMaxStamina;

	if (IsPC())
	{
		// 최대 생명력/정신력
		iMaxHP = JobInitialPoints[GetJob()].max_hp + m_points.iRandomHP + GetPoint(POINT_HT) * JobInitialPoints[GetJob()].hp_per_ht;
		iMaxSP = JobInitialPoints[GetJob()].max_sp + m_points.iRandomSP + GetPoint(POINT_IQ) * JobInitialPoints[GetJob()].sp_per_iq;
		iMaxStamina = JobInitialPoints[GetJob()].max_stamina + GetPoint(POINT_HT) * JobInitialPoints[GetJob()].stamina_per_con;

		CSkillProto* pkSk = CSkillManager::instance().Get(SKILL_ADD_HP);
		if (pkSk)
		{
			pkSk->SetPointVar("k", 1.0f * GetSkillPower(SKILL_ADD_HP) / 100.0f);
			iMaxHP += static_cast<int>(pkSk->kPointPoly.Eval());
		}

		// 기본 값들
		SetPoint(POINT_MOV_SPEED, 100);
		SetPoint(POINT_ATT_SPEED, 100);
		PointChange(POINT_ATT_SPEED, GetPoint(POINT_PARTY_HASTE_BONUS));
		SetPoint(POINT_CASTING_SPEED, 100);
	}
	else
	{
		iMaxHP = m_pkMobData->m_table.dwMaxHP;
		iMaxSP = 0;
		iMaxStamina = 0;

		SetPoint(POINT_ATT_SPEED, m_pkMobData->m_table.sAttackSpeed);
		SetPoint(POINT_MOV_SPEED, m_pkMobData->m_table.sMovingSpeed);
		SetPoint(POINT_CASTING_SPEED, m_pkMobData->m_table.sAttackSpeed);
	}

	if (IsPC())
	{
		// 말 타고 있을 때는 기본 스탯이 말의 기준 스탯보다 낮으면 높게 만든다.
		// 따라서 말의 기준 스탯이 무사 기준이므로, 수라/무당은 전체 스탯 합이
		// 대채적으로 더 올라가게 될 것이다.
		if (GetMountVnum())
		{
			if (GetHorseST() > GetPoint(POINT_ST))
				PointChange(POINT_ST, GetHorseST() - GetPoint(POINT_ST));

			if (GetHorseDX() > GetPoint(POINT_DX))
				PointChange(POINT_DX, GetHorseDX() - GetPoint(POINT_DX));

			if (GetHorseHT() > GetPoint(POINT_HT))
				PointChange(POINT_HT, GetHorseHT() - GetPoint(POINT_HT));

			if (GetHorseIQ() > GetPoint(POINT_IQ))
				PointChange(POINT_IQ, GetHorseIQ() - GetPoint(POINT_IQ));
		}
	}

	ComputeBattlePoints();

	// 기본 HP/SP 설정
	if (iMaxHP != GetMaxHP())
		SetRealPoint(POINT_MAX_HP, iMaxHP); // 기본HP를 RealPoint에 저장해 놓는다.
	PointChange(POINT_MAX_HP, 0);

	if (iMaxSP != GetMaxSP())
		SetRealPoint(POINT_MAX_SP, iMaxSP); // 기본SP를 RealPoint에 저장해 놓는다.
	PointChange(POINT_MAX_SP, 0);

	SetMaxStamina(iMaxStamina);

	m_pointsInstant.dwImmuneFlag = 0;

	for (BYTE bWearIndex = 0; bWearIndex < WEAR_MAX_NUM; bWearIndex++)
	{
		LPITEM pItem = GetWear(bWearIndex);
		if (pItem != nullptr)
		{
			pItem->ModifyPoints(true);
			SET_BIT(m_pointsInstant.dwImmuneFlag, GetWear(bWearIndex)->GetImmuneFlag());
		}
	}

	// 용혼석 시스템
	// ComputePoints에서는 케릭터의 모든 속성값을 초기화하고,
	// 아이템, 버프 등에 관련된 모든 속성값을 재계산하기 때문에,
	// 용혼석 시스템도 ActiveDeck에 있는 모든 용혼석의 속성값을 다시 적용시켜야 한다.
#if defined(__DRAGON_SOUL_SYSTEM__)
	if (DragonSoul_IsDeckActivated())
	{
		for (BYTE bWearIndex = WEAR_MAX_NUM + DS_SLOT_MAX * DragonSoul_GetActiveDeck();
			bWearIndex < WEAR_MAX_NUM + DS_SLOT_MAX * (DragonSoul_GetActiveDeck() + 1); bWearIndex++)
		{
			LPITEM pItem = GetWear(bWearIndex);
			if (pItem)
			{
				if (DSManager::instance().IsTimeLeftDragonSoul(pItem))
					pItem->ModifyPoints(true);
			}
		}
	}
#endif

	//int iCurHP = this->GetHP();
	//int iCurSP = this->GetSP();

	ComputeSkillPoints();

	RefreshAffect();

	if (GetHP() > GetMaxHP())
		PointChange(POINT_HP, GetMaxHP() - GetHP());

	if (GetSP() > GetMaxSP())
		PointChange(POINT_SP, GetMaxSP() - GetSP());

#if defined(__PET_SYSTEM__)
	CPetSystem* pPetSystem = GetPetSystem();
	if (pPetSystem != nullptr)
		pPetSystem->RefreshBuff();
#endif

	for (const auto& it : m_map_buff_on_attrs)
		it.second->GiveAllAttributes();

	/*
	if (IsPC())
	{
		if (this->GetHP() != iCurHP)
			this->PointChange(POINT_HP, iCurHP - this->GetHP());

		if (this->GetSP() != iCurSP)
			this->PointChange(POINT_SP, iCurSP - this->GetSP());
	}
	*/

	PointChange(POINT_MAX_HP, 0);
	PointChange(POINT_MAX_SP, 0);

	UpdatePacket();
}

// m_dwPlayStartTime의 단위는 milisecond다. 데이터베이스에는 분단위로 기록하기
// 때문에 플레이시간을 계산할 때 / 60000 으로 나눠서 하는데, 그 나머지 값이 남았
// 을 때 여기에 dwTimeRemain으로 넣어서 제대로 계산되도록 해주어야 한다.
void CHARACTER::ResetPlayTime(DWORD dwTimeRemain)
{
	m_dwPlayStartTime = get_dword_time() - dwTimeRemain;
}

const int aiRecoveryPercents[10] = { 1, 5, 5, 5, 5, 5, 5, 5, 5, 5 };

EVENTFUNC(recovery_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>(event->info);
	if (info == NULL)
	{
		sys_err("recovery_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER ch = info->ch;

	if (ch == NULL) // <Factor>
		return 0;

	if (!ch->IsPC())
	{
		//
		// 몬스터 회복
		//
		if (ch->IsAffectFlag(AFF_POISON))
			return PASSES_PER_SEC(MAX(1, ch->GetMobTable().bRegenCycle));

		if (ch->IsAffectFlag(AFF_BLEEDING))
			return PASSES_PER_SEC(MAX(1, ch->GetMobTable().bRegenCycle));

		DWORD vnum = ch->GetMobTable().dwVnum;
#if defined(__BLUE_DRAGON_RENEWAL__)
		if (BlueDragon_IsBoss(vnum))
#else
		if (vnum == BlueDragon::BossVnum) // Aqua Dragon
#endif
		{
			int regenPct = BlueDragon_GetRangeFactor("hp_regen", ch->GetHPPct());
			regenPct += ch->GetMobTable().bRegenPercent;

			for (int i = 1; i <= BlueDragon::StoneCount; ++i)
			{
				if (REGEN_PECT_BONUS == BlueDragon_GetIndexFactor("DragonStone", i, "effect_type"))
				{
#if defined(__LABYRINTH_DUNGEON__)
					DWORD dwDragonStoneID = BlueDragon_GetIndexFactor("DragonStone", i,
						BlueDragon_GetVnumFieldByBossVnum(vnum).c_str());
#else
					DWORD dwDragonStoneID = BlueDragon_GetIndexFactor("DragonStone", i, "vnum");
#endif
					size_t val = BlueDragon_GetIndexFactor("DragonStone", i, "val");
					size_t cnt = SECTREE_MANAGER::instance().GetMonsterCountInMap(ch->GetMapIndex(), dwDragonStoneID);

					regenPct += (val * cnt);

					break;
				}
			}

			ch->MonsterLog("AQUA_HP_REGEN +%d", MAX(1, (ch->GetMaxHP() * regenPct) / 100));
			ch->PointChange(POINT_HP, MAX(1, (ch->GetMaxHP() * regenPct) / 100));
		}
		else if (!ch->IsDoor())
		{
			ch->MonsterLog("HP_REGEN +%d", MAX(1, (ch->GetMaxHP() * ch->GetMobTable().bRegenPercent) / 100));
			ch->PointChange(POINT_HP, MAX(1, (ch->GetMaxHP() * ch->GetMobTable().bRegenPercent) / 100));
		}

		if (ch->GetHP() >= ch->GetMaxHP())
		{
			ch->m_pkRecoveryEvent = NULL;
			return 0;
		}

#if defined(__BLUE_DRAGON_RENEWAL__)
		if (BlueDragon_IsBoss(ch->GetMobTable().dwVnum))
#else
		if (BlueDragon::BossVnum == ch->GetMobTable().dwVnum)
#endif
		{
			for (int i = 1; i <= BlueDragon::StoneCount; ++i)
			{
				if (REGEN_TIME_BONUS == BlueDragon_GetIndexFactor("DragonStone", i, "effect_type"))
				{
#if defined(__LABYRINTH_DUNGEON__)
					DWORD dwDragonStoneID = BlueDragon_GetIndexFactor("DragonStone", i,
						BlueDragon_GetVnumFieldByBossVnum(ch->GetMobTable().dwVnum).c_str());
#else
					DWORD dwDragonStoneID = BlueDragon_GetIndexFactor("DragonStone", i, "vnum");
#endif
					size_t val = BlueDragon_GetIndexFactor("DragonStone", i, "val");
					size_t cnt = SECTREE_MANAGER::instance().GetMonsterCountInMap(ch->GetMapIndex(), dwDragonStoneID);

					return PASSES_PER_SEC(MAX(1, (ch->GetMobTable().bRegenCycle - (val * cnt))));
				}
			}
		}

		return PASSES_PER_SEC(MAX(1, ch->GetMobTable().bRegenCycle));
	}
	else
	{
		//
		// PC 회복
		//
		ch->CheckTarget();
		//ch->UpdateSectree(); // 여기서 이걸 왜하지?
		ch->UpdateKillerMode();

		if (ch->IsAffectFlag(AFF_POISON) == true)
		{
			// 중독인 경우 자동회복 금지
			// 파법술인 경우 자동회복 금지
			return 3;
		}

		if (ch->IsAffectFlag(AFF_BLEEDING))
			return 3;

		int iSec = (get_dword_time() - ch->GetLastMoveTime()) / 3000;

		// SP 회복 루틴.
		// 왜 이걸로 해서 함수로 빼놨는가 ?!
		ch->DistributeSP(ch);

		if (ch->GetMaxHP() <= ch->GetHP())
			return PASSES_PER_SEC(3);

		int iPercent = 0;
		int iAmount = 0;

		{
			iPercent = aiRecoveryPercents[MIN(9, iSec)];
			iAmount = 15 + (ch->GetMaxHP() * iPercent) / 100;
		}

		iAmount += (iAmount * ch->GetPoint(POINT_HP_REGEN)) / 100;

		sys_log(1, "RECOVERY_EVENT: %s %d HP_REGEN %d HP +%lld", ch->GetName(), iPercent, ch->GetPoint(POINT_HP_REGEN), iAmount);

		ch->PointChange(POINT_HP, iAmount, false);
		return PASSES_PER_SEC(3);
	}
}

void CHARACTER::StartRecoveryEvent()
{
	if (m_pkRecoveryEvent)
		return;

	if (IsDead() || IsStun())
		return;

	if (IsNPC() && GetHP() >= GetMaxHP()) // 몬스터는 체력이 다 차있으면 시작 안한다.
		return;

	if (IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_NORECOVERY))
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();
	info->ch = this;

	int iSec = IsPC() ? 3 : (MAX(1, GetMobTable().bRegenCycle));
	m_pkRecoveryEvent = event_create(recovery_event, info, PASSES_PER_SEC(iSec));
}

void CHARACTER::Standup()
{
	struct packet_position pack_position;

	if (!IsPosition(POS_SITTING))
		return;

	SetPosition(POS_STANDING);

	sys_log(1, "STANDUP: %s", GetName());

	pack_position.header = HEADER_GC_CHARACTER_POSITION;
	pack_position.vid = GetVID();
	pack_position.position = POSITION_GENERAL;

	PacketAround(&pack_position, sizeof(pack_position));
}

void CHARACTER::Sitdown(int is_ground)
{
	struct packet_position pack_position;

	if (IsPosition(POS_SITTING))
		return;

	SetPosition(POS_SITTING);
	sys_log(1, "SITDOWN: %s", GetName());

	pack_position.header = HEADER_GC_CHARACTER_POSITION;
	pack_position.vid = GetVID();
	pack_position.position = POSITION_SITTING_GROUND;
	PacketAround(&pack_position, sizeof(pack_position));
}

void CHARACTER::SetRotation(float fRot)
{
	m_pointsInstant.fRot = fRot;
}

// x, y 방향으로 보고 선다.
void CHARACTER::SetRotationToXY(long x, long y)
{
	SetRotation(GetDegreeFromPositionXY(GetX(), GetY(), x, y));
}

bool CHARACTER::CannotMoveByAffect() const
{
	return (IsAffectFlag(AFF_STUN));
}

bool CHARACTER::CanMove() const
{
	if (CannotMoveByAffect())
		return false;

	if (GetMyShop()) // 상점 연 상태에서는 움직일 수 없음
		return false;

	// 0.2초 전이라면 움직일 수 없다.
	//if (get_float_time() - m_fSyncTime < 0.2f)
	//	return false;

	return true;
}

// 무조건 x, y 위치로 이동 시킨다.
bool CHARACTER::Sync(long x, long y)
{
	if (!GetSectree())
		return false;

	if (IsPC() && IsDead())
		return false;

	LPSECTREE new_tree = SECTREE_MANAGER::instance().Get(GetMapIndex(), x, y);
	if (!new_tree)
	{
		if (GetDesc())
		{
			sys_err("cannot find tree at %d %d (name: %s)", x, y, GetName());
			GetDesc()->SetPhase(PHASE_CLOSE);
		}
		else
		{
			sys_err("no tree: %s %d %d %d", GetName(), x, y, GetMapIndex());
			Dead();
		}

		return false;
	}

	SetRotationToXY(x, y);
	SetXYZ(x, y, 0);

	if (GetDungeon())
	{
		// 던젼용 이벤트 속성 변화
		int iLastEventAttr = m_iEventAttr;
		m_iEventAttr = new_tree->GetEventAttribute(x, y);

		if (m_iEventAttr != iLastEventAttr)
		{
			if (GetParty())
			{
				quest::CQuestManager::instance().AttrOut(GetParty()->GetLeaderPID(), this, iLastEventAttr);
				quest::CQuestManager::instance().AttrIn(GetParty()->GetLeaderPID(), this, m_iEventAttr);
			}
			else
			{
				quest::CQuestManager::instance().AttrOut(GetPlayerID(), this, iLastEventAttr);
				quest::CQuestManager::instance().AttrIn(GetPlayerID(), this, m_iEventAttr);
			}
		}
	}

#if defined(__DEFENSE_WAVE__)
	if (GetDefenseWave())
		GetDefenseWave()->CheckAffect(this, x, y);
#endif

	if (GetSectree() != new_tree)
	{
		if (test_server)
			if (!IsNPC())
			{
				SECTREEID id = new_tree->GetID();
				SECTREEID old_id = GetSectree()->GetID();

				const float fDist = DISTANCE_SQRT(id.coord.x - old_id.coord.x, id.coord.y - old_id.coord.y);
				sys_log(0, "SECTREE DIFFER: %s %dx%d was %dx%d dist %.1fm",
					GetName(),
					id.coord.x,
					id.coord.y,
					old_id.coord.x,
					old_id.coord.y,
					fDist);
			}

		new_tree->InsertEntity(this);
	}

	return true;
}

void CHARACTER::Stop()
{
	if (!IsState(m_stateIdle))
		MonsterLog("[IDLE] 정지");

	GotoState(m_stateIdle);

	m_posDest.x = m_posStart.x = GetX();
	m_posDest.y = m_posStart.y = GetY();
}

bool CHARACTER::Goto(long x, long y)
{
	// TODO 거리체크 필요
	// 같은 위치면 이동할 필요 없음 (자동 성공)
	if (GetX() == x && GetY() == y)
		return false;

	if (m_posDest.x == x && m_posDest.y == y)
	{
		if (!IsState(m_stateMove))
		{
			m_dwStateDuration = 4;
			GotoState(m_stateMove);
		}
		return false;
	}

	m_posDest.x = x;
	m_posDest.y = y;

	CalculateMoveDuration();

	m_dwStateDuration = 4;

	if (!IsState(m_stateMove))
	{
		MonsterLog("[MOVE] %s", GetVictim() ? "To victim" : "Free");

		if (GetVictim())
		{
			//MonsterChat(MONSTER_CHAT_CHASE);
			//MonsterChat(MONSTER_CHAT_ATTACK);
		}
	}

	GotoState(m_stateMove);

	return true;
}

DWORD CHARACTER::GetMotionMode() const
{
	DWORD dwMode = MOTION_MODE_GENERAL;

	if (IsPolymorphed())
		return dwMode;

	LPITEM pkItem;

	if ((pkItem = GetWear(WEAR_WEAPON)))
	{
		switch (pkItem->GetProto()->bSubType)
		{
			case WEAPON_SWORD:
				dwMode = MOTION_MODE_ONEHAND_SWORD;
				break;

			case WEAPON_TWO_HANDED:
				dwMode = MOTION_MODE_TWOHAND_SWORD;
				break;

			case WEAPON_DAGGER:
				dwMode = MOTION_MODE_DUALHAND_SWORD;
				break;

			case WEAPON_BOW:
				dwMode = MOTION_MODE_BOW;
				break;

			case WEAPON_BELL:
				dwMode = MOTION_MODE_BELL;
				break;

			case WEAPON_FAN:
				dwMode = MOTION_MODE_FAN;
				break;

			case WEAPON_CLAW:
				dwMode = MOTION_MODE_CLAW;
				break;

		}
	}
	return dwMode;
}

float CHARACTER::GetMoveMotionSpeed() const
{
	DWORD dwMode = GetMotionMode();

	const CMotion* pkMotion = NULL;

	if (!GetMountVnum())
		pkMotion = CMotionManager::instance().GetMotion(GetRaceNum(), MAKE_MOTION_KEY(dwMode, (IsWalking() && IsPC()) ? MOTION_WALK : MOTION_RUN));
	else
	{
		pkMotion = CMotionManager::instance().GetMotion(GetMountVnum(), MAKE_MOTION_KEY(MOTION_MODE_GENERAL, (IsWalking() && IsPC()) ? MOTION_WALK : MOTION_RUN));

		if (!pkMotion)
			pkMotion = CMotionManager::instance().GetMotion(GetRaceNum(), MAKE_MOTION_KEY(MOTION_MODE_HORSE, (IsWalking() && IsPC()) ? MOTION_WALK : MOTION_RUN));
	}

	if (pkMotion)
		return -pkMotion->GetAccumVector().y / pkMotion->GetDuration();
	else
	{
		sys_err("cannot find motion (name %s race %d mode %d)", GetName(), GetRaceNum(), dwMode);
		return 300.0f;
	}
}

float CHARACTER::GetMoveSpeed() const
{
	return GetMoveMotionSpeed() * 10000 / CalculateDuration(GetLimitPoint(POINT_MOV_SPEED), 10000);
}

void CHARACTER::CalculateMoveDuration()
{
	m_posStart.x = GetX();
	m_posStart.y = GetY();

	float fDist = DISTANCE_SQRT(m_posStart.x - m_posDest.x, m_posStart.y - m_posDest.y);

	float motionSpeed = GetMoveMotionSpeed();

	m_dwMoveDuration = CalculateDuration(GetLimitPoint(POINT_MOV_SPEED),
		(int)((fDist / motionSpeed) * 1000.0f));

	if (IsNPC())
		sys_log(1, "%s: GOTO: distance %f, spd %u, duration %u, motion speed %f pos %d %d -> %d %d",
			GetName(), fDist, GetLimitPoint(POINT_MOV_SPEED), m_dwMoveDuration, motionSpeed,
			m_posStart.x, m_posStart.y, m_posDest.x, m_posDest.y);

	m_dwMoveStartTime = get_dword_time();
}

// x y 위치로 이동 한다. (이동할 수 있는 가 없는 가를 확인 하고 Sync 메소드로 실제 이동 한다)
// 서버는 char의 x, y 값을 바로 바꾸지만,
// 클라에서는 이전 위치에서 바꾼 x, y까지 interpolation한다.
// 걷거나 뛰는 것은 char의 m_bNowWalking에 달려있다.
// Warp를 의도한 것이라면 Show를 사용할 것.
bool CHARACTER::Move(long x, long y)
{
	if (IsPC() && IsDead())
		return false;

	// 같은 위치면 이동할 필요 없음 (자동 성공)
	if (GetX() == x && GetY() == y)
		return true;

	if (test_server)
		if (m_bDetailLog)
			sys_log(0, "%s position %u %u", GetName(), x, y);

	OnMove();
	return Sync(x, y);
}

void CHARACTER::SendMovePacket(BYTE bFunc, BYTE bArg, DWORD x, DWORD y, DWORD dwDuration, DWORD dwTime, int iRot)
{
	TPacketGCMove pack;

	if (bFunc == FUNC_WAIT)
	{
		x = m_posDest.x;
		y = m_posDest.y;
		dwDuration = m_dwMoveDuration;
	}

	EncodeMovePacket(pack, GetVID(), bFunc, bArg, x, y, dwDuration, dwTime, iRot == -1 ? (int)GetRotation() / 5 : iRot);
	PacketView(&pack, sizeof(TPacketGCMove), this);
}

POINT_VALUE CHARACTER::GetRealPoint(POINT_TYPE wPointType) const
{
	return m_points.lPoints[wPointType];
}

void CHARACTER::SetRealPoint(POINT_TYPE wPointType, POINT_VALUE lPointValue)
{
	m_points.lPoints[wPointType] = lPointValue;
}

POINT_VALUE CHARACTER::GetPolymorphPoint(POINT_TYPE wPointType) const
{
	if (IsPolymorphed() && !IsPolyMaintainStat())
	{
		DWORD dwMobVnum = GetPolymorphVnum();
		const CMob* c_pMob = CMobManager::instance().Get(dwMobVnum);
		int iPower = GetPolymorphPower();

		if (c_pMob)
		{
			switch (wPointType)
			{
				case POINT_ST:
					if (GetJob() == JOB_SHAMAN || GetJob() == JOB_SURA && GetSkillGroup() == 2)
						return c_pMob->m_table.bStr * iPower / 100 + GetPoint(POINT_IQ);
					return c_pMob->m_table.bStr * iPower / 100 + GetPoint(POINT_ST);

				case POINT_HT:
					return c_pMob->m_table.bCon * iPower / 100 + GetPoint(POINT_HT);

				case POINT_IQ:
					return c_pMob->m_table.bInt * iPower / 100 + GetPoint(POINT_IQ);

				case POINT_DX:
					return c_pMob->m_table.bDex * iPower / 100 + GetPoint(POINT_DX);
			}
		}
	}

	return GetPoint(wPointType);
}

POINT_VALUE CHARACTER::GetPoint(POINT_TYPE wPointType) const
{
	if (wPointType >= POINT_MAX_NUM)
	{
		sys_err("Point type overflow (type %u)", wPointType);
		return 0;
	}

	POINT_VALUE lPointValue = m_pointsInstant.points[wPointType];
	POINT_VALUE llMaxValue = POINT_MAX;

	switch (wPointType)
	{
		case POINT_STEAL_HP:
		case POINT_STEAL_SP:
		{
			llMaxValue = 50;
#if defined(__COSTUME_SYSTEM__) && defined(__MOVE_COSTUME_ATTR__)
			llMaxValue += 20;
#endif
#if defined(__GLOVE_SYSTEM__)
			llMaxValue += 10;
#endif
		}
		break;

		case POINT_MOV_SPEED:
		{
			if (IsAffectFlag(AFF_WAR_FLAG1) || IsAffectFlag(AFF_WAR_FLAG2) || IsAffectFlag(AFF_WAR_FLAG3))
				lPointValue = 50;
		}
		break;
	}

	if (lPointValue > llMaxValue)
		sys_err("POINT_ERROR: %s type %d val %d (max: %d)", GetName(), lPointValue, llMaxValue);

	return lPointValue;
}

POINT_VALUE CHARACTER::GetLimitPoint(POINT_TYPE wPointType) const
{
	if (wPointType >= POINT_MAX_NUM)
	{
		sys_err("Point type overflow (type %u)", wPointType);
		return 0;
	}

	POINT_VALUE lPointValue = m_pointsInstant.points[wPointType];

	POINT_VALUE llMinValue = -POINT_MAX;
	POINT_VALUE llMaxValue = POINT_MAX;
	POINT_VALUE llLimitValue = POINT_MAX;

	switch (wPointType)
	{
		case POINT_ATT_SPEED:
		{
			llMinValue = 0;

			if (IsPC())
				llLimitValue = 170;
			else
				llLimitValue = 250;
		}
		break;

		case POINT_MOV_SPEED:
		{
			llMinValue = 0;

			if (IsAffectFlag(AFF_WAR_FLAG1) || IsAffectFlag(AFF_WAR_FLAG2) || IsAffectFlag(AFF_WAR_FLAG3))
				llMinValue = 50;

			if (IsPC())
				llLimitValue = 200;
			else
				llLimitValue = 250;
		}
		break;

		case POINT_STEAL_HP:
		case POINT_STEAL_SP:
		{
			llLimitValue = 50;
			llMaxValue = 50;
		}
		break;

		case POINT_MALL_ATTBONUS:
		case POINT_MALL_DEFBONUS:
		{
			llLimitValue = 20;
			llMaxValue = 50;
		}
		break;
	}

#if defined(__CONQUEROR_LEVEL__)
	if (IsPC() && wPointType == POINT_MOV_SPEED && IsSungMaCursed(POINT_SUNGMA_MOVE))
		lPointValue /= 2;
#endif

	if (lPointValue > llMaxValue)
		sys_err("POINT_ERROR: %s type %d val %d (max: %d)", GetName(), lPointValue, llMaxValue);

	if (lPointValue > llLimitValue)
		lPointValue = llLimitValue;

	if (lPointValue < llMinValue)
		lPointValue = llMinValue;

	return lPointValue;
}

void CHARACTER::SetPoint(POINT_TYPE wPointType, POINT_VALUE lPointValue)
{
	if (wPointType >= POINT_MAX_NUM)
	{
		sys_err("Point type overflow (type %u)", wPointType);
		return;
	}

	m_pointsInstant.points[wPointType] = lPointValue;

	// 아직 이동이 다 안끝났다면 이동 시간 계산을 다시 해야 한다.
	if (wPointType == POINT_MOV_SPEED && get_dword_time() < m_dwMoveStartTime + m_dwMoveDuration)
	{
		CalculateMoveDuration();
	}
}

int CHARACTER::GetAllowedGold() const
{
	if (GetLevel() <= 10)
		return 100000;
	else if (GetLevel() <= 20)
		return 500000;
	else
		return 50000000;
}

void CHARACTER::CheckMaximumPoints()
{
	if (GetMaxHP() < GetHP())
		PointChange(POINT_HP, GetMaxHP() - GetHP());

	if (GetMaxSP() < GetSP())
		PointChange(POINT_SP, GetMaxSP() - GetSP());
}

void CHARACTER::PointChange(POINT_TYPE type, POINT_VALUE amount, bool bAmount, bool bBroadcast)
{
	POINT_VALUE val = 0;

	//sys_log(0, "PointChange %d %d | %d -> %d cHP %d mHP %d", type, amount, GetPoint(type), GetPoint(type)+amount, GetHP(), GetMaxHP());

	switch (type)
	{
		case POINT_NONE:
			return;

		case POINT_LEVEL:
		{
			if ((GetLevel() + amount) > gPlayerMaxLevel)
				return;

			SetLevel(GetLevel() + amount);
			val = GetLevel();

			sys_log(0, "LEVELUP: %s %d NEXT EXP %d", GetName(), GetLevel(), GetNextExp());

			// WOLFMAN 수인족 특수처리 (수인족은 직군이 하나이므로, 5레벨이 되면 무조건 1번 직군으로 설정함. 하드코딩 ㅈㅅ)
			if (GetJob() == JOB_WOLFMAN)
			{
				if ((5 <= val) && (GetSkillGroup() != 1))
				{
					ClearSkill();
					SetSkillGroup(1);
					SetRealPoint(POINT_SKILL, GetLevel() - 1);
					SetPoint(POINT_SKILL, GetRealPoint(POINT_SKILL));
					PointChange(POINT_SKILL, 0);
				}
			}

#if defined(__CONQUEROR_LEVEL__)
			if ((GetConquerorLevel() > 0) && (val < gPlayerMaxLevel))
				SetConqueror(false);
#endif

			PointChange(POINT_NEXT_EXP, GetNextExp(), false);

			if (amount)
			{
				quest::CQuestManager::instance().LevelUp(GetPlayerID());

				LogManager::instance().LevelLog(this, val, GetRealPoint(POINT_PLAYTIME) + (get_dword_time() - m_dwPlayStartTime) / 60000);

				if (GetGuild())
					GetGuild()->LevelChange(GetPlayerID(), GetLevel());

				if (GetParty())
					GetParty()->RequestSetMemberLevel(GetPlayerID(), GetLevel());
			}

			PointsPacket();
		} break;

		case POINT_NEXT_EXP:
		{
			val = GetNextExp();
			bAmount = false; // 무조건 bAmount는 false 여야 한다.
		} break;

		case POINT_EXP:
		{
			DWORD exp = GetExp();
			DWORD next_exp = GetNextExp();

			// exp가 0 이하로 가지 않도록 한다
			if (amount < 0 && exp <= -amount)
			{
				sys_log(0, "%s - Reduce EXP by %d, CUR EXP: %d (setting to zero)", GetName(), -amount, exp);
				amount = -(long)exp;

				SetExp(0);
				val = GetExp();
			}
			else
			{
#if defined(__CONQUEROR_LEVEL__)
				DWORD conqueror_exp = next_exp / 4;
				if (amount > 0 && (GetLevel() >= gPlayerMaxLevel && exp >= conqueror_exp))
					return;
#else
				if (gPlayerMaxLevel <= GetLevel())
					return;
#endif

#if defined(__CHATTING_WINDOW_RENEWAL__)
				ChatPacket(CHAT_TYPE_EXP_INFO, LC_STRING("%d의 경험치를 획득했습니다.", amount));
#else
				ChatPacket(CHAT_TYPE_INFO, LC_STRING("%d의 경험치를 획득했습니다.", amount));
#endif

				DWORD iExpBalance = 0;

				// 레벨 업!
				if (exp + amount >= next_exp)
				{
					iExpBalance = (exp + amount) - next_exp;
					amount = next_exp - exp;

					SetExp(0);
					exp = next_exp;
				}
#if defined(__CONQUEROR_LEVEL__)
				else if (GetLevel() >= gPlayerMaxLevel)
				{
					if (exp + amount >= conqueror_exp)
					{
						SetExp(conqueror_exp);
						exp = GetExp();
					}
					else
					{
						SetExp(exp + amount);
						exp = GetExp();
					}
				}
#endif
				else
				{
					SetExp(exp + amount);
					exp = GetExp();
				}

				DWORD q = DWORD(next_exp / 4.0f);
				int iLevStep = GetRealPoint(POINT_LEVEL_STEP);

				// iLevStep이 4 이상이면 레벨이 올랐어야 하므로 여기에 올 수 없는 값이다.
				if (iLevStep >= 4)
				{
					sys_err("%s LEVEL_STEP bigger than 4! (%d)", GetName(), iLevStep);
					iLevStep = 4;
				}

				if (exp >= next_exp && iLevStep < 4)
				{
					for (int i = 0; i < 4 - iLevStep; ++i)
						PointChange(POINT_LEVEL_STEP, 1, false, true);
				}
				else if (exp >= q * 3 && iLevStep < 3)
				{
					for (int i = 0; i < 3 - iLevStep; ++i)
						PointChange(POINT_LEVEL_STEP, 1, false, true);
				}
				else if (exp >= q * 2 && iLevStep < 2)
				{
					for (int i = 0; i < 2 - iLevStep; ++i)
						PointChange(POINT_LEVEL_STEP, 1, false, true);
				}
				else if (exp >= q && iLevStep < 1)
				{
					PointChange(POINT_LEVEL_STEP, 1);
				}

				if (iExpBalance)
				{
					PointChange(POINT_EXP, iExpBalance);
				}

				val = GetExp();
			}
		}
		break;

		case POINT_LEVEL_STEP:
		{
			if (amount > 0)
			{
				val = GetPoint(POINT_LEVEL_STEP) + amount;

				switch (val)
				{
					case 1:
					case 2:
					case 3:
						if ((GetLevel() <= gPlayerMaxLevelStats) && (GetLevel() <= gPlayerMaxLevel))
							PointChange(POINT_STAT, 1);
						break;

					case 4:
					{
						int iHP = number(JobInitialPoints[GetJob()].hp_per_lv_begin, JobInitialPoints[GetJob()].hp_per_lv_end);
						int iSP = number(JobInitialPoints[GetJob()].sp_per_lv_begin, JobInitialPoints[GetJob()].sp_per_lv_end);

						m_points.iRandomHP += iHP;
						m_points.iRandomSP += iSP;

						if (GetSkillGroup())
						{
							if (GetLevel() >= 5)
								PointChange(POINT_SKILL, 1);

							if (GetLevel() >= 9)
								PointChange(POINT_SUB_SKILL, 1);
						}

						PointChange(POINT_MAX_HP, iHP);
						PointChange(POINT_MAX_SP, iSP);
						PointChange(POINT_LEVEL, 1, false, true);

						val = 0;
					}
					break;
				}

				if (GetLevel() <= 10)
					AutoGiveItem(27001, 2);
				else if (GetLevel() <= 30)
					AutoGiveItem(27002, 2);
				else
				{
					AutoGiveItem(27002, 2);
					// AutoGiveItem(27003, 2);
				}

				PointChange(POINT_HP, GetMaxHP() - GetHP());
				PointChange(POINT_SP, GetMaxSP() - GetSP());
				PointChange(POINT_STAMINA, GetMaxStamina() - GetStamina());

				SetPoint(POINT_LEVEL_STEP, val);
				SetRealPoint(POINT_LEVEL_STEP, val);

				Save();
			}
			else
				val = GetPoint(POINT_LEVEL_STEP);
		} break;

		case POINT_HP:
		{
			if (IsDead() || IsStun())
				return;

			int prev_hp = GetHP();

			amount = MIN(GetMaxHP() - GetHP(), amount);
			SetHP(GetHP() + amount);
			val = GetHP();

			BroadcastTargetPacket();

			if (GetParty() && IsPC() && val != prev_hp)
				GetParty()->SendPartyInfoOneToAll(this);
		}
		break;

		case POINT_SP:
		{
			if (IsDead() || IsStun())
				return;

			amount = MIN(GetMaxSP() - GetSP(), amount);
			SetSP(GetSP() + amount);
			val = GetSP();
		}
		break;

		case POINT_STAMINA:
		{
			if (IsDead() || IsStun())
				return;

			int prev_val = GetStamina();
			amount = MIN(GetMaxStamina() - GetStamina(), amount);
			SetStamina(GetStamina() + amount);
			val = GetStamina();

			if (val == 0)
			{
				// Stamina가 없으니 걷자!
				SetNowWalking(true);
			}
			else if (prev_val == 0)
			{
				// 없던 스테미나가 생겼으니 이전 모드 복귀
				ResetWalking();
			}

			if (amount < 0 && val != 0) // 감소는 보내지않는다.
				return;
		}
		break;

		case POINT_MAX_HP:
		{
			SetPoint(type, GetPoint(type) + amount);

			//SetMaxHP(GetMaxHP() + amount);
			// 최대 생명력 = (기본 최대 생명력 + 추가) * 최대생명력%
			int curMaxHP = GetMaxHP();
			int hp = GetRealPoint(POINT_MAX_HP);
			int add_hp = MIN(3500, hp * GetPoint(POINT_MAX_HP_PCT) / 100);
			add_hp += GetPoint(POINT_MAX_HP);
			add_hp += GetPoint(POINT_PARTY_TANKER_BONUS);

#if defined(__CONQUEROR_LEVEL__)
			int max_hp = hp + add_hp;

			if (IsPC() && IsSungMaCursed(POINT_SUNGMA_HP))
				max_hp /= 2;

			SetMaxHP(max_hp);
#else
			SetMaxHP(hp + add_hp);
#endif

			float fRatio = (float)GetMaxHP() / (float)curMaxHP;
			PointChange(POINT_HP, GetHP()* fRatio - GetHP());

			val = GetMaxHP();
		}
		break;

		case POINT_MAX_SP:
		{
			SetPoint(type, GetPoint(type) + amount);

			//SetMaxSP(GetMaxSP() + amount);
			// 최대 정신력 = (기본 최대 정신력 + 추가) * 최대정신력%
			int curMaxSP = GetMaxSP();
			int sp = GetRealPoint(POINT_MAX_SP);
			int add_sp = MIN(800, sp * GetPoint(POINT_MAX_SP_PCT) / 100);
			add_sp += GetPoint(POINT_MAX_SP);
			add_sp += GetPoint(POINT_PARTY_SKILL_MASTER_BONUS);

			SetMaxSP(sp + add_sp);

			float fRatio = (float)GetMaxSP() / (float)curMaxSP;
			PointChange(POINT_SP, GetSP() * fRatio - GetSP());

			val = GetMaxSP();
		}
		break;

		case POINT_MAX_HP_PCT:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);

			PointChange(POINT_MAX_HP, 0);
			break;

		case POINT_MAX_SP_PCT:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);

			PointChange(POINT_MAX_SP, 0);
			break;

		case POINT_MAX_STAMINA:
			SetMaxStamina(GetMaxStamina() + amount);
			val = GetMaxStamina();
			break;

		case POINT_GOLD:
		{
			/*
			* The conversion to `int64_t` is necessary in this case because the maximum value
			* that can be represented by an int may not be sufficient to hold the sum of the
			* current gold value (GetGold()) and the additional amount of gold (`amount`).
			*/
			const int64_t nTotalGold = static_cast<int64_t>(GetGold()) + amount;
			if (GOLD_MAX <= nTotalGold)
			{
				sys_err("[OVERFLOW_GOLD] OriGold %d AddedGold %ld id %u Name %s ", GetGold(), amount, GetPlayerID(), GetName());
				LogManager::instance().CharLog(this, GetGold() + amount, "OVERFLOW_GOLD", "");
				return;
			}

			SetGold(GetGold() + amount);
			val = GetGold();
		}
		break;

		case POINT_SKILL:
		case POINT_STAT:
		case POINT_SUB_SKILL:
		case POINT_STAT_RESET_COUNT:
		case POINT_HORSE_SKILL:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);

			SetRealPoint(type, val);
			break;

		case POINT_DEF_GRADE:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);

			PointChange(POINT_CLIENT_DEF_GRADE, amount);
			break;

		case POINT_CLIENT_DEF_GRADE:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;

		case POINT_ST:
		case POINT_HT:
		case POINT_DX:
		case POINT_IQ:
		case POINT_HP_REGEN:
		case POINT_SP_REGEN:
		case POINT_ATT_SPEED:
		case POINT_ATT_GRADE:
		case POINT_MOV_SPEED:
		case POINT_CASTING_SPEED:
		case POINT_MAGIC_ATT_GRADE:
		case POINT_MAGIC_DEF_GRADE:
		case POINT_BOW_DISTANCE:
		case POINT_HP_RECOVERY:
		case POINT_SP_RECOVERY:

		case POINT_ATTBONUS_HUMAN: // 42 인간에게 강함
		case POINT_ATTBONUS_ANIMAL: // 43 동물에게 데미지 % 증가
		case POINT_ATTBONUS_ORC: // 44 웅귀에게 데미지 % 증가
		case POINT_ATTBONUS_MILGYO: // 45 밀교에게 데미지 % 증가
		case POINT_ATTBONUS_UNDEAD: // 46 시체에게 데미지 % 증가
		case POINT_ATTBONUS_DEVIL: // 47 마귀(악마)에게 데미지 % 증가
		case POINT_ATTBONUS_INSECT:
		case POINT_ATTBONUS_DESERT:

		case POINT_ATTBONUS_MONSTER:
		case POINT_ATTBONUS_SURA:
		case POINT_ATTBONUS_ASSASSIN:
		case POINT_ATTBONUS_WARRIOR:
		case POINT_ATTBONUS_SHAMAN:
		case POINT_ATTBONUS_WOLFMAN:

		case POINT_POISON_PCT:
		case POINT_BLEEDING_PCT:

		case POINT_STUN_PCT:
		case POINT_SLOW_PCT:

		case POINT_BLOCK:
		case POINT_DODGE:

		case POINT_CRITICAL_PCT:
		case POINT_RESIST_CRITICAL:
		case POINT_PENETRATE_PCT:
		case POINT_RESIST_PENETRATE:
		case POINT_CURSE_PCT:

		case POINT_STEAL_HP: // 48 생명력 흡수
		case POINT_STEAL_SP: // 49 정신력 흡수

		case POINT_MANA_BURN_PCT: // 50 마나 번
		case POINT_DAMAGE_SP_RECOVER: // 51 공격당할 시 정신력 회복 확률
		case POINT_RESIST_NORMAL_DAMAGE:
		case POINT_RESIST_SWORD:
		case POINT_RESIST_TWOHAND:
		case POINT_RESIST_DAGGER:
		case POINT_RESIST_BELL:
		case POINT_RESIST_FAN:
		case POINT_RESIST_BOW:
		case POINT_RESIST_CLAW:

		case POINT_RESIST_FIRE:
		case POINT_RESIST_ELEC:
		case POINT_RESIST_MAGIC:
#if defined(__MAGIC_REDUCTION__)
		case POINT_RESIST_MAGIC_REDUCTION:
#endif
		case POINT_RESIST_WIND:
		case POINT_RESIST_ICE:
		case POINT_RESIST_EARTH:
		case POINT_RESIST_DARK:
		case POINT_REFLECT_MELEE: // 67 공격 반사
		case POINT_REFLECT_CURSE: // 68 저주 반사
		case POINT_POISON_REDUCE: // 69 독데미지 감소
		case POINT_BLEEDING_REDUCE:

		case POINT_KILL_SP_RECOVER: // 70 적 소멸시 MP 회복
		case POINT_KILL_HP_RECOVERY: // 75
		case POINT_HIT_HP_RECOVERY:
		case POINT_HIT_SP_RECOVERY:
		case POINT_MANASHIELD:
		case POINT_ATT_BONUS:
		case POINT_DEF_BONUS:
		case POINT_SKILL_DAMAGE_BONUS:
		case POINT_NORMAL_HIT_DAMAGE_BONUS:
		case POINT_SKILL_DEFEND_BONUS:
		case POINT_NORMAL_HIT_DEFEND_BONUS:
			// DEPEND_BONUS_ATTRIBUTES
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;
			// END_OF_DEPEND_BONUS_ATTRIBUTES

		case POINT_PARTY_ATTACKER_BONUS:
		case POINT_PARTY_TANKER_BONUS:
		case POINT_PARTY_BUFFER_BONUS:
		case POINT_PARTY_SKILL_MASTER_BONUS:
		case POINT_PARTY_HASTE_BONUS:
		case POINT_PARTY_DEFENDER_BONUS:

		case POINT_RESIST_WARRIOR:
		case POINT_RESIST_ASSASSIN:
		case POINT_RESIST_SURA:
		case POINT_RESIST_SHAMAN:
		case POINT_RESIST_WOLFMAN:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;

		case POINT_MALL_ATTBONUS:
		case POINT_MALL_DEFBONUS:
		case POINT_MALL_EXPBONUS:
		case POINT_MALL_ITEMBONUS:
		case POINT_MALL_GOLDBONUS:
		case POINT_MELEE_MAGIC_ATT_BONUS_PER:
		{
			//if (GetPoint(type) + amount > 100)
			//{
			//	sys_err("MALL_BONUS exceeded over 100!! point type: %d name: %s amount %lld", type, GetName(), amount);
			//	amount = 100 - GetPoint(type);
			//}

			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
		}
		break;

		// PC_BANG_ITEM_ADD
		case POINT_PC_BANG_EXP_BONUS:
		case POINT_PC_BANG_DROP_BONUS:
		case POINT_RAMADAN_CANDY_BONUS_EXP:
			SetPoint(type, amount);
			val = GetPoint(type);
			break;
			// END_PC_BANG_ITEM_ADD

		case POINT_EXP_DOUBLE_BONUS:
		case POINT_GOLD_DOUBLE_BONUS:
		case POINT_ITEM_DROP_BONUS:
		case POINT_POTION_BONUS:
			if (GetPoint(type) + amount > 100)
			{
				sys_err("BONUS exceeded over 100!! point type: %d name: %s amount %lld", type, GetName(), amount);
				amount = 100 - GetPoint(type);
			}

			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;

		case POINT_IMMUNE_STUN:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			if (val)
				SET_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_STUN);
			else
				REMOVE_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_STUN);
			break;

		case POINT_IMMUNE_SLOW:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			if (val)
				SET_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_SLOW);
			else
				REMOVE_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_SLOW);
			break;

		case POINT_IMMUNE_FALL:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			if (val)
				SET_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_FALL);
			else
				REMOVE_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_FALL);
			break;

		case POINT_ATT_GRADE_BONUS:
			SetPoint(type, GetPoint(type) + amount);
			PointChange(POINT_ATT_GRADE, amount);
			val = GetPoint(type);
			break;

		case POINT_DEF_GRADE_BONUS:
			SetPoint(type, GetPoint(type) + amount);
			PointChange(POINT_DEF_GRADE, amount);
			val = GetPoint(type);
			break;

		case POINT_MAGIC_ATT_GRADE_BONUS:
			SetPoint(type, GetPoint(type) + amount);
			PointChange(POINT_MAGIC_ATT_GRADE, amount);
			val = GetPoint(type);
			break;

		case POINT_MAGIC_DEF_GRADE_BONUS:
			SetPoint(type, GetPoint(type) + amount);
			PointChange(POINT_MAGIC_DEF_GRADE, amount);
			val = GetPoint(type);
			break;

		case POINT_VOICE:
		case POINT_EMPIRE_POINT:
			//sys_err("CHARACTER::PointChange: %s: point cannot be changed. use SetPoint instead (type: %d)", GetName(), type);
			val = GetRealPoint(type);
			break;

		case POINT_POLYMORPH:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			SetPolymorph(val);
			break;

		case POINT_MOUNT:
		{
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
#if !defined(__MOUNT_COSTUME_SYSTEM__)
			MountVnum(val);
#endif
		}
		break;

		case POINT_ENERGY:
		case POINT_COSTUME_ATTR_BONUS:
		{
			int old_val = GetPoint(type);
			SetPoint(type, old_val + amount);
			val = GetPoint(type);
			BuffOnAttr_ValueChange(type, old_val, val);
		}
		break;

#if defined(__CHEQUE_SYSTEM__)
		case POINT_CHEQUE:
		{
			const POINT_VALUE nTotalCheque = static_cast<POINT_VALUE>(GetCheque()) + amount;
			if (CHEQUE_MAX < nTotalCheque)
			{
				sys_err("[OVERFLOW_CHEQUE] OriCheque %d AddedCheque %ld id %u Name %s ", GetCheque(), amount, GetPlayerID(), GetName());
				LogManager::instance().CharLog(this, GetCheque() + amount, "OVERFLOW_CHEQUE", "");
				return;
			}

			SetCheque(GetCheque() + amount);
			val = GetCheque();
		}
		break;
#endif

#if defined(__GEM_SYSTEM__)
		case POINT_GEM:
		{
			const POINT_VALUE nTotalGem = static_cast<POINT_VALUE>(GetGem()) + amount;
			if (GEM_MAX <= nTotalGem)
			{
				sys_err("[OVERFLOW_GEM] OriGem %d AddedGem %ld id %u Name %s ", GetGem(), amount, GetPlayerID(), GetName());
				LogManager::instance().CharLog(this, GetGem() + amount, "OVERFLOW_GEM", "");
				return;
			}

			SetGem(GetGem() + amount);
			val = GetGem();
		}
		break;
#endif

#if defined(__ELEMENT_SYSTEM__)
		case POINT_ENCHANT_ELECT:
		case POINT_ENCHANT_FIRE:
		case POINT_ENCHANT_ICE:
		case POINT_ENCHANT_WIND:
		case POINT_ENCHANT_EARTH:
		case POINT_ENCHANT_DARK:
		{
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			BroadcastTargetPacket();
		}
		break;
#endif

		case POINT_RESIST_HUMAN:
		case POINT_ATTBONUS_CZ:
		case POINT_ATTBONUS_SWORD:
		case POINT_ATTBONUS_TWOHAND:
		case POINT_ATTBONUS_DAGGER:
		case POINT_ATTBONUS_BELL:
		case POINT_ATTBONUS_FAN:
		case POINT_ATTBONUS_BOW:
		case POINT_ATTBONUS_CLAW:
		case POINT_RESIST_MOUNT_FALL:
		case POINT_RESIST_FIST:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;

		case POINT_PREMIUM_EXPBONUS:
		case POINT_PRIVILEGE_EXPBONUS:
		case POINT_MARRIAGE_EXPBONUS:
		case POINT_DEVILTOWER_EXPBONUS:
		case POINT_PREMIUM_ITEMBONUS:
		case POINT_PRIVILEGE_ITEMBONUS:
		case POINT_PREMIUM_GOLDBONUS:
		case POINT_PRIVILEGE_GOLDBONUS:
			// UNUSED
			break;

		case POINT_SKILL_DAMAGE_SAMYEON:
		case POINT_SKILL_DAMAGE_TANHWAN:
		case POINT_SKILL_DAMAGE_PALBANG:
		case POINT_SKILL_DAMAGE_GIGONGCHAM:
		case POINT_SKILL_DAMAGE_GYOKSAN:
		case POINT_SKILL_DAMAGE_GEOMPUNG:
		case POINT_SKILL_DAMAGE_AMSEOP:
		case POINT_SKILL_DAMAGE_GUNGSIN:
		case POINT_SKILL_DAMAGE_CHARYUN:
		case POINT_SKILL_DAMAGE_SANGONG:
		case POINT_SKILL_DAMAGE_YEONSA:
		case POINT_SKILL_DAMAGE_KWANKYEOK:
		case POINT_SKILL_DAMAGE_GIGUNG:
		case POINT_SKILL_DAMAGE_HWAJO:
		case POINT_SKILL_DAMAGE_SWAERYUNG:
		case POINT_SKILL_DAMAGE_YONGKWON:
		case POINT_SKILL_DAMAGE_PABEOB:
		case POINT_SKILL_DAMAGE_MARYUNG:
		case POINT_SKILL_DAMAGE_HWAYEOMPOK:
		case POINT_SKILL_DAMAGE_MAHWAN:
		case POINT_SKILL_DAMAGE_BIPABU:
		case POINT_SKILL_DAMAGE_YONGBI:
		case POINT_SKILL_DAMAGE_PAERYONG:
		case POINT_SKILL_DAMAGE_NOEJEON:
		case POINT_SKILL_DAMAGE_BYEURAK:
		case POINT_SKILL_DAMAGE_CHAIN:
		case POINT_SKILL_DAMAGE_CHAYEOL:
		case POINT_SKILL_DAMAGE_SALPOONG:
		case POINT_SKILL_DAMAGE_GONGDAB:
		case POINT_SKILL_DAMAGE_PASWAE:
			// UNUSED
			break;

		case POINT_NORMAL_HIT_DEFEND_BONUS_BOSS_OR_MORE:
		case POINT_SKILL_DEFEND_BONUS_BOSS_OR_MORE:
		case POINT_NORMAL_HIT_DAMAGE_BONUS_BOSS_OR_MORE:
		case POINT_SKILL_DAMAGE_BONUS_BOSS_OR_MORE:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;

		case POINT_HIT_BUFF_ENCHANT_FIRE:
		case POINT_HIT_BUFF_ENCHANT_ICE:
		case POINT_HIT_BUFF_ENCHANT_ELEC:
		case POINT_HIT_BUFF_ENCHANT_WIND:
		case POINT_HIT_BUFF_ENCHANT_DARK:
		case POINT_HIT_BUFF_ENCHANT_EARTH:
		case POINT_HIT_BUFF_RESIST_FIRE:
		case POINT_HIT_BUFF_RESIST_ICE:
		case POINT_HIT_BUFF_RESIST_ELEC:
		case POINT_HIT_BUFF_RESIST_WIND:
		case POINT_HIT_BUFF_RESIST_DARK:
		case POINT_HIT_BUFF_RESIST_EARTH:
		case POINT_HIT_BUFF_SUNGMA_STR:
		case POINT_HIT_BUFF_SUNGMA_MOVE:
		case POINT_HIT_BUFF_SUNGMA_HP:
		case POINT_HIT_BUFF_SUNGMA_IMMUNE:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;

		case POINT_USE_SKILL_CHEONGRANG_MOV_SPEED:
		case POINT_USE_SKILL_CHEONGRANG_CASTING_SPEED:
		case POINT_USE_SKILL_CHAYEOL_CRITICAL_PCT:
		case POINT_USE_SKILL_SANGONG_ATT_GRADE_BONUS:
		case POINT_USE_SKILL_GIGUNG_ATT_GRADE_BONUS:
		case POINT_USE_SKILL_JEOKRANG_DEF_BONUS:
		case POINT_USE_SKILL_GWIGEOM_DEF_BONUS:
		case POINT_USE_SKILL_TERROR_ATT_GRADE_BONUS:
		case POINT_USE_SKILL_MUYEONG_ATT_GRADE_BONUS:
		case POINT_USE_SKILL_MANASHILED_CASTING_SPEED:
		case POINT_USE_SKILL_HOSIN_DEF_BONUS:
		case POINT_USE_SKILL_GICHEON_ATT_GRADE_BONUS:
		case POINT_USE_SKILL_JEONGEOP_ATT_GRADE_BONUS:
		case POINT_USE_SKILL_JEUNGRYEOK_DEF_BONUS:
		case POINT_USE_SKILL_GIHYEOL_ATT_GRADE_BONUS:
		case POINT_USE_SKILL_CHUNKEON_CASTING_SPEED:
		case POINT_USE_SKILL_NOEGEOM_ATT_GRADE_BONUS:
			// UNUSED
			break;

		case POINT_SKILL_DURATION_INCREASE_EUNHYUNG:
		case POINT_SKILL_DURATION_INCREASE_GYEONGGONG:
		case POINT_SKILL_DURATION_INCREASE_GEOMKYUNG:
		case POINT_SKILL_DURATION_INCREASE_JEOKRANG:
			// UNUSED
			break;

		case POINT_USE_SKILL_PALBANG_HP_ABSORB:
		case POINT_USE_SKILL_AMSEOP_HP_ABSORB:
		case POINT_USE_SKILL_YEONSA_HP_ABSORB:
		case POINT_USE_SKILL_YONGBI_HP_ABSORB:
		case POINT_USE_SKILL_CHAIN_HP_ABSORB:
		case POINT_USE_SKILL_PASWAE_SP_ABSORB:
		case POINT_USE_SKILL_GIGONGCHAM_STUN:
		case POINT_USE_SKILL_CHARYUN_STUN:
		case POINT_USE_SKILL_PABEOB_STUN:
		case POINT_USE_SKILL_MAHWAN_STUN:
		case POINT_USE_SKILL_GONGDAB_STUN:
		case POINT_USE_SKILL_SAMYEON_STUN:
		case POINT_USE_SKILL_GYOKSAN_KNOCKBACK:
		case POINT_USE_SKILL_SEOMJEON_KNOCKBACK:
		case POINT_USE_SKILL_SWAERYUNG_KNOCKBACK:
		case POINT_USE_SKILL_HWAYEOMPOK_KNOCKBACK:
		case POINT_USE_SKILL_GONGDAB_KNOCKBACK:
		case POINT_USE_SKILL_KWANKYEOK_KNOCKBACK:
		case POINT_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_10PER:
		case POINT_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_10PER:
		case POINT_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_10PER:
		case POINT_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_10PER:
		case POINT_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_10PER:
		case POINT_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_10PER:
		case POINT_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_10PER:
		case POINT_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_10PER:
		case POINT_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_10PER:
		case POINT_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_10PER:
			// UNUSED
			break;

		case POINT_ATTBONUS_STONE:
		case POINT_DAMAGE_HP_RECOVERY:
		case POINT_DAMAGE_SP_RECOVERY:
		case POINT_ALIGNMENT_DAMAGE_BONUS:
		case POINT_NORMAL_DAMAGE_GUARD:
		case POINT_MORE_THEN_HP90_DAMAGE_REDUCE:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;

		case POINT_USE_SKILL_TUSOK_HP_ABSORB:
		case POINT_USE_SKILL_PAERYONG_HP_ABSORB:
		case POINT_USE_SKILL_BYEURAK_HP_ABSORB:
		case POINT_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_20PER:
		case POINT_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_20PER:
		case POINT_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_20PER:
		case POINT_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_20PER:
		case POINT_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_20PER:
		case POINT_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_20PER:
		case POINT_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_20PER:
		case POINT_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_20PER:
		case POINT_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_20PER:
		case POINT_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_20PER:
		case POINT_USE_SKILL_CHAYEOL_HP_ABSORB:
			// UNUSED
			break;

		case POINT_MEDAL_OF_HONOR:
		case POINT_ALL_STAT_BONUS:
			// UNUSED
			break;

#if defined(__CONQUEROR_LEVEL__)
		case POINT_SUNGMA_STR:
		{
			amount *= 1 + (GetPoint(POINT_SUNGMA_PER_STR) / 100);
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
		}
		break;

		case POINT_SUNGMA_HP:
		{
			amount *= 1 + (GetPoint(POINT_SUNGMA_PER_HP) / 100);
			SetPoint(type, GetPoint(type) + amount);

			PointChange(POINT_MAX_HP, 0);
			val = GetPoint(type);
		}
		break;

		case POINT_SUNGMA_MOVE:
		{
			amount *= 1 + (GetPoint(POINT_SUNGMA_PER_MOVE) / 100);
			SetPoint(type, GetPoint(type) + amount);

			// NOTE : Refresh move speed points. (Client)
			UpdatePointsPacket(POINT_MOV_SPEED, GetLimitPoint(POINT_MOV_SPEED));
			val = GetPoint(type);
		}
		break;

		case POINT_SUNGMA_IMMUNE:
		{
			amount *= 1 + (GetPoint(POINT_SUNGMA_PER_IMMUNE) / 100);
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
		}
		break;

		case POINT_SUNGMA_PER_STR:
		case POINT_SUNGMA_PER_HP:
		case POINT_SUNGMA_PER_MOVE:
		case POINT_SUNGMA_PER_IMMUNE:
		{
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
		}
		break;

		case POINT_CONQUEROR_LEVEL:
		{
			if ((GetConquerorLevel() + amount) > gPlayerMaxConquerorLevel)
				return;

			SetConquerorLevel(GetConquerorLevel() + amount);
			val = GetConquerorLevel();

			sys_log(0, "CONQUEROR_LEVELUP: %s %d NEXT EXP %d", GetName(), GetConquerorLevel(), GetNextConquerorExp());

			PointChange(POINT_CONQUEROR_NEXT_EXP, GetNextConquerorExp(), false);
		}
		break;

		case POINT_CONQUEROR_LEVEL_STEP:
		{
			if (amount > 0)
			{
				val = GetPoint(POINT_CONQUEROR_LEVEL_STEP) + amount;
				switch (val)
				{
					case 1:
					case 2:
					case 3:
					{
						if ((GetConquerorLevel() <= gPlayerMaxLevelStats) && (GetConquerorLevel() <= gPlayerMaxConquerorLevel))
							PointChange(POINT_CONQUEROR_POINT, 1);
					} break;
					case 4:
					{
						PointChange(POINT_CONQUEROR_POINT, 1);
						PointChange(POINT_CONQUEROR_LEVEL, 1, false, true);
						val = 0;
					} break;
				}

				SetPoint(POINT_CONQUEROR_LEVEL_STEP, val);
				SetRealPoint(POINT_CONQUEROR_LEVEL_STEP, val);

				Save();
			}
			else
				val = GetPoint(POINT_CONQUEROR_LEVEL_STEP);
		}
		break;

		case POINT_CONQUEROR_EXP:
		{
			DWORD exp = GetConquerorExp();
			DWORD next_exp = GetNextConquerorExp();

			if (amount < 0 && exp <= -amount)
			{
				sys_log(0, "%s - Reduce EXP by %d, CUR EXP: %d (setting to zero)", GetName(), -amount, exp);
				amount = -exp;

				SetConquerorExp(0);
				val = GetExp();
			}
			else
			{
				if (GetConquerorLevel() >= gPlayerMaxConquerorLevel)
					return;

#if defined(__CHATTING_WINDOW_RENEWAL__)
				ChatPacket(CHAT_TYPE_EXP_INFO, LC_STRING("You have received %lu EXP for your Champion Level.", amount));
#endif

				DWORD iExpBalance = 0;

				// 레벨 업!
				if (exp + amount >= next_exp)
				{
					iExpBalance = (exp + amount) - next_exp;
					amount = next_exp - exp;

					SetConquerorExp(0);
					exp = next_exp;
				}
				else
				{
					SetConquerorExp(exp + amount);
					exp = GetConquerorExp();
				}

				DWORD q = DWORD(next_exp / 4.0f);
				int iLevStep = GetRealPoint(POINT_CONQUEROR_LEVEL_STEP);

				if (iLevStep >= 4)
				{
					sys_err("%s CONQUEROR_LEVEL_STEP bigger than 4! (%d)", GetName(), iLevStep);
					iLevStep = 4;
				}

				if (exp >= next_exp && iLevStep < 4)
				{
					for (int i = 0; i < 4 - iLevStep; ++i)
						PointChange(POINT_CONQUEROR_LEVEL_STEP, 1, false, true);
				}
				else if (exp >= q * 3 && iLevStep < 3)
				{
					for (int i = 0; i < 3 - iLevStep; ++i)
						PointChange(POINT_CONQUEROR_LEVEL_STEP, 1, false, true);
				}
				else if (exp >= q * 2 && iLevStep < 2)
				{
					for (int i = 0; i < 2 - iLevStep; ++i)
						PointChange(POINT_CONQUEROR_LEVEL_STEP, 1, false, true);
				}
				else if (exp >= q && iLevStep < 1)
					PointChange(POINT_CONQUEROR_LEVEL_STEP, 1);

				if (iExpBalance)
					PointChange(POINT_CONQUEROR_EXP, iExpBalance);

				val = GetConquerorExp();
			}
		}
		break;

		case POINT_CONQUEROR_NEXT_EXP:
		{
			val = GetNextConquerorExp();
			bAmount = false;
		}
		break;

		case POINT_CONQUEROR_POINT:
		{
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			SetRealPoint(type, val);
		}
		break;
#endif

		case POINT_HIT_PCT:
		{
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;
		}

		case POINT_ATTBONUS_PER_HUMAN:
		case POINT_ATTBONUS_PER_ANIMAL:
		case POINT_ATTBONUS_PER_ORC:
		case POINT_ATTBONUS_PER_MILGYO:
		case POINT_ATTBONUS_PER_UNDEAD:
		case POINT_ATTBONUS_PER_DEVIL:
		case POINT_ENCHANT_PER_ELECT:
		case POINT_ENCHANT_PER_FIRE:
		case POINT_ENCHANT_PER_ICE:
		case POINT_ENCHANT_PER_WIND:
		case POINT_ENCHANT_PER_EARTH:
		case POINT_ENCHANT_PER_DARK:
		case POINT_ATTBONUS_PER_CZ:
		case POINT_ATTBONUS_PER_INSECT:
		case POINT_ATTBONUS_PER_DESERT:
		case POINT_ATTBONUS_PER_STONE:
		case POINT_ATTBONUS_PER_MONSTER:
		case POINT_RESIST_PER_HUMAN:
		case POINT_RESIST_PER_ICE:
		case POINT_RESIST_PER_DARK:
		case POINT_RESIST_PER_EARTH:
		case POINT_RESIST_PER_FIRE:
		case POINT_RESIST_PER_ELEC:
		case POINT_RESIST_PER_MAGIC:
		case POINT_RESIST_PER_WIND:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;

			//case POINT_MOUNT_MELEE_MAGIC_ATTBONUS_PER:
			//case POINT_DISMOUNT_MOVE_SPEED_BONUS_PER:
			//case POINT_HIT_AUTO_HP_RECOVERY:
			//case POINT_HIT_AUTO_SP_RECOVERY:
			//case POINT_USE_SKILL_COOLTIME_DECREASE_ALL:
			//case POINT_HIT_STONE_ATTBONUS_STONE:
			//case POINT_HIT_STONE_DEF_GRADE_BONUS:
			//case POINT_KILL_BOSS_ITEM_BONUS:
			//case POINT_MOB_HIT_MOB_AGGRESSIVE:
			//case POINT_NO_DEATH_AND_HP_RECOVERY30:
			//case POINT_AUTO_PICKUP:
		case POINT_MOUNT_NO_KNOCKBACK:

			//case POINT_IMMUNE_POISON100:
			//case POINT_IMMUNE_BLEEDING100:
		case POINT_MONSTER_DEFEND_BONUS:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;

		default:
			sys_err("CHARACTER::PointChange: %s: unknown point change type %d", GetName(), type);
			return;
	}

	switch (type)
	{
		case POINT_LEVEL:
		case POINT_ST:
		case POINT_DX:
		case POINT_IQ:
		case POINT_HT:
#if defined(__CONQUEROR_LEVEL__)
		case POINT_CONQUEROR_LEVEL:
		case POINT_SUNGMA_STR:
		case POINT_SUNGMA_HP:
		case POINT_SUNGMA_MOVE:
		case POINT_SUNGMA_IMMUNE:
#endif
			ComputeBattlePoints();
			break;

		case POINT_MAX_HP:
		case POINT_MAX_SP:
		case POINT_MAX_STAMINA:
			break;
	}

	if (type == POINT_HP && amount == 0)
		return;

	UpdatePointsPacket(type, val, amount, bAmount, bBroadcast);
}

void CHARACTER::ApplyPoint(POINT_TYPE wApplyType, POINT_VALUE lApplyValue)
{
	switch (wApplyType)
	{
		case APPLY_NONE:						// 0
			break;

			// NOTE: 아이템에 의한 최대HP 보너스나 퀘스트 보상 보너스가 똑같은 방식을 사용하므로
			// 그냥 MAX_HP만 계산하면 퀘스트 보상의 경우 문제가 생김. 사실 원래 이쪽이 합리적이기도 하고..
			// 바꾼 공식은 현재 최대 hp와 보유 hp의 비율을 구한 뒤 바뀔 최대 hp를 기준으로 hp를 보정한다.
			// 원래 PointChange에서 하는게 좋을것 같은데 설계 문제로 어려워서 skip..
			// SP도 똑같이 계산한다.
			// Mantis : 101460 ~ ity ~
		case APPLY_MAX_HP:						// 1
		{
			if (GetMaxHP() == 0) break;
			PointChange(aApplyInfo[wApplyType].wPointType, lApplyValue);
		}
		break;

		case APPLY_MAX_SP:						// 2
		{
			if (GetMaxSP() == 0) break;
			PointChange(aApplyInfo[wApplyType].wPointType, lApplyValue);
		}
		break;

		case APPLY_CON:							// 3
		{
			PointChange(POINT_HT, lApplyValue);
			PointChange(POINT_MAX_HP, (lApplyValue * JobInitialPoints[GetJob()].hp_per_ht));
			PointChange(POINT_MAX_STAMINA, (lApplyValue * JobInitialPoints[GetJob()].stamina_per_con));
		}
		break;

		case APPLY_INT:							// 4
		{
			PointChange(POINT_IQ, lApplyValue);
			PointChange(POINT_MAX_SP, (lApplyValue * JobInitialPoints[GetJob()].sp_per_iq));
		}
		break;

		case APPLY_STR:							// 5
		case APPLY_DEX:							// 6
		case APPLY_ATT_SPEED:					// 7
		case APPLY_MOV_SPEED:					// 8
		case APPLY_CAST_SPEED:					// 9
		case APPLY_HP_REGEN:					// 10
		case APPLY_SP_REGEN:					// 11
		case APPLY_POISON_PCT:					// 12
		case APPLY_STUN_PCT:					// 13
		case APPLY_SLOW_PCT:					// 14
		case APPLY_CRITICAL_PCT:				// 15
		case APPLY_PENETRATE_PCT:				// 16
		case APPLY_ATTBONUS_HUMAN:				// 17
		case APPLY_ATTBONUS_ANIMAL:				// 18
		case APPLY_ATTBONUS_ORC:				// 19
		case APPLY_ATTBONUS_MILGYO:				// 20
		case APPLY_ATTBONUS_UNDEAD:				// 21
		case APPLY_ATTBONUS_DEVIL:				// 22
		case APPLY_STEAL_HP:					// 23
		case APPLY_STEAL_SP:					// 24
		case APPLY_MANA_BURN_PCT:				// 25
		case APPLY_DAMAGE_SP_RECOVER:			// 26
		case APPLY_BLOCK:						// 27
		case APPLY_DODGE:						// 28
		case APPLY_RESIST_SWORD:				// 29
		case APPLY_RESIST_TWOHAND:				// 30
		case APPLY_RESIST_DAGGER:				// 31
		case APPLY_RESIST_BELL:					// 32
		case APPLY_RESIST_FAN:					// 33
		case APPLY_RESIST_BOW:					// 34
		case APPLY_RESIST_FIRE:					// 35
		case APPLY_RESIST_ELEC:					// 36
		case APPLY_RESIST_MAGIC:				// 37
		case APPLY_RESIST_WIND:					// 38
		case APPLY_REFLECT_MELEE:				// 39
		case APPLY_REFLECT_CURSE:				// 40
		case APPLY_POISON_REDUCE:				// 41
		case APPLY_KILL_SP_RECOVER:				// 42
		case APPLY_EXP_DOUBLE_BONUS:			// 43
		case APPLY_GOLD_DOUBLE_BONUS:			// 44
		case APPLY_ITEM_DROP_BONUS:				// 45
		case APPLY_POTION_BONUS:				// 46
		case APPLY_KILL_HP_RECOVER:				// 47
		case APPLY_IMMUNE_STUN:					// 48
		case APPLY_IMMUNE_SLOW:					// 49
		case APPLY_IMMUNE_FALL:					// 50
			PointChange(aApplyInfo[wApplyType].wPointType, lApplyValue);
			break;

		case APPLY_SKILL:						// 51
			// SKILL_DAMAGE_BONUS
		{
			// 최상위 비트 기준으로 8비트 vnum, 9비트 add, 15비트 change
			// 00000000 00000000 00000000 00000000
			// ^^^^^^^^  ^^^^^^^^^^^^^^^^^^^^^^^^^
			// vnum		^ add		change
			BYTE bSkillVnum = (BYTE)(((DWORD)lApplyValue) >> 24);
			int iAdd = lApplyValue & 0x00800000;
			int iChange = lApplyValue & 0x007fffff;

			sys_log(1, "APPLY_SKILL skill %d add? %d change %d", bSkillVnum, iAdd ? 1 : 0, iChange);

			if (0 == iAdd)
				iChange = -iChange;

			auto iter = m_SkillDamageBonus.find(bSkillVnum);
			if (iter == m_SkillDamageBonus.end())
				m_SkillDamageBonus.insert(std::make_pair(bSkillVnum, iChange));
			else
				iter->second += iChange;
		}
		// END_OF_SKILL_DAMAGE_BONUS
		break;

		case APPLY_BOW_DISTANCE:				// 52
		case APPLY_ATT_GRADE_BONUS:				// 53
		case APPLY_DEF_GRADE_BONUS:				// 54
		case APPLY_MAGIC_ATT_GRADE:				// 55
		case APPLY_MAGIC_DEF_GRADE:				// 56
		case APPLY_CURSE_PCT:					// 57
		case APPLY_MAX_STAMINA:					// 58

		case APPLY_ATTBONUS_WARRIOR:			// 59
		case APPLY_ATTBONUS_ASSASSIN:			// 60
		case APPLY_ATTBONUS_SURA:				// 61
		case APPLY_ATTBONUS_SHAMAN:				// 62
		case APPLY_ATTBONUS_MONSTER:			// 63

		case APPLY_MALL_ATTBONUS:				// 64
		case APPLY_MALL_DEFBONUS:				// 65
		case APPLY_MALL_EXPBONUS:				// 66 
		case APPLY_MALL_ITEMBONUS:				// 67
		case APPLY_MALL_GOLDBONUS:				// 68
			PointChange(aApplyInfo[wApplyType].wPointType, lApplyValue);
			break;

		case APPLY_MAX_HP_PCT:					// 69
		{
			if (GetMaxHP() == 0) break;
			PointChange(aApplyInfo[wApplyType].wPointType, lApplyValue);
		}
		break;
		case APPLY_MAX_SP_PCT:					// 70
		{
			if (GetMaxSP() == 0) break;
			PointChange(aApplyInfo[wApplyType].wPointType, lApplyValue);
		}
		break;

		case APPLY_SKILL_DAMAGE_BONUS:			// 71
		case APPLY_NORMAL_HIT_DAMAGE_BONUS:		// 72

			// DEPEND_BONUS_ATTRIBUTES
		case APPLY_SKILL_DEFEND_BONUS:			// 73
		case APPLY_NORMAL_HIT_DEFEND_BONUS:		// 74
			// END_OF_DEPEND_BONUS_ATTRIBUTES

		case APPLY_PC_BANG_EXP_BONUS:			// 75
		case APPLY_PC_BANG_DROP_BONUS:			// 76

			//case APPLY_EXTRACT_HP_PCT:			// 77 사용시 HP 소모

		case APPLY_RESIST_WARRIOR:				// 78
		case APPLY_RESIST_ASSASSIN:				// 79
		case APPLY_RESIST_SURA:					// 80
		case APPLY_RESIST_SHAMAN:				// 81

		case APPLY_ENERGY:						// 82 기력
		case APPLY_DEF_GRADE:					// 83 방어력. DEF_GRADE_BONUS는 클라에서 두배로 보여지는 의도된 버그(...)가 있다.
		case APPLY_COSTUME_ATTR_BONUS:			// 84 코스튬 아이템에 붙은 속성치 보너스
		case APPLY_MAGIC_ATTBONUS_PER:			// 85 마법 공격력 +x%
		case APPLY_MELEE_MAGIC_ATTBONUS_PER:	// 86 마법 + 밀리 공격력 +x%

		case APPLY_RESIST_ICE:					// 87 냉기 저항
		case APPLY_RESIST_EARTH:				// 88 대지 저항
		case APPLY_RESIST_DARK:					// 89 어둠 저항

		case APPLY_ANTI_CRITICAL_PCT:			// 90 크리티컬 저항
		case APPLY_ANTI_PENETRATE_PCT:			// 91 관통타격 저항

		case APPLY_BLEEDING_REDUCE:				// 92
		case APPLY_BLEEDING_PCT:				// 93
		case APPLY_ATTBONUS_WOLFMAN:			// 94
		case APPLY_RESIST_WOLFMAN:				// 95
		case APPLY_RESIST_CLAW:					// 96

		case APPLY_ACCEDRAIN_RATE:
		case APPLY_RESIST_MAGIC_REDUCTION:

		case APPLY_ENCHANT_ELECT:
		case APPLY_ENCHANT_FIRE:
		case APPLY_ENCHANT_ICE:
		case APPLY_ENCHANT_WIND:
		case APPLY_ENCHANT_EARTH:
		case APPLY_ENCHANT_DARK:

		case APPLY_ATTBONUS_CZ:
		case APPLY_ATTBONUS_INSECT:
		case APPLY_ATTBONUS_DESERT:
		case APPLY_ATTBONUS_SWORD:
		case APPLY_ATTBONUS_TWOHAND:
		case APPLY_ATTBONUS_DAGGER:
		case APPLY_ATTBONUS_BELL:
		case APPLY_ATTBONUS_FAN:
		case APPLY_ATTBONUS_BOW:
		case APPLY_ATTBONUS_CLAW:

		case APPLY_RESIST_HUMAN:
		case APPLY_RESIST_MOUNT_FALL:
		case APPLY_RESIST_FIST:

		case APPLY_MOUNT:

		case APPLY_SKILL_DAMAGE_SAMYEON:
		case APPLY_SKILL_DAMAGE_TANHWAN:
		case APPLY_SKILL_DAMAGE_PALBANG:
		case APPLY_SKILL_DAMAGE_GIGONGCHAM:
		case APPLY_SKILL_DAMAGE_GYOKSAN:
		case APPLY_SKILL_DAMAGE_GEOMPUNG:
		case APPLY_SKILL_DAMAGE_AMSEOP:
		case APPLY_SKILL_DAMAGE_GUNGSIN:
		case APPLY_SKILL_DAMAGE_CHARYUN:
		case APPLY_SKILL_DAMAGE_SANGONG:
		case APPLY_SKILL_DAMAGE_YEONSA:
		case APPLY_SKILL_DAMAGE_KWANKYEOK:
		case APPLY_SKILL_DAMAGE_GIGUNG:
		case APPLY_SKILL_DAMAGE_HWAJO:
		case APPLY_SKILL_DAMAGE_SWAERYUNG:
		case APPLY_SKILL_DAMAGE_YONGKWON:
		case APPLY_SKILL_DAMAGE_PABEOB:
		case APPLY_SKILL_DAMAGE_MARYUNG:
		case APPLY_SKILL_DAMAGE_HWAYEOMPOK:
		case APPLY_SKILL_DAMAGE_MAHWAN:
		case APPLY_SKILL_DAMAGE_BIPABU:
		case APPLY_SKILL_DAMAGE_YONGBI:
		case APPLY_SKILL_DAMAGE_PAERYONG:
		case APPLY_SKILL_DAMAGE_NOEJEON:
		case APPLY_SKILL_DAMAGE_BYEURAK:
		case APPLY_SKILL_DAMAGE_CHAIN:
		case APPLY_SKILL_DAMAGE_CHAYEOL:
		case APPLY_SKILL_DAMAGE_SALPOONG:
		case APPLY_SKILL_DAMAGE_GONGDAB:
		case APPLY_SKILL_DAMAGE_PASWAE:

		case APPLY_NORMAL_HIT_DEFEND_BONUS_BOSS_OR_MORE:
		case APPLY_SKILL_DEFEND_BONUS_BOSS_OR_MORE:
		case APPLY_NORMAL_HIT_DAMAGE_BONUS_BOSS_OR_MORE:
		case APPLY_SKILL_DAMAGE_BONUS_BOSS_OR_MORE:

		case APPLY_HIT_BUFF_ENCHANT_FIRE:
		case APPLY_HIT_BUFF_ENCHANT_ICE:
		case APPLY_HIT_BUFF_ENCHANT_ELEC:
		case APPLY_HIT_BUFF_ENCHANT_WIND:
		case APPLY_HIT_BUFF_ENCHANT_DARK:
		case APPLY_HIT_BUFF_ENCHANT_EARTH:
		case APPLY_HIT_BUFF_RESIST_FIRE:
		case APPLY_HIT_BUFF_RESIST_ICE:
		case APPLY_HIT_BUFF_RESIST_ELEC:
		case APPLY_HIT_BUFF_RESIST_WIND:
		case APPLY_HIT_BUFF_RESIST_DARK:
		case APPLY_HIT_BUFF_RESIST_EARTH:

		case APPLY_USE_SKILL_CHEONGRANG_MOV_SPEED:
		case APPLY_USE_SKILL_CHEONGRANG_CASTING_SPEED:
		case APPLY_USE_SKILL_CHAYEOL_CRITICAL_PCT:
		case APPLY_USE_SKILL_SANGONG_ATT_GRADE_BONUS:
		case APPLY_USE_SKILL_GIGUNG_ATT_GRADE_BONUS:
		case APPLY_USE_SKILL_JEOKRANG_DEF_BONUS:
		case APPLY_USE_SKILL_GWIGEOM_DEF_BONUS:
		case APPLY_USE_SKILL_TERROR_ATT_GRADE_BONUS:
		case APPLY_USE_SKILL_MUYEONG_ATT_GRADE_BONUS:
		case APPLY_USE_SKILL_MANASHILED_CASTING_SPEED:
		case APPLY_USE_SKILL_HOSIN_DEF_BONUS:
		case APPLY_USE_SKILL_GICHEON_ATT_GRADE_BONUS:
		case APPLY_USE_SKILL_JEONGEOP_ATT_GRADE_BONUS:
		case APPLY_USE_SKILL_JEUNGRYEOK_DEF_BONUS:
		case APPLY_USE_SKILL_GIHYEOL_ATT_GRADE_BONUS:
		case APPLY_USE_SKILL_CHUNKEON_CASTING_SPEED:
		case APPLY_USE_SKILL_NOEGEOM_ATT_GRADE_BONUS:

		case APPLY_SKILL_DURATION_INCREASE_EUNHYUNG:
		case APPLY_SKILL_DURATION_INCREASE_GYEONGGONG:
		case APPLY_SKILL_DURATION_INCREASE_GEOMKYUNG:
		case APPLY_SKILL_DURATION_INCREASE_JEOKRANG:

		case APPLY_USE_SKILL_PALBANG_HP_ABSORB:
		case APPLY_USE_SKILL_AMSEOP_HP_ABSORB:
		case APPLY_USE_SKILL_YEONSA_HP_ABSORB:
		case APPLY_USE_SKILL_YONGBI_HP_ABSORB:
		case APPLY_USE_SKILL_CHAIN_HP_ABSORB:
		case APPLY_USE_SKILL_PASWAE_SP_ABSORB:
		case APPLY_USE_SKILL_GIGONGCHAM_STUN:
		case APPLY_USE_SKILL_CHARYUN_STUN:
		case APPLY_USE_SKILL_PABEOB_STUN:
		case APPLY_USE_SKILL_MAHWAN_STUN:
		case APPLY_USE_SKILL_GONGDAB_STUN:
		case APPLY_USE_SKILL_SAMYEON_STUN:
		case APPLY_USE_SKILL_GYOKSAN_KNOCKBACK:
		case APPLY_USE_SKILL_SEOMJEON_KNOCKBACK:
		case APPLY_USE_SKILL_SWAERYUNG_KNOCKBACK:
		case APPLY_USE_SKILL_HWAYEOMPOK_KNOCKBACK:
		case APPLY_USE_SKILL_GONGDAB_KNOCKBACK:
		case APPLY_USE_SKILL_KWANKYEOK_KNOCKBACK:
		case APPLY_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_10PER:
		case APPLY_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_10PER:
		case APPLY_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_10PER:
		case APPLY_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_10PER:
		case APPLY_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_10PER:
		case APPLY_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_10PER:
		case APPLY_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_10PER:
		case APPLY_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_10PER:
		case APPLY_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_10PER:
		case APPLY_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_10PER:

		case APPLY_ATTBONUS_STONE:

		case APPLY_DAMAGE_HP_RECOVERY:
		case APPLY_DAMAGE_SP_RECOVERY:

		case APPLY_ALIGNMENT_DAMAGE_BONUS:

		case APPLY_NORMAL_DAMAGE_GUARD:
		case APPLY_MORE_THEN_HP90_DAMAGE_REDUCE:

		case APPLY_USE_SKILL_TUSOK_HP_ABSORB:
		case APPLY_USE_SKILL_PAERYONG_HP_ABSORB:
		case APPLY_USE_SKILL_BYEURAK_HP_ABSORB:

		case APPLY_FIRST_ATTRIBUTE_BONUS:
		case APPLY_SECOND_ATTRIBUTE_BONUS:
		case APPLY_THIRD_ATTRIBUTE_BONUS:
		case APPLY_FOURTH_ATTRIBUTE_BONUS:
		case APPLY_FIFTH_ATTRIBUTE_BONUS:

		case APPLY_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_20PER:
		case APPLY_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_20PER:
		case APPLY_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_20PER:
		case APPLY_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_20PER:
		case APPLY_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_20PER:
		case APPLY_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_20PER:
		case APPLY_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_20PER:
		case APPLY_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_20PER:
		case APPLY_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_20PER:
		case APPLY_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_20PER:
		case APPLY_USE_SKILL_CHAYEOL_HP_ABSORB:

		case APPLY_SUNGMA_STR:
		case APPLY_SUNGMA_HP:
		case APPLY_SUNGMA_MOVE:
		case APPLY_SUNGMA_IMMUNE:

		case APPLY_HIT_PCT:
			PointChange(aApplyInfo[wApplyType].wPointType, lApplyValue);
			break;

#if defined(__ITEM_APPLY_RANDOM__)
		case APPLY_RANDOM:
			break;
#endif

		case APPLY_ATTBONUS_PER_HUMAN:
		case APPLY_ATTBONUS_PER_ANIMAL:
		case APPLY_ATTBONUS_PER_ORC:
		case APPLY_ATTBONUS_PER_MILGYO:
		case APPLY_ATTBONUS_PER_UNDEAD:
		case APPLY_ATTBONUS_PER_DEVIL:

		case APPLY_ENCHANT_PER_ELECT:
		case APPLY_ENCHANT_PER_FIRE:
		case APPLY_ENCHANT_PER_ICE:
		case APPLY_ENCHANT_PER_WIND:
		case APPLY_ENCHANT_PER_EARTH:
		case APPLY_ENCHANT_PER_DARK:

		case APPLY_ATTBONUS_PER_CZ:
		case APPLY_ATTBONUS_PER_INSECT:
		case APPLY_ATTBONUS_PER_DESERT:
		case APPLY_ATTBONUS_PER_STONE:
		case APPLY_ATTBONUS_PER_MONSTER:

		case APPLY_RESIST_PER_HUMAN:
		case APPLY_RESIST_PER_ICE:
		case APPLY_RESIST_PER_DARK:
		case APPLY_RESIST_PER_EARTH:
		case APPLY_RESIST_PER_FIRE:
		case APPLY_RESIST_PER_ELEC:
		case APPLY_RESIST_PER_MAGIC:
		case APPLY_RESIST_PER_WIND:

		case APPLY_HIT_BUFF_SUNGMA_STR:
		case APPLY_HIT_BUFF_SUNGMA_MOVE:
		case APPLY_HIT_BUFF_SUNGMA_HP:
		case APPLY_HIT_BUFF_SUNGMA_IMMUNE:

		case APPLY_MOUNT_MELEE_MAGIC_ATTBONUS_PER:
		case APPLY_DISMOUNT_MOVE_SPEED_BONUS_PER:

		case APPLY_HIT_AUTO_HP_RECOVERY:
		case APPLY_HIT_AUTO_SP_RECOVERY:

		case APPLY_USE_SKILL_COOLTIME_DECREASE_ALL:

		case APPLY_HIT_STONE_ATTBONUS_STONE:
		case APPLY_HIT_STONE_DEF_GRADE_BONUS:

		case APPLY_KILL_BOSS_ITEM_BONUS:
		case APPLY_MOB_HIT_MOB_AGGRESSIVE:
		case APPLY_NO_DEATH_AND_HP_RECOVERY30:

		case APPLY_AUTO_PICKUP:
		case APPLY_MOUNT_NO_KNOCKBACK:

		case APPLY_SUNGMA_PER_STR:
		case APPLY_SUNGMA_PER_HP:
		case APPLY_SUNGMA_PER_MOVE:
		case APPLY_SUNGMA_PER_IMMUNE:

		case APPLY_IMMUNE_POISON100:
		case APPLY_IMMUNE_BLEEDING100:

		case APPLY_MONSTER_DEFEND_BONUS:
			PointChange(aApplyInfo[wApplyType].wPointType, lApplyValue);
			break;

		default:
			sys_err("Unknown apply type %u name %s", wApplyType, GetName());
			break;
	}
}

void CHARACTER::MotionPacketEncode(BYTE motion, LPCHARACTER victim, struct packet_motion* packet)
{
	packet->header = HEADER_GC_MOTION;
	packet->vid = m_vid;
	packet->motion = motion;

	if (victim)
		packet->victim_vid = victim->GetVID();
	else
		packet->victim_vid = 0;
}

void CHARACTER::Motion(BYTE motion, LPCHARACTER victim)
{
	struct packet_motion pack_motion;
	MotionPacketEncode(motion, victim, &pack_motion);
	PacketAround(&pack_motion, sizeof(struct packet_motion));
}

EVENTFUNC(save_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>(event->info);
	if (info == NULL)
	{
		sys_err("save_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER ch = info->ch;

	if (ch == NULL) // <Factor>
		return 0;

	sys_log(1, "SAVE_EVENT: %s", ch->GetName());
	ch->Save();
	ch->FlushDelayedSaveItem();
	return (save_event_second_cycle);
}

void CHARACTER::StartSaveEvent()
{
	if (m_pkSaveEvent)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;
	m_pkSaveEvent = event_create(save_event, info, save_event_second_cycle);
}

void CHARACTER::MonsterLog(const char* format, ...)
{
	if (!test_server)
		return;

	if (IsPC())
		return;

	char chatbuf[CHAT_MAX_LEN + 1];
	int len = snprintf(chatbuf, sizeof(chatbuf), "%u)", (DWORD)GetVID());

	if (len < 0 || len >= (int)sizeof(chatbuf))
		len = sizeof(chatbuf) - 1;

	va_list args;

	va_start(args, format);

	int len2 = vsnprintf(chatbuf + len, sizeof(chatbuf) - len, format, args);

	if (len2 < 0 || len2 >= (int)sizeof(chatbuf) - len)
		len += (sizeof(chatbuf) - len) - 1;
	else
		len += len2;

	// \0 문자 포함
	++len;

	va_end(args);

	TPacketGCChat pack_chat;

	pack_chat.header = HEADER_GC_CHAT;
	pack_chat.size = sizeof(TPacketGCChat) + len;
	pack_chat.type = CHAT_TYPE_TALKING;
	pack_chat.id = (DWORD)GetVID();
	pack_chat.bEmpire = 0;

	TEMP_BUFFER buf;
	buf.write(&pack_chat, sizeof(TPacketGCChat));
	buf.write(chatbuf, len);

	CHARACTER_MANAGER::instance().PacketMonsterLog(this, buf.read_peek(), buf.size());
}

void CHARACTER::ChatPacket(BYTE type, const char* format, ...)
{
	LPDESC d = GetDesc();

	if (!d || !format)
		return;

	char chatbuf[CHAT_MAX_LEN + 1];
	va_list args;

	va_start(args, format);
	int len = vsnprintf(chatbuf, sizeof(chatbuf), format, args);
	va_end(args);

	struct packet_chat pack_chat;

	pack_chat.header = HEADER_GC_CHAT;
	pack_chat.size = sizeof(struct packet_chat) + len;
	pack_chat.type = type;
	pack_chat.id = 0;
	pack_chat.bEmpire = d->GetEmpire();

	TEMP_BUFFER buf;
	buf.write(&pack_chat, sizeof(struct packet_chat));
	buf.write(chatbuf, len);

	d->Packet(buf.read_peek(), buf.size());

	if (type == CHAT_TYPE_COMMAND && test_server)
		sys_log(0, "SEND_COMMAND %s %s", GetName(), chatbuf);
}

#if defined(__MULTI_LANGUAGE_SYSTEM__)
void CHARACTER::ChatPacket(packet_chat pack_chat, const char* format, ...)
{
	LPDESC d = GetDesc();

	if (!d || !format)
		return;

	char chatbuf[CHAT_MAX_LEN + 1];
	va_list args;

	va_start(args, format);
	int len = vsnprintf(chatbuf, sizeof(chatbuf), format, args);
	va_end(args);

	pack_chat.header = HEADER_GC_CHAT;
	pack_chat.size = sizeof(struct packet_chat) + len;
	pack_chat.id = 0;
	pack_chat.bEmpire = d->GetEmpire();

	TEMP_BUFFER buf;
	buf.write(&pack_chat, sizeof(struct packet_chat));
	buf.write(chatbuf, len);

	d->Packet(buf.read_peek(), buf.size());
}
#endif

// MINING
void CHARACTER::mining_take()
{
	m_pkMiningEvent = NULL;
}

void CHARACTER::mining_cancel()
{
	if (m_pkMiningEvent)
	{
		sys_log(0, "XXX MINING CANCEL");
		event_cancel(&m_pkMiningEvent);
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("채광을 중단하였습니다."));
	}
}

void CHARACTER::mining(LPCHARACTER chLoad)
{
	if (m_pkMiningEvent)
	{
		mining_cancel();
		return;
	}

	if (!chLoad)
		return;

	if (IsRiding())
		return;

	if (GetMapIndex() != chLoad->GetMapIndex() || DISTANCE_APPROX(GetX() - chLoad->GetX(), GetY() - chLoad->GetY()) > 1000)
		return;

	if (mining::GetRawOreFromLoad(chLoad->GetRaceNum()) == 0)
		return;

	LPITEM pick = GetWear(WEAR_WEAPON);

	if (!pick || pick->GetType() != ITEM_PICK)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("곡괭이를 장착하세요."));
		return;
	}

	int count = number(5, 15); // 동작 횟수, 한 동작당 2초

	// 채광 동작을 보여줌
	TPacketGCDigMotion p;
	p.header = HEADER_GC_DIG_MOTION;
	p.vid = GetVID();
	p.target_vid = chLoad->GetVID();
	p.count = count;

	PacketAround(&p, sizeof(p));

	m_pkMiningEvent = mining::CreateMiningEvent(this, chLoad, count);
}
// END_OF_MINING

void CHARACTER::fishing()
{
	if (PreventTradeWindow(WND_ALL))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("You cannot fish while other windows are open."));
		return;
	}

	if (m_pkFishingEvent)
	{
		fishing_take();
		return;
	}

	// 못감 속성에서 낚시를 시도한다?
	{
		LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(GetMapIndex());

		int x = GetX();
		int y = GetY();

		LPSECTREE tree = pkSectreeMap->Find(x, y);
		DWORD dwAttr = tree->GetAttribute(x, y);

		if (IS_SET(dwAttr, ATTR_BLOCK))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("낚시를 할 수 있는 곳이 아닙니다"));
			return;
		}
	}

	LPITEM rod = GetWear(WEAR_WEAPON);

	// 낚시대 장착
	if (!rod || rod->GetType() != ITEM_ROD)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("낚시대를 장착 하세요."));
		return;
	}

	if (0 == rod->GetSocket(2))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("미끼를 끼고 던져 주세요."));
		return;
	}

	if (GetEmptyInventory(1) == -1)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("Make sure you have enough space in your inventory before you go fishing."));
		return;
	}

	float fx, fy;
	GetDeltaByDegree(GetRotation(), 400.0f, &fx, &fy);

#if defined(__FISHING_GAME__)
	SetFishingGameGoals(0);
#endif

	m_pkFishingEvent = fishing::CreateFishingEvent(this);
}

void CHARACTER::fishing_take()
{
	LPITEM rod = GetWear(WEAR_WEAPON);
	if (rod && rod->GetType() == ITEM_ROD)
	{
		using fishing::fishing_event_info;
		if (m_pkFishingEvent)
		{
			struct fishing_event_info* info = dynamic_cast<struct fishing_event_info*>(m_pkFishingEvent->info);

			if (info)
				fishing::Take(info, this);
		}
	}
	else
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("낚시대가 아닌 물건으로 낚시를 할 수 없습니다!"));
	}

	event_cancel(&m_pkFishingEvent);
}

bool CHARACTER::StartStateMachine(int iNextPulse)
{
	if (CHARACTER_MANAGER::instance().AddToStateList(this))
	{
		m_dwNextStatePulse = thecore_heart->pulse + iNextPulse;
		return true;
	}

	return false;
}

void CHARACTER::StopStateMachine()
{
	CHARACTER_MANAGER::instance().RemoveFromStateList(this);
}

void CHARACTER::UpdateStateMachine(DWORD dwPulse)
{
	if (dwPulse < m_dwNextStatePulse)
		return;

	if (IsDead())
		return;

	Update();
	m_dwNextStatePulse = dwPulse + m_dwStateDuration;
}

void CHARACTER::SetNextStatePulse(int iNextPulse)
{
	CHARACTER_MANAGER::instance().AddToStateList(this);
	m_dwNextStatePulse = iNextPulse;

	if (iNextPulse < 10)
		MonsterLog("다음상태로어서가자");
}

// 캐릭터 인스턴스 업데이트 함수.
void CHARACTER::UpdateCharacter(DWORD dwPulse)
{
	CFSM::Update();
}

void CHARACTER::SetShop(LPSHOP pkShop)
{
	if ((m_pkShop = pkShop))
		SET_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_SHOP);
	else
	{
		REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_SHOP);
		SetShopOwner(NULL);
	}
}

void CHARACTER::SetExchange(CExchange* pkExchange)
{
	m_pkExchange = pkExchange;
}

void CHARACTER::SetPart(BYTE bPartPos, DWORD dwVal)
{
	assert(bPartPos < PART_MAX_NUM);
	m_pointsInstant.adwParts[bPartPos] = dwVal;
}

DWORD CHARACTER::GetPart(BYTE bPartPos) const
{
	assert(bPartPos < PART_MAX_NUM);
#if defined(__HIDE_COSTUME_SYSTEM__)
	if (bPartPos == PART_MAIN && GetHiddenCostumeByPart(PART_MAIN))
	{
		if (const LPITEM pArmor = GetWear(WEAR_BODY))
#if defined(__CHANGE_LOOK_SYSTEM__)
			return pArmor->GetTransmutationVnum() != 0 ? pArmor->GetTransmutationVnum() : pArmor->GetVnum();
#else
			return pArmor->GetVnum();
#endif
		else
			return 0;
	}
	else if (bPartPos == PART_WEAPON && GetHiddenCostumeByPart(PART_WEAPON))
	{
		if (const LPITEM pWeapon = GetWear(WEAR_WEAPON))
#if defined(__CHANGE_LOOK_SYSTEM__)
			return pWeapon->GetTransmutationVnum() != 0 ? pWeapon->GetTransmutationVnum() : pWeapon->GetVnum();
#else
			return pWeapon->GetVnum();
#endif
		else
			return 0;
	}
	else
	{
		if (GetHiddenCostumeByPart(bPartPos))
			return 0;
	}
#endif
	return m_pointsInstant.adwParts[bPartPos];
}

DWORD CHARACTER::GetOriginalPart(BYTE bPartPos) const
{
	switch (bPartPos)
	{
		case PART_MAIN:
			if (!IsPC()) // PC가 아닌 경우 현재 파트를 그대로 리턴
				return GetPart(PART_MAIN);
			else
				return m_pointsInstant.bBasePart;

		case PART_HAIR:
			return GetPart(PART_HAIR);

#if defined(__ACCE_COSTUME_SYSTEM__)
		case PART_ACCE:
			return GetPart(PART_ACCE);
#endif

#if defined(__WEAPON_COSTUME_SYSTEM__)
		case PART_WEAPON:
			return GetWear(WEAR_COSTUME_WEAPON) ? GetPart(PART_WEAPON) : 0;
#endif

#if defined(__AURA_COSTUME_SYSTEM__)
		case PART_AURA:
			return GetPart(PART_AURA);
#endif

		default:
			return 0;
	}
}

BYTE CHARACTER::GetCharType() const
{
	return m_bCharType;
}

bool CHARACTER::SetSyncOwner(LPCHARACTER ch, bool bRemoveFromList)
{
	if (IsNoMove())
		return false;

	if (ch)
	{
		if (!battle_is_attackable(ch, this))
		{
			SendDamagePacket(ch, 0, DAMAGE_BLOCK);
			return false;
		}
	}

	if (ch == this)
	{
		sys_err("SetSyncOwner owner == this (%p)", this);
		return false;
	}

	if (!ch)
	{
		if (bRemoveFromList && m_pkChrSyncOwner)
		{
			m_pkChrSyncOwner->m_kLst_pkChrSyncOwned.remove(this);
		}

		if (m_pkChrSyncOwner)
			sys_log(1, "SyncRelease %s %p from %s", GetName(), this, m_pkChrSyncOwner->GetName());

		// 리스트에서 제거하지 않더라도 포인터는 NULL로 셋팅되어야 한다.
		m_pkChrSyncOwner = NULL;
	}
	else
	{
		if (!IsSyncOwner(ch))
			return false;

		// 거리가 200 이상이면 SyncOwner가 될 수 없다.
		if (DISTANCE_APPROX(GetX() - ch->GetX(), GetY() - ch->GetY()) > 250)
		{
			sys_log(1, "SetSyncOwner distance over than 250 %s %s", GetName(), ch->GetName());

			// SyncOwner일 경우 Owner로 표시한다.
			if (m_pkChrSyncOwner == ch)
				return true;

			return false;
		}

		if (m_pkChrSyncOwner != ch)
		{
			if (m_pkChrSyncOwner)
			{
				sys_log(1, "SyncRelease %s %p from %s", GetName(), this, m_pkChrSyncOwner->GetName());
				m_pkChrSyncOwner->m_kLst_pkChrSyncOwned.remove(this);
			}

			m_pkChrSyncOwner = ch;
			m_pkChrSyncOwner->m_kLst_pkChrSyncOwned.push_back(this);

			// SyncOwner가 바뀌면 LastSyncTime을 초기화한다.
			static const timeval zero_tv = { 0, 0 };
			SetLastSyncTime(zero_tv);

			sys_log(1, "SetSyncOwner set %s %p to %s", GetName(), this, ch->GetName());
		}

		m_fSyncTime = get_float_time();
	}

	// TODO: Sync Owner가 같더라도 계속 패킷을 보내고 있으므로,
	// 동기화 된 시간이 3초 이상 지났을 때 풀어주는 패킷을
	// 보내는 방식으로 하면 패킷을 줄일 수 있다.
	TPacketGCOwnership pack;

	pack.bHeader = HEADER_GC_OWNERSHIP;
	pack.dwOwnerVID = ch ? ch->GetVID() : 0;
	pack.dwVictimVID = GetVID();

	PacketAround(&pack, sizeof(TPacketGCOwnership));
	return true;
}

struct FuncClearSync
{
	void operator () (LPCHARACTER ch)
	{
		assert(ch != NULL);
		ch->SetSyncOwner(NULL, false); // false 플래그로 해야 for_each 가 제대로 돈다.
	}
};

void CHARACTER::ClearSync()
{
	SetSyncOwner(NULL);

	// 아래 for_each에서 나를 m_pkChrSyncOwner로 가진 자들의 포인터를 NULL로 한다.
	std::for_each(m_kLst_pkChrSyncOwned.begin(), m_kLst_pkChrSyncOwned.end(), FuncClearSync());
	m_kLst_pkChrSyncOwned.clear();
}

bool CHARACTER::IsSyncOwner(LPCHARACTER ch) const
{
	if (m_pkChrSyncOwner == ch)
		return true;

	// 마지막으로 동기화 된 시간이 3초 이상 지났다면 소유권이 아무에게도
	// 없다. 따라서 아무나 SyncOwner이므로 true 리턴
	if (get_float_time() - m_fSyncTime >= 3.0f)
		return true;

	return false;
}

void CHARACTER::SetParty(LPPARTY pkParty)
{
	if (pkParty == m_pkParty)
		return;

	if (pkParty && m_pkParty)
		sys_err("%s is trying to reassigning party (current %p, new party %p)", GetName(), get_pointer(m_pkParty), get_pointer(pkParty));

	sys_log(1, "PARTY set to %p", get_pointer(pkParty));

	if (m_pkDungeon && IsPC() && !pkParty)
		SetDungeon(NULL);

	m_pkParty = pkParty;

	if (IsPC())
	{
		if (m_pkParty)
			SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_PARTY);
		else
			REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_PARTY);

		UpdatePacket();
	}
}

// PARTY_JOIN_BUG_FIX
/// 파티 가입 event 정보
EVENTINFO(TPartyJoinEventInfo)
{
	DWORD dwGuestPID; ///< 파티에 참여할 캐릭터의 PID
	DWORD dwLeaderPID; ///< 파티 리더의 PID

	TPartyJoinEventInfo()
		: dwGuestPID(0)
		, dwLeaderPID(0)
	{
	}
};

EVENTFUNC(party_request_event)
{
	TPartyJoinEventInfo* info = dynamic_cast<TPartyJoinEventInfo*>(event->info);

	if (info == NULL)
	{
		sys_err("party_request_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(info->dwGuestPID);

	if (ch)
	{
		sys_log(0, "PartyRequestEvent %s", ch->GetName());
		ch->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
		ch->SetPartyRequestEvent(NULL);
	}

	return 0;
}

bool CHARACTER::RequestToParty(LPCHARACTER leader)
{
	if (leader->GetParty())
		leader = leader->GetParty()->GetLeaderCharacter();

	if (!leader)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("파티장이 접속 상태가 아니라서 요청을 할 수 없습니다."));
		return false;
	}

	if (m_pkPartyRequestEvent)
		return false;

	if (!IsPC() || !leader->IsPC())
		return false;

	if (leader->IsBlockMode(BLOCK_PARTY_REQUEST))
		return false;

	PartyJoinErrCode errcode = IsPartyJoinableCondition(leader, this);

	switch (errcode)
	{
		case PERR_NONE:
			break;

		case PERR_SERVER:
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 서버 문제로 파티 관련 처리를 할 수 없습니다."));
			return false;

		case PERR_DIFFEMPIRE:
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 다른 제국과 파티를 이룰 수 없습니다."));
			return false;

		case PERR_DUNGEON:
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 던전 안에서는 파티 초대를 할 수 없습니다."));
			return false;

		case PERR_OBSERVER:
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 관전 모드에선 파티 초대를 할 수 없습니다."));
			return false;

		case PERR_LVBOUNDARY:
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> -30 ~ +30 레벨 이내의 상대방만 초대할 수 있습니다."));
			return false;

		case PERR_LOWLEVEL:
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 파티내 최고 레벨 보다 30레벨이 낮아 초대할 수 없습니다."));
			return false;

		case PERR_HILEVEL:
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 파티내 최저 레벨 보다 30레벨이 높아 초대할 수 없습니다."));
			return false;

		case PERR_ALREADYJOIN:
			return false;

		case PERR_PARTYISFULL:
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 더 이상 파티원을 초대할 수 없습니다."));
			return false;

		default:
			sys_err("Do not process party join error(%d)", errcode);
			return false;
	}

	TPartyJoinEventInfo* info = AllocEventInfo<TPartyJoinEventInfo>();

	info->dwGuestPID = GetPlayerID();
	info->dwLeaderPID = leader->GetPlayerID();

	SetPartyRequestEvent(event_create(party_request_event, info, PASSES_PER_SEC(10)));

#if defined(__NEW_USER_CARE__)
	leader->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequest %u %s", (DWORD)GetVID(), GetName());
#else
	leader->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequest %u", (DWORD)GetVID());
#endif
	ChatPacket(CHAT_TYPE_INFO, LC_STRING("%s 님에게 파티가입 신청을 했습니다.", leader->GetName()));
	return true;
}

void CHARACTER::DenyToParty(LPCHARACTER member)
{
	sys_log(1, "DenyToParty %s member %s %p", GetName(), member->GetName(), get_pointer(member->m_pkPartyRequestEvent));

	if (!member->m_pkPartyRequestEvent)
		return;

	TPartyJoinEventInfo* info = dynamic_cast<TPartyJoinEventInfo*>(member->m_pkPartyRequestEvent->info);

	if (!info)
	{
		sys_err("CHARACTER::DenyToParty> <Factor> Null pointer");
		return;
	}

	if (info->dwGuestPID != member->GetPlayerID())
		return;

	if (info->dwLeaderPID != GetPlayerID())
		return;

	event_cancel(&member->m_pkPartyRequestEvent);

	member->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
}

void CHARACTER::AcceptToParty(LPCHARACTER member)
{
	sys_log(1, "AcceptToParty %s member %s %p", GetName(), member->GetName(), get_pointer(member->m_pkPartyRequestEvent));

	if (!member->m_pkPartyRequestEvent)
		return;

	TPartyJoinEventInfo* info = dynamic_cast<TPartyJoinEventInfo*>(member->m_pkPartyRequestEvent->info);

	if (!info)
	{
		sys_err("CHARACTER::AcceptToParty> <Factor> Null pointer");
		return;
	}

	if (info->dwGuestPID != member->GetPlayerID())
		return;

	if (info->dwLeaderPID != GetPlayerID())
		return;

	event_cancel(&member->m_pkPartyRequestEvent);

	if (!GetParty())
		member->ChatPacket(CHAT_TYPE_INFO, LC_STRING("상대방이 파티에 속해있지 않습니다."));
	else
	{
		if (GetPlayerID() != GetParty()->GetLeaderPID())
			return;

		PartyJoinErrCode errcode = IsPartyJoinableCondition(this, member);
		switch (errcode)
		{
			case PERR_NONE: member->PartyJoin(this); return;
			case PERR_SERVER: member->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 서버 문제로 파티 관련 처리를 할 수 없습니다.")); break;
			case PERR_DUNGEON: member->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 던전 안에서는 파티 초대를 할 수 없습니다.")); break;
			case PERR_OBSERVER: member->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 관전 모드에선 파티 초대를 할 수 없습니다.")); break;
			case PERR_LVBOUNDARY: member->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> -30 ~ +30 레벨 이내의 상대방만 초대할 수 있습니다.")); break;
			case PERR_LOWLEVEL: member->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 파티내 최고 레벨 보다 30레벨이 낮아 초대할 수 없습니다.")); break;
			case PERR_HILEVEL: member->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 파티내 최저 레벨 보다 30레벨이 높아 초대할 수 없습니다.")); break;
			case PERR_ALREADYJOIN: break;
			case PERR_PARTYISFULL: {
				ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 더 이상 파티원을 초대할 수 없습니다."));
				member->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 파티의 인원제한이 초과하여 파티에 참가할 수 없습니다."));
				break;
			}
			default: sys_err("Do not process party join error(%d)", errcode);
		}
	}

	member->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
}

/**
* 파티 초대 event callback 함수.
* event 가 발동하면 초대 거절로 처리한다.
**/
EVENTFUNC(party_invite_event)
{
	TPartyJoinEventInfo* pInfo = dynamic_cast<TPartyJoinEventInfo*>(event->info);

	if (pInfo == NULL)
	{
		sys_err("party_invite_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER pchInviter = CHARACTER_MANAGER::instance().FindByPID(pInfo->dwLeaderPID);

	if (pchInviter)
	{
		sys_log(1, "PartyInviteEvent %s", pchInviter->GetName());
		pchInviter->PartyInviteDeny(pInfo->dwGuestPID);
	}

	return 0;
}

void CHARACTER::PartyInvite(LPCHARACTER pchInvitee)
{
	if (GetParty() && GetParty()->GetLeaderPID() != GetPlayerID())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 파티원을 초대할 수 있는 권한이 없습니다."));
		return;
	}
	else if (pchInvitee->IsBlockMode(BLOCK_PARTY_INVITE))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> %s 님이 파티 거부 상태입니다.", pchInvitee->GetName()));
		return;
	}

	PartyJoinErrCode errcode = IsPartyJoinableCondition(this, pchInvitee);

	switch (errcode)
	{
		case PERR_NONE:
			break;

		case PERR_SERVER:
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 서버 문제로 파티 관련 처리를 할 수 없습니다."));
			return;

		case PERR_DIFFEMPIRE:
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 다른 제국과 파티를 이룰 수 없습니다."));
			return;

		case PERR_DUNGEON:
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 던전 안에서는 파티 초대를 할 수 없습니다."));
			return;

		case PERR_OBSERVER:
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 관전 모드에선 파티 초대를 할 수 없습니다."));
			return;

		case PERR_LVBOUNDARY:
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> -30 ~ +30 레벨 이내의 상대방만 초대할 수 있습니다."));
			return;

		case PERR_LOWLEVEL:
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 파티내 최고 레벨 보다 30레벨이 낮아 초대할 수 없습니다."));
			return;

		case PERR_HILEVEL:
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 파티내 최저 레벨 보다 30레벨이 높아 초대할 수 없습니다."));
			return;

		case PERR_ALREADYJOIN:
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 이미 %s님은 파티에 속해 있습니다.", pchInvitee->GetName()));
			return;

		case PERR_PARTYISFULL:
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 더 이상 파티원을 초대할 수 없습니다."));
			return;

		default:
			sys_err("Do not process party join error(%d)", errcode);
			return;
	}

	if (m_PartyInviteEventMap.end() != m_PartyInviteEventMap.find(pchInvitee->GetPlayerID()))
		return;

	//
	// EventMap 에 이벤트 추가
	//
	TPartyJoinEventInfo* info = AllocEventInfo<TPartyJoinEventInfo>();

	info->dwGuestPID = pchInvitee->GetPlayerID();
	info->dwLeaderPID = GetPlayerID();

	m_PartyInviteEventMap.insert(EventMap::value_type(pchInvitee->GetPlayerID(), event_create(party_invite_event, info, PASSES_PER_SEC(10))));

	//
	// 초대 받는 character 에게 초대 패킷 전송
	//

	TPacketGCPartyInvite p;
	p.header = HEADER_GC_PARTY_INVITE;
	p.leader_vid = GetVID();
	pchInvitee->GetDesc()->Packet(&p, sizeof(p));
}

void CHARACTER::PartyInviteAccept(LPCHARACTER pchInvitee)
{
	EventMap::iterator itFind = m_PartyInviteEventMap.find(pchInvitee->GetPlayerID());

	if (itFind == m_PartyInviteEventMap.end())
	{
		sys_log(1, "PartyInviteAccept from not invited character(%s)", pchInvitee->GetName());
		return;
	}

	event_cancel(&itFind->second);
	m_PartyInviteEventMap.erase(itFind);

	if (GetParty() && GetParty()->GetLeaderPID() != GetPlayerID())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 파티원을 초대할 수 있는 권한이 없습니다."));
		return;
	}

	PartyJoinErrCode errcode = IsPartyJoinableMutableCondition(this, pchInvitee);

	switch (errcode)
	{
		case PERR_NONE:
			break;

		case PERR_SERVER:
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 서버 문제로 파티 관련 처리를 할 수 없습니다."));
			return;

		case PERR_DUNGEON:
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 던전 안에서는 파티 초대에 응할 수 없습니다."));
			return;

		case PERR_OBSERVER:
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 관전 모드에선 파티 초대를 할 수 없습니다."));
			return;

		case PERR_LVBOUNDARY:
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> -30 ~ +30 레벨 이내의 상대방만 초대할 수 있습니다."));
			return;

		case PERR_LOWLEVEL:
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 파티내 최고 레벨 보다 30레벨이 낮아 초대할 수 없습니다."));
			return;

		case PERR_HILEVEL:
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 파티내 최저 레벨 보다 30레벨이 높아 초대할 수 없습니다."));
			return;

		case PERR_ALREADYJOIN:
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 파티 초대에 응할 수 없습니다."));
			return;

		case PERR_PARTYISFULL:
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 더 이상 파티원을 초대할 수 없습니다."));
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> 파티의 인원제한이 초과하여 파티에 참가할 수 없습니다."));
			return;

		default:
			sys_err("ignore party join error(%d)", errcode);
			return;
	}

	//
	// 파티 가입 처리
	//

	if (GetParty())
		pchInvitee->PartyJoin(this);
	else
	{
		LPPARTY pParty = CPartyManager::instance().CreateParty(this);

		pParty->Join(pchInvitee->GetPlayerID());
		pParty->Link(pchInvitee);
		pParty->SendPartyInfoAllToOne(this);
	}
}

void CHARACTER::PartyInviteDeny(DWORD dwPID)
{
	EventMap::iterator itFind = m_PartyInviteEventMap.find(dwPID);

	if (itFind == m_PartyInviteEventMap.end())
	{
		sys_log(1, "PartyInviteDeny to not exist event(inviter PID: %d, invitee PID: %d)", GetPlayerID(), dwPID);
		return;
	}

	event_cancel(&itFind->second);
	m_PartyInviteEventMap.erase(itFind);

	LPCHARACTER pchInvitee = CHARACTER_MANAGER::instance().FindByPID(dwPID);
	if (pchInvitee)
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> %s님이 파티 초대를 거절하셨습니다.", pchInvitee->GetName()));
}

void CHARACTER::PartyJoin(LPCHARACTER pLeader)
{
	pLeader->ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> %s님이 파티에 참가하셨습니다.", GetName()));
	ChatPacket(CHAT_TYPE_INFO, LC_STRING("<파티> %s님의 파티에 참가하셨습니다.", pLeader->GetName()));

	pLeader->GetParty()->Join(GetPlayerID());
	pLeader->GetParty()->Link(this);
}

CHARACTER::PartyJoinErrCode CHARACTER::IsPartyJoinableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest)
{
	if (pchLeader->GetEmpire() != pchGuest->GetEmpire())
		return PERR_DIFFEMPIRE;

	return IsPartyJoinableMutableCondition(pchLeader, pchGuest);
}

static bool __party_can_join_by_level(LPCHARACTER leader, LPCHARACTER quest)
{
	int level_limit = 30;

	if (LC_IsCanada())
		level_limit = 15;
	else if (LC_IsBrazil() == true)
	{
		level_limit = 10;
	}
	else
		level_limit = 30;

	return (abs(leader->GetLevel() - quest->GetLevel()) <= level_limit);
}

CHARACTER::PartyJoinErrCode CHARACTER::IsPartyJoinableMutableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest)
{
	if (!CPartyManager::instance().IsEnablePCParty())
		return PERR_SERVER;
	else if (pchLeader->GetDungeon()
#if defined(__GUILD_DRAGONLAIR_PARTY_SYSTEM__)
		|| pchLeader->GetGuildDragonLair()
#endif
#if defined(__DEFENSE_WAVE__)
		|| pchLeader->GetDefenseWave()
#endif
		)
		return PERR_DUNGEON;
	else if (pchGuest->IsObserverMode())
		return PERR_OBSERVER;
	else if (false == __party_can_join_by_level(pchLeader, pchGuest))
		return PERR_LVBOUNDARY;
	else if (pchGuest->GetParty())
		return PERR_ALREADYJOIN;
	else if (pchLeader->GetParty())
	{
		if (pchLeader->GetParty()->GetMemberCount() == PARTY_MAX_MEMBER)
			return PERR_PARTYISFULL;
	}

	return PERR_NONE;
}
// END_OF_PARTY_JOIN_BUG_FIX

void CHARACTER::SetDungeon(LPDUNGEON pkDungeon)
{
	if (pkDungeon && m_pkDungeon)
		sys_err("%s is trying to reassigning dungeon (current %p, new party %p)", GetName(), get_pointer(m_pkDungeon), get_pointer(pkDungeon));

	if (m_pkDungeon == pkDungeon)
		return;

	if (m_pkDungeon)
	{
		if (IsPC())
		{
			if (GetParty())
				m_pkDungeon->DecPartyMember(GetParty(), this);
			else
				m_pkDungeon->DecMember(this);
		}
		else if (IsMonster() || IsStone())
		{
			m_pkDungeon->DecMonster();
		}
	}

	m_pkDungeon = pkDungeon;

	if (pkDungeon)
	{
		//sys_log(0, "%s DUNGEON set to %p, PARTY is %p", GetName(), get_pointer(pkDungeon), get_pointer(m_pkParty));

		if (IsPC())
		{
			if (GetParty())
				m_pkDungeon->IncPartyMember(GetParty(), this);
			else
				m_pkDungeon->IncMember(this);
		}
		else if (IsMonster() || IsStone())
		{
			m_pkDungeon->IncMonster();
		}
	}
}

void CHARACTER::SetWarMap(CWarMap* pWarMap)
{
	if (m_pWarMap)
		m_pWarMap->DecMember(this);

	m_pWarMap = pWarMap;

	if (m_pWarMap)
		m_pWarMap->IncMember(this);
}

void CHARACTER::SetWeddingMap(marriage::WeddingMap* pMap)
{
	if (m_pWeddingMap)
		m_pWeddingMap->DecMember(this);

	m_pWeddingMap = pMap;

	if (m_pWeddingMap)
		m_pWeddingMap->IncMember(this);
}

bool CHARACTER::IsWearingDress() const
{
	LPITEM pkItem = GetWear(WEAR_BODY);
	if (pkItem && (pkItem->GetVnum() > 11900 && pkItem->GetVnum() < 11915))
		return true;
	return false;
}

void CHARACTER::SetRegen(LPREGEN pkRegen)
{
	m_pkRegen = pkRegen;
	if (pkRegen != NULL)
	{
		regen_id_ = pkRegen->id;
	}
	m_fRegenAngle = GetRotation();
	m_posRegen = GetXYZ();
}

bool CHARACTER::OnIdle()
{
	return false;
}

void CHARACTER::OnMove(bool bIsAttack)
{
	m_dwLastMoveTime = get_dword_time();
#if defined(__LEFT_SEAT__)
	m_dwLastRequestTime = get_dword_time();
#endif

	if (bIsAttack)
	{
		m_dwLastAttackTime = m_dwLastMoveTime;

		if (IsAffectFlag(AFF_REVIVE_INVISIBLE))
			RemoveAffect(AFFECT_REVIVE_INVISIBLE);

		if (IsAffectFlag(AFF_EUNHYUNG))
		{
			RemoveAffect(SKILL_EUNHYUNG);
			SetAffectedEunhyung();
		}
		else
		{
			ClearAffectedEunhyung();
		}

		/*
		if (IsAffectFlag(AFF_JEONSIN))
			RemoveAffect(SKILL_JEONSINBANGEO);
		*/
	}

	/*
	if (IsAffectFlag(AFF_GUNGON))
		RemoveAffect(SKILL_GUNGON);
	*/

	// MINING
	mining_cancel();
	// END_OF_MINING
}

void CHARACTER::OnClick(LPCHARACTER pkChrCauser)
{
	if (!pkChrCauser)
	{
		sys_err("OnClick %s by NULL", GetName());
		return;
	}

	DWORD vid = GetVID();
	sys_log(0, "OnClick %s[vnum %d ServerUniqueID %d, pid %d] by %s", GetName(), GetRaceNum(), vid, GetPlayerID(), pkChrCauser->GetName());

#ifdef ENABLE_QUEEN_NETHIS
	if ((IsNPC()) && (GetRaceNum() == (WORD)(SnakeLair::PORTAL_VNUM)) && (SnakeLair::CSnk::instance().IsSnakeMap(pkChrCauser->GetMapIndex())))
	{
		SnakeLair::CSnk::instance().Start(pkChrCauser);
		return;
	}
#endif

	if (mining::IsVeinOfOre(this->GetRaceNum()))
	{
		pkChrCauser->mining(this);
		return;
	}

	// 교환중일때 퀘스트를 진행할 수 없다.
	{
		if (pkChrCauser->GetExchange())
		{
			if (test_server)
				sys_err("OnClick Fail (%s->%s) - pc is exchanging", pkChrCauser->GetName(), GetName());
			return;
		}
	}

	// 상점을 연상태로 퀘스트를 진행할 수 없다.
	{
		// 단, 자신은 자신의 상점을 클릭할 수 있다.
		if (pkChrCauser->GetMyShop() && pkChrCauser != this)
		{
			if (test_server)
				sys_err("OnClick Fail (%s->%s) - pc has shop", pkChrCauser->GetName(), GetName());
			return;
		}
	}

	if (IsPC())
	{
		// 타겟으로 설정된 경우는 PC에 의한 클릭도 퀘스트로 처리하도록 합니다.
		if (!CTargetManager::instance().GetTargetInfo(pkChrCauser->GetPlayerID(), TARGET_TYPE_VID, GetVID()))
		{
			// 20050317.myevan.타겟이 아닌 경우는 개인 상점 처리 기능을 작동시킨다.
			if (GetMyShop())
			{
				if (pkChrCauser->IsDead() == true) return;

				// PREVENT_TRADE_WINDOW
				if (pkChrCauser == this) // 자기는 가능
				{
					if (PreventTradeWindow(WND_MYSHOP, true/*except*/))
					{
						pkChrCauser->ChatPacket(CHAT_TYPE_INFO, LC_STRING("다른 거래중(창고,교환,상점)에는 개인상점을 사용할 수 없습니다."));
						return;
					}
				}
				else // 다른 사람이 클릭했을때
				{
					// 클릭한 사람이 교환/창고/개인상점/상점이용중이라면 불가
					if (pkChrCauser->PreventTradeWindow(WND_ALL))
					{
						pkChrCauser->ChatPacket(CHAT_TYPE_INFO, LC_STRING("다른 거래중(창고,교환,상점)에는 개인상점을 사용할 수 없습니다."));
						return;
					}

					// 클릭한 대상이 교환/창고/상점이용중이라면 불가
					//if ((GetExchange() || IsOpenSafebox() || GetShopOwner()))
					if (PreventTradeWindow(WND_MYSHOP | WND_SHOPOWNER, true/*except*/))
					{
						pkChrCauser->ChatPacket(CHAT_TYPE_INFO, LC_STRING("상대방이 다른 거래를 하고 있는 중입니다."));
						return;
					}
				}
				// END_PREVENT_TRADE_WINDOW

				if (pkChrCauser->GetShop())
				{
					pkChrCauser->GetShop()->RemoveGuest(pkChrCauser);
					pkChrCauser->SetShop(NULL);
				}

				GetMyShop()->AddGuest(pkChrCauser, GetVID(), false);
				pkChrCauser->SetShopOwner(this);
#if defined(__PRIVATESHOP_SEARCH_SYSTEM__)
				if (CTargetManager::instance().GetTargetInfo(pkChrCauser->GetPlayerID(), TARGET_TYPE_VID_SHOP_SEARCH, static_cast<DWORD>(GetVID())))
					CTargetManager::instance().DeleteTarget(pkChrCauser->GetPlayerID(), SHOP_SEARCH_INDEX, "__SHOPSEARCH_TARGET__");
#endif
				return;
			}

			if (test_server)
				sys_err("%s.OnClickFailure(%s) - target is PC", pkChrCauser->GetName(), GetName());

			return;
		}
	}

	if (!pkChrCauser->IsDead())
	{
		pkChrCauser->SetQuestNPCID(GetVID());

		if (quest::CQuestManager::instance().Click(pkChrCauser->GetPlayerID(), this))
			return;
	}

	// NPC 전용 기능 수행 : 상점 열기 등
	if (!IsPC())
	{
		if (!m_triggerOnClick.pFunc)
		{
			// NPC 트리거 시스템 로그 보기
			/*
			sys_err("%s.OnClickFailure(%s) : triggerOnClick.pFunc is EMPTY(pid=%d)",
				pkChrCauser->GetName(),
				GetName(),
				pkChrCauser->GetPlayerID());
			*/
			return;
		}

		m_triggerOnClick.pFunc(this, pkChrCauser);
	}
}

BYTE CHARACTER::GetGMLevel() const
{
	if (test_server)
		return GM_IMPLEMENTOR;
	return m_pointsInstant.gm_level;
}

void CHARACTER::SetGMLevel()
{
	if (GetDesc())
	{
		BYTE myLevel = gm_get_level(GetName(), GetDesc()->GetHostName(), GetDesc()->GetAccountTable().login);
		m_pointsInstant.gm_level = myLevel;
	}
	else
	{
		m_pointsInstant.gm_level = GM_PLAYER;
	}
}

BOOL CHARACTER::IsGM() const
{
	if (test_server)
		return true;

	if (GetGMLevel() != GM_PLAYER)
		return true;

	return false;
}

void CHARACTER::SetStone(LPCHARACTER pkChrStone)
{
	m_pkChrStone = pkChrStone;

	if (m_pkChrStone)
	{
		if (pkChrStone->m_set_pkChrSpawnedBy.find(this) == pkChrStone->m_set_pkChrSpawnedBy.end())
			pkChrStone->m_set_pkChrSpawnedBy.insert(this);
	}
}

struct FuncDeadSpawnedByStone
{
	void operator () (LPCHARACTER ch)
	{
		ch->Dead(NULL);
		ch->SetStone(NULL);
	}
};

void CHARACTER::ClearStone()
{
	if (!m_set_pkChrSpawnedBy.empty())
	{
		// 내가 스폰시킨 몬스터들을 모두 죽인다.
		FuncDeadSpawnedByStone f;
		std::for_each(m_set_pkChrSpawnedBy.begin(), m_set_pkChrSpawnedBy.end(), f);
		m_set_pkChrSpawnedBy.clear();
	}

	if (!m_pkChrStone)
		return;

	m_pkChrStone->m_set_pkChrSpawnedBy.erase(this);
	m_pkChrStone = NULL;
}

void CHARACTER::ClearTarget()
{
	if (m_pkChrTarget)
	{
		m_pkChrTarget->m_set_pkChrTargetedBy.erase(this);
		m_pkChrTarget = NULL;
	}

	TPacketGCTarget p;

	p.header = HEADER_GC_TARGET;
	p.dwVID = 0;
	p.bHPPercent = 0;
#if defined(__VIEW_TARGET_HP__) || defined(__DEFENSE_WAVE__)
	p.iMinHP = 0;
	p.iMaxHP = 0;
	p.bAlliance = false;
#endif
#if defined(__ELEMENT_SYSTEM__)
	memset(&p.bElement, 0, sizeof(p.bElement));
#endif

	CHARACTER_SET::iterator it = m_set_pkChrTargetedBy.begin();

	while (it != m_set_pkChrTargetedBy.end())
	{
		LPCHARACTER pkChr = *(it++);
		pkChr->m_pkChrTarget = NULL;

		if (!pkChr->GetDesc())
		{
			sys_err("%s %p does not have desc", pkChr->GetName(), get_pointer(pkChr));
			abort();
		}

		pkChr->GetDesc()->Packet(&p, sizeof(TPacketGCTarget));
	}

	m_set_pkChrTargetedBy.clear();
}

void CHARACTER::SetTarget(LPCHARACTER pkChrTarget)
{
	if (m_pkChrTarget == pkChrTarget)
		return;

	// CASTLE
	if (IS_CASTLE_MAP(GetMapIndex()) && !IsGM())
		return;
	// CASTLE

	if (m_pkChrTarget)
		m_pkChrTarget->m_set_pkChrTargetedBy.erase(this);

	m_pkChrTarget = pkChrTarget;

	TPacketGCTarget p;

	p.header = HEADER_GC_TARGET;

	if (m_pkChrTarget)
	{
		m_pkChrTarget->m_set_pkChrTargetedBy.insert(this);

		p.dwVID = m_pkChrTarget->GetVID();

#if defined(__VIEW_TARGET_PLAYER_HP__)
		if ((m_pkChrTarget->GetMaxHP() <= 0))
		{
			p.bHPPercent = 0;
#if defined(__VIEW_TARGET_HP__) || defined(__DEFENSE_WAVE__)
			p.iMinHP = 0;
			p.iMaxHP = 0;
#endif
		}
		else if (m_pkChrTarget->IsPC() && !m_pkChrTarget->IsPolymorphed())
		{
			p.bHPPercent = MINMAX(0, (static_cast<int64_t>(GetHP()) * 100) / GetMaxHP(), 100);
#if defined(__VIEW_TARGET_HP__)
			p.iMinHP = m_pkChrTarget->GetHP();
			p.iMaxHP = m_pkChrTarget->GetMaxHP();
#endif
		}
#else
		if ((m_pkChrTarget->IsPC() && !m_pkChrTarget->IsPolymorphed()) || (m_pkChrTarget->GetMaxHP() <= 0))
		{
			p.bHPPercent = 0;
#if defined(__VIEW_TARGET_HP__)
			p.iMinHP = 0;
			p.iMaxHP = 0;
#endif
		}
#endif
		else
		{
			if (m_pkChrTarget->GetRaceNum() == 20101 ||
				m_pkChrTarget->GetRaceNum() == 20102 ||
				m_pkChrTarget->GetRaceNum() == 20103 ||
				m_pkChrTarget->GetRaceNum() == 20104 ||
				m_pkChrTarget->GetRaceNum() == 20105 ||
				m_pkChrTarget->GetRaceNum() == 20106 ||
				m_pkChrTarget->GetRaceNum() == 20107 ||
				m_pkChrTarget->GetRaceNum() == 20108 ||
				m_pkChrTarget->GetRaceNum() == 20109)
			{
				LPCHARACTER owner = m_pkChrTarget->GetVictim();

				if (owner)
				{
					int iHorseHealth = owner->GetHorseHealth();
					int iHorseMaxHealth = owner->GetHorseMaxHealth();

					if (iHorseMaxHealth)
					{
						p.bHPPercent = MINMAX(0, iHorseHealth * 100 / iHorseMaxHealth, 100);
#if defined(__VIEW_TARGET_HP__)
						p.iMinHP = 100;
						p.iMaxHP = 100;
#endif
					}
					else
					{
						p.bHPPercent = 100;
#if defined(__VIEW_TARGET_HP__)
						p.iMinHP = 100;
						p.iMaxHP = 100;
#endif
					}
				}
				else
				{
					p.bHPPercent = 100;
#if defined(__VIEW_TARGET_HP__)
					p.iMinHP = 100;
					p.iMaxHP = 100;
#endif
				}
			}
			else
			{
				if (m_pkChrTarget->GetMaxHP() <= 0)
				{
					p.bHPPercent = 0;
#if defined(__VIEW_TARGET_HP__)
					p.iMinHP = 0;
					p.iMaxHP = 0;
#endif
				}
				else
				{
					p.bHPPercent = MINMAX(0, (static_cast<int64_t>(m_pkChrTarget->GetHP()) * 100) / m_pkChrTarget->GetMaxHP(), 100);
#if defined(__VIEW_TARGET_HP__)
					p.iMinHP = m_pkChrTarget->GetHP();
					p.iMaxHP = m_pkChrTarget->GetMaxHP();
#endif
				}
			}
		}
	}
	else
	{
		p.dwVID = 0;
		p.bHPPercent = 0;
#if defined(__VIEW_TARGET_HP__)
		p.iMinHP = 0;
		p.iMaxHP = 0;
#endif
	}

#if defined(__ELEMENT_SYSTEM__)
	memset(&p.bElement, 0, sizeof(p.bElement));
	if (m_pkChrTarget)
	{
		if (m_pkChrTarget->IsPC())
		{
			p.bElement[MOB_ELEMENT_ELECT] = m_pkChrTarget->GetPoint(POINT_ENCHANT_ELECT);
			p.bElement[MOB_ELEMENT_FIRE] = m_pkChrTarget->GetPoint(POINT_ENCHANT_FIRE);
			p.bElement[MOB_ELEMENT_ICE] = m_pkChrTarget->GetPoint(POINT_ENCHANT_ICE);
			p.bElement[MOB_ELEMENT_WIND] = m_pkChrTarget->GetPoint(POINT_ENCHANT_WIND);
			p.bElement[MOB_ELEMENT_EARTH] = m_pkChrTarget->GetPoint(POINT_ENCHANT_EARTH);
			p.bElement[MOB_ELEMENT_DARK] = m_pkChrTarget->GetPoint(POINT_ENCHANT_DARK);
		}
		else
		{
			p.bElement[MOB_ELEMENT_ELECT] = m_pkChrTarget->GetMobElement(MOB_ELEMENT_ELECT);
			p.bElement[MOB_ELEMENT_FIRE] = m_pkChrTarget->GetMobElement(MOB_ELEMENT_FIRE);
			p.bElement[MOB_ELEMENT_ICE] = m_pkChrTarget->GetMobElement(MOB_ELEMENT_ICE);
			p.bElement[MOB_ELEMENT_WIND] = m_pkChrTarget->GetMobElement(MOB_ELEMENT_WIND);
			p.bElement[MOB_ELEMENT_EARTH] = m_pkChrTarget->GetMobElement(MOB_ELEMENT_EARTH);
			p.bElement[MOB_ELEMENT_DARK] = m_pkChrTarget->GetMobElement(MOB_ELEMENT_DARK);
		}
	}
#endif

#if defined(__DEFENSE_WAVE__)
	p.bAlliance = false;
#endif

	GetDesc()->Packet(&p, sizeof(TPacketGCTarget));
}

void CHARACTER::BroadcastTargetPacket()
{
	if (m_set_pkChrTargetedBy.empty())
		return;

	TPacketGCTarget p;

	p.header = HEADER_GC_TARGET;
	p.dwVID = GetVID();

	if (GetMaxHP() <= 0)
	{
		p.bHPPercent = 0;
#if defined(__VIEW_TARGET_HP__)
		p.iMinHP = 0;
		p.iMaxHP = 0;
#endif
	}
	else
	{
#if defined(__VIEW_TARGET_PLAYER_HP__)
		p.bHPPercent = MINMAX(0, (static_cast<int64_t>(GetHP()) * 100) / GetMaxHP(), 100);
#if defined(__VIEW_TARGET_HP__)
		p.iMinHP = GetHP();
		p.iMaxHP = GetMaxHP();
#endif
#else
		if (IsPC())
		{
			p.bHPPercent = 0;
#if defined(__VIEW_TARGET_HP__)
			p.dwMinHP = 0;
			p.dwMaxHP = 0;
#endif
		}
		else
		{
			p.bHPPercent = MINMAX(0, (static_cast<int64_t>(GetHP()) * 100) / GetMaxHP(), 100);
#if defined(__VIEW_TARGET_HP__)
			p.dwMinHP = GetHP();
			p.dwMaxHP = GetMaxHP();
#endif
		}
#endif
	}

#if defined(__ELEMENT_SYSTEM__)
	memset(&p.bElement, 0, sizeof(p.bElement));

	if (IsPC())
	{
		p.bElement[MOB_ELEMENT_ELECT] = GetPoint(POINT_ENCHANT_ELECT);
		p.bElement[MOB_ELEMENT_FIRE] = GetPoint(POINT_ENCHANT_FIRE);
		p.bElement[MOB_ELEMENT_ICE] = GetPoint(POINT_ENCHANT_ICE);
		p.bElement[MOB_ELEMENT_WIND] = GetPoint(POINT_ENCHANT_WIND);
		p.bElement[MOB_ELEMENT_EARTH] = GetPoint(POINT_ENCHANT_EARTH);
		p.bElement[MOB_ELEMENT_DARK] = GetPoint(POINT_ENCHANT_DARK);
	}
	else
	{
		p.bElement[MOB_ELEMENT_ELECT] = GetMobElement(MOB_ELEMENT_ELECT);
		p.bElement[MOB_ELEMENT_FIRE] = GetMobElement(MOB_ELEMENT_FIRE);
		p.bElement[MOB_ELEMENT_ICE] = GetMobElement(MOB_ELEMENT_ICE);
		p.bElement[MOB_ELEMENT_WIND] = GetMobElement(MOB_ELEMENT_WIND);
		p.bElement[MOB_ELEMENT_EARTH] = GetMobElement(MOB_ELEMENT_EARTH);
		p.bElement[MOB_ELEMENT_DARK] = GetMobElement(MOB_ELEMENT_DARK);
	}
#endif

#if defined(__DEFENSE_WAVE__)
	p.bAlliance = false;
#endif

	CHARACTER_SET::iterator it = m_set_pkChrTargetedBy.begin();

	while (it != m_set_pkChrTargetedBy.end())
	{
		LPCHARACTER pkChr = *it++;

		if (!pkChr->GetDesc())
		{
			sys_err("%s %p does not have desc", pkChr->GetName(), get_pointer(pkChr));
			abort();
		}

		pkChr->GetDesc()->Packet(&p, sizeof(TPacketGCTarget));
	}
}

void CHARACTER::CheckTarget()
{
	if (!m_pkChrTarget)
		return;

	if (DISTANCE_APPROX(GetX() - m_pkChrTarget->GetX(), GetY() - m_pkChrTarget->GetY()) >= 4800)
		SetTarget(NULL);
}

void CHARACTER::SetWarpLocation(long lMapIndex, long x, long y)
{
	m_posWarp.x = x * 100;
	m_posWarp.y = y * 100;
	m_lWarpMapIndex = lMapIndex;
}

void CHARACTER::SaveExitLocation()
{
	m_posExit = GetXYZ();
	m_lExitMapIndex = GetMapIndex();
}

void CHARACTER::ExitToSavedLocation()
{
	sys_log(0, "ExitToSavedLocation");
	WarpSet(m_posWarp.x, m_posWarp.y, m_lWarpMapIndex);

	m_posExit.x = m_posExit.y = m_posExit.z = 0;
	m_lExitMapIndex = 0;
}

// fixme
// 지금까진 privateMapIndex 가 현재 맵 인덱스와 같은지 체크 하는 것을 외부에서 하고,
// 다르면 warpset을 불렀는데
// 이를 warpset 안으로 넣자.
bool CHARACTER::WarpSet(long x, long y, long lPrivateMapIndex)
{
	if (!IsPC())
		return false;

	long lAddr;
	long lMapIndex;
	WORD wPort;

	if (!CMapLocation::instance().Get(x, y, lMapIndex, lAddr, wPort))
	{
		sys_err("cannot find map location index %d x %d y %d name %s", lMapIndex, x, y, GetName());
		return false;
	}

	// Send Supplementary Data Block if new map requires security packages in loading this map
	{
		long lCurAddr;
		long lCurMapIndex = 0;
		WORD wCurPort;

		CMapLocation::instance().Get(GetX(), GetY(), lCurMapIndex, lCurAddr, wCurPort);

		// Do not send SDB files if char is in the same map
		if (lCurMapIndex != lMapIndex)
		{
			const TMapRegion* rMapRgn = SECTREE_MANAGER::instance().GetMapRegion(lMapIndex);
			{
				DESC_MANAGER::instance().SendClientPackageSDBToLoadMap(GetDesc(), rMapRgn->strMapName.c_str());
			}
		}
	}

	if (lPrivateMapIndex >= 10000)
	{
		if (lPrivateMapIndex / 10000 != lMapIndex)
		{
			sys_err("Invalid map index %d, must be child of %d", lPrivateMapIndex, lMapIndex);
			return false;
		}

		lMapIndex = lPrivateMapIndex;
	}

	Stop();
	Save();

	if (GetSectree())
	{
		GetSectree()->RemoveEntity(this);
		ViewCleanup();

		EncodeRemovePacket(this);
	}

	m_lWarpMapIndex = lMapIndex;
	m_posWarp.x = x;
	m_posWarp.y = y;

	sys_log(0, "WarpSet %s %d %d current map %d target map %d", GetName(), x, y, GetMapIndex(), lMapIndex);

	TPacketGCWarp p;

	p.bHeader = HEADER_GC_WARP;
	p.lX = x;
	p.lY = y;
#if defined(__PROXY_IP__)
	p.lAddr = g_stProxyIP.empty() ? lAddr : inet_addr(g_stProxyIP.c_str());
#else
	p.lAddr = lAddr;
#endif
	p.wPort = wPort;

	GetDesc()->Packet(&p, sizeof(TPacketGCWarp));

	char buf[256];
	snprintf(buf, sizeof(buf), "%s MapIdx %ld DestMapIdx%ld DestX%ld DestY%ld Empire%d", GetName(), GetMapIndex(), lPrivateMapIndex, x, y, GetEmpire());
	LogManager::instance().CharLog(this, 0, "WARP", buf);

	return true;
}

void CHARACTER::WarpEnd()
{
	if (test_server)
		sys_log(0, "WarpEnd %s", GetName());

	if (m_posWarp.x == 0 && m_posWarp.y == 0)
		return;

	int index = m_lWarpMapIndex;

	if (index > 10000)
		index /= 10000;

	if (!map_allow_find(index))
	{
		// 이 곳으로 워프할 수 없으므로 워프하기 전 좌표로 되돌리자.
		sys_err("location %d %d not allowed to login this server", m_posWarp.x, m_posWarp.y);
		if (g_bIgnoreDisallowedMap) // 20200807.Owsap : Go home if map index isn't allowed.
			GoHome();
		else
			GetDesc()->SetPhase(PHASE_CLOSE);

		return;
	}

	sys_log(0, "WarpEnd %s %d %u %u", GetName(), m_lWarpMapIndex, m_posWarp.x, m_posWarp.y);

	Show(m_lWarpMapIndex, m_posWarp.x, m_posWarp.y, 0);
	Stop();

	m_lWarpMapIndex = 0;
	m_posWarp.x = m_posWarp.y = m_posWarp.z = 0;

	{
		// P2P Login
		TPacketGGLogin p;

		p.bHeader = HEADER_GG_LOGIN;
		strlcpy(p.szName, GetName(), sizeof(p.szName));
		p.dwPID = GetPlayerID();
		p.bEmpire = GetEmpire();
		p.lMapIndex = SECTREE_MANAGER::instance().GetMapIndex(GetX(), GetY());
		p.bChannel = g_bChannel;

		P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGLogin));
	}
}

bool CHARACTER::Return()
{
	if (!IsNPC())
		return false;

	int x, y;
	/*
	float fDist = DISTANCE_SQRT(m_pkMobData->m_posLastAttacked.x - GetX(), m_pkMobData->m_posLastAttacked.y - GetY());
	float fx, fy;
	GetDeltaByDegree(GetRotation(), fDist, &fx, &fy);
	x = GetX() + (int)fx;
	y = GetY() + (int)fy;
	*/
	SetVictim(NULL);

	x = m_pkMobInst->m_posLastAttacked.x;
	y = m_pkMobInst->m_posLastAttacked.y;

	SetRotationToXY(x, y);

	if (!Goto(x, y))
		return false;

	SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);

	if (test_server)
		sys_log(0, "%s %p 포기하고 돌아가자! %d %d", GetName(), this, x, y);

	if (GetParty())
		GetParty()->SendMessage(this, PM_RETURN, x, y);

	return true;
}

bool CHARACTER::Follow(LPCHARACTER pkChr, float fMinDistance)
{
	if (IsPC())
	{
		sys_err("CHARACTER::Follow : PC cannot use this method", GetName());
		return false;
	}

	// TRENT_MONSTER
	if (IsNoMove())
	{
		if (pkChr->IsPC()) // 쫓아가는 상대가 PC일 때
		{
			// If i'm in a party. I must obey party leader's AI.
			if (!GetParty() || !GetParty()->GetLeader() || GetParty()->GetLeader() == this)
			{
				if (get_dword_time() - m_pkMobInst->m_dwLastAttackedTime >= 15000) // 마지막으로 공격받은지 15초가 지났고
				{
					// 마지막 맞은 곳으로 부터 50미터 이상 차이나면 포기하고 돌아간다.
					if (m_pkMobData->m_table.wAttackRange < DISTANCE_APPROX(pkChr->GetX() - GetX(), pkChr->GetY() - GetY()))
						if (Return())
							return true;
				}
			}
		}
		return false;
	}
	// END_OF_TRENT_MONSTER

	long x = pkChr->GetX();
	long y = pkChr->GetY();

	if (pkChr->IsPC()) // 쫓아가는 상대가 PC일 때
	{
		// If i'm in a party. I must obey party leader's AI.
		if (!GetParty() || !GetParty()->GetLeader() || GetParty()->GetLeader() == this)
		{
			if (get_dword_time() - m_pkMobInst->m_dwLastAttackedTime >= 15000) // 마지막으로 공격받은지 15초가 지났고
			{
				// 마지막 맞은 곳으로 부터 50미터 이상 차이나면 포기하고 돌아간다.
				if (5000 < DISTANCE_APPROX(m_pkMobInst->m_posLastAttacked.x - GetX(), m_pkMobInst->m_posLastAttacked.y - GetY()))
					if (Return())
						return true;
			}
		}
	}

	if (IsGuardNPC())
	{
		if (5000 < DISTANCE_APPROX(m_pkMobInst->m_posLastAttacked.x - GetX(), m_pkMobInst->m_posLastAttacked.y - GetY()))
			if (Return())
				return true;
	}

	SECTREE* sectree = pkChr->GetSectree();
	if (sectree && sectree->IsAttr(pkChr->GetX(), pkChr->GetY(), ATTR_BANPK))
		if (Return())
			return true;

	if (pkChr->IsState(pkChr->m_stateMove) &&
		GetMobBattleType() != BATTLE_TYPE_RANGE &&
		GetMobBattleType() != BATTLE_TYPE_MAGIC
#if defined(__PET_SYSTEM__)
		&& false == IsPet()
#endif
		)
	{
		// 대상이 이동중이면 예측 이동을 한다
		// 나와 상대방의 속도차와 거리로부터 만날 시간을 예상한 후
		// 상대방이 그 시간까지 직선으로 이동한다고 가정하여 거기로 이동한다.
		float rot = pkChr->GetRotation();
		float rot_delta = GetDegreeDelta(rot, GetDegreeFromPositionXY(GetX(), GetY(), pkChr->GetX(), pkChr->GetY()));

		float yourSpeed = pkChr->GetMoveSpeed();
		float mySpeed = GetMoveSpeed();

		float fDist = DISTANCE_SQRT(x - GetX(), y - GetY());
		float fFollowSpeed = mySpeed - yourSpeed * cos(rot_delta * M_PI / 180);

		if (fFollowSpeed >= 0.1f)
		{
			float fMeetTime = fDist / fFollowSpeed;
			float fYourMoveEstimateX, fYourMoveEstimateY;

			if (fMeetTime * yourSpeed <= 100000.0f)
			{
				GetDeltaByDegree(pkChr->GetRotation(), fMeetTime * yourSpeed, &fYourMoveEstimateX, &fYourMoveEstimateY);

				x += (long)fYourMoveEstimateX;
				y += (long)fYourMoveEstimateY;

				float fDistNew = sqrt(((double)x - GetX()) * (x - GetX()) + ((double)y - GetY()) * (y - GetY()));
				if (fDist < fDistNew)
				{
					x = (long)(GetX() + (x - GetX()) * fDist / fDistNew);
					y = (long)(GetY() + (y - GetY()) * fDist / fDistNew);
				}
			}
		}
	}

	// 가려는 위치를 바라봐야 한다.
	SetRotationToXY(x, y);

	float fDist = DISTANCE_SQRT(x - GetX(), y - GetY());

	if (fDist <= fMinDistance)
		return false;

	float fx, fy;

	if (IsChangeAttackPosition(pkChr) && GetMobRank() < MOB_RANK_BOSS)
	{
		// 상대방 주변 랜덤한 곳으로 이동
		SetChangeAttackPositionTime();

		int retry = 16;
		int dx, dy;
		int rot = (int)GetDegreeFromPositionXY(x, y, GetX(), GetY());

		while (--retry)
		{
			if (fDist < 500.0f)
				GetDeltaByDegree((rot + number(-90, 90) + number(-90, 90)) % 360, fMinDistance, &fx, &fy);
			else
				GetDeltaByDegree(number(0, 359), fMinDistance, &fx, &fy);

			dx = x + (int)fx;
			dy = y + (int)fy;

			LPSECTREE tree = SECTREE_MANAGER::instance().Get(GetMapIndex(), dx, dy);
			if (NULL == tree)
				break;

			if (0 == (tree->GetAttribute(dx, dy) & (ATTR_BLOCK | ATTR_OBJECT)))
				break;
		}

		//sys_log(0, "근처 어딘가로 이동 %s retry %d", GetName(), retry);
		if (!Goto(dx, dy))
			return false;
	}
	else
	{
		// 직선 따라가기
		float fDistToGo = fDist - fMinDistance;
		GetDeltaByDegree(GetRotation(), fDistToGo, &fx, &fy);

		//sys_log(0, "직선으로 이동 %s", GetName());
		if (!Goto(GetX() + (int)fx, GetY() + (int)fy))
			return false;
	}

	SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);
	//MonsterLog("쫓아가기; %s", pkChr->GetName());
	return true;
}

float CHARACTER::GetDistanceFromSafeboxOpen() const
{
	return DISTANCE_APPROX(GetX() - m_posSafeboxOpen.x, GetY() - m_posSafeboxOpen.y);
}

void CHARACTER::SetSafeboxOpenPosition()
{
	m_posSafeboxOpen = GetXYZ();
}

CSafebox* CHARACTER::GetSafebox() const
{
	return m_pkSafebox;
}

void CHARACTER::ReqSafeboxLoad(const char* pszPassword)
{
	if (!*pszPassword || strlen(pszPassword) > SAFEBOX_PASSWORD_MAX_LEN)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("<창고> 잘못된 암호를 입력하셨습니다."));
		return;
	}
	else if (m_pkSafebox)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("<창고> 창고가 이미 열려있습니다."));
		return;
	}

	int iPulse = thecore_pulse();
	if (iPulse - GetSafeboxLoadTime() < PASSES_PER_SEC(10))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("<창고> 창고를 닫은지 10초 안에는 열 수 없습니다."));
		return;
	}
	else if (GetDistanceFromSafeboxOpen() > 1000)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("<창고> 거리가 멀어서 창고를 열 수 없습니다."));
		return;
	}
	else if (m_bOpeningSafebox)
	{
		sys_log(0, "Overlapped safebox load request from %s", GetName());
		return;
	}

	SetSafeboxLoadTime();
	m_bOpeningSafebox = true;

	TSafeboxLoadPacket p;
	p.dwID = GetDesc()->GetAccountTable().id;
	strlcpy(p.szLogin, GetDesc()->GetAccountTable().login, sizeof(p.szLogin));
	strlcpy(p.szPassword, pszPassword, sizeof(p.szPassword));

	db_clientdesc->DBPacket(HEADER_GD_SAFEBOX_LOAD, GetDesc()->GetHandle(), &p, sizeof(p));
}

void CHARACTER::LoadSafebox(int iSize, DWORD dwGold, int iItemCount, TPlayerItem* pItems)
{
	bool bLoaded = false;

	// PREVENT_TRADE_WINDOW
	SetOpenSafebox(true);
	// END_PREVENT_TRADE_WINDOW

	if (m_pkSafebox)
		bLoaded = true;

	if (!m_pkSafebox)
		m_pkSafebox = M2_NEW CSafebox(this, iSize, dwGold);
	else
		m_pkSafebox->ChangeSize(iSize);

	m_iSafeboxSize = iSize;

	TPacketCGSafeboxSize p;
	p.bHeader = HEADER_GC_SAFEBOX_SIZE;
	p.bSize = iSize;

	GetDesc()->Packet(&p, sizeof(TPacketCGSafeboxSize));

	if (!bLoaded)
	{
		for (int i = 0; i < iItemCount; ++i, ++pItems)
		{
			if (!m_pkSafebox->IsValidPosition(pItems->wPos))
				continue;

			LPITEM item = ITEM_MANAGER::instance().CreateItem(pItems->dwVnum, pItems->dwCount, pItems->dwID);
			if (!item)
			{
				sys_err("cannot create item vnum %d id %u (name: %s)", pItems->dwVnum, pItems->dwID, GetName());
				continue;
			}

			item->SetSkipSave(true);
#if defined(__SOUL_BIND_SYSTEM__)
			item->SealItem(pItems->lSealDate);
#endif
			item->SetSockets(pItems->alSockets);
			item->SetAttributes(pItems->aAttr);
#if defined(__CHANGE_LOOK_SYSTEM__)
			item->SetTransmutationVnum(pItems->dwTransmutationVnum);
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
			item->SetRefineElement(&pItems->RefineElement);
#endif
#if defined(__ITEM_APPLY_RANDOM__)
			item->SetRandomApplies(pItems->aApplyRandom);
#endif
#if defined(__SET_ITEM__)
			item->SetItemSetValue(pItems->bSetValue);
#endif

			if (!m_pkSafebox->Add(pItems->wPos, item))
			{
				M2_DESTROY_ITEM(item);
			}
			else
				item->SetSkipSave(false);
		}
	}
}

void CHARACTER::ChangeSafeboxSize(BYTE bSize)
{
	//if (!m_pkSafebox)
	//	return;

	TPacketCGSafeboxSize p;
	p.bHeader = HEADER_GC_SAFEBOX_SIZE;
	p.bSize = bSize;

	GetDesc()->Packet(&p, sizeof(TPacketCGSafeboxSize));

	if (m_pkSafebox)
		m_pkSafebox->ChangeSize(bSize);

	m_iSafeboxSize = bSize;
}

void CHARACTER::CloseSafebox()
{
	if (!m_pkSafebox)
		return;

	// PREVENT_TRADE_WINDOW
	SetOpenSafebox(false);
	// END_PREVENT_TRADE_WINDOW

	m_pkSafebox->Save();

	M2_DELETE(m_pkSafebox);
	m_pkSafebox = NULL;

	ChatPacket(CHAT_TYPE_COMMAND, "CloseSafebox");

	SetSafeboxLoadTime();
	m_bOpeningSafebox = false;

	Save();
}

void CHARACTER::LoadSafeboxBuff()
{
	if (!g_bSafeboxChangePasswordBuff)
		return;

	if (FindAffect(AFFECT_SAFE_BOX_BUFF))
		return;

	char szQuery[254 + 1];
	snprintf(szQuery, sizeof(szQuery), "SELECT `buff_point`, `buff_value`, `buff_duration` FROM safebox%s WHERE `account_id` = %u",
		get_table_postfix(), GetDesc()->GetAccountTable().id);

	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
	SQLResult* pRes = pMsg->Get();
	if (pRes->uiNumRows > 0)
	{
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);

		BYTE bPoint = POINT_NONE;
		str_to_number(bPoint, row[0]);

		long lValue = 0;
		str_to_number(lValue, row[1]);

		long lDuration = 0;
		str_to_number(lDuration, row[2]);

		if (lDuration > std::time(nullptr))
		{
			long lRemainingTime = lDuration - std::time(nullptr);
			if (lRemainingTime > 300)
				AddAffect(AFFECT_SAFE_BOX_BUFF, bPoint, lValue, AFF_NONE, lRemainingTime, 0, true);
		}
	}
}

void CHARACTER::SetSafeboxBuff()
{
	if (!g_bSafeboxChangePasswordBuff)
		return;

	if (FindAffect(AFFECT_SAFE_BOX_BUFF))
		return;

	//
	// NOTE: Safebox Change Password Buff
	//

	long lDuration = 60 * 60 * 24 * 90;
	using SafeboxBuff = struct {
		BYTE bPoint;
		long lValue;
		std::string stText;
	};
	using SafeboxBuffVec = std::vector<SafeboxBuff>;

	static SafeboxBuffVec s_vSafeboxBuf{
		{ POINT_MALL_DEFBONUS, 20, "You have received a storage bonus of %d defence.", },
		{ POINT_MAX_HP_PCT, 1, "You have received a storage bonus of %d%% HP.", },
		{ POINT_MALL_EXPBONUS, 5, "You have received a storage bonus of %d%% EXP.", },
		{ POINT_ATTBONUS_MONSTER, 3, "You have received a storage bonus of %d%% strength against monsters." },
	};

	SafeboxBuffVec::const_iterator it = s_vSafeboxBuf.begin();
	std::advance(it, std::rand() % s_vSafeboxBuf.size());

	char szQuery[254 + 1];
	snprintf(szQuery, sizeof(szQuery), "UPDATE safebox%s SET `buff_point` = %d, `buff_value` = %d, `buff_duration` = %d WHERE `account_id` = %u",
		get_table_postfix(), it->bPoint, it->lValue, std::time(nullptr) + lDuration, GetDesc()->GetAccountTable().id);
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
	if (pMsg->Get()->uiAffectedRows > 0)
	{
		AddAffect(AFFECT_SAFE_BOX_BUFF, it->bPoint, it->lValue, AFF_NONE, lDuration, 0, true);
		ChatPacket(CHAT_TYPE_INFO, LC_STRING(it->stText.c_str(), it->lValue));
	}
}

CSafebox* CHARACTER::GetMall() const
{
	return m_pkMall;
}

void CHARACTER::LoadMall(int iItemCount, TPlayerItem* pItems)
{
	bool bLoaded = false;

	if (m_pkMall)
		bLoaded = true;

	if (!m_pkMall)
#if defined(__EXTEND_MALLBOX__)
		m_pkMall = M2_NEW CSafebox(this, 5 * SAFEBOX_PAGE_SIZE, 0);
	else
		m_pkMall->ChangeSize(5 * SAFEBOX_PAGE_SIZE);
#else
		m_pkMall = M2_NEW CSafebox(this, 3 * SAFEBOX_PAGE_SIZE, 0);
	else
		m_pkMall->ChangeSize(3 * SAFEBOX_PAGE_SIZE);
#endif

	m_pkMall->SetWindowMode(MALL);

	TPacketCGSafeboxSize p;

	p.bHeader = HEADER_GC_MALL_OPEN;
#if defined(__EXTEND_MALLBOX__)
	p.bSize = 5 * SAFEBOX_PAGE_SIZE;
#else
	p.bSize = 3 * SAFEBOX_PAGE_SIZE;
#endif

	GetDesc()->Packet(&p, sizeof(TPacketCGSafeboxSize));

	if (!bLoaded)
	{
		for (int i = 0; i < iItemCount; ++i, ++pItems)
		{
			if (!m_pkMall->IsValidPosition(pItems->wPos))
				continue;

			LPITEM item = ITEM_MANAGER::instance().CreateItem(pItems->dwVnum, pItems->dwCount, pItems->dwID);

			if (!item)
			{
				sys_err("cannot create item vnum %d id %u (name: %s)", pItems->dwVnum, pItems->dwID, GetName());
				continue;
			}

			item->SetSkipSave(true);
#if defined(__SOUL_BIND_SYSTEM__)
			item->SealItem(pItems->lSealDate);
#endif
			item->SetSockets(pItems->alSockets);
			item->SetAttributes(pItems->aAttr);
#if defined(__CHANGE_LOOK_SYSTEM__)
			item->SetTransmutationVnum(pItems->dwTransmutationVnum);
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
			item->SetRefineElement(&pItems->RefineElement);
#endif
#if defined(__ITEM_APPLY_RANDOM__)
			item->SetRandomApplies(pItems->aApplyRandom);
#endif
#if defined(__SET_ITEM__)
			item->SetItemSetValue(pItems->bSetValue);
#endif

			if (!m_pkMall->Add(pItems->wPos, item))
				M2_DESTROY_ITEM(item);
			else
				item->SetSkipSave(false);
		}
	}
}

void CHARACTER::CloseMall()
{
	if (!m_pkMall)
		return;

	m_pkMall->Save();

	M2_DELETE(m_pkMall);
	m_pkMall = NULL;

	ChatPacket(CHAT_TYPE_COMMAND, "CloseMall");
}

bool CHARACTER::BuildUpdatePartyPacket(TPacketGCPartyUpdate& out)
{
	if (!GetParty())
		return false;

	memset(&out, 0, sizeof(out));

	out.header = HEADER_GC_PARTY_UPDATE;
	out.pid = GetPlayerID();
	out.percent_hp = (GetMaxHP() <= 0) ? 0 : MINMAX(0, GetHP() * 100 / GetMaxHP(), 100);
	out.role = GetParty()->GetRole(GetPlayerID());

	sys_log(1, "PARTY %s role is %d", GetName(), out.role);

	LPCHARACTER l = GetParty()->GetLeaderCharacter();

	if (l && DISTANCE_APPROX(GetX() - l->GetX(), GetY() - l->GetY()) < PARTY_DEFAULT_RANGE)
	{
		if (g_iUseLocale)
			out.affects[0] = GetParty()->GetPartyBonusExpPercent();
		else
			out.affects[0] = GetParty()->GetExpBonusPercent();
		out.affects[1] = GetPoint(POINT_PARTY_ATTACKER_BONUS);
		out.affects[2] = GetPoint(POINT_PARTY_TANKER_BONUS);
		out.affects[3] = GetPoint(POINT_PARTY_BUFFER_BONUS);
		out.affects[4] = GetPoint(POINT_PARTY_SKILL_MASTER_BONUS);
		out.affects[5] = GetPoint(POINT_PARTY_HASTE_BONUS);
		out.affects[6] = GetPoint(POINT_PARTY_DEFENDER_BONUS);
	}

#if defined(__WJ_SHOW_PARTY_ON_MINIMAP__)
	out.x = GetX();
	out.y = GetY();
#endif

	return true;
}

int CHARACTER::GetLeadershipSkillLevel() const
{
	return GetSkillLevel(SKILL_LEADERSHIP);
}

#if defined(__PARTY_PROFICY__)
int CHARACTER::GetRoleProficiencySkillLevel() const
{
	return GetSkillLevel(SKILL_ROLE_PROFICIENCY );
}
#endif

#if defined(__PARTY_INSIGHT__)
int CHARACTER::GetInSightSkillLevel() const
{
	return GetSkillLevel(SKILL_INSIGHT);
}
#endif

void CHARACTER::QuerySafeboxSize()
{
	if (m_iSafeboxSize == -1)
	{
		DBManager::instance().ReturnQuery(QID_SAFEBOX_SIZE,
			GetPlayerID(),
			NULL,
			"SELECT size FROM safebox%s WHERE account_id = %u",
			get_table_postfix(),
			GetDesc()->GetAccountTable().id);
	}
}

void CHARACTER::SetSafeboxSize(int iSize)
{
	sys_log(1, "SetSafeboxSize: %s %d", GetName(), iSize);
	m_iSafeboxSize = iSize;
	DBManager::instance().Query("UPDATE safebox%s SET size = %d WHERE account_id = %u", get_table_postfix(), iSize / SAFEBOX_PAGE_SIZE, GetDesc()->GetAccountTable().id);
}

int CHARACTER::GetSafeboxSize() const
{
	return m_iSafeboxSize;
}

void CHARACTER::SetNowWalking(bool bWalkFlag)
{
	//if (m_bNowWalking != bWalkFlag || IsNPC())
	if (m_bNowWalking != bWalkFlag)
	{
		if (bWalkFlag)
		{
			m_bNowWalking = true;
			m_dwWalkStartTime = get_dword_time();
		}
		else
		{
			m_bNowWalking = false;
		}

		//if (m_bNowWalking)
		{
			TPacketGCWalkMode p;
			p.vid = GetVID();
			p.header = HEADER_GC_WALK_MODE;
			p.mode = m_bNowWalking ? WALKMODE_WALK : WALKMODE_RUN;

			PacketView(&p, sizeof(p));
		}

		if (IsNPC())
		{
			if (m_bNowWalking)
				MonsterLog("걷는다");
			else
				MonsterLog("뛴다");
		}

		//sys_log(0, "%s is now %s", GetName(), m_bNowWalking ? "walking." : "running.");
	}
}

void CHARACTER::StartStaminaConsume()
{
	if (m_bStaminaConsume)
		return;

	PointChange(POINT_STAMINA, 0);
	m_bStaminaConsume = true;
	//ChatPacket(CHAT_TYPE_COMMAND, "StartStaminaConsume %d %d", STAMINA_PER_STEP * passes_per_sec, GetStamina());
	if (IsStaminaHalfConsume())
		ChatPacket(CHAT_TYPE_COMMAND, "StartStaminaConsume %d %d", STAMINA_PER_STEP * passes_per_sec / 2, GetStamina());
	else
		ChatPacket(CHAT_TYPE_COMMAND, "StartStaminaConsume %d %d", STAMINA_PER_STEP * passes_per_sec, GetStamina());
}

void CHARACTER::StopStaminaConsume()
{
	if (!m_bStaminaConsume)
		return;

	PointChange(POINT_STAMINA, 0);
	m_bStaminaConsume = false;
	ChatPacket(CHAT_TYPE_COMMAND, "StopStaminaConsume %d", GetStamina());
}

bool CHARACTER::IsStaminaConsume() const
{
	return m_bStaminaConsume;
}

bool CHARACTER::IsStaminaHalfConsume() const
{
	return IsEquipUniqueItem(UNIQUE_ITEM_HALF_STAMINA);
}

void CHARACTER::ResetStopTime()
{
	m_dwStopTime = get_dword_time();
}

DWORD CHARACTER::GetStopTime() const
{
	return m_dwStopTime;
}

void CHARACTER::ResetPoint(int iLv)
{
	const BYTE bJob = GetJob();

	PointChange(POINT_LEVEL, iLv - GetLevel(), false, true);

	SetRealPoint(POINT_ST, JobInitialPoints[bJob].st);
	SetPoint(POINT_ST, GetRealPoint(POINT_ST));

	SetRealPoint(POINT_HT, JobInitialPoints[bJob].ht);
	SetPoint(POINT_HT, GetRealPoint(POINT_HT));

	SetRealPoint(POINT_DX, JobInitialPoints[bJob].dx);
	SetPoint(POINT_DX, GetRealPoint(POINT_DX));

	SetRealPoint(POINT_IQ, JobInitialPoints[bJob].iq);
	SetPoint(POINT_IQ, GetRealPoint(POINT_IQ));

	SetRandomHP((iLv - 1) * number(JobInitialPoints[GetJob()].hp_per_lv_begin, JobInitialPoints[GetJob()].hp_per_lv_end));
	SetRandomSP((iLv - 1) * number(JobInitialPoints[GetJob()].sp_per_lv_begin, JobInitialPoints[GetJob()].sp_per_lv_end));

	PointChange(POINT_STAT, (MINMAX(1, iLv, gPlayerMaxLevelStats) * 3) + GetPoint(POINT_LEVEL_STEP) - GetPoint(POINT_STAT));

	ComputePoints();

	// 회복
	PointChange(POINT_HP, GetMaxHP() - GetHP());
	PointChange(POINT_SP, GetMaxSP() - GetSP());

	PointsPacket();

	LogManager::instance().CharLog(this, 0, "RESET_POINT", "");
}

void CHARACTER::ResetExp()
{
	SetExp(0);

	PointsPacket();

	UpdatePointsPacket(POINT_EXP, GetExp());

	LogManager::instance().CharLog(this, 0, "RESET_EXP", "");
}

#if defined(__CONQUEROR_LEVEL__)
void CHARACTER::ResetConquerorPoint(int iLv)
{
	PointChange(POINT_CONQUEROR_LEVEL, MINMAX(0, iLv, gPlayerMaxConquerorLevel) - GetConquerorLevel());

	SetRealPoint(POINT_CONQUEROR_LEVEL_STEP, 0);
	SetPoint(POINT_CONQUEROR_LEVEL_STEP, GetRealPoint(POINT_CONQUEROR_LEVEL_STEP));

	SetRealPoint(POINT_CONQUEROR_POINT, MINMAX(0, (iLv * 4) - 4, (4 * (gPlayerMaxConquerorLevel - 1))));
	SetPoint(POINT_CONQUEROR_POINT, GetRealPoint(POINT_CONQUEROR_POINT));

	SetRealPoint(POINT_SUNGMA_STR, 0);
	SetPoint(POINT_SUNGMA_STR, GetRealPoint(POINT_SUNGMA_STR));

	SetRealPoint(POINT_SUNGMA_HP, 0);
	SetPoint(POINT_SUNGMA_HP, GetRealPoint(POINT_SUNGMA_HP));

	SetRealPoint(POINT_SUNGMA_MOVE, 0);
	SetPoint(POINT_SUNGMA_MOVE, GetRealPoint(POINT_SUNGMA_MOVE));

	SetRealPoint(POINT_SUNGMA_IMMUNE, 0);
	SetPoint(POINT_SUNGMA_IMMUNE, GetRealPoint(POINT_SUNGMA_IMMUNE));

	SetRealPoint(POINT_CONQUEROR_LEVEL_STEP, 0);
	SetPoint(POINT_CONQUEROR_LEVEL_STEP, GetRealPoint(POINT_CONQUEROR_LEVEL_STEP));

	ComputePoints();
	PointsPacket();

	LogManager::instance().CharLog(this, 0, "RESET_CONQUEROR_POINT", "");
}

void CHARACTER::ResetConquerorExp()
{
	SetConquerorExp(0);

	PointsPacket();

	UpdatePointsPacket(POINT_CONQUEROR_EXP, GetConquerorExp());

	LogManager::instance().CharLog(this, 0, "RESET_CONQUEROR_EXP", "");
}
#endif

bool CHARACTER::IsChangeAttackPosition(LPCHARACTER target) const
{
	if (!IsNPC())
		return true;

	DWORD dwChangeTime = AI_CHANGE_ATTACK_POISITION_TIME_NEAR;

	if (DISTANCE_APPROX(GetX() - target->GetX(), GetY() - target->GetY()) >
		AI_CHANGE_ATTACK_POISITION_DISTANCE + GetMobAttackRange())
		dwChangeTime = AI_CHANGE_ATTACK_POISITION_TIME_FAR;

	return get_dword_time() - m_dwLastChangeAttackPositionTime > dwChangeTime;
}

void CHARACTER::GiveRandomSkillBook()
{
	LPITEM item = AutoGiveItem(ITEM_SKILLBOOK_VNUM);

	if (NULL != item)
	{
		BYTE bJob = 0;

		if (!number(0, 1))
			bJob = GetJob() + 1;

		DWORD dwSkillVnum = 0;

		do
		{
#if defined(__WOLFMAN_CHARACTER__)
			dwSkillVnum = number(1, 175);
			if (dwSkillVnum > 111 && dwSkillVnum < 170)
				continue;
#else
			dwSkillVnum = number(1, 111);
#endif
			const CSkillProto* pkSk = CSkillManager::instance().Get(dwSkillVnum);

			if (NULL == pkSk)
				continue;

			if (bJob && bJob != pkSk->dwType)
				continue;

			break;
		} while (true);

		item->SetSocket(0, dwSkillVnum);
	}
}

void CHARACTER::ReviveInvisible(int iDur)
{
	AddAffect(AFFECT_REVIVE_INVISIBLE, POINT_NONE, 0, AFF_REVIVE_INVISIBLE, iDur, 0, true);
}

void CHARACTER::ToggleMonsterLog()
{
	m_bMonsterLog = !m_bMonsterLog;

	if (m_bMonsterLog)
	{
		ChatPacket(CHAT_TYPE_NOTICE, "Registered Monster Log");
		CHARACTER_MANAGER::instance().RegisterForMonsterLog(this);
	}
	else
	{
		ChatPacket(CHAT_TYPE_NOTICE, "Unregistered Monster Log");
		CHARACTER_MANAGER::instance().UnregisterForMonsterLog(this);
	}
}

void CHARACTER::SetGuild(CGuild* pGuild)
{
	if (m_pGuild != pGuild)
	{
		m_pGuild = pGuild;
		UpdatePacket();
	}
}

void CHARACTER::SendGreetMessage()
{
	auto v = DBManager::instance().GetGreetMessage();
	for (auto it = v.begin(); it != v.end(); ++it)
	{
		ChatPacket(CHAT_TYPE_NOTICE, it->c_str());
	}
}

void CHARACTER::BeginStateEmpty()
{
	MonsterLog("!");
}

void CHARACTER::EffectPacket(BYTE bEffectNum, BYTE bEffectType, const PIXEL_POSITION& rEffectPos)
{
	TPacketGCSpecialEffect p;
	p.bHeader = HEADER_GC_SEPCIAL_EFFECT;
	p.bEffectNum = bEffectNum;
	p.dwVID = GetVID();
	p.bEffectType = bEffectType;
	p.xEffectPos = rEffectPos.x;
	p.yEffectPos = rEffectPos.y;
	PacketAround(&p, sizeof(TPacketGCSpecialEffect));
}

void CHARACTER::SpecificEffectPacket(const char filename[MAX_EFFECT_FILE_NAME])
{
	TPacketGCSpecificEffect p;

	p.header = HEADER_GC_SPECIFIC_EFFECT;
	p.vid = GetVID();
	memcpy(p.effect_file, filename, MAX_EFFECT_FILE_NAME);

	PacketAround(&p, sizeof(TPacketGCSpecificEffect));
}

void CHARACTER::MonsterChat(BYTE bMonsterChatType)
{
	if (IsPC())
		return;

	char sbuf[256 + 1] = { 0 };

	if (IsMonster())
	{
		if (number(0, 60))
			return;

		snprintf(sbuf, sizeof(sbuf),
			"(locale.monster_chat[%i] and locale.monster_chat[%i][%d] or '')",
			GetRaceNum(), GetRaceNum(), bMonsterChatType * 3 + number(1, 3));
	}
	else
	{
		if (bMonsterChatType != MONSTER_CHAT_WAIT)
			return;

		if (IsGuardNPC())
		{
			if (number(0, 6))
				return;
		}
		else
		{
			if (number(0, 30))
				return;
		}

		snprintf(sbuf, sizeof(sbuf), "(locale.monster_chat[%i] and locale.monster_chat[%i][number(1, table.getn(locale.monster_chat[%i]))] or '')", GetRaceNum(), GetRaceNum(), GetRaceNum());
	}

	std::string text = quest::ScriptToString(sbuf);
	if (text.empty())
		return;

	TPacketGCChat pack_chat;
	pack_chat.header = HEADER_GC_CHAT;
	pack_chat.size = sizeof(struct packet_chat) + text.size() + 1;
	pack_chat.type = CHAT_TYPE_TALKING;
	pack_chat.id = GetVID();
	pack_chat.bEmpire = 0;

	TEMP_BUFFER buf;
	buf.write(&pack_chat, sizeof(struct packet_chat));
	buf.write(text.c_str(), text.size() + 1);

	PacketAround(buf.read_peek(), buf.size());
}

void CHARACTER::SetQuestNPCID(DWORD vid)
{
	m_dwQuestNPCVID = vid;
}

LPCHARACTER CHARACTER::GetQuestNPC() const
{
	return CHARACTER_MANAGER::instance().Find(m_dwQuestNPCVID);
}

void CHARACTER::SetQuestItemPtr(LPITEM item)
{
	m_pQuestItem = item;
}

void CHARACTER::ClearQuestItemPtr()
{
	m_pQuestItem = NULL;
}

LPITEM CHARACTER::GetQuestItemPtr() const
{
	return m_pQuestItem;
}

LPDUNGEON CHARACTER::GetDungeonForce() const
{
	if (m_lWarpMapIndex > 10000)
		return CDungeonManager::instance().FindByMapIndex(m_lWarpMapIndex);

	return m_pkDungeon;
}

void CHARACTER::SetBlockMode(BYTE bFlag)
{
	m_pointsInstant.bBlockMode = bFlag;

	ChatPacket(CHAT_TYPE_COMMAND, "setblockmode %d", m_pointsInstant.bBlockMode);

	SetQuestFlag("game_option.block_exchange", bFlag & BLOCK_EXCHANGE ? 1 : 0);
	SetQuestFlag("game_option.block_party_invite", bFlag & BLOCK_PARTY_INVITE ? 1 : 0);
	SetQuestFlag("game_option.block_guild_invite", bFlag & BLOCK_GUILD_INVITE ? 1 : 0);
	SetQuestFlag("game_option.block_whisper", bFlag & BLOCK_WHISPER ? 1 : 0);
	SetQuestFlag("game_option.block_messenger_invite", bFlag & BLOCK_MESSENGER_INVITE ? 1 : 0);
	SetQuestFlag("game_option.block_party_request", bFlag & BLOCK_PARTY_REQUEST ? 1 : 0);
}

void CHARACTER::SetBlockModeForce(BYTE bFlag)
{
	m_pointsInstant.bBlockMode = bFlag;
	ChatPacket(CHAT_TYPE_COMMAND, "setblockmode %d", m_pointsInstant.bBlockMode);
}

bool CHARACTER::IsGuardNPC() const
{
	return IsNPC() && (GetRaceNum() == 11000 || GetRaceNum() == 11002 || GetRaceNum() == 11004);
}

int CHARACTER::GetPolymorphPower() const
{
	if (test_server)
	{
		int value = quest::CQuestManager::instance().GetEventFlag("poly");
		if (value)
			return value;
	}
	return aiPolymorphPowerByLevel[MINMAX(0, GetSkillLevel(SKILL_POLYMORPH), 40)];
}

void CHARACTER::SetPolymorph(DWORD dwRaceNum, bool bMaintainStat)
{
	if (dwRaceNum < MAIN_RACE_MAX_NUM)
	{
		dwRaceNum = 0;
		bMaintainStat = false;
	}

	if (m_dwPolymorphRace == dwRaceNum)
		return;

	m_bPolyMaintainStat = bMaintainStat;
	m_dwPolymorphRace = dwRaceNum;

	sys_log(0, "POLYMORPH: %s race %u ", GetName(), dwRaceNum);

	if (dwRaceNum != 0)
		StopRiding();

	SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);
	m_afAffectFlag.Set(AFF_SPAWN);

	ViewReencode();

	REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);

	if (!bMaintainStat)
	{
		PointChange(POINT_ST, 0);
		PointChange(POINT_DX, 0);
		PointChange(POINT_IQ, 0);
		PointChange(POINT_HT, 0);
	}

	// 폴리모프 상태에서 죽는 경우, 폴리모프가 풀리게 되는데
	// 폴리 모프 전후로 valid combo interval이 다르기 때문에
	// Combo 핵 또는 Hacker로 인식하는 경우가 있다.
	// 따라서 폴리모프를 풀거나 폴리모프 하게 되면,
	// valid combo interval을 reset한다.
	SetValidComboInterval(0);
	SetComboSequence(0);

	ComputeBattlePoints();
}

int CHARACTER::GetQuestFlag(const std::string& flag) const
{
	if (!IsPC())
	{
		sys_err("Trying to get qf %s from non player character", flag.c_str());
		return 0;
	}

	DWORD pid = GetPlayerID();

	quest::CQuestManager& q = quest::CQuestManager::instance();
	quest::PC* pPC = q.GetPC(pid);

	if (!pPC)
	{
		sys_err("Nullpointer when trying to access questflag %s for player with pid %lu", flag.c_str(), pid);
		return 0;
	}

	return pPC->GetFlag(flag);
}

void CHARACTER::SetQuestFlag(const std::string& flag, int value)
{
	DWORD pid = GetPlayerID();

	quest::CQuestManager& q = quest::CQuestManager::instance();
	quest::PC* pPC = q.GetPC(pid);

	if (!pPC)
	{
		sys_err("Nullpointer when trying to set questflag %s for player with pid %lu", flag.c_str(), pid);
		return;
	}

	pPC->SetFlag(flag, value);
}

void CHARACTER::DetermineDropMetinStone()
{
#if defined(__MAGIC_REDUCTION__)
	const int METIN_STONE_NUM = 17;
#else
	const int METIN_STONE_NUM = 15;
#endif

	static DWORD c_adwMetin[METIN_STONE_NUM] =
	{
		28030, // 60
		28031,
		28032,
		28033,
		28034,
		28035,
		28036,
		28037,
		28038,
		28039,
		28040,
		28041,
		28042,
		28043,
#if defined(__MAGIC_REDUCTION__)
		28044,
		28045,
#endif
		28012,
	};

	DWORD stone_num = GetRaceNum();
	int idx = std::lower_bound(aStoneDrop, aStoneDrop + STONE_INFO_MAX_NUM, stone_num) - aStoneDrop;
	if (idx >= STONE_INFO_MAX_NUM || aStoneDrop[idx].dwMobVnum != stone_num)
	{
		m_dwDropMetinStone = 0;
	}
	else
	{
		const SStoneDropInfo& info = aStoneDrop[idx];
		m_bDropMetinStonePct = info.iDropPct;
		{
			m_dwDropMetinStone = c_adwMetin[number(0, METIN_STONE_NUM - 1)];
			int iGradePct = number(1, 100);

			for (int iStoneLevel = 0; iStoneLevel < STONE_LEVEL_MAX_NUM; iStoneLevel++)
			{
				int iLevelGradePortion = info.iLevelPct[iStoneLevel];

				if (iGradePct <= iLevelGradePortion)
				{
					break;
				}
				else
				{
					iGradePct -= iLevelGradePortion;
					m_dwDropMetinStone += 100; // 돌 +a -> +(a+1)이 될때마다 100씩 증가
				}
			}
		}
	}
}

void CHARACTER::SendEquipment(LPCHARACTER pChar)
{
	TPacketGCViewEquip Packet;
	Packet.bHeader = HEADER_GC_VIEW_EQUIP;
	Packet.dwVID = GetVID();
	for (BYTE bSlotIdx = 0; bSlotIdx < WEAR_MAX_NUM; bSlotIdx++)
	{
		LPITEM pItem = GetWear(bSlotIdx);
		if (pItem)
		{
			Packet.Equips[bSlotIdx].dwVnum = pItem->GetVnum();
			Packet.Equips[bSlotIdx].bCount = static_cast<BYTE>(pItem->GetCount());
			thecore_memcpy(Packet.Equips[bSlotIdx].alSockets, pItem->GetSockets(), sizeof(Packet.Equips[bSlotIdx].alSockets));
			thecore_memcpy(Packet.Equips[bSlotIdx].aAttr, pItem->GetAttributes(), sizeof(Packet.Equips[bSlotIdx].aAttr));
#if defined(__CHANGE_LOOK_SYSTEM__)
			Packet.Equips[bSlotIdx].dwTransmutationVnum = pItem->GetTransmutationVnum();
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
			thecore_memcpy(&Packet.Equips[bSlotIdx].RefineElement, pItem->GetRefineElement(), sizeof(Packet.Equips[bSlotIdx].RefineElement));
#endif
#if defined(__ITEM_APPLY_RANDOM__)
			thecore_memcpy(Packet.Equips[bSlotIdx].aApplyRandom, pItem->GetRandomApplies(), sizeof(Packet.Equips[bSlotIdx].aApplyRandom));
#endif
#if defined(__SET_ITEM__)
			Packet.Equips[bSlotIdx].bSetValue = pItem->GetItemSetValue();
#endif
		}
		else
		{
			Packet.Equips[bSlotIdx].dwVnum = 0;
			Packet.Equips[bSlotIdx].bCount = 0;
			memset(&Packet.Equips[bSlotIdx].alSockets, 0, sizeof(Packet.Equips[bSlotIdx].alSockets));
			memset(&Packet.Equips[bSlotIdx].aAttr, 0, sizeof(Packet.Equips[bSlotIdx].aAttr));
#if defined(__CHANGE_LOOK_SYSTEM__)
			Packet.Equips[bSlotIdx].dwTransmutationVnum = 0;
#endif
#if defined(__REFINE_ELEMENT_SYSTEM__)
			memset(&Packet.Equips[bSlotIdx].RefineElement, 0, sizeof(Packet.Equips[bSlotIdx].RefineElement));
#endif
#if defined(__ITEM_APPLY_RANDOM__)
			memset(&Packet.Equips[bSlotIdx].aApplyRandom, 0, sizeof(Packet.Equips[bSlotIdx].aApplyRandom));
#endif
#if defined(__SET_ITEM__)
			Packet.Equips[bSlotIdx].bSetValue = 0;
#endif
		}
	}
	pChar->GetDesc()->Packet(&Packet, sizeof(Packet));
}

bool CHARACTER::CanSummon(int iLeaderShip)
{
	return (iLeaderShip >= 30 && m_dwLastDeadTime + 300 > get_dword_time());
}

void CHARACTER::UnMount(bool bUnequipItem)
{
	if (bUnequipItem)
		UnEquipSpecialRideUniqueItem();

	RemoveAffect(AFFECT_MOUNT);
	RemoveAffect(AFFECT_MOUNT_BONUS);

	if (IsHorseRiding())
	{
		StopRiding();
	}
	else if (GetMountVnum())
	{
		MountVnum(0);
	}
}

void CHARACTER::MountVnum(DWORD vnum)
{
	if (IsPolymorphed())
		return;

	if (m_dwMountVnum == vnum)
		return;

	if ((m_dwMountVnum != 0) && (vnum != 0))
		MountVnum(0);

	m_dwMountVnum = vnum;
	m_dwMountTime = get_dword_time();

	if (m_bIsObserver)
		return;

	m_posDest.x = m_posStart.x = GetX();
	m_posDest.y = m_posStart.y = GetY();

	// NOTE : Mount한다고 해서 Client Side의 객체를 삭제하진 않는다.
	// 그리고 서버Side에서 탔을때 위치 이동은 하지 않는다. 왜냐하면 Client Side에서 Coliision Adjust를 할수 있는데
	// 객체를 소멸시켰다가 서버위치로 이동시키면 이때 collision check를 하지는 않으므로 배경에 끼거나 뚫고 나가는 문제가 존재한다.
	m_posDest.x = m_posStart.x = GetX();
	m_posDest.y = m_posStart.y = GetY();
#if defined(__MOUNT_ENTITY_REFRESH__)
	EncodeRemovePacket(this);
#endif
	EncodeInsertPacket(this);

	ENTITY_MAP::iterator it = m_map_view.begin();

	while (it != m_map_view.end())
	{
		LPENTITY entity = (it++)->first;

		// Mount한다고 해서 Client Side의 객체를 삭제하진 않는다.
#if defined(__MOUNT_ENTITY_REFRESH__)
		EncodeRemovePacket(entity);
		if (!m_bIsObserver)
			EncodeInsertPacket(entity);
#else
		EncodeInsertPacket(entity);
#endif

#if defined(__MOUNT_ENTITY_REFRESH__)
		if (!entity->IsObserverMode())
			entity->EncodeInsertPacket(this);
#endif
	}

	SetValidComboInterval(0);
	SetComboSequence(0);

	ComputePoints();
}

namespace
{
	class FuncCheckWarp
	{
	public:
		FuncCheckWarp(LPCHARACTER pkWarp)
		{
			m_lTargetY = 0;
			m_lTargetX = 0;

			m_lX = pkWarp->GetX();
			m_lY = pkWarp->GetY();

			m_bInvalid = false;
			m_bEmpire = pkWarp->GetEmpire();

			char szTmp[64];

			if (3 != sscanf(pkWarp->GetName(), " %s %ld %ld ", szTmp, &m_lTargetX, &m_lTargetY))
			{
				if (number(1, 100) < 5)
					sys_err("Warp NPC name wrong : vnum(%d) name(%s)", pkWarp->GetRaceNum(), pkWarp->GetName());

				m_bInvalid = true;

				return;
			}

			m_lTargetX *= 100;
			m_lTargetY *= 100;

			m_bUseWarp = true;

			if (pkWarp->IsGoto())
			{
				LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(pkWarp->GetMapIndex());
				m_lTargetX += pkSectreeMap->m_setting.iBaseX;
				m_lTargetY += pkSectreeMap->m_setting.iBaseY;
				m_bUseWarp = false;
			}
		}

		bool Valid()
		{
			return !m_bInvalid;
		}

		void operator () (LPENTITY ent)
		{
			if (!Valid())
				return;

			if (!ent->IsType(ENTITY_CHARACTER))
				return;

			LPCHARACTER pkChr = (LPCHARACTER)ent;

			if (!pkChr->IsPC())
				return;

			int iDist = DISTANCE_APPROX(pkChr->GetX() - m_lX, pkChr->GetY() - m_lY);

			if (iDist > 300)
				return;

			if (m_bEmpire && pkChr->GetEmpire() && m_bEmpire != pkChr->GetEmpire())
				return;

			if (pkChr->IsHack())
				return;

			if (!pkChr->CanHandleItem(false, true))
				return;

			if (m_bUseWarp)
				pkChr->WarpSet(m_lTargetX, m_lTargetY);
			else
			{
				pkChr->Show(pkChr->GetMapIndex(), m_lTargetX, m_lTargetY);
				pkChr->Stop();
			}
		}

		bool m_bInvalid;
		bool m_bUseWarp;

		long m_lX;
		long m_lY;
		long m_lTargetX;
		long m_lTargetY;

		BYTE m_bEmpire;
	};
}

EVENTFUNC(warp_npc_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>(event->info);
	if (info == NULL)
	{
		sys_err("warp_npc_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER ch = info->ch;

	if (ch == NULL) // <Factor>
		return 0;

	if (!ch->GetSectree())
	{
		ch->m_pkWarpNPCEvent = NULL;
		return 0;
	}

	FuncCheckWarp f(ch);
	if (f.Valid())
		ch->GetSectree()->ForEachAround(f);

	return passes_per_sec / 2;
}

void CHARACTER::StartWarpNPCEvent()
{
	if (m_pkWarpNPCEvent)
		return;

	if (!IsWarp() && !IsGoto())
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	m_pkWarpNPCEvent = event_create(warp_npc_event, info, passes_per_sec / 2);
}

void CHARACTER::SyncPacket()
{
	TEMP_BUFFER buf;

	TPacketCGSyncPositionElement elem;

	elem.dwVID = GetVID();
	elem.lX = GetX();
	elem.lY = GetY();

	TPacketGCSyncPosition pack;

	pack.bHeader = HEADER_GC_SYNC_POSITION;
	pack.wSize = sizeof(TPacketGCSyncPosition) + sizeof(elem);

	buf.write(&pack, sizeof(pack));
	buf.write(&elem, sizeof(elem));

	PacketAround(buf.read_peek(), buf.size());
}

LPCHARACTER CHARACTER::GetMarryPartner() const
{
	return m_pkChrMarried;
}

void CHARACTER::SetMarryPartner(LPCHARACTER ch)
{
	m_pkChrMarried = ch;
}

int CHARACTER::GetMarriageBonus(DWORD dwItemVnum, bool bSum)
{
	if (IsNPC())
		return 0;

	marriage::TMarriage* pMarriage = marriage::CManager::instance().Get(GetPlayerID());

	if (!pMarriage)
		return 0;

	return pMarriage->GetBonus(dwItemVnum, bSum, this);
}

void CHARACTER::ConfirmWithMsg(const char* szMsg, int iTimeout, DWORD dwRequestPID)
{
	if (!IsPC())
		return;

	TPacketGCQuestConfirm p;

	p.header = HEADER_GC_QUEST_CONFIRM;
	p.requestPID = dwRequestPID;
	p.timeout = iTimeout;
	strlcpy(p.msg, szMsg, sizeof(p.msg));

	GetDesc()->Packet(&p, sizeof(p));
}

bool CHARACTER::IsRunningQuest() const
{
	return quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning();
}

int CHARACTER::GetPremiumRemainSeconds(BYTE bType) const
{
	if (bType >= PREMIUM_MAX_NUM)
		return 0;

	return m_aiPremiumTimes[bType] - get_global_time();
}

bool CHARACTER::WarpToPID(DWORD dwPID, bool bWarpForce)
{
	LPCHARACTER victim;
	if ((victim = (CHARACTER_MANAGER::instance().FindByPID(dwPID))))
	{
		int mapIdx = victim->GetMapIndex();
		if (IS_SUMMONABLE_ZONE(mapIdx))
		{
			if (CAN_ENTER_ZONE(this, mapIdx))
			{
				WarpSet(victim->GetX(), victim->GetY());
			}
			else
			{
				ChatPacket(CHAT_TYPE_INFO, LC_STRING("상대방이 있는 곳으로 워프할 수 없습니다."));
				return false;
			}
		}
		else
		{
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("상대방이 있는 곳으로 워프할 수 없습니다."));
			return false;
		}
	}
	else
	{
		// 다른 서버에 로그인된 사람이 있음 -> 메시지 보내 좌표를 받아오자
		// 1. A.pid, B.pid 를 뿌림
		// 2. B.pid를 가진 서버가 뿌린서버에게 A.pid, 좌표 를 보냄
		// 3. 워프
		CCI* pcci = P2P_MANAGER::instance().FindByPID(dwPID);

		if (!pcci)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("상대방이 온라인 상태가 아닙니다."));
			return false;
		}

		if ((pcci->bChannel != g_bChannel) && !bWarpForce)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("상대방이 %d 채널에 있습니다. (현재 채널 %d)", pcci->bChannel, g_bChannel));
			return false;
		}
		else if (false == IS_SUMMONABLE_ZONE(pcci->lMapIndex))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("상대방이 있는 곳으로 워프할 수 없습니다."));
			return false;
		}
		else
		{
			if (!CAN_ENTER_ZONE(this, pcci->lMapIndex))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_STRING("상대방이 있는 곳으로 워프할 수 없습니다."));
				return false;
			}

			TPacketGGFindPosition p;
			p.header = HEADER_GG_FIND_POSITION;
			p.dwFromPID = GetPlayerID();
			p.dwTargetPID = dwPID;
			pcci->pkDesc->Packet(&p, sizeof(TPacketGGFindPosition));

			if (test_server)
				ChatPacket(CHAT_TYPE_PARTY, "sent find position packet for teleport");
		}
	}
	return true;
}

// ADD_REFINE_BUILDING
CGuild* CHARACTER::GetRefineGuild() const
{
	LPCHARACTER chRefineNPC = CHARACTER_MANAGER::instance().Find(m_dwRefineNPCVID);

	return (chRefineNPC ? chRefineNPC->GetGuild() : NULL);
}

bool CHARACTER::IsRefineThroughGuild() const
{
	return GetRefineGuild() != NULL;
}

int CHARACTER::ComputeRefineFee(int iCost, int iMultiply) const
{
	CGuild* pGuild = GetRefineGuild();
	if (pGuild)
	{
		if (pGuild == GetGuild())
			return iCost * iMultiply * 9 / 10;

		// 다른 제국 사람이 시도하는 경우 추가로 3배 더
		LPCHARACTER chRefineNPC = CHARACTER_MANAGER::instance().Find(m_dwRefineNPCVID);
		if (chRefineNPC && chRefineNPC->GetEmpire() != GetEmpire())
			return iCost * iMultiply * 3;

		return iCost * iMultiply;
	}
	else
		return iCost;
}

void CHARACTER::PayRefineFee(int iTotalMoney)
{
	int iFee = iTotalMoney / 10;
	CGuild* pGuild = GetRefineGuild();

	int iRemain = iTotalMoney;

	if (pGuild)
	{
		// 자기 길드이면 iTotalMoney에 이미 10%가 제외되어있다
		if (pGuild != GetGuild())
		{
			pGuild->RequestDepositMoney(this, iFee);
			iRemain -= iFee;
		}
	}

	PointChange(POINT_GOLD, -iRemain);
}
// END_OF_ADD_REFINE_BUILDING

// PREVENT_TRADE_WINDOW
bool CHARACTER::PreventTradeWindow(int flags, bool except) const
{
	if (WND_ALL & flags)
	{
		flags = 0x0;
		except = true;
	}

	if (except && !(WND_EXCHANGE & flags) || !except && (WND_EXCHANGE & flags))
		if (GetExchange())
			return true;

	if (except && !(WND_MYSHOP & flags) || !except && (WND_MYSHOP & flags))
		if (GetMyShop())
			return true;

	if (except && !(WND_SHOPOWNER & flags) || !except && (WND_SHOPOWNER & flags))
		if (GetShopOwner())
			return true;

	if (except && !(WND_SAFEBOX & flags) || !except && (WND_SAFEBOX & flags))
		if (IsOpenSafebox())
			return true;

	if (except && !(WND_REFINE & flags) || !except && (WND_REFINE & flags))
		if (IsUnderRefine()
#if defined(__REFINE_ELEMENT_SYSTEM__)
			|| IsUnderRefineElement()
#endif
			)
			return true;

	if (except && !(WND_CUBE & flags) || !except && (WND_CUBE & flags))
		if (IsCubeOpen())
			return true;

#if defined(__MOVE_COSTUME_ATTR__)
	if (except && !(WND_ITEM_COMB & flags) || !except && (WND_ITEM_COMB & flags))
		if (IsItemComb())
			return true;
#endif

#if defined(__CHANGED_ATTR__)
	if (except && !(WND_SELECT_ATTR & flags) || !except && (WND_SELECT_ATTR & flags))
		if (IsSelectAttr())
			return true;
#endif

#if defined(__ACCE_COSTUME_SYSTEM__)
	if (except && !(WND_ACCE & flags) || !except && (WND_ACCE & flags))
		if (IsAcceRefineWindowOpen())
			return true;
#endif

#if defined(__CHANGE_LOOK_SYSTEM__)
	if (except && !(WND_CHANGELOOK & flags) || !except && (WND_CHANGELOOK & flags))
		if (GetChangeLook())
			return true;
#endif

#if defined(__PRIVATESHOP_SEARCH_SYSTEM__)
	if (except && !(WND_SHOPSEARCH & flags) || !except && (WND_SHOPSEARCH & flags))
		if (GetPrivateShopSearchState() != SHOP_SEARCH_OFF)
			return true;
#endif

#if defined(__MAILBOX__)
	if (except && !(WND_MAILBOX & flags) || !except && (WND_MAILBOX & flags))
		if (GetMailBox())
			return true;
#endif

#if defined(__ATTR_6TH_7TH__)
	if (except && !(WND_ATTR67ADD & flags) || !except && (WND_ATTR67ADD & flags))
		if (IsOpenAttr67Add())
			return true;
#endif

#if defined(__LUCKY_BOX__)
	if (except && !(WND_LUCKY_BOX & flags) || !except && (WND_LUCKY_BOX & flags))
		if (IsLuckyBoxOpen())
			return true;
#endif

#if defined(__AURA_COSTUME_SYSTEM__)
	if (except && !(WND_AURA & flags) || !except && (WND_AURA & flags))
		if (IsAuraRefineWindowOpen())
			return true;
#endif

#if defined(__SUMMER_EVENT_ROULETTE__)
	if (except && !(WND_MINIGAME_ROULETTE & flags) || !except && (WND_MINIGAME_ROULETTE & flags))
		if (GetMiniGameRoulette())
			return true;
#endif

	return false;
}
// END_PREVENT_TRADE_WINDOW

// Hack 방지를 위한 체크.
bool CHARACTER::IsHack(bool bSendMsg, bool bCheckShopOwner, int limittime)
{
	const int iPulse = thecore_pulse();

	if (test_server)
		bSendMsg = true;

	// 창고 연후 체크
	if (iPulse - GetSafeboxLoadTime() < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("창고를 연후 %d초 이내에는 다른곳으로 이동할수 없습니다.", limittime));

		if (test_server)
			ChatPacket(CHAT_TYPE_INFO, "[TestOnly]Pulse %d LoadTime %d PASS %d", iPulse, GetSafeboxLoadTime(), PASSES_PER_SEC(limittime));
		return true;
	}

	// 거래관련 창 체크
	if (bCheckShopOwner)
	{
		if (PreventTradeWindow(WND_ALL))
		{
			if (bSendMsg)
				ChatPacket(CHAT_TYPE_INFO, LC_STRING("거래창,창고 등을 연 상태에서는 다른곳으로 이동,종료 할수 없습니다"));

			return true;
		}
	}
	else
	{
		if (PreventTradeWindow(WND_SHOPOWNER, true/*except*/))
		{
			if (bSendMsg)
				ChatPacket(CHAT_TYPE_INFO, LC_STRING("거래창,창고 등을 연 상태에서는 다른곳으로 이동,종료 할수 없습니다"));

			return true;
		}
	}

	// PREVENT_PORTAL_AFTER_EXCHANGE
	// 교환 후 시간체크
	if (iPulse - GetExchangeTime() < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("거래 후 %d초 이내에는 다른지역으로 이동 할 수 없습니다.", limittime));
		return true;
	}
	// END_PREVENT_PORTAL_AFTER_EXCHANGE

	// PREVENT_ITEM_COPY
	if (iPulse - GetMyShopTime() < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("거래 후 %d초 이내에는 다른지역으로 이동 할 수 없습니다.", limittime));
		return true;
	}

	if (iPulse - GetRefineTime() < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("아이템 개량후 %d초 이내에는 귀환부,귀환기억부를 사용할 수 없습니다.", limittime));
		return true;
	}

#if defined(__MAILBOX__)
	if (iPulse - GetMyMailBoxTime() < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("You have to wait %d sec. after opening the Mailbox before you can switch channels or teleport.", limittime));
		return true;
	}
#endif
	// END_PREVENT_ITEM_COPY

	return false;
}

BOOL CHARACTER::IsMonarch() const
{
	// MONARCH_LIMIT
	if (CMonarch::instance().IsMonarch(GetPlayerID(), GetEmpire()))
		return true;

	return false;

	// END_MONARCH_LIMIT
}

void CHARACTER::Say(const std::string& s)
{
	struct ::packet_script packet_script;

	packet_script.header = HEADER_GC_SCRIPT;
	packet_script.skin = 1;
	packet_script.src_size = s.size();
	packet_script.size = packet_script.src_size + sizeof(struct packet_script);

	TEMP_BUFFER buf;

	buf.write(&packet_script, sizeof(struct packet_script));
	buf.write(&s[0], s.size());

	if (IsPC())
	{
		GetDesc()->Packet(buf.read_peek(), buf.size());
	}
}

//
// Monarch
//
void CHARACTER::InitMC()
{
	for (int n = 0; n < MI_MAX; ++n)
	{
		m_dwMonarchCooltime[n] = thecore_pulse();
	}

	m_dwMonarchCooltimelimit[MI_HEAL] = PASSES_PER_SEC(MC_HEAL);
	m_dwMonarchCooltimelimit[MI_WARP] = PASSES_PER_SEC(MC_WARP);
	m_dwMonarchCooltimelimit[MI_TRANSFER] = PASSES_PER_SEC(MC_TRANSFER);
	m_dwMonarchCooltimelimit[MI_TAX] = PASSES_PER_SEC(MC_TAX);
	m_dwMonarchCooltimelimit[MI_SUMMON] = PASSES_PER_SEC(MC_SUMMON);

	m_dwMonarchCooltime[MI_HEAL] -= PASSES_PER_SEC(GetMCL(MI_HEAL));
	m_dwMonarchCooltime[MI_WARP] -= PASSES_PER_SEC(GetMCL(MI_WARP));
	m_dwMonarchCooltime[MI_TRANSFER] -= PASSES_PER_SEC(GetMCL(MI_TRANSFER));
	m_dwMonarchCooltime[MI_TAX] -= PASSES_PER_SEC(GetMCL(MI_TAX));
	m_dwMonarchCooltime[MI_SUMMON] -= PASSES_PER_SEC(GetMCL(MI_SUMMON));
}

DWORD CHARACTER::GetMC(enum MONARCH_INDEX e) const
{
	return m_dwMonarchCooltime[e];
}

void CHARACTER::SetMC(enum MONARCH_INDEX e)
{
	m_dwMonarchCooltime[e] = thecore_pulse();
}

bool CHARACTER::IsMCOK(enum MONARCH_INDEX e) const
{
	int iPulse = thecore_pulse();

	if ((iPulse - GetMC(e)) < GetMCL(e))
	{
		if (test_server)
			sys_log(0, " Pulse %d cooltime %d, limit %d", iPulse, GetMC(e), GetMCL(e));

		return false;
	}

	if (test_server)
		sys_log(0, " Pulse %d cooltime %d, limit %d", iPulse, GetMC(e), GetMCL(e));

	return true;
}

DWORD CHARACTER::GetMCL(enum MONARCH_INDEX e) const
{
	return m_dwMonarchCooltimelimit[e];
}

DWORD CHARACTER::GetMCLTime(enum MONARCH_INDEX e) const
{
	int iPulse = thecore_pulse();

	if (test_server)
		sys_log(0, " Pulse %d cooltime %d, limit %d", iPulse, GetMC(e), GetMCL(e));

	return (GetMCL(e)) / passes_per_sec - (iPulse - GetMC(e)) / passes_per_sec;
}

bool CHARACTER::IsSiegeNPC() const
{
	return IsNPC() && (GetRaceNum() == 11000 || GetRaceNum() == 11002 || GetRaceNum() == 11004);
}

//------------------------------------------------
void CHARACTER::UpdateDepositPulse()
{
	m_deposit_pulse = thecore_pulse() + PASSES_PER_SEC(60 * 5); // 5분
}

bool CHARACTER::CanDeposit() const
{
	return (m_deposit_pulse == 0 || (m_deposit_pulse < thecore_pulse()));
}
//------------------------------------------------

ESex GET_SEX(LPCHARACTER ch)
{
	switch (ch->GetRaceNum())
	{
		case MAIN_RACE_WARRIOR_M:
		case MAIN_RACE_SURA_M:
		case MAIN_RACE_ASSASSIN_M:
		case MAIN_RACE_SHAMAN_M:
		case MAIN_RACE_WOLFMAN_M:
			return SEX_MALE;

		case MAIN_RACE_ASSASSIN_W:
		case MAIN_RACE_SHAMAN_W:
		case MAIN_RACE_WARRIOR_W:
		case MAIN_RACE_SURA_W:
			return SEX_FEMALE;
	}

	/* default sex = male */
	return SEX_MALE;
}

int CHARACTER::GetHPPct() const
{
	if (GetMaxHP() <= 0)
		return 0;

	return (GetHP() * 100) / GetMaxHP();
}

bool CHARACTER::IsBerserk() const
{
	if (m_pkMobInst != NULL)
		return m_pkMobInst->m_IsBerserk;
	else
		return false;
}

void CHARACTER::SetBerserk(bool mode)
{
	if (m_pkMobInst != NULL)
		m_pkMobInst->m_IsBerserk = mode;
}

bool CHARACTER::IsGodSpeed() const
{
	if (m_pkMobInst != NULL)
	{
		return m_pkMobInst->m_IsGodSpeed;
	}
	else
	{
		return false;
	}
}

void CHARACTER::SetGodSpeed(bool mode)
{
	if (m_pkMobInst != NULL)
	{
		m_pkMobInst->m_IsGodSpeed = mode;

		if (mode == true)
		{
			SetPoint(POINT_ATT_SPEED, 250);
		}
		else
		{
			SetPoint(POINT_ATT_SPEED, m_pkMobData->m_table.sAttackSpeed);
		}
	}
}

bool CHARACTER::IsDeathBlow() const
{
	if (number(1, 100) <= m_pkMobData->m_table.bDeathBlowPoint)
	{
		return true;
	}
	else
	{
		return false;
	}
}

struct FFindReviver
{
	FFindReviver()
	{
		pChar = NULL;
		HasReviver = false;
	}

	void operator() (LPCHARACTER ch)
	{
		if (ch->IsMonster() != true)
		{
			return;
		}

		if (ch->IsReviver() == true && pChar != ch && ch->IsDead() != true)
		{
			if (number(1, 100) <= ch->GetMobTable().bRevivePoint)
			{
				HasReviver = true;
				pChar = ch;
			}
		}
	}

	LPCHARACTER pChar;
	bool HasReviver;
};

bool CHARACTER::HasReviverInParty() const
{
	LPPARTY party = GetParty();

	if (party != NULL)
	{
		if (party->GetMemberCount() == 1) return false;

		FFindReviver f;
		party->ForEachMemberPtr(f);
		return f.HasReviver;
	}

	return false;
}

bool CHARACTER::IsRevive() const
{
	if (m_pkMobInst != NULL)
	{
		return m_pkMobInst->m_IsRevive;
	}

	return false;
}

void CHARACTER::SetRevive(bool mode)
{
	if (m_pkMobInst != NULL)
	{
		m_pkMobInst->m_IsRevive = mode;
	}
}

#define IS_SPEED_HACK_PLAYER(ch) (ch->m_speed_hack_count > SPEEDHACK_LIMIT_COUNT)

EVENTFUNC(check_speedhack_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>(event->info);
	if (info == NULL)
	{
		sys_err("check_speedhack_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER ch = info->ch;

	if (NULL == ch || ch->IsNPC())
		return 0;

	if (IS_SPEED_HACK_PLAYER(ch))
	{
		// Write hack log
		LogManager::instance().SpeedHackLog(ch->GetPlayerID(), ch->GetX(), ch->GetY(), ch->m_speed_hack_count);

		//if (false == LC_IsEurope())
		{
			// Close connection
			LPDESC desc = ch->GetDesc();

			if (desc)
			{
				DESC_MANAGER::instance().DestroyDesc(desc);
				return 0;
			}
		}
	}

	ch->m_speed_hack_count = 0;

	ch->ResetComboHackCount();
	return PASSES_PER_SEC(60);
}

void CHARACTER::StartCheckSpeedHackEvent()
{
	if (m_pkCheckSpeedHackEvent)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	m_pkCheckSpeedHackEvent = event_create(check_speedhack_event, info, PASSES_PER_SEC(60)); // 1분
}

void CHARACTER::GoHome()
{
	WarpSet(EMPIRE_START_X(GetEmpire()), EMPIRE_START_Y(GetEmpire()));
}

void CHARACTER::SendGuildName(CGuild* pGuild)
{
	if (NULL == pGuild) return;

	DESC* desc = GetDesc();

	if (NULL == desc) return;
	if (m_known_guild.find(pGuild->GetID()) != m_known_guild.end()) return;

	m_known_guild.insert(pGuild->GetID());

	TPacketGCGuildName pack;
	memset(&pack, 0x00, sizeof(pack));

	pack.header = HEADER_GC_GUILD;
	pack.subheader = GUILD_SUBHEADER_GC_GUILD_NAME;
	pack.size = sizeof(TPacketGCGuildName);
	pack.guildID = pGuild->GetID();
	memcpy(pack.guildName, pGuild->GetName(), GUILD_NAME_MAX_LEN);

	desc->Packet(&pack, sizeof(pack));
}

void CHARACTER::SendGuildName(DWORD dwGuildID)
{
	SendGuildName(CGuildManager::instance().FindGuild(dwGuildID));
}

EVENTFUNC(destroy_when_idle_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>(event->info);
	if (info == NULL)
	{
		sys_err("destroy_when_idle_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER ch = info->ch;

	if (ch == NULL) // <Factor>
		return 0;

	if (ch->GetVictim())
		return PASSES_PER_SEC(300);

	sys_log(1, "DESTROY_WHEN_IDLE: %s", ch->GetName());

	ch->m_pkDestroyWhenIdleEvent = NULL;
	M2_DESTROY_CHARACTER(ch);
	return 0;
}

void CHARACTER::StartDestroyWhenIdleEvent()
{
	if (m_pkDestroyWhenIdleEvent)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	m_pkDestroyWhenIdleEvent = event_create(destroy_when_idle_event, info, PASSES_PER_SEC(300));
}

void CHARACTER::SetComboSequence(BYTE seq)
{
	m_bComboSequence = seq;
}

BYTE CHARACTER::GetComboSequence() const
{
	return m_bComboSequence;
}

void CHARACTER::SetLastComboTime(DWORD time)
{
	m_dwLastComboTime = time;
}

DWORD CHARACTER::GetLastComboTime() const
{
	return m_dwLastComboTime;
}

void CHARACTER::SetValidComboInterval(int interval)
{
	m_iValidComboInterval = interval;
}

int CHARACTER::GetValidComboInterval() const
{
	return m_iValidComboInterval;
}

BYTE CHARACTER::GetComboIndex() const
{
	return m_bComboIndex;
}

void CHARACTER::IncreaseComboHackCount(int k)
{
	m_iComboHackCount += k;

	if (m_iComboHackCount >= 10)
	{
		if (GetDesc())
			if (GetDesc()->DelayedDisconnect(number(2, 7)))
			{
				sys_log(0, "COMBO_HACK_DISCONNECT: %s count: %d", GetName(), m_iComboHackCount);
				LogManager::instance().HackLog("Combo", this);
			}
	}
}

void CHARACTER::ResetComboHackCount()
{
	m_iComboHackCount = 0;
}

void CHARACTER::SkipComboAttackByTime(int interval)
{
	m_dwSkipComboAttackByTime = get_dword_time() + interval;
}

DWORD CHARACTER::GetSkipComboAttackByTime() const
{
	return m_dwSkipComboAttackByTime;
}

// 말이나 다른것을 타고 있나?
bool CHARACTER::IsRiding() const
{
	return IsHorseRiding() || GetMountVnum();
}

bool CHARACTER::CanWarp() const
{
	const int iPulse = thecore_pulse();
	const int limit_time = PASSES_PER_SEC(g_nPortalLimitTime);

	if ((iPulse - GetSafeboxLoadTime()) < limit_time)
		return false;

	if ((iPulse - GetExchangeTime()) < limit_time)
		return false;

	if ((iPulse - GetMyShopTime()) < limit_time)
		return false;

	if ((iPulse - GetRefineTime()) < limit_time)
		return false;

#if defined(__MAILBOX__)
	if ((iPulse - GetMyMailBoxTime()) < limit_time)
		return false;
#endif

	if (PreventTradeWindow(WND_ALL))
		return false;

	return true;
}

DWORD CHARACTER::GetNextExp() const
{
	if (PLAYER_EXP_TABLE_MAX < GetLevel())
		return 2500000000;
	else
		return exp_table[GetLevel()];
}

#if defined(__CONQUEROR_LEVEL__)
DWORD CHARACTER::GetNextConquerorExp() const
{
	if (PLAYER_CONQUEROR_EXP_TABLE_MAX < GetConquerorLevel())
		return 2500000000;
	else
		return conqueror_exp_table[GetConquerorLevel()];
}
#endif

int CHARACTER::GetSkillPowerByLevel(int level, bool bMob) const
{
	return CTableBySkill::instance().GetSkillPowerByLevelFromType(GetJob(), GetSkillGroup(), MINMAX(0, level, SKILL_MAX_LEVEL), bMob);
}

bool CHARACTER::IsInSafezone() const
{
	LPSECTREE sectree = GetSectree();
	return (sectree && sectree->IsAttr(GetX(), GetY(), ATTR_BANPK));
}

bool CHARACTER::IsInBlockedArea(long x, long y) const
{
	LPSECTREE sectree = GetSectree();
	return (sectree && sectree->IsAttr(x > 0 ? x : GetX(), x > 0 ? y : GetY(), ATTR_BLOCK | ATTR_OBJECT));
}

#if defined(__MOVE_CHANNEL__)
EVENTINFO(move_channel_info)
{
	DynamicCharacterPtr ch;
	int iSec;
	long lNewAddr;
	WORD wNewPort;

	move_channel_info()
		: iSec(0),
		ch(),
		lNewAddr(0),
		wNewPort(0)
	{
	}
};

EVENTFUNC(move_channel)
{
	move_channel_info* info = dynamic_cast<move_channel_info*>(event->info);

	if (!info) // no info
		return 0;

	LPCHARACTER ch = info->ch;

	if (!ch) // no char
		return 0;

	if (!ch->GetDesc()) // no desc
		return 0;

	if (info->iSec > 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("Moving channel in %d seconds.", info->iSec));
		--info->iSec;
		return PASSES_PER_SEC(1);
	}

	ch->MoveChannel(info->lNewAddr, info->wNewPort);
	ch->m_pkTimedEvent = NULL;

	return 0;
}

bool CHARACTER::MoveChannel(long lNewAddr, WORD wNewPort)
{
	if (!IsPC())
		return false;

	if (!CanWarp())
		return false;

	if (!GetDesc())
		return false;

	long x = GetX();
	long y = GetY();

	long lAddr = lNewAddr;
	long lMapIndex = GetMapIndex();
	WORD wPort = wNewPort;

	if (lMapIndex >= 10000)
	{
		sys_err("Invalid change channel request from dungeon %d!", lMapIndex);
		return false;
	}

	if (g_bChannel == 99)
	{
		sys_err("%s attempted to change channel from CH99, ignoring request.", GetName());
		return false;
	}

	Stop();
	Save();

	if (GetSectree())
	{
		GetSectree()->RemoveEntity(this);
		ViewCleanup();

		EncodeRemovePacket(this);
	}

	m_lWarpMapIndex = lMapIndex;
	m_posWarp.x = x;
	m_posWarp.y = y;

	sys_log(0, "ChangeChannel %s, %ld %ld map %ld to port %d", GetName(), x, y, GetMapIndex(), wPort);

	TPacketGCWarp p;

	p.bHeader = HEADER_GC_WARP;
	p.lX = x;
	p.lY = y;
#if defined(__PROXY_IP__)
	p.lAddr = g_stProxyIP.empty() ? lAddr : inet_addr(g_stProxyIP.c_str());
#else
	p.lAddr = lAddr;
#endif
	p.wPort = wPort;

	GetDesc()->Packet(&p, sizeof(p));

	char buf[256];
	snprintf(buf, sizeof(buf), "%s Port%d Map%ld x%ld y%ld", GetName(), wPort, GetMapIndex(), x, y);
	LogManager::instance().CharLog(this, 0, "CHANGE_CH", buf);

	return true;
}

bool CHARACTER::StartMoveChannel(long lNewAddr, WORD wNewPort)
{
	if (IsHack(false, true, 10))
		return false;

	move_channel_info* info = AllocEventInfo<move_channel_info>();
	info->ch = this;
	info->iSec = CanWarp() && !IsPosition(POS_FIGHTING) ? 3 : 10;
	info->lNewAddr = lNewAddr;
	info->wNewPort = wNewPort;

	m_pkTimedEvent = event_create(move_channel, info, 1);

	return true;
}
#endif

#if defined(__PRIVATESHOP_SEARCH_SYSTEM__)
void CHARACTER::OpenPrivateShopSearch(DWORD dwVnum)
{
	if (GetDesc() == NULL)
		return;

	if (PreventTradeWindow(WND_SHOPSEARCH, true/*except*/))
		return;

	TPacketGCPrivateShopSearchOpen p;
	p.header = HEADER_GC_PRIVATESHOP_SEARCH_OPEN;
	GetDesc()->Packet(&p, sizeof(p));

	bPrivateShopSearchState = (dwVnum == ITEM_PRIVATESHOP_SEARCH_LOOKING_GLASS) ? SHOP_SEARCH_LOOKING : SHOP_SEARCH_TRADING;
}
#endif

#if defined(__CHANGE_LOOK_SYSTEM__)
void CHARACTER::SetChangeLook(CChangeLook* pChangeLook)
{
	if (m_pkChangeLook != nullptr)
		delete m_pkChangeLook;

	m_pkChangeLook = pChangeLook;
}

CChangeLook* CHARACTER::GetChangeLook() const
{
	return m_pkChangeLook;
}
#endif

#if defined(__MAILBOX__)
void CHARACTER::SetMailBox(CMailBox* m)
{
	if (m_pkMailBox)
		delete m_pkMailBox;

	m_pkMailBox = m;
}
#endif

#if defined(__MINI_GAME_RUMI__)
void CHARACTER::SetMiniGameRumi(CMiniGameRumi* pClass)
{
	if (m_pkMiniGameRumi)
		delete m_pkMiniGameRumi;

	m_pkMiniGameRumi = pClass;
}
#endif

#if defined(__MINI_GAME_YUTNORI__)
void CHARACTER::SetMiniGameYutnori(CMiniGameYutnori* pClass)
{
	if (m_pkMiniGameYutnori)
		delete m_pkMiniGameYutnori;

	m_pkMiniGameYutnori = pClass;
}
#endif

#if defined(__CONQUEROR_LEVEL__)
void CHARACTER::SetConqueror(bool bSet)
{
	if (bSet)
	{
		if (GetConquerorLevel() > 0)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("You have already reached the Champion Level."));
			return;
		}

		DWORD dwNextExp = GetNextExp() / 4;
		if ((GetLevel() < gPlayerMaxLevel) || (GetLevel() >= gPlayerMaxLevel && GetExp() < dwNextExp))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_STRING("You must fill up at least 1 EXP Marble at level 120 to reach Champion Level."));
			return;
		}
	}

	PointChange(POINT_CONQUEROR_LEVEL, MINMAX(0, bSet ? 1 : 0, gPlayerMaxLevel) - GetConquerorLevel(), false, true);

	SetRealPoint(POINT_CONQUEROR_LEVEL_STEP, 0);
	SetPoint(POINT_CONQUEROR_LEVEL_STEP, GetRealPoint(POINT_CONQUEROR_LEVEL_STEP));

	SetRealPoint(POINT_CONQUEROR_POINT, 0);
	SetPoint(POINT_CONQUEROR_POINT, GetRealPoint(POINT_CONQUEROR_POINT));

	SetRealPoint(POINT_SUNGMA_STR, 0);
	SetPoint(POINT_SUNGMA_STR, GetRealPoint(POINT_SUNGMA_STR));

	SetRealPoint(POINT_SUNGMA_HP, 0);
	SetPoint(POINT_SUNGMA_HP, GetRealPoint(POINT_SUNGMA_HP));

	SetRealPoint(POINT_SUNGMA_MOVE, 0);
	SetPoint(POINT_SUNGMA_MOVE, GetRealPoint(POINT_SUNGMA_MOVE));

	SetRealPoint(POINT_SUNGMA_IMMUNE, 0);
	SetPoint(POINT_SUNGMA_IMMUNE, GetRealPoint(POINT_SUNGMA_IMMUNE));

	SetConquerorExp(0);

	ComputePoints();
	PointsPacket();
	UpdatePointsPacket(POINT_CONQUEROR_EXP, GetConquerorExp());
}

bool CHARACTER::IsNewWorldMapIndex() const
{
	return SECTREE_MANAGER::Instance().IsNewWorldMapIndex(GetMapIndex());
}

long CHARACTER::GetNewWorldSungMa(POINT_TYPE wPointType, bool bPremium) const
{
	if (!IsPC())
		return 0;

	if (!GetConquerorLevel())
		return 0;

	return SECTREE_MANAGER::Instance().GetNewWorldSungMa(GetMapIndex(), wPointType);
}

bool CHARACTER::IsSungMaCursed(const POINT_TYPE wPointType) const
{
	if (GetNewWorldSungMa(wPointType) > GetPoint(wPointType))
		return true;

	return false;
}
#endif

#if defined(__LOOT_FILTER_SYSTEM__)
CLootFilter* CHARACTER::GetLootFilter()
{
	return m_pLootFilter;
}

void CHARACTER::SetLootFilter()
{
	if (!GetDesc())
		return;

	if (m_pLootFilter)
		return;

	m_pLootFilter = new CLootFilter();

	TPacketGCLootFilter p;
	p.header = HEADER_GC_LOOT_FILTER;
	p.enable = true;
	p.vid = 0;
	GetDesc()->Packet(&p, sizeof(p));
}

void CHARACTER::ClearLootFilter()
{
	if (m_pLootFilter)
	{
		delete m_pLootFilter;
		m_pLootFilter = nullptr;
	}
}
#endif

#if defined(__GEM_SYSTEM__)
void CHARACTER::SelectItemEx(DWORD dwInventoryPos, BYTE bType)
{
	const LPITEM c_lpMetinStoneItem = GetInventoryItem(dwInventoryPos);
	if (c_lpMetinStoneItem == nullptr)
		return;

	switch (bType)
	{
		default:
			GemRefine(c_lpMetinStoneItem);
			break;
	}

	ChatPacket(CHAT_TYPE_COMMAND, "BINARY_RefreshSelectItemWindowEx");
}

bool CHARACTER::GemRefine(LPITEM lpMetinStoneItem)
{
	const SGemRefineInfo* pGemRefineInfo = &ITEM_MANAGER::instance().GetGemRefineInfo();
	if (pGemRefineInfo == nullptr)
		return false;

	TItemTable* pItemTable = ITEM_MANAGER::instance().GetTable(pGemRefineInfo->dwRefineItemVNum);
	if (!pItemTable || CountSpecifyItem(pItemTable->dwVnum) < pGemRefineInfo->wRefineItemCount)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Gaya] You don't have enough Glimmerstones."));
		return false;
	}

	if (GetGem() + pGemRefineInfo->wRefineResultCount >= GEM_MAX)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Gaya] You already have the maximum amount of Gaya."));
		return 0;
	}

	if (GetGold() < pGemRefineInfo->wRefineCost)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Gaya] You don't have enough Yang to cut Glimmerstones."));
		return false;
	}

	RemoveSpecifyItem(pItemTable->dwVnum, pGemRefineInfo->wRefineItemCount);
	PointChange(POINT_GOLD, static_cast<int64_t>(-pGemRefineInfo->wRefineCost));

	if (lpMetinStoneItem)
		lpMetinStoneItem->SetCount(lpMetinStoneItem->GetCount() - 1);

	BYTE bRefinePct = pGemRefineInfo->bRefinePct;
#	if defined(__CONQUEROR_LEVEL__)
	bRefinePct = (GetMapIndex() == pGemRefineInfo->iRefineSpecialMapIndex ? pGemRefineInfo->bRefinePctSpecial : pGemRefineInfo->bRefinePct);
#	endif

	if (number(1, 100) <= bRefinePct)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Gaya] Cutting successful."));
		PointChange(POINT_GEM, pGemRefineInfo->wRefineResultCount);
		return true;
	}
	else
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("[Gaya] Cutting failed."));

	return false;
}

#	if defined(__GEM_SHOP__)
void CHARACTER::SetGemShop(CGemShop* pGemShop)
{
	if (m_pGemShop != nullptr)
		delete m_pGemShop;

	m_pGemShop = pGemShop;
}
#	endif
#endif

#if defined(__EXTEND_INVEN_SYSTEM__)
void CHARACTER::ExtendInvenRequest()
{
	const LPDESC c_pDesc = GetDesc();
	if (c_pDesc == nullptr)
		return;

	if (GetExtendInvenStage() >= EX_INVENTORY_STAGE_MAX)
	{
		TPacketGCExtendInvenItemUse Packet;
		Packet.bHeader = HEADER_GC_EXTEND_INVEN_ITEM_USE;
		Packet.bMsgResult = EX_INVEN_FAIL_FOURTH_PAGE_STAGE_MAX;
		Packet.bEnoughCount = 0;
		c_pDesc->Packet(&Packet, sizeof(TPacketGCExtendInvenItemUse));
		return;
	}

	const BYTE c_bNeedKeys = abInvenStageKeys[GetExtendInvenStage()];
	WORD wTicketCount = 0;
	LPITEM pItem = nullptr;
	for (WORD wCell = 0; wCell < GetExtendInvenMax(); ++wCell)
	{
		pItem = GetInventoryItem(wCell);
		if (pItem == nullptr)
			continue;

		switch (pItem->GetVnum())
		{
			case ITEM_EXTEND_INVEN_TICKET:
			case ITEM_EXTEND_INVEN_TICKET_MALL:
				wTicketCount += pItem->GetCount();
				break;
		}
	}

	TPacketGCExtendInvenItemUse Packet;
	Packet.bHeader = HEADER_GC_EXTEND_INVEN_ITEM_USE;
	if (wTicketCount >= c_bNeedKeys)
	{
		Packet.bMsgResult = EX_INVEN_SUCCESS;
		Packet.bEnoughCount = c_bNeedKeys;
	}
	else
	{
		Packet.bMsgResult = EX_INVEN_FAIL_FALL_SHORT;
		Packet.bEnoughCount = MINMAX(0, c_bNeedKeys - wTicketCount, c_bNeedKeys);
	}
	c_pDesc->Packet(&Packet, sizeof(TPacketGCExtendInvenItemUse));
}

void CHARACTER::ExtendInvenUpgrade()
{
	const BYTE c_bStage = GetExtendInvenStage();
	if (c_bStage >= EX_INVENTORY_STAGE_MAX)
		return;

	if (GetExchange())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("You cannot expand your inventory while trading."));
		return;
	}

	const WORD c_wMaxInven = GetExtendInvenMax();

	LPITEM pItem = nullptr;
	WORD wTicketCount = 0;

	std::vector<WORD> vTicketCells;
	vTicketCells.reserve(c_wMaxInven);

	for (WORD wCell = 0; wCell < c_wMaxInven; ++wCell)
	{
		pItem = GetInventoryItem(wCell);
		if (pItem == nullptr)
			continue;

		const DWORD dwVnum = pItem->GetVnum();
		if (dwVnum == ITEM_EXTEND_INVEN_TICKET || dwVnum == ITEM_EXTEND_INVEN_TICKET_MALL)
		{
			wTicketCount += pItem->GetCount();
			vTicketCells.emplace_back(pItem->GetCell());
		}
	}

	const BYTE c_bNeedKeys = abInvenStageKeys[c_bStage];
	if (wTicketCount < c_bNeedKeys)
		return;

	SetExtendInvenStage(c_bStage + 1);
	SendExtendInvenPacket();

	for (BYTE bKey = 0; bKey < c_bNeedKeys; ++bKey)
	{
		for (const WORD wCell : vTicketCells)
		{
			if (pItem = GetInventoryItem(wCell))
			{
				RemoveSpecifyItem(pItem->GetVnum());
				break;
			}
		}
	}
}

void CHARACTER::SendExtendInvenPacket()
{
	TPacketGCExtendInven Packet;
	Packet.bHeader = HEADER_GC_EXTEND_INVEN;
	Packet.bStage = GetExtendInvenStage();
	Packet.wMaxNum = GetExtendInvenMax();
	GetDesc()->Packet(&Packet, sizeof(TPacketGCExtendInven));
}

WORD CHARACTER::GetExtendInvenMax() const
{
	BYTE bPageSize = INVENTORY_PAGE_SIZE * EX_INVENTORY_PAGE_COUNT;
	WORD wMaxNum = bPageSize + (INVENTORY_WIDTH * GetExtendInvenStage());
	if (wMaxNum >= INVENTORY_MAX_NUM)
		return INVENTORY_MAX_NUM;

	return wMaxNum;
}
#endif

#if defined(__ATTR_6TH_7TH__)
EVENTFUNC(hit_buff_event)
{
	const char_event_info* info = dynamic_cast<char_event_info*>(event->info);
	if (info == nullptr)
	{
		sys_err("hit_buff_event> <Factor> Null pointer");
		return 0;
	}

	const LPCHARACTER ch = info->ch;
	if (ch == nullptr)
		return 0;

	const int buff_cooldown = 60;
	const long buff_value = 30;
	std::unordered_map<WORD, WORD> map_buff_point
	{
		{ POINT_HIT_BUFF_ENCHANT_FIRE, POINT_ENCHANT_FIRE },
		{ POINT_HIT_BUFF_ENCHANT_ICE, POINT_ENCHANT_ICE },
		{ POINT_HIT_BUFF_ENCHANT_ELEC, POINT_ENCHANT_ELECT },
		{ POINT_HIT_BUFF_ENCHANT_WIND, POINT_ENCHANT_WIND },
		{ POINT_HIT_BUFF_ENCHANT_DARK, POINT_ENCHANT_DARK },
		{ POINT_HIT_BUFF_ENCHANT_EARTH, POINT_ENCHANT_EARTH },
		{ POINT_HIT_BUFF_RESIST_FIRE, POINT_ENCHANT_FIRE },
		{ POINT_HIT_BUFF_RESIST_ICE, POINT_RESIST_ICE },
		{ POINT_HIT_BUFF_RESIST_ELEC, POINT_RESIST_ELEC },
		{ POINT_HIT_BUFF_RESIST_WIND, POINT_RESIST_WIND },
		{ POINT_HIT_BUFF_RESIST_DARK, POINT_RESIST_DARK },
		{ POINT_HIT_BUFF_RESIST_EARTH, POINT_RESIST_EARTH },

		{ POINT_HIT_BUFF_SUNGMA_STR, POINT_SUNGMA_STR },
		{ POINT_HIT_BUFF_SUNGMA_MOVE, POINT_SUNGMA_MOVE },
		{ POINT_HIT_BUFF_SUNGMA_HP, POINT_SUNGMA_HP },
		{ POINT_HIT_BUFF_SUNGMA_IMMUNE , POINT_SUNGMA_IMMUNE },
	};

	for (const auto& point : map_buff_point)
	{
		const auto buff_time = ch->GetPoint(point.first);
		if (buff_time != 0)
			ch->AddAffect(AFFECT_ELEMENT_BUFF_CRACK, point.second, buff_value, AFF_NONE, buff_time, 0, true, true);
	}

	return PASSES_PER_SEC(buff_cooldown);
}

void CHARACTER::StartHitBuffEvent()
{
	if (m_pHitBuffEvent)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();
	info->ch = this;
	m_pHitBuffEvent = event_create(hit_buff_event, info, PASSES_PER_SEC(1));
}

void CHARACTER::StopHitBuffEvent()
{
	event_cancel(&m_pHitBuffEvent);
}
#endif

#if defined(__CLIENT_TIMER__)
void CHARACTER::SendClientTimer(BYTE bSubHeader, DWORD dwEndTime, DWORD dwAlarmSec)
{
	TPacketGCClientTimer Packet(bSubHeader);
	Packet.dwData[ECLIENT_TIMER_END_TIME] = std::time(nullptr) + dwEndTime;
	Packet.dwData[ECLIENT_TIMER_ALARM_SECOND] = dwAlarmSec;
	GetDesc()->Packet(&Packet, sizeof(TPacketGCClientTimer));
}
#endif

#if defined(__EXPRESSING_EMOTIONS__)
void CHARACTER::AddEmote(const INT iEmoteIndex)
{
	TPacketGDEmote GDPacket = {};
	GDPacket.dwPID = GetPlayerID();
	if (iEmoteIndex == 0)
		db_clientdesc->DBPacket(HEADER_GD_EMOTE_CLEAR, GetDesc()->GetHandle(), &GDPacket, sizeof(GDPacket));
	else
	{
		GDPacket.dwVnum = iEmoteIndex != -1 ? iEmoteIndex : number(SPECIAL_ACTION_START_INDEX, EMOTION_NUM - 1);
		GDPacket.dwDuration = SPECIAL_ACTION_DURATION;

		int iQFValue = quest::CQuestManager::Instance().GetEventFlag("special_action_duration");
		if (iQFValue != 0)
			GDPacket.dwDuration = iQFValue;

		db_clientdesc->DBPacket(HEADER_GD_EMOTE_ADD, GetDesc()->GetHandle(), &GDPacket, sizeof(GDPacket));
	}
}

void CHARACTER::SetEmotes(const TPacketGDEmote* pTable, const WORD c_wSize)
{
	if (c_wSize == 0)
	{
		TPacketGCEmote GCPacket = {};
		GCPacket.bHeader = HEADER_GC_EMOTE;
		GCPacket.bSubHeader = SUBHEADER_EMOTE_CLEAR;
		GetDesc()->Packet(&GCPacket, sizeof(TPacketGCEmote));
	}
	else
	{
		for (WORD wSize = 0; wSize < c_wSize; ++wSize, ++pTable)
		{
			TPacketGCEmote GCPacket = {};
			GCPacket.bHeader = HEADER_GC_EMOTE;
			GCPacket.bSubHeader = SUBHEADER_EMOTE_ADD;
			GCPacket.dwEmoteVnum = pTable->dwVnum;
			GCPacket.dwDuration = pTable->dwDuration;
			GetDesc()->Packet(&GCPacket, sizeof(TPacketGCEmote));
		}
	}
}
#endif

#if defined(__RACE_SWAP__)
void CHARACTER::SetEventRaceNum(DWORD dwRaceNum)
{
	if (m_dwEventRaceNum == dwRaceNum)
		return;

	m_dwEventRaceNum = dwRaceNum;

	sys_log(0, "EVENT_RACE: %s race %u ", GetName(), dwRaceNum);

	SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);
	m_afAffectFlag.Set(AFF_SPAWN);

	ViewReencode();

	REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);

	SetValidComboInterval(0);
}
#endif

#if defined(__HIDE_COSTUME_SYSTEM__)
void CHARACTER::SetHiddenCostumePart(BYTE bCostumeSubType, bool bHidden, bool bSave)
{
	switch (bCostumeSubType)
	{
		case COSTUME_BODY:
		case COSTUME_HAIR:
#if defined(__ACCE_COSTUME_SYSTEM__)
		case COSTUME_ACCE:
#endif
#if defined(__ACCE_COSTUME_SYSTEM__)
		case COSTUME_WEAPON:
#endif
#if defined(__ACCE_COSTUME_SYSTEM__)
		case COSTUME_AURA:
#endif
			break;

		default:
		{
			sys_err("CHARACTER::SetHiddenCostumePart: %s cannot hide unknown costume sub type %u",
				GetName(), bCostumeSubType);
			return;
		}
	}

	if (IsDead() || IsWarping())
		return;

	bool bAttacking = (get_dword_time() - GetLastAttackTime()) < 1500;
	bool bMoving = (get_dword_time() - GetLastMoveTime()) < 1500;
	bool bDelayedCMD = false;

	if (IsStateMove() || bAttacking || bMoving)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("You have to stand still to hide your costume."));
		return;
	}

	int iPulse = thecore_pulse();
	if (iPulse - m_dwHideCostumePulse < passes_per_sec * 3)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_STRING("You have to wait 3 seconds before you can hide your costume again."));
		return;
	}

	m_dwHideCostumePulse = thecore_pulse();
	m_bHiddenCostumePart[bCostumeSubType] = bHidden;

	if (bSave)
		SetQuestFlag(HiddenCostumePartMap[bCostumeSubType], bHidden ? 1 : 0);

	UpdatePacket();

	ChatPacket(CHAT_TYPE_COMMAND, "SetHiddenCostumePart %d %d", bCostumeSubType, bHidden ? 1 : 0);
}

bool CHARACTER::GetHiddenCostumeByPart(BYTE bPartPos) const
{
	switch (bPartPos)
	{
		case PART_MAIN:
			if (m_bHiddenCostumePart[COSTUME_BODY] && GetWear(WEAR_COSTUME_BODY))
				return true;
			break;

		case PART_HAIR:
			if (m_bHiddenCostumePart[COSTUME_HAIR] && GetWear(WEAR_COSTUME_HAIR))
				return true;
			break;

#if defined(__ACCE_COSTUME_SYSTEM__)
		case PART_ACCE:
			if (m_bHiddenCostumePart[COSTUME_ACCE] && GetWear(WEAR_COSTUME_ACCE))
				return true;
			break;
#endif

#if defined(__WEAPON_COSTUME_SYSTEM__)
		case PART_WEAPON:
			if (m_bHiddenCostumePart[COSTUME_WEAPON] && GetWear(WEAR_COSTUME_WEAPON))
				return true;
			break;
#endif

#if defined(__AURA_COSTUME_SYSTEM__)
		case PART_AURA:
			if (m_bHiddenCostumePart[COSTUME_AURA] && GetWear(WEAR_COSTUME_AURA))
				return true;
			break;
#endif

		default:
			return false;
	}

	return false;
}

void CHARACTER::SetHiddenCostumeParts()
{
	bool bHidden = false;
	for (const THiddenCostumePartMap::value_type& it : HiddenCostumePartMap)
	{
		bHidden = (GetQuestFlag(it.second) != 0 ? 1 : 0);
		m_bHiddenCostumePart[it.first] = bHidden;

		ChatPacket(CHAT_TYPE_COMMAND, "SetHiddenCostumePart %d %d", it.first, static_cast<BYTE>(bHidden));
	}
}
#endif

#if defined(__MYSHOP_DECO__)
void CHARACTER::OpenPrivateShop(BYTE bTabCount, bool bIsCashItem)
{
	if (bTabCount > MYSHOP_MAX_TABS)
		bTabCount = bTabCount;

	if (bIsCashItem == false)
	{
		SetMyShopDecoType(0);
		SetMyShopDecoPolyVnum(30000);
	}

	SetMyPrivShopTabCount(bTabCount);
	SetMyPrivShopIsCashItem(bIsCashItem);

	TPacketGCShop Packet;
	Packet.header = HEADER_GC_SHOP;
	Packet.subheader = SHOP_SUBHEADER_GC_MYPRIV_SHOP_OPEN;

	TPacketGCMyPrivShopOpen Packet2;
	Packet2.bCashItem = bIsCashItem;
	Packet2.bTabCount = bTabCount;

	Packet.size = sizeof(Packet) + sizeof(Packet2);

	if (GetDesc())
	{
		GetDesc()->BufferedPacket(&Packet, sizeof(TPacketGCShop));
		GetDesc()->Packet(&Packet2, sizeof(TPacketGCMyPrivShopOpen));
	}
}
#endif

#if defined(__LEFT_SEAT__)
EVENTFUNC(left_seat_wait_timer_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>(event->info);
	if (info == NULL)
	{
		sys_err("left_seat_wait_timer_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER ch = info->ch;
	if (ch == NULL) // <Factor>
		return 0;

	if (ch->GetMyShop())
		return PASSES_PER_SEC(LEFT_SEAT_PULSE);

	if (ch->IsRunningQuest())
		return PASSES_PER_SEC(LEFT_SEAT_PULSE);

	if (get_dword_time() - ch->GetLastRequestTime() > 1000 * ch->GetLeftSeatWaitTime())
	{
		if (ch->LeftSeat() == false)
		{
			ch->SetLeftSeat(true);

			if (ch->GetLeftSeatLogoutTime())
				ch->RestartLeftSeatLogoutTimer();
		}
	}

	return PASSES_PER_SEC(LEFT_SEAT_PULSE);
}

void CHARACTER::RestartLeftSeatWaitTimer()
{
	if (m_pLeftSeatWaitTimerEvent)
		event_cancel(&m_pLeftSeatWaitTimerEvent);

	char_event_info* info = AllocEventInfo<char_event_info>();
	info->ch = this;
	m_pLeftSeatWaitTimerEvent = event_create(left_seat_wait_timer_event, info, PASSES_PER_SEC(LEFT_SEAT_PULSE));
}

void CHARACTER::SetLeftSeatWaitTime(BYTE bIndex)
{
	DWORD dwTime = 0;
	switch (bIndex)
	{
		case LEFT_SEAT_TIME_10_MIN:
			dwTime = 60 * 10;
			break;
		case LEFT_SEAT_TIME_30_MIN:
			dwTime = 60 * 30;
			break;
		case LEFT_SEAT_TIME_90_MIN:
			dwTime = 60 * 90;
			break;
		default:
			dwTime = 60 * 10;
			break;
	}

	//if (test_server)
	//	dwTime = 10;

	m_dwLeftSeatWaitTime = dwTime;
	RestartLeftSeatWaitTimer();
}

EVENTFUNC(left_seat_logout_timer_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>(event->info);
	if (info == NULL)
	{
		sys_err("left_seat_logout_timer_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER ch = info->ch;
	if (ch == NULL) // <Factor>
		return 0;

	LPDESC d = ch->GetDesc();
	if (ch->LeftSeat())
	{
		ch->Disconnect("LEFT_SEAT");
		if (d)
			d->SetPhase(PHASE_CLOSE);
	}

	return 0;
}

void CHARACTER::RestartLeftSeatLogoutTimer()
{
	if (m_pLeftSeatLogoutTimerEvent)
		event_cancel(&m_pLeftSeatLogoutTimerEvent);

	const DWORD dwTime = GetLeftSeatLogoutTime();
	if (dwTime == 0)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();
	info->ch = this;
	m_pLeftSeatLogoutTimerEvent = event_create(left_seat_logout_timer_event, info, PASSES_PER_SEC(dwTime));
}

void CHARACTER::SetLeftSeatLogoutTime(BYTE bIndex)
{
	DWORD dwTime = 0;
	switch (bIndex)
	{
		case LEFT_SEAT_LOGOUT_TIME_30_MIN:
			dwTime = 60 * 30;
			break;
		case LEFT_SEAT_LOGOUT_TIME_60_MIN:
			dwTime = 60 * 60;
			break;
		case LEFT_SEAT_LOGOUT_TIME_120_MIN:
			dwTime = 60 * 120;
			break;
		case LEFT_SEAT_LOGOUT_TIME_180_MIN:
			dwTime = 60 * 180;
			break;
		case LEFT_SEAT_LOGOUT_TIME_OFF:
			dwTime = 0;
			break;
		default:
			dwTime = 60 * 30;
			break;
	}

	//if (test_server)
	//	dwTime = 10;

	m_dwLeftSeatLogoutTime = dwTime;
}

void CHARACTER::DisableLeftSeatLogOutState(bool bClosePopup)
{
	if (bClosePopup)
		ChatPacket(CHAT_TYPE_COMMAND, "CloseLeftSeatDialog");

	if (m_pLeftSeatLogoutTimerEvent)
		event_cancel(&m_pLeftSeatLogoutTimerEvent);

	m_dwLastRequestTime = get_dword_time();

	SetLeftSeat(false);
	RestartLeftSeatWaitTimer();
}

void CHARACTER::SetLeftSeat(bool bLeftSeat)
{
	if (bLeftSeat)
		ChatPacket(CHAT_TYPE_COMMAND, "OpenLeftSeatDialog");

	m_bLeftSeat = bLeftSeat;
	UpdatePacket();
}
#endif

#if defined(__ELEMENTAL_DUNGEON__)
EVENTFUNC(elemental_curse_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>(event->info);
	if (info == NULL)
	{
		sys_err("elemental_curse_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER ch = info->ch;
	if (ch == NULL)
		return 0;

	const static float fReductionPct[10] = { 1.0f, 2.5f, 4.5f, 6.5f, 8.5f, 10.5f, 12.5f, 15.0f, 16.5f, 18.0f };
	DWORD dwAccumulatedDamage = ch->GetAccumulatedDamage();

	// NOTE : Increase the hp reduction level every 200.000 damage.
	int iReductionLevel = MINMAX(1, dwAccumulatedDamage / 200000, 10);
	int iDropHP = static_cast<int>(ch->GetMaxHP() * (fReductionPct[iReductionLevel - 1] / 100.0f));

	CAffect* pAffect = ch->FindAffect(AFFECT_CURSE_OF_ELEMENTAL);
	if (pAffect == NULL || iReductionLevel > pAffect->lApplyValue)
		ch->AddAffect(AFFECT_CURSE_OF_ELEMENTAL, APPLY_NONE, iReductionLevel, AFF_NONE, INFINITE_AFFECT_DURATION, 0, true);

	if (ch->FindAffect(AFFECT_PROTECTION_OF_ELEMENTAL))
	{
		if (test_server)
			ch->ChatPacket(CHAT_TYPE_PARTY, "elemental_curse_event> protection success");

		return PASSES_PER_SEC(15);
	}

	if (test_server)
		ch->ChatPacket(CHAT_TYPE_PARTY, "elemental_curse_event> drain hp -%d (reduction level %d)",
			-iDropHP, ch->GetMaxHP(), iReductionLevel);

	ch->PointChange(POINT_HP, -iDropHP);

	return PASSES_PER_SEC(15);
}

void CHARACTER::StartElementalCurseEvent()
{
	if (m_pElementalCurseEvent)
		return;

	if (test_server)
		ChatPacket(CHAT_TYPE_PARTY, "Elemental Curse SUCCESS");

	char_event_info* info = AllocEventInfo<char_event_info>();
	info->ch = this;
	m_pElementalCurseEvent = event_create(elemental_curse_event, info, PASSES_PER_SEC(1));
}

void CHARACTER::StopElementalCurseEvent()
{
	if (m_pElementalCurseEvent)
		event_cancel(&m_pElementalCurseEvent);

	m_pElementalCurseEvent = NULL;

	RemoveAffect(AFFECT_CURSE_OF_ELEMENTAL);
}

void CHARACTER::SetAccumulatedDamage(DWORD dwDamage)
{
	if (m_dwAccumulatedDamage > UINT32_MAX - dwDamage)
		m_dwAccumulatedDamage = UINT32_MAX;
	else
		m_dwAccumulatedDamage += dwDamage;
}
#endif

#if defined(__GUILD_DRAGONLAIR_SYSTEM__)
void CHARACTER::SetGuildDragonLair(CGuildDragonLair* pGuildDragonLair)
{
	if (pGuildDragonLair && m_pGuildDragonLair)
		sys_err("%s is trying to reassign guild_dragonlair (current %p, new %p)",
			GetName(), get_pointer(m_pGuildDragonLair), get_pointer(pGuildDragonLair));

	if (m_pGuildDragonLair == pGuildDragonLair)
		return;

	if (IsPC() && m_pGuildDragonLair)
		m_pGuildDragonLair->DecMember(this);

	m_pGuildDragonLair = pGuildDragonLair;

	if (IsPC() && pGuildDragonLair)
		m_pGuildDragonLair->IncMember(this);
}
#endif

#if defined(__SUMMER_EVENT_ROULETTE__)
void CHARACTER::SetMiniGameRoulette(CMiniGameRoulette* pMiniGameRoulette)
{
	if (m_pMiniGameRoulette)
		M2_DELETE(m_pMiniGameRoulette);

	m_pMiniGameRoulette = pMiniGameRoulette;
}
#endif

#if defined(__DEFENSE_WAVE__)
void CHARACTER::SetDefenseWave(LPDEFENSE_WAVE pDefenseWave)
{
	if (pDefenseWave && m_pDefenseWave)
		sys_err("%s is trying to reassign defense_wave (current %p, new %p)",
			GetName(), get_pointer(m_pDefenseWave), get_pointer(pDefenseWave));

	if (m_pDefenseWave == pDefenseWave)
		return;

	m_pDefenseWave = pDefenseWave;
	SetDungeon(m_pDefenseWave);
}

LPDEFENSE_WAVE CHARACTER::GetDefenseWave() const
{
	if (m_pDefenseWave)
		return m_pDefenseWave;

	if (CDefenseWaveManager::Instance().IsDefenseWaveMap(GetMapIndex()))
		return dynamic_cast<LPDEFENSE_WAVE>(m_pkDungeon);

	return NULL;
}
#endif

#ifdef ENABLE_QUEEN_NETHIS
bool CHARACTER::IsSnakeMap()
{
	return SnakeLair::CSnk::instance().IsSnakeMap(GetMapIndex());
}
#endif
