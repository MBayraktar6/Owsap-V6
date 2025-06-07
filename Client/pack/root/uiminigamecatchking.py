import app
import ui
import uiScriptLocale
import wndMgr
import player
import localeInfo
import net
import constInfo
import event
import uiCommon
import grpImage
import grp
import chat
import item
import uiToolTip

from _weakref import proxy
from collections import deque

#CATCHKING_WINDOW_HEIGHT = 336
#CATCHKING_WINDOW_WIDTH = 392

STATE_NONE = 0
STATE_WAITING = 1
STATE_PLAY = 2

DESC_WIDTH_COUNT = 57
DEFAULT_DESC_Y = 7

#POS_TYPE_FIELD = 0
#POS_TYPE_HAND = 1
#POS_TYPE_MYNUMBER = 2
#POS_TYPE_SELECTION_NUMER = 3

HAND_CARD_MAX = 6
#FILED_CARD_X = 5
#FIELD_CARD_Y = 5
FIELD_CARD_MAX = 25
#FIELD_CARD_GEP_X = 1
#FIELD_CARD_GEP_Y = 1

card_img_path = {
	0 : "d:/ymir work/ui/minigame/catchking/card_number_k.sub",
	1 : "d:/ymir work/ui/minigame/catchking/card_number_1.sub",
	2 : "d:/ymir work/ui/minigame/catchking/card_number_2.sub",
	3 : "d:/ymir work/ui/minigame/catchking/card_number_3.sub",
	4 : "d:/ymir work/ui/minigame/catchking/card_number_4.sub",
	5 : "d:/ymir work/ui/minigame/catchking/card_number_5.sub",
	6 : "d:/ymir work/ui/minigame/catchking/card_number_6.sub",
	7 : "d:/ymir work/ui/minigame/catchking/card_pack.sub"
}

card_end_img_path = {
	1 : "d:/ymir work/ui/minigame/catchking/end_card_number_1.sub",
	2 : "d:/ymir work/ui/minigame/catchking/end_card_number_2.sub",
	3 : "d:/ymir work/ui/minigame/catchking/end_card_number_3.sub",
	4 : "d:/ymir work/ui/minigame/catchking/end_card_number_4.sub",
	5 : "d:/ymir work/ui/minigame/catchking/end_card_number_5.sub",
	6 : "d:/ymir work/ui/minigame/catchking/end_card_number_6.sub"
}

#card_default_count = {
#	1 : 5,
#	2 : 2,
#	3 : 2,
#	4 : 1,
#	5 : 1,
#	6 : 1
#}

#score_tooltip = (localeInfo.MINI_GAME_CATCHKING_TOOLTIP_SCORE1, localeInfo.MINI_GAME_CATCHKING_TOOLTIP_SCORE2, localeInfo.MINI_GAME_CATCHKING_TOOLTIP_SCORE3, localeInfo.MINI_GAME_CATCHKING_TOOLTIP_SCORE4)
#challenge_tooltip = (localeInfo.MINI_GAME_CATCHKING_CHALLENGE_TOOLTIP1, localeInfo.MINI_GAME_CATCHKING_CHALLENGE_TOOLTIP2)

#EVENT_TYPE_INIT_FIELD_CARD = 0
#EVENT_TYPE_INSER_DELAY = 1
#EVENT_TYPE_DELAY = 2
#EVENT_TYPE_SET_NUMBER = 3
#EVNET_TYPE_SET_EXPLOSION = 4
#EVENT_TYPE_SET_STATE = 5
#EVENT_TYPE_SET_SCORE = 6
#EVENT_TYPE_SET_ARROW = 7
#EVENT_TYPE_SUCCESS_EFFECT = 8
#EVNET_TYPE_BINGO_CHECK = 9
#EVENT_TYPE_POPUP = 10
#EVENT_TYPE_HAND_CARD_CLICK = 11
#EVENT_TYPE_SET_END_NUMBER = 12
#EVENT_TYPE_SHOW_NUMBER5_AREA = 13
#EVENT_TYPE_HIDE_NUMBER5_AREA = 14
#EVENT_TYPE_HELP_POPUP = 15

#CK_STATE_START = 0
#CK_STATE_WAIT = 1
#CK_STATE_FIELD_CLICK = 2
#CK_STATE_HAND_CLICK = 3
#CK_STATE_REWARD = 4
#CK_STATE_REWARD_END = 5

#TOTAL_SCORE_LOW_FONT_COLOR = -3750202
#TOTAL_SCORE_MID_FONT_COLOR = 4293830912L
#TOTAL_SCORE_HIGH_FONT_COLOR = 4294967193L

LOW_SCORE_VNUM = 50930
MID_SCORE_VNUM = 50929
HIGH_SCORE_VNUM = 50928

LOW_TOTAL_SCORE = 400
MID_TOTAL_SCORE = 550

#HELP_MSG_TYPE_LOW_NUMBER = 1
#HELP_MSG_TYPE_SAME_NUMBER = 2
#HELP_MSG_TYPE_HIGH_NUMBER = 3
#HELP_MSG_TYPE_SEARCH_NUMBER5 = 4

CATCHKING_START_YANG = 30000
CATCHKING_CHALLENGE_MAX = 5

if app.ENABLE_CATCH_KING_EVENT_FLAG_RENEWAL:
	CATCHKING_CARD_PIECE_COUNT_MAX = 25
	CATCHKING_CARD_COUNT_MAX = 999

def LoadScript(self, fileName):
	try:
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, fileName)
	except:
		import exception
		exception.Abort("MiniGameCatchKing.LoadScript")

