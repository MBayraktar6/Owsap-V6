import ui
import app
import localeInfo
import uiToolTip
import ranking
import colorInfo

class RankingBoardWindow(ui.ScriptWindow):

	if app.ENABLE_GUILD_DRAGONLAIR_PARTY_SYSTEM:
		RANK_1 = 0
		RANK_2 = 1
		RANK_3 = 2

	def __init__(self):
		self.M2JOBLIST = {
			0 : localeInfo.JOB_WARRIOR,
			1 : localeInfo.JOB_ASSASSIN,
			2 : localeInfo.JOB_SURA,
			3 : localeInfo.JOB_SHAMAN,
			4 : localeInfo.JOB_WOLFMAN,
		}

		self.SOLO_RANK_BOARD_NAME = {
			0 : ( "", "", "" ), # SOLO_RK_CATEGORY_BF_WEAK
			1 : ( "", "", "" ), # SOLO_RK_CATEGORY_BF_TOTAL
		}

		self.PARTY_RANK_BOARD_NAME = {
			0 : ( "", localeInfo.CLEAR_FLOOR, localeInfo.CLEAR_TIME), # 임시 데이터.
		}

		ui.ScriptWindow.__init__(self)

		self.curType = 0
		self.curCategory = 0
		self.ResultSlotList = {}
		self.ResutlSlotCoverList = {}
		self.MyResultSlotList = []
		self.MyResultSlotCoverList = []
		self.toolTipMemberName = uiToolTip.ToolTip()

		self.TitleBar = None
		self.ColumnNameRank = None
		self.ColumnNamePlayer = None
		self.ColumnNameRecord = None
		self.ColumnNameRecord2 = None
		self.ColumnNameUpdateTime = None

		if app.ENABLE_GUILD_DRAGONLAIR_PARTY_SYSTEM:
			self.ResultButtonRankList = []
			self.ToolTipButton = None
			self.ToolTip = None
			self.PARTY_RANK_BOARD_NAME[ranking.PARTY_RK_CATEGORY_GUILD_DRAGONLAIR_RED_ALL] = (localeInfo.GUILD_DRAGONLAIR_PARTY_ALL, localeInfo.GUILD_DRAGONLAIR_PARTY_TIME, localeInfo.GUILD_DRAGONLAIR_PARTY_MEMBER)
			self.PARTY_RANK_BOARD_NAME[ranking.PARTY_RK_CATEGORY_GUILD_DRAGONLAIR_RED_NOW_WEEK] = (localeInfo.GUILD_DRAGONLAIR_PARTY_NOW, localeInfo.GUILD_DRAGONLAIR_PARTY_TIME, localeInfo.GUILD_DRAGONLAIR_PARTY_MEMBER)
			self.PARTY_RANK_BOARD_NAME[ranking.PARTY_RK_CATEGORY_GUILD_DRAGONLAIR_RED_PAST_WEEK] = (localeInfo.GUILD_DRAGONLAIR_PARTY_PAST, localeInfo.GUILD_DRAGONLAIR_PARTY_TIME, localeInfo.GUILD_DRAGONLAIR_PARTY_MEMBER)

			self.PartyRankingSystemToolTipList = [
				localeInfo.GUILD_DRAGONLAIR_PARTY_TOOLTIP_TITLE,
				localeInfo.GUILD_DRAGONLAIR_PARTY_LINE_ONE,
				localeInfo.GUILD_DRAGONLAIR_PARTY_LINE_TWO,
				localeInfo.GUILD_DRAGONLAIR_PARTY_LINE_THREE,
				localeInfo.GUILD_DRAGONLAIR_PARTY_LINE_FOUR
			]

		if app.ENABLE_DEFENSE_WAVE:
			self.PARTY_RANK_BOARD_NAME[ranking.PARTY_RK_CATEGORY_DEFENSE_WAVE] = (localeInfo.MAP_DEFENSE_WAVE, localeInfo.GUILD_DRAGONLAIR_PARTY_TIME, localeInfo.GUILD_DRAGONLAIR_PARTY_MEMBER)

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

		self.curType = 0
		self.curCategory = 0
		self.ResultSlotList = {}
		self.ResutlSlotCoverList = {}
		self.MyResultSlotList = []

		if app.ENABLE_GUILD_DRAGONLAIR_PARTY_SYSTEM:
			self.ResultButtonRankList = []
			self.ToolTipButton = None
			self.ToolTip = None

	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/RankingBoardWindow.py")

			## 타이틀 및 컬럼이름
			self.TitleBar = self.GetChild("board")
			self.ColumnNameRank = self.GetChild("ColumnNameRank")
			self.ColumnNamePlayer = self.GetChild("ColumnNamePlayer")
			self.ColumnNameRecord = self.GetChild("ColumnNameRecord")
			self.ColumnNameRecord2 = self.GetChild("ColumnNameRecord2")
			self.ColumnNameUpdateTime = self.GetChild("ColumnNameUpdateTime")

			self.TitleBar.SetCloseEvent(ui.__mem_func__(self.Close))

		except:
			import exception
			exception.Abort("RankingBoardWindow.__LoadWindow.UIScript/RankingBoardWindow.py")

		self.__MakeRankingUI()

	def Open(self, type, category):

		self.curType = type
		self.curCategory = category

		self.__SetUIColumnName()
		self.RefreshRankingList()

		self.Show()

	def Show(self):
		ui.ScriptWindow.Show(self)

	def Close(self):
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def Destroy(self):
		self.Hide()

	def __SetUIColumnName(self):
		if self.curType == ranking.TYPE_RK_SOLO:
			self.TitleBar.SetTitleName(self.SOLO_RANK_BOARD_NAME[self.curCategory][0])
			self.ColumnNameRank.SetText(localeInfo.RANKING)
			self.ColumnNamePlayer.SetText(localeInfo.CHARACTER_NAME)
			self.ColumnNameRecord.SetText(self.SOLO_RANK_BOARD_NAME[self.curCategory][1])
			self.ColumnNameRecord2.SetText(self.SOLO_RANK_BOARD_NAME[self.curCategory][2])
			self.ColumnNameUpdateTime.SetText(localeInfo.UPDATE_TIME)
		elif self.curType == ranking.TYPE_RK_PARTY:
			self.TitleBar.SetTitleName(self.PARTY_RANK_BOARD_NAME[self.curCategory][0])
			self.ColumnNameRank.SetText(localeInfo.RANKING)
			self.ColumnNamePlayer.SetText(localeInfo.LEADER_NAME)
			self.ColumnNameRecord.SetText(self.PARTY_RANK_BOARD_NAME[self.curCategory][1])
			self.ColumnNameRecord2.SetText(self.PARTY_RANK_BOARD_NAME[self.curCategory][2])
			self.ColumnNameUpdateTime.SetText(localeInfo.UPDATE_TIME)

	def __MakeResultSlotUI(self, yPos):
		## 순위
		RankingSlotImage = ui.MakeImageBox(self, "d:/ymir work/ui/public/parameter_slot_00.sub", 23, yPos)
		RankingSlotImage.SetAlpha(0)
		RankingSlot = ui.MakeTextLine(RankingSlotImage)
		self.Children.append(RankingSlotImage)
		self.Children.append(RankingSlot)

		## 이름
		NameImage = ui.MakeImageBox(self, "d:/ymir work/ui/public/parameter_slot_04.sub", 77, yPos)
		NameImage.SetAlpha(0)
		NameSlot = ui.MakeTextLine(NameImage)
		self.Children.append(NameImage)
		self.Children.append(NameSlot)

		## 기록 1순위
		RecordSlotImage = ui.MakeImageBox(self, "d:/ymir work/ui/public/parameter_slot_00.sub", 205, yPos)
		RecordSlotImage.SetAlpha(0)
		RecordSlot = ui.MakeTextLine(RecordSlotImage)
		self.Children.append(RecordSlotImage)
		self.Children.append(RecordSlot)

		## 기록 2순위
		Record2SlotImage = ui.MakeImageBox(self, "d:/ymir work/ui/public/parameter_slot_00.sub", 270, yPos)
		Record2SlotImage.SetAlpha(0)
		Record2Slot = ui.MakeTextLine(Record2SlotImage)
		self.Children.append(Record2SlotImage)
		self.Children.append(Record2Slot)

		## 갱신시간
		UpdateTimeImage = ui.MakeImageBox(self, "d:/ymir work/ui/public/parameter_slot_01.sub", 357, yPos)
		UpdateTimeImage.SetAlpha(0)
		UpdateTimeSlot = ui.MakeTextLine(UpdateTimeImage)
		self.Children.append(UpdateTimeImage)
		self.Children.append(UpdateTimeSlot)

		## 아랍 포지션 변경
		if localeInfo.IsARABIC():
			RankingSlotImage.SetPosition(self.GetWidth() - (23) - RankingSlotImage.GetWidth(), yPos)
			NameImage.SetPosition(self.GetWidth() - (77) - NameImage.GetWidth(), yPos)
			RecordSlotImage.SetPosition(self.GetWidth() - (205) - RecordSlotImage.GetWidth(), yPos)
			Record2SlotImage.SetPosition(self.GetWidth() - (270) - Record2SlotImage.GetWidth(), yPos)
			UpdateTimeImage.SetPosition(self.GetWidth() - (357) - UpdateTimeImage.GetWidth(), yPos)

		rankingslotlist = []
		rankingslotlist.append(RankingSlot)
		rankingslotlist.append(NameSlot)
		rankingslotlist.append(RecordSlot)
		rankingslotlist.append(Record2Slot)
		rankingslotlist.append(UpdateTimeSlot)

		return rankingslotlist

	def __MakeResultSlotCoverUI(self, yPos):
		itemSlotCoverImage = ui.MakeImageBox(self,"d:/ymir work/ui/public/ranking_party_list.sub", 22, yPos)
		itemSlotCoverImage.SetAlpha(0.5)
		self.Children.append(itemSlotCoverImage)

		if localeInfo.IsARABIC():
			itemSlotCoverImage.LeftRightReverse()
			itemSlotCoverImage.SetPosition(self.GetWidth() - 22 - itemSlotCoverImage.GetWidth(),yPos)

		TempitemSlotCoverImage = []
		TempitemSlotCoverImage.append(itemSlotCoverImage)

		return TempitemSlotCoverImage

	def __MakeRankingUI(self):
		yPos = 345

		self.MyResultSlotList = self.__MakeResultSlotUI(yPos)
		self.MyResultSlotCoverList = self.__MakeResultSlotCoverUI(yPos)
		self.MyResultSlotCoverList[0].SetEvent(ui.__mem_func__(self.MyEventProgress), "mouse_over_in")
		self.MyResultSlotCoverList[0].SetEvent(ui.__mem_func__(self.MyEventProgress), "mouse_over_out")

		for i in range(0,10):
			yPos = 84 + i * 25
			self.ResultSlotList[i] = self.__MakeResultSlotUI(yPos)
			self.ResutlSlotCoverList[i] = self.__MakeResultSlotCoverUI(yPos)
			self.ResutlSlotCoverList[i][0].SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_in", i)
			self.ResutlSlotCoverList[i][0].SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_out", i)

			if app.ENABLE_GUILD_DRAGONLAIR_PARTY_SYSTEM:
				if i == self.RANK_1:
					RankingRoundImage = ui.MakeImageBox(self, "d:/ymir work/ui/public/ranking_party_gold.sub", 20, yPos - 1)
					if localeInfo.IsARABIC():
						RankingRoundImage.SetPosition(self.GetWidth() - 20 - RankingRoundImage.GetWidth(), yPos - 1)
					RankingRoundImage.Hide()
					RankingRoundImage.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_in", i)
					RankingRoundImage.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_out", i)
					self.Children.append(RankingRoundImage)
					self.ResultButtonRankList.append(RankingRoundImage)
				elif i == self.RANK_2:
					RankingRoundImage = ui.MakeImageBox(self, "d:/ymir work/ui/public/ranking_party_silver.sub", 20, yPos - 1)
					if localeInfo.IsARABIC():
						RankingRoundImage.SetPosition(self.GetWidth() - 20 - RankingRoundImage.GetWidth(), yPos - 1)
					RankingRoundImage.Hide()
					RankingRoundImage.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_in", i)
					RankingRoundImage.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_out", i)
					self.Children.append(RankingRoundImage)
					self.ResultButtonRankList.append(RankingRoundImage)
				elif i == self.RANK_3:
					RankingRoundImage = ui.MakeImageBox(self, "d:/ymir work/ui/public/ranking_party_brown.sub", 20, yPos - 1)
					if localeInfo.IsARABIC():
						RankingRoundImage.SetPosition(self.GetWidth() - 20 - RankingRoundImage.GetWidth(), yPos - 1)
					RankingRoundImage.Hide()
					RankingRoundImage.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_in", i)
					RankingRoundImage.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_out", i)
					self.Children.append(RankingRoundImage)
					self.ResultButtonRankList.append(RankingRoundImage)

		if app.ENABLE_GUILD_DRAGONLAIR_PARTY_SYSTEM:
			self.ToolTipButton = ui.MakeButton(self, 480 - 50, 8, "", "d:/ymir work/ui/pattern/", "q_mark_01.tga", "q_mark_02.tga", "q_mark_01.tga")
			self.ToolTipButton.Hide()
			self.ToolTip = self.__CreateGameTypeToolTip(localeInfo.GUILD_DRAGONLAIR_PARTY_LINE_HELP, self.PartyRankingSystemToolTipList)
			self.ToolTip.SetTop()
			self.ToolTipButton.SetToolTipWindow(self.ToolTip)
			if localeInfo.IsARABIC():
				self.ToolTipButton.SetPosition(34, 8)

	if app.ENABLE_GUILD_DRAGONLAIR_PARTY_SYSTEM:
		def __CreateGameTypeToolTip(self, title, descList):
			toolTip = uiToolTip.ToolTip()
			toolTip.SetTitle(title)
			toolTip.AppendSpace(5)

			for desc in descList:
				toolTip.AutoAppendTextLine(desc)

			toolTip.AlignHorizonalCenter()
			toolTip.SetTop()
			return toolTip

	def RefreshRankingList(self):
		Getinfo = None
		GetMyinfo = None

		if self.curType == ranking.TYPE_RK_SOLO:
			GetInfo = ranking.GetHighRankingInfoSolo
			GetMyinfo = ranking.GetMyRankingInfoSolo
		elif self.curType == ranking.TYPE_RK_PARTY:
			GetInfo = ranking.GetHighRankingInfoParty
			GetMyinfo = ranking.GetMyRankingInfoParty

		for line, ResultSlotList in self.ResultSlotList.items():
			(charname, record0, record1, time, empire) = GetInfo(self.curCategory, line)
			if "" == charname:
				ResultSlotList[0].SetText("")
				ResultSlotList[1].SetText("")
				ResultSlotList[2].SetText("")
				ResultSlotList[3].SetText("")
				ResultSlotList[4].SetText("")
				self.ResutlSlotCoverList[line][0].Hide()

				if app.ENABLE_GUILD_DRAGONLAIR_PARTY_SYSTEM:
					if line == self.RANK_1:
						self.ResultButtonRankList[self.RANK_1].Hide()
					elif line == self.RANK_2:
						self.ResultButtonRankList[self.RANK_2].Hide()
					elif line == self.RANK_3:
						self.ResultButtonRankList[self.RANK_3].Hide()
			else:
				ResultSlotList[0].SetText(str((line + 1)))
				ResultSlotList[1].SetText(charname)

				if app.ENABLE_GUILD_DRAGONLAIR_PARTY_SYSTEM and self.curType == ranking.TYPE_RK_PARTY and self.curCategory >= ranking.PARTY_RK_CATEGORY_GUILD_DRAGONLAIR_RED_ALL and self.curCategory <= ranking.PARTY_RK_CATEGORY_GUILD_DRAGONLAIR_RED_PAST_WEEK:
					ResultSlotList[2].SetText(localeInfo.SecondToColonTypeHMS(record0))
					ResultSlotList[3].SetText(str(record1))
				elif app.ENABLE_DEFENSE_WAVE and self.curType == ranking.TYPE_RK_PARTY and self.curCategory == ranking.PARTY_RK_CATEGORY_DEFENSE_WAVE:
					ResultSlotList[2].SetText(localeInfo.SecondToColonTypeHMS(record0))
					ResultSlotList[3].SetText(str(record1))
				else:
					ResultSlotList[2].SetText(str(record0))
					ResultSlotList[3].SetText(str(record1))

				ResultSlotList[4].SetText(app.GetTimeString(time))
				self.ResutlSlotCoverList[line][0].Show()

				if app.ENABLE_GUILD_DRAGONLAIR_PARTY_SYSTEM:
					if line == self.RANK_1:
						self.ResultButtonRankList[self.RANK_1].Show()
					elif line == self.RANK_2:
						self.ResultButtonRankList[self.RANK_2].Show()
					elif line == self.RANK_3:
						self.ResultButtonRankList[self.RANK_3].Show()

		if app.ENABLE_GUILD_DRAGONLAIR_PARTY_SYSTEM:
			if self.curType == ranking.TYPE_RK_PARTY and self.curCategory >= ranking.PARTY_RK_CATEGORY_GUILD_DRAGONLAIR_RED_ALL and self.curCategory <= ranking.PARTY_RK_CATEGORY_GUILD_DRAGONLAIR_RED_PAST_WEEK:
				self.ToolTipButton.Show()
			else:
				self.ToolTipButton.Hide()

		(rankingIdx, mycharname, myrecord0, myrecord1, mytime, myempire) = GetMyinfo(self.curCategory)
		rankingStr = ""

		if rankingIdx > 0:
			rankingStr = str(rankingIdx)

		if "" == mycharname:
			self.MyResultSlotList[0].SetText("")
			self.MyResultSlotList[1].SetText("")
			self.MyResultSlotList[2].SetText("")
			self.MyResultSlotList[3].SetText("")
			self.MyResultSlotList[4].SetText("")
			self.MyResultSlotCoverList[0].Hide()
		else:
			self.MyResultSlotList[0].SetText(rankingStr)
			self.MyResultSlotList[1].SetText(mycharname)

			if app.ENABLE_GUILD_DRAGONLAIR_PARTY_SYSTEM and self.curType == ranking.TYPE_RK_PARTY and self.curCategory >= ranking.PARTY_RK_CATEGORY_GUILD_DRAGONLAIR_RED_ALL and self.curCategory <= ranking.PARTY_RK_CATEGORY_GUILD_DRAGONLAIR_RED_PAST_WEEK:
				self.MyResultSlotList[2].SetText(localeInfo.SecondToColonTypeHMS(myrecord0))
				self.MyResultSlotList[3].SetText(str(myrecord1))
			elif app.ENABLE_DEFENSE_WAVE and self.curType == ranking.TYPE_RK_PARTY and self.curCategory == ranking.PARTY_RK_CATEGORY_DEFENSE_WAVE:
				self.MyResultSlotList[2].SetText(localeInfo.SecondToColonTypeHMS(myrecord0))
				self.MyResultSlotList[3].SetText(str(myrecord1))
			else:
				self.MyResultSlotList[2].SetText(str(myrecord0))
				self.MyResultSlotList[3].SetText(str(myrecord1))

			self.MyResultSlotList[4].SetText(app.GetTimeString(mytime))
			self.MyResultSlotCoverList[0].Show()

	def EventProgress(self, event_type, slot):
		self.toolTipMemberName.ClearToolTip()
		if "mouse_over_in" == event_type:
			self.ResutlSlotCoverList[slot][0].SetAlpha(1)

			# 파티일때만 마우스 오버시 멤버명을 표시해준다.
			if self.curType == ranking.TYPE_RK_PARTY:
				membername = ranking.GetPartyMemberName(self.curCategory, slot)
				if membername != "":
					tooltipTitleColor = ui.GenerateColor(colorInfo.CHAT_RGB_PARTY[0], colorInfo.CHAT_RGB_PARTY[1], colorInfo.CHAT_RGB_PARTY[2])
					self.toolTipMemberName.AutoAppendTextLine(localeInfo.RANKING_BOARD_MEMBER, tooltipTitleColor)
					self.toolTipMemberName.AutoAppendTextLine(membername)
					self.toolTipMemberName.AlignHorizonalCenter()
					self.toolTipMemberName.ShowToolTip()

		elif "mouse_over_out" == event_type:
			self.ResutlSlotCoverList[slot][0].SetAlpha(0.5)
			self.toolTipMemberName.HideToolTip()

		else:
			print " uiRankingBoard.py ::EventProgress : FALSE"

	def MyEventProgress(self, event_type):
		self.toolTipMemberName.ClearToolTip()
		if "mouse_over_in" == event_type:
			self.MyResultSlotCoverList[0].SetAlpha(1)

			# 파티일때만 마우스 오버시 멤버명을 표시해준다.
			if self.curType == ranking.TYPE_RK_PARTY:
				# 나의 파티원 정보 가져오기.
				membername = ranking.GetMyPartyMemberName(self.curCategory)
				if membername != "":
					tooltipTitleColor = ui.GenerateColor(colorInfo.CHAT_RGB_PARTY[0], colorInfo.CHAT_RGB_PARTY[1], colorInfo.CHAT_RGB_PARTY[2])
					self.toolTipMemberName.AutoAppendTextLine(localeInfo.RANKING_BOARD_MEMBER, tooltipTitleColor)
					self.toolTipMemberName.AutoAppendTextLine(membername)
					self.toolTipMemberName.AlignHorizonalCenter()
					self.toolTipMemberName.ShowToolTip()

		elif "mouse_over_out" == event_type:
			self.MyResultSlotCoverList[0].SetAlpha(0.5)
			self.toolTipMemberName.HideToolTip()

		else:
			print " uiRankingBoard.py ::MyEventProgress : FALSE"
