import net
import app
import ui
import uiOption

import uiSystemOption
import uiGameOption
import uiScriptLocale
import networkModule
import constInfo
import localeInfo

if app.ENABLE_MOVE_CHANNEL:
	import chat

if app.ENABLE_GAME_OPTION_ESCAPE:
	import uiToolTip, wndMgr

SYSTEM_MENU_FOR_PORTAL = False

###################################################################################################
## System
class SystemDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__Initialize()

		if app.ENABLE_GAME_OPTION_ESCAPE:
			self.tooltip = uiToolTip.ToolTip()

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		if app.ENABLE_GAME_OPTION_ESCAPE:
			del self.tooltip

	def __Initialize(self):
		self.eventOpenHelpWindow = None
		self.systemOptionDlg = None

		if app.ENABLE_LEFT_SEAT:
			self.gameOptionDlg = uiGameOption.OptionDialog()
		else:
			self.gameOptionDlg = None

		if app.ENABLE_MOVE_CHANNEL:
			self.moveChannelDlg = None

		if app.ENABLE_LOOTING_SYSTEM or app.ENABLE_GAME_OPTION_ESCAPE or app.ENABLE_LEFT_SEAT:
			self.interface = None

		if app.ENABLE_GAME_OPTION_ESCAPE:
			self.is_show_tooltip = False

	def LoadDialog(self):
		if SYSTEM_MENU_FOR_PORTAL:
			self.__LoadSystemMenu_ForPortal()
		else:
			self.__LoadSystemMenu_Default()

	def __LoadSystemMenu_Default(self):
		pyScrLoader = ui.PythonScriptLoader()
		if constInfo.IN_GAME_SHOP_ENABLE:
			pyScrLoader.LoadScriptFile(self, uiScriptLocale.LOCALE_COMMON_UISCRIPT_PATH + "SystemDialog.py")
		else:
			pyScrLoader.LoadScriptFile(self, "uiscript/systemdialog.py")

		self.GetChild("system_option_button").SAFE_SetEvent(self.__ClickSystemOptionButton)
		self.GetChild("game_option_button").SAFE_SetEvent(self.__ClickGameOptionButton)
		self.GetChild("change_button").SAFE_SetEvent(self.__ClickChangeCharacterButton)
		self.GetChild("logout_button").SAFE_SetEvent(self.__ClickLogOutButton)
		self.GetChild("exit_button").SAFE_SetEvent(self.__ClickExitButton)
		self.GetChild("help_button").SAFE_SetEvent(self.__ClickHelpButton)
		self.GetChild("cancel_button").SAFE_SetEvent(self.Close)

		if constInfo.IN_GAME_SHOP_ENABLE:
			self.GetChild("mall_button").SAFE_SetEvent(self.__ClickInGameShopButton)

		if app.ENABLE_MOVE_CHANNEL:
			self.GetChild("movechannel_button").SAFE_SetEvent(self.__ClickMoveChannelButton)

		if app.ENABLE_GAME_OPTION_ESCAPE:
			self.GetChild("escape_button").SAFE_SetEvent(self.__ClickEscapeButton)
			self.GetChild("escape_button").ShowToolTip = lambda arg = localeInfo.OPTION_ESCAPE_SYSTEM_BUTTON_TOOLTIP : self.OverInToolTipButton(arg)
			self.GetChild("escape_button").HideToolTip = lambda : self.OverOutToolTipButton()

	def __LoadSystemMenu_ForPortal(self):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "uiscript/systemdialog_forportal.py")

		self.GetChild("system_option_button").SAFE_SetEvent(self.__ClickSystemOptionButton)
		self.GetChild("game_option_button").SAFE_SetEvent(self.__ClickGameOptionButton)
		self.GetChild("change_button").SAFE_SetEvent(self.__ClickChangeCharacterButton)
		self.GetChild("exit_button").SAFE_SetEvent(self.__ClickExitButton)
		self.GetChild("help_button").SAFE_SetEvent(self.__ClickHelpButton)
		self.GetChild("cancel_button").SAFE_SetEvent(self.Close)
		if app.ENABLE_MOVE_CHANNEL:
			self.GetChild("movechannel_button").SAFE_SetEvent(self.__ClickMoveChannelButton)

	def Destroy(self):
		self.ClearDictionary()

		if self.gameOptionDlg:
			self.gameOptionDlg.Destroy()

		if self.systemOptionDlg:
			self.systemOptionDlg.Destroy()

		self.__Initialize()

	def SetOpenHelpWindowEvent(self, event):
		self.eventOpenHelpWindow = event

	def OpenDialog(self):
		self.Show()

	def __ClickChangeCharacterButton(self):
		self.Close()

		net.ExitGame()

	def __OnClosePopupDialog(self):
		self.popup = None

	def __ClickLogOutButton(self):
		if SYSTEM_MENU_FOR_PORTAL:
			if app.loggined:
				self.Close()
				net.ExitApplication()
			else:
				self.Close()
				net.LogOutGame()
		else:
			self.Close()
			net.LogOutGame()

	def __ClickExitButton(self):
		self.Close()
		net.ExitApplication()

	def __ClickSystemOptionButton(self):
		self.Close()

		if not self.systemOptionDlg:
			self.systemOptionDlg = uiSystemOption.OptionDialog()

		self.systemOptionDlg.Show()

	def __ClickGameOptionButton(self):
		self.Close()

		if not self.gameOptionDlg:
			self.gameOptionDlg = uiGameOption.OptionDialog()

			if app.ENABLE_LOOTING_SYSTEM or app.ENABLE_GAME_OPTION_ESCAPE or app.ENABLE_LEFT_SEAT:
				if self.interface:
					self.gameOptionDlg.BindInterface(self.interface)

		self.gameOptionDlg.Show()

	def __ClickHelpButton(self):
		self.Close()

		if None != self.eventOpenHelpWindow:
			self.eventOpenHelpWindow()

	def __ClickInGameShopButton(self):
		self.Close()
		net.SendCommandPacket("/in_game_mall")

	def Close(self):
		self.Hide()
		return True

	def OnBlockMode(self, mode):
		uiGameOption.blockMode = mode
		if self.gameOptionDlg:
			self.gameOptionDlg.OnBlockMode(mode)
		#self.optionDialog.OnBlockMode(mode)

	def OnChangePKMode(self):
		if self.gameOptionDlg:
			self.gameOptionDlg.OnChangePKMode()
		#self.optionDialog.OnChangePKMode()

	def OnPressExitKey(self):
		self.Close()
		return True

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def GetSystemOption(self):
		return self.systemOptionDlg

	if app.ENABLE_MOVE_CHANNEL:
		def __ClickMoveChannelButton(self):
			## 99 서버 예외 처리 
			if net.GetChannelNumber() == 99:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOVE_CHANNEL_NOT_MOVE)
				return

			## 인스턴스 던전 예외처리
			elif net.GetMapIndex() >= 10000:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOVE_CHANNEL_NOT_MOVE)
				return

			if self.moveChannelDlg:
				self.moveChannelDlg.Show()
			else:
				moveChannelDlg = MoveChannelDialog()
				moveChannelDlg.Show()
				self.moveChannelDlg = moveChannelDlg

			self.Close()

	def HideAllSystemOptioin(self):
		if self.systemOptionDlg:
			self.systemOptionDlg.Hide()
		if self.gameOptionDlg:
			self.gameOptionDlg.Hide()

		if app.ENABLE_MOVE_CHANNEL:
			if self.moveChannelDlg:
				self.moveChannelDlg.Hide()

	def ShowAllSystemOptioin(self):
		if self.systemOptionDlg and self.systemOptionDlg.IsShowWindow():
			self.systemOptionDlg.Show()
		if self.gameOptionDlg and self.gameOptionDlg.IsShowWindow():
			self.gameOptionDlg.Show()
		if app.ENABLE_MOVE_CHANNEL:
			if self.moveChannelDlg and self.moveChannelDlg.IsShowWindow():
				self.moveChannelDlg.Show()

	if app.ENABLE_LOOTING_SYSTEM or app.ENABLE_GAME_OPTION_ESCAPE or app.ENABLE_LEFT_SEAT:
		def BindInterface(self, interface):
			from _weakref import proxy
			self.interface = proxy(interface)

			if self.gameOptionDlg and self.interface:
				self.gameOptionDlg.BindInterface(self.interface)

	if app.ENABLE_GAME_OPTION_ESCAPE:
		def __ClickEscapeButton(self):
			self.Close()

			if self.interface and not self.interface.IsEscapeQuestionPopupOpen():
				self.interface.OpenEscapeQuestionPopup()

		def ToolTipProgress(self):
			if self.is_show_tooltip:
				pos_x, pos_y = wndMgr.GetMousePosition()
				self.tooltip.SetToolTipPosition(pos_x + 50, pos_y + 50)

		def OverInToolTipButton(self, arg):
			pos_x, pos_y = wndMgr.GetMousePosition()
			self.tooltip.ClearToolTip()
			self.tooltip.SetThinBoardSize(11 * app.GetTextLength(arg))
			self.tooltip.SetToolTipPosition(pos_x + 50, pos_y + 50)
			self.tooltip.AppendTextLine(arg, 0xffffffff)
			self.tooltip.Show()
			self.is_show_tooltip = True

		def OverOutToolTipButton(self):
			self.tooltip.Hide()
			self.is_show_tooltip = False

		def OnUpdate(self):
			self.ToolTipProgress()

	if app.ENABLE_LOCALE_CLIENT:
		def LanguageChange(self):
			if self.systemOptionDlg:
				self.systemOptionDlg.LanguageChange()

	if app.ENABLE_LEFT_SEAT:
		def LoadLeftSeatWaitTimeIndexData(self):
			if self.gameOptionDlg:
				self.gameOptionDlg.LoadLeftSeatWaitTimeIndexData()