class CatchKingWaitingPage(ui.ScriptWindow):

	class DescriptionBox(ui.Window):
		def __init__(self):
			ui.Window.__init__(self)
			self.desc_index = -1

		def __del__(self):
			ui.Window.__del__(self)

		def SetIndex(self, index):
			self.desc_index = index

		def OnRender(self):
			event.RenderEventSet(self.desc_index)

	VISIBLE_LINE_COUNT = 8

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.is_loaded = False

		self.start_button = None
		self.desc_board = None
		self.description_box = None

		self.desc_index = -1
		self.desc_y = DEFAULT_DESC_Y

		self.challenge_item_slot = None
		self.challenge_item_count_text = None
		self.challenge_text_window = None
		self.challenge_up_arrow_button = None
		self.challenge_down_arrow_button = None

		self.cur_challenge_count = 1
		self.tooltip_challenge = None

		self.start_question_dialog = None

		self.btn_prev = None
		self.btn_next = None

		if app.ENABLE_CATCH_KING_EVENT_FLAG_RENEWAL:
			self.is_data_requested = False

			self.card_piece_count = 0
			self.card_piece_slot = None
			self.card_piece_text = None

			self.card_pack_count = 0
			self.card_pack_slot = None
			self.card_pack_text = None

			self.tooltip_item = None

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadWindow(self):
		if self.is_loaded == 1:
			return

		self.is_loaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/MiniGameCatchKingWaitingPage.py")
		except:
			import exception
			exception.Abort("CatchKingWaitingPage.LoadWindow.LoadObject")

		try:
			self.GetChild("board").SetCloseEvent(ui.__mem_func__(self.Close))

			self.start_button = self.GetChild("game_start_button")
			self.start_button.SetEvent(ui.__mem_func__(self.__ClickStartButton))

			self.desc_board = self.GetChild("desc_board")
			self.description_box = self.DescriptionBox()
			self.description_box.SetParent(self.desc_board)
			self.description_box.Show()

			self.btn_prev = self.GetChild("prev_button")
			self.btn_prev.SetEvent(ui.__mem_func__(self.PrevDescriptionPage))

			self.btn_next = self.GetChild("next_button")
			self.btn_next.SetEvent(ui.__mem_func__(self.NextDescriptionPage))

			self.challenge_text_window = self.GetChild("challenge_text_window")
			self.challenge_text_window.SetOverEvent(ui.__mem_func__(self.__OverInChallengeText))
			self.challenge_text_window.SetOverOutEvent(ui.__mem_func__(self.__OverOutChallengeText))

			self.challenge_item_slot = self.GetChild("ChallengeItemSlot")
			self.challenge_item_slot.SetOverInItemEvent(ui.__mem_func__(self.__SlotOverInItem), item.ITEM_VNUM_CATCH_KING_PACK)
			self.challenge_item_slot.SetOverOutItemEvent(ui.__mem_func__(self.__SlotOverOutItem))

			self.challenge_item_count_text = self.GetChild("challenge_count_text")

			self.challenge_up_arrow_button = self.GetChild("up_arrow_button")
			self.challenge_up_arrow_button.SetEvent(ui.__mem_func__(self.__ClickUpArrowButton))
			self.challenge_down_arrow_button = self.GetChild("down_arrow_button")
			self.challenge_down_arrow_button.SetEvent(ui.__mem_func__(self.__ClickDownArrowButton))

			self.tooltip_challenge = uiToolTip.ToolTip()
			self.tooltip_challenge.HideToolTip()

			if localeInfo.IsARABIC():
				self.btn_prev.LeftRightReverse()
				self.btn_next.LeftRightReverse()

			if app.ENABLE_CATCH_KING_EVENT_FLAG_RENEWAL:
				self.card_piece_slot = self.GetChild("challenge_piece_item_count_slot")
				self.card_piece_slot.SetOverInItemEvent(ui.__mem_func__(self.__SlotOverInItem), item.ITEM_VNUM_CATCH_KING_PIECE)
				self.card_piece_slot.SetOverOutItemEvent(ui.__mem_func__(self.__SlotOverOutItem))

				self.card_piece_text = self.GetChild("challenge_piece_item_count_text")
				self.card_piece_text.SetText("%d/%d" % (0, CATCHKING_CARD_PIECE_COUNT_MAX))

				self.card_pack_slot = self.GetChild("challenge_pack_item_count_slot")
				self.card_pack_slot.SetOverInItemEvent(ui.__mem_func__(self.__SlotOverInItem), item.ITEM_VNUM_CATCH_KING_PACK)
				self.card_pack_slot.SetOverOutItemEvent(ui.__mem_func__(self.__SlotOverOutItem))
				self.card_pack_text = self.GetChild("challenge_pack_item_count_text")
				self.card_pack_text.SetText("%d/%d" % (0, CATCHKING_CARD_COUNT_MAX))

		except:
			import exception
			exception.Abort("CatchKingWaitingPage.LoadWindow.BindObject")

		self.Hide()

	def Open(self):
		self.Show()

	def Show(self):
		if app.ENABLE_CATCH_KING_EVENT_FLAG_RENEWAL:
			if not self.is_data_requested:
				net.SendMiniGameCatchKingRequestQuestFlag()
				self.is_data_requested = True

		ui.ScriptWindow.Show(self)

		event.ClearEventSet(self.desc_index)
		self.desc_index = event.RegisterEventSet(uiScriptLocale.MINIGAME_CATCH_KING_DESC)

		event.SetFontColor(self.desc_index, 0.7843, 0.7843, 0.7843)
		event.SetVisibleLineCount(self.desc_index, self.VISIBLE_LINE_COUNT)

		if localeInfo.IsARABIC():
			event.SetEventSetWidth(self.desc_index, self.desc_board.GetWidth() - 20)

		event.SetRestrictedCount(self.desc_index, DESC_WIDTH_COUNT)

		if self.description_box:
			self.description_box.Show()

		try:
			item.SelectItem(item.ITEM_VNUM_CATCH_KING_PACK)
			self.challenge_item_slot.SetSlot(0, 0, 32, 32, item.GetIconImage(), (1.0, 1.0, 1.0, 1.0))

			if app.ENABLE_CATCH_KING_EVENT_FLAG_RENEWAL:
				item.SelectItem(item.ITEM_VNUM_CATCH_KING_PIECE)
				self.card_piece_slot.SetSlot(0, 0, 32, 32, item.GetIconImage(), (1.0, 1.0, 1.0, 1.0))

				item.SelectItem(item.ITEM_VNUM_CATCH_KING_PACK)
				self.card_pack_slot.SetSlot(0, 0, 32, 32, item.GetIconImage(), (1.0, 1.0, 1.0, 1.0))
		except:
			import exception
			exception.Abort("CatchKingWaitingPage.Show.SelectItem")

	def Close(self):
		self.Hide()

		self.CloseStartDlg()

		event.ClearEventSet(self.desc_index)
		self.desc_index = -1
		self.desc_y = DEFAULT_DESC_Y

		if self.description_box:
			self.description_box.Hide()

		if self.tooltip_challenge:
			self.tooltip_challenge.Hide()

	def Destroy(self):
		self.is_loaded = 0

		self.start_button = None
		self.desc_board = None
		self.description_box = None

		self.desc_index = -1
		self.desc_y = DEFAULT_DESC_Y

		self.challenge_item_slot = None
		self.challenge_item_count_text = None
		self.challenge_text_window = None
		self.challenge_up_arrow_button = None
		self.challenge_down_arrow_button = None

		self.cur_challenge_count = 1
		self.tooltip_challenge = None

		self.start_question_dialog = None

		self.btn_prev = None
		self.btn_next = None

		if app.ENABLE_CATCH_KING_EVENT_FLAG_RENEWAL:
			self.card_piece_count = 0
			self.card_piece_slot = None
			self.card_piece_text = None

			self.card_pack_count = 0
			self.card_pack_slot = None
			self.card_pack_text = None

			self.tooltip_item = None

		self.CloseStartDlg()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def OnUpdate(self):
		(xposEventSet, yposEventSet) = self.desc_board.GetGlobalPosition()
		event.UpdateEventSet(self.desc_index, xposEventSet + 7, -(yposEventSet + self.desc_y))
		self.description_box.SetIndex(self.desc_index)

	def PrevDescriptionPage(self):
		line_height = event.GetLineHeight(self.desc_index) + 4
		cur_start_line = event.GetVisibleStartLine(self.desc_index)

		decrease_count = self.VISIBLE_LINE_COUNT
		if cur_start_line - decrease_count < 0:
			return

		event.SetVisibleStartLine(self.desc_index, cur_start_line - decrease_count)
		self.desc_y += (line_height * decrease_count)

	def NextDescriptionPage(self):
		line_height = event.GetLineHeight(self.desc_index) + 4
		total_line_count = event.GetProcessedLineCount(self.desc_index)
		cur_start_line = event.GetVisibleStartLine(self.desc_index)

		increase_count = self.VISIBLE_LINE_COUNT
		if cur_start_line + increase_count >= total_line_count:
			increase_count = total_line_count - cur_start_line

		if increase_count < 0 or cur_start_line + increase_count >= total_line_count:
			return

		event.SetVisibleStartLine(self.desc_index, cur_start_line + increase_count)
		self.desc_y -= (line_height * increase_count)

	def __ClickUpArrowButton(self):
		if self.cur_challenge_count == CATCHKING_CHALLENGE_MAX:
			return

		challenge_count = self.cur_challenge_count + 1
		if challenge_count > CATCHKING_CHALLENGE_MAX:
			return

		self.cur_challenge_count = challenge_count
		self.challenge_item_count_text.SetText(str(self.cur_challenge_count))

	def __ClickDownArrowButton(self):
		if self.cur_challenge_count == 1:
			return

		self.cur_challenge_count = self.cur_challenge_count - 1
		self.challenge_item_count_text.SetText(str(self.cur_challenge_count))

	def __ClickStartButton(self):
		if app.ENABLE_CATCH_KING_EVENT_FLAG_RENEWAL:
			if self.card_pack_count == 0 or player.GetElk() < CATCHKING_START_YANG:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CATCHKING_EVENT_MESSAGE_NOT_ENOUGHT_ITEM)
				return

		if None == self.start_question_dialog:
			self.start_question_dialog = uiCommon.QuestionDialog()
			self.start_question_dialog.SetAcceptEvent(ui.__mem_func__(self.__StartAccept))
			self.start_question_dialog.SetCancelEvent(ui.__mem_func__(self.__StartCancel))

		item.SelectItem(item.ITEM_VNUM_CATCH_KING_PACK)
		self.start_question_dialog.SetText(localeInfo.MINI_GAME_CATCHKING_START_QUESTION % (CATCHKING_START_YANG * self.cur_challenge_count, item.GetItemName(), self.cur_challenge_count))

		w, h = self.start_question_dialog.GetTextSize()
		self.start_question_dialog.SetWidth(w + 60)

		#line_count = self.start_question_dialog.GetTextLineCount()
		#if line_count > 1:
		#	height = self.start_question_dialog.GetLineHeight()
		#	self.start_question_dialog.SetLineHeight(line_count + 3)

		self.start_question_dialog.Open()

	def __IsStartGame(self): pass

	def __OverInChallengeText(self):
		if not self.tooltip_challenge:
			return

		item.SelectItem(item.ITEM_VNUM_CATCH_KING_PACK)

		self.tooltip_challenge.ClearToolTip()
		self.tooltip_challenge.AutoAppendTextLine(localeInfo.MINI_GAME_CATCHKING_CHALLENGE_TOOLTIP1 % item.GetItemName())
		self.tooltip_challenge.AutoAppendTextLine(localeInfo.MINI_GAME_CATCHKING_CHALLENGE_TOOLTIP2 % CATCHKING_CHALLENGE_MAX)
		self.tooltip_challenge.AlignHorizonalCenter()
		self.tooltip_challenge.Show()

	def __OverOutChallengeText(self):
		if self.tooltip_challenge:
			self.tooltip_challenge.Hide()

	def __StartAccept(self):
		net.SendMiniGameCatchKing(0, self.cur_challenge_count)

	def __StartCancel(self):
		self.start_question_dialog.Close()

	def CloseStartDlg(self):
		if self.start_question_dialog:
			self.start_question_dialog.Close()
			self.start_question_dialog = None

	if app.ENABLE_CATCH_KING_EVENT_FLAG_RENEWAL:
		def __SlotOverInItem(self, slot_index, vnum):
			if self.tooltip_item:
				self.tooltip_item.SetItemToolTip(vnum)

		def __SlotOverOutItem(self):
			if self.tooltip_item:
				self.tooltip_item.HideToolTip()

		def SetItemToolTip(self, tooltip):
			self.tooltip_item = tooltip

		def DecreaseMiniGameCatchKingCardCount(self):
			self.card_pack_count = max(0, self.card_pack_count - 1)
			self.card_pack_text.SetText("%d/%d" % (self.card_pack_count, CATCHKING_CARD_COUNT_MAX))

		def CatchKingFlagProcess(self, type, data):
			(card_piece_count, card_pack_count) = data

			self.card_piece_count = card_piece_count
			self.card_pack_count = card_pack_count

			self.card_piece_text.SetText("%d/%d" % (self.card_piece_count, CATCHKING_CARD_PIECE_COUNT_MAX))
			self.card_pack_text.SetText("%d/%d" % (self.card_pack_count, CATCHKING_CARD_COUNT_MAX))

