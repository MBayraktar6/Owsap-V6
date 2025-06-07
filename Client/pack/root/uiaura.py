import app
import ui
import uiCommon
import mouseModule
import localeInfo
import snd
import player
import chat
import item
import net

class AuraWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.Initialize()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Initialize(self):
		self.isLoaded = 0
		self.type = 0
		self.tooltipItem = None
		self.xAuraWindowStart = 0
		self.yAuraWindowStart = 0

	def __LoadWindow(self, type):
		pyScriptFileNames = {
			player.AURA_WINDOW_TYPE_ABSORB : "UIScript/Aura_AbsorbWindow.py",
			player.AURA_WINDOW_TYPE_GROWTH : "UIScript/Aura_LevelUpWindow.py",
			player.AURA_WINDOW_TYPE_EVOLVE : "UIScript/Aura_LevelUpWindow.py"
		}

		wndTitleNames = {
			player.AURA_WINDOW_TYPE_ABSORB : localeInfo.AURA_TITLE_ABSORB,
			player.AURA_WINDOW_TYPE_GROWTH : localeInfo.AURA_TITLE_GROWTH,
			player.AURA_WINDOW_TYPE_EVOLVE : localeInfo.AURA_TITLE_EVOLUTION
		}

		if pyScriptFileNames.has_key(type):
			try:
				pyScrLoader = ui.PythonScriptLoader()
				pyScrLoader.LoadScriptFile(self, pyScriptFileNames[type])
			except:
				import exception
				exception.Abort("AuraWindow.__LoadWindow.LoadScript")
		else:
			import exception
			exception.Abort("AuraWindow.__LoadWindow.InvalidWindow[%d]" % (type))
			return

		try:
			wndItem = self.GetChild("AuraSlot")
			self.GetChild("AcceptButton").SetEvent(ui.__mem_func__(self.Accept))
			self.GetChild("CancelButton").SetEvent(ui.__mem_func__(self.Close))
			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
			self.GetChild("TitleName").SetText(wndTitleNames[type])
		except:
			import exception
			exception.Abort("AuraWindow.__LoadWindow.LoadBindGenerals")

		self.AuraToolTip1 = self.GetChild2("AuraToolTip1")
		self.AuraToolTip2 = self.GetChild2("AuraToolTip2")
		self.AuraToolTip3 = self.GetChild2("AuraToolTip3")
		self.AuraToolTip4 = self.GetChild2("AuraToolTip4")
		self.AuraToolTip5 = self.GetChild2("AuraToolTip5")
		self.AuraToolTip6 = self.GetChild2("AuraToolTip6")

		wndItem.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
		wndItem.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
		wndItem.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndItem.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndItem.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		wndItem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		wndItem.Show()
		self.wndItem = wndItem

	def ClearAuraLevelUpWindowToolTip(self):
		if self.type == player.AURA_WINDOW_TYPE_GROWTH or self.type == player.AURA_WINDOW_TYPE_EVOLVE:
			self.AuraToolTip1.SetText("")
			self.AuraToolTip2.SetText("")
			self.AuraToolTip3.SetText("")
			self.AuraToolTip4.SetText("")
			self.AuraToolTip5.SetText("")
			self.AuraToolTip6.SetText("")

	def __ShowToolTip(self, slotIndex):
		if self.tooltipItem:
			self.tooltipItem.SetAuraWindowItem(slotIndex)

	def SetItemToolTip(self, tooltip):
		self.tooltipItem = tooltip

	def SelectEmptySlot(self, selectedSlotPos):
		if selectedSlotPos == (player.AURA_SLOT_MAX - 1):
			return

		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedSlotWindow = player.SlotTypeToInvenType(attachedSlotType)

			itemVnum = player.GetItemIndex(attachedSlotWindow, attachedSlotPos)
			item.SelectItem(itemVnum)

			itemType = item.GetItemType()
			itemSubType = item.GetItemSubType()

			window = player.SlotTypeToInvenType(attachedSlotType)
			if window == player.EQUIPMENT:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_EQUIPITEM)
				return

			if player.SLOT_TYPE_AURA != attachedSlotType:
				attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)
				if player.RESERVED_WINDOW == attachedSlotWindow:
					return

				possableCheckIn = False
				if player.GetAuraWindowType() == player.AURA_WINDOW_TYPE_ABSORB:
					if selectedSlotPos == player.AURA_SLOT_MAIN:
						if itemType == item.ITEM_TYPE_COSTUME:
							if itemSubType == item.COSTUME_TYPE_AURA:
								if player.GetItemMetinSocket(attachedSlotWindow, attachedSlotPos, item.ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM) == 0:
									possableCheckIn = True
								else:
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_ABSORBITEM)
									return
							else:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURAITEM)
								return
						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURAITEM)
							return

					elif selectedSlotPos == player.AURA_SLOT_SUB:
						if item.IsWeddingItem(itemVnum) == 1:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_ABSORBITEM)
							return

						if itemType == item.ITEM_TYPE_ARMOR:
							if itemSubType in [item.ARMOR_SHIELD, item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR]:
								if player.FindUsingAuraSlot(player.AURA_SLOT_MAIN) != player.NPOS():
									possableCheckIn = True

							else:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_REFINEITEM)
								return
						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_ABSORBITEM)
							return

				elif player.GetAuraWindowType() == player.AURA_WINDOW_TYPE_GROWTH:
					if selectedSlotPos == player.AURA_SLOT_MAIN:
						if itemType == item.ITEM_TYPE_COSTUME:
							if itemSubType == item.COSTUME_TYPE_AURA:
								socketLevelValue = player.GetItemMetinSocket(attachedSlotWindow, attachedSlotPos, item.ITEM_SOCKET_AURA_LEVEL_VALUE)
								curLevel = (socketLevelValue / 100000) - 1000
								curExp = socketLevelValue % 100000;
								if curLevel >= player.AURA_MAX_LEVEL:
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_MAX_LEVEL)
									return

								if curExp >= player.GetAuraRefineInfo(curLevel, item.AURA_REFINE_INFO_NEED_EXP):
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_GROWTHITEM)
									return

								possableCheckIn = True
							else:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURAITEM)
								return
						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURAITEM)
							return

					elif selectedSlotPos == player.AURA_SLOT_SUB:
						if player.FindUsingAuraSlot(player.AURA_SLOT_MAIN) != player.NPOS():
							if itemType == item.ITEM_TYPE_RESOURCE:
								if itemSubType == item.RESOURCE_AURA:
									possableCheckIn = True

				elif player.GetAuraWindowType() == player.AURA_WINDOW_TYPE_EVOLVE:
					if selectedSlotPos == player.AURA_SLOT_MAIN:
						if itemType == item.ITEM_TYPE_COSTUME:
							if itemSubType == item.COSTUME_TYPE_AURA:
								socketLevelValue = player.GetItemMetinSocket(attachedSlotWindow, attachedSlotPos, item.ITEM_SOCKET_AURA_LEVEL_VALUE)
								curLevel = (socketLevelValue / 100000) - 1000
								curExp = socketLevelValue % 100000;
								if curLevel >= player.AURA_MAX_LEVEL:
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_MAX_LEVEL)
									return

								if curLevel != player.GetAuraRefineInfo(curLevel, item.AURA_REFINE_INFO_LEVEL_MAX) or curExp < player.GetAuraRefineInfo(curLevel, item.AURA_REFINE_INFO_NEED_EXP):
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_EVOLUTION_ITEM)
									return

								possableCheckIn = True
							else:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURAITEM)
								return
						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURAITEM)
							return

					elif selectedSlotPos == player.AURA_SLOT_SUB:
						Cell = player.FindUsingAuraSlot(player.AURA_SLOT_MAIN)
						if Cell != player.NPOS():
							socketLevelValue = player.GetItemMetinSocket(*(Cell + (item.ITEM_SOCKET_AURA_LEVEL_VALUE,)))
							curLevel = (socketLevelValue / 100000) - 1000
							if itemVnum == player.GetAuraRefineInfo(curLevel, item.AURA_REFINE_INFO_MATERIAL_VNUM):
								if player.GetItemCount(attachedSlotWindow, attachedSlotPos) >= player.GetAuraRefineInfo(curLevel, item.AURA_REFINE_INFO_MATERIAL_COUNT):
									possableCheckIn = True
								else:
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_EVOLUTION_ITEMCOUNT)
									return
							else:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_EVOLUTION_ITEM)
								return

				if possableCheckIn:
					if app.ENABLE_SOULBIND_SYSTEM:
						if player.GetItemSealDate(player.INVENTORY, attachedSlotPos) != item.E_SEAL_DATE_DEFAULT_TIMESTAMP:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_SEALITEM)
							return

					if player.GetAuraWindowType() == player.AURA_WINDOW_TYPE_ABSORB:
						if selectedSlotPos == player.AURA_SLOT_SUB:
							popup = uiCommon.QuestionDialog()
							popup.SetText(localeInfo.AURA_NOTICE_DEL_ABSORDITEM)
							popup.SetAcceptEvent(lambda arg1 = attachedInvenType, arg2 = attachedSlotPos, arg3 = selectedSlotPos: self.OnAuraAcceptEvent(arg1, arg2, arg3))
							popup.SetCancelEvent(self.OnAuraCloseEvent)
							popup.Open()
							self.popup = popup
						else:
							net.SendAuraRefineCheckIn(attachedInvenType, attachedSlotPos, selectedSlotPos, self.type)

					elif player.GetAuraWindowType() == player.AURA_WINDOW_TYPE_GROWTH:
						net.SendAuraRefineCheckIn(attachedInvenType, attachedSlotPos, selectedSlotPos, self.type)

					elif player.GetAuraWindowType() == player.AURA_WINDOW_TYPE_EVOLVE:
						net.SendAuraRefineCheckIn(attachedInvenType, attachedSlotPos, selectedSlotPos, self.type)

					snd.PlaySound("sound/ui/drop.wav")

			mouseModule.mouseController.DeattachObject()

	def SelectItemSlot(self, selectedSlotPos):
		if selectedSlotPos == (player.AURA_SLOT_MAX - 1):
			return

		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			if app.ENABLE_EXTEND_INVEN_SYSTEM:
				if attachedSlotType in [player.SLOT_TYPE_INVENTORY, player.SLOT_TYPE_DRAGON_SOUL_INVENTORY, player.SLOT_TYPE_BELT_INVENTORY]:
					snd.PlaySound("sound/ui/drop.wav")
			else:
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
				selectedItemID = player.GetAuraItemID(selectedSlotPos)
				mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_AURA, player.AURA_REFINE, selectedSlotPos, selectedItemID)
				snd.PlaySound("sound/ui/pick.wav")

	def UseItemSlot(self, slotIndex):
		if slotIndex == (player.AURA_SLOT_MAX - 1):
			return

		mouseModule.mouseController.DeattachObject()
		net.SendAuraRefineCheckOut(slotIndex, self.type)

	def OverInItem(self, slotIndex):
		self.wndItem.SetUsableItem(False)
		self.__ShowToolTip(slotIndex)

	def OverOutItem(self):
		self.wndItem.SetUsableItem(False)
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def OnAuraAcceptEvent(self, attachedInvenType, attachedSlotPos, selectedSlotPos):
		self.__OnClosePopupDialog()
		net.SendAuraRefineCheckIn(attachedInvenType, attachedSlotPos, selectedSlotPos, self.type)

	def OnAuraCloseEvent(self):
		self.__OnClosePopupDialog()

	def __OnClosePopupDialog(self):
		self.popup.Close()
		self.popup = None

	def RefreshAuraWindow(self):
		if not self.wndItem:
			return

		getAuraItem = player.GetAuraItemID
		getAuraCount = player.GetAuraItemCount
		setAuraItem = self.wndItem.SetItemSlot
		AuraItemSize = player.GetAuraCurrentItemSlotCount()

		self.ClearAuraLevelUpWindowToolTip()
		[self.wndItem.ClearSlot(i) for i in range(player.AURA_SLOT_MAX)]

		for i in xrange(AuraItemSize):
			itemCount = getAuraCount(i)
			if 1 == itemCount:
				itemCount = 0

			setAuraItem(i, getAuraItem(i), itemCount)

			if self.type == player.AURA_WINDOW_TYPE_GROWTH:
				if i == player.AURA_SLOT_MAIN:
					if getAuraItem(i) != 0:
						curLevel = player.GetAuraRefineInfoLevel(player.AURA_REFINE_INFO_SLOT_CURRENT)
						curStep = player.GetAuraRefineInfo(curLevel, item.AURA_REFINE_INFO_STEP)
						curDrainPct = 1.0 * curLevel / 10.
						curExpPct = player.GetAuraRefineInfoExpPer(player.AURA_REFINE_INFO_SLOT_CURRENT)

						self.AuraToolTip1.SetText(localeInfo.AURA_TOOLTIP_NOW)
						self.AuraToolTip2.SetText(localeInfo.AURA_TOOLTIP_LEVEL % (curLevel, curStep, curDrainPct))
						self.AuraToolTip3.SetText(localeInfo.AURA_TOOLTIP_EXP % (curExpPct))

				if i == player.AURA_SLOT_SUB:
					if getAuraItem(i) != 0:
						nextLevel = player.GetAuraRefineInfoLevel(player.AURA_REFINE_INFO_SLOT_NEXT)
						nextStep = player.GetAuraRefineInfo(nextLevel, item.AURA_REFINE_INFO_STEP)
						nextDrainPct = 1.0 * nextLevel / 10.
						nextExpPct = player.GetAuraRefineInfoExpPer(player.AURA_REFINE_INFO_SLOT_NEXT)

						self.AuraToolTip4.SetText(localeInfo.AURA_TOOLTIP_NEXT)
						self.AuraToolTip5.SetText(localeInfo.AURA_TOOLTIP_LEVEL % (nextLevel, curStep, nextDrainPct))
						self.AuraToolTip6.SetText(localeInfo.AURA_TOOLTIP_EXP % (nextExpPct))

			if self.type == player.AURA_WINDOW_TYPE_EVOLVE:
				if i == player.AURA_SLOT_MAIN:
					if getAuraItem(i) != 0:
						curLevel = player.GetAuraRefineInfoLevel(player.AURA_REFINE_INFO_SLOT_CURRENT)
						needMaterialVnum = player.GetAuraRefineInfo(curLevel, item.AURA_REFINE_INFO_MATERIAL_VNUM)
						needMaterialCount = player.GetAuraRefineInfo(curLevel, item.AURA_REFINE_INFO_MATERIAL_COUNT)
						needGold = player.GetAuraRefineInfo(curLevel, item.AURA_REFINE_INFO_NEED_GOLD)
						evolPer = player.GetAuraRefineInfo(curLevel, item.AURA_REFINE_INFO_EVOLVE_PCT)

						nextLevel = player.GetAuraRefineInfoLevel(player.AURA_REFINE_INFO_SLOT_EVOLVED)
						nextStep = player.GetAuraRefineInfo(nextLevel, item.AURA_REFINE_INFO_STEP)
						nextDrainPct = 1.0 * nextLevel / 10.

						self.AuraToolTip1.SetText(localeInfo.AURA_TOOLTIP_NEXT)
						self.AuraToolTip2.SetText(localeInfo.AURA_TOOLTIP_LEVEL % (nextLevel, nextStep, nextDrainPct))
						self.AuraToolTip3.SetText(localeInfo.REFINE_SUCCESS_PROBALITY % (evolPer))

						self.AuraToolTip4.SetText(localeInfo.AURA_TOOLTIP_RESOURCE)
						self.AuraToolTip5.SetText(localeInfo.AURA_TOOLTIP_RESOURCE_ITEM % (item.GetItemNameByVnum(needMaterialVnum), needMaterialCount))
						self.AuraToolTip6.SetText(localeInfo.AURA_TOOLTIP_NEED_YANG % (localeInfo.NumberToMoneyString(needGold)))

		self.wndItem.RefreshSlot()

	def Accept(self):
		if player.IsAuraSlotEmpty():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_ITEM_NEED_REGISTER)
			return

		net.SendAuraRefineAccept(self.type)

	def Open(self, type):
		if self.isLoaded == 0:
			self.isLoaded = 1
			self.__LoadWindow(type)

		self.SetCenterPosition()
		self.SetTop()
		ui.ScriptWindow.Show(self)

		(self.xAuraWindowStart, self.yAuraWindowStart, z) = player.GetMainCharacterPosition()
		self.type = type

	def Close(self):
		net.SendAuraRefineCancel()

		self.ClearAuraLevelUpWindowToolTip()

		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

		ui.ScriptWindow.Hide(self)
		self.isLoaded = 0

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def OnUpdate(self):
		(x, y, z) = player.GetMainCharacterPosition()
		if abs(x - self.xAuraWindowStart) > player.SHOW_UI_WINDOW_LIMIT_RANGE or abs(y - self.yAuraWindowStart) > player.SHOW_UI_WINDOW_LIMIT_RANGE:
			self.Close()