if app.ENABLE_MOVE_CHANNEL:
	class MoveChannelDialog(ui.ScriptWindow):
		def __init__(self):
			ui.ScriptWindow.__init__(self)
			self.__LoadDialog()
			self.IsShow = False

		def __del__(self):
			ui.ScriptWindow.__del__(self)

		def __LoadDialog(self):
			try:
				pyScrLoader = ui.PythonScriptLoader()
				pyScrLoader.LoadScriptFile(self, "UIScript/MoveChannelDialog.py")
			except:
				import exception
				exception.Abort("MoveChannelDialog.__LoadDialog")

			self.ParentBoard = self.GetChild("MoveChannelBoard")
			self.ChildBoard = self.GetChild("BlackBoard")
			self.GetChild("MoveChannelTitle").SetCloseEvent(ui.__mem_func__(self.Close))

			self.ChannelList = []
			cnt = self.GetChannelCount()
			# cnt = net.GetChannelCount()
			# cnt = cnt - 1 # 99서버 제외

			self.DlgWidht = 190
			self.BlackBoardHeight = 23*cnt + 5*(cnt-1) + 13
			self.DlgHeight = self.BlackBoardHeight + 75

			self.AcceptBtn = ui.MakeButton(self.ParentBoard, 13, self.DlgHeight - 33, "", "d:/ymir work/ui/public/", "middle_button_01.sub", "middle_button_02.sub", "middle_button_03.sub")
			self.AcceptBtn.SetText( localeInfo.MOVE_CHANNEL_SELECT )
			self.AcceptBtn.SetEvent(ui.__mem_func__(self.AcceptButton))
			self.CloseBtn = ui.MakeButton(self.ParentBoard, self.DlgWidht - 73, self.DlgHeight - 33, "", "d:/ymir work/ui/public/", "middle_button_01.sub", "middle_button_02.sub", "middle_button_03.sub")
			self.CloseBtn.SetText( localeInfo.MOVE_CHANNEL_CANCEL )
			self.CloseBtn.SetEvent(ui.__mem_func__(self.Close))

			for i in xrange(cnt):
				btn = ui.MakeButton(self.ChildBoard, 8, 6 + i*28, "", "d:/ymir work/ui/game/myshop_deco/", "select_btn_01.sub", "select_btn_02.sub", "select_btn_03.sub")
				btn.SetText(self.GetChannelName(i+1))
				btn.SetEvent(ui.__mem_func__(self.__SelectChannel), i+1)
				self.ChannelList.append(btn)

			self.ParentBoard.SetSize(self.DlgWidht, self.DlgHeight)
			self.ChildBoard.SetSize(self.DlgWidht - 26, self.BlackBoardHeight)
			self.SetSize(self.DlgWidht, self.DlgHeight)

			self.UpdateRect()

		def __SelectChannel(self, idx):
			self.ChangeChannelNumber = idx

			for btn in self.ChannelList:
				btn.SetUp()
				btn.Enable()

			self.ChannelList[idx-1].Down()
			self.ChannelList[idx-1].Disable()

		def AcceptButton(self):
			if self.ChangeChannelNumber == self.StartChannelNumber:
				return

			# net.MoveChannelGame(self.ChangeChannelNumber)
			net.SendCommandPacket("/move_channel %d" % self.ChangeChannelNumber)
			self.__SaveChannelInfo()
			self.StartChannelNumber = self.ChangeChannelNumber
			self.Close()

		def Show(self):
			ui.ScriptWindow.Show(self)

			self.StartChannelNumber = net.GetChannelNumber()
			self.__SelectChannel(self.StartChannelNumber)

			self.IsShow = True

		def Close(self):
			self.Hide()
			self.IsShow = False

		def OnPressEscapeKey(self):
			self.Close()
			return True

		def IsShowWindow(self):
			return self.IsShow

		# 2019.09.02.owsap - Get channel info
		def __SaveChannelInfo(self):
			loadRegionID, loadServerID, loadChannelID = self.__LoadChannelInfo()
			try:
				file = open("channel.inf", "w")
				file.write("%d %d %d" % (loadServerID, self.ChangeChannelNumber, loadRegionID))
			except:
				print "MoveChannelDialog.__SaveChannelInfo - SaveError"

		def __LoadChannelInfo(self):
			try:
				file = open("channel.inf")
				lines = file.readlines()

				if len(lines) > 0:
					tokens = lines[0].split()

					selServerID = int(tokens[0])
					selChannelID = int(tokens[1])

					if len(tokens) == 3:
						regionID = int(tokens[2])

					return regionID, selServerID, selChannelID
			except:
				print "MoveChannelDialog.__LoadChannelInfo - OpenError"
				return -1, -1, -1

		def GetChannelName(self, idx):
			return "CH %d" % idx

		def GetChannelCount(self):
			import serverInfo
			loadRegionID, loadServerID, loadChannelID = self.__LoadChannelInfo()
			channelDict = serverInfo.REGION_DICT[loadRegionID][loadServerID]["channel"]
			return len(channelDict)

		def GetChannelNumber(self):
			loadRegionID, loadServerID, loadChannelID = self.__LoadChannelInfo()
			return loadChannelID

if __name__ == "__main__":
	import app
	import wndMgr
	import systemSetting
	import mouseModule
	import grp
	import ui
	import chr
	import background
	import player

	# wndMgr.SetOutlineFlag(True)

	app.SetMouseHandler(mouseModule.mouseController)
	app.SetHairColorEnable(True)
	wndMgr.SetMouseHandler(mouseModule.mouseController)
	wndMgr.SetScreenSize(systemSetting.GetWidth(), systemSetting.GetHeight())
	app.Create("METIN2 CLOSED BETA", systemSetting.GetWidth(), systemSetting.GetHeight(), 1)
	mouseModule.mouseController.Create()

	wnd = SystemDialog()
	wnd.LoadDialog()
	wnd.Show()

	app.Loop()