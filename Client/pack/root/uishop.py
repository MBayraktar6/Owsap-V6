import net
import player
import item
import snd
import shop
import net
import wndMgr
import app
import chat

import ui
import uiCommon
import mouseModule
import localeInfo
import constInfo

"""
Note:
	class ShopDialog 가 개인상점, NPC 상점을 유동적으로 사용 하였는데,
	개인상점 꾸미기 적용 후( #define ENABLE_MYSHOP_DECO ) 부터
	class ShopDialog ( NPC 상점 ) class PrivateShopDialog ( 개인 상점 ) 분리 됨
"""

## NPC 상점 ##
class ShopDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.tooltipItem = 0
		self.xShopStart = 0
		self.yShopStart = 0
		self.questionDialog = None
		self.popup = None
		self.itemBuyQuestionDialog = None
		self.interface = None

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __GetRealIndex(self, i):
		return self.tabIdx * shop.SHOP_SLOT_COUNT + i

	def Refresh(self):
		getItemID = shop.GetItemID
		getItemCount = shop.GetItemCount
		setItemID = self.itemSlotWindow.SetItemSlot

		for i in xrange(shop.SHOP_SLOT_COUNT):
			idx = self.__GetRealIndex(i)
			itemCount = getItemCount(idx)
			if itemCount <= 1:
				itemCount = 0
			setItemID(i, getItemID(idx), itemCount)

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				changelookvnum = shop.GetItemChangeLookVnum(idx)
				if not changelookvnum == 0:
					self.itemSlotWindow.SetSlotCoverImage(i, "icon/item/ingame_convert_Mark.tga")
				else:
					self.itemSlotWindow.EnableSlotCoverImage(i, False)

		wndMgr.RefreshSlot(self.itemSlotWindow.GetWindowHandle())

	def SetItemData(self, pos, itemID, itemCount, itemPrice):
		shop.SetItemData(pos, itemID, itemCount, itemPrice)

	def LoadDialog(self):
		try:
			PythonScriptLoader = ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self, "UIScript/shopdialog.py")
		except:
			import exception
			exception.Abort("ShopDialog.LoadDialog.LoadObject")

		smallTab1 = None
		smallTab2 = None
		smallTab3 = None
		if app.ENABLE_SHOPEX_TAB4:
			xSmallTab1 = None
			xSmallTab2 = None
			xSmallTab3 = None
			xSmallTab4 = None
		middleTab1 = None
		middleTab2 = None

		try:
			GetObject = self.GetChild
			self.itemSlotWindow = GetObject("ItemSlot")
			self.btnBuy = GetObject("BuyButton")
			self.btnSell = GetObject("SellButton")
			self.btnClose = GetObject("CloseButton")
			self.titleBar = GetObject("TitleBar")
			middleTab1 = GetObject("MiddleTab1")
			middleTab2 = GetObject("MiddleTab2")
			smallTab1 = GetObject("SmallTab1")
			smallTab2 = GetObject("SmallTab2")
			smallTab3 = GetObject("SmallTab3")
			if app.ENABLE_SHOPEX_TAB4:
				xSmallTab1 = GetObject("xSmallTab1")
				xSmallTab2 = GetObject("xSmallTab2")
				xSmallTab3 = GetObject("xSmallTab3")
				xSmallTab4 = GetObject("xSmallTab4")

		except:
			import exception
			exception.Abort("ShopDialog.LoadDialog.BindObject")

		self.itemSlotWindow.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
		self.itemSlotWindow.SAFE_SetButtonEvent("LEFT", "EMPTY", self.SelectEmptySlot)
		self.itemSlotWindow.SAFE_SetButtonEvent("LEFT", "EXIST", self.SelectItemSlot)
		self.itemSlotWindow.SAFE_SetButtonEvent("RIGHT", "EXIST", self.UnselectItemSlot)

		self.itemSlotWindow.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		self.itemSlotWindow.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

		self.btnBuy.SetToggleUpEvent(ui.__mem_func__(self.CancelShopping))
		self.btnBuy.SetToggleDownEvent(ui.__mem_func__(self.OnBuy))

		self.btnSell.SetToggleUpEvent(ui.__mem_func__(self.CancelShopping))
		self.btnSell.SetToggleDownEvent(ui.__mem_func__(self.OnSell))

		self.btnClose.SetEvent(ui.__mem_func__(self.AskClosePrivateShop))

		self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))

		if app.ENABLE_SHOPEX_TAB4:
			self.xSmallRadioButtonGroup = ui.RadioButtonGroup.Create([
				[xSmallTab1, ui.__mem_func__(self.OnClickTabButton), None],
				[xSmallTab2, ui.__mem_func__(self.OnClickTabButton), None],
				[xSmallTab3, ui.__mem_func__(self.OnClickTabButton), None],
				[xSmallTab4, ui.__mem_func__(self.OnClickTabButton), None],
			])
		self.smallRadioButtonGroup = ui.RadioButtonGroup.Create([
			[smallTab1, ui.__mem_func__(self.OnClickTabButton), None],
			[smallTab2, ui.__mem_func__(self.OnClickTabButton), None],
			[smallTab3, ui.__mem_func__(self.OnClickTabButton), None]
		])
		self.middleRadioButtonGroup = ui.RadioButtonGroup.Create([
			[middleTab1, ui.__mem_func__(self.OnClickTabButton), None],
			[middleTab2, ui.__mem_func__(self.OnClickTabButton), None]
		])

		self.__HideMiddleTabs()
		self.__HideSmallTabs()
		if app.ENABLE_SHOPEX_TAB4:
			self.__HideXSmallTabs()

		self.tabIdx = 0
		self.coinType = shop.SHOP_COIN_TYPE_GOLD

		self.Refresh()

	def __ShowBuySellButton(self):
		self.btnBuy.Show()
		self.btnSell.Show()

	def __ShowMiddleTabs(self):
		self.middleRadioButtonGroup.Show()

	def __ShowSmallTabs(self):
		self.smallRadioButtonGroup.Show()

	if app.ENABLE_SHOPEX_TAB4:
		def __ShowXSmallTabs(self):
			self.xSmallRadioButtonGroup.Show()

	def __HideBuySellButton(self):
		self.btnBuy.Hide()
		self.btnSell.Hide()

	def __HideMiddleTabs(self):
		self.middleRadioButtonGroup.Hide()

	def __HideSmallTabs(self):
		self.smallRadioButtonGroup.Hide()

	if app.ENABLE_SHOPEX_TAB4:
		def __HideXSmallTabs(self):
			self.xSmallRadioButtonGroup.Hide()

	def __SetTabNames(self):
		if shop.GetTabCount() == 2:
			self.middleRadioButtonGroup.SetText(0, shop.GetTabName(0))
			self.middleRadioButtonGroup.SetText(1, shop.GetTabName(1))
		elif shop.GetTabCount() == 3:
			self.smallRadioButtonGroup.SetText(0, shop.GetTabName(0))
			self.smallRadioButtonGroup.SetText(1, shop.GetTabName(1))
			self.smallRadioButtonGroup.SetText(2, shop.GetTabName(2))
		elif shop.GetTabCount() == 4 and app.ENABLE_SHOPEX_TAB4:
			self.xSmallRadioButtonGroup.SetText(0, shop.GetTabName(0))
			self.xSmallRadioButtonGroup.SetText(1, shop.GetTabName(1))
			self.xSmallRadioButtonGroup.SetText(2, shop.GetTabName(2))
			self.xSmallRadioButtonGroup.SetText(3, shop.GetTabName(3))

	def Destroy(self):
		self.Close()
		self.ClearDictionary()

		self.tooltipItem = 0
		self.itemSlotWindow = 0
		self.btnBuy = 0
		self.btnSell = 0
		self.btnClose = 0
		self.titleBar = 0
		self.questionDialog = None
		self.popup = None

	def Open(self, vid):
		isPrivateShop = False
		isMainPlayerPrivateShop = False

		import chr
		if chr.IsNPC(vid):
			isPrivateShop = False
		else:
			isPrivateShop = True

		if player.IsMainCharacterIndex(vid):
			isMainPlayerPrivateShop = True

			self.btnBuy.Hide()
			self.btnSell.Hide()
			self.btnClose.Show()
		else:
			isMainPlayerPrivateShop = False

			self.btnBuy.Show()
			self.btnSell.Show()
			self.btnClose.Hide()

		shop.Open(isPrivateShop, isMainPlayerPrivateShop)

		self.tabIdx = 0

		if isPrivateShop:
			self.__HideMiddleTabs()
			self.__HideSmallTabs()
		else:
			if shop.GetTabCount() == 1:
				self.__ShowBuySellButton()
				self.__HideMiddleTabs()
				self.__HideSmallTabs()
				if app.ENABLE_SHOPEX_TAB4:
					self.__HideXSmallTabs()
			elif shop.GetTabCount() == 2:
				self.__HideBuySellButton()
				self.__ShowMiddleTabs()
				self.__HideSmallTabs()
				if app.ENABLE_SHOPEX_TAB4:
					self.__HideXSmallTabs()
				self.__SetTabNames()
				if app.ENABLE_SHOPEX_RENEWAL:
					self.middleRadioButtonGroup.OnClick(0)
				else:
					self.middleRadioButtonGroup.OnClick(1)
			elif shop.GetTabCount() == 3:
				self.__HideBuySellButton()
				self.__HideMiddleTabs()
				self.__ShowSmallTabs()
				if app.ENABLE_SHOPEX_TAB4:
					self.__HideXSmallTabs()
				self.__SetTabNames()
				if app.ENABLE_SHOPEX_RENEWAL:
					self.smallRadioButtonGroup.OnClick(0)
				else:
					self.middleRadioButtonGroup.OnClick(1)
			elif shop.GetTabCount() == 4 and app.ENABLE_SHOPEX_TAB4:
				self.__HideBuySellButton()
				self.__HideMiddleTabs()
				self.__HideSmallTabs()
				self.__ShowXSmallTabs()
				self.__SetTabNames()
				self.xSmallRadioButtonGroup.OnClick(0)

		self.Refresh()
		self.SetTop()

		self.Show()

		(self.xShopStart, self.yShopStart, z) = player.GetMainCharacterPosition()

	def Close(self):
		if app.WJ_ENABLE_TRADABLE_ICON:
			if self.interface:
				self.interface.SetOnTopWindow(player.ON_TOP_WND_NONE)
				self.interface.RefreshMarkInventoryBag()

		if self.itemBuyQuestionDialog:
			self.itemBuyQuestionDialog.Close()
			self.itemBuyQuestionDialog = None
			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

		if self.questionDialog:
			self.OnCloseQuestionDialog()

		shop.Close()
		net.SendShopEndPacket()
		self.CancelShopping()
		self.tooltipItem.HideToolTip()
		self.Hide()

	def GetIndexFromSlotPos(self, slotPos):
		return self.tabIdx * shop.SHOP_SLOT_COUNT + slotPos

	def OnClickTabButton(self, idx):
		self.tabIdx = idx
		self.Refresh()

	def AskClosePrivateShop(self):
		questionDialog = uiCommon.QuestionDialog()
		questionDialog.SetText(localeInfo.PRIVATE_SHOP_CLOSE_QUESTION)
		questionDialog.SetAcceptEvent(ui.__mem_func__(self.OnClosePrivateShop))
		questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
		questionDialog.Open()
		self.questionDialog = questionDialog

		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

		return True

	def OnClosePrivateShop(self):
		net.SendCommandPacket("/close_shop")
		self.OnCloseQuestionDialog()
		return True

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def OnPressExitKey(self):
		self.Close()
		return True

	def OnBuy(self):
		chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SHOP_BUY_INFO)
		app.SetCursor(app.BUY)
		self.btnSell.SetUp()

	def OnSell(self):
		chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SHOP_SELL_INFO)
		app.SetCursor(app.SELL)
		self.btnBuy.SetUp()

	def CancelShopping(self):
		self.btnBuy.SetUp()
		self.btnSell.SetUp()
		app.SetCursor(app.NORMAL)

	def __OnClosePopupDialog(self):
		self.pop = None
		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	## 용혼석 팔리는 기능 추가.
	def SellAttachedItem(self):
		if shop.IsPrivateShop():
			mouseModule.mouseController.DeattachObject()
			return

		attachedSlotType = mouseModule.mouseController.GetAttachedType()
		attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
		attachedCount = mouseModule.mouseController.GetAttachedItemCount()
		if localeInfo.IsBRAZIL() == 0:
			attachedItemIndex = mouseModule.mouseController.GetAttachedItemIndex()

		if player.SLOT_TYPE_INVENTORY == attachedSlotType or \
			player.SLOT_TYPE_DRAGON_SOUL_INVENTORY == attachedSlotType or \
			player.SLOT_TYPE_EQUIPMENT == attachedSlotType or \
			player.SLOT_TYPE_BELT_INVENTORY == attachedSlotType:

			attachedSlotWindow = player.SlotTypeToInvenType(attachedSlotType)

			if localeInfo.IsBRAZIL():
				itemIndex = player.GetItemIndex(attachedSlotWindow, attachedSlotPos)
				item.SelectItem(itemIndex)
			else:
				item.SelectItem(attachedItemIndex)

			if item.IsAntiFlag(item.ANTIFLAG_SELL):
				popup = uiCommon.PopupDialog()
				popup.SetText(localeInfo.SHOP_CANNOT_SELL_ITEM)
				popup.SetAcceptEvent(self.__OnClosePopupDialog)
				popup.Open()
				self.popup = popup
				return

			if player.IsValuableItem(attachedSlotWindow, attachedSlotPos):
				itemPrice = item.GetISellItemPrice()

				if item.Is1GoldItem():
					itemPrice = attachedCount / itemPrice / 5
				else:
					itemPrice = itemPrice * max(1, attachedCount) / 5

				itemName = item.GetItemName()
				if app.ENABLE_SET_ITEM:
					itemName = item.GetItemNameBySetValue(player.GetItemSetValue(attachedSlotWindow, attachedSlotPos))

				questionDialog = uiCommon.QuestionDialog()
				questionDialog.SetText(localeInfo.DO_YOU_SELL_ITEM(itemName, attachedCount, itemPrice))

				questionDialog.SetAcceptEvent(lambda arg1=attachedSlotPos, arg2=attachedCount, arg3 = attachedSlotWindow: self.OnSellItem(arg1, arg2, arg3))
				questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
				questionDialog.Open()
				self.questionDialog = questionDialog

				constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)
			else:
				self.OnSellItem(attachedSlotPos, attachedCount, attachedSlotWindow)
		else:
			snd.PlaySound("sound/ui/loginfail.wav")

		mouseModule.mouseController.DeattachObject()

	def OnSellItem(self, slotPos, count, itemtype):
		net.SendShopSellPacketNew(slotPos, count, itemtype)
		snd.PlaySound("sound/ui/money.wav")
		self.OnCloseQuestionDialog()

	def OnCloseQuestionDialog(self):
		if not self.questionDialog:
			return

		self.questionDialog.Close()
		self.questionDialog = None
		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	def SelectEmptySlot(self, selectedSlotPos):
		isAttached = mouseModule.mouseController.isAttached()
		if isAttached:
			self.SellAttachedItem()

	def UnselectItemSlot(self, selectedSlotPos):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return

		if shop.IsPrivateShop():
			self.AskBuyItem(selectedSlotPos)
		else:
			net.SendShopBuyPacket(self.__GetRealIndex(selectedSlotPos))

	def SelectItemSlot(self, selectedSlotPos):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return

		isAttached = mouseModule.mouseController.isAttached()
		selectedSlotPos = self.__GetRealIndex(selectedSlotPos)

		if isAttached:
			self.SellAttachedItem()
		else:
			if True == shop.IsMainPlayerPrivateShop():
				return

			curCursorNum = app.GetCursor()
			if app.BUY == curCursorNum:
				self.AskBuyItem(selectedSlotPos)
			elif app.SELL == curCursorNum:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SHOP_SELL_INFO)
			else:
				selectedItemID = shop.GetItemID(selectedSlotPos)
				itemCount = shop.GetItemCount(selectedSlotPos)

				type = player.SLOT_TYPE_SHOP
				if shop.IsPrivateShop():
					type = player.SLOT_TYPE_PRIVATE_SHOP

				mouseModule.mouseController.AttachObject(self, type, selectedSlotPos, selectedItemID, itemCount)
				mouseModule.mouseController.SetCallBack("INVENTORY", ui.__mem_func__(self.DropToInventory))
				snd.PlaySound("sound/ui/pick.wav")

	def DropToInventory(self):
		attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
		self.AskBuyItem(attachedSlotPos)

	def AskBuyItem(self, slotPos):
		slotPos = self.__GetRealIndex(slotPos)

		itemIndex = shop.GetItemID(slotPos)
		itemPrice = shop.GetItemPrice(slotPos)
		itemCount = shop.GetItemCount(slotPos)

		item.SelectItem(itemIndex)
		itemName = item.GetItemName()
		if app.ENABLE_SET_ITEM:
			itemName = item.GetItemNameBySetValue(shop.GetItemSetValue(slotPos))

		itemBuyQuestionDialog = uiCommon.QuestionDialog()
		if app.ENABLE_CHEQUE_SYSTEM:
			itemBuyQuestionDialog.SetText(localeInfo.DO_YOU_BUY_ITEM(itemName, itemCount, localeInfo.NumberToMoneyString(itemPrice), shop.GetItemCheque(slotPos)))
		else:
			itemBuyQuestionDialog.SetText(localeInfo.DO_YOU_BUY_ITEM(itemName, itemCount, localeInfo.NumberToMoneyString(itemPrice)))

		itemBuyQuestionDialog.SetAcceptEvent(lambda arg=True: self.AnswerBuyItem(arg))
		itemBuyQuestionDialog.SetCancelEvent(lambda arg=False: self.AnswerBuyItem(arg))
		itemBuyQuestionDialog.Open()
		itemBuyQuestionDialog.pos = slotPos
		self.itemBuyQuestionDialog = itemBuyQuestionDialog

		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

	def AnswerBuyItem(self, flag):
		if flag:
			pos = self.itemBuyQuestionDialog.pos
			net.SendShopBuyPacket(pos)

		self.itemBuyQuestionDialog.Close()
		self.itemBuyQuestionDialog = None

		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem

	def OverInItem(self, slotIndex):
		slotIndex = self.__GetRealIndex(slotIndex)
		if mouseModule.mouseController.isAttached():
			return

		if 0 != self.tooltipItem:
			if app.ENABLE_SHOPEX_RENEWAL:
				PriceType = shop.GetItemPriceType(slotIndex)
				if PriceType == shop.SHOP_COIN_TYPE_GOLD:
					self.tooltipItem.SetShopItem(slotIndex)
				elif PriceType == shop.SHOP_COIN_TYPE_SECONDARY_COIN:
					self.tooltipItem.SetShopItemBySecondaryCoin(slotIndex)
				elif PriceType == shop.SHOP_COIN_TYPE_ITEM:
					self.tooltipItem.SetShopItemByShopItem(slotIndex)
				else:
					self.tooltipItem.SetShopItem(slotIndex)
			else:
				if shop.SHOP_COIN_TYPE_GOLD == shop.GetTabCoinType(self.tabIdx):
					self.tooltipItem.SetShopItem(slotIndex)
				else:
					self.tooltipItem.SetShopItemBySecondaryCoin(slotIndex)

	def OverOutItem(self):
		if 0 != self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def OnUpdate(self):
		USE_SHOP_LIMIT_RANGE = 1000

		(x, y, z) = player.GetMainCharacterPosition()
		if abs(x - self.xShopStart) > USE_SHOP_LIMIT_RANGE or abs(y - self.yShopStart) > USE_SHOP_LIMIT_RANGE:
			self.Close()

	if app.WJ_ENABLE_TRADABLE_ICON:
		def CantSellInvenItem(self, slotIndex):
			itemIndex = player.GetItemIndex(slotIndex)

			if itemIndex:
				if app.ENABLE_SOULBIND_SYSTEM:
					if player.GetItemSealDate(player.INVENTORY, slotIndex) != item.E_SEAL_DATE_DEFAULT_TIMESTAMP:
						return True

				return player.IsAntiFlagBySlot(slotIndex, item.ANTIFLAG_SELL)
			return False

		def BindInterface(self, interface):
			from _weakref import proxy
			self.interface = proxy(interface)

		def OnTop(self):
			if not self.interface:
				return

			if shop.IsPrivateShop():
				self.interface.SetOnTopWindow(player.ON_TOP_WND_NONE)
				self.interface.RefreshMarkInventoryBag()
			else:
				self.interface.SetOnTopWindow(player.ON_TOP_WND_SHOP)
				self.interface.RefreshMarkInventoryBag()

class MallPageDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Destroy(self):
		self.ClearDictionary()

	def Open(self):
		scriptLoader = ui.PythonScriptLoader()
		scriptLoader.LoadScriptFile(self, "uiscript/mallpagedialog.py")

		self.GetChild("titlebar").SetCloseEvent(ui.__mem_func__(self.Close))

		(x, y) = self.GetGlobalPosition()
		x += 10
		y += 30

		MALL_PAGE_WIDTH = 600
		MALL_PAGE_HEIGHT = 480

		app.ShowWebPage(app.BROWSER_TYPE_ITEM_MALL,
			"http://metin2.co.kr/08_mall/game_mall/login_fail.htm", 
			(x, y, x + MALL_PAGE_WIDTH, y + MALL_PAGE_HEIGHT))

		self.Lock()
		self.Show()

	def Close(self):
		app.HideWebPage(app.BROWSER_TYPE_ITEM_MALL)
		self.Unlock()
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True
