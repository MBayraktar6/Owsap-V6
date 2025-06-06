import dbg
import app
import net
import ui
import ime
import snd
import wndMgr
import musicInfo
import serverInfo
import systemSetting
import ServerStateChecker
import localeInfo
import constInfo
import osfInfo
import uiCommon
import time
import serverCommandParser
import ime
import uiScriptLocale
import debugInfo

LOGIN_DELAY_SEC = 0.0
SKIP_LOGIN_PHASE = False
SKIP_LOGIN_PHASE_SUPPORT_CHANNEL = False
FULL_BACK_IMAGE = False

VIRTUAL_KEYBOARD_NUM_KEYS = 46
VIRTUAL_KEYBOARD_RAND_KEY = True

def Suffle(src):
	if VIRTUAL_KEYBOARD_RAND_KEY:
		items = [item for item in src]

		itemCount = len(items)
		for oldPos in xrange(itemCount):
			newPos = app.GetRandom(0, itemCount - 1)
			items[newPos], items[oldPos] = items[oldPos], items[newPos]

		return "".join(items)
	else:
		return src

if localeInfo.IsNEWCIBN() or localeInfo.IsCIBN10():
	LOGIN_DELAY_SEC = 60.0
	FULL_BACK_IMAGE = True

elif localeInfo.IsYMIR() or localeInfo.IsCHEONMA():
	FULL_BACK_IMAGE = True

elif localeInfo.IsHONGKONG():
	FULL_BACK_IMAGE = True

elif localeInfo.IsJAPAN():
	FULL_BACK_IMAGE = True

elif localeInfo.IsBRAZIL() or osfInfo.LOGIN_UI_MODIFY:
	if app.LOGIN_COUNT_DOWN_UI_MODIFY:
		LOGIN_DELAY_SEC = 30.0
	else:
		LOGIN_DELAY_SEC = 60.0

def IsFullBackImage():
	global FULL_BACK_IMAGE
	return FULL_BACK_IMAGE

def IsLoginDelay():
	global LOGIN_DELAY_SEC
	if LOGIN_DELAY_SEC > 0.0:
		return True
	else:
		return False

def GetLoginDelay():
	global LOGIN_DELAY_SEC
	return LOGIN_DELAY_SEC

app.SetGuildMarkPath("test")

class ConnectingDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadDialog()
		self.eventTimeOver = lambda *arg: None
		self.eventExit = lambda *arg: None

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadDialog(self):
		try:
			PythonScriptLoader = ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self, "UIScript/ConnectingDialog.py")

			self.board = self.GetChild("board")
			self.message = self.GetChild("message")
			self.countdownMessage = self.GetChild("countdown_message")

		except:
			import exception
			exception.Abort("ConnectingDialog.LoadDialog.BindObject")

	def Open(self, waitTime):
		curTime = time.clock()
		self.endTime = curTime + waitTime

		self.Lock()
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		self.Unlock()
		self.Hide()

	def Destroy(self):
		self.Hide()
		self.ClearDictionary()

	def SetText(self, text):
		self.message.SetText(text)

	def SetCountDownMessage(self, waitTime):
		self.countdownMessage.SetText("%.0f%s" % (waitTime, localeInfo.SECOND))

	def SAFE_SetTimeOverEvent(self, event):
		self.eventTimeOver = ui.__mem_func__(event)

	def SAFE_SetExitEvent(self, event):
		self.eventExit = ui.__mem_func__(event)

	def OnUpdate(self):
		lastTime = max(0, self.endTime - time.clock())
		if 0 == lastTime:
			self.Close()
			self.eventTimeOver()
		else:
			self.SetCountDownMessage(self.endTime - time.clock())

	def OnPressExitKey(self):
		# self.eventExit()
		return True

