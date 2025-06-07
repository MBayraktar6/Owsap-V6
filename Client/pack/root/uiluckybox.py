import app
import uiCommon
import player
import ui
import localeInfo
import wndMgr

class LuckyBoxWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.isLoaded = 0
		self.tooltipItem = None
		self.questionDialog = None

		self.itemVNum = 0
		self.slotIndex = -1

		if app.WJ_ENABLE_TRADABLE_ICON:
			self.interface = None
			self.inven = None

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/LuckyBoxWindow.py")
		except:
			import exception
			exception.Abort("LuckyBoxWindow.__LoadWindow.LoadScript")

		try:
			self.TitleBar = self.GetChild("LuckyBox_TitleBar")
			self.TitleBar.CloseButtonHide()

			self.RewardItemSlot = self.GetChild("RewardItemSlot")
			self.RetryButton = self.GetChild("RetryButton")
			self.RecvButton = self.GetChild("RecvButton")
			self.NeedMoneySlot = self.GetChild("NeedMoneySlot")
			self.NeedMoneySlot.SetButtonScale(1.9, 1.0)
			self.NeedMoney = self.GetChild("NeedMoney")

			self.RewardItemSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			self.RewardItemSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

			self.RecvButton.SetEvent(ui.__mem_func__(self.OnRecv))
			self.RetryButton.SetEvent(ui.__mem_func__(self.OnRetry))

		except:
			import exception
			exception.Abort("LuckyBoxWindow.__LoadWindow.BindObject")

	def Destroy(self):
		self.ClearDictionary()

		self.tooltipItem = None
		self.questionDialog = None

		self.TitleBar = None
		self.RewardItemSlot = None
		self.RetryButton = None
		self.RecvButton = None
		self.NeedMoneySlot = None
		self.NeedMoney = None

		if app.WJ_ENABLE_TRADABLE_ICON:
			self.interface = None
			self.inven = None

	def Close(self):
		self.Hide()

		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

		if app.WJ_ENABLE_TRADABLE_ICON:
			if self.interface:
				self.interface.SetOnTopWindow(player.ON_TOP_WND_NONE)
				self.interface.RefreshMarkInventoryBag()

			if self.inven:
				self.inven.wndItem.SetCanMouseEventSlot(self.slotIndex)

	def OnPressEscapeKey(self):
		return True

	def OnUpdate(self):
		if app.WJ_ENABLE_TRADABLE_ICON:
			if not self.inven:
				return

			lock_idx = self.slotIndex
			page = self.inven.GetInventoryPageIndex()
			if (page * player.INVENTORY_PAGE_SIZE) <= lock_idx < ((page + 1) * player.INVENTORY_PAGE_SIZE):
				lock_idx = lock_idx - (page * player.INVENTORY_PAGE_SIZE)

			self.inven.wndItem.SetCantMouseEventSlot(self.slotIndex)
		else:
			pass

	def OnRecv(self):
		if self.questionDialog:
			return

		questionDialog = uiCommon.QuestionDialog()
		questionDialog.SetText(localeInfo.LUCKY_BOX_DIALOG_RECV_TEXT)
		questionDialog.SetAcceptEvent(ui.__mem_func__(self.__popUpDialogAccept))
		questionDialog.SetCancelEvent(ui.__mem_func__(self.__popUpDialogCancel))
		questionDialog.type = player.LUCKY_BOX_ACTION_RECEIVE
		questionDialog.Open()
		self.questionDialog = questionDialog

	def OnRetry(self):
		if self.questionDialog:
			return

		questionDialog = uiCommon.QuestionDialog()
		questionDialog.SetText(localeInfo.LUCKY_BOX_DIALOG_RETRY_TEXT)
		questionDialog.SetAcceptEvent(ui.__mem_func__(self.__popUpDialogAccept))
		questionDialog.SetCancelEvent(ui.__mem_func__(self.__popUpDialogCancel))
		questionDialog.type = player.LUCKY_BOX_ACTION_RETRY
		questionDialog.Open()
		self.questionDialog = questionDialog

	def OverInItem(self):
		if self.tooltipItem:
			self.tooltipItem.ClearToolTip()

			metinSlot = [0 for i in xrange(player.METIN_SOCKET_MAX_NUM)]
			attrSlot = [(0, 0) for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]

			self.tooltipItem.AddItemData(self.itemVNum, metinSlot, attrSlot)

	def OverOutItem(self):
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def SetItemToolTip(self, tooltip):
		self.tooltipItem = tooltip

	if app.WJ_ENABLE_TRADABLE_ICON:
		def BindInterface(self, interface):
			from _weakref import proxy
			self.interface = proxy(interface)

		def SetInven(self, inven):
			self.inven = inven

		def SetTop(self):
			if not self.interface:
				return

			self.interface.SetOnTopWindow(player.ON_TOP_WND_LUCKY_BOX)
			self.interface.RefreshMarkInventoryBag()

	def ShowLuckyBoxWindow(self, dwItemVnum, byItemCount, iNeedMoney, wSlotIndex):
		self.itemVNum = dwItemVnum
		self.slotIndex = wSlotIndex

		self.RewardItemSlot.SetItemSlot(0, dwItemVnum, byItemCount)
		self.NeedMoney.SetText(localeInfo.NumberToMoneyString(iNeedMoney))

		self.__ShowDialog()

	def __popUpDialogAccept(self):
		if self.questionDialog:
			player.SendLuckyBoxAction(self.questionDialog.type)

			if self.questionDialog.type == player.LUCKY_BOX_ACTION_RECEIVE:
				self.Close()

			self.__popUpDialogCancel()

	def __popUpDialogCancel(self):
		if self.questionDialog:
			self.questionDialog.Close()
			self.questionDialog = None

	def __ShowDialog(self):
		ui.ScriptWindow.Show(self)
		self.SetTop()
