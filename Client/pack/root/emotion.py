import app
import chr
import localeInfo
import player
import chrmgr
import net

EMOTION_CLAP = player.EMOTION_CLAP
EMOTION_CONGRATULATION = player.EMOTION_CONGRATULATION
EMOTION_FORGIVE = player.EMOTION_FORGIVE
EMOTION_ANGRY = player.EMOTION_ANGRY
EMOTION_ATTRACTIVE = player.EMOTION_ATTRACTIVE
EMOTION_SAD = player.EMOTION_SAD
EMOTION_SHY = player.EMOTION_SHY
EMOTION_CHEERUP = player.EMOTION_CHEERUP
EMOTION_BANTER =player. EMOTION_BANTER
EMOTION_JOY = player.EMOTION_JOY
EMOTION_CHEERS_1 = player.EMOTION_CHEERS_1
EMOTION_CHEERS_2 = player.EMOTION_CHEERS_2
EMOTION_DANCE_1 = player.EMOTION_DANCE_1
EMOTION_DANCE_2 = player.EMOTION_DANCE_2
EMOTION_DANCE_3 = player.EMOTION_DANCE_3
EMOTION_DANCE_4 = player.EMOTION_DANCE_4
EMOTION_DANCE_5 = player.EMOTION_DANCE_5
EMOTION_DANCE_6 = player.EMOTION_DANCE_6

EMOTION_KISS = player.EMOTION_KISS
EMOTION_FRENCH_KISS = player.EMOTION_FRENCH_KISS
EMOTION_SLAP = player.EMOTION_SLAP

if app.ENABLE_EXPRESSING_EMOTION:
	EMOTION_PUSH_UP = app.SPECIAL_ACTION_START_INDEX
	EMOTION_DANCE_7 = app.SPECIAL_ACTION_START_INDEX + 1
	EMOTION_EXERCISE = app.SPECIAL_ACTION_START_INDEX + 2
	EMOTION_DOZE = app.SPECIAL_ACTION_START_INDEX + 3
	EMOTION_SELFIE = app.SPECIAL_ACTION_START_INDEX + 4
	EMOTION_CHARGING = app.SPECIAL_ACTION_START_INDEX + 5
	EMOTION_NOSAY = app.SPECIAL_ACTION_START_INDEX + 6
	EMOTION_WEATHER_1 = app.SPECIAL_ACTION_START_INDEX + 7
	EMOTION_WEATHER_2 = app.SPECIAL_ACTION_START_INDEX + 8
	EMOTION_WEATHER_3 = app.SPECIAL_ACTION_START_INDEX + 9
	EMOTION_HUNGRY = app.SPECIAL_ACTION_START_INDEX + 10
	EMOTION_SIREN = app.SPECIAL_ACTION_START_INDEX + 11
	EMOTION_LETTER = app.SPECIAL_ACTION_START_INDEX + 12
	EMOTION_CALL = app.SPECIAL_ACTION_START_INDEX + 13
	EMOTION_CELEBRATION = app.SPECIAL_ACTION_START_INDEX + 14
	EMOTION_ALCOHOL = app.SPECIAL_ACTION_START_INDEX + 15
	EMOTION_BUSY = app.SPECIAL_ACTION_START_INDEX + 16
	EMOTION_WHIRL = app.SPECIAL_ACTION_START_INDEX + 17
	EMOTION_SMH = app.SPECIAL_ACTION_START_INDEX + 18

