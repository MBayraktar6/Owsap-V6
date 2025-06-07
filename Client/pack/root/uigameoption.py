import ui
import snd
import systemSetting
import net
import chat
import app
import localeInfo
import constInfo
import chrmgr
import player
import uiPrivateShopBuilder # ±Ë¡ÿ»£
import interfaceModule # ±Ë¡ÿ»£
if app.ENABLE_OPTIMIZATION:
	import wndMgr
	import uiScriptLocale
	import uiToolTip

if app.ENABLE_LEFT_SEAT:
	import os

blockMode = 0
viewChatMode = 0

class OptionDialog(ui.ScriptWindow):
	if app.ENABLE_OPTIMIZATION:
		ATTACKED_MOTION_SELF = 0
		ATTACKED_MOTION_ALL = 1

	if app.ENABLE_LEFT_SEAT:
		LEFT_SEAT_TIME_DATA = {
			player.LEFT_SEAT_TIME_10_MIN : uiScriptLocale.LEFT_SEAT_10_MIN,
			player.LEFT_SEAT_TIME_30_MIN : uiScriptLocale.LEFT_SEAT_30_MIN,
			player.LEFT_SEAT_TIME_90_MIN : uiScriptLocale.LEFT_SEAT_90_MIN,
		}

		LEFT_SEAT_LOGOUT_TIME_DATA = {
			player.LEFT_SEAT_LOGOUT_TIME_30_MIN : uiScriptLocale.LEFT_SEAT_30_MIN,
			player.LEFT_SEAT_LOGOUT_TIME_60_MIN : uiScriptLocale.LEFT_SEAT_60_MIN,
			player.LEFT_SEAT_LOGOUT_TIME_120_MIN : uiScriptLocale.LEFT_SEAT_120_MIN,
			player.LEFT_SEAT_LOGOUT_TIME_180_MIN : uiScriptLocale.LEFT_SEAT_180_MIN,
			player.LEFT_SEAT_LOGOUT_TIME_OFF : uiScriptLocale.LEFT_SEAT_OFF,
		}

		LEFT_SEAT_WAIT_TIME_INDEX = 0
		LEFT_SEAT_LOGOUT_WAIT_TIME_INDEX = 1
		LEFT_SEAT_WAIT_TIME_INDEX_MAX = 2

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__Initialize()
		self.__Load()

		self.RefreshViewChat()
		self.RefreshAlwaysShowName()
		self.RefreshShowDamage()
		if app.ENABLE_OPTIMIZATION:
			self.RefreshOtherCharAttacked()
		self.RefreshShowSalesText()
		if app.WJ_SHOW_MOB_INFO:
			self.RefreshShowMobInfo()

		if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
			self.RefreshAlwaysShowCountry()

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		#print " -------------------------------------- DELETE GAME OPTION DIALOG"

	def __Initialize(self):
		self.titleBar = 0
		self.nameColorModeButtonList = []
		self.viewTargetBoardButtonList = []
		self.pvpModeButtonDict = {}
		self.blockButtonList = []
		self.viewChatButtonList = []
		self.alwaysShowNameButtonList = []
		self.showDamageButtonList = []
		if app.ENABLE_OPTIMIZATION:
			self.other_char_attacked_button_list = []
		self.showsalesTextButtonList = []
		if app.WJ_SHOW_MOB_INFO:
			self.showMobInfoButtonList = []

		if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
			self.alwaysShowCountryButtonList = []

		if app.ENABLE_LOOTING_SYSTEM:
			self.interface = None

		if app.ENABLE_OPTIMIZATION:
			self.tooltip = uiToolTip.ToolTip()

		if app.ENABLE_LEFT_SEAT:
			self.is_load_left_seat_wait_time_data = False

			## left_seat_time
			self.left_seat_time_index = -1
			self.left_seat_time_list_window = None
			self.left_seat_list_time_arrow_button = None
			self.left_seat_time_list_button = None
			self.left_seat_list_time_mouse_over_image = None
			self.left_seat_time_list = []

			## left_seat_logout
			self.left_seat_logout_time_index = -1
			self.left_seat_logout_list_window = None
			self.left_seat_logout_list_arrow_button = None
			self.left_seat_logout_list_button = None
			self.left_seat_logout_list_mouse_over_image = None
			self.left_seat_logout_list = []

		self.IsShow = False
		if app.ENABLE_OPTIMIZATION:
			self.IsShowToolTip = False

	def Destroy(self):
		self.ClearDictionary()

		self.__Initialize()
		#print " -------------------------------------- DESTROY GAME OPTION DIALOG"

	def __Load_LoadScript(self, fileName):
		try:
			pyScriptLoader = ui.PythonScriptLoader()
			pyScriptLoader.LoadScriptFile(self, fileName)

		except:
			import exception
			exception.Abort("OptionDialog.__Load_LoadScript")

	def __Load_BindObject(self):
		try:
			GetObject = self.GetChild
			self.titleBar = GetObject("titlebar")

			self.nameColorModeButtonList.append(GetObject("name_color_normal"))
			self.nameColorModeButtonList.append(GetObject("name_color_empire"))

			self.viewTargetBoardButtonList.append(GetObject("target_board_no_view"))
			self.viewTargetBoardButtonList.append(GetObject("target_board_view"))

			self.pvpModeButtonDict[player.PK_MODE_PEACE] = GetObject("pvp_peace")
			self.pvpModeButtonDict[player.PK_MODE_REVENGE] = GetObject("pvp_revenge")
			self.pvpModeButtonDict[player.PK_MODE_GUILD] = GetObject("pvp_guild")
			self.pvpModeButtonDict[player.PK_MODE_FREE] = GetObject("pvp_free")

			self.blockButtonList.append(GetObject("block_exchange_button"))
			self.blockButtonList.append(GetObject("block_party_button"))
			self.blockButtonList.append(GetObject("block_guild_button"))
			self.blockButtonList.append(GetObject("block_whisper_button"))
			self.blockButtonList.append(GetObject("block_friend_button"))
			self.blockButtonList.append(GetObject("block_party_request_button"))

			self.viewChatButtonList.append(GetObject("view_chat_on_button"))
			self.viewChatButtonList.append(GetObject("view_chat_off_button"))

			self.alwaysShowNameButtonList.append(GetObject("always_show_name_on_button"))
			self.alwaysShowNameButtonList.append(GetObject("always_show_name_off_button"))

			self.showDamageButtonList.append(GetObject("show_damage_on_button"))
			self.showDamageButtonList.append(GetObject("show_damage_off_button"))

			if app.ENABLE_OPTIMIZATION:
				self.other_char_attacked_button_list.append(GetObject("other_char_attacked_all"))
				self.other_char_attacked_button_list.append(GetObject("other_char_attacked_self"))

			self.showsalesTextButtonList.append(GetObject("salestext_on_button"))
			self.showsalesTextButtonList.append(GetObject("salestext_off_button"))

			if app.WJ_SHOW_MOB_INFO:
				self.showMobInfoButtonList.append(GetObject("show_mob_level_button"))
				self.showMobInfoButtonList.append(GetObject("show_mob_AI_flag_button"))

			if app.ENABLE_KEYCHANGE_SYSTEM:
				GetObject("key_setting_show_button").SetEvent(ui.__mem_func__(self.OpenKeyChangeWindow))

			if app.ENABLE_LOOTING_SYSTEM:
				GetObject("looting_system_button").SetEvent(ui.__mem_func__(self.__OnClickLootingSystemButton))

			if app.ENABLE_LEFT_SEAT:
				## left_seat_time
				self.left_seat_time_list_window = GetObject("left_seat_time_list_window")
				self.left_seat_list_time_arrow_button = GetObject("left_seat_list_time_arrow_button")
				self.left_seat_time_list_button = GetObject("left_seat_time_list_button")
				self.left_seat_list_time_mouse_over_image = GetObject("left_seat_list_time_mouse_over_image")
				self.left_seat_time_list.append(GetObject("left_seat_time_10_min"))
				self.left_seat_time_list.append(GetObject("left_seat_time_30_min"))
				self.left_seat_time_list.append(GetObject("left_seat_time_90_min"))

				## left_seat_logout
				self.left_seat_logout_list_window = GetObject("left_seat_logout_list_window")
				self.left_seat_logout_list_arrow_button = GetObject("left_seat_logout_list_arrow_button")
				self.left_seat_logout_list_button = GetObject("left_seat_logout_list_button")
				self.left_seat_logout_list_mouse_over_image = GetObject("left_seat_logout_list_mouse_over_image")
				self.left_seat_logout_list.append(GetObject("left_seat_logout_30_min"))
				self.left_seat_logout_list.append(GetObject("left_seat_logout_60_min"))
				self.left_seat_logout_list.append(GetObject("left_seat_logout_120_min"))
				self.left_seat_logout_list.append(GetObject("left_seat_logout_180_min"))
				self.left_seat_logout_list.append(GetObject("left_seat_logout_off"))

				self.__LeftSeatListWindowAllHide()

			if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
				self.alwaysShowCountryButtonList.append(GetObject("always_show_country_on_button"))
				self.alwaysShowCountryButtonList.append(GetObject("always_show_country_off_button"))
		except:
			import exception
			exception.Abort("OptionDialog.__Load_BindObject")

	def __Load(self):
		self.__Load_LoadScript("UIScript/GameOptionDialog.py")
		self.__Load_BindObject()

		self.SetCenterPosition()

		self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))

		self.nameColorModeButtonList[0].SAFE_SetEvent(self.__OnClickNameColorModeNormalButton)
		self.nameColorModeButtonList[1].SAFE_SetEvent(self.__OnClickNameColorModeEmpireButton)

		self.viewTargetBoardButtonList[0].SAFE_SetEvent(self.__OnClickTargetBoardViewButton)
		self.viewTargetBoardButtonList[1].SAFE_SetEvent(self.__OnClickTargetBoardNoViewButton)

		self.pvpModeButtonDict[player.PK_MODE_PEACE].SAFE_SetEvent(self.__OnClickPvPModePeaceButton)
		self.pvpModeButtonDict[player.PK_MODE_REVENGE].SAFE_SetEvent(self.__OnClickPvPModeRevengeButton)
		self.pvpModeButtonDict[player.PK_MODE_GUILD].SAFE_SetEvent(self.__OnClickPvPModeGuildButton)
		self.pvpModeButtonDict[player.PK_MODE_FREE].SAFE_SetEvent(self.__OnClickPvPModeFreeButton)

		self.blockButtonList[0].SetToggleUpEvent(self.__OnClickBlockExchangeButton)
		self.blockButtonList[1].SetToggleUpEvent(self.__OnClickBlockPartyButton)
		self.blockButtonList[2].SetToggleUpEvent(self.__OnClickBlockGuildButton)
		self.blockButtonList[3].SetToggleUpEvent(self.__OnClickBlockWhisperButton)
		self.blockButtonList[4].SetToggleUpEvent(self.__OnClickBlockFriendButton)
		self.blockButtonList[5].SetToggleUpEvent(self.__OnClickBlockPartyRequest)

		self.blockButtonList[0].SetToggleDownEvent(self.__OnClickBlockExchangeButton)
		self.blockButtonList[1].SetToggleDownEvent(self.__OnClickBlockPartyButton)
		self.blockButtonList[2].SetToggleDownEvent(self.__OnClickBlockGuildButton)
		self.blockButtonList[3].SetToggleDownEvent(self.__OnClickBlockWhisperButton)
		self.blockButtonList[4].SetToggleDownEvent(self.__OnClickBlockFriendButton)
		self.blockButtonList[5].SetToggleDownEvent(self.__OnClickBlockPartyRequest)

		self.viewChatButtonList[0].SAFE_SetEvent(self.__OnClickViewChatOnButton)
		self.viewChatButtonList[1].SAFE_SetEvent(self.__OnClickViewChatOffButton)

		self.alwaysShowNameButtonList[0].SAFE_SetEvent(self.__OnClickAlwaysShowNameOnButton)
		self.alwaysShowNameButtonList[1].SAFE_SetEvent(self.__OnClickAlwaysShowNameOffButton)

		self.showDamageButtonList[0].SAFE_SetEvent(self.__OnClickShowDamageOnButton)
		self.showDamageButtonList[1].SAFE_SetEvent(self.__OnClickShowDamageOffButton)

		if app.ENABLE_OPTIMIZATION:
			self.other_char_attacked_button_list[0].SAFE_SetEvent(self.__OnClickOtherCharAttacked, self.ATTACKED_MOTION_ALL)
			self.other_char_attacked_button_list[0].ShowToolTip = lambda arg = uiScriptLocale.OPTION_ATTACKED_MOTION_ALL_DESC : self.ShowGameOptionToolTip(arg)
			self.other_char_attacked_button_list[0].HideToolTip = lambda : self.CloseGameOptionToolTip()

			self.other_char_attacked_button_list[1].SAFE_SetEvent(self.__OnClickOtherCharAttacked, self.ATTACKED_MOTION_SELF)
			self.other_char_attacked_button_list[1].ShowToolTip = lambda arg = uiScriptLocale.OPTION_ATTACKED_MOTION_SELF_DESC : self.ShowGameOptionToolTip(arg)
			self.other_char_attacked_button_list[1].HideToolTip = lambda : self.CloseGameOptionToolTip()

		self.showsalesTextButtonList[0].SAFE_SetEvent(self.__OnClickSalesTextOnButton)
		self.showsalesTextButtonList[1].SAFE_SetEvent(self.__OnClickSalesTextOffButton)

		if app.WJ_SHOW_MOB_INFO:
			self.showMobInfoButtonList[0].SetToggleUpEvent(self.__OnClickShowMobLevelButton)
			self.showMobInfoButtonList[1].SetToggleUpEvent(self.__OnClickShowMobAIFlagButton)
			self.showMobInfoButtonList[0].SetToggleDownEvent(self.__OnClickShowMobLevelButton)
			self.showMobInfoButtonList[1].SetToggleDownEvent(self.__OnClickShowMobAIFlagButton)

		if app.ENABLE_LEFT_SEAT:
			## left_seat_time
			self.left_seat_time_list_button.SetEvent(ui.__mem_func__(self.__OnClickLeftSeatTimeOpenButton))
			self.left_seat_time_list_button.ShowToolTip = lambda arg = localeInfo.LEFT_SEAT_WAIT_TIME_TOOLTIP : self.ShowGameOptionToolTip(arg)
			self.left_seat_time_list_button.HideToolTip = lambda : self.CloseGameOptionToolTip()
			self.left_seat_list_time_arrow_button.SetEvent(ui.__mem_func__(self.__OnClickLeftSeatTimeOpenButton))

			for time_index, button in enumerate(self.left_seat_time_list):
				button.SetEvent(ui.__mem_func__(self.__SetLeftSeatTime), time_index)
				button.SetOverEvent(ui.__mem_func__(self.__LeftSeatTimeListMouseOver), button)
				button.SetOverOutEvent(ui.__mem_func__(self.__LeftSeatTimeListMouseOverOut))

			## left_seat_logout
			self.left_seat_logout_list_button.SetEvent(ui.__mem_func__(self.__OnClickLeftSeatLogoutTimeOpenButton))
			self.left_seat_logout_list_button.ShowToolTip = lambda arg = localeInfo.LEFT_SEAT_LOGOUT_WAIT_TIME_TOOLTIP : self.ShowGameOptionToolTip(arg)
			self.left_seat_logout_list_button.HideToolTip = lambda : self.CloseGameOptionToolTip()
			self.left_seat_logout_list_arrow_button.SetEvent(ui.__mem_func__(self.__OnClickLeftSeatLogoutTimeOpenButton))

			for time_index, button in enumerate(self.left_seat_logout_list):
				button.SetEvent(ui.__mem_func__(self.__SetLeftSeatLogoutTime), time_index)
				button.SetOverEvent(ui.__mem_func__(self.__LeftSeatTimeListMouseOver), button)
				button.SetOverOutEvent(ui.__mem_func__(self.__LeftSeatTimeListMouseOverOut))

		if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
			self.alwaysShowCountryButtonList[0].SAFE_SetEvent(self.__OnClickAlwaysShowCountryOnButton)
			self.alwaysShowCountryButtonList[1].SAFE_SetEvent(self.__OnClickAlwaysShowCountryOffButton)

		self.__ClickRadioButton(self.nameColorModeButtonList, constInfo.GET_CHRNAME_COLOR_INDEX())
		self.__ClickRadioButton(self.viewTargetBoardButtonList, constInfo.GET_VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD())
		self.__SetPeacePKMode()

	if app.ENABLE_KEYCHANGE_SYSTEM:
		def OpenKeyChangeWindow(self):
			player.OpenKeyChangeWindow()

	if app.ENABLE_LOOTING_SYSTEM:
		def BindInterface(self, interface):
			self.interface = interface

		def __OnClickLootingSystemButton(self):
			if self.interface:
				self.interface.OpenLootingSystemWindow()

	def __ClickRadioButton(self, buttonList, buttonIndex):
		try:
			selButton = buttonList[buttonIndex]

		except IndexError:
			return

		for eachButton in buttonList:
			eachButton.SetUp()

		selButton.Down()

	def __SetNameColorMode(self, index):
		constInfo.SET_CHRNAME_COLOR_INDEX(index)
		self.__ClickRadioButton(self.nameColorModeButtonList, index)

	def __SetTargetBoardViewMode(self, flag):
		constInfo.SET_VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD(flag)
		self.__ClickRadioButton(self.viewTargetBoardButtonList, flag)

	def __OnClickNameColorModeNormalButton(self):
		self.__SetNameColorMode(0)

	def __OnClickNameColorModeEmpireButton(self):
		self.__SetNameColorMode(1)

	def __OnClickTargetBoardViewButton(self):
		self.__SetTargetBoardViewMode(0)

	def __OnClickTargetBoardNoViewButton(self):
		self.__SetTargetBoardViewMode(1)

	def __OnClickCameraModeShortButton(self):
		self.__SetCameraMode(0)

	def __OnClickCameraModeLongButton(self):
		self.__SetCameraMode(1)

	def __OnClickFogModeLevel0Button(self):
		self.__SetFogLevel(0)

	def __OnClickFogModeLevel1Button(self):
		self.__SetFogLevel(1)

	def __OnClickFogModeLevel2Button(self):
		self.__SetFogLevel(2)

	def __OnClickBlockExchangeButton(self):
		self.RefreshBlock()
		global blockMode
		net.SendCommandPacket("/setblockmode " + str(blockMode ^ player.BLOCK_EXCHANGE))

	def __OnClickBlockPartyButton(self):
		self.RefreshBlock()
		global blockMode
		net.SendCommandPacket("/setblockmode " + str(blockMode ^ player.BLOCK_PARTY))

	def __OnClickBlockGuildButton(self):
		self.RefreshBlock()
		global blockMode
		net.SendCommandPacket("/setblockmode " + str(blockMode ^ player.BLOCK_GUILD))

	def __OnClickBlockWhisperButton(self):
		self.RefreshBlock()
		global blockMode
		net.SendCommandPacket("/setblockmode " + str(blockMode ^ player.BLOCK_WHISPER))

	def __OnClickBlockFriendButton(self):
		self.RefreshBlock()
		global blockMode
		net.SendCommandPacket("/setblockmode " + str(blockMode ^ player.BLOCK_FRIEND))

	def __OnClickBlockPartyRequest(self):
		self.RefreshBlock()
		global blockMode
		net.SendCommandPacket("/setblockmode " + str(blockMode ^ player.BLOCK_PARTY_REQUEST))

	def __OnClickViewChatOnButton(self):
		global viewChatMode
		viewChatMode = 1
		systemSetting.SetViewChatFlag(viewChatMode)
		self.RefreshViewChat()

	def __OnClickViewChatOffButton(self):
		global viewChatMode
		viewChatMode = 0
		systemSetting.SetViewChatFlag(viewChatMode)
		self.RefreshViewChat()

	def __OnClickAlwaysShowNameOnButton(self):
		systemSetting.SetAlwaysShowNameFlag(True)
		self.RefreshAlwaysShowName()

	def __OnClickAlwaysShowNameOffButton(self):
		systemSetting.SetAlwaysShowNameFlag(False)
		self.RefreshAlwaysShowName()

	if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
		def __OnClickAlwaysShowCountryOnButton(self):
			systemSetting.SetAlwaysShowCountryFlag(True)
			self.RefreshAlwaysShowCountry()

		def __OnClickAlwaysShowCountryOffButton(self):
			systemSetting.SetAlwaysShowCountryFlag(False)
			self.RefreshAlwaysShowCountry()

	def __OnClickShowDamageOnButton(self):
		systemSetting.SetShowDamageFlag(True)
		self.RefreshShowDamage()

	def __OnClickShowDamageOffButton(self):
		systemSetting.SetShowDamageFlag(False)
		self.RefreshShowDamage()

	def __OnClickSalesTextOnButton(self):
		systemSetting.SetShowSalesTextFlag(True)
		self.RefreshShowSalesText()
		uiPrivateShopBuilder.UpdateADBoard()

	def __OnClickSalesTextOffButton(self):
		systemSetting.SetShowSalesTextFlag(False)
		self.RefreshShowSalesText()

	if app.WJ_SHOW_MOB_INFO:
		def __OnClickShowMobLevelButton(self):
			if systemSetting.IsShowMobLevel():
				systemSetting.SetShowMobLevel(False)
			else:
				systemSetting.SetShowMobLevel(True)
			self.RefreshShowMobInfo()

		def __OnClickShowMobAIFlagButton(self):
			if systemSetting.IsShowMobAIFlag():
				systemSetting.SetShowMobAIFlag(False)
			else:
				systemSetting.SetShowMobAIFlag(True)
			self.RefreshShowMobInfo()

	def __CheckPvPProtectedLevelPlayer(self):
		if player.GetStatus(player.LEVEL)<constInfo.PVPMODE_PROTECTED_LEVEL:
			self.__SetPeacePKMode()
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_PVPMODE_PROTECT % (constInfo.PVPMODE_PROTECTED_LEVEL))
			return 1

		return 0

	if app.ENABLE_OPTIMIZATION:
		def __OnClickOtherCharAttacked(self, attacked_motion):
			systemSetting.SetShowOtherCharAttacked(attacked_motion)
			self.RefreshOtherCharAttacked()

		def RefreshOtherCharAttacked(self):
			if systemSetting.IsShowOtherCharAttacked():
				self.other_char_attacked_button_list[0].Down()
				self.other_char_attacked_button_list[1].SetUp()
			else:
				self.other_char_attacked_button_list[0].SetUp()
				self.other_char_attacked_button_list[1].Down()

		def ShowGameOptionToolTip(self, text):
			(pos_x, pos_y) = wndMgr.GetMousePosition()

			self.tooltip.ClearToolTip()
			self.tooltip.SetThinBoardSize(app.GetTextWidth(text) + 30)
			self.tooltip.SetToolTipPosition(pos_x, pos_y - 3)
			self.tooltip.AppendTextLine(text)
			self.tooltip.Show()
			self.IsShowToolTip = True

		def CloseGameOptionToolTip(self):
			self.tooltip.Hide()
			self.IsShowToolTip = False

		def ToolTipProgress(self):
			if self.IsShowToolTip:
				(pos_x, pos_y) = wndMgr.GetMousePosition()
				self.tooltip.SetToolTipPosition(pos_x, pos_y - 3)

		def OnUpdate(self):
			self.ToolTipProgress()

	def __SetPKMode(self, mode):
		for btn in self.pvpModeButtonDict.values():
			btn.SetUp()
		if self.pvpModeButtonDict.has_key(mode):
			self.pvpModeButtonDict[mode].Down()

	def __SetPeacePKMode(self):
		self.__SetPKMode(player.PK_MODE_PEACE)

	def __RefreshPVPButtonList(self):
		self.__SetPKMode(player.GetPKMode())

	def __OnClickPvPModePeaceButton(self):
		if self.__CheckPvPProtectedLevelPlayer():
			return

		self.__RefreshPVPButtonList()

		if constInfo.PVPMODE_ENABLE:
			net.SendCommandPacket("/pkmode 0")
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_PVPMODE_NOT_SUPPORT)

	def __OnClickPvPModeRevengeButton(self):
		if self.__CheckPvPProtectedLevelPlayer():
			return

		self.__RefreshPVPButtonList()

		if constInfo.PVPMODE_ENABLE:
			net.SendCommandPacket("/pkmode 1")
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_PVPMODE_NOT_SUPPORT)

	def __OnClickPvPModeFreeButton(self):
		if self.__CheckPvPProtectedLevelPlayer():
			return

		self.__RefreshPVPButtonList()

		if constInfo.PVPMODE_ENABLE:
			net.SendCommandPacket("/pkmode 2")
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_PVPMODE_NOT_SUPPORT)

	def __OnClickPvPModeGuildButton(self):
		if self.__CheckPvPProtectedLevelPlayer():
			return

		self.__RefreshPVPButtonList()

		if 0 == player.GetGuildID():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_PVPMODE_CANNOT_SET_GUILD_MODE)
			return

		if constInfo.PVPMODE_ENABLE:
			net.SendCommandPacket("/pkmode 4")
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_PVPMODE_NOT_SUPPORT)

	def OnChangePKMode(self):
		self.__RefreshPVPButtonList()

	def OnCloseInputDialog(self):
		self.inputDialog.Close()
		self.inputDialog = None
		return True

	def OnCloseQuestionDialog(self):
		self.questionDialog.Close()
		self.questionDialog = None
		return True

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def RefreshBlock(self):
		global blockMode
		for i in xrange(len(self.blockButtonList)):
			if 0 != (blockMode & (1 << i)):
				self.blockButtonList[i].Down()
			else:
				self.blockButtonList[i].SetUp()

	def RefreshViewChat(self):
		if systemSetting.IsViewChat():
			self.viewChatButtonList[0].Down()
			self.viewChatButtonList[1].SetUp()
		else:
			self.viewChatButtonList[0].SetUp()
			self.viewChatButtonList[1].Down()

	def RefreshAlwaysShowName(self):
		if systemSetting.IsAlwaysShowName():
			self.alwaysShowNameButtonList[0].Down()
			self.alwaysShowNameButtonList[1].SetUp()
		else:
			self.alwaysShowNameButtonList[0].SetUp()
			self.alwaysShowNameButtonList[1].Down()

	if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
		def RefreshAlwaysShowCountry(self):
			if systemSetting.IsAlwaysShowCountry():
				self.alwaysShowCountryButtonList[0].Down()
				self.alwaysShowCountryButtonList[1].SetUp()
			else:
				self.alwaysShowCountryButtonList[0].SetUp()
				self.alwaysShowCountryButtonList[1].Down()

	def RefreshShowDamage(self):
		if systemSetting.IsShowDamage():
			self.showDamageButtonList[0].Down()
			self.showDamageButtonList[1].SetUp()
		else:
			self.showDamageButtonList[0].SetUp()
			self.showDamageButtonList[1].Down()

	def RefreshShowSalesText(self):
		if systemSetting.IsShowSalesText():
			self.showsalesTextButtonList[0].Down()
			self.showsalesTextButtonList[1].SetUp()
		else:
			self.showsalesTextButtonList[0].SetUp()
			self.showsalesTextButtonList[1].Down()

	if app.WJ_SHOW_MOB_INFO:
		def RefreshShowMobInfo(self):
			if systemSetting.IsShowMobLevel():
				self.showMobInfoButtonList[0].Down()
			else:
				self.showMobInfoButtonList[0].SetUp()

			if systemSetting.IsShowMobAIFlag():
				self.showMobInfoButtonList[1].Down()
			else:
				self.showMobInfoButtonList[1].SetUp()

	def OnBlockMode(self, mode):
		global blockMode
		blockMode = mode
		self.RefreshBlock()

	def Show(self):
		self.RefreshBlock()
		ui.ScriptWindow.Show(self)
		self.IsShow = True

	def Close(self):
		self.Hide()
		self.IsShow = False

		if app.ENABLE_CHAR_CONFIG:
			systemSetting.SaveCharConfig()

		if app.ENABLE_LEFT_SEAT:
			self.__LeftSeatListWindowAllHide()

	def IsShowWindow(self):
		return self.IsShow

	if app.ENABLE_LEFT_SEAT:
		def LoadLeftSeatWaitTimeIndexData(self):
			if self.is_load_left_seat_wait_time_data:
				return

			try:
				path = self.__CheckLeftSeatWaitTimeDataPath()
				if os.path.exists(path):
					with open(path, "rt") as file:
						line = file.readline().strip()
						if line:
							tokens = line.split()
							if len(tokens) == 2:
								self.__SetLeftSeatTime(int(tokens[0]), True)
								self.__SetLeftSeatLogoutTime(int(tokens[1]), True)

								self.is_load_left_seat_wait_time_data = True
							else:
								raise RuntimeError("Unknown TokenSize")
						else:
							self.__SetLeftSeatTime(0, False)
							self.__SetLeftSeatLogoutTime(0, False)
				else:
					with open(path, "wt") as file:
						file.write("%d\t%d\n" % (player.LEFT_SEAT_TIME_10_MIN, player.LEFT_SEAT_LOGOUT_TIME_180_MIN))

					self.__SetLeftSeatTime(player.LEFT_SEAT_TIME_10_MIN, True)
					self.__SetLeftSeatLogoutTime(player.LEFT_SEAT_LOGOUT_TIME_180_MIN, True)

					self.is_load_left_seat_wait_time_data = True

			except Exception as e:
				import dbg
				dbg.TraceError("OptionDialog.LoadLeftSeatWaitTimeIndexData - %s" % (e))
				app.Abort()

		def __CheckLeftSeatWaitTimeDataPath(self):
			try:
				path = os.getcwd() + os.sep + "UserData" + os.sep + "leftseat"
				if not os.path.exists(path):
					os.mkdir(path)
				return os.path.join(path, player.GetName())
			except Exception as e:
				import dbg
				dbg.TraceError("OptionDialog.CheckLeftSeatWaitTimeDataPath - %s" % (e))
				app.Abort()

		def __SetLeftSeatTime(self, time_index, is_save = True):
			if self.left_seat_time_list_window.IsShow():
				self.left_seat_time_list_window.Hide()

			if time_index not in self.LEFT_SEAT_TIME_DATA or time_index == self.left_seat_time_index:
				return

			self.left_seat_time_index = time_index
			self.left_seat_time_list_button.SetText(self.LEFT_SEAT_TIME_DATA[time_index])

			net.SendLeftSeatWaitTimeIndexPacket(time_index)

			if is_save:
				self.__SaveLeftSeatData()

		def __SetLeftSeatLogoutTime(self, time_index, is_save = True):
			if self.left_seat_logout_list_window.IsShow():
				self.left_seat_logout_list_window.Hide()

			if time_index not in self.LEFT_SEAT_LOGOUT_TIME_DATA or time_index == self.left_seat_logout_time_index:
				return

			self.left_seat_logout_time_index = time_index
			self.left_seat_logout_list_button.SetText(self.LEFT_SEAT_LOGOUT_TIME_DATA[time_index])

			net.SendLeftSeatOffPacket(time_index)

			if is_save:
				self.__SaveLeftSeatData()

		def __SaveLeftSeatData(self):
			try:
				path = self.__CheckLeftSeatWaitTimeDataPath()
				with open(path, "wt") as file:
					file.write("%d %d" % (self.left_seat_time_index, self.left_seat_logout_time_index))
			except Exception as e:
				import dbg
				dbg.TraceError("OptionDialog.SaveLeftSeatData - %s" % (e))
				app.Abort()

		def __OnClickLeftSeatTimeOpenButton(self):
			if not self.left_seat_time_list_window.IsShow():
				self.left_seat_time_list_window.Show()
			else:
				self.left_seat_time_list_window.Hide()

		def __LeftSeatTimeListMouseOver(self, button):
			if self.left_seat_list_time_mouse_over_image:
				self.left_seat_list_time_mouse_over_image.SetParent(button)
				self.left_seat_list_time_mouse_over_image.SetPosition(0, 0)
				if localeInfo.IsARABIC():
					self.left_seat_list_time_mouse_over_image.SetWindowHorizontalAlignLeft()
				self.left_seat_list_time_mouse_over_image.Show()

		def __LeftSeatTimeListMouseOverOut(self):
			if self.left_seat_list_time_mouse_over_image:
				self.left_seat_list_time_mouse_over_image.Hide()

		def __OnClickLeftSeatLogoutTimeOpenButton(self):
			if not self.left_seat_logout_list_window.IsShow():
				self.left_seat_logout_list_window.Show()
			else:
				self.left_seat_logout_list_window.Hide()

		def __LeftSeatLogoutTimeListMouseOver(self, button):
			if self.left_seat_logout_list_mouse_over_image:
				self.left_seat_logout_list_mouse_over_image.SetParent(button)
				self.left_seat_logout_list_mouse_over_image.SetPosition(0, 0)
				if localeInfo.IsARABIC():
					self.left_seat_list_time_mouse_over_image.SetWindowHorizontalAlignLeft()
				self.left_seat_logout_list_mouse_over_image.Show()

		def __LeftSeatLogoutTimeListMouseOverOut(self):
			if self.left_seat_logout_list_mouse_over_image:
				self.left_seat_logout_list_mouse_over_image.Hide()

		def __LeftSeatListWindowAllHide(self):
			if self.left_seat_time_list_window:
				self.left_seat_time_list_window.Hide()

			if self.left_seat_list_time_mouse_over_image:
				self.left_seat_list_time_mouse_over_image.Hide()

			if self.left_seat_logout_list_window:
				self.left_seat_logout_list_window.Hide()

			if self.left_seat_logout_list_mouse_over_image:
				self.left_seat_logout_list_mouse_over_image.Hide()