class CatchKingCard:
	def __init__(self, miniGame, cardType, parent, x, y, clickFunc, clickArg):
		self.pos = (x, y)
		self.posGlobal = parent.GetLocalPosition()

		self.backgroundImage = None
		self.backgroundOverImage = None
		self.backgroundRowImage = None
		self.fiveNearEffect = None
		self.arrowImage = None
		self.cardCountImage = None
		self.cardCountText = None
		self.destroyCardEffect = None
		self.miniGame = miniGame
		self.cardNumber = 0
		self.cardCount = 0
		self.isTheEnd = False

		self.__CreateBackgroundImg(parent, cardType, clickFunc, clickArg)

		if cardType == 0:
			self.__CreateBackgroundOverImg(parent)
			self.__CreateCardDestroyEffect(parent)
			self.__CreateFiveNearEffect(parent)
			self.__CreateBackgroundRowImg(parent)

		self.__CreateArrowImg(parent, cardType)

		if cardType == 1:
			self.__CreateCardCount(parent)

	def __del__(self):
		self.pos = (0, 0)
		self.posGlobal = (0, 0)

		self.backgroundImage = None
		self.backgroundOverImage = None
		self.backgroundRowImage = None
		self.arrowImage = None
		self.fiveNearEffect = None
		self.cardCountImage = None
		self.cardCountText = None
		self.destroyCardEffect = None
		self.miniGame = None
		self.cardNumber = 0
		self.cardCount = 0
		self.isTheEnd = False

	def SetCardNumber(self, cardNumber, isEmpty = True, cardType = 0, overInFunc = None, overOutFunc = None):
		self.cardNumber = cardNumber

		if self.backgroundImage:
			if cardType == 1 and overInFunc and overOutFunc:
				self.backgroundImage.SetEvent(ui.__mem_func__(overInFunc), "mouse_over_in", self.cardNumber)
				self.backgroundImage.SetEvent(ui.__mem_func__(overOutFunc), "mouse_over_out")

			if isEmpty:
				self.isTheEnd = False

				if self.backgroundRowImage:
					self.backgroundRowImage.Hide()

				self.backgroundImage.LoadImage(card_img_path[0])

				if self.destroyCardEffect:
					self.destroyCardEffect.Hide()
			else:
				self.backgroundImage.LoadImage(card_img_path[cardNumber])

	def SetEndCardNumber(self, cardNumber):
		self.isTheEnd = True
		if self.backgroundImage:
			self.backgroundImage.LoadImage(card_end_img_path[cardNumber])

	def ShowRowBackground(self):
		if self.backgroundRowImage:
			self.backgroundRowImage.Show()

	def GetCardNumber(self):
		return self.cardNumber

	def __CreateCardDestroyEffect(self, parent):
		self.destroyCardEffect = ui.AniImageBox()
		self.destroyCardEffect.SetParent(proxy(parent))
		self.destroyCardEffect.SetEndFrameEvent(ui.__mem_func__(self.SetCardNumber), 0, True)
		self.destroyCardEffect.SetDelay(6)
		self.destroyCardEffect.AppendImage("D:/Ymir Work/UI/minigame/catchking/effect/explosion/1.sub")
		self.destroyCardEffect.AppendImage("D:/Ymir Work/UI/minigame/catchking/effect/explosion/2.sub")
		self.destroyCardEffect.AppendImage("D:/Ymir Work/UI/minigame/catchking/effect/explosion/3.sub")
		self.destroyCardEffect.AppendImage("D:/Ymir Work/UI/minigame/catchking/effect/explosion/4.sub")
		self.destroyCardEffect.AppendImage("D:/Ymir Work/UI/minigame/catchking/effect/explosion/5.sub")
		self.destroyCardEffect.AppendImage("D:/Ymir Work/UI/minigame/catchking/effect/explosion/6.sub")
		self.destroyCardEffect.AppendImage("D:/Ymir Work/UI/minigame/catchking/effect/explosion/7.sub")
		self.destroyCardEffect.AppendImage("D:/Ymir Work/UI/minigame/catchking/effect/explosion/8.sub")
		self.destroyCardEffect.Hide()
		self.destroyCardEffect.AddFlag("not_pick")
		self.destroyCardEffect.AddFlag("float")

	def __CreateFiveNearEffect(self, parent):
		(x, y) = self.pos
		self.fiveNearEffect = ui.AniImageBox()
		self.fiveNearEffect.SetParent(proxy(parent))
		self.fiveNearEffect.SetEndFrameEvent(ui.__mem_func__(self.HideFiveNearEffect))
		self.fiveNearEffect.SetDelay(6)
		self.fiveNearEffect.AppendImage("D:/Ymir Work/UI/minigame/catchking/effect/over5/1.sub")
		self.fiveNearEffect.AppendImage("D:/Ymir Work/UI/minigame/catchking/effect/over5/1.sub")
		self.fiveNearEffect.AppendImage("D:/Ymir Work/UI/minigame/catchking/effect/over5/2.sub")
		self.fiveNearEffect.AppendImage("D:/Ymir Work/UI/minigame/catchking/effect/over5/3.sub")
		self.fiveNearEffect.AppendImage("D:/Ymir Work/UI/minigame/catchking/effect/over5/4.sub")
		self.fiveNearEffect.AppendImage("D:/Ymir Work/UI/minigame/catchking/effect/over5/5.sub")
		self.fiveNearEffect.AppendImage("D:/Ymir Work/UI/minigame/catchking/effect/over5/6.sub")
		self.fiveNearEffect.SetPosition(x, y)
		self.fiveNearEffect.Hide()
		self.fiveNearEffect.AddFlag("not_pick")
		self.fiveNearEffect.AddFlag("float")

	def HideFiveNearEffect(self):
		if self.fiveNearEffect:
			self.fiveNearEffect.Hide()

	def ShowFiveNearEffect(self, endFunc, *args):
		if self.fiveNearEffect:
			if endFunc:
				self.fiveNearEffect.SetEndFrameEvent(ui.__mem_func__(endFunc), *args)
			self.fiveNearEffect.SetTop()
			self.fiveNearEffect.ResetFrame()
			self.fiveNearEffect.Show()

	def ShowCardDestroyEffect(self):
		if self.destroyCardEffect:
			(x, y) = self.pos
			self.destroyCardEffect.SetPosition(x - 39, y - 50)
			self.destroyCardEffect.SetTop()
			self.destroyCardEffect.ResetFrame()
			self.destroyCardEffect.Show()

	def __CreateBackgroundImg(self, parent, cardType, clickFunc, clickArg):
		(x, y) = self.pos
		self.backgroundImage = ui.ImageBox()
		self.backgroundImage.SetParent(proxy(parent))
		self.backgroundImage.LoadImage(card_img_path[0])

		if cardType == 0:
			self.backgroundImage.SetEvent(ui.__mem_func__(self.__OverInFunc), "mouse_over_in")
			self.backgroundImage.SetEvent(ui.__mem_func__(self.__OverOutFunc), "mouse_over_out")

		if clickFunc:
			self.backgroundImage.SetEvent(ui.__mem_func__(clickFunc), "mouse_click", clickArg)

		self.backgroundImage.SetPosition(x, y)
		self.backgroundImage.Show()
		self.backgroundImage.AddFlag("float")

	def __CreateBackgroundOverImg(self, parent):
		(x, y) = self.pos
		self.backgroundOverImage = ui.ImageBox()
		self.backgroundOverImage.SetParent(proxy(parent))
		self.backgroundOverImage.LoadImage("d:/ymir work/ui/minigame/catchking/card_over_img.sub")
		self.backgroundOverImage.SetPosition(x, y)
		self.backgroundOverImage.Hide()
		self.backgroundOverImage.AddFlag("not_pick")
		self.backgroundOverImage.AddFlag("float")

	def __CreateBackgroundRowImg(self, parent):
		(x, y) = self.pos
		self.backgroundRowImage = ui.ImageBox()
		self.backgroundRowImage.SetParent(proxy(parent))
		self.backgroundRowImage.LoadImage("d:/ymir work/ui/minigame/catchking/card_cover_bingo.sub")
		self.backgroundRowImage.SetPosition(x, y)
		self.backgroundRowImage.Hide()
		self.backgroundRowImage.AddFlag("not_pick")
		self.backgroundRowImage.AddFlag("float")

	def __CreateArrowImg(self, parent, cardType):
		(x, y) = self.pos
		self.arrowImage = ui.AniImageBox()
		self.arrowImage.SetParent(proxy(parent))
		self.arrowImage.SetDelay(10)
		self.arrowImage.AppendImage("D:/Ymir Work/UI/minigame/yutnori/move_arrow/1.sub")
		self.arrowImage.AppendImage("D:/Ymir Work/UI/minigame/yutnori/move_arrow/2.sub")
		self.arrowImage.AppendImage("D:/Ymir Work/UI/minigame/yutnori/move_arrow/3.sub")
		self.arrowImage.AppendImage("D:/Ymir Work/UI/minigame/yutnori/move_arrow/4.sub")
		self.arrowImage.AppendImage("D:/Ymir Work/UI/minigame/yutnori/move_arrow/5.sub")
		self.arrowImage.AppendImage("D:/Ymir Work/UI/minigame/yutnori/move_arrow/4.sub")
		self.arrowImage.AppendImage("D:/Ymir Work/UI/minigame/yutnori/move_arrow/3.sub")
		self.arrowImage.AppendImage("D:/Ymir Work/UI/minigame/yutnori/move_arrow/2.sub")

		if cardType == 0:
			self.arrowImage.SetPosition(x + 17, y - 10)
		elif cardType == 1:
			self.arrowImage.SetPosition(x + 17, y - 34)

		self.arrowImage.Hide()
		self.arrowImage.AddFlag("not_pick")
		self.arrowImage.AddFlag("float")

	def __CreateCardCount(self, parent):
		(x, y) = self.pos
		self.cardCountImage = ui.ImageBox()
		self.cardCountImage.SetParent(proxy(parent))
		self.cardCountImage.LoadImage("d:/ymir work/ui/minigame/catchking/card_count_bg.sub")
		self.cardCountImage.SetPosition(x + 29, y - 6)
		self.cardCountImage.Hide()
		self.cardCountImage.AddFlag("not_pick")
		self.cardCountImage.AddFlag("float")

		self.cardCountText = ui.TextLine()
		self.cardCountText.SetParent(self.cardCountImage)
		self.cardCountText.SetWindowHorizontalAlignCenter()
		self.cardCountText.SetWindowVerticalAlignCenter()
		self.cardCountText.SetVerticalAlignCenter()
		self.cardCountText.SetHorizontalAlignCenter()
		self.cardCountText.Show()

	def SetCardCount(self, cardCount):
		if self.cardCountImage:
			if not self.cardCountImage.IsShow():
				self.cardCountImage.Show()

		if not cardCount:
			if self.cardCountImage:
				self.cardCountImage.Hide()

			if self.backgroundImage:
				self.backgroundImage.Hide()
		else:
			if self.cardCountImage:
				self.cardCountImage.Show()

			if self.backgroundImage:
				self.backgroundImage.Show()

		self.cardCount = cardCount

		if self.cardCountText:
			self.cardCountText.SetText("X%d" % cardCount)

	def GetCardCount(self):
		return self.cardCount

	def GetPos(self):
		return self.pos

	def GetGlobalPos(self):
		return self.posGlobal

	def GetLocalPosition(self):
		if self.backgroundImage:
			return self.backgroundImage.GetLocalPosition()

		return (0, 0)

	def SetPosition(self, x, y):
		if self.backgroundImage:
			self.backgroundImage.SetPosition(x, y)

		if self.arrowImage:
			self.arrowImage.SetPosition(x + 7, y - 34)

	def SetTop(self):
		if self.backgroundImage:
			self.backgroundImage.SetTop()

		if self.backgroundOverImage:
			self.backgroundOverImage.SetTop()

		if self.arrowImage:
			self.arrowImage.SetTop()

	def __OverInFunc(self):
		if self.cardNumber:
			return

		if self.isTheEnd:
			return

		if self.backgroundOverImage:
			self.backgroundOverImage.Show()

		if self.miniGame:
			if self.miniGame.HaveCardInHand():
				self.ArrowImgShow()

	def __OverOutFunc(self):
		if self.backgroundOverImage:
			self.backgroundOverImage.Hide()

		self.ArrowImgHide()

	def IsShowArrow(self):
		if self.arrowImage:
			return self.arrowImage.IsShow()

		return False

	def ArrowImgShow(self):
		if self.arrowImage:
			self.arrowImage.SetTop()
			self.arrowImage.ResetFrame()
			self.arrowImage.Show()

	def ArrowImgHide(self):
		if self.arrowImage:
			self.arrowImage.Hide()