EMOTION_DICT = {
	EMOTION_CLAP : { "name" : localeInfo.EMOTION_CLAP, "target" : 0 },
	EMOTION_DANCE_1 : { "name" : localeInfo.EMOTION_DANCE_1, "target" : 0 },
	EMOTION_DANCE_2 : { "name" : localeInfo.EMOTION_DANCE_2, "target" : 0 },
	EMOTION_DANCE_3 : { "name" : localeInfo.EMOTION_DANCE_3, "target" : 0 },
	EMOTION_DANCE_4 : { "name" : localeInfo.EMOTION_DANCE_4, "target" : 0 },
	EMOTION_DANCE_5 : { "name" : localeInfo.EMOTION_DANCE_5, "target" : 0 },
	EMOTION_DANCE_6 : { "name" : localeInfo.EMOTION_DANCE_6, "target" : 0 },
	EMOTION_CONGRATULATION : { "name" : localeInfo.EMOTION_CONGRATULATION, "target" : 0 },
	EMOTION_FORGIVE : { "name" : localeInfo.EMOTION_FORGIVE, "target" : 0 },
	EMOTION_ANGRY : { "name" : localeInfo.EMOTION_ANGRY, "target" : 0 },
	EMOTION_ATTRACTIVE : { "name" : localeInfo.EMOTION_ATTRACTIVE, "target" : 0 },
	EMOTION_SAD : { "name" : localeInfo.EMOTION_SAD, "target" : 0 },
	EMOTION_SHY : { "name" : localeInfo.EMOTION_SHY, "target" : 0 },
	EMOTION_CHEERUP : { "name" : localeInfo.EMOTION_CHEERUP, "target" : 0 },
	EMOTION_BANTER : { "name" : localeInfo.EMOTION_BANTER, "target" : 0 },
	EMOTION_JOY : { "name" : localeInfo.EMOTION_JOY, "target" : 0 },
	EMOTION_CHEERS_1 : { "name" : localeInfo.EMOTION_CHEERS_1, "target" : 0 },
	EMOTION_CHEERS_2 : { "name" : localeInfo.EMOTION_CHEERS_2, "target" : 0 },
	EMOTION_KISS : { "name" : localeInfo.EMOTION_CLAP_KISS, "target" : 1 },
	EMOTION_FRENCH_KISS : { "name" : localeInfo.EMOTION_FRENCH_KISS, "target" : 1 },
	EMOTION_SLAP : { "name" : localeInfo.EMOTION_SLAP, "target" : 1 },
}

if app.ENABLE_EXPRESSING_EMOTION:
	EMOTION_DICT[EMOTION_PUSH_UP] = { "name" : localeInfo.EMOTION_PUSH_UP, "target" : 0 }
	EMOTION_DICT[EMOTION_DANCE_7] = { "name" : localeInfo.EMOTION_DANCE_7, "target" : 0 }
	EMOTION_DICT[EMOTION_EXERCISE] = { "name" : localeInfo.EMOTION_EXERCISE, "target" : 0 }
	EMOTION_DICT[EMOTION_DOZE] = { "name" : localeInfo.EMOTION_DOZE, "target" : 0 }
	EMOTION_DICT[EMOTION_SELFIE] = { "name" : localeInfo.EMOTION_SELFIE, "target" : 0 }
	EMOTION_DICT[EMOTION_CHARGING] = { "name" : localeInfo.EMOTION_CHARGING, "target" : 0 }
	EMOTION_DICT[EMOTION_NOSAY] = { "name" : localeInfo.EMOTION_NOSAY, "target" : 0 }
	EMOTION_DICT[EMOTION_WEATHER_1] = { "name" : localeInfo.EMOTION_WEATHER_1, "target" : 0 }
	EMOTION_DICT[EMOTION_WEATHER_2] = { "name" : localeInfo.EMOTION_WEATHER_2, "target" : 0 }
	EMOTION_DICT[EMOTION_WEATHER_3] = { "name" : localeInfo.EMOTION_WEATHER_3, "target" : 0 }
	EMOTION_DICT[EMOTION_HUNGRY] = { "name" : localeInfo.EMOTION_HUNGRY, "target" : 0 }
	EMOTION_DICT[EMOTION_SIREN] = { "name" : localeInfo.EMOTION_SIREN, "target" : 0 }
	EMOTION_DICT[EMOTION_LETTER] = { "name" : localeInfo.EMOTION_LETTER, "target" : 0 }
	EMOTION_DICT[EMOTION_CALL] = { "name" : localeInfo.EMOTION_CALL, "target" : 0 }
	EMOTION_DICT[EMOTION_CELEBRATION] = {"name": localeInfo.EMOTION_CELEBRATION, "target" : 0 }
	EMOTION_DICT[EMOTION_ALCOHOL] = { "name" : localeInfo.EMOTION_ALCOHOL, "target" : 0 }
	EMOTION_DICT[EMOTION_BUSY] = { "name" : localeInfo.EMOTION_BUSY, "target" : 0 }
	EMOTION_DICT[EMOTION_WHIRL] = { "name" : localeInfo.EMOTION_WHIRL, "target" : 0 }

