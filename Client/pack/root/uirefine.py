import app
import net
import player
import item
import ui
import uiToolTip
import mouseModule
import localeInfo
import uiCommon
import constInfo
import osfInfo

if app.WJ_ENABLE_TRADABLE_ICON:
	INVENTORY_PAGE_SIZE = player.INVENTORY_PAGE_SIZE

if osfInfo.SHOW_REFINE_ITEM_DESC == True:
	TOOLTIP_DATA = {
		"materials" : [],
		"slot_count" : 0
	}

class RefineDialog(ui.ScriptWindow):
	makeSocketSuccessPercentage = ( 100, 33, 20, 15, 10, 5, 0 )
	upgradeStoneSuccessPercentage = ( 30, 29, 28, 27, 26, 25, 24, 23, 22 )
	upgradeArmorSuccessPercentage = ( 99, 66, 33, 33, 33, 33, 33, 33, 33 )
	upgradeAccessorySuccessPercentage = ( 99, 88, 77, 66, 33, 33, 33, 33, 33 )
	upgradeSuccessPercentage = ( 99, 66, 33, 33, 33, 33, 33, 33, 33 )

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadScript()

		self.scrollItemPos = 0
		self.targetItemPos = 0
		self.IsShow = False

	def __LoadScript(self):
		self.__LoadQuestionDialog()

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/refinedialog.py")

		except:
			import exception
			exception.Abort("RefineDialog.__LoadScript.LoadObject")

		try:
			self.board = self.GetChild("Board")
			self.titleBar = self.GetChild("TitleBar")
			self.successPercentage = self.GetChild("SuccessPercentage")
			self.GetChild("AcceptButton").SetEvent(self.OpenQuestionDialog)
			self.GetChild("CancelButton").SetEvent(self.Close)
		except:
			import exception
			exception.Abort("RefineDialog.__LoadScript.BindObject")

		## 936 : 개량 확률 표시 안함
		##if 936 == app.GetDefaultCodePage():
		if osfInfo.SHOW_REFINE_PERCENTAGE == True:
			self.successPercentage.Show()
		else:
			self.successPercentage.Hide()

		toolTip = uiToolTip.ItemToolTip()
		toolTip.SetParent(self)
		toolTip.SetPosition(15, 38)
		toolTip.SetFollow(False)
		toolTip.Show()
		self.toolTip = toolTip

		self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadQuestionDialog(self):
		self.dlgQuestion = ui.ScriptWindow()

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self.dlgQuestion, "uiscript/questiondialog2.py")
		except:
			import exception
			exception.Abort("RefineDialog.__LoadQuestionDialog.LoadScript")

		try:
			GetObject=self.dlgQuestion.GetChild
			GetObject("message1").SetText(localeInfo.REFINE_DESTROY_WARNING)
			GetObject("message2").SetText(localeInfo.REFINE_WARNING2)
			GetObject("accept").SetEvent(ui.__mem_func__(self.Accept))
			GetObject("cancel").SetEvent(ui.__mem_func__(self.dlgQuestion.Hide))
		except:
			import exception
			exception.Abort("SelectCharacterWindow.__LoadQuestionDialog.BindObject")

	def Destroy(self):
		self.ClearDictionary()
		self.board = 0
		self.successPercentage = 0
		self.titleBar = 0
		self.toolTip = 0
		self.dlgQuestion = 0

	def GetRefineSuccessPercentage(self, scrollSlotIndex, itemSlotIndex):
		if -1 != scrollSlotIndex:
			if player.IsRefineGradeScroll(scrollSlotIndex):
				curGrade = player.GetItemGrade(itemSlotIndex)
				itemIndex = player.GetItemIndex(itemSlotIndex)

				item.SelectItem(itemIndex)
				itemType = item.GetItemType()
				itemSubType = item.GetItemSubType()

				if item.ITEM_TYPE_METIN == itemType:
					if curGrade >= len(self.upgradeStoneSuccessPercentage):
						return 0
					return self.upgradeStoneSuccessPercentage[curGrade]

				elif item.ITEM_TYPE_ARMOR == itemType:
					if item.ARMOR_BODY == itemSubType:
						if curGrade >= len(self.upgradeArmorSuccessPercentage):
							return 0
						return self.upgradeArmorSuccessPercentage[curGrade]
					else:
						if curGrade >= len(self.upgradeAccessorySuccessPercentage):
							return 0
						return self.upgradeAccessorySuccessPercentage[curGrade]
				else:
					if curGrade >= len(self.upgradeSuccessPercentage):
						return 0
					return self.upgradeSuccessPercentage[curGrade]

		for i in xrange(player.METIN_SOCKET_MAX_NUM+1):
			if 0 == player.GetItemMetinSocket(itemSlotIndex, i):
				break

		return self.makeSocketSuccessPercentage[i]

	def Open(self, scrollItemPos, targetItemPos):
		self.scrollItemPos = scrollItemPos
		self.targetItemPos = targetItemPos

		percentage = self.GetRefineSuccessPercentage(scrollItemPos, targetItemPos)
		if 0 == percentage:
			return
		self.successPercentage.SetText(localeInfo.REFINE_SUCCESS_PROBALITY % (percentage))

		itemIndex = player.GetItemIndex(targetItemPos)
		self.toolTip.ClearToolTip()
		metinSlot = []
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			metinSlot.append(player.GetItemMetinSocket(targetItemPos, i))
		self.toolTip.AddItemData(itemIndex, metinSlot)

		self.UpdateDialog()
		self.SetTop()
		self.Show()
		self.IsShow = True

	def UpdateDialog(self):
		newWidth = self.toolTip.GetWidth() + 30
		newHeight = self.toolTip.GetHeight() + 98
		self.board.SetSize(newWidth, newHeight)
		self.titleBar.SetWidth(newWidth-15)
		self.SetSize(newWidth, newHeight)

		(x, y) = self.GetLocalPosition()
		self.SetPosition(x, y)

	def OpenQuestionDialog(self):
		percentage = self.GetRefineSuccessPercentage(-1, self.targetItemPos)
		if 100 == percentage:
			self.Accept()
			return

		self.dlgQuestion.SetTop()
		self.dlgQuestion.Show()

	def Accept(self):
		net.SendItemUseToItemPacket(self.scrollItemPos, self.targetItemPos)
		self.Close()

	def Close(self):
		self.dlgQuestion.Hide()
		self.Hide()
		self.IsShow = False

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def IsShow(self):
		return self.IsShow