if app.ENABLE_SERVER_SELECT_RENEWAL:
	import uiToolTip
	class ServerListBox(ui.ListBox2):
		STATE_IMAGE_PATH = (
			"",
			"locale/common" + "/ui/login/choise_new.tga",
			"locale/common" + "/ui/login/choise_special.tga",
			"locale/common" + "/ui/login/choise_close.tga",
			"locale/common" + "/ui/login/choise_close.tga",
		)

		SERVER_FLAG_IMAGE_PATH_DICT = {
			"AE" : "d:/ymir work/ui/intro/login/server_flag_AE.sub",
			"ALL" : "d:/ymir work/ui/intro/login/server_flag_ALL.sub",
			"BR" : "d:/ymir work/ui/intro/login/server_flag_BR.sub",
			"CZ" : "d:/ymir work/ui/intro/login/server_flag_CZ.sub",
			"DE" : "d:/ymir work/ui/intro/login/server_flag_DE.sub",
			"DK" : "d:/ymir work/ui/intro/login/server_flag_DK.sub",
			"ES" : "d:/ymir work/ui/intro/login/server_flag_ES.sub",
			"EU" : "d:/ymir work/ui/intro/login/server_flag_EU.sub",
			"FR" : "d:/ymir work/ui/intro/login/server_flag_FR.sub",
			"GR" : "d:/ymir work/ui/intro/login/server_flag_GR.sub",
			"HU" : "d:/ymir work/ui/intro/login/server_flag_HU.sub",
			"IT" : "d:/ymir work/ui/intro/login/server_flag_IT.sub",
			"KR" : "d:/ymir work/ui/intro/login/server_flag_KR.sub",
			"NL" : "d:/ymir work/ui/intro/login/server_flag_NL.sub",
			"PL" : "d:/ymir work/ui/intro/login/server_flag_PL.sub",
			"PT" : "d:/ymir work/ui/intro/login/server_flag_PT.sub",
			"RO" : "d:/ymir work/ui/intro/login/server_flag_RO.sub",
			"RU" : "d:/ymir work/ui/intro/login/server_flag_RU.sub",
			"SP1" : "d:/ymir work/ui/intro/login/server_flag_SP1.sub",
			"SP2" : "d:/ymir work/ui/intro/login/server_flag_SP2.sub",
			"SP3" : "d:/ymir work/ui/intro/login/server_flag_SP3.sub",
			"SP4" : "d:/ymir work/ui/intro/login/server_flag_SP4.sub",
			"SP5" : "d:/ymir work/ui/intro/login/server_flag_SP5.sub",
			"SP6" : "d:/ymir work/ui/intro/login/server_flag_SP6.sub",
			"SP7" : "d:/ymir work/ui/intro/login/server_flag_SP7.sub",
			"TR" : "d:/ymir work/ui/intro/login/server_flag_TR.sub",
			"UK" : "d:/ymir work/ui/intro/login/server_flag_UK.sub"
		}

		def __init__(self, layer = "UI"):
			ui.ListBox2.__init__(self, layer)
			self.stateList = []
			self.stateList2 = []
			self.flagList = []
			self.stateDict = {}
			self.stateDict2 = {}
			self.flagDict = {}
			self.toolTip = uiToolTip.ToolTip()

			self.STATE_IMAGE_TOOLTIP = (
				"",
				localeInfo.SERVER_STATUS_NEW,
				localeInfo.SERVER_STATUS_SPECIAL,
				localeInfo.SERVER_STATUS_CLOSE,
				localeInfo.SERVER_STATUS_STANDBY,
			)

		def ClearItem(self):
			ui.ListBox2.ClearItem(self)
			self.stateList = []
			self.stateList2 = []
			self.flagList = []
			self.stateDict = {}
			self.stateDict2 = {}
			self.flagDict = {}

		def StateImageEventProgress(self, event_type, arg):
			if "mouse_click" == event_type:
				self.SelectItem(arg)
			elif "mouse_over_in" == event_type:
				arglen = len(self.STATE_IMAGE_TOOLTIP[arg])
				pos_x, pos_y = wndMgr.GetMousePosition()

				self.toolTip.ClearToolTip()
				self.toolTip.SetThinBoardSize(11 * arglen)
				self.toolTip.SetToolTipPosition(pos_x + 50, pos_y + 50)
				self.toolTip.AppendTextLine(self.STATE_IMAGE_TOOLTIP[arg], 0xffffffff)
				self.toolTip.Show()
			elif "mouse_over_out" == event_type:
				self.toolTip.Hide()

		def FlagImageEventProgress(self, event_type, arg):
			if "mouse_click" == event_type:
				self.SelectItem(arg)

		def GetState(self, line):
			if line >= len(self.itemList) or line < 0:
				return (0, 0)

			if line >= len(self.stateList) or line >= len(self.stateList2):
				return (0, 0)

			return self.stateList[line], self.stateList2[line]

		def _LocateItem(self):
			pos = (0, self.TEMPORARY_PLACE)

			self.showLineCount = 0
			for textLine in self.itemList:
				x, y = self._CalcRenderPos(pos, self.showLineCount)
				if self.flagDict.get(self.showLineCount) != None:
					flagWidth = self.flagDict[self.showLineCount].GetWidth()

					self.flagDict[self.showLineCount].SetPosition(0, y - 3)
					if localeInfo.IsARABIC():
						self.flagDict[self.showLineCount].SetPosition(flagWidth, y - 3)
						self.flagDict[self.showLineCount].SetWindowHorizontalAlignRight()

					if localeInfo.IsARABIC():
						textLine.SetPosition(x + flagWidth + 5, y)
					else:
						textLine.SetPosition(x + flagWidth, y)
				else:
					textLine.SetPosition(x, y)
				textLine.Show()

				if self.stateDict.get(self.showLineCount) != None:
					w = app.GetTextWidth(textLine.GetText())

					if self.flagDict.get(self.showLineCount) != None:
						flagWidth = self.flagDict[self.showLineCount].GetWidth()
					else:
						flagWidth = 0

					if localeInfo.IsARABIC():
						self.stateDict[self.showLineCount].SetPosition(self.GetWidth() - w - flagWidth - 15, y - 3)
					else:
						self.stateDict[self.showLineCount].SetPosition(x + w + flagWidth + 5, y - 3)

					imageWidth = self.stateDict[self.showLineCount].GetWidth()

					if self.stateDict2.get(self.showLineCount) != None:
						if localeInfo.IsARABIC():
							self.stateDict2[self.showLineCount].SetPosition(self.GetWidth() - w - flagWidth - 15 - imageWidth, y - 3)
							#self.stateDict2[self.showLineCount].SetPosition(x, y)
						else:
							self.stateDict2[self.showLineCount].SetPosition(x + w + flagWidth + 5 + imageWidth, y - 3)

				self.showLineCount += 1

		def InsertItem(self, number, text, state = 0, state2 = 0, flag = "ALL"):
			self.keyDict[len(self.itemList)] = number
			self.textDict[len(self.itemList)] = text
			textLine = ui.TextLine()
			textLine.SetParent(self)
			textLine.SetText(text)
			textLine.Show()

			if self.itemCenterAlign:
				textLine.SetWindowHorizontalAlignCenter()
				textLine.SetHorizontalAlignCenter()
			elif localeInfo.IsARABIC():
				textLine.SetWindowHorizontalAlignRight()
				textLine.SetHorizontalAlignLeft()

			if state != 0:
				imgBox = ui.ExpandedImageBox()
				imgBox.SetParent(self)
				imgBox.LoadImage(self.STATE_IMAGE_PATH[state])
				imgBox.Show()

				imgBox.SetEvent(ui.__mem_func__(self.StateImageEventProgress), "mouse_click", len(self.itemList))
				imgBox.SetEvent(ui.__mem_func__(self.StateImageEventProgress), "mouse_over_in", state)
				imgBox.SetEvent(ui.__mem_func__(self.StateImageEventProgress), "mouse_over_out", 0)

				self.stateDict[len(self.itemList)] = imgBox

			if state2 != 0:
				imgBox2 = ui.ExpandedImageBox()
				imgBox2.SetParent(self)
				imgBox2.LoadImage(self.STATE_IMAGE_PATH[state2])
				imgBox2.Show()

				imgBox2.SetEvent(ui.__mem_func__(self.StateImageEventProgress), "mouse_click", len(self.itemList))
				imgBox2.SetEvent(ui.__mem_func__(self.StateImageEventProgress), "mouse_over_in", state2)
				imgBox2.SetEvent(ui.__mem_func__(self.StateImageEventProgress), "mouse_over_out", 0)

				self.stateDict2[len(self.itemList)] = imgBox2

			if flag.upper() in self.SERVER_FLAG_IMAGE_PATH_DICT.keys():
				imgBox3 = ui.ExpandedImageBox()
				imgBox3.SetParent(self)
				imgBox3.LoadImage(self.SERVER_FLAG_IMAGE_PATH_DICT[flag.upper()])
				imgBox3.Show()

				imgBox3.SetEvent(ui.__mem_func__(self.FlagImageEventProgress), "mouse_click", len(self.itemList))
				imgBox3.SetEvent(ui.__mem_func__(self.FlagImageEventProgress), "mouse_over_in", flag)
				imgBox3.SetEvent(ui.__mem_func__(self.FlagImageEventProgress), "mouse_over_out", 0)

				self.flagDict[len(self.itemList)] = imgBox3

			self.itemList.append(textLine)
			self.stateList.append(state)
			self.stateList2.append(state2)
			self.flagList.append(flag)

			self._LocateItem()
			self._RefreshForm()

	class ChannelListBox(ui.ListBox):
		TEMPORARY_PLACE = 3

		def __init__(self, layer = "UI"):
			ui.ListBox.__init__(self, layer)
			self.stateList = []

		def ClearItem(self):
			ui.ListBox.ClearItem(self)
			self.stateList = []

		def InsertItem(self, number, text, text2, textColor = 0x00000000):
			self.keyDict[len(self.itemList)] = number
			self.textDict[len(self.itemList)] = text

			textLine = ui.TextLine()
			textLine.SetParent(self)
			textLine.SetText(text)
			textLine.Show()

			if self.itemCenterAlign:
				textLine.SetWindowHorizontalAlignCenter()
				textLine.SetHorizontalAlignCenter()
			elif localeInfo.IsARABIC():
				textLine.SetWindowHorizontalAlignRight()
				textLine.SetHorizontalAlignRight()

			self.itemList.append(textLine)

			statetextLine = ui.TextLine()
			statetextLine.SetParent(textLine)
			statetextLine.SetText(text2)
			if textColor != 0x00000000:
				statetextLine.SetPackedFontColor(textColor)
			statetextLine.Show()

			if self.itemCenterAlign:
				statetextLine.SetWindowHorizontalAlignCenter()
				statetextLine.SetHorizontalAlignCenter()

			w, h = textLine.GetTextSize()
			if localeInfo.IsARABIC():
				statetextLine.SetPosition(10, 0)
			else:
				statetextLine.SetPosition(w, 0)

			statetextLine.AddFlag("not_pick")

			self.stateList.append(statetextLine)

			self._LocateItem()

