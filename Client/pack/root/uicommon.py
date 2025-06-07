import ui
import localeInfo
import app
import ime
import uiScriptLocale
import chat

if app.ENABLE_CHEQUE_SYSTEM or app.ENABLE_NEW_DROP_DIALOG:
	import player

if app.ENABLE_NEW_DROP_DIALOG:
	import item, uiToolTip

if app.ENABLE_MOUSE_WHEEL_TOP_WINDOW:
	import wndMgr

class PopupDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadDialog()
		self.acceptEvent = lambda *arg: None

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadDialog(self):
		try:
			PythonScriptLoader = ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self, "UIScript/PopupDialog.py")

			self.board = self.GetChild("board")
			self.message = self.GetChild("message")
			self.accceptButton = self.GetChild("accept")
			self.accceptButton.SetEvent(ui.__mem_func__(self.Close))

		except:
			import exception
			exception.Abort("PopupDialog.LoadDialog.BindObject")

	def Open(self):
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		self.Hide()
		self.acceptEvent()

	def Destroy(self):
		self.Close()
		self.ClearDictionary()

	def SetWidth(self, width):
		height = self.GetHeight()
		self.SetSize(width, height)

		if self.board.IsRTL():
			self.board.SetPosition(width, 0)

		self.board.SetSize(width, height)
		self.SetCenterPosition()
		self.UpdateRect()

	def SetText(self, text):
		self.message.SetText(text)

	def SetAcceptEvent(self, event):
		self.acceptEvent = event

	def SetButtonName(self, name):
		self.accceptButton.SetText(name)

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def OnIMEReturn(self):
		self.Close()
		return True

	def GetTextSize(self):
		if self.message:
			return self.message.GetTextSize()

		return (0, 0)

	def GetLineHeight(self):
		if self.message:
			return self.message.GetLineHeight()

		return 0

	def SetLineHeight(self, Height):
		self.message.SetLineHeight(Height)

	def GetTextLineCount(self):
		return self.message.GetTextLineCount()

	if app.ENABLE_MINI_GAME_YUTNORI:
		def SetButtonNameAutoSize(self, name):
			self.accceptButton.SetAutoSizeText(name)

		def SetButtonHorizontalAlignCenter(self):
			self.accceptButton.SetWindowHorizontalAlignCenter()

		def SetButtonUpVisual(self, filename):
			self.accceptButton.SetUpVisual(filename)

		def SetButtonOverVisual(self, filename):
			self.accceptButton.SetOverVisual(filename)

		def SetButtonDownVisual(self, filename):
			self.accceptButton.SetDownVisual(filename)

if app.ENABLE_MINI_GAME_YUTNORI:
	class PopupDialog2(ui.ScriptWindow):

		def __init__(self):
			ui.ScriptWindow.__init__(self)
			self.__LoadDialog()
			self.acceptEvent = lambda *arg: None

		def __del__(self):
			ui.ScriptWindow.__del__(self)

		def __LoadDialog(self):
			try:
				PythonScriptLoader = ui.PythonScriptLoader()
				PythonScriptLoader.LoadScriptFile(self, "UIScript/PopupDialog2.py")

				self.board = self.GetChild("board")
				self.message1 = self.GetChild("message1")
				self.message2 = self.GetChild("message2")
				self.accceptButton = self.GetChild("accept")
				self.accceptButton.SetEvent(ui.__mem_func__(self.Close))

			except:
				import exception
				exception.Abort("PopupDialog2.LoadDialog.BindObject")

		def Open(self):
			self.SetCenterPosition()
			self.SetTop()
			self.Show()

		def Close(self):
			self.Hide()
			self.acceptEvent()

		def Destroy(self):
			self.Close()
			self.ClearDictionary()

		def SetWidth(self, width):
			height = self.GetHeight()
			self.SetSize(width, height)

			if self.board.IsRTL():
				self.board.SetPosition(width, 0)

			self.board.SetSize(width, height)
			self.SetCenterPosition()
			self.UpdateRect()

		def SetText1(self, text):
			self.message1.SetText(text)
		def SetText2(self, text):
			self.message2.SetText(text)

		def SetAcceptEvent(self, event):
			self.acceptEvent = event

		def SetButtonName(self, name):
			self.accceptButton.SetText(name)

		def OnPressEscapeKey(self):
			self.Close()
			return True

		def OnIMEReturn(self):
			self.Close()
			return True

		def GetTextSize(self):
			if self.message1:
				return self.message1.GetTextSize()

			return (0,0)

		def GetLineHeight(self):
			if self.message1:
				return self.message1.GetLineHeight()

			return 0

		def SetLineHeight(self, Height):
			self.message1.SetLineHeight(Height)

		def GetTextLineCount(self):
			return self.message1.GetTextLineCount()

class InputDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.__CreateDialog()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __CreateDialog(self):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "uiscript/inputdialog.py")

		getObject = self.GetChild
		self.board = getObject("Board")
		self.acceptButton = getObject("AcceptButton")
		self.cancelButton = getObject("CancelButton")
		self.inputSlot = getObject("InputSlot")
		self.inputValue = getObject("InputValue")

	def Open(self):
		self.inputValue.SetFocus()
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		self.ClearDictionary()
		self.board = None
		self.acceptButton = None
		self.cancelButton = None
		self.inputSlot = None
		self.inputValue = None
		self.Hide()

	def SetTitle(self, name):
		self.board.SetTitleName(name)

	def SetNumberMode(self):
		self.inputValue.SetNumberMode()

	#MT-679 개인 상점 타이틀의 CodePage 이슈
	def SetUseCodePage(self, bUse = True):
		self.inputValue.SetUseCodePage(bUse)

	def SetSecretMode(self):
		self.inputValue.SetSecret()

	def SetFocus(self):
		self.inputValue.SetFocus()

	def SetMaxLength(self, length):
		width = length * 6 + 10
		self.SetBoardWidth(max(width + 50, 160))
		self.SetSlotWidth(width)
		self.inputValue.SetMax(length)

	def SetSlotWidth(self, width):
		self.inputSlot.SetSize(width, self.inputSlot.GetHeight())
		self.inputValue.SetSize(width, self.inputValue.GetHeight())
		if self.IsRTL():
			self.inputValue.SetPosition(self.inputValue.GetWidth(), 0)

	def SetBoardWidth(self, width):
		self.SetSize(max(width + 50, 160), self.GetHeight())
		self.board.SetSize(max(width + 50, 160), self.GetHeight())
		if self.IsRTL():
			self.board.SetPosition(self.board.GetWidth(), 0)
		self.UpdateRect()

	def SetAcceptEvent(self, event):
		self.acceptButton.SetEvent(event)
		self.inputValue.OnIMEReturn = event

	def SetCancelEvent(self, event):
		self.board.SetCloseEvent(event)
		self.cancelButton.SetEvent(event)
		self.inputValue.OnPressEscapeKey = event

	def GetText(self):
		return self.inputValue.GetText()

class InputDialogWithDescription(InputDialog):
	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.__CreateDialog()

	def __del__(self):
		InputDialog.__del__(self)

	def __CreateDialog(self):
		pyScrLoader = ui.PythonScriptLoader()
		if localeInfo.IsARABIC():
			pyScrLoader.LoadScriptFile(self, uiScriptLocale.LOCALE_UISCRIPT_PATH + "inputdialogwithdescription.py")
		else:
			pyScrLoader.LoadScriptFile(self, "uiscript/inputdialogwithdescription.py")

		try:
			getObject = self.GetChild
			self.board = getObject("Board")
			self.acceptButton = getObject("AcceptButton")
			self.cancelButton = getObject("CancelButton")
			self.inputSlot = getObject("InputSlot")
			self.inputValue = getObject("InputValue")
			self.description = getObject("Description")

		except:
			import exception
			exception.Abort("InputDialogWithDescription.LoadBoardDialog.BindObject")

	def SetDescription(self, text):
		self.description.SetText(text)

