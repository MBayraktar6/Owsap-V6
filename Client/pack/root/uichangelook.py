import ui
import mouseModule
import net
import app
import snd
import item
import player
import chat
import grp
import uiScriptLocale
import uiRefine
import uiAttachMetin
import uiPickMoney
import uiCommon
import uiPrivateShopBuilder # 개인상점 열동안 ItemMove 방지
import localeInfo
import constInfo
import ime
import wndMgr
import uiToolTip

class ChangeLookWindow(ui.ScriptWindow):

	CHANGELOOK_SLOT_LEFT = 0
	CHANGELOOK_SLOT_RIGHT = 1
	CHANGELOOK_SLOT_MAX = 2

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.isLoaded = 0
		self.toolTipItem = None
		self.xChangeLookWindowStart = 0
		self.yChangeLookWindowStart = 0
		self.pop = None
		self.ChangeLookToolTipButton = None
		self.ChangeLookToolTip = None

		self.ChangeLookCost = None
		self.ChangeLookToolTipList = [
			localeInfo.CHANGE_TOOLTIP_LINE1,
			localeInfo.CHANGE_TOOLTIP_LINE2,
			localeInfo.CHANGE_TOOLTIP_LINE3,
			localeInfo.CHANGE_TOOLTIP_LINE4,
			localeInfo.CHANGE_TOOLTIP_LINE5,
			localeInfo.CHANGE_TOOLTIP_LINE6
		]
		self.ChangeLookMountToolTipList = [
			localeInfo.CHANGE_TOOLTIP2_LINE1,
			localeInfo.CHANGE_TOOLTIP2_LINE2,
			localeInfo.CHANGE_TOOLTIP2_LINE3,
			localeInfo.CHANGE_TOOLTIP2_LINE4,
			localeInfo.CHANGE_TOOLTIP2_LINE5
		]

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		self.isLoaded = 0
		self.toolTipItem = None
		self.xChangeLookWindowStart = 0
		self.yChangeLookWindowStart = 0
		self.pop = None
		self.ChangeLookToolTipButton = None
		self.ChangeLookToolTip = None

	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/ChangeLookWindow.py")
		except:
			import exception
			exception.Abort("ChangeLookWindow.__LoadWindow.UIScript/ChangeLookWindow.py")

		try:
			wndItem = self.GetChild("ChangeLookSlot")
			wndPassItem = self.GetChild("ChangeLookSlot_PassYangItem")

			self.GetChild("CancelButton").SetEvent(ui.__mem_func__(self.Close))
			self.GetChild("AcceptButton").SetEvent(ui.__mem_func__(self.Accept))
			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))

			self.TitleName = self.GetChild("TitleName")
			self.ChangeLookCost = self.GetChild("Cost")
		except:
			import exception
			exception.Abort("ChangeLookWindow.__LoadWindow.ChangeLookSlot")

		wndItem.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		wndItem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		wndItem.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndItem.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndItem.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
		wndItem.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
		wndItem.Show()
		self.wndItem = wndItem

		wndPassItem.SetOverInItemEvent(ui.__mem_func__(self.OverInItemFreeYang))
		wndPassItem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		wndPassItem.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlotFreepass))
		wndPassItem.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlotFreepass))
		wndPassItem.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlotFreepass))
		wndPassItem.Show()
		self.wndPassItem = wndPassItem

		self.ChangeLookToolTipButton = self.GetChild("ChangeLookToolTIpButton")

	def __CreateGameTypeToolTip(self, title, descList):
		toolTip = uiToolTip.ToolTip()
		toolTip.SetTitle(title)
		toolTip.AppendSpace(7)

		for desc in descList:
			toolTip.AutoAppendTextLine(desc)

		toolTip.AlignHorizonalCenter()
		toolTip.SetTop()
		return toolTip

	def OnUpdate(self):
		(x, y, z) = player.GetMainCharacterPosition()
		if abs(x - self.xChangeLookWindowStart) > player.SHOW_UI_WINDOW_LIMIT_RANGE or abs(y - self.yChangeLookWindowStart) > player.SHOW_UI_WINDOW_LIMIT_RANGE:
			self.Close()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def Open(self, type):
		if self.isLoaded == 0:
			self.isLoaded = 1
			self.__LoadWindow()

		self.SetCenterPosition()
		self.SetTop()
		ui.ScriptWindow.Show(self)

		(self.xChangeLookWindowStart, self.yChangeLookWindowStart, z) = player.GetMainCharacterPosition()
		player.SetChangeLookWindow(True)
		player.SetChangeLookWindowType(int(type))

		self.SetChangeLookType(type)
		self.RefreshChangeLookWindow()

	def SetChangeLookType(self, type):
		self.RefreshGameTypeToolTip(type)
		self.ChangeLookCost.SetText(localeInfo.CHANGE_LOOK_COST % (localeInfo.NumberToMoneyString(player.GetChangeChangeLookPrice())))

	def RefreshGameTypeToolTip(self, type):
		if int(type) == player.CHANGE_LOOK_TYPE_MOUNT:
			self.TitleName.SetText(localeInfo.CHANGE_LOOK_TITLE2)
			self.ChangeLookToolTip = self.__CreateGameTypeToolTip(localeInfo.CHANGE_TOOLTIP_TITLE, self.ChangeLookMountToolTipList)
		else:
			self.TitleName.SetText(localeInfo.CHANGE_LOOK_TITLE)
			self.ChangeLookToolTip = self.__CreateGameTypeToolTip(localeInfo.CHANGE_TOOLTIP_TITLE, self.ChangeLookToolTipList)

		self.ChangeLookToolTip.SetTop()
		self.ChangeLookToolTipButton.SetToolTipWindow(self.ChangeLookToolTip)

	def Close(self):
		self.Hide()
		net.SendChangeLookCancel()
		player.SetChangeLookWindow(False)

	def Accept(self):
		leftVnum = player.GetChangeLookItemID(self.CHANGELOOK_SLOT_LEFT)
		rightVnum = player.GetChangeLookItemID(self.CHANGELOOK_SLOT_RIGHT)
		if leftVnum == 0 or rightVnum == 0:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CHANGE_LOOK_INSERT_ITEM)
		else:
			popup = uiCommon.QuestionDialog()
			popup.SetText(localeInfo.CHANGE_LOOK_CHANGE_ITEM)
			popup.SetAcceptEvent(self.SendAccept)
			popup.SetCancelEvent(self.OnCloseEvent)
			popup.Open()
			self.pop = popup

	def SendAccept(self):
		self.pop.Close()
		self.pop = None
		net.SendChangeLookAccept()

	def SetItemToolTip(self, tooltip):
		self.toolTipItem = tooltip

	def __ShowToolTip(self, slotIndex):
		if self.toolTipItem:
			self.toolTipItem.SetChangeLookWindowItem(slotIndex)

	# 아이템 툴팁 보여주기
	def OverInItem(self, slotIndex):
		self.wndItem.SetUsableItem(False)
		self.__ShowToolTip(slotIndex)

	## 아이템 툴팁 감추기
	def OverOutItem(self):
		self.wndItem.SetUsableItem(False)
		if self.toolTipItem:
			self.toolTipItem.HideToolTip()

	def OverInItemFreeYang(self, slotIndex):
		self.wndItem.SetUsableItem(False)
		self.__ShowToolTip_FreeItem(player.GetChangeLookFreeYangInvenSlotPos())

	def __ShowToolTip_FreeItem(self, slotIndex):
		if self.toolTipItem:
			self.toolTipItem.SetInventoryItem(slotIndex, player.INVENTORY)

	def OverInItemFreeYang(self, slotIndex):
		self.wndItem.SetUsableItem(False)
		self.__ShowToolTip_FreeItem(player.GetChangeLookFreeYangInvenSlotPos())

	def __ShowToolTip_FreeItem(self, slotIndex):
		if self.toolTipItem:
			self.toolTipItem.SetInventoryItem(slotIndex, player.INVENTORY)

	## 외형변경 -> 인벤 (양 패스 아이템)
	def UseItemSlotFreepass(self, slotIndex):
		mouseModule.mouseController.DeattachObject()
		net.SendChangeLookCheckOutFreeYangItem()
		self.ChangeLookCost.SetText(localeInfo.CHANGE_LOOK_COST % (localeInfo.NumberToMoneyString(player.GetChangeChangeLookPrice())))

	## 인벤 -> 외형변경 (양 패스 아이템)
	def SelectEmptySlotFreepass(self, selectedSlotPos):
		if mouseModule.mouseController.isAttached():

			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)
			ItemVNum = player.GetItemIndex(attachedSlotPos)

			## 아이템 체크.
			if item.IsChangeLookFreePassYangItem(ItemVNum) == 0:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CHANGE_LOOK_DO_NOT_REGISTER_ITEM)
				return

			net.SendChangeLookCheckInFreeYangItem(attachedInvenType, attachedSlotPos)
			mouseModule.mouseController.DeattachObject()
			self.ChangeLookCost.SetText(localeInfo.CHANGE_LOOK_COST % (localeInfo.NumberToMoneyString(0)))

	## 인벤 -> 외형변경 창.
	def SelectEmptySlot(self, selectedSlotPos):
		if mouseModule.mouseController.isAttached():

			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			ItemVNum = player.GetItemIndex(attachedSlotPos)

			window = player.SlotTypeToInvenType(attachedSlotType)
			if window == player.EQUIPMENT:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CHANGE_LOOK_DO_NOT_EQUIP_ITEM)
				return

			## 아이템 체크.
			if selectedSlotPos == self.CHANGELOOK_SLOT_LEFT:
				if item.IsPossibleChangeLookLeft(ItemVNum) == 0:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CHANGE_LOOK_DO_NOT_CHANGE_LOOK_ITEM)
					return
			else:
				if player.GetChangeLookItemID(self.CHANGELOOK_SLOT_LEFT) == 0:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CHANGE_LOOK_INSERT_CHANGE_LOOK_ITEM)
					return
				if item.IsPossibleChangeLookRight(player.GetChangeLookItemID(self.CHANGELOOK_SLOT_LEFT), ItemVNum) == 0:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CHANGE_LOOK_DO_NOT_REGISTER_ITEM)
					return
			## 아이템 체크.

			if player.SLOT_TYPE_CHANGE_LOOK != attachedSlotType:
				attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)
				if player.RESERVED_WINDOW == attachedInvenType:
					return

				if selectedSlotPos == self.CHANGELOOK_SLOT_LEFT:
					net.SendChangeLookCheckIn(attachedInvenType, attachedSlotPos, selectedSlotPos)
				else:
					if app.ENABLE_SOULBIND_SYSTEM:
						if player.GetItemSealDate(player.INVENTORY, attachedSlotPos) != item.E_SEAL_DATE_DEFAULT_TIMESTAMP:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CHANGE_LOOK_DO_NOT_SEAL_ITEM)
							return

					if player.GetChangeLookItemInvenSlot(self.CHANGELOOK_SLOT_LEFT) == attachedSlotPos:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CHANGE_ALREADY_REGISTER)
						return

					net.SendChangeLookCheckIn(attachedInvenType, attachedSlotPos, selectedSlotPos)
					popup = uiCommon.PopupDialog()
					popup.SetText(localeInfo.CHANGE_LOOK_DEL_ITEM)
					popup.SetAcceptEvent(self.__OnClosePopupDialog)
					popup.Open()
					self.pop = popup

			mouseModule.mouseController.DeattachObject()

	## 아이템 사용 시 없애기
	def UseItemSlot(self, slotIndex):
		if slotIndex == self.CHANGELOOK_SLOT_LEFT:
			if player.GetChangeLookItemID(self.CHANGELOOK_SLOT_RIGHT) == 0:
				mouseModule.mouseController.DeattachObject()
				net.SendChangeLookCheckOut(slotIndex)
			else:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CHANGE_LOOK_CHECK_OUT_REGISTER_ITEM)
		else:
			mouseModule.mouseController.DeattachObject()
			net.SendChangeLookCheckOut(slotIndex)

	## 아이템 클릭
	def SelectItemSlot(self, selectedSlotPos):
		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			if player.SLOT_TYPE_INVENTORY == attachedSlotType:
				attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
				snd.PlaySound("sound/ui/drop.wav")
			mouseModule.mouseController.DeattachObject()
		else:
			curCursorNum = app.GetCursor()
			if app.SELL == curCursorNum:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SAFEBOX_SELL_DISABLE_SAFEITEM)
			elif app.BUY == curCursorNum:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SHOP_BUY_INFO)
			else:
				if selectedSlotPos == self.CHANGELOOK_SLOT_LEFT:
					if player.GetChangeLookItemID(self.CHANGELOOK_SLOT_RIGHT) == 0:
						selectedItemID = player.GetChangeLookItemID(selectedSlotPos)
						mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_CHANGE_LOOK, selectedSlotPos, selectedItemID)
						snd.PlaySound("sound/ui/pick.wav")
					else:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CHANGE_LOOK_CHECK_OUT_REGISTER_ITEM)
				else:
					selectedItemID = player.GetChangeLookItemID(selectedSlotPos)
					mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_CHANGE_LOOK, selectedSlotPos, selectedItemID)
					snd.PlaySound("sound/ui/pick.wav")

	## 갱신
	def RefreshChangeLookWindow(self):
		if not self.wndItem:
			return

		if not player.GetChangeLookWindowOpen():
			return

		setChangeLookItem = self.wndItem.SetItemSlot
		getChangeLookItem = player.GetChangeLookItemID

		for i in xrange(self.CHANGELOOK_SLOT_MAX):
			ChangeLookSlotVnum = getChangeLookItem(i)
			if not ChangeLookSlotVnum == player.ITEM_SLOT_COUNT:
				setChangeLookItem(i, ChangeLookSlotVnum, 1)
			else:
				setChangeLookItem(i, 0, 1)

			changelookvnum = player.GetChangeWIndowChangeLookVnum(i)
			if not changelookvnum == 0:
				self.wndItem.SetSlotCoverImage(i, "icon/item/ingame_convert_Mark.tga")
			else:
				self.wndItem.EnableSlotCoverImage(i, False)

		self.wndItem.RefreshSlot()

		ChangeLookFreeItemVnum = player.GetChangeLookFreeYangItemID()
		if not ChangeLookFreeItemVnum == 0:
			self.wndPassItem.SetItemSlot(0, ChangeLookFreeItemVnum, 1)
		else:
			self.ChangeLookCost.SetText(localeInfo.CHANGE_LOOK_COST % (localeInfo.NumberToMoneyString(player.GetChangeChangeLookPrice())))
			self.wndPassItem.SetItemSlot(0, 0, 1)

	def OnCloseEvent(self):
		self.pop.Close()
		self.pop = None

	def __OnClosePopupDialog(self):
		self.popup = None
