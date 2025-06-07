import dbg
import app
import net

import ui
import uiCommon
import localeInfo

import player

###################################################################################################
## Restart
class RestartDialog(ui.ScriptWindow):

	CAN_IMMEDIATE_RESTART_ZONE = { 357 } # ENABLE_BATTLE_FIELD
	CAN_GIVEUP_RESTART_ZONE = { 358 } # ENABLE_DEFENSE_WAVE

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.popup = None

		self.ONLY_GIVEUP_POPUP_ZONE = {
			360 : localeInfo.RESTART_POPUP_GIVEUP_MIST_ISLAND,
		}
		if app.ENABLE_ELEMENTAL_DUNGEON:
			self.ONLY_GIVEUP_POPUP_ZONE[378] = localeInfo.RESTART_POPUP_GIVEUP_ELEMENTAL_DUNGEON
			self.ONLY_GIVEUP_POPUP_ZONE[379] = localeInfo.RESTART_POPUP_GIVEUP_ELEMENTAL_DUNGEON
			self.ONLY_GIVEUP_POPUP_ZONE[380] = localeInfo.RESTART_POPUP_GIVEUP_ELEMENTAL_DUNGEON
			self.ONLY_GIVEUP_POPUP_ZONE[381] = localeInfo.RESTART_POPUP_GIVEUP_ELEMENTAL_DUNGEON

		self.ONLY_RESTART_HERE_ZONE_BUTTON_TEXT = {
			404 : localeInfo.RESTART_HERE_BUTTON_TEXT_SUNGMAHEE_GATE,
			405 : localeInfo.RESTART_HERE_BUTTON_TEXT_SUNGMAHEE_GATE,
			406 : localeInfo.RESTART_HERE_BUTTON_TEXT_SUNGMAHEE_GATE,
			407 : localeInfo.RESTART_HERE_BUTTON_TEXT_SUNGMAHEE_GATE,
			408 : localeInfo.RESTART_HERE_BUTTON_TEXT_SUNGMAHEE_GATE,
			409 : localeInfo.RESTART_HERE_BUTTON_TEXT_SUNGMAHEE_GATE,
			410 : localeInfo.RESTART_HERE_BUTTON_TEXT_SUNGMAHEE_GATE,
			411 : localeInfo.RESTART_HERE_BUTTON_TEXT_SUNGMAHEE_GATE,
			412 : localeInfo.RESTART_HERE_BUTTON_TEXT_SUNGMAHEE_GATE,
		}

		self.ONLY_RESTART_HERE_ZONE_POPUP_TEXT = {
			404 : localeInfo.RESTART_POPUP_RESTART_HERE_SUNGMAHEE_GATE,
			405 : localeInfo.RESTART_POPUP_RESTART_HERE_SUNGMAHEE_GATE,
			406 : localeInfo.RESTART_POPUP_RESTART_HERE_SUNGMAHEE_GATE,
			407 : localeInfo.RESTART_POPUP_RESTART_HERE_SUNGMAHEE_GATE,
			408 : localeInfo.RESTART_POPUP_RESTART_HERE_SUNGMAHEE_GATE,
			409 : localeInfo.RESTART_POPUP_RESTART_HERE_SUNGMAHEE_GATE,
			410 : localeInfo.RESTART_POPUP_RESTART_HERE_SUNGMAHEE_GATE,
			411 : localeInfo.RESTART_POPUP_RESTART_HERE_SUNGMAHEE_GATE,
			412 : localeInfo.RESTART_POPUP_RESTART_HERE_SUNGMAHEE_GATE,
		}

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def LoadDialog(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/RestartDialog.py")
		except Exception, msg:
			import sys
			(type, msg, tb) = sys.exc_info()
			dbg.TraceError("RestartDialog.LoadDialog - %s:%s" % (type, msg))
			app.Abort()
			return 0

		try:
			self.board = self.GetChild("board")
			self.restartHereButton = self.GetChild("restart_here_button")
			self.restartTownButton = self.GetChild("restart_town_button")
			self.restartImmediatelyButton = self.GetChild("restart_immediately_button")
			self.restartGiveUpButton = self.GetChild("restart_giveup_button")
		except:
			import sys
			(type, msg, tb) = sys.exc_info()
			dbg.TraceError("RestartDialog.LoadDialog - %s:%s" % (type, msg))
			app.Abort()
			return 0

		self.restartHereButton.SetEvent(ui.__mem_func__(self.RestartHere))
		self.restartTownButton.SetEvent(ui.__mem_func__(self.RestartTown))
		self.restartImmediatelyButton.SetEvent(ui.__mem_func__(self.RestartImmediately))
		self.restartGiveUpButton.SetEvent(ui.__mem_func__(self.RestartGiveUp))

		return 1

	def Destroy(self):
		self.restartHereButton = None
		self.restartTownButton = None
		self.restartImmediatelyButton = None
		self.restartGiveUpButton = None
		self.popup = None

		self.ClearDictionary()

	def ShowAll(self):
		self.restartHereButton.Show()
		self.restartTownButton.Show()
		self.restartImmediatelyButton.Show()
		self.restartGiveUpButton.Show()

	def HideAll(self):
		self.restartHereButton.Hide()
		self.restartTownButton.Hide()
		self.restartImmediatelyButton.Hide()
		self.restartGiveUpButton.Hide()
	
	def AcceptGiveUp(self):
		self.RestartGiveUp()

		if self.popup:
			self.popup.Close()
			self.popup = None

	def CancelGiveUp(self):
		if self.popup:
			self.popup.Close()
			self.popup = None

	def __GiveUpPopupDialogOpen(self, text):
		if self.popup:
			return

		popup = uiCommon.QuestionDialog()
		popup.SetText(text)
		popup.SetAcceptEvent(ui.__mem_func__(self.AcceptGiveUp))
		popup.SetCancelEvent(ui.__mem_func__(self.CancelGiveUp))
		popup.Open()
		self.popup = popup

	def AcceptRestartHere(self):
		self.RestartHere()

		if self.popup:
			self.popup.Close()
			self.popup = None

	def CancelGiveUp(self):
		if self.popup:
			self.popup.Close()
			self.popup = None

	def __RestartHerePopupDialogOpen(self, text):
		if self.popup:
			return

		popup = uiCommon.QuestionDialog()
		popup.SetText(text)
		popup.SetAcceptEvent(ui.__mem_func__(self.AcceptRestartHere))
		popup.SetCancelEvent(ui.__mem_func__(self.CancelRestartHere))
		popup.Open()
		self.popup = popup

	def ShowDialogButton(self, map_index, dialog_type):
		if dialog_type == player.DEAD_DIALOG_NORMAL:

			if map_index in self.CAN_IMMEDIATE_RESTART_ZONE:
				self.restartImmediatelyButton.Show()
				self.restartGiveUpButton.Hide()
				self.board.SetSize(200, 118)
				self.Show()

			elif map_index in self.CAN_GIVEUP_RESTART_ZONE:
				self.restartImmediatelyButton.Hide()

				x, y = self.restartImmediatelyButton.GetLocalPosition()
				self.restartGiveUpButton.SetPosition(x, y)
				self.restartGiveUpButton.SetEvent(ui.__mem_func__(self.__GiveUpPopupDialogOpen), localeInfo.GIVEUP_QUESTION)
				self.restartGiveUpButton.Show()

				self.board.SetSize(200, 118)
				self.Show()

			else:
				self.restartImmediatelyButton.Hide()
				self.restartGiveUpButton.Hide()
				self.board.SetSize(200, 88)
				self.Show()

			if map_index in self.ONLY_RESTART_HERE_ZONE_POPUP_TEXT:

				if map_index in self.ONLY_RESTART_HERE_ZONE_BUTTON_TEXT:
					self.restartTownButton.SetText(self.ONLY_RESTART_HERE_ZONE_BUTTON_TEXT[map_index])
					self.restartTownButton.SetEvent(ui.__mem_func__(self.RestartImmediately))

				popup = uiCommon.PopupDialog()
				popup.SetText(self.ONLY_RESTART_HERE_ZONE_POPUP_TEXT[map_index])
				w, h = popup.GetTextSize()
				popup.SetWidth(w + 40)

				line_count = popup.GetTextLineCount()
				if line_count > 1:
					height = popup.GetLineHeight()
					popup.SetLineHeight(height + 3)

				popup.Open()
				self.popup = popup

		elif dialog_type == player.DEAD_DIALOG_GIVE_UP:

			if map_index in self.ONLY_GIVEUP_POPUP_ZONE:
				popup = uiCommon.PopupDialog()
				popup.SetText(self.ONLY_GIVEUP_POPUP_ZONE[map_index])
				popup.SetAcceptEvent(ui.__mem_func__(self.RestartGiveUp))
				popup.Open()
				self.popup = popup

			else:
				self.restartImmediatelyButton.Hide()
				self.restartGiveUpButton.Hide()
				self.board.SetSize(200, 88)
				self.Show()

	def OpenDialog(self, map_index, dialog_type):
		if map_index > 10000:
			map_index = map_index / 10000;

		self.ShowDialogButton(map_index, dialog_type)

	def Close(self):
		self.Hide()
		return True

	def RestartHere(self):
		net.SendCommandPacket("/restart_here")

	def RestartTown(self):
		net.SendCommandPacket("/restart_town")

	def RestartImmediately(self):
		net.SendCommandPacket("/restart_immediate")

	def RestartGiveUp(self):
		net.SendCommandPacket("/restart_giveup")

	def OnPressExitKey(self):
		return True

	def OnPressEscapeKey(self):
		return True