class InputDialogWithDescription2(InputDialog):
	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.__CreateDialog()

	def __del__(self):
		InputDialog.__del__(self)

	def __CreateDialog(self):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "uiscript/inputdialogwithdescription2.py")

		try:
			getObject = self.GetChild
			self.board = getObject("Board")
			self.acceptButton = getObject("AcceptButton")
			self.cancelButton = getObject("CancelButton")
			self.inputSlot = getObject("InputSlot")
			self.inputValue = getObject("InputValue")
			self.description1 = getObject("Description1")
			self.description2 = getObject("Description2")

		except:
			import exception
			exception.Abort("InputDialogWithDescription.LoadBoardDialog.BindObject")

	def SetDescription1(self, text):
		self.description1.SetText(text)

	def SetDescription2(self, text):
		self.description2.SetText(text)

class QuestionDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__CreateDialog()

		if app.ENABLE_GAME_OPTION_ESCAPE:
			self.accept_event_func = None
			self.cancel_event_func = None

	def __del__(self):
		ui.ScriptWindow.__del__(self)

		if app.ENABLE_GAME_OPTION_ESCAPE:
			self.accept_event_func = None
			self.cancel_event_func = None

	def __CreateDialog(self):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "uiscript/questiondialog.py")

		self.board = self.GetChild("board")
		self.textLine = self.GetChild("message")
		self.acceptButton = self.GetChild("accept")
		self.cancelButton = self.GetChild("cancel")

	def Open(self):
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		self.Hide()

	def SetWidth(self, width):
		height = self.GetHeight()
		self.SetSize(width, height)

		if self.board.IsRTL():
			self.board.SetPosition(width, 0)

		self.board.SetSize(width, height)
		self.SetCenterPosition()
		self.UpdateRect()

	def SAFE_SetAcceptEvent(self, event):
		if app.ENABLE_GAME_OPTION_ESCAPE:
			self.accept_event_func = ui.__mem_func__(event)
		self.acceptButton.SAFE_SetEvent(event)

	def SAFE_SetCancelEvent(self, event):
		if app.ENABLE_GAME_OPTION_ESCAPE:
			self.cancel_event_func = ui.__mem_func__(event)
		self.cancelButton.SAFE_SetEvent(event)

	def SetAcceptEvent(self, event):
		if app.ENABLE_GAME_OPTION_ESCAPE:
			self.accept_event_func = event
		self.acceptButton.SetEvent(event)

	def SetCancelEvent(self, event):
		if app.ENABLE_GAME_OPTION_ESCAPE:
			self.cancel_event_func = event
		self.cancelButton.SetEvent(event)

	def SetText(self, text):
		self.textLine.SetText(text)

	def SetAcceptText(self, text):
		self.acceptButton.SetText(text)

	def SetCancelText(self, text):
		self.cancelButton.SetText(text)

	def OnPressEscapeKey(self):
		if app.ENABLE_GAME_OPTION_ESCAPE:
			if self.cancel_event_func:
				apply(self.cancel_event_func)

		self.Close()
		return True

	def GetTextSize(self):
		if self.textLine:
			return self.textLine.GetTextSize()

		return (0,0)

	def GetLineHeight(self):
		if self.textLine:
			return self.textLine.GetLineHeight()

		return 0

	if app.WJ_MULTI_TEXTLINE:
		def SetLineHeight(self, height):
			self.textLine.SetLineHeight(height)

			x_accept, y_accept = self.acceptButton.GetLocalPosition()
			self.acceptButton.SetPosition(x_accept, y_accept + height)

			x_cancel, y_cancel = self.cancelButton.GetLocalPosition()
			self.cancelButton.SetPosition(x_cancel, y_cancel + height)

			height_board = self.GetHeight() + height
			width_board = self.GetWidth()
			self.SetSize(width_board, height_board)

			if self.board.IsRTL():
				self.board.SetPosition(width_board, 0)

			self.board.SetSize(width_board, height_board)
			self.SetCenterPosition()
			self.UpdateRect()

		def GetTextLineCount(self):
			return self.textLine.GetTextLineCount()

