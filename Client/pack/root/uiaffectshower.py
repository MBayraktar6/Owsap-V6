import ui
import localeInfo
import chr
import item
import app
import skill
import player
import uiToolTip
import math

if app.ENABLE_SET_ITEM:
	import uiToolTip

AFFECT_TOOLTIP_POS_X = 0
AFFECT_TOOLTIP_POS_Y = 40

# WEDDING
class LovePointImage(ui.ExpandedImageBox):

	FILE_PATH = "d:/ymir work/ui/pattern/LovePoint/"
	FILE_DICT = {
		0 : FILE_PATH + "01.dds",
		1 : FILE_PATH + "02.dds",
		2 : FILE_PATH + "02.dds",
		3 : FILE_PATH + "03.dds",
		4 : FILE_PATH + "04.dds",
		5 : FILE_PATH + "05.dds",
	}

	def __init__(self):
		ui.ExpandedImageBox.__init__(self)

		self.loverName = ""
		self.lovePoint = 0

		self.toolTip = uiToolTip.ToolTip(100)
		self.toolTip.HideToolTip()

	def __del__(self):
		ui.ExpandedImageBox.__del__(self)

	def SetLoverInfo(self, name, lovePoint):
		self.loverName = name
		self.lovePoint = lovePoint
		self.__Refresh()

	def OnUpdateLovePoint(self, lovePoint):
		self.lovePoint = lovePoint
		self.__Refresh()

	def __Refresh(self):
		self.lovePoint = max(0, self.lovePoint)
		self.lovePoint = min(100, self.lovePoint)

		if 0 == self.lovePoint:
			loveGrade = 0
		else:
			loveGrade = self.lovePoint / 25 + 1
		fileName = self.FILE_DICT.get(loveGrade, self.FILE_PATH + "00.dds")

		try:
			self.LoadImage(fileName)
		except:
			import dbg
			dbg.TraceError("LovePointImage.SetLoverInfo(lovePoint=%d) - LoadError %s" % (self.lovePoint, fileName))

		self.SetScale(0.7, 0.7)

		self.toolTip.ClearToolTip()
		self.toolTip.SetTitle(self.loverName)
		self.toolTip.AppendTextLine(localeInfo.AFF_LOVE_POINT % (self.lovePoint))
		self.toolTip.ResizeToolTip()

	def OnMouseOverIn(self):
		self.toolTip.ShowToolTip()

	def OnMouseOverOut(self):
		self.toolTip.HideToolTip()
# END_OF_WEDDING

if app.ENABLE_CONQUEROR_LEVEL:
	class SungMaAffectImage(ui.ExpandedImageBox):
		def __init__(self, str, hp, move, immune):
			ui.ExpandedImageBox.__init__(self)

			self.sungmaStr = str
			self.sungmaHp = hp
			self.sungmaMove = move
			self.sungmaImmune = immune

			self.toolTip = uiToolTip.ToolTip(100)
			self.toolTip.HideToolTip()

		def SetSungmaValue(self, str, hp, move, immune):
			self.sungmaStr = str
			self.sungmaHp = hp
			self.sungmaMove = move
			self.sungmaImmune = immune

			self.Refresh()

		def Refresh(self):
			fileName = "d:/ymir work/ui/skill/common/affect/sungma_buff.sub"
			try:
				self.LoadImage(fileName)
			except:
				import dbg
				dbg.TraceError("SungMaAffectImage.SetSungmaValue- LoadError %s" % (fileName))

			self.SetScale(0.7, 0.7)

			self.toolTip.ClearToolTip()

			self.toolTip.AppendSpace(5)
			self.toolTip.AutoAppendTextLine(localeInfo.TOOLTIP_AFFECT_SUNGMA_DESC, self.toolTip.TITLE_COLOR, False)
			self.toolTip.AutoAppendTextLine(localeInfo.TOOLTIP_AFFECT_SUNGMA_STR % (self.sungmaStr), self.toolTip.FONT_COLOR, False)
			self.toolTip.AutoAppendTextLine(localeInfo.TOOLTIP_AFFECT_SUNGMA_HP % (self.sungmaHp), self.toolTip.FONT_COLOR, False)
			self.toolTip.AutoAppendTextLine(localeInfo.TOOLTIP_AFFECT_SUNGMA_MOVE % (self.sungmaMove), self.toolTip.FONT_COLOR, False)
			self.toolTip.AutoAppendTextLine(localeInfo.TOOLTIP_AFFECT_SUNGMA_IMMUNE % (self.sungmaImmune), self.toolTip.FONT_COLOR, False)
			self.toolTip.ResizeToolTip()

		def OnMouseOverIn(self):
			self.toolTip.ShowToolTip()

		def OnMouseOverOut(self):
			self.toolTip.HideToolTip()

class HorseImage(ui.ExpandedImageBox):

	FILE_PATH = "d:/ymir work/ui/pattern/HorseState/"
	FILE_DICT = {
		00 : FILE_PATH + "00.dds",
		01 : FILE_PATH + "00.dds",
		02 : FILE_PATH + "00.dds",
		03 : FILE_PATH + "00.dds",
		10 : FILE_PATH + "10.dds",
		11 : FILE_PATH + "11.dds",
		12 : FILE_PATH + "12.dds",
		13 : FILE_PATH + "13.dds",
		20 : FILE_PATH + "20.dds",
		21 : FILE_PATH + "21.dds",
		22 : FILE_PATH + "22.dds",
		23 : FILE_PATH + "23.dds",
		30 : FILE_PATH + "30.dds",
		31 : FILE_PATH + "31.dds",
		32 : FILE_PATH + "32.dds",
		33 : FILE_PATH + "33.dds",
	}

	def __init__(self):
		ui.ExpandedImageBox.__init__(self)

		#self.textLineList = []
		self.toolTip = uiToolTip.ToolTip(100)
		self.toolTip.HideToolTip()

	def __del__(self):
		ui.ExpandedImageBox.__del__(self)

	def __GetHorseGrade(self, level):
		if 0 == level:
			return 0

		return (level - 1) / 10 + 1

	def SetState(self, level, health, battery):
		# self.textLineList = []
		self.toolTip.ClearToolTip()

		if level > 0:
			try:
				grade = self.__GetHorseGrade(level)
				self.__AppendText(localeInfo.LEVEL_LIST[grade])
			except IndexError:
				print "HorseImage.SetState(level=%d, health=%d, battery=%d) - Unknown Index" % (level, health, battery)
				return

			try:
				healthName = localeInfo.HEALTH_LIST[health]
				if len(healthName) > 0:
					self.__AppendText(healthName)
			except IndexError:
				print "HorseImage.SetState(level=%d, health=%d, battery=%d) - Unknown Index" % (level, health, battery)
				return

			if health > 0:
				if battery == 0:
					self.__AppendText(localeInfo.NEEFD_REST)

			try:
				fileName = self.FILE_DICT[health * 10 + battery]
			except KeyError:
				print "HorseImage.SetState(level=%d, health=%d, battery=%d) - KeyError" % (level, health, battery)

			try:
				self.LoadImage(fileName)
			except:
				print "HorseImage.SetState(level=%d, health=%d, battery=%d) - LoadError %s" % (level, health, battery, fileName)

		self.SetScale(0.7, 0.7)

	def __AppendText(self, text):
		self.toolTip.AppendTextLine(text)
		self.toolTip.ResizeToolTip()

		#x = self.GetWidth()/2
		#textLine = ui.TextLine()
		#textLine.SetParent(self)
		#textLine.SetSize(0, 0)
		#textLine.SetOutline()
		#textLine.Hide()
		#textLine.SetPosition(x, 40 + len(self.textLineList) * 16)
		#textLine.SetText(text)
		#self.textLineList.append(textLine)

	def OnMouseOverIn(self):
		#for textLine in self.textLineList:
		#	textLine.Show()

		self.toolTip.ShowToolTip()

	def OnMouseOverOut(self):
		#for textLine in self.textLineList:
		#	textLine.Hide()

		self.toolTip.HideToolTip()