ICON_DICT = {
	EMOTION_CLAP : "d:/ymir work/ui/game/windows/emotion_clap.sub",
	EMOTION_CONGRATULATION : "icon/action/congratulation.tga",
	EMOTION_FORGIVE : "icon/action/forgive.tga",
	EMOTION_ANGRY : "icon/action/angry.tga",
	EMOTION_ATTRACTIVE : "icon/action/attractive.tga",
	EMOTION_SAD : "icon/action/sad.tga",
	EMOTION_SHY : "icon/action/shy.tga",
	EMOTION_CHEERUP : "icon/action/cheerup.tga",
	EMOTION_BANTER : "icon/action/banter.tga",
	EMOTION_JOY : "icon/action/joy.tga",
	EMOTION_CHEERS_1 : "d:/ymir work/ui/game/windows/emotion_cheers_1.sub",
	EMOTION_CHEERS_2 : "d:/ymir work/ui/game/windows/emotion_cheers_2.sub",
	EMOTION_DANCE_1 : "icon/action/dance1.tga",
	EMOTION_DANCE_2 : "icon/action/dance2.tga",
	EMOTION_DANCE_3 : "icon/action/dance3.tga",
	EMOTION_DANCE_4 : "icon/action/dance4.tga",
	EMOTION_DANCE_5 : "icon/action/dance5.tga",
	EMOTION_DANCE_6 : "icon/action/dance6.tga",
	EMOTION_KISS : "d:/ymir work/ui/game/windows/emotion_kiss.sub",
	EMOTION_FRENCH_KISS : "d:/ymir work/ui/game/windows/emotion_french_kiss.sub",
	EMOTION_SLAP : "d:/ymir work/ui/game/windows/emotion_slap.sub",
}

if app.ENABLE_EXPRESSING_EMOTION:
	ICON_DICT[EMOTION_PUSH_UP] = "icon/action/pushup.tga"
	ICON_DICT[EMOTION_DANCE_7] = "icon/action/dance7.tga"
	ICON_DICT[EMOTION_EXERCISE] = "icon/action/exercise.tga"
	ICON_DICT[EMOTION_DOZE] = "icon/action/doze.tga"
	ICON_DICT[EMOTION_SELFIE] = "icon/action/selfie.tga"
	ICON_DICT[EMOTION_CHARGING] = "icon/action/charging.tga"
	ICON_DICT[EMOTION_NOSAY] = "icon/action/nosay.tga"
	ICON_DICT[EMOTION_WEATHER_1] = "icon/action/weather1.tga"
	ICON_DICT[EMOTION_WEATHER_2] = "icon/action/weather2.tga"
	ICON_DICT[EMOTION_WEATHER_3] = "icon/action/weather3.tga"
	ICON_DICT[EMOTION_HUNGRY] = "icon/action/hungry.tga"
	ICON_DICT[EMOTION_SIREN] = "icon/action/siren.tga"
	ICON_DICT[EMOTION_LETTER] = "icon/action/letter.tga"
	ICON_DICT[EMOTION_CALL] = "icon/action/call.tga"
	ICON_DICT[EMOTION_CELEBRATION] = "icon/action/celebration.tga"
	ICON_DICT[EMOTION_ALCOHOL] = "icon/action/alcohol.tga"
	ICON_DICT[EMOTION_BUSY] = "icon/action/busy.tga"
	ICON_DICT[EMOTION_WHIRL] = "icon/action/whirl.tga"
	ICON_DICT[EMOTION_SMH] = "icon/action/sungmahee_tower.tga"

