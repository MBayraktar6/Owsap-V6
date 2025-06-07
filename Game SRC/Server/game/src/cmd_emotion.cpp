#include "stdafx.h"
#include "utils.h"
#include "cmd.h"
#include "constants.h"
#include "char.h"
#include "char_manager.h"
#include "marriage.h"
#include "messenger_manager.h"
#include "questmanager.h"
#include "config.h"
#include "wedding.h"
#include "unique_item.h"

std::set<std::pair<DWORD, DWORD>> s_EmotionAllowSet;

bool CHARACTER_CanEmotion(const CHARACTER& rkChar)
{
	// 결혼식 맵에서는 사용할 수 있다.
	if (marriage::WeddingManager::instance().IsWeddingMap(rkChar.GetMapIndex()))
		return true;

	if (g_bDisableEmotionMask)
		return true;

	// 열정의 가면 착용시 사용할 수 있다.
	if (rkChar.IsEquipUniqueGroup(UNIQUE_GROUP_EMOTION_MASK))
		return true;

	return false;
}

ACMD(do_emotion_allow)
{
	char szArg1[256];
	one_argument(argument, szArg1, sizeof(szArg1));

	if (!*szArg1)
		return;

	if (!isnhdigit(*szArg1))
		return;

	DWORD dwVID = 0;
	str_to_number(dwVID, szArg1);

#if defined(__MESSENGER_BLOCK_SYSTEM__)
	const LPCHARACTER pkTarget = CHARACTER_MANAGER::instance().Find(dwVID);
	if (pkTarget == nullptr)
		return;

	if (CMessengerManager::instance().IsBlocked(ch->GetName(), pkTarget->GetName()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("Unblock %s to continue.", pkTarget->GetName()));
		return;
	}
	else if (CMessengerManager::instance().IsBlocked(pkTarget->GetName(), ch->GetName()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("%s has blocked you.", pkTarget->GetName()));
		return;
	}
#endif

	s_EmotionAllowSet.emplace(std::make_pair(ch->GetVID(), dwVID));
}

ACMD(do_emotion_play)
{
	char szArg1[256];
	one_argument(argument, szArg1, sizeof(szArg1));

	if (!*szArg1)
		return;

	if (!isnhdigit(*szArg1))
		return;

	if (ch->IsRiding())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("말을 탄 상태에서 감정표현을 할 수 없습니다."));
		return;
	}

	if (!CHARACTER_CanEmotion(*ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("열정의 가면을 착용시에만 할 수 있습니다."));
		return;
	}

	DWORD dwEmoteVnum = EMOTION_NONE;
	str_to_number(dwEmoteVnum, szArg1);

	const TEmotionInfoMap::const_iterator& it = EmotionInfoMap.find(dwEmoteVnum);
	if (it == EmotionInfoMap.end())
	{
		sys_err("cannot find emotion vnum %u", dwEmoteVnum);
		return;
	}

	LPCHARACTER pkTarget = nullptr;

	const TEmotionInfo Emotion = it->second;
	if (IS_SET(Emotion.dwFlag, EMOTION_FLAG_TARGET))
	{
		pkTarget = ch->GetTarget();
		if (pkTarget == nullptr)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("그런 사람이 없습니다."));
			return;
		}
	}

	if (pkTarget)
	{
		if (!pkTarget->IsPC() || pkTarget == ch)
			return;

		if (pkTarget->IsRiding())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("말을 탄 상대와 감정표현을 할 수 없습니다."));
			return;
		}

		long lDistance = DISTANCE_APPROX(ch->GetX() - pkTarget->GetX(), ch->GetY() - pkTarget->GetY());

		if (lDistance < 10)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("너무 가까이 있습니다."));
			return;
		}

		if (lDistance > 500)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("너무 멀리 있습니다"));
			return;
		}

		if (IS_SET(Emotion.dwFlag, EMOTION_FLAG_OTHER_SEX))
		{
			if (GET_SEX(ch) == GET_SEX(pkTarget))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("이성간에만 할 수 있습니다."));
				return;
			}
		}

		if (s_EmotionAllowSet.find(std::make_pair(pkTarget->GetVID(), ch->GetVID())) == s_EmotionAllowSet.end())
		{
			if (marriage::CManager::instance().IsMarried(ch->GetPlayerID()))
			{
				const marriage::TMarriage* pMarriageInfo = marriage::CManager::instance().Get(ch->GetPlayerID());
				const DWORD dwPID = pMarriageInfo->GetOther(ch->GetPlayerID());
				if (0 == dwPID || dwPID != pkTarget->GetPlayerID())
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("이 행동은 상호동의 하에 가능 합니다."));
					return;
				}
			}
			else
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("이 행동은 상호동의 하에 가능 합니다."));
				return;
			}
		}

		s_EmotionAllowSet.emplace(std::make_pair(ch->GetVID(), pkTarget->GetVID()));
	}

	TPacketGCEmote Packet;
	Packet.bHeader = HEADER_GC_EMOTE;
	Packet.bSubHeader = SUBHEADER_EMOTE_MOTION;
	Packet.dwEmoteVnum = dwEmoteVnum;
	Packet.dwMainVID = ch->GetVID();
	Packet.dwTargetVID = (pkTarget ? pkTarget->GetVID() : 0);
	ch->PacketAround(&Packet, sizeof(TPacketGCEmote));

#if defined(__QUEST_EVENT_EMOTION__)
	ch->SetQuestNPCID(pkTarget ? pkTarget->GetVID() : 0);
	quest::CQuestManager::instance().Emotion(ch->GetPlayerID(), pkTarget ? pkTarget->GetRaceNum() : quest::QUEST_NO_NPC);
#endif
}

ACMD(do_emoticon)
{
	char szArg1[256];
	one_argument(argument, szArg1, sizeof(szArg1));

	if (!*szArg1)
		return;

	if (!isnhdigit(*szArg1))
		return;

	DWORD dwEmoteVnum = 0;
	str_to_number(dwEmoteVnum, szArg1);

	TPacketGCEmote Packet = {};
	Packet.bHeader = HEADER_GC_EMOTE;
	Packet.bSubHeader = SUBHEADER_EMOTE_ICON;
	Packet.dwEmoteVnum = dwEmoteVnum;
	Packet.dwMainVID = ch->GetVID();
	ch->PacketAround(&Packet, sizeof(TPacketGCEmote));
}