class LoginWindow(ui.ScriptWindow):
	IS_TEST = net.IsTest()

	def __init__(self, stream):
		# print "NEW LOGIN WINDOW -----------------------------------------------------------------------------"
		ui.ScriptWindow.__init__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_LOGIN, self)
		net.SetAccountConnectorHandler(self)

		self.lastLoginTime = 0
		self.inputDialog = None
		self.connectingDialog = None
		self.stream = stream
		self.isNowCountDown = False
		self.isStartError = False

		self.xServerBoard = 0
		self.yServerBoard = 0

		self.loadingImage = None

		self.virtualKeyboard = None
		self.virtualKeyboardMode = "ALPHABET"
		self.virtualKeyboardIsUpper = False

		self.timeOutMsg = False;

	def __del__(self):
		net.ClearPhaseWindow(net.PHASE_WINDOW_LOGIN, self)
		net.SetAccountConnectorHandler(0)
		ui.ScriptWindow.__del__(self)
		# print "---------------------------------------------------------------------------- DELETE LOGIN WINDOW"

	def Open(self):
		ServerStateChecker.Create(self)

		# print "LOGIN WINDOW OPEN ----------------------------------------------------------------------------"

		self.loginFailureMsgDict = {
			#"DEFAULT" : localeInfo.LOGIN_FAILURE_UNKNOWN,

			"ALREADY" : localeInfo.LOGIN_FAILURE_ALREAY,
			"NOID" : localeInfo.LOGIN_FAILURE_NOT_EXIST_ID,
			"WRONGPWD" : localeInfo.LOGIN_FAILURE_WRONG_PASSWORD,
			"FULL" : localeInfo.LOGIN_FAILURE_TOO_MANY_USER,
			"SHUTDOWN" : localeInfo.LOGIN_FAILURE_SHUTDOWN,
			"REPAIR" : localeInfo.LOGIN_FAILURE_REPAIR_ID,
			"BLOCK" : localeInfo.LOGIN_FAILURE_BLOCK_ID,
			#"WRONGMAT" : localeInfo.LOGIN_FAILURE_WRONG_MATRIX_CARD_NUMBER,
			"QUIT" : localeInfo.LOGIN_FAILURE_WRONG_MATRIX_CARD_NUMBER_TRIPLE,
			"BESAMEKEY" : localeInfo.LOGIN_FAILURE_BE_SAME_KEY,
			"NOTAVAIL" : localeInfo.LOGIN_FAILURE_NOT_AVAIL,
			"BLKLOGIN" : localeInfo.LOGIN_FAILURE_BLOCK_LOGIN,
			"WEBBLK" : localeInfo.LOGIN_FAILURE_WEB_BLOCK,
			#"BADSCLID" : localeInfo.LOGIN_FAILURE_WRONG_SOCIALID,
			#"AGELIMIT" : localeInfo.LOGIN_FAILURE_SHUTDOWN_TIME,
			#"LOCKED" : localeInfo.LOGIN_FAILURE_LOCKED,
			"BACKENDERR" : localeInfo.LOGIN_FAILURE_BACKEND_ERR,
			"INTEGRTING" : localeInfo.LOGIN_FAILURE_INTEGRTING,
			"COUNTRYERR" : localeInfo.LOGIN_FAILURE_COUNTRY_ERR,
			"IOVATION" : localeInfo.LOGIN_FAILURE_IOVATION_ERR,
			"TNTERR" : localeInfo.LOGIN_FAILURE_TNT_SESSION,
			"SERVER_CLOSED" : localeInfo.LOGIN_FAILURE_SERVER_CLOSED,
			"SERVER_GRADE" : localeInfo.LOGIN_FAILURE_SERVER_GRADE,
		}

		self.loginFailureFuncDict = {
			"WRONGPWD" : self.__DisconnectAndInputPassword,
			#"WRONGMAT" : self.__DisconnectAndInputMatrix,
			"QUIT" : app.Exit,
		}

		if localeInfo.IsEUROPE():
			self.loginFailureMsgDict["CONFIRM"] = localeInfo.LOGIN_FAILURE_NOT_MAIL_CONFIRM

		self.SetSize(wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())
		self.SetWindowName("LoginWindow")

		if not self.__LoadScript(uiScriptLocale.LOCALE_COMMON_UISCRIPT_PATH + "LoginWindow.py"):
			dbg.TraceError("LoginWindow.Open - __LoadScript Error")
			return

		'''
		if app.GetLoginType() == app.LOGIN_TYPE_NONE:
			self.__LoadLoginInfo("loginInfo.xml")
		else:
			app.loggined = False
		'''

		self.__LoadLoginInfo("loginInfo.xml")

		'''
		if debugInfo.IsDebugMode():
			self.__LoadLoginFile("loginInfo.py")
		'''

		if app.loggined:
			self.loginFailureFuncDict = {
				"WRONGPWD" : app.Exit,
				#"WRONGMAT" : app.Exit,
				"QUIT" : app.Exit,
			}

		if musicInfo.loginMusic != "":
			snd.SetMusicVolume(systemSetting.GetMusicVolume())
			snd.FadeInMusic("BGM/"+musicInfo.loginMusic)

		snd.SetSoundVolume(systemSetting.GetSoundVolume())

		# pevent key "[" "]"
		ime.AddExceptKey(91)
		ime.AddExceptKey(93)

		self.Show()

		global SKIP_LOGIN_PHASE
		if SKIP_LOGIN_PHASE:
			if self.isStartError:
				self.connectBoard.Hide()
				self.loginBoard.Hide()
				self.serverBoard.Hide()
				self.PopupNotifyMessage(localeInfo.LOGIN_CONNECT_FAILURE, self.__ExitGame)
				return

			if self.loginInfo:
				self.serverBoard.Hide()
			else:
				self.__RefreshServerList()
				self.__OpenServerBoard()
		else:
			connectingIP = self.stream.GetConnectAddr()
			if connectingIP:
				self.__OpenLoginBoard()
				if IsFullBackImage():
					self.GetChild("bg1").Hide()
					self.GetChild("bg2").Show()
			else:
				self.__RefreshServerList()
				self.__OpenServerBoard()

		if app.ENABLE_LEFT_SEAT:
			if net.IsLeftSeatLogOutState():
				self.PopupNotifyMessage(localeInfo.LEFT_SEAT_LOGOUT)

		app.ShowCursor()

	def Close(self):
		if self.connectingDialog:
			self.connectingDialog.Close()
		self.connectingDialog = None

		ServerStateChecker.Initialize(self)

		# print "---------------------------------------------------------------------------- CLOSE LOGIN WINDOW "
		#
		# selectMusic이 없으면 BGM이 끊기므로 두개 다 체크한다. 
		#
		if musicInfo.loginMusic != "" and musicInfo.selectMusic != "":
			snd.FadeOutMusic("BGM/"+musicInfo.loginMusic)

		## NOTE : idEditLine와 pwdEditLine은 이벤트가 서로 연결 되어있어서
		##		Event를 강제로 초기화 해주어야만 합니다 - [levites]
		self.idEditLine.SetTabEvent(0)
		self.idEditLine.SetReturnEvent(0)
		self.pwdEditLine.SetReturnEvent(0)
		self.pwdEditLine.SetTabEvent(0)

		self.connectBoard = None
		self.loginBoard = None
		self.idEditLine = None
		self.pwdEditLine = None
		self.inputDialog = None
		self.connectingDialog = None
		self.loadingImage = None

		self.serverBoard = None
		self.serverList = None
		self.channelList = None

		self.VIRTUAL_KEY_ALPHABET_LOWERS = None
		self.VIRTUAL_KEY_ALPHABET_UPPERS = None
		self.VIRTUAL_KEY_SYMBOLS = None
		self.VIRTUAL_KEY_NUMBERS = None

		# VIRTUAL_KEYBOARD_BUG_FIX
		if self.virtualKeyboard:
			for keyIndex in xrange(0, VIRTUAL_KEYBOARD_NUM_KEYS+1):
				key = self.GetChild2("key_%d" % keyIndex)
				if key:
					key.SetEvent(None)

			self.GetChild("key_space").SetEvent(None)
			self.GetChild("key_backspace").SetEvent(None)
			self.GetChild("key_enter").SetEvent(None)
			self.GetChild("key_shift").SetToggleDownEvent(None)
			self.GetChild("key_shift").SetToggleUpEvent(None)
			self.GetChild("key_at").SetToggleDownEvent(None)
			self.GetChild("key_at").SetToggleUpEvent(None)

			self.virtualKeyboard = None

		self.KillFocus()
		self.Hide()

		self.stream.popupWindow.Close()
		self.loginFailureFuncDict = None

		ime.ClearExceptKey()

		app.HideCursor()

	def __SaveChannelInfo(self):
		try:
			file = open("channel.inf", "w")
			file.write("%d %d %d" % (self.__GetServerID(), self.__GetChannelID(), self.__GetRegionID()))
		except:
			print "LoginWindow.__SaveChannelInfo - SaveError"

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
			print "LoginWindow.__LoadChannelInfo - OpenError"
			return -1, -1, -1

	def __ExitGame(self):
		app.Exit()

	def SetIDEditLineFocus(self):
		if self.idEditLine != None:
			self.idEditLine.SetFocus()

	def SetPasswordEditLineFocus(self):
		if localeInfo.IsEUROPE():
			if self.idEditLine != None: #0000862: [M2EU] 로그인창 팝업 에러: 종료시 먼저 None 설정됨
				self.idEditLine.SetText("")
				self.idEditLine.SetFocus() #0000685: [M2EU] 아이디/비밀번호 유추 가능 버그 수정: 무조건 아이디로 포커스가 가게 만든다

			if self.pwdEditLine != None: #0000862: [M2EU] 로그인창 팝업 에러: 종료시 먼저 None 설정됨
				self.pwdEditLine.SetText("")
		else:
			if self.pwdEditLine != None:
				self.pwdEditLine.SetFocus()

	def OnEndCountDown(self):
		self.isNowCountDown = False
		if localeInfo.IsBRAZIL():
			self.timeOutMsg = True
		else:
			self.timeOutMsg = False
		self.OnConnectFailure()

	if app.LOGIN_COUNT_DOWN_UI_MODIFY:
		def SetNowCountDown(self, value):
			self.isNowCountDown = value

	def OnConnectFailure(self):
		if self.isNowCountDown:
			return

		snd.PlaySound("sound/ui/loginfail.wav")

		if self.connectingDialog:
			self.connectingDialog.Close()
		self.connectingDialog = None

		if app.loggined:
			self.PopupNotifyMessage(localeInfo.LOGIN_CONNECT_FAILURE, self.__ExitGame)
		elif self.timeOutMsg:
			self.PopupNotifyMessage(localeInfo.LOGIN_FAILURE_TIMEOUT, self.SetPasswordEditLineFocus)
		else:
			self.PopupNotifyMessage(localeInfo.LOGIN_CONNECT_FAILURE, self.SetPasswordEditLineFocus)

	def OnHandShake(self):
		if not IsLoginDelay():
			snd.PlaySound("sound/ui/loginok.wav")
			self.PopupDisplayMessage(localeInfo.LOGIN_CONNECT_SUCCESS)

	def OnLoginStart(self):
		if not IsLoginDelay():
			self.PopupDisplayMessage(localeInfo.LOGIN_PROCESSING)

	def OnLoginFailure(self, error):
		if self.connectingDialog:
			self.connectingDialog.Close()
		self.connectingDialog = None

		try:
			loginFailureMsg = self.loginFailureMsgDict[error]
		except KeyError:
			loginFailureMsg = localeInfo.LOGIN_FAILURE_UNKNOWN + error

		#0000685: [M2EU] 아이디/비밀번호 유추 가능 버그 수정: 무조건 패스워드로 포커스가 가게 만든다
		loginFailureFunc = self.loginFailureFuncDict.get(error, self.SetPasswordEditLineFocus)

		if app.loggined:
			self.PopupNotifyMessage(loginFailureMsg, self.__ExitGame)
		else:
			self.PopupNotifyMessage(loginFailureMsg, loginFailureFunc)

		snd.PlaySound("sound/ui/loginfail.wav")

	def __DisconnectAndInputID(self):
		if self.connectingDialog:
			self.connectingDialog.Close()
		self.connectingDialog = None

		self.SetIDEditLineFocus()
		net.Disconnect()

	def __DisconnectAndInputPassword(self):
		if self.connectingDialog:
			self.connectingDialog.Close()
		self.connectingDialog = None

		self.SetPasswordEditLineFocus()
		net.Disconnect()

	def __LoadScript(self, fileName):
		import dbg
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, fileName)
		except:
			import exception
			exception.Abort("LoginWindow.__LoadScript.LoadObject")
		try:
			GetObject = self.GetChild
			self.serverBoard = GetObject("ServerBoard")
			self.serverSelectButton = GetObject("ServerSelectButton")
			self.serverExitButton = GetObject("ServerExitButton")
			self.connectBoard = GetObject("ConnectBoard")
			self.loginBoard = GetObject("LoginBoard")
			self.idEditLine = GetObject("ID_EditLine")
			self.pwdEditLine = GetObject("Password_EditLine")
			self.serverInfo = GetObject("ConnectName")
			self.selectConnectButton = GetObject("SelectConnectButton")
			self.loginButton = GetObject("LoginButton")
			self.loginExitButton = GetObject("LoginExitButton")

			if app.ENABLE_SERVER_SELECT_RENEWAL:
				self.serverList = ServerListBox()
				self.serverList.SetParent(self.serverBoard)

				if localeInfo.IsARABIC():
					self.serverList.SetPosition(133, 40)
					self.serverList.SetSize(232, 103 if serverInfo.SMALL_SERVER_SELECT_LIST else 171)
					self.serverList.SetTextCenterAlign(0)
				else:
					self.serverList.SetPosition(10, 40)
					self.serverList.SetSize(232, (103 if serverInfo.SMALL_SERVER_SELECT_LIST else 171) + 180)
					self.serverList.SetTextCenterAlign(0)
					self.serverList.SetRowCount(15)
				self.serverList.Show()

				self.channelList = ChannelListBox()
				self.channelList.SetParent(self.serverBoard)

				if localeInfo.IsARABIC():
					self.channelList.SetPosition(10, 40)
					self.channelList.SetSize(109, 112 if serverInfo.SMALL_SERVER_SELECT_LIST else 180)
					self.channelList.SetTextCenterAlign(0)
				else:
					self.channelList.SetPosition(255, 40)
					self.channelList.SetSize(109, 103 if serverInfo.SMALL_SERVER_SELECT_LIST else 171)
					self.channelList.SetTextCenterAlign(0)
				self.channelList.Show()
			else:
				self.serverList = GetObject("ServerList")
				self.channelList = GetObject("ChannelList")

			if localeInfo.IsVIETNAM():
				self.checkButton = GetObject("CheckButton")
				self.checkButton.Down()

			self.virtualKeyboard = self.GetChild2("VirtualKeyboard")

			if self.virtualKeyboard:
				self.VIRTUAL_KEY_ALPHABET_UPPERS = Suffle(localeInfo.VIRTUAL_KEY_ALPHABET_UPPERS)
				self.VIRTUAL_KEY_ALPHABET_LOWERS = "".join([localeInfo.VIRTUAL_KEY_ALPHABET_LOWERS[localeInfo.VIRTUAL_KEY_ALPHABET_UPPERS.index(e)] for e in self.VIRTUAL_KEY_ALPHABET_UPPERS])
				if localeInfo.IsBRAZIL():
					self.VIRTUAL_KEY_SYMBOLS_BR = Suffle(localeInfo.VIRTUAL_KEY_SYMBOLS_BR)
				else:
					self.VIRTUAL_KEY_SYMBOLS = Suffle(localeInfo.VIRTUAL_KEY_SYMBOLS)
				self.VIRTUAL_KEY_NUMBERS = Suffle(localeInfo.VIRTUAL_KEY_NUMBERS)
				self.__VirtualKeyboard_SetAlphabetMode()

				self.GetChild("key_space").SetEvent(lambda : self.__VirtualKeyboard_PressKey(' '))
				self.GetChild("key_backspace").SetEvent(lambda : self.__VirtualKeyboard_PressBackspace())
				self.GetChild("key_enter").SetEvent(lambda : self.__VirtualKeyboard_PressReturn())
				self.GetChild("key_shift").SetToggleDownEvent(lambda : self.__VirtualKeyboard_SetUpperMode())
				self.GetChild("key_shift").SetToggleUpEvent(lambda : self.__VirtualKeyboard_SetLowerMode())
				self.GetChild("key_at").SetToggleDownEvent(lambda : self.__VirtualKeyboard_SetSymbolMode())
				self.GetChild("key_at").SetToggleUpEvent(lambda : self.__VirtualKeyboard_SetAlphabetMode())

		except:
			import exception
			exception.Abort("LoginWindow.__LoadScript.BindObject")

		if self.IS_TEST:
			self.selectConnectButton.Hide()
		else:
			self.selectConnectButton.SetEvent(ui.__mem_func__(self.__OnClickSelectConnectButton))

		self.serverBoard.OnKeyUp = ui.__mem_func__(self.__ServerBoard_OnKeyUp)
		self.xServerBoard, self.yServerBoard = self.serverBoard.GetLocalPosition()

		self.serverSelectButton.SetEvent(ui.__mem_func__(self.__OnClickSelectServerButton))
		self.serverExitButton.SetEvent(ui.__mem_func__(self.__OnClickExitButton))

		self.loginButton.SetEvent(ui.__mem_func__(self.__OnClickLoginButton))
		self.loginExitButton.SetEvent(ui.__mem_func__(self.__OnClickExitButton))

		self.serverList.SetEvent(ui.__mem_func__(self.__OnSelectServer))

		self.idEditLine.SetReturnEvent(ui.__mem_func__(self.pwdEditLine.SetFocus))
		self.idEditLine.SetTabEvent(ui.__mem_func__(self.pwdEditLine.SetFocus))

		self.pwdEditLine.SetReturnEvent(ui.__mem_func__(self.__OnClickLoginButton))
		self.pwdEditLine.SetTabEvent(ui.__mem_func__(self.idEditLine.SetFocus))

		if IsFullBackImage():
			self.GetChild("bg1").Show()
			self.GetChild("bg2").Hide()

		return 1

	def __VirtualKeyboard_SetKeys(self, keyCodes):
		uiDefFontBackup = localeInfo.UI_DEF_FONT
		localeInfo.UI_DEF_FONT = localeInfo.UI_DEF_FONT_LARGE

		keyIndex = 1
		for keyCode in keyCodes:
			key = self.GetChild2("key_%d" % keyIndex)
			if key:
				key.SetEvent(lambda x=keyCode: self.__VirtualKeyboard_PressKey(x))
				key.SetText(keyCode)
				key.ButtonText.SetFontColor(0, 0, 0)
				keyIndex += 1

		for keyIndex in xrange(keyIndex, VIRTUAL_KEYBOARD_NUM_KEYS+1):
			key = self.GetChild2("key_%d" % keyIndex)
			if key:
				key.SetEvent(lambda x=' ': self.__VirtualKeyboard_PressKey(x))
				key.SetText(' ')

		localeInfo.UI_DEF_FONT = uiDefFontBackup

	def __VirtualKeyboard_PressKey(self, code):
		ime.PasteString(code)

		# if self.virtualKeyboardMode == "ALPHABET" and self.virtualKeyboardIsUpper:
			# self.__VirtualKeyboard_SetLowerMode()

	def __VirtualKeyboard_PressBackspace(self):
		ime.PasteBackspace()

	def __VirtualKeyboard_PressReturn(self):
		ime.PasteReturn()

	def __VirtualKeyboard_SetUpperMode(self):
		self.virtualKeyboardIsUpper = True

		if self.virtualKeyboardMode == "ALPHABET":
			self.__VirtualKeyboard_SetKeys(self.VIRTUAL_KEY_ALPHABET_UPPERS)
		elif self.virtualKeyboardMode == "NUMBER":
			if localeInfo.IsBRAZIL():
				self.__VirtualKeyboard_SetKeys(self.VIRTUAL_KEY_SYMBOLS_BR)
			else:
				self.__VirtualKeyboard_SetKeys(self.VIRTUAL_KEY_SYMBOLS)
		else:
			self.__VirtualKeyboard_SetKeys(self.VIRTUAL_KEY_NUMBERS)

	def __VirtualKeyboard_SetLowerMode(self):
		self.virtualKeyboardIsUpper = False

		if self.virtualKeyboardMode == "ALPHABET":
			self.__VirtualKeyboard_SetKeys(self.VIRTUAL_KEY_ALPHABET_LOWERS)
		elif self.virtualKeyboardMode == "NUMBER":
			self.__VirtualKeyboard_SetKeys(self.VIRTUAL_KEY_NUMBERS)
		else:
			if localeInfo.IsBRAZIL():
				self.__VirtualKeyboard_SetKeys(self.VIRTUAL_KEY_SYMBOLS_BR)
			else:
				self.__VirtualKeyboard_SetKeys(self.VIRTUAL_KEY_SYMBOLS)

	def __VirtualKeyboard_SetAlphabetMode(self):
		self.virtualKeyboardIsUpper = False
		self.virtualKeyboardMode = "ALPHABET"
		self.__VirtualKeyboard_SetKeys(self.VIRTUAL_KEY_ALPHABET_LOWERS)

	def __VirtualKeyboard_SetNumberMode(self):
		self.virtualKeyboardIsUpper = False
		self.virtualKeyboardMode = "NUMBER"
		self.__VirtualKeyboard_SetKeys(self.VIRTUAL_KEY_NUMBERS)

	def __VirtualKeyboard_SetSymbolMode(self):
		self.virtualKeyboardIsUpper = False
		self.virtualKeyboardMode = "SYMBOL"
		if localeInfo.IsBRAZIL():
			self.__VirtualKeyboard_SetKeys(self.VIRTUAL_KEY_SYMBOLS_BR)
		else:
			self.__VirtualKeyboard_SetKeys(self.VIRTUAL_KEY_SYMBOLS)

	def Connect(self, id, pwd):
		if app.ENABLE_SERVER_SELECT_RENEWAL:
			regionID = self.__GetRegionID()
			serverID = self.__GetServerID()
			channelID = self.__GetChannelID()

			if (serverInfo.REGION_DICT.has_key(regionID)) and (serverInfo.REGION_DICT[regionID].has_key(serverID)):
				try:
					channelDict = serverInfo.REGION_DICT[regionID][serverID]["channel"]
				except KeyError:
					return

				try:
					state = channelDict[channelID]["state"]
				except KeyError:
					self.PopupNotifyMessage(localeInfo.CHANNEL_SELECT_CHANNEL)
					return

				# 상태가 CLOSED면과 같으면 진입 금지
				tokens = serverInfo.REGION_DICT[regionID][serverID].get("state", "NONE").split("|")
				is_server_closed = False

				for idx in xrange(len(tokens)):
					if tokens[idx].strip() == "CLOSE":
						is_server_closed = True

				if is_server_closed == True:
					self.PopupNotifyMessage(localeInfo.SERVER_NOTIFY_CLOSED)
					return

				# 상태가 FULL 과 같으면 진입 금지
				if state == serverInfo.STATE_DICT[len(serverInfo.STATE_DICT) - 1]:
					self.PopupNotifyMessage(localeInfo.CHANNEL_NOTIFY_FULL)
					return

		if constInfo.SEQUENCE_PACKET_ENABLE:
			net.SetPacketSequenceMode()

		if IsLoginDelay():
			loginDelay = GetLoginDelay()
			self.connectingDialog = ConnectingDialog()
			self.connectingDialog.Open(loginDelay)
			self.connectingDialog.SAFE_SetTimeOverEvent(self.OnEndCountDown)
			self.connectingDialog.SAFE_SetExitEvent(self.OnPressExitKey)
			self.isNowCountDown = True
		else:
			self.stream.popupWindow.Close()
			self.stream.popupWindow.Open(localeInfo.LOGIN_CONNETING, self.SetPasswordEditLineFocus, localeInfo.UI_CANCEL)

		self.stream.SetLoginInfo(id, pwd)
		if app.LOGIN_COUNT_DOWN_UI_MODIFY:
			connect_result = self.stream.Connect()
			if not connect_result:
				self.isNowCountDown = False
		else:
			self.stream.Connect()

	def __OnClickExitButton(self):
		self.stream.SetPhaseWindow(0)

	def __SetServerInfo(self, name):
		net.SetServerInfo(name.strip())
		self.serverInfo.SetText(name)

	def __LoadLoginInfo(self, loginInfoFileName):
		def getValue(element, name, default):
			if [] != element.getElementsByTagName(name):
				return element.getElementsByTagName(name).item(0).firstChild.nodeValue
			else:
				return default

		self.id = None
		self.pwd = None
		self.loginnedServer = None
		self.loginnedChannel = None
		app.loggined = False

		self.loginInfo = True

		from xml.dom.minidom import parse
		try:
			f = open(loginInfoFileName, "r")
			dom = parse(f)
		except:
			return
		serverLst = dom.getElementsByTagName("server")
		if [] != dom.getElementsByTagName("logininfo"):
			logininfo = dom.getElementsByTagName("logininfo")[0]
		else:
			return

		try:
			server_name = logininfo.getAttribute("name")
			channel_idx = int(logininfo.getAttribute("channel_idx"))
		except:
			return

		try:
			matched = False

			for k, v in serverInfo.REGION_DICT[0].iteritems():
				if v["name"] == server_name:
					account_addr = serverInfo.REGION_AUTH_SERVER_DICT[0][k]["ip"]
					account_port = serverInfo.REGION_AUTH_SERVER_DICT[0][k]["port"]

					channel_info = v["channel"][channel_idx]
					channel_name = channel_info["name"]
					addr = channel_info["ip"]
					port = channel_info["tcp_port"]

					net.SetMarkServer(addr, port)
					self.stream.SetConnectInfo(addr, port, account_addr, account_port)

					matched = True
					break

			if False == matched:
				return
		except:
			return

		if app.ENABLE_MOVE_CHANNEL:
			self.serverInfo.SetText("%s, %s " % (server_name, channel_name))
		else:
			self.__SetServerInfo("%s, %s " % (server_name, channel_name))

		id = getValue(logininfo, "id", "")
		pwd = getValue(logininfo, "pwd", "")
		self.idEditLine.SetText(id)
		self.pwdEditLine.SetText(pwd)
		slot = getValue(logininfo, "slot", "0")
		locale = getValue(logininfo, "locale", "")
		locale_dir = getValue(logininfo, "locale_dir", "")
		is_auto_login = int(getValue(logininfo, "auto_login", "0"))

		self.stream.SetCharacterSlot(int(slot))
		self.stream.isAutoLogin = is_auto_login
		self.stream.isAutoSelect = is_auto_login

		if locale and locale_dir:
			app.ForceSetLocale(locale, locale_dir)

		if 0 != is_auto_login:
			self.Connect(id, pwd)

	'''
	if debugInfo.IsDebugMode():
		def __LoadLoginFile(self, loginInfoFileName):
			try:
				loginInfo = {}
				execfile(loginInfoFileName, loginInfo)
			except IOError:
				print(\
					"자동 로그인을 하시려면" + loginInfoFileName + "파일을 작성해주세요\n"\
					"\n"\
					"내용:\n"\
					"================================================================\n"
				);

			self.id = None
			self.pwd = None
			self.loginnedServer = None
			self.loginnedChannel = None
			app.loggined = False

			self.loginInfo = loginInfo
	'''

	def PopupDisplayMessage(self, msg):
		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(msg)

	def PopupNotifyMessage(self, msg, func = 0):
		if not func:
			func = self.EmptyFunc

		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(msg, func, localeInfo.UI_OK)

	def __OnCloseInputDialog(self):
		if self.inputDialog:
			self.inputDialog.Close()
		self.inputDialog = None
		return True

	def OnPressExitKey(self):
		self.stream.popupWindow.Close()
		self.stream.SetPhaseWindow(0)
		return True

	def OnExit(self):
		self.stream.popupWindow.Close()

	def OnUpdate(self):
		ServerStateChecker.Update()

	def EmptyFunc(self):
		pass

	#####################################################################################

	def __ServerBoard_OnKeyUp(self, key):
		if self.serverBoard.IsShow():
			if app.DIK_RETURN==key:
				self.__OnClickSelectServerButton()
		return True

	def __GetRegionID(self):
		return 0

	def __GetServerID(self):
		return self.serverList.GetSelectedItem()

	def __GetChannelID(self):
		return self.channelList.GetSelectedItem()

	# SEVER_LIST_BUG_FIX
	def __ServerIDToServerIndex(self, regionID, targetServerID):
		try:
			regionDict = serverInfo.REGION_DICT[regionID]
		except KeyError:
			return -1

		retServerIndex = 0
		for eachServerID, regionDataDict in regionDict.items():
			if eachServerID == targetServerID:
				return retServerIndex

			retServerIndex += 1

		return -1

	def __ChannelIDToChannelIndex(self, channelID):
		return channelID - 1
	# END_OF_SEVER_LIST_BUG_FIX

	def __OpenServerBoard(self):
		loadRegionID, loadServerID, loadChannelID = self.__LoadChannelInfo()

		serverIndex = self.__ServerIDToServerIndex(loadRegionID, loadServerID)
		channelIndex = self.__ChannelIDToChannelIndex(loadChannelID)

		# 신규유저 서버 선택 로직.
		if app.ENABLE_SERVER_SELECT_RENEWAL and serverIndex == -1: # 선택된 서버가 없을 때 신규유저라고 판단
			newIdx = serverInfo.SERVER_STATE_DICT.get("NEW", 0)
			specialIdx = serverInfo.SERVER_STATE_DICT.get("SPECIAL", 0)
			closeIdx = serverInfo.SERVER_STATE_DICT.get("CLOSE", 0)
			standbyIdx = serverInfo.SERVER_STATE_DICT.get("STANDBY", 0)
			serverSelect1st = -1;
			serverSelect2nd = -1;
			serverSelect3rd = -1;
			serverSelect4th = -1;

			for idx in xrange(self.serverList.GetItemCount()):
				(state, state2) = self.serverList.GetState(idx)
				if state == newIdx or state2 == newIdx: # 1순위로 NEW를 선택한다.
					serverSelect1st = idx
					break
				elif serverSelect2nd == -1 and (state == specialIdx or state2 == specialIdx): # 2순위로 SPECIAL를 선택한다.
					serverSelect2nd = idx
				elif serverSelect3rd == -1 and (state != closeIdx and state2 != closeIdx): # 3순위 접속가능한 서버 중 최상위.
					serverSelect3rd = idx
				elif serverSelect4th == -1 and (state != standbyIdx and state2 != standbyIdx):
					serverSelect4th = idx

			if serverSelect1st != -1:
				serverIndex = serverSelect1st
			elif serverSelect2nd != -1:
				serverIndex = serverSelect2nd
			elif serverSelect3rd != -1:
				serverIndex = serverSelect3rd
			elif serverSelect4th != -1:
				serverIndex = serverSelect4th

		self.serverList.SelectItem(serverIndex)

		if localeInfo.IsEUROPE():
			if app.ENABLE_NEW_USER_CARE:
				self.channelList.SelectItem(channelIndex)
			else:
				self.channelList.SelectItem(app.GetRandom(0, self.channelList.GetItemCount()))
		else:
			if channelIndex >= 0:
				self.channelList.SelectItem(channelIndex)

		## Show/Hide 코드에 문제가 있어서 임시 - [levites]
		self.serverBoard.SetPosition(self.xServerBoard, self.yServerBoard)
		self.serverBoard.Show()
		self.connectBoard.Hide()
		self.loginBoard.Hide()

		if self.virtualKeyboard:
			self.virtualKeyboard.Hide()

		if app.loggined and not SKIP_LOGIN_PHASE_SUPPORT_CHANNEL:
			self.serverList.SelectItem(self.loginnedServer - 1)
			self.channelList.SelectItem(self.loginnedChannel - 1)
			self.__OnClickSelectServerButton()

	def __OpenLoginBoard(self):
		self.serverExitButton.SetEvent(ui.__mem_func__(self.__OnClickExitServerButton))
		self.serverExitButton.SetText(localeInfo.UI_CLOSE)

		self.serverBoard.SetPosition(self.xServerBoard, wndMgr.GetScreenHeight())
		self.serverBoard.Hide()

		if self.virtualKeyboard:
			self.virtualKeyboard.Show()

		if app.loggined:
			self.Connect(self.id, self.pwd)
			self.connectBoard.Hide()
			self.loginBoard.Hide()
		elif not self.stream.isAutoLogin:
			self.connectBoard.Show()
			self.loginBoard.Show()

		## if users have the login infomation, then don't initialize.2005.9 haho
		if self.idEditLine == None:
			self.idEditLine.SetText("")
		if self.pwdEditLine == None:
			self.pwdEditLine.SetText("")

		self.idEditLine.SetFocus()

		global SKIP_LOGIN_PHASE
		if SKIP_LOGIN_PHASE:
			if not self.loginInfo:
				self.connectBoard.Hide()

	def __OnSelectRegionGroup(self):
		self.__RefreshServerList()

	def __OnSelectSettlementArea(self):
		# SEVER_LIST_BUG_FIX
		regionID = self.__GetRegionID()
		serverID = self.serverListOnRegionBoard.GetSelectedItem()

		serverIndex = self.__ServerIDToServerIndex(regionID, serverID)
		self.serverList.SelectItem(serverIndex)
		# END_OF_SEVER_LIST_BUG_FIX

		self.__OnSelectServer()

	def __RefreshServerList(self):
		regionID = self.__GetRegionID()

		if not serverInfo.REGION_DICT.has_key(regionID):
			return

		self.serverList.ClearItem()

		regionDict = serverInfo.REGION_DICT[regionID]

		# SEVER_LIST_BUG_FIX
		visible_index = 1
		if app.ENABLE_SERVER_SELECT_RENEWAL:
			for id, regionDataDict in regionDict.items():
				name = regionDataDict.get("name", "noname")
				state = 0
				state2 = 0
				flag = regionDataDict.get("flag", "ALL")

				tokens = regionDataDict.get("state", "NONE").split("|")
				if len(tokens) == 1:
					state = serverInfo.SERVER_STATE_DICT.get(tokens[0].strip(), 0)
				elif len(tokens) == 2:
					state = serverInfo.SERVER_STATE_DICT.get(tokens[0].strip(), 0)
					state2 = serverInfo.SERVER_STATE_DICT.get(tokens[1].strip(), 0)

				if localeInfo.IsBRAZIL() or localeInfo.IsCANADA():
					self.serverList.InsertItem(id, "%s" % (name))
				else:
					if localeInfo.IsCIBN10():
						if name[0] == "#":
							self.serverList.InsertItem(-1, "  %s" % (name[1:]), 1)
						else:
							self.serverList.InsertItem(id, "  %s" % (name), 2)
							visible_index += 1
					else:
						try:
							server_id = serverInfo.SERVER_ID_DICT[id]
						except:
							server_id = visible_index

						self.serverList.InsertItem(id, "  %s" % (name), state, state2, flag) # 서버의 상태 대한 값 설정.
						# self.serverList.InsertItem(id, "  %02d. %s" % (int(server_id), name), state, state2, locale) # 서버의 상태 대한 값 설정.
						#self.serverList.InsertItem(id, "  %s" % (name), state, state2)	# 서버의 상태 대한 값 설정.

						visible_index += 1
		else:
			for id, regionDataDict in regionDict.items():
				name = regionDataDict.get("name", "noname")
				if localeInfo.IsBRAZIL() or localeInfo.IsCANADA():
					self.serverList.InsertItem(id, "%s" % (name))
				else:
					if localeInfo.IsCIBN10():
						if name[0] == "#":
							self.serverList.InsertItem(-1, "  %s" % (name[1:]))
						else:
							self.serverList.InsertItem(id, "  %s" % (name))
							visible_index += 1
					else:
						try:
							server_id = serverInfo.SERVER_ID_DICT[id]
						except:
							server_id = visible_index

						self.serverList.InsertItem(id, "  %02d. %s" % (int(server_id), name))

						visible_index += 1
		# END_OF_SEVER_LIST_BUG_FIX

	def __OnSelectServer(self):
		self.__OnCloseInputDialog()
		self.__RequestServerStateList()
		self.__RefreshServerStateList()

	def __RequestServerStateList(self):
		regionID = self.__GetRegionID()
		serverID = self.__GetServerID()

		try:
			channelDict = serverInfo.REGION_DICT[regionID][serverID]["channel"]
		except:
			print " __RequestServerStateList - serverInfo.REGION_DICT(%d, %d)" % (regionID, serverID)
			return

		ServerStateChecker.Initialize();
		# ServerStateChecker.SetAuth(serverInfo.REGION_AUTH_SERVER_DICT[regionID][serverID]["ip"], serverInfo.REGION_AUTH_SERVER_DICT[regionID][serverID]["port"])
		for id, channelDataDict in channelDict.items():
			key = channelDataDict["key"]
			ip = channelDataDict["ip"]
			udp_port = channelDataDict["udp_port"]
			ServerStateChecker.AddChannel(key, ip, udp_port)

		ServerStateChecker.Request()

	def __RefreshServerStateList(self):
		regionID = self.__GetRegionID()
		serverID = self.__GetServerID()
		bakChannelID = self.channelList.GetSelectedItem()

		self.channelList.ClearItem()

		try:
			channelDict = serverInfo.REGION_DICT[regionID][serverID]["channel"]
		except:
			print " __RequestServerStateList - serverInfo.REGION_DICT(%d, %d)" % (regionID, serverID)
			return

		for channelID, channelDataDict in channelDict.items():
			channelName = channelDataDict["name"]
			channelState = channelDataDict["state"]
			if app.ENABLE_SERVER_SELECT_RENEWAL:
				channelStateColor = serverInfo.STATE_COLOR_DICT[channelState]
				self.channelList.InsertItem(channelID, " %s" % channelName, " %s" % channelState, channelStateColor)
			elif app.ENABLE_CHANNEL_LIST:
				channelStateColor = serverInfo.STATE_COLOR_DICT[channelState]
				self.channelList.InsertItem(channelID, " %s %s" % (channelName, channelState), channelStateColor)
			else:
				self.channelList.InsertItem(channelID, " %s %s" % (channelName, channelState))

		self.channelList.SelectItem(bakChannelID - 1)

	if app.ENABLE_CHANNEL_LIST or app.ENABLE_SERVER_SELECT_RENEWAL:
		def AutoSelectChannel(self):
			regionID = self.__GetRegionID()
			serverID = self.__GetServerID()
			bakChannelID = self.channelList.GetSelectedItem()

			try:
				channelDict = serverInfo.REGION_DICT[regionID][serverID]["channel"]
			except:
				print " __RequestServerStateList - serverInfo.REGION_DICT(%d, %d)" % (regionID, serverID)
				return

			if bakChannelID>len(channelDict):
				bakChannelID = 0

			if app.ENABLE_SERVER_SELECT_RENEWAL:
				if bakChannelID > 0:
					self.channelList.SelectItem(bakChannelID - 1)
					return

				mostID = 0
				most = 0
				secondMostID = 0
				secondMost = 0

				for channelID, channelDataDict in channelDict.items():
					channelState = serverInfo.STATE_REVERSE_DICT.get(channelDataDict["state"], 0)

					if mostID == 0:
						mostID = channelID
						most = channelState
					elif channelState > most:
						secondMostID = mostID
						secondMost = most
						mostID = channelID
						most = channelState
					elif secondMostID == 0 or channelState > secondMost:
						secondMostID = channelID
						secondMost = channelState

				if secondMost <= serverInfo.STATE_REVERSE_DICT.get("Vacant", 2):
					self.channelList.SelectItem(mostID - 1)
				else:
					self.channelList.SelectItem(secondMostID - 1)
			else:
				AvailableChannelID = 0
				BusyChannelID = 0

				for channelID, channelDataDict in channelDict.items():
					channelState = channelDataDict["state"]

					if AvailableChannelID == 0 and channelState == serverInfo.STATE_DICT[1]:
						AvailableChannelID = channelID

					if BusyChannelID == 0 and channelState == serverInfo.STATE_DICT[2]:
						BusyChannelID = channelID

					# 상태가 FULL이나 OFFLINE이면 다른 채널을 선택하도록 함
					if bakChannelID == channelID and (channelState == serverInfo.STATE_DICT[0] or channelState == serverInfo.STATE_DICT[3]):
						bakChannelID = 0

				if bakChannelID == 0:
					if AvailableChannelID != 0:
						self.channelList.SelectItem(AvailableChannelID - 1)
					elif BusyChannelID != 0:
						self.channelList.SelectItem(BusyChannelID - 1)
					else:
						self.channelList.SelectItem(0)
				else:
					self.channelList.SelectItem(bakChannelID - 1)

	def __GetChannelName(self, regionID, selServerID, selChannelID):
		try:
			return serverInfo.REGION_DICT[regionID][selServerID]["channel"][selChannelID]["name"]
		except KeyError:
			if 9 == selChannelID:
				return localeInfo.CHANNEL_PVP
			else:
				return localeInfo.CHANNEL_NORMAL % (selChannelID)

	def NotifyChannelState(self, addrKey, state):
		try:
			stateName = serverInfo.STATE_DICT[state]
		except:
			stateName = serverInfo.STATE_NONE

		regionID = self.__GetRegionID()
		serverID = self.__GetServerID()
		channelID = addrKey % 10

		try:
			serverInfo.REGION_DICT[regionID][serverID]["channel"][channelID]["state"] = stateName
			self.__RefreshServerStateList()

		except:
			import exception
			exception.Abort(localeInfo.CHANNEL_NOT_FIND_INFO)

	def __OnClickExitServerButton(self):
		print "exit server"
		self.__OpenLoginBoard()

		if IsFullBackImage():
			self.GetChild("bg1").Hide()
			self.GetChild("bg2").Show()

	def __OnClickSelectRegionButton(self):
		regionID = self.__GetRegionID()
		serverID = self.__GetServerID()

		if (not serverInfo.REGION_DICT.has_key(regionID)):
			self.PopupNotifyMessage(localeInfo.CHANNEL_SELECT_REGION)
			return

		if (not serverInfo.REGION_DICT[regionID].has_key(serverID)):
			self.PopupNotifyMessage(localeInfo.CHANNEL_SELECT_SERVER)
			return

		self.__SaveChannelInfo()

		self.serverExitButton.SetEvent(ui.__mem_func__(self.__OnClickExitServerButton))
		self.serverExitButton.SetText(localeInfo.UI_CLOSE)

		self.__RefreshServerList()
		self.__OpenServerBoard()

	def __OnClickSelectServerButton(self):
		if IsFullBackImage():
			self.GetChild("bg1").Hide()
			self.GetChild("bg2").Show()

		regionID = self.__GetRegionID()
		serverID = self.__GetServerID()
		channelID = self.__GetChannelID()

		if (not serverInfo.REGION_DICT.has_key(regionID)):
			self.PopupNotifyMessage(localeInfo.CHANNEL_SELECT_REGION)
			return

		if (not serverInfo.REGION_DICT[regionID].has_key(serverID)):
			self.PopupNotifyMessage(localeInfo.CHANNEL_SELECT_SERVER)
			return

		try:
			channelDict = serverInfo.REGION_DICT[regionID][serverID]["channel"]
		except KeyError:
			return

		try:
			state = channelDict[channelID]["state"]
		except KeyError:
			self.PopupNotifyMessage(localeInfo.CHANNEL_SELECT_CHANNEL)
			return

		if app.ENABLE_SERVER_SELECT_RENEWAL:
			tokens = serverInfo.REGION_DICT[regionID][serverID].get("state", "NONE").split("|")
			is_server_closed = False

			for idx in xrange(len(tokens)):
				if tokens[idx].strip() == "CLOSE":
					is_server_closed = True

			# 상태가 CLOSED면 진입 금지
			if is_server_closed == True:
				self.PopupNotifyMessage(localeInfo.SERVER_NOTIFY_CLOSED)
				return

			# 상태가 FULL 과 같으면 진입 금지
			if state == serverInfo.STATE_DICT[len(serverInfo.STATE_DICT) - 1]:
				self.PopupNotifyMessage(localeInfo.CHANNEL_NOTIFY_FULL)
				return
		else:
			# 상태가 FULL 과 같으면 진입 금지
			if state == serverInfo.STATE_DICT[3]:
				self.PopupNotifyMessage(localeInfo.CHANNEL_NOTIFY_FULL)
				return

		self.__SaveChannelInfo()

		try:
			serverName = serverInfo.REGION_DICT[regionID][serverID]["name"]
			channelName = serverInfo.REGION_DICT[regionID][serverID]["channel"][channelID]["name"]
			addrKey = serverInfo.REGION_DICT[regionID][serverID]["channel"][channelID]["key"]

			if "천마 서버" == serverName:
				app.ForceSetLocale("ymir", "locale/ymir")
			elif "쾌도 서버" == serverName:
				app.ForceSetLocale("we_korea", "locale/we_korea")

		except:
			print " ERROR __OnClickSelectServerButton(%d, %d, %d)" % (regionID, serverID, channelID)
			serverName = localeInfo.CHANNEL_EMPTY_SERVER
			channelName = localeInfo.CHANNEL_NORMAL % channelID

		self.__SetServerInfo("%s, %s " % (serverName, channelName))

		try:
			ip = serverInfo.REGION_DICT[regionID][serverID]["channel"][channelID]["ip"]
			tcp_port = serverInfo.REGION_DICT[regionID][serverID]["channel"][channelID]["tcp_port"]
		except:
			import exception
			exception.Abort("LoginWindow.__OnClickSelectServerButton - 서버 선택 실패")

		try:
			account_ip = serverInfo.REGION_AUTH_SERVER_DICT[regionID][serverID]["ip"]
			account_port = serverInfo.REGION_AUTH_SERVER_DICT[regionID][serverID]["port"]
		except:
			account_ip = 0
			account_port = 0

		try:
			markKey = regionID * 1000 + serverID * 10
			markAddrValue=serverInfo.MARKADDR_DICT[markKey]
			print("MarkAddrValue %s" % str(markKey))
			net.SetMarkServer(markAddrValue["ip"], markAddrValue["tcp_port"])
			app.SetGuildMarkPath(markAddrValue["mark"])
			# GUILD_SYMBOL
			app.SetGuildSymbolPath(markAddrValue["symbol_path"])
			# END_OF_GUILD_SYMBOL

		except:
			import exception
			exception.Abort("LoginWindow.__OnClickSelectServerButton - 마크 정보 없음")

		self.stream.SetConnectInfo(ip, tcp_port, account_ip, account_port)
		print("SetConnectInfo %s, %s, %s, %s" % (str(ip), str(tcp_port), str(account_ip), str(account_port)))
		self.__OpenLoginBoard()

	def __OnClickSelectConnectButton(self):
		if IsFullBackImage():
			self.GetChild("bg1").Show()
			self.GetChild("bg2").Hide()

		self.__RefreshServerList()
		self.__OpenServerBoard()

	def __OnClickLoginButton(self):
		id = self.idEditLine.GetText()
		pwd = self.pwdEditLine.GetText()

		if len(id) == 0:
			self.PopupNotifyMessage(localeInfo.LOGIN_INPUT_ID, self.SetIDEditLineFocus)
			return

		if len(pwd) == 0:
			self.PopupNotifyMessage(localeInfo.LOGIN_INPUT_PASSWORD, self.SetPasswordEditLineFocus)
			return

		self.Connect(id, pwd)

	def SameLogin_OpenUI(self):
		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(localeInfo.LOGIN_FAILURE_SAMELOGIN, 0, localeInfo.UI_OK)