class CatchKingGamePage(ui.ScriptWindow):
	def __init__(self, wndMiniGameCatchKing):
		import exception

		if not wndMiniGameCatchKing:
			exception.Abort("wndMiniGameCatchKing parameter must be set to MiniGameCatchKing")
			return

		self.card_tooltip = {
			1 : localeInfo.MINI_GAME_CATCHKING_TOOLTIP_CARD1,
			2 : localeInfo.MINI_GAME_CATCHKING_TOOLTIP_CARD2,
			3 : localeInfo.MINI_GAME_CATCHKING_TOOLTIP_CARD3,
			4 : localeInfo.MINI_GAME_CATCHKING_TOOLTIP_CARD4,
			5 : localeInfo.MINI_GAME_CATCHKING_TOOLTIP_CARD5,
			6 : localeInfo.MINI_GAME_CATCHKING_TOOLTIP_CARD6
		}

		ui.ScriptWindow.__init__(self)

		self.isLoaded = 0
		self.SetWindowName("CatchKingEventGamePageWindow")
		self.wndMiniGameCatchKing = wndMiniGameCatchKing

		self.fieldBackground = None

		self.myHandCardImage = None
		self.selectedCardImage = None

		self.gameCardList = []
		self.handCardList = []
		self.scoreInfo = []

		self.handCardNumber = 0

		self.infoTooltip = None

		self.confirmWindowCheckButton = None
		self.checkImage = None

		self.bigScoreText = None
		self.scoreWnd = None
		self.scoreText = None

		self.rewardButton = None

		self.myHandCardBg = None
		self.selectedCardBg = None

		self.destroyCardEffect = None
		self.popupResult = None

		self.scoreTextEffect = None
		self.scoreEffect1 = None
		self.scoreEffect2 = None
		self.scoreEffect3 = None

		self.isLocked = False

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			LoadScript(self, "UIScript/MiniGameCatchKingGamePage.py")
		except:
			import exception
			exception.Abort("CatchKingGamePage.LoadScript")

		try:
			self.GetChild("board").SetCloseEvent(ui.__mem_func__(self.Close))
			self.fieldBackground = self.GetChild("field_bg")

			self.confirmWindowCheckButton = self.GetChild("confirm_check_button")
			self.confirmWindowCheckButton.SetEvent(ui.__mem_func__(self.__ClickConfirmCheckButton), "mouse_click")

			self.checkImage = self.GetChild("check_image")
			self.checkImage.Show()

			self.bigScoreText = self.GetChild("high_score_text")
			self.scoreText = self.GetChild("score_text")
			self.scoreWnd = self.GetChild("score_window")
			self.myHandCardBg = self.GetChild("my_number_card_bg")
			self.selectedCardBg = self.GetChild("selection_number_card_bg")

			self.rewardButton = self.GetChild("reward_button")
			self.rewardButton.SetEvent(ui.__mem_func__(self.__ClickRewardButton))

			self.scoreTextEffect = self.GetChild("success_text_effect")
			self.scoreTextEffect.SetEndFrameEvent(ui.__mem_func__(self.__ScoreTextEffectEndFrameEvent))
			self.scoreTextEffect.Hide()

			self.scoreEffect1 = self.GetChild("success_effect1")
			self.scoreEffect2 = self.GetChild("success_effect2")
			self.scoreEffect3 = self.GetChild("success_effect3")
			self.scoreEffect1.Hide()
			self.scoreEffect2.Hide()
			self.scoreEffect3.Hide()
			self.scoreEffect1.SetEndFrameEvent(ui.__mem_func__(self.__ScoreEffectEndFrameEvent1))
			self.scoreEffect2.SetEndFrameEvent(ui.__mem_func__(self.__ScoreEffectEndFrameEvent2))
			self.scoreEffect3.SetEndFrameEvent(ui.__mem_func__(self.__ScoreEffectEndFrameEvent3))

			self.scoreEffect1.SetKeyFrameEvent(ui.__mem_func__(self.__ScoreEffectKeyFrameEvent1))
			self.scoreEffect2.SetKeyFrameEvent(ui.__mem_func__(self.__ScoreEffectKeyFrameEvent2))

			self.__ClearScoreCompletionEffect()

			self.infoTooltip = uiToolTip.ToolTip()
			self.infoTooltip.ClearToolTip()

			self.scoreWnd.OnMouseOverIn = lambda : ui.__mem_func__(self.OverInScoreToolTip)()
			self.scoreWnd.OnMouseOverOut = lambda : ui.__mem_func__(self.OverOutToolTip)()

			for i in range(FIELD_CARD_MAX):
				self.gameCardList.append(CatchKingCard(self, 0, self.fieldBackground, 8 + ((i % 5) * 51), 8 + ((i / 5) * 37), self.ClickCard, i))
				self.gameCardList[i].SetCardNumber(0, True)

			for k in range(HAND_CARD_MAX):
				self.handCardList.append(CatchKingCard(self, 1, self.GetChild("hand_card_bg%d" % int(k + 1)), 4, 4, self.ClickHandCard, k))
				self.handCardList[k].SetCardNumber(k + 1, False, 1, self.OverInToolTip, self.OverOutToolTip)

			self.myHandCardImage = ui.ImageBox()
			self.myHandCardImage.SetParent(self.myHandCardBg)
			self.myHandCardImage.LoadImage(card_img_path[0])
			self.myHandCardImage.SetPosition(8, 8)
			self.myHandCardImage.AddFlag("float")
			self.myHandCardImage.Hide()

			self.selectedCardImage = ui.ImageBox()
			self.selectedCardImage.SetParent(self.selectedCardBg)
			self.selectedCardImage.LoadImage(card_img_path[0])
			self.selectedCardImage.SetPosition(8, 8)
			self.selectedCardImage.AddFlag("float")
			self.selectedCardImage.Hide()

			self.CreateCardDestroyEffect()
		except:
			import exception
			exception.Abort("CatchKingGamePage.LoadWindow.BindObject")

		self.CreateScoreTooltip()

		self.Hide()

	def __ClickRewardButton(self):
		if self.HaveCardInHand():
			return

		if self.isLocked:
			return

		net.SendMiniGameCatchKing(3, 0)

	def __ScoreTextEffectEndFrameEvent(self):
		if self.scoreTextEffect: 
			self.scoreTextEffect.Hide()

	def __ScoreEffectKeyFrameEvent1(self, cur_frame):
		if cur_frame == 2:
			if self.scoreTextEffect:
				self.scoreTextEffect.Show()

			if self.scoreEffect2:
				self.scoreEffect2.Show()

	def __ScoreEffectKeyFrameEvent2(self, cur_frame):
		if cur_frame == 1:
			if self.scoreEffect3:
				self.scoreEffect3.Show()

	def __ScoreEffectEndFrameEvent1(self):
		if self.scoreEffect1:
			self.scoreEffect1.Hide()

	def __ScoreEffectEndFrameEvent2(self):
		if self.scoreEffect2:
			self.scoreEffect2.Hide()

	def __ScoreEffectEndFrameEvent3(self):
		if self.scoreEffect3:
			self.scoreEffect3.Hide()

	def __ClearScoreCompletionEffect(self):
		if self.scoreTextEffect:
			self.scoreTextEffect.Hide()
			self.scoreTextEffect.ResetFrame()
			self.scoreTextEffect.SetDelay(6)

		if self.scoreEffect1:
			self.scoreEffect1.Hide()
			self.scoreEffect1.ResetFrame()
			self.scoreEffect1.SetDelay(6)

		if self.scoreEffect2:
			self.scoreEffect2.Hide()
			self.scoreEffect2.ResetFrame()
			self.scoreEffect2.SetDelay(6)

		if self.scoreEffect3:
			self.scoreEffect3.Hide()
			self.scoreEffect3.ResetFrame()
			self.scoreEffect3.SetDelay(6)

	def CreateScoreTooltip(self):
		item.SelectItem(LOW_SCORE_VNUM)
		self.scoreInfo.append(localeInfo.MINI_GAME_CATCHKING_TOOLTIP_SCORE3 % item.GetItemName())

		item.SelectItem(MID_SCORE_VNUM)
		self.scoreInfo.append(localeInfo.MINI_GAME_CATCHKING_TOOLTIP_SCORE2 % item.GetItemName())

		item.SelectItem(HIGH_SCORE_VNUM)
		self.scoreInfo.append(localeInfo.MINI_GAME_CATCHKING_TOOLTIP_SCORE1 % item.GetItemName())

		self.scoreInfo.append(localeInfo.MINI_GAME_CATCHKING_TOOLTIP_SCORE4)

	def CreateCardDestroyEffect(self):
		self.destroyCardEffect = ui.AniImageBox()
		self.destroyCardEffect.SetParent(self.myHandCardBg)
		self.destroyCardEffect.SetDelay(6)
		self.destroyCardEffect.AppendImage("D:/Ymir Work/UI/minigame/catchking/effect/explosion/1.sub")
		self.destroyCardEffect.AppendImage("D:/Ymir Work/UI/minigame/catchking/effect/explosion/2.sub")
		self.destroyCardEffect.AppendImage("D:/Ymir Work/UI/minigame/catchking/effect/explosion/3.sub")
		self.destroyCardEffect.AppendImage("D:/Ymir Work/UI/minigame/catchking/effect/explosion/4.sub")
		self.destroyCardEffect.AppendImage("D:/Ymir Work/UI/minigame/catchking/effect/explosion/5.sub")
		self.destroyCardEffect.AppendImage("D:/Ymir Work/UI/minigame/catchking/effect/explosion/6.sub")
		self.destroyCardEffect.AppendImage("D:/Ymir Work/UI/minigame/catchking/effect/explosion/7.sub")
		self.destroyCardEffect.AppendImage("D:/Ymir Work/UI/minigame/catchking/effect/explosion/8.sub")
		self.destroyCardEffect.Hide()
		self.destroyCardEffect.AddFlag("not_pick")
		self.destroyCardEffect.AddFlag("float")

	def SetEndEffectCardEvent(self, event, keepFieldCard, destroyHandCard, cardValue, isFiveNear):
		if self.destroyCardEffect:
			self.destroyCardEffect.SetEndFrameEvent(ui.__mem_func__(event), keepFieldCard, destroyHandCard, cardValue, isFiveNear)

	def ShowCardDestroyEffect(self):
		if not self.myHandCardImage:
			return

		if self.destroyCardEffect:
			self.destroyCardEffect.SetPosition(-32, -42)
			self.destroyCardEffect.SetTop()
			self.destroyCardEffect.ResetFrame()
			self.destroyCardEffect.Show()

	def IsCheckShowPopUp(self):
		if self.checkImage:
			if self.checkImage.IsShow():
				return True
			else:
				return False

		return False

	def __ClickConfirmCheckButton(self, event_type):
		if self.checkImage:
			if "mouse_click" == event_type:
				if self.checkImage.IsShow():
					self.checkImage.Hide()
				else:
					self.checkImage.Show()
			else:
				pass

	def ClickCard(self, eventType, cardNumber):
		if not self.HaveCardInHand():
			return

		if self.isLocked:
			return

		net.SendMiniGameCatchKing(2, cardNumber)

	def ClickHandCard(self, eventType, cardNumber):
		if self.HaveCardInHand():
			return

		if not self.handCardList[cardNumber].IsShowArrow():
			return

		if self.isLocked:
			return

		net.SendMiniGameCatchKing(1, 0)

	def SetEndCard(self, cardPos, cardNumber):
		if cardPos < 0 or cardPos >= FIELD_CARD_MAX:
			return

		if cardNumber < 1 or cardNumber > HAND_CARD_MAX:
			return

		self.gameCardList[cardPos].SetEndCardNumber(cardNumber)

	def SetHandCard(self, cardNumber):
		if cardNumber < 1 or cardNumber > HAND_CARD_MAX:
			return

		cardPos = cardNumber - 1

		self.handCardList[cardPos].ArrowImgHide()
		self.handCardList[cardPos].SetCardCount(self.handCardList[cardPos].GetCardCount() - 1)

		(x, y) = self.handCardList[cardPos].GetGlobalPos()

		self.handCardNumber = cardNumber

		if self.myHandCardImage:
			self.myHandCardImage.LoadImage(card_img_path[self.handCardNumber])
			self.myHandCardImage.Show()

	def ShowPopupDialog(self, handCard, fieldCard, isFiveNear):
		if not self.popupResult:
			self.popupResult = uiCommon.PopupDialog()
			self.popupResult.SetWidth(390)

		if handCard == HAND_CARD_MAX - 1 and isFiveNear:
			self.popupResult.SetText(localeInfo.MINI_GAME_CATCHKING_HELP_MSG4)
			self.popupResult.Open()
			return

		if handCard == HAND_CARD_MAX and fieldCard != HAND_CARD_MAX:
			self.popupResult.SetText(localeInfo.MINI_GAME_CATCHKING_POPUP_CATCH_FAIL)
			self.popupResult.Open()
			return

		if handCard < fieldCard:
			startText = localeInfo.MINI_GAME_CATCHKING_HELP_MSG1
		elif handCard == fieldCard:
			startText = localeInfo.MINI_GAME_CATCHKING_HELP_MSG2
		elif handCard > fieldCard:
			startText = localeInfo.MINI_GAME_CATCHKING_HELP_MSG3

		self.popupResult.SetText(startText)
		self.popupResult.Open()

	def ResultEffectHandCard(self, keepFieldCard, destroyHandCard, cardValue, isFiveNear):
		if self.IsCheckShowPopUp():
			self.ShowPopupDialog(self.handCardNumber, cardValue, isFiveNear)

		if destroyHandCard:
			if self.myHandCardImage:
				self.myHandCardImage.Hide()

			for card in self.handCardList:
				if card.GetCardCount() > 0:
					if self.handCardNumber == card.GetCardNumber():
						net.SendMiniGameCatchKing(1, 0)
						self.handCardNumber = 0
					else:
						card.ArrowImgShow()
					break

			self.handCardNumber = 0

		if self.selectedCardImage:
			self.selectedCardImage.Hide()

		if self.destroyCardEffect:
			self.destroyCardEffect.Hide()

		self.isLocked = False

	def ShowFiveNearEffect(self, cardPos, endFunc, rowType, cardValue, keepFieldCard, destroyHandCard, getReward, isFiveNear):
		checkPos = []
		checkPos.append(cardPos - 5)
		checkPos.append(cardPos + 5)

		if cardPos % 10 != 4 and cardPos % 10 != 9:
			checkPos.append(cardPos + 1)
			checkPos.append(cardPos - 5 + 1)
			checkPos.append(cardPos + 5 + 1)

		if cardPos % 10 != 0 and cardPos % 10 != 5:
			checkPos.append(cardPos - 1)
			checkPos.append(cardPos - 5 - 1)
			checkPos.append(cardPos + 5 - 1)

		for i in xrange(len(self.gameCardList)):
			if i in checkPos:
				self.gameCardList[i].ShowFiveNearEffect(endFunc, rowType, cardPos, cardValue, keepFieldCard, destroyHandCard, getReward, isFiveNear)

	def ResultField(self, score, rowType, cardPos, cardValue, keepFieldCard, destroyHandCard, getReward, isFiveNear):
		if cardPos < 0 or cardPos >= FIELD_CARD_MAX:
			return

		self.isLocked = True

		self.SetScore(score)

		self.gameCardList[cardPos].ArrowImgHide()
		self.gameCardList[cardPos].SetCardNumber(cardValue, False)

		if self.selectedCardImage:
			self.selectedCardImage.LoadImage(card_img_path[cardValue])
			self.selectedCardImage.Show()

		if isFiveNear:
			self.ShowFiveNearEffect(cardPos, self.EndFiveNearEffect, rowType, cardValue, keepFieldCard, destroyHandCard, getReward, isFiveNear)
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MINI_GAME_CATCHKING_SEARCH_NUMBER5)
		else:
			self.EndFiveNearEffect(rowType, cardPos, cardValue, keepFieldCard, destroyHandCard, getReward, isFiveNear)

	def SetRow(self, cardPos, rowType):
		if rowType == 0:
			return

		rowStart = 5 * (cardPos / 5);
		rowEnd = 4 + (5 * (cardPos / 5));
		colStart = cardPos - (5 * (cardPos / 5));
		colEnd = cardPos + 20 - (5 * (cardPos / 5));

		if rowType == 1 or rowType == 3: 
			while rowStart <= rowEnd:
				self.gameCardList[rowStart].ShowRowBackground()
				rowStart += 1

		if rowType == 2 or rowType == 3: 
			while colStart <= colEnd:
				self.gameCardList[colStart].ShowRowBackground()
				colStart += 5

	def EndFiveNearEffect(self, rowType, cardPos, cardValue, keepFieldCard, destroyHandCard, getReward, isFiveNear):
		if self.handCardNumber == HAND_CARD_MAX:
			self.gameCardList[cardPos].SetEndCardNumber(cardValue)

			if cardValue == HAND_CARD_MAX:
				self.__ClearScoreCompletionEffect()
				self.scoreEffect1.Show()
		else:
			if not keepFieldCard:
				self.gameCardList[cardPos].ShowCardDestroyEffect()

		self.SetEndEffectCardEvent(self.ResultEffectHandCard, keepFieldCard, destroyHandCard, cardValue, isFiveNear)
		self.ShowCardDestroyEffect()

		for card in self.gameCardList:
			card.HideFiveNearEffect()

		self.SetRow(cardPos, rowType)

	def HaveCardInHand(self):
		return self.handCardNumber

	def OverInToolTip(self, eventType, arg):
		if arg < 1 or arg > HAND_CARD_MAX:
			return

		(x, y) = self.GetGlobalPosition()
		self.infoTooltip.ClearToolTip()
		self.infoTooltip.SetThinBoardSize(395)
		self.infoTooltip.AppendTextLine(self.card_tooltip[arg])

		self.infoTooltip.SetToolTipPosition(x + (self.GetWidth() / 2), y + self.GetHeight() + self.infoTooltip.GetHeight() - 10)
		self.infoTooltip.Show()

	def OverInScoreToolTip(self):
		self.infoTooltip.ClearToolTip()
		self.infoTooltip.SetThinBoardSize(395)
		self.infoTooltip.SetToolTipPosition(-1, -1)
		for text in self.scoreInfo:
			self.infoTooltip.AppendTextLine(text)
		self.infoTooltip.Show()

	def OverOutToolTip(self):
		self.infoTooltip.Hide()

	def SetBigScore(self, bigScore):
		if self.bigScoreText:
			self.bigScoreText.SetText(str(bigScore))

	def SetScore(self, score):
		if self.scoreText:
			if score < LOW_TOTAL_SCORE:
				self.scoreText.SetPackedFontColor(grp.GenerateColor(0.78, 0.78, 0.78, 1.0))
			elif score < MID_TOTAL_SCORE:
				self.scoreText.SetPackedFontColor(grp.GenerateColor(1.0, 0.85, 0.39, 1.0))
			else:
				self.scoreText.SetPackedFontColor(grp.GenerateColor(1.0, 0.0, 0.0, 1.0))

			self.scoreText.SetText(str(score))

	def SetReward(self, rewardCode):
		if not self.popupResult:
			self.popupResult = uiCommon.PopupDialog()
			self.popupResult.SetWidth(390)

		if rewardCode == 0:
			if self.rewardButton:
				self.rewardButton.SetEvent(ui.__mem_func__(self.__ClickRetryButton))
				self.rewardButton.SetText(uiScriptLocale.MINI_GAME_CATCHKING_RETRY)

		elif rewardCode == 1:
			self.popupResult.SetText(localeInfo.MINI_GAME_CATCHKING_POPUP_REWARD_FAIL)
			self.popupResult.Open()

			if self.rewardButton:
				self.rewardButton.SetEvent(ui.__mem_func__(self.__ClickRetryButton))
				self.rewardButton.SetText(uiScriptLocale.MINI_GAME_CATCHKING_RETRY)

	def __ClickRetryButton(self):
		if self.wndMiniGameCatchKing:
			self.wndMiniGameCatchKing.StartAgain()

	def OnPressEscapeKey(self):
		self.Hide()
		return True

	def Close(self):
		if self.popupResult:
			self.popupResult.Close()

		if self.infoTooltip:
			self.infoTooltip.HideToolTip()

		self.isLocked = False

		self.Hide()

	def Clear(self):
		self.handCardList[0].SetCardCount(5)
		self.handCardList[1].SetCardCount(2)
		self.handCardList[2].SetCardCount(2)
		self.handCardList[3].SetCardCount(1)
		self.handCardList[4].SetCardCount(1)
		self.handCardList[5].SetCardCount(1)
		self.handCardList[0].ArrowImgShow()

		self.SetScore(0)

		for card in self.gameCardList:
			card.SetCardNumber(0, True)

		self.handCardNumber = 0

		if self.rewardButton:
			self.rewardButton.SetEvent(ui.__mem_func__(self.__ClickRewardButton))
			self.rewardButton.SetText(uiScriptLocale.MINI_GAME_CATCHKING_REWARD)

	def Destroy(self):
		self.isLoaded = 0

		self.fieldBackground = None

		self.myHandCardImage = None
		self.selectedCardImage = None

		self.gameCardList = []
		self.handCardList = []
		self.scoreInfo = []

		self.handCardNumber = 0

		self.infoTooltip = None

		self.confirmWindowCheckButton = None
		self.checkImage = None

		self.bigScoreText = None
		self.scoreWnd = None
		self.scoreText = None

		self.rewardButton = None

		self.myHandCardBg = None
		self.selectedCardBg = None

		self.destroyCardEffect = None
		self.popupResult = None

		self.scoreTextEffect = None
		self.scoreEffect1 = None
		self.scoreEffect2 = None
		self.scoreEffect3 = None

		self.isLocked = False

	def Show(self):
		ui.ScriptWindow.Show(self)

		if self.checkImage:
			self.checkImage.Show()