ANI_DICT = {
	chr.MOTION_CLAP : "clap.msa",
	chr.MOTION_CHEERS_1 : "cheers_1.msa",
	chr.MOTION_CHEERS_2 : "cheers_2.msa",
	chr.MOTION_KISS_WITH_WARRIOR : "kiss_with_warrior.msa",
	chr.MOTION_KISS_WITH_ASSASSIN : "kiss_with_assassin.msa",
	chr.MOTION_KISS_WITH_SURA : "kiss_with_sura.msa",
	chr.MOTION_KISS_WITH_SHAMAN : "kiss_with_shaman.msa",
	chr.MOTION_KISS_WITH_WOLFMAN : "kiss_with_wolfman.msa",
	chr.MOTION_FRENCH_KISS_WITH_WARRIOR : "french_kiss_with_warrior.msa",
	chr.MOTION_FRENCH_KISS_WITH_ASSASSIN : "french_kiss_with_assassin.msa",
	chr.MOTION_FRENCH_KISS_WITH_SURA : "french_kiss_with_sura.msa",
	chr.MOTION_FRENCH_KISS_WITH_SHAMAN : "french_kiss_with_shaman.msa",
	chr.MOTION_FRENCH_KISS_WITH_WOLFMAN : "french_kiss_with_wolfman.msa",
	chr.MOTION_SLAP_HIT_WITH_WARRIOR : "slap_hit.msa",
	chr.MOTION_SLAP_HIT_WITH_ASSASSIN : "slap_hit.msa",
	chr.MOTION_SLAP_HIT_WITH_SURA : "slap_hit.msa",
	chr.MOTION_SLAP_HIT_WITH_SHAMAN : "slap_hit.msa",
	chr.MOTION_SLAP_HIT_WITH_WOLFMAN : "slap_hit.msa",
	chr.MOTION_SLAP_HURT_WITH_WARRIOR : "slap_hurt.msa",
	chr.MOTION_SLAP_HURT_WITH_ASSASSIN : "slap_hurt.msa",
	chr.MOTION_SLAP_HURT_WITH_SURA : "slap_hurt.msa",
	chr.MOTION_SLAP_HURT_WITH_SHAMAN : "slap_hurt.msa",
	chr.MOTION_SLAP_HURT_WITH_WOLFMAN : "slap_hurt.msa",
	chr.MOTION_DANCE_1 : "dance_1.msa",
	chr.MOTION_DANCE_2 : "dance_2.msa",
	chr.MOTION_DANCE_3 : "dance_3.msa",
	chr.MOTION_DANCE_4 : "dance_4.msa",
	chr.MOTION_DANCE_5 : "dance_5.msa",
	chr.MOTION_DANCE_6 : "dance_6.msa",
	chr.MOTION_CONGRATULATION : "congratulation.msa",
	chr.MOTION_FORGIVE : "forgive.msa",
	chr.MOTION_ANGRY : "angry.msa",
	chr.MOTION_ATTRACTIVE : "attractive.msa",
	chr.MOTION_SAD : "sad.msa",
	chr.MOTION_SHY : "shy.msa",
	chr.MOTION_CHEERUP : "cheerup.msa",
	chr.MOTION_BANTER : "banter.msa",
	chr.MOTION_JOY : "joy.msa",
}

if app.ENABLE_EXPRESSING_EMOTION:
	ANI_DICT[chr.MOTION_EMOTION_PUSH_UP] = "pushup.msa"
	ANI_DICT[chr.MOTION_EMOTION_DANCE_7] = "dance_7.msa"
	ANI_DICT[chr.MOTION_EMOTION_EXERCISE] = "exercise.msa"
	ANI_DICT[chr.MOTION_EMOTION_DOZE] = "doze.msa"
	ANI_DICT[chr.MOTION_EMOTION_SELFIE] = "selfie.msa"

def __RegisterSharedEmotionAnis(mode, path):
	chrmgr.SetPathName(path)
	chrmgr.RegisterMotionMode(mode)

	for key, val in ANI_DICT.items():
		chrmgr.RegisterMotionData(mode, key, val)

def RegisterEmotionAnis(path):
	actionPath = path + "action/"
	weddingPath = path + "wedding/"

	__RegisterSharedEmotionAnis(chr.MOTION_MODE_GENERAL, actionPath)
	__RegisterSharedEmotionAnis(chr.MOTION_MODE_WEDDING_DRESS, actionPath)

	chrmgr.SetPathName(weddingPath)
	chrmgr.RegisterMotionMode(chr.MOTION_MODE_WEDDING_DRESS)
	chrmgr.RegisterCacheMotionData(chr.MOTION_MODE_WEDDING_DRESS, chr.MOTION_WAIT, "wait.msa")
	chrmgr.RegisterCacheMotionData(chr.MOTION_MODE_WEDDING_DRESS, chr.MOTION_WALK, "walk.msa")
	chrmgr.RegisterCacheMotionData(chr.MOTION_MODE_WEDDING_DRESS, chr.MOTION_RUN, "walk.msa")

def RegisterEmotionIcons():
	for key, val in ICON_DICT.items():
		player.RegisterEmotionIcon(key, val)

