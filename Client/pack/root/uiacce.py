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

class AcceWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.isLoaded = 0
		self.type = 0
		self.tooltipItem = None
		self.xAcceWindowStart = 0
		self.yAcceWindowStart = 0

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		self.tooltipItem = None

	def __LoadWindow(self, type):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			if type == player.ACCE_SLOT_TYPE_COMBINE:
				pyScrLoader.LoadScriptFile(self, "UIScript/Acce_CombineWindow.py")
				self.cost = self.GetChild("Cost")
			else:
				pyScrLoader.LoadScriptFile(self, "UIScript/Acce_AbsorbWindow.py")
		except:
			import exception
			exception.Abort("AcceWindow.__LoadWindow.UIScript/Acce_CombineWindow.py")

		try:
			wndItem = self.GetChild("AcceSlot")
			self.GetChild("CancelButton").SetEvent(ui.__mem_func__(self.Close))
			self.GetChild("AcceptButton").SetEvent(ui.__mem_func__(self.Accept))
			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
		except:
			import exception
			exception.Abort("AcceWindow.__LoadWindow.AcceSlot")

		wndItem.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		wndItem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		wndItem.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndItem.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndItem.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
		wndItem.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
		wndItem.Show()

		self.wndItem = wndItem

	def Accept(self):
		if player.GetAcceCurrentItemCount() != player.ACCE_SLOT_MAX:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ACCE_INITEM)
			return

		net.SendAcceRefineAccept(self.type)

	def Open(self, type):
		if self.isLoaded == 0:
			self.isLoaded = 1
			self.__LoadWindow(type)

		self.SetCenterPosition()
		self.SetTop()
		ui.ScriptWindow.Show(self)

		(self.xAcceWindowStart, self.yAcceWindowStart, z) = player.GetMainCharacterPosition()
		self.type = type

	def Close(self):
		net.SendAcceRefineCancel()

		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

		ui.ScriptWindow.Hide(self)
		self.isLoaded = 0

	def SetItemToolTip(self, tooltip):
		self.tooltipItem = tooltip

	def __ShowToolTip(self, slotIndex):
		if self.tooltipItem:
			self.tooltipItem.SetAcceWindowItem(slotIndex)

	# 아이템 툴팁 보여주기
	def OverInItem(self, slotIndex):
		self.wndItem.SetUsableItem(False)
		self.__ShowToolTip(slotIndex)

	# 아이템 툴팁 감추기
	def OverOutItem(self):
		self.wndItem.SetUsableItem(False)
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

	# 인벤 -> 악사세리 창.
	def SelectEmptySlot(self, selectedSlotPos):
		if selectedSlotPos == (player.ACCE_SLOT_MAX - 1):
			return

		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			ItemVNum = player.GetItemIndex(attachedSlotPos)
			item.SelectItem(ItemVNum)

			window = player.SlotTypeToInvenType(attachedSlotType)
			if window == player.EQUIPMENT:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ACCE_NOT_USINGITEM)
				return

			if player.SLOT_TYPE_ACCE != attachedSlotType:
				attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)
				if player.RESERVED_WINDOW == attachedInvenType:
					return

				possibleCheckIn = 0
				# 조합창일때
				if player.GetAcceRefineWindowType() == player.ACCE_SLOT_TYPE_COMBINE:
					if item.GetItemType() == item.ITEM_TYPE_COSTUME:
						if item.GetItemSubType() == item.COSTUME_TYPE_ACCE:
							if app.ENABLE_ACCE_SECOND_COSTUME_SYSTEM:
								socketInDrainValue = player.GetItemMetinSocket(attachedInvenType, attachedSlotPos, 1)
								if socketInDrainValue >= player.ACCE_MAX_DRAINRATE:
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ACCE_MAX_DRAINRATE)
									return

								usingSlot = player.FindActivatedAcceSlot(attachedInvenType, attachedSlotPos)
								if player.FindUsingAcceSlot(usingSlot) == (attachedInvenType, attachedSlotPos):
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ACCE_ALREADY_REGISTER)
									return

								possibleCheckIn = 1
							else:
								if item.GetRefinedVnum() == 0: ## 전선등급 아이템은 걸러냄.
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ACCE_MAXGRADE)
									return
								else:
									usingSlot = player.FindActivatedAcceSlot(attachedInvenType, attachedSlotPos)
									if player.FindUsingAcceSlot(usingSlot) == (attachedInvenType, attachedSlotPos):
										chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ACCE_ALREADY_REGISTER)
										return

									possibleCheckIn = 1
						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ACCE_POSSIBLE_ACCE)
							return
					else:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ACCE_POSSIBLE_ACCE)
						return

				# 흡수창일때 악세서리, 아이템 구분
				if player.GetAcceRefineWindowType() == player.ACCE_SLOT_TYPE_ABSORB:
					if selectedSlotPos == player.ACCE_SLOT_LEFT:
						if item.GetItemType() == item.ITEM_TYPE_COSTUME:
							if item.GetItemSubType() == item.COSTUME_TYPE_ACCE:
								if player.GetItemMetinSocket(attachedSlotPos,0) == 0:
									possibleCheckIn = 1
							else:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ACCE_POSSIBLE_ACCE)
								return
						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ACCE_POSSIBLE_ACCE)
							return

					elif selectedSlotPos == player.ACCE_SLOT_RIGHT:
						if item.GetItemType() == item.ITEM_TYPE_WEAPON:
							possibleCheckIn = 1
						elif item.GetItemType() == item.ITEM_TYPE_ARMOR:
							if item.GetItemSubType() == item.ARMOR_BODY:
								possibleCheckIn = 1
							else:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ACCE_POSSIBLE_ACCEITEM)
								return
						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ACCE_POSSIBLE_ACCEITEM)
							return

						if localeInfo.IsBRAZIL():
							## 브라질에서 아래 나열된 아이템은 능력치가 상당히 강해서 흡수 안되게 해달라고 조름.
							## 어쩔수 없이 하드코딩함.
							itemvnum = item.GetVnum()
							if itemvnum == 11979 or itemvnum == 11980 or itemvnum == 11981 or itemvnum == 11982 or itemvnum == 11971 or itemvnum == 11972 or itemvnum == 11973 or itemvnum == 11974:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ACCE_DONOT_ABSORDITEM)
								return

						if app.ENABLE_ACCE_SECOND_COSTUME_SYSTEM:
							itemvnum = item.GetVnum()
							if item.IsWeddingItem(itemvnum) == 1:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ACCE_POSSIBLE_ACCEITEM)
								return

				if possibleCheckIn:
					## 봉인아이템 걸러냄
					if app.ENABLE_SOULBIND_SYSTEM:
						if player.GetItemSealDate(player.INVENTORY, attachedSlotPos) != item.E_SEAL_DATE_DEFAULT_TIMESTAMP:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ACCE_NOT_SEALITEM)
							return

					if player.GetAcceRefineWindowType() == player.ACCE_SLOT_TYPE_COMBINE:
						player.SetAcceActivatedItemSlot(selectedSlotPos, attachedSlotPos)
						net.SendAcceRefineCheckIn(attachedInvenType, attachedSlotPos, selectedSlotPos, self.type)

					elif player.GetAcceRefineWindowType() == player.ACCE_SLOT_TYPE_ABSORB:

						if selectedSlotPos == player.ACCE_SLOT_RIGHT:
							popup = uiCommon.QuestionDialog()
							popup.SetText(localeInfo.ACCE_DEL_ABSORDITEM)
							popup.SetAcceptEvent(lambda arg1 = attachedInvenType, arg2 = attachedSlotPos, arg3 = selectedSlotPos : self.OnAcceAcceptEvent(arg1, arg2, arg3))
							popup.SetCancelEvent(self.OnAcceCloseEvent)
							popup.Open()
							self.pop = popup
						else:
							player.SetAcceActivatedItemSlot(selectedSlotPos, attachedSlotPos)
							net.SendAcceRefineCheckIn(attachedInvenType, attachedSlotPos, selectedSlotPos, self.type)

					snd.PlaySound("sound/ui/drop.wav")

			## 경고 메시지 띄우기.
			if not player.FindUsingAcceSlot(player.ACCE_SLOT_RIGHT) == player.NPOS() and not player.FindUsingAcceSlot(player.ACCE_SLOT_LEFT) == player.NPOS():
				if selectedSlotPos != player.ACCE_SLOT_MAX:
					popup = uiCommon.PopupDialog()
					if player.GetAcceRefineWindowType() == player.ACCE_SLOT_TYPE_COMBINE:
						if app.ENABLE_ACCE_SECOND_COSTUME_SYSTEM:
							socketInDrainValue = player.GetAcceItemMetinSocket(0, 1)
							socketInDrainValue2 = player.GetAcceItemMetinSocket(1, 1)
							socketInDrainValue3 = player.GetItemMetinSocket(attachedSlotPos, 1)
							## 메인 서버 중. 등록된 아이템이 전설일때 경고 메시지 변경.
							if socketInDrainValue > 0 or socketInDrainValue2 > 0 or socketInDrainValue3 > 0:
								popup.SetText(localeInfo.ACCE_DEL_SERVEITEM2)
							else:
								popup.SetText(localeInfo.ACCE_DEL_SERVEITEM)
						else:
							popup.SetText(localeInfo.ACCE_DEL_SERVEITEM)

						popup.SetAcceptEvent(self.__OnClosePopupDialog)
						popup.Open()
						self.popup = popup
				pass

			mouseModule.mouseController.DeattachObject()

	## 아이템 흡수시 흡수될 아이템 할지 안할지 선택 팝업
	def OnAcceAcceptEvent(self, attachedInvenType, attachedSlotPos, selectedSlotPos):
		self.pop.Close()
		self.pop = None

		player.SetAcceActivatedItemSlot(selectedSlotPos, attachedSlotPos)
		net.SendAcceRefineCheckIn(attachedInvenType, attachedSlotPos, selectedSlotPos, self.type)

	def OnAcceCloseEvent(self):
		self.pop.Close()
		self.pop = None

	def UseItemSlot(self, slotIndex):
		if slotIndex == (player.ACCE_SLOT_MAX - 1):
			print("UseItemSlot > if slotIndex == (player.ACCE_SLOT_MAX - 1))")
			return

		mouseModule.mouseController.DeattachObject()
		net.SendAcceRefineCheckOut(slotIndex, self.type)

	def SelectItemSlot(self, selectedSlotPos):
		if selectedSlotPos == (player.ACCE_SLOT_MAX - 1):
			print("SelectItemSlot > if selectedSlotPos == (player.ACCE_SLOT_MAX - 1)")
			return

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
				selectedItemID = player.GetAcceItemID(selectedSlotPos)
				mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_ACCE, selectedSlotPos, selectedItemID)
				snd.PlaySound("sound/ui/pick.wav")

	def RefreshAcceWindow(self):
		getAcceItem = player.GetAcceItemID
		setAcceItem = self.wndItem.SetItemSlot
		AcceItemSize = player.GetAcceItemSize()

		for i in xrange(AcceItemSize):
			setAcceItem(i, getAcceItem(i), 1)
			if self.type == player.ACCE_SLOT_TYPE_COMBINE:

				if app.ENABLE_ACCE_SECOND_COSTUME_SYSTEM:
					if i == player.ACCE_SLOT_LEFT:
						if getAcceItem(i) != 0:
							item.SelectItem(getAcceItem(i))
							if self.cost != None:
								self.cost.SetText(localeInfo.ACCE_ABSORB_COST % (item.GetIBuyItemPrice()))
						else:
							if self.cost != None:
								self.cost.SetText("")
				else:
					if i == player.ACCE_SLOT_MAX - player.ACCE_SLOT_MAX:
						if getAcceItem(i) != 0:
							item.SelectItem(getAcceItem(i))
							if self.cost != None:
								self.cost.SetText(localeInfo.ACCE_ABSORB_COST % (item.GetIBuyItemPrice()))
						else:
							if self.cost != None:
								self.cost.SetText("")

				if app.ENABLE_ACCE_SECOND_COSTUME_SYSTEM:
					if i == player.ACCE_SLOT_RIGHT:
						if getAcceItem(i) != 0:
							item.SelectItem(getAcceItem(i))
							if item.GetRefinedVnum() == 0:
								if self.cost != None:
									self.cost.SetText(localeInfo.ACCE_ABSORB_COST % (item.GetIBuyItemPrice()))

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				if self.type == player.ACCE_SLOT_TYPE_ABSORB:
					changelookvnum = player.GetAcceWindowChangeLookVnum(i)
					if not changelookvnum == 0:
						self.wndItem.SetSlotCoverImage(i, "icon/item/ingame_convert_Mark.tga")
					else:
						self.wndItem.EnableSlotCoverImage(i, False)

		self.wndItem.RefreshSlot()

	def __OnClosePopupDialog(self):
		self.popup = None

	def OnUpdate(self):
		(x, y, z) = player.GetMainCharacterPosition()
		if abs(x - self.xAcceWindowStart) > player.SHOW_UI_WINDOW_LIMIT_RANGE or abs(y - self.yAcceWindowStart) > player.SHOW_UI_WINDOW_LIMIT_RANGE:
			self.Close()

	def OnPressEscapeKey(self):
		self.Close()
		return True
