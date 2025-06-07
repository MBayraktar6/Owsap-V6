import player
import exchange
import net
import localeInfo
import chat
import item

import ui
import mouseModule
import uiPickMoney
import wndMgr
import app

###################################################################################################
## Exchange
if app.WJ_ENABLE_TRADABLE_ICON:
	INVENTORY_PAGE_SIZE = player.INVENTORY_PAGE_SIZE

class ExchangeDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.TitleName = 0
		self.tooltipItem = 0
		self.xStart = 0
		self.yStart = 0
		self.interface = None
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.inven = None

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.inven = None

	def LoadDialog(self):
		PythonScriptLoader = ui.PythonScriptLoader()
		PythonScriptLoader.LoadScriptFile(self, "UIScript/exchangedialog.py")

		## Owner
		self.OwnerSlot = self.GetChild("Owner_Slot")
		self.OwnerSlot.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectOwnerEmptySlot))
		self.OwnerSlot.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectOwnerItemSlot))
		self.OwnerSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInOwnerItem))
		self.OwnerSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		self.OwnerMoney = self.GetChild("Owner_Money_Value")
		self.OwnerAcceptLight = self.GetChild("Owner_Accept_Light")
		self.OwnerAcceptLight.Disable()
		self.OwnerMoneyButton = self.GetChild("Owner_Money")
		if not app.ENABLE_CHEQUE_SYSTEM:
			self.OwnerMoneyButton.SetEvent(ui.__mem_func__(self.OpenPickMoneyDialog))

		## Target
		self.TargetSlot = self.GetChild("Target_Slot")
		self.TargetSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInTargetItem))
		self.TargetSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		self.TargetMoney = self.GetChild("Target_Money_Value")
		self.TargetAcceptLight = self.GetChild("Target_Accept_Light")
		self.TargetAcceptLight.Disable()

		if app.ENABLE_CHEQUE_SYSTEM:
			self.OwnerCheque = self.GetChild("Owner_Cheque_Value")
			self.OwnerChequeButton = self.GetChild("Owner_Cheque")
			self.OwnerMoneyButton.SetEvent(ui.__mem_func__(self.OpenPickMoneyDialog), 0)
			self.OwnerChequeButton.SetEvent(ui.__mem_func__(self.OpenPickMoneyDialog), 1)
			self.TargetCheque = self.GetChild("Target_Cheque_Value")

		## PickMoneyDialog
		dlgPickMoney = uiPickMoney.PickMoneyDialog()
		dlgPickMoney.LoadDialog()
		dlgPickMoney.SetAcceptEvent(ui.__mem_func__(self.OnPickMoney))
		dlgPickMoney.SetTitleName(localeInfo.EXCHANGE_MONEY)
		dlgPickMoney.SetMax(app.YANG_PICKUP_INPUT_DIGIT_LIMIT)
		dlgPickMoney.Hide()
		self.dlgPickMoney = dlgPickMoney

		## Button
		self.AcceptButton = self.GetChild("Owner_Accept_Button")
		self.AcceptButton.SetToggleDownEvent(ui.__mem_func__(self.AcceptExchange))

		self.TitleName = self.GetChild("TitleName")
		self.GetChild("TitleBar").SetCloseEvent(net.SendExchangeExitPacket)

	def Destroy(self):
		#print "---------------------------------------------------------------------------- DESTROY EXCHANGE"
		self.ClearDictionary()
		self.dlgPickMoney.Destroy()
		self.dlgPickMoney = 0
		self.OwnerSlot = 0
		self.OwnerMoney = 0
		self.OwnerAcceptLight = 0
		self.OwnerMoneyButton = 0
		self.TargetSlot = 0
		self.TargetMoney = 0
		self.TargetAcceptLight = 0
		self.TitleName = 0
		self.AcceptButton = 0
		self.tooltipItem = 0

		if app.ENABLE_CHEQUE_SYSTEM:
			self.OwnerCheque = 0
			self.OwnerChequeButton = 0
			self.TargetCheque = 0

	def OpenDialog(self):
		if localeInfo.IsEUROPE() or localeInfo.IsYMIR():
			self.TitleName.SetText(localeInfo.EXCHANGE_TITLE % (exchange.GetNameFromTarget(), exchange.GetLevelFromTarget()))
		else:
			self.TitleName.SetText(localeInfo.EXCHANGE_TITLE % (exchange.GetNameFromTarget()))

		self.AcceptButton.Enable()
		self.AcceptButton.SetUp()
		self.SetTop()
		self.Show()

		(self.xStart, self.yStart, z) = player.GetMainCharacterPosition()

		if app.WJ_ENABLE_TRADABLE_ICON:
			self.ItemListIdx = []

	def CloseDialog(self):
		wndMgr.OnceIgnoreMouseLeftButtonUpEvent()

		if 0 != self.tooltipItem:
			self.tooltipItem.HideToolTip()

		self.dlgPickMoney.Close()
		self.Hide()
		if app.WJ_ENABLE_TRADABLE_ICON:
			if self.interface:
				self.interface.SetOnTopWindow(player.ON_TOP_WND_NONE)
				self.interface.RefreshMarkInventoryBag()

			self.ItemListIdx = None

	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem

	if app.ENABLE_CHEQUE_SYSTEM:
		def OpenPickMoneyDialog(self, focus_idx):
			if exchange.GetElkFromSelf() > 0 or exchange.GetChequeFromSelf() > 0:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.EXCHANGE_CANT_EDIT_MONEY)
				return

			self.dlgPickMoney.SetChequeDigitMax(app.EXCHANGE_CHEQUE_DIGIT_LIMIT)
			self.dlgPickMoney.Open(player.GetElk(), player.GetCheque())
			self.dlgPickMoney.SetFocus(focus_idx)

		def OnPickMoney(self, money, cheque):
			net.SendExchangeElkAddPacket(money)
			net.SendExchangeChequeAddPacket(cheque)
	else:
		def OpenPickMoneyDialog(self):
			if exchange.GetElkFromSelf() > 0:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.EXCHANGE_CANT_EDIT_MONEY)
				return

			self.dlgPickMoney.Open(player.GetElk())

		def OnPickMoney(self, money):
			net.SendExchangeElkAddPacket(money)

	def AcceptExchange(self):
		net.SendExchangeAcceptPacket()
		self.AcceptButton.Disable()

	def SelectOwnerEmptySlot(self, SlotIndex):
		if False == mouseModule.mouseController.isAttached():
			return

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			if player.GetAcceRefineWindowOpen() == 1:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ACCE_NOT_WINDOWOPEN)
				return

		if app.ENABLE_CHANGE_LOOK_SYSTEM:
			if player.GetChangeLookWindowOpen() == 1:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CHANGE_LOOK_OPEN_OTHER_WINDOW)
				return

		if mouseModule.mouseController.IsAttachedMoney():
			if app.ENABLE_CHEQUE_SYSTEM:
				net.SendExchangeElkAddPacket(mouseModule.mouseController.GetAttachedMoneyAmount(), mouseModule.mouseController.GetCheque())
			else:
				net.SendExchangeElkAddPacket(mouseModule.mouseController.GetAttachedMoneyAmount())
		else:
			attachedSlotType = mouseModule.mouseController.GetAttachedType()

			if (player.SLOT_TYPE_INVENTORY == attachedSlotType
				or player.SLOT_TYPE_DRAGON_SOUL_INVENTORY == attachedSlotType
				or player.SLOT_TYPE_SKILL_BOOK_INVENTORY == attachedSlotType
				or player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY == attachedSlotType
				or player.SLOT_TYPE_STONE_INVENTORY == attachedSlotType
				or player.SLOT_TYPE_GIFT_BOX_INVENTORY == attachedSlotType):

				attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)
				SrcSlotNumber = mouseModule.mouseController.GetAttachedSlotNumber()
				DstSlotNumber = SlotIndex

				itemID = player.GetItemIndex(attachedInvenType, SrcSlotNumber)
				item.SelectItem(itemID)

				#if player.IsAntiFlagBySlot(SrcSlotNumber, item.ANTIFLAG_GIVE):
				if item.IsAntiFlag(item.ANTIFLAG_GIVE):
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.EXCHANGE_CANNOT_GIVE)
					mouseModule.mouseController.DeattachObject()
					return

				net.SendExchangeItemAddPacket(attachedInvenType, SrcSlotNumber, DstSlotNumber)
				if app.WJ_ENABLE_TRADABLE_ICON:
					self.ItemListIdx.append(SrcSlotNumber)

		mouseModule.mouseController.DeattachObject()

	def SelectOwnerItemSlot(self, SlotIndex):
		if player.ITEM_MONEY == mouseModule.mouseController.GetAttachedItemIndex():
			money = mouseModule.mouseController.GetAttachedItemCount()
			net.SendExchangeElkAddPacket(money)

		if app.ENABLE_CHEQUE_SYSTEM:
			if player.ITEM_CHEQUE == mouseModule.mouseController.GetAttachedItemIndex():
				cheque = mouseModule.mouseController.GetAttachedItemCount()
				net.SendExchangeChequeAddPacket(cheque)

	def RefreshOwnerSlot(self):
		for i in xrange(exchange.EXCHANGE_ITEM_MAX_NUM):
			itemIndex = exchange.GetItemVnumFromSelf(i)
			itemCount = exchange.GetItemCountFromSelf(i)
			if 1 == itemCount:
				itemCount = 0
			self.OwnerSlot.SetItemSlot(i, itemIndex, itemCount)

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				changelookvnum = exchange.GetChangeLookVnumFromSelf(i)
				if not changelookvnum == 0:
					self.OwnerSlot.SetSlotCoverImage(i, "icon/item/ingame_convert_Mark.tga")
				else:
					self.OwnerSlot.EnableSlotCoverImage(i, False)

		self.OwnerSlot.RefreshSlot()

	def RefreshTargetSlot(self):
		for i in xrange(exchange.EXCHANGE_ITEM_MAX_NUM):
			itemIndex = exchange.GetItemVnumFromTarget(i)
			itemCount = exchange.GetItemCountFromTarget(i)
			if 1 == itemCount:
				itemCount = 0
			self.TargetSlot.SetItemSlot(i, itemIndex, itemCount)

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				changelookvnum = exchange.GetChangeLookVnumFromTarget(i)
				if not changelookvnum == 0:
					self.TargetSlot.SetSlotCoverImage(i, "icon/item/ingame_convert_Mark.tga")
				else:
					self.TargetSlot.EnableSlotCoverImage(i, False)

		self.TargetSlot.RefreshSlot()

	def Refresh(self):
		self.RefreshOwnerSlot()
		self.RefreshTargetSlot()

		if app.ENABLE_CHEQUE_SYSTEM:
			self.OwnerCheque.SetText(str(exchange.GetChequeFromSelf()))
			self.TargetCheque.SetText(str(exchange.GetChequeFromTarget()))
			self.OwnerMoney.SetText(localeInfo.NumberToMoneyString(exchange.GetElkFromSelf()))
			self.TargetMoney.SetText(localeInfo.NumberToMoneyString(exchange.GetElkFromTarget()))
		else:
			self.OwnerMoney.SetText(str(exchange.GetElkFromSelf()))
			self.TargetMoney.SetText(str(exchange.GetElkFromTarget()))

		if True == exchange.GetAcceptFromSelf():
			self.OwnerAcceptLight.Down()
		else:
			self.AcceptButton.Enable()
			self.AcceptButton.SetUp()
			self.OwnerAcceptLight.SetUp()

		if True == exchange.GetAcceptFromTarget():
			self.TargetAcceptLight.Down()
		else:
			self.TargetAcceptLight.SetUp()

	def OverInOwnerItem(self, slotIndex):
		if 0 != self.tooltipItem:
			self.tooltipItem.SetExchangeOwnerItem(slotIndex)

	def OverInTargetItem(self, slotIndex):
		if 0 != self.tooltipItem:
			self.tooltipItem.SetExchangeTargetItem(slotIndex)

	def OverOutItem(self):
		if 0 != self.tooltipItem:
			self.tooltipItem.HideToolTip()

	if not app.WJ_ENABLE_TRADABLE_ICON:
		def OnTop(self):
			self.tooltipItem.SetTop()

	def OnUpdate(self):
		USE_EXCHANGE_LIMIT_RANGE = 1000

		(x, y, z) = player.GetMainCharacterPosition()
		if abs(x - self.xStart) > USE_EXCHANGE_LIMIT_RANGE or abs(y - self.yStart) > USE_EXCHANGE_LIMIT_RANGE:
			(self.xStart, self.yStart, z) = player.GetMainCharacterPosition()
			net.SendExchangeExitPacket()

		if app.WJ_ENABLE_TRADABLE_ICON:
			if not self.inven:
				return

			page = self.inven.GetInventoryPageIndex() # range 0 ~ 1
			for i in self.ItemListIdx:
				if (page * INVENTORY_PAGE_SIZE) <= i < ((page + 1) * INVENTORY_PAGE_SIZE): # range 0 ~ 44, 45 ~ 89
					lock_idx = i - (page * INVENTORY_PAGE_SIZE)
					self.inven.wndItem.SetCantMouseEventSlot(lock_idx)

	if app.WJ_ENABLE_TRADABLE_ICON:
		def CantTradableItem(self, slotIndex):
			itemIndex = player.GetItemIndex(slotIndex)

			if itemIndex:
				if app.ENABLE_SOULBIND_SYSTEM:
					if player.GetItemSealDate(player.INVENTORY, slotIndex) != item.E_SEAL_DATE_DEFAULT_TIMESTAMP:
						return True

				return player.IsAntiFlagBySlot(slotIndex, item.ANTIFLAG_GIVE)

			return False

		def BindInterface(self, interface):
			from _weakref import proxy
			self.interface = proxy(interface)

		def OnTop(self):
			self.tooltipItem.SetTop()
			if not self.interface:
				return

			self.interface.SetOnTopWindow(player.ON_TOP_WND_EXCHANGE)
			self.interface.RefreshMarkInventoryBag()

		def SetInven(self, inven):
			self.inven = inven

		def AddExchangeItemSlotIndex(self, idx):
			self.ItemListIdx.append(idx)