class AffectImage(ui.ExpandedImageBox):
	def __init__(self):
		ui.ExpandedImageBox.__init__(self)

		if app.ENABLE_AFFECT_RENEWAL:
			self.tool_tip = uiToolTip.ToolTip(100)
			self.tool_tip.HideToolTip()
			self.tool_tip_text = ""
		else:
			self.toolTipText = None

		self.isSkillAffect = True
		self.description = None
		self.endTime = 0
		self.affect = None
		self.isClocked = True

		if app.ENABLE_SET_ITEM:
			self.tooltipItem = uiToolTip.ItemToolTip()
			self.tooltipItem.Hide()

		if app.ENABLE_SET_ITEM or app.ENABLE_SNOWFLAKE_STICK_EVENT:
			self.affect_dict = {}

	if app.ENABLE_SET_ITEM or app.ENABLE_AFFECT_RENEWAL:
		def __del__(self):
			ui.ExpandedImageBox.__del__(self)
			if app.ENABLE_SET_ITEM:
				del self.tooltipItem

			if app.ENABLE_AFFECT_RENEWAL:
				del self.tool_tip

			if app.ENABLE_SET_ITEM or app.ENABLE_SNOWFLAKE_STICK_EVENT:
				self.affect_dict = {}

	def SetAffect(self, affect):
		self.affect = affect

	def GetAffect(self):
		return self.affect

	if app.ENABLE_AFFECT_RENEWAL:
		def SetToolTipText(self, text, x = 0, y = -19, adjust_line_height = False, line_height_distance = 20):
			self.tool_tip_text = text

	elif app.ENABLE_SET_ITEM:
		def SetToolTipText(self, text, x = 0, y = -19, adjust_line_height = False, line_height_distance = 20):
			if not self.toolTipText:
				textLine = ui.TextLine()
				textLine.SetParent(self)
				textLine.SetSize(0, 0)
				textLine.SetOutline()
				textLine.Hide()
				self.toolTipText = textLine

				if adjust_line_height:
					line_height = self.toolTipText.GetLineHeight()
					self.toolTipText.SetLineHeight(line_height + line_height_distance)

			self.toolTipText.SetText(text)
			w, h = self.toolTipText.GetTextSize()
			if localeInfo.IsARABIC():
				self.toolTipText.SetPosition(w + 20, y)
			else:
				self.toolTipText.SetPosition(max(0, x + self.GetWidth() / 2 - w / 2), y)
	else:
		def SetToolTipText(self, text, x = 0, y = -19):
			if not self.toolTipText:
				textLine = ui.TextLine()
				textLine.SetParent(self)
				textLine.SetSize(0, 0)
				textLine.SetOutline()
				textLine.Hide()
				self.toolTipText = textLine

			self.toolTipText.SetText(text)
			w, h = self.toolTipText.GetTextSize()
			if localeInfo.IsARABIC():
				self.toolTipText.SetPosition(w + 20, y)
			else:
				self.toolTipText.SetPosition(max(0, x + self.GetWidth() / 2 - w / 2), y)

	def SetDescription(self, description):
		self.description = description

	def SetDuration(self, duration):
		self.endTime = 0
		if duration > 0:
			self.endTime = app.GetGlobalTimeStamp() + duration

	def UpdateAutoPotionDescription(self):
		potionType = 0
		if self.affect == chr.NEW_AFFECT_AUTO_HP_RECOVERY:
			potionType = player.AUTO_POTION_TYPE_HP
		else:
			potionType = player.AUTO_POTION_TYPE_SP

		potionWindow, potionSlot = player.GetAutoPotionInfo(potionType)
		metinSocket = [player.GetItemMetinSocket(potionWindow, potionSlot, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]

		currentAmount = int(metinSocket[1])
		totalAmount = int(metinSocket[2])

		amountPercent = 0.0
		try:
			amountPercent = 100.0 - (float(currentAmount) / totalAmount) * 100.0
		except:
			amountPercent = 100.0

		self.SetToolTipText(self.description % amountPercent, AFFECT_TOOLTIP_POS_X, AFFECT_TOOLTIP_POS_Y)

	def SetClock(self, isClocked):
		self.isClocked = isClocked

	def UpdateDescription(self):
		if not self.isClocked:
			self.__UpdateDescription2()
			return

		if not self.description:
			return

		toolTip = self.description
		if self.endTime > 0:
			leftTime = localeInfo.SecondToDHM(self.endTime - app.GetGlobalTimeStamp())
			toolTip += " (%s : %s)" % (localeInfo.LEFT_TIME, leftTime)
		self.SetToolTipText(toolTip, AFFECT_TOOLTIP_POS_X, AFFECT_TOOLTIP_POS_Y)

	# 독일버전에서 시간을 제거하기 위해서 사용
	def __UpdateDescription2(self):
		if not self.description:
			return

		toolTip = self.description
		self.SetToolTipText(toolTip, AFFECT_TOOLTIP_POS_X, AFFECT_TOOLTIP_POS_Y)

	if app.ENABLE_SET_ITEM:
		def UpdateSetItemDescription(self, affect_type = chr.NEW_AFFECT_SET_ITEM):
			if not self.description:
				return

			if not self.tooltipItem:
				toolTip = self.description
			else:
				toolTip = self.description
				toolTip += "\\n"

				self.affect_dict = player.GetSetItemEffect(affect_type)

				if type(self.affect_dict) is dict:
					if len(self.affect_dict) > 0:
						for k in self.affect_dict.keys():
							toolTip += self.tooltipItem.GetAffectString(k, self.affect_dict[k])
							toolTip += "\\n"

			self.SetToolTipText(toolTip, AFFECT_TOOLTIP_POS_X, AFFECT_TOOLTIP_POS_Y, True, 15)

	def SetSkillAffectFlag(self, flag):
		self.isSkillAffect = flag

	def IsSkillAffect(self):
		return self.isSkillAffect

	def OnMouseOverIn(self):
		if app.ENABLE_AFFECT_RENEWAL:
			if not self.tool_tip:
				return

			self.tool_tip.ClearToolTip()

			lines = self.tool_tip_text.split("\\n")
			if not lines:
				return

			for line in lines:
				self.tool_tip.AutoAppendTextLine(line)

			self.tool_tip.AlignHorizonalCenter()
			self.tool_tip.ShowToolTip()
		else:
			if self.toolTipText:
				self.toolTipText.Show()

	def OnMouseOverOut(self):
		if app.ENABLE_AFFECT_RENEWAL:
			if self.tool_tip:
				self.tool_tip.HideToolTip()
		else:
			if self.toolTipText:
				self.toolTipText.Hide()

	if app.ENABLE_SNOWFLAKE_STICK_EVENT:
		def UpdateSnowflakeStickEventRankBuffDescription(self):
			return

		def UpdateSnowflakeStickEventSnowflakeBuffDescription(self, update_clock = False):
			if not self.description:
				return

			if not self.tooltipItem:
				toolTip = self.description
				if self.endTime > 0:
					leftTime = localeInfo.SecondToDHM(self.endTime - app.GetGlobalTimeStamp())
					toolTip += " (%s : %s)" % (localeInfo.LEFT_TIME, leftTime)
			else:
				toolTip = self.description
				if self.endTime > 0:
					leftTime = localeInfo.SecondToDHM(self.endTime - app.GetGlobalTimeStamp())
					toolTip += " (%s : %s)" % (localeInfo.LEFT_TIME, leftTime)

				toolTip += "\\n"
				toolTip += localeInfo.TOOLTIP_AFFECT_SNOWFLAKE_STICK_EVENT_SNOWFLAKE_BUFF_2
				toolTip += "\\n"

				if not update_clock:
					self.affect_dict = player.GetAffectData(self.affect)

				if type(self.affect_dict) is dict and self.affect_dict:
					if len(self.affect_dict) > 0:
						for k in self.affect_dict.keys():
							toolTip += self.tooltipItem.GetAffectString(k, self.affect_dict[k])
							toolTip += "\\n"

			self.SetToolTipText(toolTip, AFFECT_TOOLTIP_POS_X, AFFECT_TOOLTIP_POS_Y, True, 15)

class AffectShower(ui.Window):

	MALL_DESC_IDX_START = 1000
	IMAGE_STEP = 25
	AFFECT_MAX_NUM = 32

	INFINITE_AFFECT_DURATION = 0x1FFFFFFF

	if app.ENABLE_AFFECT_RENEWAL:
		BASE_POS_X = 10
		BASE_POS_Y = 10

	def __init__(self):
		ui.Window.__init__(self)

		self.AFFECT_DATA_DICT = {
			chr.AFFECT_POISON : (localeInfo.SKILL_TOXICDIE, "d:/ymir work/ui/skill/common/affect/poison.sub"),
			chr.AFFECT_BLEEDING : (localeInfo.SKILL_BLEEDING, "d:/ymir work/ui/skill/common/affect/poison.sub"),
			chr.AFFECT_SLOW : (localeInfo.SKILL_SLOW, "d:/ymir work/ui/skill/common/affect/slow.sub"),
			chr.AFFECT_STUN : (localeInfo.SKILL_STUN, "d:/ymir work/ui/skill/common/affect/stun.sub"),

			chr.AFFECT_ATT_SPEED_POTION : (localeInfo.SKILL_INC_ATKSPD, "d:/ymir work/ui/skill/common/affect/Increase_Attack_Speed.sub"),
			chr.AFFECT_MOV_SPEED_POTION : (localeInfo.SKILL_INC_MOVSPD, "d:/ymir work/ui/skill/common/affect/Increase_Move_Speed.sub"),
			chr.AFFECT_FISH_MIND : (localeInfo.SKILL_FISHMIND, "d:/ymir work/ui/skill/common/affect/fishmind.sub"),

			chr.AFFECT_JEONGWI : (localeInfo.SKILL_JEONGWI, "d:/ymir work/ui/skill/warrior/jeongwi_03.sub",),
			chr.AFFECT_GEOMGYEONG : (localeInfo.SKILL_GEOMGYEONG, "d:/ymir work/ui/skill/warrior/geomgyeong_03.sub",),
			chr.AFFECT_CHEONGEUN : (localeInfo.SKILL_CHEONGEUN, "d:/ymir work/ui/skill/warrior/cheongeun_03.sub",),
			chr.AFFECT_GYEONGGONG : (localeInfo.SKILL_GYEONGGONG, "d:/ymir work/ui/skill/assassin/gyeonggong_03.sub",),
			chr.AFFECT_EUNHYEONG : (localeInfo.SKILL_EUNHYEONG, "d:/ymir work/ui/skill/assassin/eunhyeong_03.sub",),
			chr.AFFECT_GWIGEOM : (localeInfo.SKILL_GWIGEOM, "d:/ymir work/ui/skill/sura/gwigeom_03.sub",),
			chr.AFFECT_GONGPO : (localeInfo.SKILL_GONGPO, "d:/ymir work/ui/skill/sura/gongpo_03.sub",),
			chr.AFFECT_JUMAGAP : (localeInfo.SKILL_JUMAGAP, "d:/ymir work/ui/skill/sura/jumagap_03.sub"),
			chr.AFFECT_HOSIN : (localeInfo.SKILL_HOSIN, "d:/ymir work/ui/skill/shaman/hosin_03.sub",),
			chr.AFFECT_BOHO : (localeInfo.SKILL_BOHO, "d:/ymir work/ui/skill/shaman/boho_03.sub",),
			chr.AFFECT_KWAESOK : (localeInfo.SKILL_KWAESOK, "d:/ymir work/ui/skill/shaman/kwaesok_03.sub",),
			chr.AFFECT_HEUKSIN : (localeInfo.SKILL_HEUKSIN, "d:/ymir work/ui/skill/sura/heuksin_03.sub",),
			chr.AFFECT_MUYEONG : (localeInfo.SKILL_MUYEONG, "d:/ymir work/ui/skill/sura/muyeong_03.sub",),
			chr.AFFECT_GICHEON : (localeInfo.SKILL_GICHEON, "d:/ymir work/ui/skill/shaman/gicheon_03.sub",),
			chr.AFFECT_JEUNGRYEOK : (localeInfo.SKILL_JEUNGRYEOK, "d:/ymir work/ui/skill/shaman/jeungryeok_03.sub",),
			chr.AFFECT_PABEOP : (localeInfo.SKILL_PABEOP, "d:/ymir work/ui/skill/sura/pabeop_03.sub",),
			chr.AFFECT_FALLEN_CHEONGEUN : (localeInfo.SKILL_CHEONGEUN, "d:/ymir work/ui/skill/warrior/cheongeun_03.sub",),
			28 : (localeInfo.SKILL_FIRE, "d:/ymir work/ui/skill/sura/hwayeom_03.sub",),
			chr.AFFECT_CHINA_FIREWORK : (localeInfo.SKILL_POWERFUL_STRIKE, "d:/ymir work/ui/skill/common/affect/powerfulstrike.sub",),
			chr.AFFECT_RED_POSSESSION : (localeInfo.SKILL_GWIGEOM, "d:/ymir work/ui/skill/wolfman/red_possession_03.sub",),
			chr.AFFECT_BLUE_POSSESSION : (localeInfo.SKILL_CHEONGEUN, "d:/ymir work/ui/skill/wolfman/blue_possession_03.sub",),

			#64 - END
			chr.NEW_AFFECT_EXP_BONUS : (localeInfo.TOOLTIP_MALL_EXPBONUS_STATIC, "d:/ymir work/ui/skill/common/affect/exp_bonus.sub",),

			chr.NEW_AFFECT_ITEM_BONUS : (localeInfo.TOOLTIP_MALL_ITEMBONUS_STATIC, "d:/ymir work/ui/skill/common/affect/item_bonus.sub",),
			chr.NEW_AFFECT_SAFEBOX : (localeInfo.TOOLTIP_MALL_SAFEBOX, "d:/ymir work/ui/skill/common/affect/safebox.sub",),
			chr.NEW_AFFECT_AUTOLOOT : (localeInfo.TOOLTIP_MALL_AUTOLOOT, "d:/ymir work/ui/skill/common/affect/autoloot.sub",),
			chr.NEW_AFFECT_FISH_MIND : (localeInfo.TOOLTIP_MALL_FISH_MIND, "d:/ymir work/ui/skill/common/affect/fishmind.sub",),
			chr.NEW_AFFECT_MARRIAGE_FAST : (localeInfo.TOOLTIP_MALL_MARRIAGE_FAST, "d:/ymir work/ui/skill/common/affect/marriage_fast.sub",),
			chr.NEW_AFFECT_GOLD_BONUS : (localeInfo.TOOLTIP_MALL_GOLDBONUS_STATIC, "d:/ymir work/ui/skill/common/affect/gold_bonus.sub",),

			chr.NEW_AFFECT_NO_DEATH_PENALTY : (localeInfo.TOOLTIP_APPLY_NO_DEATH_PENALTY, "d:/ymir work/ui/skill/common/affect/gold_premium.sub"),
			chr.NEW_AFFECT_SKILL_BOOK_BONUS : (localeInfo.TOOLTIP_APPLY_SKILL_BOOK_BONUS, "d:/ymir work/ui/skill/common/affect/gold_premium.sub"),
			chr.NEW_AFFECT_SKILL_BOOK_NO_DELAY : (localeInfo.TOOLTIP_APPLY_SKILL_BOOK_NO_DELAY, "d:/ymir work/ui/skill/common/affect/gold_premium.sub"),

			# 자동물약 hp, sp
			chr.NEW_AFFECT_AUTO_HP_RECOVERY : (localeInfo.TOOLTIP_AUTO_POTION_REST, "d:/ymir work/ui/pattern/auto_hpgauge/05.dds"),
			chr.NEW_AFFECT_AUTO_SP_RECOVERY : (localeInfo.TOOLTIP_AUTO_POTION_REST, "d:/ymir work/ui/pattern/auto_spgauge/05.dds"),
			#chr.NEW_AFFECT_AUTO_HP_RECOVERY : (localeInfo.TOOLTIP_AUTO_POTION_REST, "d:/ymir work/ui/skill/common/affect/gold_premium.sub"),
			#chr.NEW_AFFECT_AUTO_SP_RECOVERY : (localeInfo.TOOLTIP_AUTO_POTION_REST, "d:/ymir work/ui/skill/common/affect/gold_bonus.sub"),

			self.MALL_DESC_IDX_START + player.POINT_MALL_ATTBONUS : (localeInfo.TOOLTIP_MALL_ATTBONUS_STATIC, "d:/ymir work/ui/skill/common/affect/att_bonus.sub",),
			self.MALL_DESC_IDX_START + player.POINT_MALL_DEFBONUS : (localeInfo.TOOLTIP_MALL_DEFBONUS_STATIC, "d:/ymir work/ui/skill/common/affect/def_bonus.sub",),
			self.MALL_DESC_IDX_START + player.POINT_MALL_EXPBONUS : (localeInfo.TOOLTIP_MALL_EXPBONUS, "d:/ymir work/ui/skill/common/affect/exp_bonus.sub",),
			self.MALL_DESC_IDX_START + player.POINT_MALL_ITEMBONUS : (localeInfo.TOOLTIP_MALL_ITEMBONUS, "d:/ymir work/ui/skill/common/affect/item_bonus.sub",),
			self.MALL_DESC_IDX_START + player.POINT_MALL_GOLDBONUS : (localeInfo.TOOLTIP_MALL_GOLDBONUS, "d:/ymir work/ui/skill/common/affect/gold_bonus.sub",),
			self.MALL_DESC_IDX_START + player.POINT_CRITICAL_PCT : (localeInfo.TOOLTIP_APPLY_CRITICAL_PCT,"d:/ymir work/ui/skill/common/affect/critical.sub"),
			self.MALL_DESC_IDX_START + player.POINT_PENETRATE_PCT : (localeInfo.TOOLTIP_APPLY_PENETRATE_PCT, "d:/ymir work/ui/skill/common/affect/gold_premium.sub"),
			self.MALL_DESC_IDX_START + player.POINT_MAX_HP_PCT : (localeInfo.TOOLTIP_MAX_HP_PCT, "d:/ymir work/ui/skill/common/affect/gold_premium.sub"),
			self.MALL_DESC_IDX_START + player.POINT_MAX_SP_PCT : (localeInfo.TOOLTIP_MAX_SP_PCT, "d:/ymir work/ui/skill/common/affect/gold_premium.sub"),

			self.MALL_DESC_IDX_START + player.POINT_PC_BANG_EXP_BONUS : (localeInfo.TOOLTIP_MALL_EXPBONUS_P_STATIC, "d:/ymir work/ui/skill/common/affect/EXP_Bonus_p_on.sub",),
			self.MALL_DESC_IDX_START + player.POINT_PC_BANG_DROP_BONUS: (localeInfo.TOOLTIP_MALL_ITEMBONUS_P_STATIC, "d:/ymir work/ui/skill/common/affect/Item_Bonus_p_on.sub",),
			self.MALL_DESC_IDX_START + player.POINT_MELEE_MAGIC_ATT_BONUS_PER : (localeInfo.TOOLTIP_MALL_ATTBONUS_STATIC, "d:/ymir work/ui/skill/common/affect/att_bonus.sub",),
		}

		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			# 용혼석 천, 지 덱.
			self.AFFECT_DATA_DICT[chr.NEW_AFFECT_DRAGON_SOUL_DECK1] = (localeInfo.TOOLTIP_DRAGON_SOUL_DECK1, "d:/ymir work/ui/dragonsoul/buff_ds_sky1.tga")
			self.AFFECT_DATA_DICT[chr.NEW_AFFECT_DRAGON_SOUL_DECK2] = (localeInfo.TOOLTIP_DRAGON_SOUL_DECK2, "d:/ymir work/ui/dragonsoul/buff_ds_land1.tga")

		self.AFFECT_DATA_DICT[chr.NEW_AFFECT_POLYMORPH] = (localeInfo.TOOLTIP_AFFECT_POLYMORPH, "d:/ymir work/ui/game/monster_card/poly_affect.sub")

		if app.ENABLE_SET_ITEM:
			self.AFFECT_DATA_DICT[chr.NEW_AFFECT_SET_ITEM] = (localeInfo.TOOLTIP_SET_ITEM, "d:/ymir work/ui/skill/common/affect/set_bonus.sub")
			self.AFFECT_DATA_DICT[chr.NEW_AFFECT_SET_ITEM_SET_VALUE_1] = (localeInfo.TOOLTIP_AFFECT_SET_ITEM_1, "d:/ymir work/ui/skill/common/affect/set_bonus.sub")
			self.AFFECT_DATA_DICT[chr.NEW_AFFECT_SET_ITEM_SET_VALUE_2] = (localeInfo.TOOLTIP_AFFECT_SET_ITEM_2, "d:/ymir work/ui/skill/common/affect/set_bonus.sub")
			self.AFFECT_DATA_DICT[chr.NEW_AFFECT_SET_ITEM_SET_VALUE_3] = (localeInfo.TOOLTIP_AFFECT_SET_ITEM_3, "d:/ymir work/ui/skill/common/affect/set_bonus.sub")
			self.AFFECT_DATA_DICT[chr.NEW_AFFECT_SET_ITEM_SET_VALUE_4] = (localeInfo.TOOLTIP_AFFECT_SET_ITEM_4, "d:/ymir work/ui/skill/common/affect/set_bonus.sub")
			self.AFFECT_DATA_DICT[chr.NEW_AFFECT_SET_ITEM_SET_VALUE_5] = (localeInfo.TOOLTIP_AFFECT_SET_ITEM_5, "d:/ymir work/ui/skill/common/affect/set_bonus.sub")

		self.AFFECT_DATA_DICT[chr.NEW_AFFECT_MOUNT_FALL] = (localeInfo.TOOLTIP_AFFECT_MOUNT_FALL, "d:/ymir work/ui/skill/common/affect/mount_fall.sub")

		if app.ENABLE_FLOWER_EVENT:
			self.AFFECT_DATA_DICT[chr.NEW_AFFECT_FLOWER_EVENT] = ("", "d:/ymir work/ui/skill/common/affect/flower_event.sub")

		if app.ENABLE_DS_SET:
			self.AFFECT_DATA_DICT[chr.NEW_AFFECT_DS_SET] = (localeInfo.TOOLTIP_DS_SET, "d:/ymir work/ui/skill/common/affect/ds_set_bonus.sub")

		self.AFFECT_DATA_DICT[chr.NEW_AFFECT_RESEARCHER_ELIXIR] = (localeInfo.TOOLTIP_AFFECT_RESEARCHER_ELIXIR, "d:/ymir work/ui/skill/common/affect/researcher_elixir.sub")
		self.AFFECT_DATA_DICT[chr.NEW_AFFECT_SAFE_BOX_BUFF] = (localeInfo.TOOLTIP_AFFECT_SAFEBOX_BUFF_01, "d:/ymir work/ui/skill/common/affect/safebox_buff.sub")

		if app.ENABLE_FISHING_GAME:
			self.AFFECT_DATA_DICT[chr.AFFECT_FISHING_GOLD_TUNA] = (localeInfo.TOOLTIP_AFFECT_FISHING_GOLD_TUNA, "d:/ymir work/ui/skill/common/affect/fishing_gold_tuna_buff.sub")
			self.AFFECT_DATA_DICT[chr.AFFECT_FISHING_MOVE_SPEED_DOWN] = (localeInfo.TOOLTIP_AFFECT_FISHING_MOVE_SPEED_DOWN, "d:/ymir work/ui/skill/common/affect/fishing_move_speed_down.sub")

		if app.ENABLE_LOOTING_SYSTEM and app.ENABLE_PREMIUM_LOOTING:
			self.AFFECT_DATA_DICT[chr.NEW_AFFECT_LOOTING_SYSTEM] = (localeInfo.TOOLTIP_AFFECT_LOOTING_SYSTEM, "d:/ymir work/ui/skill/common/affect/looting_system.sub")

		if app.ENABLE_CONQUEROR_LEVEL:
			self.AFFECT_DATA_DICT[chr.NEW_AFFECT_SKILL_9_CHEONUN] = (localeInfo.TOOLTIP_AFFECT_CHEONUN, "d:/ymir work/ui/skill/shaman/cheonun_03.sub")
			self.AFFECT_DATA_DICT[chr.NEW_AFFECT_SUNGMA_BONUS] = (localeInfo.TOOLTIP_SUNGMA_ATTR_PRIMIUM, "d:/ymir work/ui/skill/common/affect/sungma_premium_buff.sub")

		if app.ENABLE_YUTNORI_EVENT_FLAG_RENEWAL:
			self.AFFECT_DATA_DICT[chr.NEW_AFFECT_HALLOWEEN_EVENT] = (localeInfo.TOOLTIP_HALLOWEEN_EVENT_BUFF, "d:/ymir work/ui/skill/common/affect/halloween_bonus.sub")

		if app.ENABLE_SNOWFLAKE_STICK_EVENT:
			self.AFFECT_DATA_DICT[chr.NEW_AFFECT_SNOWFLAKE_STICK_EVENT_RANK_BUFF] = (localeInfo.TOOLTIP_AFFECT_SNOWFLAKE_STICK_EVENT_RANK_BUFF, "d:/ymir work/ui/skill/common/affect/snowflake_stick_event_rank_buff.sub")
			self.AFFECT_DATA_DICT[chr.NEW_AFFECT_SNOWFLAKE_STICK_EVENT_SNOWFLAKE_BUFF] = (localeInfo.TOOLTIP_AFFECT_SNOWFLAKE_STICK_EVENT_SNOWFLAKE_BUFF_1, "d:/ymir work/ui/skill/common/affect/snowflake_stick_event_snowflake_buff.sub")

		if app.ENABLE_ELEMENTAL_DUNGEON:
			self.AFFECT_DATA_DICT[chr.NEW_AFFECT_CURSE_OF_ELEMENTAL] = (localeInfo.TOOLTIP_CURSE_OF_ELEMENTAL, "d:/ymir work/ui/skill/common/affect/curse_of_elemental.sub")
			self.AFFECT_DATA_DICT[chr.NEW_AFFECT_PROTECTION_OF_ELEMENTAL] = (localeInfo.TOOLTIP_PROTECTION_OF_ELEMENTAL, "d:/ymir work/ui/skill/common/affect/protection_of_elemental.sub")
			self.AFFECT_DATA_DICT[chr.NEW_AFFECT_POTION_OF_ELEMENTAL] = (localeInfo.TOOLTIP_POTION_OF_ELEMENTAL, "d:/ymir work/ui/skill/common/affect/potion_of_elemental.sub")

		if app.ENABLE_GUILD_DRAGONLAIR_SYSTEM:
			self.AFFECT_DATA_DICT[chr.NEW_AFFECT_RED_DRAGONLAIR_BUFF] = (localeInfo.TOOLTIP_AFFECT_RED_DRAGONLAIR_BUFF, "d:/ymir work/ui/skill/common/affect/def_grade_down.sub")

		if app.ENABLE_SUMMER_EVENT_ROULETTE:
			self.AFFECT_DATA_DICT[chr.NEW_AFFECT_LATE_SUMMER_EVENT_BUFF] = (localeInfo.TOOLTIP_AFFECT_LATE_SUMMER_EVENT_BUFF, "d:/ymir work/ui/skill/common/affect/late_summber_buff_affect.sub")
			self.AFFECT_DATA_DICT[chr.NEW_AFFECT_LATE_SUMMER_EVENT_PRIMIUM_BUFF] = (localeInfo.TOOLTIP_AFFECT_LATE_SUMMER_EVENT_PRIMIUM_BUFF, "d:/ymir work/ui/skill/common/affect/late_summber_primium_buff_affect.sub")

		if app.ENABLE_DEFENSE_WAVE:
			self.AFFECT_DATA_DICT[chr.NEW_AFFECT_DEFENSEWAVE_LASER] = (localeInfo.TOOLTIP_AFFECT_DEFENSEWAVE_LASER, "d:/ymir work/ui/skill/common/affect/laser.sub")

		self.serverPlayTime = 0
		self.clientPlayTime = 0

		self.lastUpdateTime = 0
		self.affectImageDict = {}
		self.horseImage = None
		self.lovePointImage = None

		if app.ENABLE_CONQUEROR_LEVEL:
			self.sungmaImage = None

		self.SetPosition(10, 10)
		self.Show()

		if app.ENABLE_SET_ITEM:
			self.SetItemAffectCheck()

	def __del__(self):
		ui.Window.__del__(self)

	if app.ENABLE_SET_ITEM:
		def SetItemAffectCheck(self):
			if player.EmptySetItemEffect(chr.NEW_AFFECT_SET_ITEM):
				self.BINARY_NEW_RemoveAffect(chr.NEW_AFFECT_SET_ITEM, 0)
			else:
				self.BINARY_NEW_RemoveAffect(chr.NEW_AFFECT_SET_ITEM, 0)
				self.BINARY_NEW_AddAffect(chr.NEW_AFFECT_SET_ITEM, 0, 0, self.INFINITE_AFFECT_DURATION)

			for affect in xrange(chr.NEW_AFFECT_SET_ITEM_SET_VALUE_1, chr.NEW_AFFECT_SET_ITEM_SET_VALUE_5 + 1):
				if player.EmptySetItemEffect(affect):
					self.BINARY_NEW_RemoveAffect(affect, 0)
				else:
					self.BINARY_NEW_RemoveAffect(affect, 0)
					self.BINARY_NEW_AddAffect(affect, 0, 0, self.INFINITE_AFFECT_DURATION)

	def ClearAllAffects(self):
		self.horseImage = None
		self.lovePointImage = None
		if app.ENABLE_CONQUEROR_LEVEL:
			self.sungmaImage = None
		self.affectImageDict = {}

		self.__ArrangeImageList()

	def ClearAffects(self): ## 스킬 이펙트만 없앱니다.
		self.living_affectImageDict = {}
		for key, image in self.affectImageDict.items():
			if not image.IsSkillAffect():
				self.living_affectImageDict[key] = image

		self.affectImageDict = self.living_affectImageDict
		self.__ArrangeImageList()

	def BINARY_NEW_AddAffect(self, type, pointIdx, value, duration):
		print "BINARY_NEW_AddAffect", type, pointIdx, value, duration

		if type < 500:
			pass_affect_list = []
			pass_affect_list.append(chr.NEW_AFFECT_POLYMORPH)
			if app.ENABLE_CONQUEROR_LEVEL:
				pass_affect_list.append(chr.NEW_AFFECT_SKILL_9_CHEONUN)

			if not type in pass_affect_list:
				return

		if type == chr.NEW_AFFECT_MALL:
			affect = self.MALL_DESC_IDX_START + pointIdx
		else:
			affect = type

		if self.affectImageDict.has_key(affect):
			if app.ENABLE_SET_ITEM and affect == chr.NEW_AFFECT_SET_ITEM or affect >= chr.NEW_AFFECT_SET_ITEM_SET_VALUE_1 and affect <= chr.NEW_AFFECT_SET_ITEM_SET_VALUE_5:
				self.affectImageDict[affect].UpdateSetItemDescription(affect)

			if app.ENABLE_SNOWFLAKE_STICK_EVENT and affect == chr.NEW_AFFECT_SNOWFLAKE_STICK_EVENT_SNOWFLAKE_BUFF:
				self.affectImageDict[affect].UpdateSnowflakeStickEventSnowflakeBuffDescription()

			return

		if not self.AFFECT_DATA_DICT.has_key(affect):
			return

		## 용신의 가호, 선인의 교훈은 Duration 을 0 으로 설정한다.
		if affect == chr.NEW_AFFECT_NO_DEATH_PENALTY or\
			affect == chr.NEW_AFFECT_SKILL_BOOK_BONUS or\
			affect == chr.NEW_AFFECT_AUTO_SP_RECOVERY or\
			affect == chr.NEW_AFFECT_AUTO_HP_RECOVERY or\
			affect == chr.NEW_AFFECT_SKILL_BOOK_NO_DELAY:
			duration = 0

		affectData = self.AFFECT_DATA_DICT[affect]
		description = affectData[0]
		filename = affectData[1]

		if pointIdx == player.POINT_MALL_ITEMBONUS or\
			pointIdx == player.POINT_MALL_GOLDBONUS:
			value = float(value) * 100.0 / 100.0 # 1 + float(value) / 100.0

		if affect == chr.NEW_AFFECT_SAFE_BOX_BUFF:
			if pointIdx == player.POINT_MALL_EXPBONUS:
				description = localeInfo.TOOLTIP_AFFECT_SAFEBOX_BUFF_01

			elif pointIdx == player.POINT_MALL_DEFBONUS:
				description = localeInfo.TOOLTIP_AFFECT_SAFEBOX_BUFF_02

			elif pointIdx == player.POINT_MAX_HP_PCT:
				description = localeInfo.TOOLTIP_AFFECT_SAFEBOX_BUFF_03

			elif pointIdx == player.POINT_ATTBONUS_MONSTER:
				description = localeInfo.TOOLTIP_AFFECT_SAFEBOX_BUFF_04

			else:
				description = affectData[0]

		if affect != chr.NEW_AFFECT_AUTO_SP_RECOVERY and affect != chr.NEW_AFFECT_AUTO_HP_RECOVERY:
			if app.ENABLE_FLOWER_EVENT:
				if affect == chr.NEW_AFFECT_FLOWER_EVENT:
					description = uiToolTip.ItemToolTip().GetAffectString(item.GetPointApply(pointIdx), float(value))
				elif value != 0:
					description = description(float(value))
			else:
				description = description(float(value))

		try:
			print "Add affect %s" % affect
			image = AffectImage()
			image.SetParent(self)
			image.LoadImage(filename)
			image.SetDescription(description)
			image.SetDuration(duration)
			image.SetAffect(affect)

			if app.ENABLE_SET_ITEM and affect == chr.NEW_AFFECT_SET_ITEM or affect >= chr.NEW_AFFECT_SET_ITEM_SET_VALUE_1 and affect <= chr.NEW_AFFECT_SET_ITEM_SET_VALUE_5:
				image.SetClock(False)
				image.UpdateSetItemDescription(affect)

			elif app.ENABLE_SNOWFLAKE_STICK_EVENT and affect == chr.NEW_AFFECT_SNOWFLAKE_STICK_EVENT_SNOWFLAKE_BUFF:
				image.SetClock(False)
				image.UpdateSnowflakeStickEventSnowflakeBuffDescription()

			elif affect == chr.NEW_AFFECT_EXP_BONUS_EURO_FREE or\
				affect == chr.NEW_AFFECT_EXP_BONUS_EURO_FREE_UNDER_15 or\
				self.INFINITE_AFFECT_DURATION < duration:
				image.SetClock(False)
				image.UpdateDescription()

			elif affect == chr.NEW_AFFECT_AUTO_SP_RECOVERY or affect == chr.NEW_AFFECT_AUTO_HP_RECOVERY:
				image.UpdateAutoPotionDescription()

			elif affect == chr.NEW_AFFECT_MOUNT_FALL:
				image.SetClock(False)
				image.UpdateDescription()

			elif affect == chr.NEW_AFFECT_RESEARCHER_ELIXIR:
				image.SetClock(False)
				image.UpdateDescription()

			else:
				image.UpdateDescription()

			if affect == chr.NEW_AFFECT_DRAGON_SOUL_DECK1 or affect == chr.NEW_AFFECT_DRAGON_SOUL_DECK2:
				image.SetScale(1, 1)
			else:
				image.SetScale(0.7, 0.7)

			image.SetSkillAffectFlag(False)
			image.Show()
			self.affectImageDict[affect] = image
			self.__ArrangeImageList()

		except Exception, e:
			print "except Aff affect ", e
			pass

	def BINARY_NEW_RemoveAffect(self, type, pointIdx):
		if type == chr.NEW_AFFECT_MALL:
			affect = self.MALL_DESC_IDX_START + pointIdx
		else:
			affect = type

		print "Remove Affect %s %s" % (type, pointIdx)
		self.__RemoveAffect(affect)
		self.__ArrangeImageList()

	def BINARY_NEW_UpdateAffect(self, type, point_type, value, duration):
		if type == chr.NEW_AFFECT_MALL:
			affect = self.MALL_DESC_IDX_START + pointIdx
		else:
			affect = type

		if not self.AFFECT_DATA_DICT.has_key(affect):
			print("BINARY_NEW_UpdateAffect type not found %d" % affect)
			return

		affectData = self.AFFECT_DATA_DICT[affect]
		description = affectData[0]

		if point_type == player.POINT_MALL_ITEMBONUS or\
			point_type == player.POINT_MALL_GOLDBONUS:
			value = float(value) * 100.0 / 100.0 # 1 + float(value) / 100.0

		if affect == chr.NEW_AFFECT_SAFE_BOX_BUFF:
			if point_type == player.POINT_MALL_EXPBONUS:
				description = localeInfo.TOOLTIP_AFFECT_SAFEBOX_BUFF_01
			elif point_type == player.POINT_MALL_DEFBONUS:
				description = localeInfo.TOOLTIP_AFFECT_SAFEBOX_BUFF_02
			elif point_type == player.POINT_MAX_HP_PCT:
				description = localeInfo.TOOLTIP_AFFECT_SAFEBOX_BUFF_03
			elif point_type == player.POINT_ATTBONUS_MONSTER:
				description = localeInfo.TOOLTIP_AFFECT_SAFEBOX_BUFF_04
			else:
				description = affectData[0]

		if self.affectImageDict.has_key(affect):
			image = self.affectImageDict[affect]

			if affect != chr.NEW_AFFECT_AUTO_SP_RECOVERY and affect != chr.NEW_AFFECT_AUTO_HP_RECOVERY:
				if app.ENABLE_FLOWER_EVENT:
					if affect == chr.NEW_AFFECT_FLOWER_EVENT:
						description = uiToolTip.ItemToolTip().GetAffectString(item.GetPointApply(point_type), float(value))
					else:
						description = description(float(value))
				else:
					description = description(float(value))

			image.SetDescription(description)
			image.SetDuration(duration)
			image.UpdateDescription()
		else:
			print("BINARY_NEW_UpdateAffect affectImageDict not found %d" % affect)

	def SetAffect(self, affect):
		self.__AppendAffect(affect)
		self.__ArrangeImageList()

	def ResetAffect(self, affect):
		self.__RemoveAffect(affect)
		self.__ArrangeImageList()

	def SetLoverInfo(self, name, lovePoint):
		image = LovePointImage()
		image.SetParent(self)
		image.SetLoverInfo(name, lovePoint)
		self.lovePointImage = image
		self.__ArrangeImageList()

	def ShowLoverState(self):
		if self.lovePointImage:
			self.lovePointImage.Show()
			self.__ArrangeImageList()

	def HideLoverState(self):
		if self.lovePointImage:
			self.lovePointImage.Hide()
			self.__ArrangeImageList()

	def ClearLoverState(self):
		self.lovePointImage = None
		self.__ArrangeImageList()

	def OnUpdateLovePoint(self, lovePoint):
		if self.lovePointImage:
			self.lovePointImage.OnUpdateLovePoint(lovePoint)

	def SetHorseState(self, level, health, battery):
		if level == 0:
			self.horseImage = None
		else:
			image = HorseImage()
			image.SetParent(self)
			image.SetState(level, health, battery)
			image.Show()

			self.horseImage = image

		self.__ArrangeImageList()

	def SetPlayTime(self, playTime):
		self.serverPlayTime = playTime
		self.clientPlayTime = app.GetTime()

		if localeInfo.IsVIETNAM():
			image = PlayTimeImage()
			image.SetParent(self)
			image.SetPlayTime(playTime)
			image.Show()

			self.playTimeImage=image
			self.__ArrangeImageList()

	if app.ENABLE_CONQUEROR_LEVEL:
		def SetSungMaAffectImage(self, str, hp, move, immune):
			image = SungMaAffectImage(str, hp, move, immune)
			image.SetParent(self)
			image.SetSungmaValue(str, hp, move, immune)
			image.Show()

			self.sungmaImage = image
			self.__ArrangeImageList()

	def __AppendAffect(self, affect):
		if self.affectImageDict.has_key(affect):
			return

		try:
			affectData = self.AFFECT_DATA_DICT[affect]
		except KeyError:
			return

		name = affectData[0]
		filename = affectData[1]

		skillIndex = player.AffectIndexToSkillIndex(affect)
		if 0 != skillIndex:
			name = skill.GetSkillName(skillIndex)

		image = AffectImage()
		image.SetParent(self)
		image.SetSkillAffectFlag(True)

		try:
			image.LoadImage(filename)
		except:
			pass

		image.SetToolTipText(name, AFFECT_TOOLTIP_POS_X, AFFECT_TOOLTIP_POS_Y)
		image.SetScale(0.7, 0.7)
		image.Show()
		self.affectImageDict[affect] = image

	def __RemoveAffect(self, affect):
		if not self.affectImageDict.has_key(affect):
			#print "__RemoveAffect %s ( No Affect )" % affect
			return

		#print "__RemoveAffect %s ( Affect )" % affect
		del self.affectImageDict[affect]

		self.__ArrangeImageList()

	def __ArrangeImageList(self):
		width = len(self.affectImageDict) * self.IMAGE_STEP

		if self.lovePointImage:
			width += self.IMAGE_STEP

		if self.horseImage:
			width += self.IMAGE_STEP

		if app.ENABLE_CONQUEROR_LEVEL:
			if self.sungmaImage:
				width += self.IMAGE_STEP

		self.SetSize(width, 26)

		xPos = 0

		if self.lovePointImage:
			if self.lovePointImage.IsShow():
				self.lovePointImage.SetPosition(xPos, 0)
				xPos += self.IMAGE_STEP

		if self.horseImage:
			self.horseImage.SetPosition(xPos, 0)
			xPos += self.IMAGE_STEP

		if app.ENABLE_CONQUEROR_LEVEL:
			if self.sungmaImage:
				self.sungmaImage.SetPosition(xPos, 0)
				xPos += self.IMAGE_STEP

		for image in self.affectImageDict.values():
			image.SetPosition(xPos, 0)
			xPos += self.IMAGE_STEP

	def OnUpdate(self):
		try:
			# MT-342 둔갑시 시간 표시 버그
			if app.GetGlobalTimeStamp() > self.lastUpdateTime:
			#if app.GetGlobalTime() - self.lastUpdateTime > 500:
			#if 0 < app.GetGlobalTime():
				# MT-342 둔갑시 시간 표시 버그
				#self.lastUpdateTime = app.GetGlobalTime()
				self.lastUpdateTime = app.GetGlobalTimeStamp()

				for image in self.affectImageDict.values():
					if image.GetAffect() == chr.NEW_AFFECT_AUTO_HP_RECOVERY or image.GetAffect() == chr.NEW_AFFECT_AUTO_SP_RECOVERY:
						image.UpdateAutoPotionDescription()
						continue

					if app.ENABLE_SET_ITEM:
						if image.GetAffect() == chr.NEW_AFFECT_SET_ITEM or image.GetAffect() >= chr.NEW_AFFECT_SET_ITEM_SET_VALUE_1 and image.GetAffect() <= chr.NEW_AFFECT_SET_ITEM_SET_VALUE_5:
							continue

					if app.ENABLE_SNOWFLAKE_STICK_EVENT:
						if image.GetAffect() == chr.NEW_AFFECT_SNOWFLAKE_STICK_EVENT_SNOWFLAKE_BUFF:
							image.UpdateSnowflakeStickEventSnowflakeBuffDescription(True)
							continue

					if not image.IsSkillAffect():
						image.UpdateDescription()

		except Exception, e:
			print "AffectShower::OnUpdate error : ", e