if app.ENABLE_LOCALE_CLIENT:
	def ReloadEmotionDict():
		global EMOTION_DICT
		EMOTION_DICT = {
			EMOTION_CLAP : { "name" : localeInfo.EMOTION_CLAP, "target" : 0 },
			EMOTION_DANCE_1 : { "name" : localeInfo.EMOTION_DANCE_1, "target" : 0 },
			EMOTION_DANCE_2 : { "name" : localeInfo.EMOTION_DANCE_2, "target" : 0 },
			EMOTION_DANCE_3 : { "name" : localeInfo.EMOTION_DANCE_3, "target" : 0 },
			EMOTION_DANCE_4 : { "name" : localeInfo.EMOTION_DANCE_4, "target" : 0 },
			EMOTION_DANCE_5 : { "name" : localeInfo.EMOTION_DANCE_5, "target" : 0 },
			EMOTION_DANCE_6 : { "name" : localeInfo.EMOTION_DANCE_6, "target" : 0 },
			EMOTION_CONGRATULATION : { "name" : localeInfo.EMOTION_CONGRATULATION, "target" : 0 },
			EMOTION_FORGIVE : { "name" : localeInfo.EMOTION_FORGIVE, "target" : 0 },
			EMOTION_ANGRY : { "name" : localeInfo.EMOTION_ANGRY, "target" : 0 },
			EMOTION_ATTRACTIVE : { "name" : localeInfo.EMOTION_ATTRACTIVE, "target" : 0 },
			EMOTION_SAD : { "name" : localeInfo.EMOTION_SAD, "target" : 0 },
			EMOTION_SHY : { "name" : localeInfo.EMOTION_SHY, "target" : 0 },
			EMOTION_CHEERUP : { "name" : localeInfo.EMOTION_CHEERUP, "target" : 0 },
			EMOTION_BANTER : { "name" : localeInfo.EMOTION_BANTER, "target" : 0 },
			EMOTION_JOY : { "name" : localeInfo.EMOTION_JOY, "target" : 0 },
			EMOTION_CHEERS_1 : { "name" : localeInfo.EMOTION_CHEERS_1, "target" : 0 },
			EMOTION_CHEERS_2 : { "name" : localeInfo.EMOTION_CHEERS_2, "target" : 0 },
			EMOTION_KISS : { "name" : localeInfo.EMOTION_CLAP_KISS, "target" : 1 },
			EMOTION_FRENCH_KISS : { "name" : localeInfo.EMOTION_FRENCH_KISS, "target" : 1 },
			EMOTION_SLAP : { "name" : localeInfo.EMOTION_SLAP, "target" : 1 },
		}

		if app.ENABLE_EXPRESSING_EMOTION:
			EMOTION_DICT[EMOTION_PUSH_UP] = { "name" : localeInfo.EMOTION_PUSH_UP, "target" : 0 }
			EMOTION_DICT[EMOTION_DANCE_7] = { "name" : localeInfo.EMOTION_DANCE_7, "target" : 0 }
			EMOTION_DICT[EMOTION_EXERCISE] = { "name" : localeInfo.EMOTION_EXERCISE, "target" : 0 }
			EMOTION_DICT[EMOTION_DOZE] = { "name" : localeInfo.EMOTION_DOZE, "target" : 0 }
			EMOTION_DICT[EMOTION_SELFIE] = { "name" : localeInfo.EMOTION_SELFIE, "target" : 0 }
			EMOTION_DICT[EMOTION_CHARGING] = { "name" : localeInfo.EMOTION_CHARGING, "target" : 0 }
			EMOTION_DICT[EMOTION_NOSAY] = { "name" : localeInfo.EMOTION_NOSAY, "target" : 0 }
			EMOTION_DICT[EMOTION_WEATHER_1] = { "name" : localeInfo.EMOTION_WEATHER_1, "target" : 0 }
			EMOTION_DICT[EMOTION_WEATHER_2] = { "name" : localeInfo.EMOTION_WEATHER_2, "target" : 0 }
			EMOTION_DICT[EMOTION_WEATHER_3] = { "name" : localeInfo.EMOTION_WEATHER_3, "target" : 0 }
			EMOTION_DICT[EMOTION_HUNGRY] = { "name" : localeInfo.EMOTION_HUNGRY, "target" : 0 }
			EMOTION_DICT[EMOTION_SIREN] = { "name" : localeInfo.EMOTION_SIREN, "target" : 0 }
			EMOTION_DICT[EMOTION_LETTER] = { "name" : localeInfo.EMOTION_LETTER, "target" : 0 }
			EMOTION_DICT[EMOTION_CALL] = { "name" : localeInfo.EMOTION_CALL, "target" : 0 }
			EMOTION_DICT[EMOTION_CELEBRATION] = {"name": localeInfo.EMOTION_CELEBRATION, "target" : 0 }
			EMOTION_DICT[EMOTION_ALCOHOL] = { "name" : localeInfo.EMOTION_ALCOHOL, "target" : 0 }
			EMOTION_DICT[EMOTION_BUSY] = { "name" : localeInfo.EMOTION_BUSY, "target" : 0 }
			EMOTION_DICT[EMOTION_WHIRL] = { "name" : localeInfo.EMOTION_WHIRL, "target" : 0 }
