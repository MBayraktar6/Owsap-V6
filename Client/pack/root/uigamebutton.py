import app
import ui
import player
import net
import localeInfo

if app.ENABLE_POPUP_NOTICE:
	import uiScriptLocale
	import uiToolTip
	import wndMgr

class GameButtonWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadWindow("UIScript/gamewindow.py")

		if app.ENABLE_POPUP_NOTICE:
			self.popup_notice_window = None

			self.popup_notice_icon_anim_img = None
			self.is_popup_notice_btn_blink = False

			self.game_button_tooltip = uiToolTip.ToolTip()
			self.game_button_tooltip.Hide()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadWindow(self, filename):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, filename)

		except Exception, msg:
			import dbg
			dbg.TraceError("GameButtonWindow.LoadScript - %s" % (msg))
			app.Abort()
			return False

		try:
			self.gameButtonDict = {
				"STATUS" : self.GetChild("StatusPlusButton"),
				"SKILL" : self.GetChild("SkillPlusButton"),
				"QUEST" : self.GetChild("QuestButton"),
				"HELP" : self.GetChild("HelpButton"),
				"BUILD" : self.GetChild("BuildGuildBuilding"),
				"EXIT_OBSERVER" : self.GetChild("ExitObserver"),
			}
			if app.ENABLE_POPUP_NOTICE:
				self.gameButtonDict.update({ "POPUP_NOTICE" : self.GetChild("PopupNoticeButton") })
				self.popup_notice_icon_anim_img = self.GetChild("PopupNoticeAnimImage")

			self.gameButtonDict["EXIT_OBSERVER"].SetEvent(ui.__mem_func__(self.__OnClickExitObserver))

			if app.ENABLE_GEM_SYSTEM:
				posx, posy = self.gameButtonDict["SKILL"].GetGlobalPosition()
				self.gameButtonDict["SKILL"].SetPosition(posx, posy - 25)

				posx, posy = self.gameButtonDict["BUILD"].GetGlobalPosition()
				self.gameButtonDict["BUILD"].SetPosition(posx, posy - 25)

				posx, posy = self.gameButtonDict["EXIT_OBSERVER"].GetGlobalPosition()
				self.gameButtonDict["EXIT_OBSERVER"].SetPosition(posx, posy - 25)

			if app.ENABLE_POPUP_NOTICE:
				if localeInfo.IsARABIC():
					self.gameButtonDict["STATUS"].SetWindowHorizontalAlignLeft()

					self.gameButtonDict["SKILL"].SetWindowHorizontalAlignRight()
					posx, posy = self.gameButtonDict["SKILL"].GetGlobalPosition()
					self.gameButtonDict["SKILL"].SetPosition(64 + 10, posy)

					self.gameButtonDict["POPUP_NOTICE"].SetWindowHorizontalAlignRight()
					posx, posy = self.gameButtonDict["POPUP_NOTICE"].GetGlobalPosition()
					self.gameButtonDict["POPUP_NOTICE"].SetPosition(32 + 5, posy)
					self.popup_notice_icon_anim_img.SetPosition(32 + 5, posy)

		except Exception, msg:
			import dbg
			dbg.TraceError("GameButtonWindow.LoadScript - %s" % (msg))
			app.Abort()
			return False

		self.__HideAllGameButton()
		self.SetObserverMode(player.IsObserverMode())
		return True

	def Destroy(self):
		for key in self.gameButtonDict:
			self.gameButtonDict[key].SetEvent(0)

		self.gameButtonDict = {}

	def SetButtonEvent(self, name, event):
		try:
			self.gameButtonDict[name].SetEvent(event)

		except Exception, msg:
			print "GameButtonWindow.LoadScript - %s" % (msg)
			app.Abort()
			return

	def ShowBuildButton(self):
		self.gameButtonDict["BUILD"].Show()

	def HideBuildButton(self):
		self.gameButtonDict["BUILD"].Hide()

	def CheckGameButton(self):
		if not self.IsShow():
			return

		statusPlusButton = self.gameButtonDict["STATUS"]
		skillPlusButton = self.gameButtonDict["SKILL"]
		helpButton = self.gameButtonDict["HELP"]

		if player.GetStatus(player.STAT) > 0:
			statusPlusButton.Show()
		else:
			statusPlusButton.Hide()

		if localeInfo.IsARABIC():
			statusPlusButton.SetWindowHorizontalAlignLeft()

		if self.__IsSkillStat():
			skillPlusButton.Show()
		else:
			skillPlusButton.Hide()

		if 0 == player.GetPlayTime():
			helpButton.Show()
		else:
			helpButton.Hide()

		if app.ENABLE_POPUP_NOTICE:
			popupNoticeButton = self.gameButtonDict["POPUP_NOTICE"]

			if self.popup_notice_window:
				if self.popup_notice_window.GetPopupNoticeEventFlagValue() > 0:
					popupNoticeButton.Show()
				else:
					popupNoticeButton.Hide()
			else:
				popupNoticeButton.Hide()

	def __IsSkillStat(self):
		if player.GetStatus(player.SKILL_ACTIVE) > 0:
			return True

		return False

	def __OnClickExitObserver(self):
		net.SendCommandPacket("/observer_exit")

	def __HideAllGameButton(self):
		for btn in self.gameButtonDict.values():
			btn.Hide()

		if app.ENABLE_POPUP_NOTICE:
			if self.popup_notice_icon_anim_img:
				self.popup_notice_icon_anim_img.Hide()

	def SetObserverMode(self, isEnable):
		if isEnable:
			self.gameButtonDict["EXIT_OBSERVER"].Show()
		else:
			self.gameButtonDict["EXIT_OBSERVER"].Hide()

	if app.ENABLE_POPUP_NOTICE:
		POPUP_NOTICE_BUTTON_TOOLTIP_GAP_NUMBER = 50

		def BindPopupNoticeWindow(self, popup_notice_window):
			self.popup_notice_window = popup_notice_window

		def SetPopupNoticeIconBlink(self, is_blink):
			self.is_popup_notice_btn_blink = is_blink

		def BlinkPopupNoticeIcon(self, is_blink):
			if self.popup_notice_icon_anim_img:
				return

			if self.is_blink:
				self.popup_notice_icon_anim_img.Show()
			else:
				self.popup_notice_icon_anim_img.Hide()

		def IsPopupNoticeIconBlink(self):
			return self.is_popup_notice_btn_blink

		def __OnMouseOverInPopupNoticeAnimImage(self):
			pass

		def __OnMouseOverOutPopupNoticeBtn(self):
			pass

		def __ShowPopupNoticeButtonAnimImage(self):
			if self.popup_notice_icon_anim_img:
				self.popup_notice_icon_anim_img.Show()

		def __ShowPopupNoticeButtonToolTip(self):
			arglen = len(uiScriptLocale.POPUP_NOTICE_BUTTON_TEXT)
			pos_x, pos_y = wndMgr.GetMousePosition()

			self.game_button_tooltip.ClearToolTip()
			self.game_button_tooltip.SetThinBoardSize(11 * arglen)
			self.game_button_tooltip.SetToolTipPosition(pos_x + self.POPUP_NOTICE_BUTTON_TOOLTIP_GAP_NUMBER, pos_y + self.POPUP_NOTICE_BUTTON_TOOLTIP_GAP_NUMBER)
			self.game_button_tooltip.AppendTextLine(uiScriptLocale.POPUP_NOTICE_BUTTON_TEXT)
			self.game_button_tooltip.Show()

		def __HidePopupNoticeButtonToolTip(self):
			self.game_button_tooltip.Hide()

		def OnUpdate(self):
			self.UpdateButtonToolTip()

		def UpdateButtonToolTip(self):
			if self.gameButtonDict["POPUP_NOTICE"].IsIn():
				self.__ShowPopupNoticeButtonToolTip()
			else:
				self.__HidePopupNoticeButtonToolTip()