class QuestionDialog2(QuestionDialog):
	def __init__(self):
		QuestionDialog.__init__(self)
		self.__CreateDialog()

	def __del__(self):
		QuestionDialog.__del__(self)

	def __CreateDialog(self):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "uiscript/questiondialog2.py")

		self.board = self.GetChild("board")
		self.textLine1 = self.GetChild("message1")
		self.textLine2 = self.GetChild("message2")
		self.acceptButton = self.GetChild("accept")
		self.cancelButton = self.GetChild("cancel")

	def SetText1(self, text):
		self.textLine1.SetText(text)

	def SetText2(self, text):
		self.textLine2.SetText(text)

class QuestionDialogWithTimeLimit(QuestionDialog2):
	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.__CreateDialog()
		self.endTime = 0
		self.timeoverMsg = None
		self.isCancelOnTimeover = False

	def __del__(self):
		QuestionDialog2.__del__(self)

	def __CreateDialog(self):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "uiscript/questiondialog2.py")

		self.board = self.GetChild("board")
		self.textLine1 = self.GetChild("message1")
		self.textLine2 = self.GetChild("message2")
		self.acceptButton = self.GetChild("accept")
		self.cancelButton = self.GetChild("cancel")

	def Open(self, msg, timeout):
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

		self.SetText1(msg)
		self.endTime = app.GetTime() + timeout

	def OnUpdate(self):
		leftTime = max(0, self.endTime - app.GetTime())
		self.SetText2(localeInfo.UI_LEFT_TIME % (leftTime))
		if leftTime < 0.5:
			if self.timeoverMsg:
				chat.AppendChat(chat.CHAT_TYPE_INFO, self.timeoverMsg)
			if self.isCancelOnTimeover:
				self.cancelButton.CallEvent()

	def SetTimeOverMsg(self, msg):
		self.timeoverMsg = msg

	def SetCancelOnTimeOver(self):
		self.isCancelOnTimeover = True

	def OnPressEscapeKey(self):
		if self.cancelButton:
			self.cancelButton.CallEvent()

		self.Close()
		return True

if app.ENABLE_NEW_DROP_DIALOG:
	class QuestionDropDialog(ui.ScriptWindow):
		def __init__(self):
			ui.ScriptWindow.__init__(self)
			self.__LoadWindow()

		def __del__(self):
			ui.ScriptWindow.__del__(self)
			del self.itemSlot
			del self.itemToolTip

		def __LoadWindow(self):
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/questiondropdialog.py")

			self.board = self.GetChild("board")
			self.textLine = self.GetChild("message")
			self.acceptButton = self.GetChild("accept")
			self.destroyButton = self.GetChild("destroy")
			self.cancelButton = self.GetChild("cancel")

			self.itemSlot = self.GetChild("ItemSlot")
			self.itemSlot.SetOverInItemEvent(ui.__mem_func__(self.__OnOverInItem))
			self.itemSlot.SetOverOutItemEvent(ui.__mem_func__(self.__OnOverOutItem))

		def SetItemSlot(self, wndType, slotIndex):
			itemIndex = player.GetItemIndex(wndType, slotIndex)
			itemCount = player.GetItemCount(wndType, slotIndex)

			self.itemSlot.SetItemSlot(0, itemIndex, itemCount)

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				if not player.GetChangeLookVnum(wndType, slotIndex) == 0:
					self.itemSlot.SetSlotCoverImage(slotIndex, "icon/item/ingame_convert_Mark.tga")
				else:
					self.itemSlot.EnableSlotCoverImage(slotIndex, False)

			item.SelectItem(itemIndex)

			if item.IsAntiFlag(item.ITEM_ANTIFLAG_DROP):
				self.acceptButton.Down()
				self.acceptButton.Disable()

			if item.IsAntiFlag(item.ITEM_ANTIFLAG_DESTROY):
				self.destroyButton.Down()
				self.destroyButton.Disable()

			if app.ENABLE_SOULBIND_SYSTEM:
				if player.GetItemSealDate(wndType, slotIndex) != item.E_SEAL_DATE_DEFAULT_TIMESTAMP:

					self.acceptButton.Down()
					self.acceptButton.Disable()

					self.destroyButton.Down()
					self.destroyButton.Disable()

			self.itemToolTip = uiToolTip.ItemToolTip()
			self.itemToolTip.SetInventoryItem(slotIndex, wndType)

		def __OnOverInItem(self, slotIndex):
			if self.itemToolTip:
				self.itemToolTip.ShowToolTip()

		def __OnOverOutItem(self):
			if self.itemToolTip:
				self.itemToolTip.HideToolTip()

		def Open(self):
			if self.itemToolTip:
				self.itemToolTip.HideToolTip()

			self.SetCenterPosition()
			self.SetTop()
			self.Show()

		def Close(self):
			if self.itemToolTip:
				self.itemToolTip.HideToolTip()

			self.Hide()

		def OnPressEscapeKey(self):
			self.Close()
			return True

		def SetText(self, text):
			self.textLine.SetText(text)

		def SetAcceptEvent(self, event):
			self.acceptButton.SetEvent(event)

		def SetDestroyEvent(self, event):
			self.destroyButton.SetEvent(event)

		def SetCancelEvent(self, event):
			self.cancelButton.SetEvent(event)

class MoneyInputDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.moneyHeaderText = localeInfo.MONEY_INPUT_DIALOG_SELLPRICE
		self.__CreateDialog()

		self.SetMaxLength(10)

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __CreateDialog(self):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "UIScript/MoneyInputDialog.py")

		getObject = self.GetChild
		self.board = self.GetChild("board")
		self.acceptButton = getObject("AcceptButton")
		self.cancelButton = getObject("CancelButton")
		self.inputValue = getObject("InputValue")
		self.inputValue.SetNumberMode()
		self.inputValue.OnIMEUpdate = ui.__mem_func__(self.__OnValueUpdate)
		self.moneyText = getObject("MoneyValue")

		if app.ENABLE_CHEQUE_SYSTEM:
			self.chequeText = getObject("ChequeValue")
			self.inputChequeValue = getObject("InputValue_Cheque")
			self.inputChequeValue.OnIMEUpdate = ui.__mem_func__(self.__OnValueUpdate)
			self.inputChequeValue.OnMouseLeftButtonDown = ui.__mem_func__(self.__ClickChequeEditLine)
			self.inputValue.OnMouseLeftButtonDown = ui.__mem_func__(self.__ClickValueEditLine)

	def Open(self):
		self.inputValue.SetText("")
		self.inputValue.SetFocus()
		self.__OnValueUpdate()
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		if app.ENABLE_CHEQUE_SYSTEM:
			self.inputChequeValue = None

		self.ClearDictionary()
		self.board = None
		self.acceptButton = None
		self.cancelButton = None
		self.inputValue = None
		self.Hide()

	def SetTitle(self, name):
		self.board.SetTitleName(name)

	def SetFocus(self):
		self.inputValue.SetFocus()

	def SetMaxLength(self, length):
		length = min(10, length)

		self.inputValue.SetMax(length)

	def SetMoneyHeaderText(self, text):
		self.moneyHeaderText = text

	def SetAcceptEvent(self, event):
		self.acceptButton.SetEvent(event)
		self.inputValue.OnIMEReturn = event

	def SetCancelEvent(self, event):
		self.board.SetCloseEvent(event)
		self.cancelButton.SetEvent(event)
		self.inputValue.OnPressEscapeKey = event

	def SetValue(self, value):
		value = str(value)
		self.inputValue.SetText(value)
		self.__OnValueUpdate()
		ime.SetCursorPosition(len(value) + 1)

	def GetText(self):
		return self.inputValue.GetText()

	if app.ENABLE_CHEQUE_SYSTEM:
		def SetCheque(self, cheque):
			cheque = str(cheque)
			self.inputChequeValue.SetText(cheque)
			self.__OnValueUpdate()
			ime.SetCursorPosition(len(cheque) + 1)

		def __ClickChequeEditLine(self):
			self.inputChequeValue.SetFocus()
			if len(self.inputValue.GetText()) <= 0:
				self.inputValue.SetText(str(0))

		def __ClickValueEditLine(self):
			self.inputValue.SetFocus()
			if len(self.inputChequeValue.GetText()) <= 0:
				self.inputChequeValue.SetText(str(0))

		def GetCheque(self):
			return self.inputChequeValue.GetText()

		def __OnValueUpdate(self):
			if self.inputValue.IsFocus():
				ui.EditLine.OnIMEUpdate(self.inputValue)
			elif self.inputChequeValue.IsFocus():
				ui.EditLine.OnIMEUpdate(self.inputChequeValue)
			else:
				pass

			text = self.inputValue.GetText()
			cheque_text = self.inputChequeValue.GetText()

			money = 0
			cheque = 0

			if text and text.isdigit():
				try:
					money = int(text)

					if money >= player.GOLD_MAX:
						money = player.GOLD_MAX - 1
						self.inputValue.SetText(str(money))
				except ValueError:
					money = 0

			if cheque_text and cheque_text.isdigit():
				try:
					cheque = int(cheque_text)

					if cheque > player.CHEQUE_MAX:
						cheque = player.CHEQUE_MAX
						self.inputChequeValue.SetText(str(cheque))
				except ValueError:
					cheque = 0

			self.chequeText.SetText(str(cheque) + " " + localeInfo.CHEQUE_SYSTEM_UNIT_WON)
			self.moneyText.SetText(localeInfo.NumberToMoneyString(money) + " " + localeInfo.CHEQUE_SYSTEM_UNIT_YANG)
	else:
		def __OnValueUpdate(self):
			ui.EditLine.OnIMEUpdate(self.inputValue)

			text = self.inputValue.GetText()

			money = 0
			if text and text.isdigit():
				try:
					money = int(text)

				except ValueError:
					money = 199999999

			self.moneyText.SetText(self.moneyHeaderText + localeInfo.NumberToMoneyString(money))