class RefineDialogNew(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__Initialize()
		self.isLoaded = False

		if app.WJ_ENABLE_TRADABLE_ICON:
			self.interface = None
			self.inven = None

	def __Initialize(self):
		self.dlgQuestion = None
		self.children = []
		self.vnum = 0
		self.targetItemPos = 0
		self.dialogHeight = 0
		self.cost = 0
		self.percentage = 0
		self.type = 0
		self.src_vnum = 0
		self.IsShow = False

	def __LoadScript(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/refinedialog.py")

		except:
			import exception
			exception.Abort("RefineDialog.__LoadScript.LoadObject")

		try:
			self.board = self.GetChild("Board")
			self.titleBar = self.GetChild("TitleBar")
			self.probText = self.GetChild("SuccessPercentage")
			self.costText = self.GetChild("Cost")
			self.GetChild("AcceptButton").SetEvent(self.OpenQuestionDialog)
			self.GetChild("CancelButton").SetEvent(self.CancelRefine)
		except:
			import exception
			exception.Abort("RefineDialog.__LoadScript.BindObject")

		## 936 : 개량 확률 표시 안함
		##if 936 == app.GetDefaultCodePage():
		if osfInfo.SHOW_REFINE_PERCENTAGE == True:
			self.probText.Show()
		else:
			self.probText.Hide()

		toolTip = uiToolTip.ItemToolTip()
		toolTip.SetParent(self)
		toolTip.SetFollow(False)
		toolTip.SetPosition(15, 38)
		toolTip.Show()
		self.toolTip = toolTip

		if osfInfo.SHOW_REFINE_ITEM_DESC == True:
			self.toolTipItem = uiToolTip.ItemToolTip()
			self.toolTipItem.Hide()

		self.slotList = []
		for i in xrange(3):
			slot = self.__MakeSlot()
			slot.SetParent(toolTip)
			slot.SetWindowVerticalAlignCenter()
			self.slotList.append(slot)

		itemImage = self.__MakeItemImage()
		itemImage.SetParent(toolTip)
		itemImage.SetWindowVerticalAlignCenter()
		itemImage.SetPosition(-35, 0)
		self.itemImage = itemImage

		self.titleBar.SetCloseEvent(ui.__mem_func__(self.CancelRefine))
		self.isLoaded = True

	def __del__(self):
		ui.ScriptWindow.__del__(self)

		if app.WJ_ENABLE_TRADABLE_ICON:
			self.inven = None

	def __MakeSlot(self):
		slot = ui.ImageBox()
		slot.LoadImage("d:/ymir work/ui/public/slot_base.sub")
		slot.Show()
		self.children.append(slot)
		return slot

	if osfInfo.SHOW_REFINE_ITEM_DESC == True:
		def __MakeItemSlot(self, slotIndex):
			slot = ui.SlotWindow()
			slot.SetParent(self)
			slot.SetSize(32, 32)
			slot.SetSlotBaseImage("d:/ymir work/ui/public/Slot_Base.sub", 1.0, 1.0, 1.0, 1.0)
			slot.AppendSlot(slotIndex, 0, 0, 32, 32)
			slot.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			slot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			slot.RefreshSlot()
			slot.Show()
			self.children.append(slot)
			return slot

		def OverInItem(self, slotIndex):
			if slotIndex > len(TOOLTIP_DATA["materials"]):
				return

			itemVnum = TOOLTIP_DATA["materials"][slotIndex]
			metinSlot = [0 for i in xrange(player.METIN_SOCKET_MAX_NUM)]
			attrSlot = [(0, 0) for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]

			if self.toolTipItem:
				self.toolTipItem.ClearToolTip()
				self.toolTipItem.AddItemData(itemVnum, metinSlot, attrSlot)

		def OverOutItem(self):
			if self.toolTipItem:
				self.toolTipItem.HideToolTip()

	def __MakeItemImage(self):
		itemImage = ui.ImageBox()
		itemImage.Show()
		self.children.append(itemImage)
		return itemImage

	def __MakeThinBoard(self):
		thinBoard = ui.ThinBoard()
		thinBoard.SetParent(self)
		thinBoard.Show()
		self.children.append(thinBoard)
		return thinBoard

	def Destroy(self):
		self.ClearDictionary()
		self.dlgQuestion = None
		self.board = 0
		self.probText = 0
		self.costText = 0
		self.titleBar = 0
		self.toolTip = 0
		self.slotList = []
		self.children = []

	def Open(self, targetItemPos, nextGradeItemVnum, cost, prob, type, apply_random_list, src_vnum):
		if False == self.isLoaded:
			self.__LoadScript()

		self.__Initialize()

		self.targetItemPos = targetItemPos
		self.vnum = nextGradeItemVnum
		self.cost = cost
		self.percentage = prob
		self.type = type
		self.src_vnum = src_vnum

		self.probText.SetText(localeInfo.REFINE_SUCCESS_PROBALITY % (self.percentage))
		self.costText.SetText(localeInfo.REFINE_COST % (self.cost))

		self.toolTip.ClearToolTip()
		metinSlot = []
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			metinSlot.append(player.GetItemMetinSocket(targetItemPos, i))

		attrSlot = []
		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			attrSlot.append(player.GetItemAttribute(targetItemPos, i))

		refine_element = player.GetItemRefineElement(targetItemPos) if app.ENABLE_REFINE_ELEMENT_SYSTEM else None
		set_value = player.GetItemSetValue(targetItemPos) if app.ENABLE_SET_ITEM else 0

		self.toolTip.AddRefineItemData(nextGradeItemVnum, metinSlot, attrSlot, None, type, refine_element, apply_random_list, set_value)

		item.SelectItem(nextGradeItemVnum)
		self.itemImage.LoadImage(item.GetIconImageFileName())
		xSlotCount, ySlotCount = item.GetItemSize()
		for slot in self.slotList:
			slot.Hide()
		for i in xrange(min(3, ySlotCount)):
			self.slotList[i].SetPosition(-35, i*32 - (ySlotCount-1)*16)
			self.slotList[i].Show()

		self.dialogHeight = self.toolTip.GetHeight() + 46
		self.UpdateDialog()

		self.SetTop()

		self.Show()
		self.IsShow = True

	def Close(self):
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.SetCanMouseEventSlot(self.targetItemPos)

		self.dlgQuestion = None
		self.Hide()
		self.IsShow = False

	def IsShow(self):
		return self.IsShow

	def AppendMaterial(self, vnum, count):
		if osfInfo.SHOW_REFINE_ITEM_DESC == True:
			slotIndex = len(TOOLTIP_DATA["materials"])

			slot = self.__MakeItemSlot(slotIndex)
			slot.SetPosition(15, self.dialogHeight)
			slot.SetItemSlot(slotIndex, vnum, count)

			TOOLTIP_DATA["materials"].append(vnum)
		else:
			slot = self.__MakeSlot()
			slot.SetParent(self)
			slot.SetPosition(15, self.dialogHeight)

			itemImage = self.__MakeItemImage()
			itemImage.SetParent(slot)
			item.SelectItem(vnum)
			itemImage.LoadImage(item.GetIconImageFileName())

		thinBoard = self.__MakeThinBoard()
		thinBoard.SetPosition(50, self.dialogHeight)
		thinBoard.SetSize(self.toolTip.GetWidth(), 20)

		textLine = ui.TextLine()
		textLine.SetParent(thinBoard)
		textLine.SetFontName(localeInfo.UI_DEF_FONT)
		textLine.SetPackedFontColor(0xffdddddd)
		textLine.SetText("%s x %02d" % (item.GetItemName(), count))
		textLine.SetOutline()
		textLine.SetFeather(False)
		textLine.SetWindowVerticalAlignCenter()
		textLine.SetVerticalAlignCenter()

		if localeInfo.IsARABIC():
			(x,y) = textLine.GetTextSize()
			textLine.SetPosition(x, 0)
		else:
			textLine.SetPosition(15, 0)

		textLine.Show()
		self.children.append(textLine)

		self.dialogHeight += 34
		self.UpdateDialog()

	def UpdateDialog(self):
		newWidth = self.toolTip.GetWidth() + 60
		newHeight = self.dialogHeight + 69
		if osfInfo.SHOW_REFINE_PERCENTAGE == True:
			newHeight += 15

		## 936 : 개량 확률 표시 안함
		##if 936 == app.GetDefaultCodePage():
		newHeight -= 8

		if localeInfo.IsARABIC():
			self.board.SetPosition(newWidth, 0)

			(x, y) = self.titleBar.GetLocalPosition()
			self.titleBar.SetPosition(newWidth - 15, y)

		self.board.SetSize(newWidth, newHeight)
		self.toolTip.SetPosition(15 + 35, 38)
		self.titleBar.SetWidth(newWidth-15)
		self.SetSize(newWidth, newHeight)

		(x, y) = self.GetLocalPosition()
		self.SetPosition(x, y)

	def OpenQuestionDialog(self):
		if 100 == self.percentage:
			self.Accept()
			return

		if 5 == self.type: ## 무신의 축복서
			self.Accept()
			return

		if app.ENABLE_STONE_OF_BLESS:
			if 7 == self.type:
				self.Accept()
				return

		if app.ENABLE_SOUL_SYSTEM:
			if 8 == self.type or 9 == self.type:
				self.Accept()
				return

		dlgQuestion = uiCommon.QuestionDialog2()
		dlgQuestion.SetText2(localeInfo.REFINE_WARNING2)
		dlgQuestion.SetAcceptEvent(ui.__mem_func__(self.Accept))
		dlgQuestion.SetCancelEvent(ui.__mem_func__(dlgQuestion.Close))

		if 3 == self.type: ## 현철
			if localeInfo.IsEUROPE():
				if dlgQuestion:
					del dlgQuestion

				dlgQuestion = uiCommon.QuestionDialog()
				dlgQuestion.SetAcceptEvent(ui.__mem_func__(self.Accept))
				dlgQuestion.SetCancelEvent(ui.__mem_func__(dlgQuestion.Close))
				dlgQuestion.SetText(localeInfo.REFINE_DESTROY_WARNING_WITH_BONUS_PERCENT_1)
			else:
				dlgQuestion.SetText1(localeInfo.REFINE_DESTROY_WARNING_WITH_BONUS_PERCENT_1)
				dlgQuestion.SetText2(localeInfo.REFINE_DESTROY_WARNING_WITH_BONUS_PERCENT_2)
		elif 2 == self.type: ## 축복서
			dlgQuestion.SetText1(localeInfo.REFINE_DOWN_GRADE_WARNING)
		else:
			dlgQuestion.SetText1(localeInfo.REFINE_DESTROY_WARNING)

		dlgQuestion.Open()
		self.dlgQuestion = dlgQuestion

	def Accept(self):
		net.SendRefinePacket(self.targetItemPos, self.type)
		self.Close()

	def CancelRefine(self):
		net.SendRefinePacket(255, 255)
		self.Close()

		if osfInfo.SHOW_REFINE_ITEM_DESC:
			TOOLTIP_DATA["materials"] = []

	def OnPressEscapeKey(self):
		self.CancelRefine()
		return True

	if app.WJ_ENABLE_TRADABLE_ICON:
		def BindInterface(self, interface):
			from _weakref import proxy
			self.interface = proxy(interface)

		def SetInven(self, inven):
			self.inven = inven

		def SetCanMouseEventSlot(self, idx):
			if idx >= INVENTORY_PAGE_SIZE:
				if app.ENABLE_EXTEND_INVEN_SYSTEM:
					page = self.inven.GetInventoryPageIndex() # 0,1,2,3
					idx -= (page * INVENTORY_PAGE_SIZE)
				else:
					idx -= INVENTORY_PAGE_SIZE

			self.inven.wndItem.SetCanMouseEventSlot(idx)

		def OnUpdate(self):
			if not self.inven:
				return

			targetItemPos = self.targetItemPos
			if targetItemPos <= 0:
				return

			page = self.inven.GetInventoryPageIndex() # range 0 ~ 1

			if (page * INVENTORY_PAGE_SIZE) <= targetItemPos < ((page + 1) * INVENTORY_PAGE_SIZE): # range 0 ~ 44, 45 ~ 89
				lock_idx = targetItemPos - (page * INVENTORY_PAGE_SIZE)
				self.inven.wndItem.SetCantMouseEventSlot(lock_idx)

if app.ENABLE_REFINE_ELEMENT_SYSTEM:
	import chat

	MIN_REFINE_LEVEL = 7

	REFINE_ELEMENT_ITEM_TYPE_TUPLE = (item.ITEM_TYPE_WEAPON,)
	REFINE_ELEMENT_KIND_TUPLE = (item.APPLY_ENCHANT_ELECT, item.APPLY_ENCHANT_FIRE, item.APPLY_ENCHANT_ICE, item.APPLY_ENCHANT_WIND, item.APPLY_ENCHANT_EARTH, item.APPLY_ENCHANT_DARK)

	REFINE_ELEMENT_UPGRADE_YANG = 3000000
	REFINE_ELEMENT_DOWNGRADE_YANG = 10000000
	REFINE_ELEMENT_CHANGE_YANG = 10000000

	class RefineElementChangeDialog(ui.ScriptWindow):
		def CanRefineElementChange(cls, material_window, material_pos, item_window, item_pos, is_fail_msg = True, is_yang_check = True):
			if item_window != player.INVENTORY:
				if is_fail_msg:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_ELEMENT_NOT_EXSIT)
				return False

			material_vnum = player.GetItemIndex(material_window, material_pos)
			if material_vnum == 0:
				return False

			item_vnum = player.GetItemIndex(item_window, item_pos)
			if item_vnum == 0:
				return False

			item.SelectItem(item_vnum)
			if not item.GetItemType() in REFINE_ELEMENT_ITEM_TYPE_TUPLE:
				if is_fail_msg:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_ELEMENT_NOT_EXSIT)
				return False

			if item.GetRefineLevel() < MIN_REFINE_LEVEL:
				if is_fail_msg:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_ELEMENT_NOT_ENOUGHT_GRADE)
				return False

			if app.ENABLE_SOULBIND_SYSTEM:
				if player.GetItemSealDate(item_window, item_pos) != item.E_SEAL_DATE_DEFAULT_TIMESTAMP:
					if is_fail_msg:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_ELEMENT_NOT_EXSIT)
					return False

			refine_element = player.GetItemRefineElement(item_window, item_pos)
			if refine_element[uiToolTip.ItemToolTip.REFINE_ELEMENT_INDEX_GRADE] == 0:
				if is_fail_msg:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_ELEMENT_NOT_EXSIT)
				return False

			if is_yang_check:
				if player.GetElk() < REFINE_ELEMENT_UPGRADE_YANG:
					if is_fail_msg:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_ELEMENT_NOT_ENOUGHT_YANG)
					return False

			return True

		CanRefineElementChange = classmethod(CanRefineElementChange)

		def __init__(self):
			ui.ScriptWindow.__init__(self)
			self.is_loaded = False

			self.tabButtonDict = {}
			self.cost_text = None

			self.popup_dlg = None
			if app.WJ_ENABLE_TRADABLE_ICON:
				self.inven = None

			self.disable_type = 0
			self.apply_type = 0
			self.refine_type = 0
			self.material_pos = 0
			self.item_pos = 0
			self.item_name = None
			self.change_element = None

		def __del__(self):
			ui.ScriptWindow.__del__(self)

		def __Clear(self):
			if app.WJ_ENABLE_TRADABLE_ICON:
				self.__SetMouseEventSlot(False)

			self.disable_type = 0
			self.apply_type = 0
			self.refine_type = 0
			self.material_pos = 0
			self.item_pos = 0
			self.item_name = None
			self.change_element = None

			self.Hide()

		def __LoadScript(self):
			try:
				pyScrLoader = ui.PythonScriptLoader()
				pyScrLoader.LoadScriptFile(self, "UIScript/RefineElementChange.py")
			except:
				import exception
				exception.Abort("RefineElementChangeDialog.__LoadScript.LoadObject")

			try:
				self.__BindObject()
			except:
				import exception
				exception.Abort("RefineElementChangeDialog.__LoadScript.BindObject")

			try:
				self.__BindEvent()
			except:
				import exception
				exception.Abort("RefineElementChangeDialog.__LoadScript.BindEvent")

			self.is_loaded = True

		def __BindObject(self):
			self.tabButtonDict = {
				item.APPLY_ENCHANT_FIRE : self.GetChild("FireButton"),
				item.APPLY_ENCHANT_ICE : self.GetChild("IceButton"),
				item.APPLY_ENCHANT_WIND : self.GetChild("WindButton"),
				item.APPLY_ENCHANT_ELECT : self.GetChild("ElectButton"),
				item.APPLY_ENCHANT_EARTH : self.GetChild("EarthButton"),
				item.APPLY_ENCHANT_DARK : self.GetChild("DarkButton"),
			}

			self.cost_text = self.GetChild("Cost")
			self.cost_text.SetText(localeInfo.NumberToMoneyString(REFINE_ELEMENT_CHANGE_YANG))

			self.popup_dlg = uiCommon.PopupDialog()
			self.popup_dlg.Hide()

		def __BindEvent(self):
			for key, button in self.tabButtonDict.items():
				button.SetEvent(ui.__mem_func__(self.__OnClickTabButton), key)

			self.GetChild("board").SetCloseEvent(ui.__mem_func__(self.__CancelRefine))
			self.GetChild("AcceptButton").SetEvent(self.__ClickAcceptButton)
			self.GetChild("CancelButton").SetEvent(self.__CancelRefine)

		def __ClickAcceptButton(self):
			if self.refine_type != player.REFINE_ELEMENT_CHANGE:
				return

			if self.disable_type == self.apply_type:
				return

			if self.CanRefineElementChange(player.INVENTORY, self.material_pos, player.INVENTORY, self.item_pos):
				net.SendRefineElementPacket(True, self.apply_type)

		def __CancelRefine(self):
			net.SendRefineElementPacket(False)
			self.__Clear()

		def __DisableButton(self, key):
			for apply_type, button in self.tabButtonDict.items():
				button.SetUp()
				button.Enable()

			self.tabButtonDict[key].Disable()

		def __OnClickTabButton(self, key):
			for apply_type, button in self.tabButtonDict.items():
				if apply_type != key and apply_type != self.disable_type:
					button.SetUp()

			self.apply_type = key
			self.change_element = self.tabButtonDict[key].GetText()

		def __Open(self, refine_type, material_pos, item_pos):
			if False == self.is_loaded:
				self.__LoadScript()

			refine_element = player.GetItemRefineElement(item_pos)
			self.disable_type = refine_element[uiToolTip.ItemToolTip.REFINE_ELEMENT_INDEX_APPLY_TYPE]

			self.refine_type = refine_type
			self.material_pos = material_pos
			self.item_pos = item_pos

			item_vnum = player.GetItemIndex(item_pos)
			if item_vnum != 0:
				item.SelectItem(item_vnum)
				self.item_name = item.GetItemName()

			self.__DisableButton(self.disable_type)
			if app.WJ_ENABLE_TRADABLE_ICON:
				self.__SetMouseEventSlot(True)

			self.SetTop()
			self.Show()

		def Destroy(self):
			self.ClearDictionary()

			self.tabButtonDict = {}
			self.cost_text = None

			self.popup_dlg = None
			if app.WJ_ENABLE_TRADABLE_ICON:
				self.inven = None

			self.__Clear()

		def OnPressEscapeKey(self):
			self.__CancelRefine()
			return True

		if app.WJ_ENABLE_TRADABLE_ICON:
			def __SetMouseEventSlot(self, is_cant_mouse_event):
				if not self.inven:
					return

				page = self.inven.GetInventoryPageIndex() # range 0 ~ 1
				min_range = page * player.INVENTORY_PAGE_SIZE
				max_range = (page + 1) * player.INVENTORY_PAGE_SIZE

				if self.material_pos >= 0:
					if (min_range) <= self.material_pos < (max_range): # range 0 ~ 44, 45 ~ 89
						lock_idx = self.material_pos - (min_range)
						if is_cant_mouse_event:
							self.inven.wndItem.SetCantMouseEventSlot(lock_idx)
						else:
							self.inven.wndItem.SetCanMouseEventSlot(lock_idx)

				if self.item_pos >= 0:
					if (min_range) <= self.item_pos < (max_range): # range 0 ~ 44, 45 ~ 89
						lock_idx = self.item_pos - (min_range)
						if is_cant_mouse_event:
							self.inven.wndItem.SetCantMouseEventSlot(lock_idx)
						else:
							self.inven.wndItem.SetCanMouseEventSlot(lock_idx)

			def SetInven(self, inven):
				self.inven = inven

			def OnUpdate(self):
				if not self.inven:
					return

				self.__SetMouseEventSlot(True)

		def RefineElementChangeProcess(self, type, refine_type, data):
			if type == player.REFINE_ELEMENT_GC_OPEN:
				if refine_type == player.REFINE_ELEMENT_CHANGE:
					material_pos, item_pos = data
					self.__Open(refine_type, material_pos, item_pos)

			elif type == player.REFINE_ELEMENT_GC_RESULT:
				if refine_type == player.REFINE_ELEMENT_CHANGE:
					if self.popup_dlg:
						self.popup_dlg.SetText(localeInfo.REFINE_ELEMENT_CHANGE_TEXT % (self.item_name, self.change_element))
						self.popup_dlg.Open()

				self.__Clear()

	class RefineElementDialog(ui.ScriptWindow):
		REFINE_ELEMENT_RANDOM_VALUE_MIN = 1
		REFINE_ELEMENT_RANDOM_VALUE_MAX = 8

		REFINE_ELEMENT_RANDOM_BONUS_VALUE_MIN = 2
		REFINE_ELEMENT_RANDOM_BONUS_VALUE_MAX = 12

		def CanRefineElementUpgrade(cls, material_window, material_pos, item_window, item_pos, is_fail_msg = True, is_yang_check = True):
			if item_window != player.INVENTORY:
				if is_fail_msg:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_ELEMENT_NOT_EXSIT)
				return False

			material_vnum = player.GetItemIndex(material_window, material_pos)
			if material_vnum == 0:
				return False

			item.SelectItem(material_vnum)
			material_element_apply_type = item.GetValue(0)

			item_vnum = player.GetItemIndex(item_window, item_pos)
			if item_vnum == 0:
				return False

			item.SelectItem(item_vnum)
			if not item.GetItemType() in REFINE_ELEMENT_ITEM_TYPE_TUPLE:
				if is_fail_msg:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_ELEMENT_NOT_EXSIT)
				return False

			if item.GetRefineLevel() < MIN_REFINE_LEVEL:
				if is_fail_msg:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_ELEMENT_NOT_ENOUGHT_GRADE)
				return False

			if app.ENABLE_SOULBIND_SYSTEM:
				if player.GetItemSealDate(item_window, item_pos) != item.E_SEAL_DATE_DEFAULT_TIMESTAMP:
					if is_fail_msg:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_ELEMENT_NOT_EXSIT)
					return False

			refine_element = player.GetItemRefineElement(item_window, item_pos)
			if refine_element[uiToolTip.ItemToolTip.REFINE_ELEMENT_INDEX_APPLY_TYPE] != 0 and\
				refine_element[uiToolTip.ItemToolTip.REFINE_ELEMENT_INDEX_APPLY_TYPE] != material_element_apply_type:
				if is_fail_msg:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_ELEMENT_EXIST)
				return False

			if refine_element[uiToolTip.ItemToolTip.REFINE_ELEMENT_INDEX_GRADE] >= item.REFINE_ELEMENT_MAX:
				if is_fail_msg:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_ELEMENT_GRADE_MAX)
				return False

			if is_yang_check:
				if player.GetElk() < REFINE_ELEMENT_UPGRADE_YANG:
					if is_fail_msg:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_ELEMENT_NOT_ENOUGHT_YANG)
					return False

			return True

		def CanRefineElementDowngrade(cls, material_window, material_pos, item_window, item_pos, is_fail_msg = True, is_yang_check = True):
			if item_window != player.INVENTORY:
				if is_fail_msg:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_ELEMENT_NOT_EXSIT)
				return False

			material_vnum = player.GetItemIndex(material_window, material_pos)
			if material_vnum == 0:
				return False

			item_vnum = player.GetItemIndex(item_window, item_pos)
			if item_vnum == 0:
				return False

			item.SelectItem(item_vnum)
			if not item.GetItemType() in REFINE_ELEMENT_ITEM_TYPE_TUPLE:
				if is_fail_msg:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_ELEMENT_NOT_EXSIT)
				return False

			if item.GetRefineLevel() < MIN_REFINE_LEVEL:
				if is_fail_msg:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_ELEMENT_NOT_ENOUGHT_GRADE)
				return False

			if app.ENABLE_SOULBIND_SYSTEM:
				if player.GetItemSealDate(item_window, item_pos) != item.E_SEAL_DATE_DEFAULT_TIMESTAMP:
					if is_fail_msg:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_ELEMENT_NOT_EXSIT)
					return False

			refine_element = player.GetItemRefineElement(item_window, item_pos)
			if refine_element[uiToolTip.ItemToolTip.REFINE_ELEMENT_INDEX_GRADE] == 0:
				if is_fail_msg:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_ELEMENT_NOT_EXSIT)
				return False

			if is_yang_check:
				if player.GetElk() < REFINE_ELEMENT_UPGRADE_YANG:
					if is_fail_msg:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_ELEMENT_NOT_ENOUGHT_YANG)
					return False

			return True

		CanRefineElementUpgrade = classmethod(CanRefineElementUpgrade)
		CanRefineElementDowngrade = classmethod(CanRefineElementDowngrade)

		def __init__(self):
			ui.ScriptWindow.__init__(self)
			self.is_loaded = False

			self.board = None
			self.title_bar = None
			self.title_name = None
			self.cost_text = None
			self.item_image = None
			self.slot_list = []

			self.tooltip = None
			self.popup_dlg = None
			if app.WJ_ENABLE_TRADABLE_ICON:
				self.inven = None

			self.refine_type = 0
			self.material_pos = 0
			self.item_pos = 0
			self.apply_type = 0

		def __del__(self):
			ui.ScriptWindow.__del__(self)

		def __Clear(self):
			if app.WJ_ENABLE_TRADABLE_ICON:
				self.__SetMouseEventSlot(False)

			self.refine_type = 0
			self.material_pos = 0
			self.item_pos = 0
			self.apply_type = 0

			self.Hide()

		def __LoadScript(self):
			try:
				pyScrLoader = ui.PythonScriptLoader()
				pyScrLoader.LoadScriptFile(self, "UIScript/RefineDialog.py")
			except:
				import exception
				exception.Abort("RefineElementDialog.__LoadScript.LoadObject")

			try:
				self.board = self.GetChild("Board")
				self.title_bar = self.GetChild("TitleBar")
				self.cost_text = self.GetChild("Cost")
				self.GetChild("SuccessPercentage").Hide()
				self.GetChild("AcceptButton").SetEvent(self.__ClickAcceptButton)
				self.GetChild("CancelButton").SetEvent(self.__CancelRefine)
			except:
				import exception
				exception.Abort("RefineDialog.__LoadScript.BindObject")

			tooltip = uiToolTip.ItemToolTip()
			tooltip.SetParent(self)
			tooltip.SetFollow(False)
			tooltip.SetPosition(15, 38)
			tooltip.Show()
			self.tooltip = tooltip

			self.slot_list = []
			for i in xrange(3):
				slot = self.__MakeSlot()
				slot.SetParent(tooltip)
				slot.SetWindowVerticalAlignCenter()
				self.slot_list.append(slot)

			item_image = self.__MakeItemImage()
			item_image.SetParent(tooltip)
			item_image.SetWindowVerticalAlignCenter()
			item_image.SetPosition(-35, 0)
			self.item_image = item_image

			self.popup_dlg = uiCommon.PopupDialog()
			self.popup_dlg.Hide()

			self.title_bar.SetCloseEvent(ui.__mem_func__(self.__CancelRefine))
			self.is_loaded = True

		def __MakeSlot(self):
			slot = ui.ImageBox()
			slot.LoadImage("d:/ymir work/ui/public/slot_base.sub")
			slot.Show()
			return slot

		def __MakeItemImage(self):
			item_image = ui.ImageBox()
			item_image.Show()
			return item_image

		def __MakeThinBoard(self):
			thinboard = ui.ThinBoard()
			thinboard.SetParent(self)
			thinboard.Show()
			return thinboard

		def __UpdateDialog(self):
			new_width = self.tooltip.GetWidth() + 60
			new_height = self.tooltip.GetHeight() + 103

			if localeInfo.IsARABIC():
				self.board.SetPosition(new_width, 0)

				(x, y) = self.title_bar.GetLocalPosition()
				self.title_bar.SetPosition(new_width - 15, y)

			self.board.SetSize(new_width, new_height)
			self.tooltip.SetPosition(15 + 35, 38)
			self.title_bar.SetWidth(new_width - 15)
			self.SetSize(new_width, new_height)

			(x, y) = self.GetLocalPosition()
			self.SetPosition(x, y)

		def __UpgradeUIOpen(self, material_pos, item_pos):
			material_vnum = player.GetItemIndex(material_pos)
			if material_vnum == 0:
				return

			item.SelectItem(material_vnum)
			self.apply_type = item.GetValue(0)

			item_vnum = player.GetItemIndex(item_pos)
			if item_vnum == 0:
				return

			metin_slot = [player.GetItemMetinSocket(item_pos, i) for i in xrange(player.METIN_SOCKET_MAX_NUM)]
			attr_slot = [player.GetItemAttribute(item_pos, i) for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]
			refine_element = player.GetItemRefineElement(item_pos)
			apply_random_list = [player.GetItemApplyRandom(item_pos, i) for i in xrange(player.APPLY_RANDOM_SLOT_MAX_NUM)] if app.ENABLE_APPLY_RANDOM else []
			set_value = player.GetItemSetValue(item_pos) if app.ENABLE_SET_ITEM else 0

			new_refine_element = list(refine_element)
			new_refine_element[uiToolTip.ItemToolTip.REFINE_ELEMENT_INDEX_APPLY_TYPE] = self.apply_type
			new_refine_element[uiToolTip.ItemToolTip.REFINE_ELEMENT_INDEX_GRADE] += 1

			self.tooltip.ClearToolTip()
			self.tooltip.isRefineElementItem = True
			self.tooltip.AddRefineItemData(item_vnum, metin_slot, attr_slot, None, 0, new_refine_element, apply_random_list, set_value)

			item.SelectItem(item_vnum)
			self.item_image.LoadImage(item.GetIconImageFileName())

			x_slot_count, y_slot_count = item.GetItemSize()
			for slot in self.slot_list:
				slot.Hide()

			for i in xrange(min(3, y_slot_count)):
				self.slot_list[i].SetPosition(-35, i * 32 - (y_slot_count - 1) * 16)
				self.slot_list[i].Show()

			self.cost_text.SetText(localeInfo.REFINE_COST % REFINE_ELEMENT_UPGRADE_YANG)

		def __DowngradeUIOpen(self, material_pos, item_pos):
			material_vnum = player.GetItemIndex(material_pos)
			if material_vnum == 0:
				return

			item_vnum = player.GetItemIndex(item_pos)
			if item_vnum == 0:
				return

			metin_slot = [player.GetItemMetinSocket(item_pos, i) for i in xrange(player.METIN_SOCKET_MAX_NUM)]
			attr_slot = [player.GetItemAttribute(item_pos, i) for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]
			refine_element = player.GetItemRefineElement(item_pos)
			apply_random_list = [player.GetItemApplyRandom(item_pos, i) for i in xrange(player.APPLY_RANDOM_SLOT_MAX_NUM)] if app.ENABLE_APPLY_RANDOM else []
			set_value = player.GetItemSetValue(item_pos) if app.ENABLE_SET_ITEM else 0

			new_refine_element = list(refine_element)
			new_refine_element[uiToolTip.ItemToolTip.REFINE_ELEMENT_INDEX_GRADE] -= 1

			self.tooltip.ClearToolTip()
			self.tooltip.isRefineElementItem = False
			self.tooltip.AddRefineItemData(item_vnum, metin_slot, attr_slot, None, 0, new_refine_element, apply_random_list, set_value)

			item.SelectItem(item_vnum)
			self.item_image.LoadImage(item.GetIconImageFileName())

			x_slot_count, y_slot_count = item.GetItemSize()
			for slot in self.slot_list:
				slot.Hide()

			for i in xrange(min(3, y_slot_count)):
				self.slot_list[i].SetPosition(-35, i * 32 - (y_slot_count - 1) * 16)
				self.slot_list[i].Show()

			self.cost_text.SetText(localeInfo.REFINE_COST % REFINE_ELEMENT_DOWNGRADE_YANG)

		def __Open(self, refine_type, material_pos, item_pos):
			if False == self.is_loaded:
				self.__LoadScript()

			self.refine_type = refine_type
			self.material_pos = material_pos
			self.item_pos = item_pos

			if app.WJ_ENABLE_TRADABLE_ICON:
				self.__SetMouseEventSlot(True)

			if refine_type == player.REFINE_ELEMENT_UPGRADE:
				self.__UpgradeUIOpen(material_pos, item_pos)
			elif refine_type == player.REFINE_ELEMENT_DOWNGRADE:
				self.__DowngradeUIOpen(material_pos, item_pos)

			self.__UpdateDialog()

			self.SetTop()
			self.Show()

		def __ClickAcceptButton(self):
			if self.refine_type == player.REFINE_ELEMENT_UPGRADE:
				if self.CanRefineElementUpgrade(player.INVENTORY, self.material_pos, player.INVENTORY, self.item_pos):
					net.SendRefineElementPacket(True)

			elif self.refine_type == player.REFINE_ELEMENT_DOWNGRADE:
				if self.CanRefineElementDowngrade(player.INVENTORY, self.material_pos, player.INVENTORY, self.item_pos):
					net.SendRefineElementPacket(True)

		def __CancelRefine(self):
			net.SendRefineElementPacket(False)
			self.__Clear()

		if app.WJ_ENABLE_TRADABLE_ICON:
			def __SetMouseEventSlot(self, is_cant_mouse_event):
				if not self.inven:
					return

				page = self.inven.GetInventoryPageIndex() # range 0 ~ 1
				min_range = page * player.INVENTORY_PAGE_SIZE
				max_range = (page + 1) * player.INVENTORY_PAGE_SIZE

				if self.material_pos >= 0:
					if (min_range) <= self.material_pos < (max_range): # range 0 ~ 44, 45 ~ 89
						lock_idx = self.material_pos - (min_range)
						if is_cant_mouse_event:
							self.inven.wndItem.SetCantMouseEventSlot(lock_idx)
						else:
							self.inven.wndItem.SetCanMouseEventSlot(lock_idx)

				if self.item_pos >= 0:
					if (min_range) <= self.item_pos < (max_range): # range 0 ~ 44, 45 ~ 89
						lock_idx = self.item_pos - (min_range)
						if is_cant_mouse_event:
							self.inven.wndItem.SetCantMouseEventSlot(lock_idx)
						else:
							self.inven.wndItem.SetCanMouseEventSlot(lock_idx)

			def SetInven(self, inven):
				self.inven = inven

			def OnUpdate(self):
				if not self.inven:
					return

				self.__SetMouseEventSlot(True)

		def Destroy(self):
			self.ClearDictionary()

			self.board = None
			self.title_bar = None
			self.title_name = None
			self.cost_text = None
			self.item_image = None
			self.slot_list = []

			self.tooltip = None
			self.popup_dlg = None
			if app.WJ_ENABLE_TRADABLE_ICON:
				self.inven = None

			self.__Clear()

		def OnPressEscapeKey(self):
			self.__CancelRefine()
			return True

		def RefineElementProcess(self, type, refine_type, data):
			if type == player.REFINE_ELEMENT_GC_OPEN:
				if refine_type != player.REFINE_ELEMENT_CHANGE:
					material_pos, item_pos = data
					self.__Open(refine_type, material_pos, item_pos)

			elif type == player.REFINE_ELEMENT_GC_RESULT:
				if refine_type == player.REFINE_ELEMENT_UPGRADE:
					if data:
						if self.popup_dlg:
							self.popup_dlg.SetText(localeInfo.REFINE_ELEMENT_UPGRADE_SUCCESS_TEXT)
							self.popup_dlg.Open()
					else:
						if self.popup_dlg:
							self.popup_dlg.SetText(localeInfo.REFINE_ELEMENT_UPGRADE_FAIL_TEXT)
							self.popup_dlg.Open()

				elif refine_type == player.REFINE_ELEMENT_DOWNGRADE and data:
					if self.popup_dlg:
						self.popup_dlg.SetText(localeInfo.REFINE_ELEMENT_DOWNGRADE_TEXT)
						self.popup_dlg.Open()

				self.__Clear()