class MiniGameCatchKing(ui.Window):
	def __init__(self):
		ui.Window.__init__(self)
		self.isLoaded = 0
		self.state = STATE_NONE

		self.cur_page = None
		self.waiting_page = None
		self.game_page = None

		self.__LoadWindow()

	def __del__(self):
		ui.Window.__del__(self)

	def Show(self):
		self.__LoadWindow()
		ui.Window.Show(self)

	def Close(self):
		self.Hide()

		if self.cur_page:
			self.cur_page.Close()

	def Destroy(self):
		self.isLoaded = 0
		self.state = STATE_NONE
		self.cur_page = None

		if self.waiting_page:
			self.waiting_page.Destroy()
			self.waiting_page = None

		if self.game_page:
			self.game_page.Destroy()
			self.game_page = None

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1
		self.state = STATE_WAITING

		try:
			self.waiting_page = CatchKingWaitingPage()
			self.game_page = CatchKingGamePage(self)
		except:
			import exception
			exception.Abort("MiniGameCatchKing.LoadWindow")

		self.Hide()

	def Open(self):
		if STATE_WAITING == self.state:
			self.cur_page = self.waiting_page

		elif STATE_PLAY == self.state:
			self.cur_page = self.game_page

		else:
			return

		if self.cur_page.IsShow():
			self.cur_page.Close()
		else:
			self.cur_page.Show()
			self.cur_page.SetTop()

	def GameStart(self, bigScore):
		if self.cur_page:
			if app.ENABLE_CATCH_KING_EVENT_FLAG_RENEWAL:
				self.cur_page.DecreaseMiniGameCatchKingCardCount()
			self.cur_page.Close()

		self.state = STATE_PLAY

		if self.game_page:
			self.game_page.Clear()
			self.game_page.SetBigScore(bigScore)
			self.game_page.Show()

	def GameEnd(self):
		self.state = STATE_WAITING
		if self.game_page:
			self.game_page.Clear()
			self.game_page.Close()

	def StartAgain(self):
		self.state = STATE_WAITING
		if self.game_page:
			self.game_page.Clear()
			self.game_page.Close()

		if self.waiting_page:
			self.waiting_page.Show()

	def CatchKingSetHandCard(self, cardNumber):
		if self.game_page:
			self.game_page.SetHandCard(cardNumber)

	def CatchKingResultField(self, score, rowType, cardPos, cardValue, keepFieldCard, destroyHandCard, getReward, isFiveNear):
		if self.game_page:
			self.game_page.ResultField(score, rowType, cardPos, cardValue, keepFieldCard, destroyHandCard, getReward, isFiveNear)

	def CatchKingSetEndCard(self, cardPos, cardNumber):
		if self.game_page:
			self.game_page.SetEndCard(cardPos, cardNumber)

	def CatchKingReward(self, rewardCode):
		if self.game_page:
			self.game_page.SetReward(rewardCode)

	if app.ENABLE_CATCH_KING_EVENT_FLAG_RENEWAL:
		def SetItemToolTip(self, tooltip):
			if self.waiting_page:
				self.waiting_page.SetItemToolTip(tooltip)

		def CatchKingFlagProcess(self, type, data):
			if self.waiting_page:
				self.waiting_page.CatchKingFlagProcess(type, data)