if app.ENABLE_EVENT_BANNER:
	class RewardListDialog(ui.ScriptWindow):

		REWARD_LIST_WIDTH_COUNT = 5
		SHOW_REWARD_LIMIT_COUNT = 25

		def __init__(self):
			ui.ScriptWindow.__init__(self)

			self.called_window = None
			self.reward_slot_window = None
			self.reward_tooltip = None
			self.scroll_bar = None
			self.cur_scroll_bar_index = 0
			self.item_data_dict = {}

			self.__LoadWindow()

		def __del__(self):
			ui.ScriptWindow.__del__(self)

		def __LoadWindow(self):
			try:
				pyScrLoader = ui.PythonScriptLoader()
				pyScrLoader.LoadScriptFile(self, "UIScript/RewardListWindow.py")
			except:
				import exception
				exception.Abort("RewardListDialog.LoadWindow.LoadObject")

			try:
				self.GetChild("title_bar").SetCloseEvent(ui.__mem_func__(self.Close))

				self.reward_slot_window = self.GetChild("reward_item_slot")
				self.reward_slot_window.SetOverInItemEvent(ui.__mem_func__(self.__SlotOverInItem))
				self.reward_slot_window.SetOverOutItemEvent(ui.__mem_func__(self.__SlotOverOutItem))

				self.scroll_bar = self.GetChild("reward_list_scroll_bar")
				self.scroll_bar.SetScrollEvent(ui.__mem_func__(self.__OnScrollBarRewardListEvent))
			except:
				import exception
				exception.Abort("RewardListDialog.LoadWindow.BindObject")

		def __SlotOverInItem(self, slot_index):
			if self.reward_tooltip == None:
				return

			scroll_slot_idx = self.cur_scroll_bar_index + slot_index
			(item_vnum, item_count) = self.item_data_dict[scroll_slot_idx]

			if item_vnum != 0 and item_count != 0:
				item_socket_list = [0 for i in xrange(player.METIN_SOCKET_MAX_NUM)]
				item_attr_list = [(0, 0) for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]

				self.reward_tooltip.ClearToolTip()
				self.reward_tooltip.AddItemData(item_vnum, item_socket_list, item_attr_list)

		def __SlotOverOutItem(self):
			if self.reward_tooltip != None:
				self.reward_tooltip.HideToolTip()

		def __SetScrollBar(self):
			reward_item_count = len(self.item_data_dict)
			if reward_item_count > self.reward_slot_window.GetSlotCount():
				scroll_diff = reward_item_count - self.reward_slot_window.GetSlotCount()

				if scroll_diff > 0:
					scroll_step_size = 1.0 / scroll_diff
					self.scroll_bar.SetScrollStep(scroll_step_size)
					self.cur_scroll_bar_index = int(self.scroll_bar.GetPos() * scroll_diff)
				else:
					self.cur_scroll_bar_index = 0

				self.scroll_bar.Show()
			else:
				self.cur_scroll_bar_index = 0
				self.scroll_bar.Hide()

		def __OnScrollBarRewardListEvent(self):
			self.RefreshRewardList()

		def Show(self):
			ui.ScriptWindow.Show(self)

			if app.ENABLE_MOUSE_WHEEL_TOP_WINDOW:
				self.SetTop()
				wndMgr.SetWheelTopWindow(self.hWnd)

		def Close(self):
			self.ClearRewardData()

			if app.ENABLE_MOUSE_WHEEL_TOP_WINDOW:
				wndMgr.ClearWheelTopWindow(self.hWnd)

			self.Hide()

		def Hide(self):
			ui.ScriptWindow.Hide(self)

		def Destroy(self):
			self.called_window = None
			self.reward_slot_window = None
			self.reward_tooltip = None
			self.scroll_bar = None
			self.cur_scroll_bar_index = 0
			self.item_data_dict = {}

		def ScrollShow(self):
			if self.scroll_bar:
				self.scroll_bar.Show()

		def ClearRewardData(self):
			if self.scroll_bar:
				self.scroll_bar.SetPos(0.0)

			self.cur_scroll_bar_index = 0
			self.item_data_dict = {}

			for slot_idx in range(self.reward_slot_window.GetSlotCount()):
				self.reward_slot_window.ClearSlot(slot_idx)

		def RefreshRewardList(self):
			if not self.reward_slot_window:
				return

			self.__SetScrollBar()

			for slot_idx in range(self.reward_slot_window.GetSlotCount()):
				scroll_slot_idx = self.cur_scroll_bar_index + slot_idx

				if scroll_slot_idx in self.item_data_dict:
					(item_vnum, item_count) = self.item_data_dict[scroll_slot_idx]
					self.reward_slot_window.SetItemSlot(slot_idx, item_vnum, item_count)

		def SetCalledWindow(self, window):
			self.called_window = window

		def SetItemToolTip(self, tooltip):
			self.reward_tooltip = tooltip

		def SetRewardInfo(self, reward_info):
			self.item_data_dict = reward_info

		def SetRewardInfoFromCommonRewardList(self, common_reward_vnum):
			#self.item_data_dict = item.GetCommonRewardData(common_reward_vnum)
			return

		def OnPressEscapeKey(self):
			self.Close()
			return True

		if app.ENABLE_MOUSE_WHEEL_TOP_WINDOW:
			def OnMouseWheelButtonUp(self):
				return self.scroll_bar.OnUp() if self.scroll_bar else False

			def OnMouseWheelButtonDown(self):
				return self.scroll_bar.OnDown() if self.scroll_bar else False
