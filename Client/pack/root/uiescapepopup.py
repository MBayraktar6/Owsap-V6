#
# Filename: uiEscapePopup.py
# Copyright (C) 2024 Owsap Development
#
# Author: Owsap
# Website: https://owsap.dev/
#

import app
import ui
import uiCommon
import localeInfo
import player
import chat
import net

class EscapeManager(ui.Window):
	ESCAPE_WAIT_TIME_SEC = 10
	ESCAPE_DIALOG_TEXT_LINE_HEIGHT = 20
	ESCAPE_DIALOG_ADJUSTED_WIDTH = 60
	ESCAPE_BUTTON_COOLTIME = 1

	def __init__(self):
		ui.Window.__init__(self)
		self.__Initialize()

	def __del__(self):
		ui.Window.__del__(self)
		self.Destroy()

	def __Initialize(self):
		self.escape_button_click_time = 0
		self.escape_wait_time = 0
		self.escape_window_open_x = 0
		self.escape_window_open_y = 0
		self.escape_question_dialog = None
		self.escape_wait_dialog = None

	def __OpenEscapePopup(self):
		if self.escape_question_dialog:
			return

		escape_question_dialog = uiCommon.QuestionDialog()
		escape_question_dialog.SetText(localeInfo.OPTION_ESCAPE_QUESTION_MESSAGE)
		escape_question_dialog.SetAcceptEvent(lambda answer = True : self.ProcessEscapeQuestionAnswer(answer))
		escape_question_dialog.SetCancelEvent(lambda answer = False : self.ProcessEscapeQuestionAnswer(answer))

		width, height = escape_question_dialog.GetTextSize()
		escape_question_dialog.SetWidth(width + self.ESCAPE_DIALOG_ADJUSTED_WIDTH)
		escape_question_dialog.SetLineHeight(self.ESCAPE_DIALOG_TEXT_LINE_HEIGHT - height)
		escape_question_dialog.Open()
		self.escape_question_dialog = escape_question_dialog

		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def ProcessEscapeQuestionAnswer(self, answer):
		self.Close()

		if answer == True:
			self.__OpenEscapeWaitPopupDialog()
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_ESCAPE_QUESTION_ANSWER_NO)

	def __OpenEscapeWaitPopupDialog(self):
		self.escape_wait_time = app.GetTime() + self.ESCAPE_WAIT_TIME_SEC

		## Dialog Board
		escape_wait_dialog = ui.Board()
		escape_wait_dialog.SetParent(self)
		escape_wait_dialog.SetSize(280, 90)
		escape_wait_dialog.SetPosition(0, 0)
		escape_wait_dialog.SetWindowHorizontalAlignCenter()
		escape_wait_dialog.SetWindowVerticalAlignCenter()
		escape_wait_dialog.Lock()
		escape_wait_dialog.SetTop()
		escape_wait_dialog.Show()

		## Dialog Message
		escape_wait_dialog.message = ui.TextLine()
		escape_wait_dialog.message.SetParent(escape_wait_dialog)
		escape_wait_dialog.message.SetText(localeInfo.OPTION_ESCAPE_WAIT_MESSAGE % self.ESCAPE_WAIT_TIME_SEC)
		escape_wait_dialog.message.SetPosition(0, 0)
		escape_wait_dialog.message.SetWindowHorizontalAlignCenter()
		escape_wait_dialog.message.SetWindowVerticalAlignCenter()
		escape_wait_dialog.message.SetHorizontalAlignCenter()
		escape_wait_dialog.message.SetVerticalAlignCenter()
		escape_wait_dialog.message.Show()

		self.escape_wait_dialog = escape_wait_dialog

		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def __EscapePopupRangeCheck(self):
		(x, y, z) = player.GetMainCharacterPosition()
		if abs(x - self.escape_window_open_x) > player.SHOW_UI_WINDOW_LIMIT_RANGE or abs(y - self.escape_window_open_y) > player.SHOW_UI_WINDOW_LIMIT_RANGE:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_ESCAPE_FAR_DISTANCE)
			self.Close()

	def __EscapeWaitTimeUpdate(self):
		if not self.escape_wait_dialog:
			return

		time_sec = max(0, self.escape_wait_time - app.GetTime())

		if self.escape_wait_dialog.message:
			self.escape_wait_dialog.message.SetText(localeInfo.OPTION_ESCAPE_WAIT_MESSAGE % (time_sec))

		if time_sec < 0.5:
			net.SendCommandPacket("/escape") # PLAYER_CMD_ESCAPE
			self.Close()

	def OnUpdate(self):
		self.__EscapePopupRangeCheck()
		self.__EscapeWaitTimeUpdate()

	def IsEscapeQuestionPopupOpen(self):
		if self.escape_question_dialog and self.escape_question_dialog.IsShow():
			return True

		if self.escape_wait_dialog and self.escape_wait_dialog.IsShow():
			return True

		return False

	def OpenEscapePopup(self):
		self.escape_button_click_time = app.GetTime()
		self.escape_window_open_x, self.escape_window_open_y, z = player.GetMainCharacterPosition()

		if player.CanEscapeState():
			self.__OpenEscapePopup()
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_ESCAPE_CANNOT_ESCAPE_STATE)

	def Close(self):
		self.Destroy()
		self.Hide()

	def Destroy(self):
		if self.escape_question_dialog:
			self.escape_question_dialog.Close()
		self.escape_question_dialog = None

		if self.escape_wait_dialog:
			self.escape_wait_dialog.Unlock()
			self.escape_wait_dialog.Hide()
		self.escape_wait_dialog = None

	def OnPressEscapeKey(self):
		self.Close()
		return True
