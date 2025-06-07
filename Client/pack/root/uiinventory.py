import ui
import player
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

if app.ENABLE_CHEQUE_SYSTEM:
	import uiToolTip
	import uiPickETC

ITEM_FLAG_APPLICABLE = 1 << 14

if app.ENABLE_EXTEND_INVEN_SYSTEM:
	EX_INVEN_COVER_IMG_OPEN = "d:/ymir work/ui/game/windows/ex_inven_cover_button_open.sub"
	EX_INVEN_COVER_IMG_CLOSE = "d:/ymir work/ui/game/windows/ex_inven_cover_button_close.sub"

if app.ENABLE_HIDE_COSTUME_SYSTEM:
	import chr

def ReprGetItemIndex(window):
	return lambda pos : player.GetItemIndex(window, pos)

def ReprGetItemCount(window):
	return lambda pos : player.GetItemCount(window, pos)

if app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
	class EquipmentWindow(ui.ScriptWindow):
		EQUIPMENT_CHANGE_COOLTIME = 3
		EXPANSION_CHANGE_HEIGHT_SIZE = 20

		def __init__(self):
			self.wnd_costume = None
			self.wnd_inventory = None
			self.wnd_belt = None

			ui.ScriptWindow.__init__(self)

			self.__Initialize()
			self.__LoadWindow()

		def __del__(self):
			ui.ScriptWindow.__del__(self)

		def __Initialize(self):
			self.is_loaded = 0

			self.interface = None

			self.current_page = 0
			self.tab_btn = []
			self.tab_img = []

		def __LoadWindow(self):
			if self.is_loaded == 1:
				return

			self.is_loaded = 1

			try:
				pyScrLoader = ui.PythonScriptLoader()
				pyScrLoader.LoadScriptFile(self, "UIScript/EquipmentDialog.py")
			except:
				import exception
				exception.Abort("CostumeWindow.LoadWindow.LoadObject")

			try:
				self.GetChild("board").SetTitleName(localeInfo.EQUIPMENT_TITLE_NAME)
				self.GetChild("board").SetCloseEvent(ui.__mem_func__(self.Close))

				self.equipment_slot = self.GetChild("equipment_slot")
				self.unique_slot = self.GetChild("unique_slot")

				self.dragon_soul_button = self.GetChild("dragon_soul_button")
				self.mall_button = self.GetChild("mall_button")
				self.premium_private_shop_button = self.GetChild("premium_private_shop_button")
				self.costume_button = self.GetChild("costume_button")

				self.tab_btn = []
				self.tab_btn.append(self.GetChild("tab_btn_01"))
				self.tab_btn.append(self.GetChild("tab_btn_02"))

				self.tab_img = []
				self.tab_img.append(self.GetChild("tab_img_01"))
				self.tab_img.append(self.GetChild("tab_img_02"))

				for btn in self.tab_btn:
					btn.Hide()

				for img in self.tab_img:
					img.Hide()
			except:
				import exception
				exception.Abort("EquipmentWindow.LoadWindow.BindObject")

			self.equipment_slot.SetSlotType(player.SLOT_TYPE_EQUIPMENT)
			self.unique_slot.SetSlotType(player.SLOT_TYPE_EQUIPMENT)

			self.dragon_soul_button.SetEvent(ui.__mem_func__(self.__ClickDSSButton))
			self.mall_button.SetEvent(ui.__mem_func__(self.__ClickMallButton))
			self.premium_private_shop_button.SetEvent(ui.__mem_func__(self.__ClickPremiumPrivateShopButton))
			self.costume_button.SetEvent(ui.__mem_func__(self.__ClickCostumeButton))

		def __CreateCostumeWindow(self):
			pass

		def BindInterface(self, interface):
			from _weakref import proxy
			self.interface = proxy(interface)

		def BindInventoryWindow(self, inventory_window):
			from _weakref import proxy
			self.wnd_inventory = proxy(inventory_window)

			x, y = self.wnd_inventory.GetGlobalPosition()
			self.SetPosition(x, y - self.GetHeight())

			self.equipment_slot.SetOverInItemEvent(ui.__mem_func__(self.wnd_inventory.OverInItem), player.EQUIPMENT)
			self.equipment_slot.SetOverOutItemEvent(ui.__mem_func__(self.wnd_inventory.OverOutItem))
			self.equipment_slot.SetUnselectItemSlotEvent(ui.__mem_func__(self.wnd_inventory.UseItemSlot), player.EQUIPMENT)
			self.equipment_slot.SetUseSlotEvent(ui.__mem_func__(self.wnd_inventory.UseItemSlot), player.EQUIPMENT)
			self.equipment_slot.SetSelectEmptySlotEvent(ui.__mem_func__(self.wnd_inventory.SelectEmptySlot), player.EQUIPMENT)
			self.equipment_slot.SetSelectItemSlotEvent(ui.__mem_func__(self.wnd_inventory.SelectItemSlot), player.EQUIPMENT)

			self.unique_slot.SetOverInItemEvent(ui.__mem_func__(self.wnd_inventory.OverInItem), player.EQUIPMENT)
			self.unique_slot.SetOverOutItemEvent(ui.__mem_func__(self.wnd_inventory.OverOutItem))
			self.unique_slot.SetUnselectItemSlotEvent(ui.__mem_func__(self.wnd_inventory.UseItemSlot), player.EQUIPMENT)
			self.unique_slot.SetUseSlotEvent(ui.__mem_func__(self.wnd_inventory.UseItemSlot), player.EQUIPMENT)
			self.unique_slot.SetSelectEmptySlotEvent(ui.__mem_func__(self.wnd_inventory.SelectEmptySlot), player.EQUIPMENT)
			self.unique_slot.SetSelectItemSlotEvent(ui.__mem_func__(self.wnd_inventory.SelectItemSlot), player.EQUIPMENT)

			self.wnd_costume = CostumeWindow(self.wnd_inventory)
			self.wnd_belt = BeltInventoryWindow(self.wnd_inventory)
			self.wnd_belt.SetMoveParentWindow(self)

		def __ClickDSSButton(self):
			self.interface.ToggleDragonSoulWindow()

		def __ClickMallButton(self):
			net.SendCommandPacket("/click_mall")

		def __ClickPremiumPrivateShopButton(self):
			pass

		def __ClickCostumeButton(self):
			if not self.wnd_costume:
				return

			if not self.wnd_costume.IsShow():
				self.wnd_costume.Show()
			else:
				self.wnd_costume.Close()

		def ShowCostumeInventory(self):
			if self.wnd_costume:
				self.wnd_costume.Show()

		def Show(self):
			self.__LoadWindow()

			if self.wnd_costume and self.wnd_costume.IsOpen():
				self.wnd_costume.Show()

			if self.wnd_belt:
				self.wnd_belt.Show()

			ui.ScriptWindow.Show(self)
			self.SetTop()

		def Hide(self):
			if self.wnd_costume:
				self.wnd_costume.Hide()

			if self.wnd_belt:
				self.wnd_belt.Hide()

			ui.ScriptWindow.Hide(self)

		def OnTop(self):
			(x, y) = self.GetLocalPosition()
			if y < 0:
				self.SetPosition(x, 0)

		def Close(self):
			self.Hide()

		def OnPressEscapeKey(self):
			self.Close()
			return True

		def Destroy(self):
			if self.wnd_costume:
				del self.wnd_costume
				self.wnd_costume = None

			if self.wnd_belt:
				del self.wnd_belt
				self.wnd_belt = None

		def RefreshSlot(self):
			self.__RefreshEquipmentSlot()
			self.__RefreshUniqueSlot()

			if self.wnd_belt:
				self.wnd_belt.RefreshSlot()

		def __RefreshEquipmentSlot(self):
			if not self.equipment_slot:
				return

			getItemVNum = ReprGetItemIndex(player.EQUIPMENT)
			getItemCount = ReprGetItemCount(player.EQUIPMENT)
			setItemVNum = self.equipment_slot.SetItemSlot

			for i in xrange(player.WEAR_MAX): ## 32 ( 0 ~ 31 )
				itemCount = getItemCount(i)
				if itemCount <= 1:
					itemCount = 0

				setItemVNum(i, getItemVNum(i), itemCount)

				if app.ENABLE_CHANGE_LOOK_SYSTEM:
					if not player.GetChangeLookVnum(player.EQUIPMENT, i) == 0:
						self.equipment_slot.SetSlotCoverImage(i, "icon/item/ingame_convert_Mark.tga")
					else:
						self.equipment_slot.EnableSlotCoverImage(i,False)

			self.equipment_slot.RefreshSlot()

			if self.wnd_costume:
				self.wnd_costume.RefreshCostumeSlot()

		def __RefreshUniqueSlot(self):
			if not self.unique_slot:
				return

			getItemVNum = ReprGetItemIndex(player.EQUIPMENT)
			getItemCount = ReprGetItemCount(player.EQUIPMENT)
			setItemVNum = self.unique_slot.SetItemSlot

			for i in xrange(player.WEAR_MAX):
				itemCount = getItemCount(i)
				if itemCount <= 1:
					itemCount = 0

				setItemVNum(i, getItemVNum(i), itemCount)

			self.unique_slot.RefreshSlot()

		def OnMoveWindow(self, x, y):
			if self.wnd_belt:
				self.wnd_belt.SetMoveParentWindow(self)

		def GetBeltWnd(self):
			return self.wnd_belt

		def __RequestChangeEquipmentpage(self, page_index): pass
		def __UseChangeEquipmentPageSkill(self, page_index): pass
		def AdditionalEquipmentPageSetting(self, page_index): pass
		def EquipmentPageTabSetting(self, page_index): pass
		def IsActiveAdditionalEquipmentPage(self): pass
		def ToggleEquipmentPage(self): pass

		if app.ENABLE_HIDE_COSTUME_SYSTEM:
			def SetHiddenCostumePart(self, part, hidden):
				if self.wnd_costume:
					self.wnd_costume.SetHiddenCostumePart(part, hidden)

class CostumeWindow(ui.ScriptWindow):
	def __init__(self, wndInventory):
		import exception

		if not wndInventory:
			exception.Abort("wndInventory parameter must be set to InventoryWindow")
			return

		ui.ScriptWindow.__init__(self)

		self.isLoaded = 0
		if app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
			self.isOpen = 0

		if app.ENABLE_HIDE_COSTUME_SYSTEM:
			self.slot_image_dict = {}
			self.slot_btn_dict = {}

			self.visible_wnd_dict = { item.COSTUME_TYPE_BODY : (62, 45), item.COSTUME_TYPE_HAIR : (62, 9), }
			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				self.visible_wnd_dict.update({ item.COSTUME_TYPE_ACCE : (70, 126) })
			if app.ENABLE_WEAPON_COSTUME_SYSTEM:
				self.visible_wnd_dict.update({ item.COSTUME_TYPE_WEAPON : (13, 13) })
			if app.ENABLE_AURA_COSTUME_SYSTEM:
				self.visible_wnd_dict.update({ item.COSTUME_TYPE_AURA : (37, 126) })

		self.wndInventory = wndInventory;

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		self.__LoadWindow()
		self.RefreshCostumeSlot()

		ui.ScriptWindow.Show(self)
		self.SetTop()
		if app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
			self.isOpen = 1

	if app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
		def IsOpen(self):
			return self.isOpen

	def Close(self):
		if app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
			self.isOpen = 0

		self.Hide()

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/CostumeWindow.py")
		except:
			import exception
			exception.Abort("CostumeWindow.LoadWindow.LoadObject")

		try:
			wndEquip = self.GetChild("CostumeSlot")
			if app.ENABLE_HIDE_COSTUME_SYSTEM:
				self.base_image = self.GetChild("Costume_Base")

			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))

		except:
			import exception
			exception.Abort("CostumeWindow.LoadWindow.BindObject")

		if app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
			x, y = self.wndInventory.GetGlobalPosition()
			self.SetPosition(x - 150, y - self.wndInventory.GetHeight() + 65)
			(x, y) = self.GetLocalPosition()
			if y < 0:
				self.SetPosition(x, 0)

		if app.ENABLE_HIDE_COSTUME_SYSTEM:
			## Slot Image List
			self.slot_image_dict = {}
			for key, (x, y) in self.visible_wnd_dict.items():
				slot_image = ui.ImageBox()
				slot_image.SetParent(self.base_image)
				slot_image.AddFlag("not_pick")
				slot_image.SetPosition(x + 4, y + 4)
				if key == item.COSTUME_TYPE_BODY:
					slot_image.LoadImage("d:/ymir work/ui/game/costume/hideslot_02.tga")
				elif app.ENABLE_WEAPON_COSTUME_SYSTEM and key == item.COSTUME_TYPE_WEAPON:
					slot_image.LoadImage("d:/ymir work/ui/game/costume/hideslot_03.tga")
				else:
					slot_image.LoadImage("d:/ymir work/ui/game/costume/hideslot_01.tga")
				slot_image.Hide()
				self.slot_image_dict[key] = slot_image

			## Slot Button List
			self.slot_btn_dict = {}
			for key, (x, y) in self.visible_wnd_dict.items():
				btn = ui.Button()
				btn.SetParent(self.base_image)
				btn.SetPosition(x, y)
				btn.SetUpVisual("d:/ymir work/ui/game/costume/eye_normal_01.tga")
				btn.SetOverVisual("d:/ymir work/ui/game/costume/eye_normal_02.tga")
				btn.SetDownVisual("d:/ymir work/ui/game/costume/eye_normal_01.tga")
				btn.SetToolTipText(localeInfo.HIDE_COSTUME_TOOLTIP)
				btn.SetEvent(ui.__mem_func__(self.__ClickVisibleButton), key, 1)
				btn.Show()
				self.slot_btn_dict[key] = btn

		## Equipment
		wndEquip.SetOverInItemEvent(ui.__mem_func__(self.wndInventory.OverInItem), player.EQUIPMENT)
		wndEquip.SetOverOutItemEvent(ui.__mem_func__(self.wndInventory.OverOutItem))
		wndEquip.SetUnselectItemSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot), player.EQUIPMENT)
		wndEquip.SetUseSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot), player.EQUIPMENT)
		wndEquip.SetSelectEmptySlotEvent(ui.__mem_func__(self.wndInventory.SelectEmptySlot), player.EQUIPMENT)
		wndEquip.SetSelectItemSlotEvent(ui.__mem_func__(self.wndInventory.SelectItemSlot), player.EQUIPMENT)

		self.wndEquip = wndEquip

	def RefreshCostumeSlot(self):
		getItemVNum = ReprGetItemIndex(player.EQUIPMENT)

		for i in xrange(item.COSTUME_SLOT_COUNT):
			slotNumber = item.COSTUME_SLOT_START + i
			self.wndEquip.SetItemSlot(slotNumber, getItemVNum(slotNumber), 0)

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				if not player.GetChangeLookVnum(player.EQUIPMENT, slotNumber) == 0:
					self.wndEquip.SetSlotCoverImage(slotNumber, "icon/item/ingame_convert_Mark.tga")
				else:
					self.wndEquip.EnableSlotCoverImage(slotNumber, False)

		if app.ENABLE_WEAPON_COSTUME_SYSTEM:
			self.wndEquip.SetItemSlot(item.COSTUME_SLOT_WEAPON, getItemVNum(item.COSTUME_SLOT_WEAPON), 0)

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				if not player.GetChangeLookVnum(player.EQUIPMENT, item.COSTUME_SLOT_WEAPON) == 0:
					self.wndEquip.SetSlotCoverImage(item.COSTUME_SLOT_WEAPON, "icon/item/ingame_convert_Mark.tga")
				else:
					self.wndEquip.EnableSlotCoverImage(item.COSTUME_SLOT_WEAPON, False)

		self.wndEquip.RefreshSlot()

	if app.ENABLE_HIDE_COSTUME_SYSTEM:
		def __ClickVisibleButton(self, part, hidden):
			if part in self.visible_wnd_dict:
				net.SendCommandPacket("/hide_costume_part %d %d" % (part, hidden))

		def SetHiddenCostumePart(self, part, hidden):
			if not part in self.visible_wnd_dict:
				return

			btn = self.slot_btn_dict[part]
			if hidden:
				btn.SetUpVisual("d:/ymir work/ui/game/costume/eye_closed_01.tga")
				btn.SetOverVisual("d:/ymir work/ui/game/costume/eye_closed_02.tga")
				btn.SetDownVisual("d:/ymir work/ui/game/costume/eye_closed_01.tga")
				btn.SetToolTipText(localeInfo.SHOW_COSTUME_TOOLTIP)
				btn.SetEvent(ui.__mem_func__(self.__ClickVisibleButton), part, 0)
				self.slot_image_dict[part].Show()
			else:
				btn.SetUpVisual("d:/ymir work/ui/game/costume/eye_normal_01.tga")
				btn.SetOverVisual("d:/ymir work/ui/game/costume/eye_normal_02.tga")
				btn.SetDownVisual("d:/ymir work/ui/game/costume/eye_normal_01.tga")
				btn.SetToolTipText(localeInfo.HIDE_COSTUME_TOOLTIP)
				btn.SetEvent(ui.__mem_func__(self.__ClickVisibleButton), part, 1)
				self.slot_image_dict[part].Hide()

class BeltInventoryWindow(ui.ScriptWindow):
	def __init__(self, wndInventory):
		import exception

		if not wndInventory:
			exception.Abort("wndInventory parameter must be set to InventoryWindow")
			return

		ui.ScriptWindow.__init__(self)

		self.isLoaded = 0
		self.wndInventory = wndInventory;

		self.wndBeltInventoryLayer = None
		self.wndBeltInventorySlot = None
		self.expandBtn = None
		self.minBtn = None

		if app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
			self.parent = None

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self, openBeltSlot = False):
		self.__LoadWindow()
		self.RefreshSlot()

		ui.ScriptWindow.Show(self)

		if openBeltSlot:
			self.OpenInventory()
		else:
			self.CloseInventory()

		self.SetTop()

	def Close(self):
		self.Hide()

	def IsOpeningInventory(self):
		return self.wndBeltInventoryLayer.IsShow()

	def OpenInventory(self):
		self.wndBeltInventoryLayer.Show()
		self.expandBtn.Hide()

		self.AdjustPosition()

	def CloseInventory(self):
		self.wndBeltInventoryLayer.Hide()
		self.expandBtn.Show()

		self.AdjustPosition()

	## 현재 인벤토리 위치를 기준으로 BASE 위치를 계산, 리턴.. 숫자 하드코딩하기 정말 싫지만 방법이 없다..
	def GetBasePosition(self):
		if app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
			x, y = self.parent.GetGlobalPosition()
			return x - 148, y + 65
		else:
			x, y = self.wndInventory.GetGlobalPosition()
			return x - 148, y + 241

	def AdjustPosition(self):
		bx, by = self.GetBasePosition()

		if self.IsOpeningInventory():
			self.SetPosition(bx, by)

			if localeInfo.IsARABIC():
				self.SetPosition(bx, by)
				self.wndBeltInventoryLayer.SetPosition(self.ORIGINAL_WIDTH - 5, 0)
			else:
				self.SetPosition(bx, by)
				self.SetSize(self.ORIGINAL_WIDTH, self.GetHeight())
		else:
			if localeInfo.IsARABIC():
				self.SetPosition(bx + 140 , by)
				self.wndBeltInventoryLayer.SetPosition(self.ORIGINAL_WIDTH - 10, 0)
				self.expandBtn.SetPosition(self.ORIGINAL_WIDTH + 1 , 15)
			else:
				self.SetPosition(bx + 138, by);
				self.SetSize(10, self.GetHeight())

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/BeltInventoryWindow.py")
		except:
			import exception
			exception.Abort("BeltInventoryWindow.LoadWindow.LoadObject")

		try:
			self.ORIGINAL_WIDTH = self.GetWidth()
			wndBeltInventorySlot = self.GetChild("BeltInventorySlot")
			self.wndBeltInventoryLayer = self.GetChild("BeltInventoryLayer")
			self.expandBtn = self.GetChild("ExpandBtn")
			self.minBtn = self.GetChild("MinimizeBtn")

			self.expandBtn.SetEvent(ui.__mem_func__(self.OpenInventory))
			self.minBtn.SetEvent(ui.__mem_func__(self.CloseInventory))

			if localeInfo.IsARABIC() :
				self.minBtn.SetPosition(self.minBtn.GetWidth() + 3, 15)

			for i in xrange(item.BELT_INVENTORY_SLOT_COUNT):
				slotNumber = item.BELT_INVENTORY_SLOT_START + i
				wndBeltInventorySlot.SetCoverButton(slotNumber,
					"d:/ymir work/ui/game/quest/slot_button_01.sub",\
					"d:/ymir work/ui/game/quest/slot_button_01.sub",\
					"d:/ymir work/ui/game/quest/slot_button_01.sub",\
					"d:/ymir work/ui/game/belt_inventory/slot_disabled.tga", False, False)
		except:
			import exception
			exception.Abort("CostumeWindow.LoadWindow.BindObject")

		## Equipment
		wndBeltInventorySlot.SetOverInItemEvent(ui.__mem_func__(self.wndInventory.OverInItem), player.BELT_INVENTORY)
		wndBeltInventorySlot.SetOverOutItemEvent(ui.__mem_func__(self.wndInventory.OverOutItem))
		wndBeltInventorySlot.SetUnselectItemSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot), player.BELT_INVENTORY)
		wndBeltInventorySlot.SetUseSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot), player.BELT_INVENTORY)
		wndBeltInventorySlot.SetSelectEmptySlotEvent(ui.__mem_func__(self.wndInventory.SelectEmptySlot), player.BELT_INVENTORY)
		wndBeltInventorySlot.SetSelectItemSlotEvent(ui.__mem_func__(self.wndInventory.SelectItemSlot), player.BELT_INVENTORY)

		## MT-436 slot bug fix
		wndBeltInventorySlot.SetSlotType(player.SLOT_TYPE_BELT_INVENTORY)
		self.wndBeltInventorySlot = wndBeltInventorySlot

	def RefreshSlot(self):
		getItemVNum = ReprGetItemIndex(player.BELT_INVENTORY)
		getItemCount = ReprGetItemCount(player.BELT_INVENTORY)

		for i in xrange(item.BELT_INVENTORY_SLOT_COUNT):
			slotNumber = item.BELT_INVENTORY_SLOT_START + i
			self.wndBeltInventorySlot.SetItemSlot(slotNumber, getItemVNum(slotNumber), getItemCount(slotNumber))
			self.wndBeltInventorySlot.SetAlwaysRenderCoverButton(slotNumber, True)

			if player.IsAvailableBeltInventoryCell(slotNumber):
				self.wndBeltInventorySlot.EnableCoverButton(slotNumber)
			else:
				self.wndBeltInventorySlot.DisableCoverButton(slotNumber)

		self.wndBeltInventorySlot.RefreshSlot()

	def OnTop(self):
		if app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
			pass
		else:
			if self.wndInventory:
				self.wndInventory.SetClickBeltInventory(True)
				self.wndInventory.SetTop()

	if app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
		def SetMoveParentWindow(self, move_window):
			from _weakref import proxy
			self.parent = proxy(move_window)
			self.AdjustPosition()

class InventoryWindow(ui.ScriptWindow):
	USE_TYPE_TUPLE = (
		"USE_CLEAN_SOCKET",
		"USE_CHANGE_ATTRIBUTE",
		"USE_ADD_ATTRIBUTE",
		"USE_ADD_ATTRIBUTE2",
		"USE_ADD_ACCESSORY_SOCKET",
		"USE_PUT_INTO_ACCESSORY_SOCKET",
		"USE_PUT_INTO_BELT_SOCKET",
		"USE_PUT_INTO_RING_SOCKET"
	)

	if app.ENABLE_MOVE_COSTUME_ATTR:
		USE_TYPE_TUPLE += ("USE_CHANGE_COSTUME_ATTR", "USE_RESET_COSTUME_ATTR",)

	if app.ENABLE_CHANGED_ATTR:
		USE_TYPE_TUPLE += ("USE_SELECT_ATTRIBUTE",)

	if app.ENABLE_ATTR_6TH_7TH:
		USE_TYPE_TUPLE += ("USE_CHANGE_ATTRIBUTE2",)

	if app.ENABLE_REFINE_ELEMENT_SYSTEM:
		USE_TYPE_TUPLE += ("USE_ELEMENT_UPGRADE", "USE_ELEMENT_DOWNGRADE", "USE_ELEMENT_CHANGE",)

	questionDialog = None
	tooltipItem = None

	if not app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
		wndCostume = None
		wndBelt = None

	dlgPickMoney = None
	if app.ENABLE_CHEQUE_SYSTEM:
		dlgPickETC = None

	sellingSlotNumber = -1
	sellingSlotWindow = player.INVENTORY

	if not app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
		isOpenedCostumeWindowWhenClosingInventory = 0 # 인벤토리 닫을 때 코스츔이 열려있었는지 여부-_-; 네이밍 ㅈㅅ
		isOpenedBeltWindowWhenClosingInventory = 0 # 인벤토리 닫을 때 벨트 인벤토리가 열려있었는지 여부-_-; 네이밍 ㅈㅅ

	isLoaded = 0
	interface = None
	pop = None

	def __init__(self):
		if app.ENABLE_GEM_SYSTEM:
			self.wndExpandedMoneyBar = None
			self.wndGem = None

		if app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
			self.wndEquipment = None

		ui.ScriptWindow.__init__(self)

		self.isOpenedBeltWindowWhenClosingInventory = 0 # 인벤토리 닫을 때 벨트 인벤토리가 열려있었는지 여부-_-; 네이밍 ㅈㅅ

		self.__LoadWindow()

		self.IsClickBeltInventory = False

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		if app.ENABLE_GEM_SYSTEM:
			self.wndExpandedMoneyBar = None
			self.wndGem = None

		if app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
			self.wndEquipment = None

	def SetClickBeltInventory(self, isclick):
		self.IsClickBeltInventory = isclick

	def Show(self):
		self.__LoadWindow()

		ui.ScriptWindow.Show(self)

		self.RefreshItemSlot()
		self.RefreshStatus()

		if not app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
			# 인벤토리를 닫을 때 코스츔이 열려있었다면 인벤토리를 열 때 코스츔도 같이 열도록 함.
			if self.isOpenedCostumeWindowWhenClosingInventory and self.wndCostume:
				self.wndCostume.Show()

			# 인벤토리를 닫을 때 벨트 인벤토리가 열려있었다면 같이 열도록 함.
			if self.wndBelt:
				self.wndBelt.Show(self.isOpenedBeltWindowWhenClosingInventory)

		if app.ENABLE_GEM_SYSTEM:
			if self.wndExpandedMoneyBar:
				self.wndExpandedMoneyBar.Show()

		if app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
			if self.wndEquipment:
				self.wndEquipment.Show()

	def BindInterfaceClass(self, interface):
		from _weakref import proxy
		self.interface = proxy(interface)

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, uiScriptLocale.LOCALE_COMMON_UISCRIPT_PATH + "InventoryWindow.py")
		except:
			import exception
			exception.Abort("InventoryWindow.LoadWindow.LoadObject")

		try:
			wndItem = self.GetChild("ItemSlot")
			if not app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
				wndEquip = self.GetChild("EquipmentSlot")
			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
			self.wndMoney = self.GetChild("Money")
			self.wndMoneySlot = self.GetChild("Money_Slot")
			if not app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
				self.mallButton = self.GetChild2("MallButton")
				self.DSSButton = self.GetChild2("DSSButton")
				self.costumeButton = self.GetChild2("CostumeButton")

			if app.ENABLE_CHEQUE_SYSTEM:
				self.wndCheque = self.GetChild("Cheque")
				self.wndChequeSlot = self.GetChild("Cheque_Slot")

				if app.ENABLE_GEM_SYSTEM:
					self.wndMoneyIcon = self.GetChild("Money_Icon")
					self.wndChequeIcon = self.GetChild("Cheque_Icon")
					self.wndMoneyIcon.Hide()
					self.wndMoneySlot.Hide()
					self.wndChequeIcon.Hide()
					self.wndChequeSlot.Hide()

					## 높이 조절
					height = self.GetHeight()
					width = self.GetWidth()
					self.SetSize(width, height - 22)
					self.GetChild("board").SetSize(width, height - 22)

				else:
					self.wndMoneyIcon = self.GetChild("Money_Icon")
					self.wndChequeIcon = self.GetChild("Cheque_Icon")

					self.wndMoneyIcon.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_in", 0)
					self.wndChequeIcon.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_in", 1)

					self.wndMoneyIcon.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_out", 0)
					self.wndChequeIcon.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_out", 1)

					self.toolTip = uiToolTip.ToolTip()
					self.toolTip.ClearToolTip()

			self.inventoryTab = []
			self.inventoryTab.append(self.GetChild("Inventory_Tab_01"))
			self.inventoryTab.append(self.GetChild("Inventory_Tab_02"))
			self.inventoryTab.append(self.GetChild("Inventory_Tab_03"))
			self.inventoryTab.append(self.GetChild("Inventory_Tab_04"))

			if not app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
				self.equipmentTab = []
				self.equipmentTab.append(self.GetChild("Equipment_Tab_01"))
				self.equipmentTab.append(self.GetChild("Equipment_Tab_02"))

				# Belt Inventory Window
				self.wndBelt = None

			self.dlgQuestion = uiCommon.QuestionDialog2()
			self.dlgQuestion.Close()

			if not app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
				self.wndBelt = BeltInventoryWindow(self)

			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				self.listHighlightedAcceSlot = []

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				self.listHighlightedChangeLookSlot = []

			if app.ENABLE_AURA_COSTUME_SYSTEM:
				self.listHighlightedAuraSlot = []

			if app.ENABLE_CUBE_RENEWAL:
				self.listHighlightedCubeSlot = []

			if app.WJ_ENABLE_PICKUP_ITEM_EFFECT:
				self.listHighlightedSlot = []

			if app.ENABLE_EXTEND_INVEN_SYSTEM:
				self.ExInvenQuestionDlg = uiCommon.QuestionDialog()
				self.ExInvenQuestionDlg.Close()

			if app.ENABLE_AURA_COSTUME_SYSTEM:
				self.clear_aura_item_question_dialog = None

			if app.ENABLE_GEM_CONVERTER:
				self.gem_converter_question_dialog = None
				self.gem_converter_item_pos = (-1, -1)
		except:
			import exception
			exception.Abort("InventoryWindow.LoadWindow.BindObject")

		## Item
		wndItem.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot), player.INVENTORY)
		wndItem.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot), player.INVENTORY)
		wndItem.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot), player.INVENTORY)
		wndItem.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot), player.INVENTORY)
		wndItem.SetOverInItemEvent(ui.__mem_func__(self.OverInItem), player.INVENTORY)
		wndItem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

		if not app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
			## Equipment
			wndEquip.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot), player.EQUIPMENT)
			wndEquip.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot), player.EQUIPMENT)
			wndEquip.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot), player.EQUIPMENT)
			wndEquip.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot), player.EQUIPMENT)
			wndEquip.SetOverInItemEvent(ui.__mem_func__(self.OverInItem), player.EQUIPMENT)
			wndEquip.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

		## PickMoneyDialog
		dlgPickMoney = uiPickMoney.PickMoneyDialog()
		dlgPickMoney.LoadDialog()
		dlgPickMoney.Hide()

		## PickETCDialog
		if app.ENABLE_CHEQUE_SYSTEM:
			dlgPickETC = uiPickETC.PickETCDialog()
			dlgPickETC.LoadDialog()
			dlgPickETC.Hide()
			self.dlgPickETC = dlgPickETC

		## RefineDialog
		self.refineDialog = uiRefine.RefineDialog()
		self.refineDialog.Hide()

		## AttachMetinDialog
		self.attachMetinDialog = uiAttachMetin.AttachMetinDialog()
		self.attachMetinDialog.Hide()

		## MoneySlot
		if app.ENABLE_CHEQUE_SYSTEM:
			self.wndMoneySlot.SetEvent(ui.__mem_func__(self.OpenPickMoneyDialog), 0)
			self.wndChequeSlot.SetEvent(ui.__mem_func__(self.OpenPickMoneyDialog), 1)
		else:
			self.wndMoneySlot.SetEvent(ui.__mem_func__(self.OpenPickMoneyDialog))

		for inven_index in range(player.INVENTORY_PAGE_COUNT): ## 0, 1, 2, 3
			self.inventoryTab[inven_index].SetEvent(lambda arg = inven_index : self.SetInventoryPage(arg))
		self.inventoryTab[0].Down()

		if not app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
			self.equipmentTab[0].SetEvent(lambda arg = 0: self.SetEquipmentPage(arg))
			self.equipmentTab[1].SetEvent(lambda arg = 1: self.SetEquipmentPage(arg))
			self.equipmentTab[0].Down()
			self.equipmentTab[0].Hide()
			self.equipmentTab[1].Hide()

		## MT-436 slot bug fix
		wndItem.SetSlotType(player.SLOT_TYPE_INVENTORY)
		if not app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
			wndEquip.SetSlotType(player.SLOT_TYPE_EQUIPMENT)

		self.wndItem = wndItem
		if not app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
			self.wndEquip = wndEquip
		self.dlgPickMoney = dlgPickMoney

		if not app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
			# MallButton
			if self.mallButton:
				self.mallButton.SetEvent(ui.__mem_func__(self.ClickMallButton))

			if self.DSSButton:
				self.DSSButton.SetEvent(ui.__mem_func__(self.ClickDSSButton))

			# Costume Button
			if self.costumeButton:
				self.costumeButton.SetEvent(ui.__mem_func__(self.ClickCostumeButton))

			self.wndCostume = None

		if app.ENABLE_EXTEND_INVEN_SYSTEM:
			## Extend Inven Button
			self.__CreateExtendInvenButton()

		## Refresh
		self.SetInventoryPage(0)
		if not app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
			self.SetEquipmentPage(0)
		self.RefreshItemSlot()
		self.RefreshStatus()

	def Destroy(self):
		self.ClearDictionary()

		self.dlgPickMoney.Destroy()
		self.dlgPickMoney = 0

		if app.ENABLE_CHEQUE_SYSTEM:
			self.dlgPickETC.Destroy()
			self.dlgPickETC = 0

		self.refineDialog.Destroy()
		self.refineDialog = 0

		self.attachMetinDialog.Destroy()
		self.attachMetinDialog = 0

		self.tooltipItem = None
		self.wndItem = 0
		if not app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
			self.wndEquip = 0
		self.dlgPickMoney = 0
		self.wndMoney = 0
		self.wndMoneySlot = 0
		self.questionDialog = None
		self.mallButton = None
		self.DSSButton = None
		self.interface = None

		if app.ENABLE_CHEQUE_SYSTEM:
			self.wndCheque = 0
			self.wndChequeSlot = 0
			self.dlgPickETC = 0

		if not app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
			if self.wndCostume:
				self.wndCostume.Destroy()
				self.wndCostume = 0

			if self.wndBelt:
				self.wndBelt.Destroy()
				self.wndBelt = None

		self.inventoryTab = []
		if not app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
			self.equipmentTab = []

		if app.ENABLE_EXTEND_INVEN_SYSTEM:
			self.ExInvenButton = []

			if self.ExInvenQuestionDlg:
				self.ExInvenQuestionDlg.Close()

		if app.ENABLE_AURA_COSTUME_SYSTEM:
			if self.clear_aura_item_question_dialog:
				self.clear_aura_item_question_dialog.Close()
			self.clear_aura_item_question_dialog = None

		if app.ENABLE_GEM_CONVERTER:
			if self.gem_converter_question_dialog:
				self.gem_converter_question_dialog.Close()
			self.gem_converter_question_dialog = None

		if self.pop:
			self.pop.Destroy()
			self.pop = None

		if app.ENABLE_GEM_SYSTEM:
			self.wndExpandedMoneyBar = None

	def Hide(self):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS():
			self.OnCloseQuestionDialog()
			return

		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()

		if not app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
			if self.wndCostume:
				self.isOpenedCostumeWindowWhenClosingInventory = self.wndCostume.IsShow() # 인벤토리 창이 닫힐 때 코스츔이 열려 있었는가?
				self.wndCostume.Close()

			if self.wndBelt:
				self.isOpenedBeltWindowWhenClosingInventory = self.wndBelt.IsOpeningInventory() # 인벤토리 창이 닫힐 때 벨트 인벤토리도 열려 있었는가?
				#print "Is Opening Belt Inven?? ", self.isOpenedBeltWindowWhenClosingInventory
				self.wndBelt.Close()

		if self.dlgPickMoney:
			self.dlgPickMoney.Close()

		if app.ENABLE_CHEQUE_SYSTEM:
			if self.dlgPickETC:
				self.dlgPickETC.Close()

		if app.ENABLE_GEM_SYSTEM:
			if self.wndExpandedMoneyBar:
				self.wndExpandedMoneyBar.Close()

		wndMgr.Hide(self.hWnd)

	def Close(self):
		self.Hide()

	if app.ENABLE_GEM_SYSTEM:
		def SetExpandedMoneyBar(self, wndBar):
			self.wndExpandedMoneyBar = wndBar

			if self.wndExpandedMoneyBar:
				self.wndMoneySlot = self.wndExpandedMoneyBar.GetMoneySlot()
				self.wndMoney = self.wndExpandedMoneyBar.GetMoney()

				if app.ENABLE_CHEQUE_SYSTEM:
					## 양 관련
					self.wndMoneyIcon = self.wndExpandedMoneyBar.GetMoneyIcon()
					if self.wndMoneyIcon:
						self.wndMoneyIcon.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_in", 0)
						self.wndMoneyIcon.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_out", 0)
					if self.wndMoneySlot:
						self.wndMoneySlot.SetEvent(ui.__mem_func__(self.OpenPickMoneyDialog), 0)

					## 2차화폐 관련
					self.wndChequeIcon = self.wndExpandedMoneyBar.GetChequeIcon()
					if self.wndChequeIcon:
						self.wndChequeIcon.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_in", 1)
						self.wndChequeIcon.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_out", 1)
					self.wndChequeSlot = self.wndExpandedMoneyBar.GetChequeSlot()
					if self.wndChequeSlot:
						self.wndChequeSlot.SetEvent(ui.__mem_func__(self.OpenPickMoneyDialog), 1)
					self.wndCheque = self.wndExpandedMoneyBar.GetCheque()

					## 보석 관련
					self.wndGemIcon = self.wndExpandedMoneyBar.GetGemIcon()
					if self.wndGemIcon:
						self.wndGemIcon.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_in", 2)
						self.wndGemIcon.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_out", 2)
					self.wndGem = self.wndExpandedMoneyBar.GetGem()

					self.toolTip = uiToolTip.ToolTip()
					self.toolTip.ClearToolTip()
				else:
					if self.wndMoneySlot:
						self.wndMoneySlot.SetEvent(ui.__mem_func__(self.OpenPickMoneyDialog))

	def SetInventoryPage(self, page):
		self.inventoryPageIndex = page

		for index in range(len(self.inventoryTab)):
			if index == page:
				continue

			self.inventoryTab[index].SetUp()

		self.RefreshBagSlotWindow()

	if app.ENABLE_EXTEND_INVEN_SYSTEM:
		## Extend Inven Button
		def __CreateExtendInvenButton(self):
			parent = self.GetChild("board")

			self.ExInvenButton = []
			start_x = 8
			start_y = 246
			if app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
				start_y = 55
			img_height = 32

			for button_index in range(player.EX_INVENTORY_STAGE_MAX): ## 18
				ex_inven_button = ui.Button()

				increase_y = img_height * (button_index % (player.EX_INVENTORY_STAGE_MAX / player.EX_INVENTORY_PAGE_COUNT))

				ex_inven_button.SetParent(parent)
				ex_inven_button.SetPosition(start_x, start_y + increase_y)
				ex_inven_button.SetUpVisual(EX_INVEN_COVER_IMG_CLOSE)
				ex_inven_button.SetOverVisual(EX_INVEN_COVER_IMG_CLOSE)
				ex_inven_button.SetDownVisual(EX_INVEN_COVER_IMG_CLOSE)
				ex_inven_button.SetDisableVisual(EX_INVEN_COVER_IMG_CLOSE)
				ex_inven_button.SetEvent(ui.__mem_func__(self.__ClickExtendInvenButton), button_index + 1)
				ex_inven_button.Hide()
				self.ExInvenButton.append(ex_inven_button)

		def __ClickExtendInvenButton(self, index):
			net.SendExtendInvenButtonClick(index)

		def __HideAllExtendInvenButton(self):
			for index in range(len(self.ExInvenButton)):
				self.ExInvenButton[index].Hide()

		def __ShowExtendInvenButton(self, cur_stage):
			## self.inventoryPageIndex 는 0,1,2,3 라서 EX_INVENTORY_PAGE_START -1 을 사용한다.
			if self.inventoryPageIndex < player.EX_INVENTORY_PAGE_START - 1:
				return

			## 한 페이지 당 보여줄 단계수
			count = player.EX_INVENTORY_STAGE_MAX / player.EX_INVENTORY_PAGE_COUNT

			## 최소, 최대
			min_range = (self.inventoryPageIndex - player.EX_INVENTORY_PAGE_START + 1) * count
			max_range = min_range + count

			for button_index in range(min_range, max_range):
				if button_index == cur_stage:
					self.ExInvenButton[button_index].SetUpVisual(EX_INVEN_COVER_IMG_OPEN)
					self.ExInvenButton[button_index].SetOverVisual(EX_INVEN_COVER_IMG_OPEN)
					self.ExInvenButton[button_index].SetDownVisual(EX_INVEN_COVER_IMG_OPEN)
					self.ExInvenButton[button_index].SetDisableVisual(EX_INVEN_COVER_IMG_OPEN)

				if button_index < cur_stage:
					self.ExInvenButton[button_index].Hide()
				else:
					self.ExInvenButton[button_index].Show()

		def ExInvenItemUseMsg(self, item_vnum, msg, enough_count):
			## 개수 부족
			if player.EX_INVEN_FAIL_FALL_SHORT == msg:
				self.__OpenExInvenFallShortCountMsgDlg(enough_count)

			## 개수가 충분함
			elif player.EX_INVEN_SUCCESS == msg:
				self.__OpenExInvenQuestionDlg(enough_count)

			## 4번페이지 마지막임
			elif player.EX_INVEN_FAIL_FOURTH_PAGE_STAGE_MAX == msg:
				self.__OpenExInvenMsgDlg(localeInfo.EXINVEN_USE_ITEM_FAIL_FOURTH_PAGE_STAGE_MAX)
				return

			else:
				return

		def __OpenExInvenMsgDlg(self, msg):
			popup = uiCommon.PopupDialog()
			popup.SetText(msg)
			popup.SetAcceptEvent(self.__OnClosePopupDialog)
			w, h = popup.GetTextSize()
			popup.SetWidth(w + 40)
			line_count = popup.GetTextLineCount()

			if line_count > 1:
				height = popup.GetLineHeight()
				popup.SetLineHeight(height + 3)

			popup.Open()

			if self.pop:
				self.pop.Destroy()

			self.pop = popup

		def __OpenExInvenFallShortCountMsgDlg(self, enough_count):
			popup = uiCommon.PopupDialog()
			popup.SetText(localeInfo.EXINVEN_USE_ITEM_FAIL_FALL_SHORT % (enough_count))
			popup.SetAcceptEvent(self.__OnClosePopupDialog)
			popup.Open()

			if self.pop:
				self.pop.Destroy()

			self.pop = popup

		def __OpenExInvenQuestionDlg(self, enough_count):
			if self.ExInvenQuestionDlg:
				self.ExInvenQuestionDlg.SetText(localeInfo.EXINVEN_USE_ITEM_QUESTION % enough_count)
				self.ExInvenQuestionDlg.SetAcceptEvent(ui.__mem_func__(self.__AcceptExInvenItemUse))
				self.ExInvenQuestionDlg.SetCancelEvent(ui.__mem_func__(self.__CancelExInvenItemUse))
				w, h = self.ExInvenQuestionDlg.GetTextSize()
				self.ExInvenQuestionDlg.SetWidth(w + 40)
				self.ExInvenQuestionDlg.Open()

		def __AcceptExInvenItemUse(self):
			net.SendExtendInvenUpgrade()
			self.ExInvenQuestionDlg.Close()

		def __CancelExInvenItemUse(self):
			self.ExInvenQuestionDlg.Close()

	if not app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
		def SetEquipmentPage(self, page):
			self.equipmentPageIndex = page
			self.equipmentTab[1 - page].SetUp()
			self.RefreshEquipSlotWindow()

		def ClickMallButton(self):
			net.SendCommandPacket("/click_mall")

		# DSSButton
		def ClickDSSButton(self):
			self.interface.ToggleDragonSoulWindow()

		def ClickCostumeButton(self):
			if self.wndCostume:
				if self.wndCostume.IsShow():
					self.wndCostume.Hide()
				else:
					self.wndCostume.Show()
			else:
				self.wndCostume = CostumeWindow(self)
				self.wndCostume.Show()

		def ShowCostumeInventory(self):
			if self.wndCostume:
				if not self.wndCostume.IsShow():
					self.wndCostume.Show()
			else:
				self.wndCostume = CostumeWindow(self)
				self.wndCostume.Show()

	def OpenPickMoneyDialog(self, focus_idx = 0):
		if mouseModule.mouseController.isAttached():
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			if player.SLOT_TYPE_SAFEBOX == mouseModule.mouseController.GetAttachedType():
				if player.ITEM_MONEY == mouseModule.mouseController.GetAttachedItemIndex():
					net.SendSafeboxWithdrawMoneyPacket(mouseModule.mouseController.GetAttachedItemCount())
					snd.PlaySound("sound/ui/money.wav")

			mouseModule.mouseController.DeattachObject()
		else:
			curMoney = player.GetElk()
			curCheque = player.GetCheque()

			if curMoney <= 0 and curCheque <= 0:
				return

			self.dlgPickMoney.SetTitleName(localeInfo.PICK_MONEY_TITLE)
			self.dlgPickMoney.SetAcceptEvent(ui.__mem_func__(self.OnPickMoney))
			self.dlgPickMoney.Open(curMoney, curCheque)
			self.dlgPickMoney.SetMax(7) # 인벤토리 990000 제한 버그 수정
			if app.ENABLE_CHEQUE_SYSTEM:
				self.dlgPickMoney.SetFocus(focus_idx)

	if app.ENABLE_CHEQUE_SYSTEM:
		def OnPickMoney(self, money, cheque):
			mouseModule.mouseController.AttachMoney(self, player.SLOT_TYPE_INVENTORY, money, cheque)
	else:
		def OnPickMoney(self, money):
			mouseModule.mouseController.AttachMoney(self, player.SLOT_TYPE_INVENTORY, money)

	def OnPickItem(self, count):
		if app.ENABLE_CHEQUE_SYSTEM:
			itemSlotWindow = self.dlgPickETC.itemGlobalSlotWindow
			itemSlotIndex = self.dlgPickETC.itemGlobalSlotIndex
		else:
			itemSlotWindow = self.dlgPickMoney.itemGlobalSlotWindow
			itemSlotIndex = self.dlgPickMoney.itemGlobalSlotIndex

		selectedItemVNum = player.GetItemIndex(itemSlotWindow, itemSlotIndex)
		slotType = player.WindowTypeToSlotType(itemSlotWindow)
		mouseModule.mouseController.AttachObject(self, slotType, itemSlotIndex, selectedItemVNum, count)

	def __InventoryLocalSlotPosToGlobalSlotPos(self, local, window_type = player.INVENTORY):
		if player.INVENTORY == window_type:
			return self.inventoryPageIndex * player.INVENTORY_PAGE_SIZE + local
		else:
			return local

	def RefreshBagSlotWindow(self):
		if not self.wndItem:
			return

		getItemVNum = player.GetItemIndex
		getItemCount = player.GetItemCount
		setItemVNum = self.wndItem.SetItemSlot

		## 리프레쉬 하기전에 하이라이트 모두 끈다.
		for i in xrange(self.wndItem.GetSlotCount()):
			self.wndItem.DeactivateSlot(i)

		if app.WJ_ENABLE_TRADABLE_ICON or app.ENABLE_MOVE_COSTUME_ATTR or app.ENABLE_MAILBOX:
			if self.interface:
				onTopWindow = self.interface.GetOnTopWindow()

		for i in xrange(player.INVENTORY_PAGE_SIZE):
			slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)

			itemCount = getItemCount(slotNumber)
			# itemCount == 0이면 소켓을 비운다.
			if 0 == itemCount:
				self.wndItem.ClearSlot(i)
				continue
			elif 1 == itemCount:
				itemCount = 0

			itemVnum = getItemVNum(slotNumber)
			setItemVNum(i, itemVnum, itemCount)

			## 자동물약 (HP: #72723 ~ #72726, SP: #72727 ~ #72730) 특수처리 - 아이템인데도 슬롯에 활성화/비활성화 표시를 위한 작업임 - [hyo]
			if constInfo.IS_AUTO_POTION(itemVnum):
				# metinSocket - [0] : 활성화 여부, [1] : 사용한 양, [2] : 최대 용량
				metinSocket = [player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
				tempSlotNum = slotNumber
				if tempSlotNum >= player.INVENTORY_PAGE_SIZE:
					tempSlotNum -= (self.inventoryPageIndex * player.INVENTORY_PAGE_SIZE)

				isActivated = 0 != metinSocket[0]

				if isActivated:
					self.wndItem.ActivateSlot(tempSlotNum)

					potionType = 0
					if constInfo.IS_AUTO_POTION_HP(itemVnum):
						potionType = player.AUTO_POTION_TYPE_HP
					elif constInfo.IS_AUTO_POTION_SP(itemVnum):
						potionType = player.AUTO_POTION_TYPE_SP

					player.SetAutoPotionInfo(potionType, player.INVENTORY, slotNumber)
				else:
					self.wndItem.DeactivateSlot(tempSlotNum)

			if app.WJ_ENABLE_TRADABLE_ICON or app.ENABLE_MOVE_COSTUME_ATTR or app.ENABLE_MAILBOX:
				if itemVnum and self.interface and onTopWindow:
					if self.interface.MarkUnusableInvenSlotOnTopWnd(onTopWindow, slotNumber):
						self.wndItem.SetUnusableSlotOnTopWnd(i)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(i)
				else:
					self.wndItem.SetUsableSlotOnTopWnd(i)

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				if not player.GetChangeLookVnum(player.INVENTORY, slotNumber) == 0:
					self.wndItem.SetSlotCoverImage(i, "icon/item/ingame_convert_Mark.tga")
				else:
					self.wndItem.EnableSlotCoverImage(i, False)

			if app.ENABLE_GEM_CONVERTER:
				if slotNumber in self.gem_converter_item_pos:
					self.wndItem.SetCantMouseEventSlot(i)

			if app.ENABLE_SOUL_SYSTEM:
				if item.IsSoulItem(itemVnum):
					metinSocket = [player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
					tempSlotNum = slotNumber
					if tempSlotNum >= player.INVENTORY_PAGE_SIZE:
						tempSlotNum -= (self.inventoryPageIndex * player.INVENTORY_PAGE_SIZE)

					if 0 != metinSocket[1]:
						self.wndItem.ActivateSlot(tempSlotNum)
					else:
						self.wndItem.DeactivateSlot(tempSlotNum)

			## [MT-662] 일반펫 소환표시 마크
			if (item.ITEM_TYPE_QUEST == item.GetItemType() and item.QUEST_PET_PAY == item.GetItemSubType()) or\
				(item.ITEM_TYPE_PET == item.GetItemType() and item.PET_PAY == item.GetItemSubType()):
				metinSocket = [player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
				tempSlotNum = slotNumber
				if tempSlotNum >= player.INVENTORY_PAGE_SIZE:
					tempSlotNum -= (self.inventoryPageIndex * player.INVENTORY_PAGE_SIZE)
				if 0 != metinSocket[1]:
					self.wndItem.ActivateSlot(tempSlotNum)
				else:
					self.wndItem.DeactivateSlot(tempSlotNum)

		self.__HighlightSlot_Refresh()

		if app.ENABLE_EXTEND_INVEN_SYSTEM:
			self.__RefreshExinvenCoverSlot()
			self.__RefreshExinvenSlotState()

		self.wndItem.RefreshSlot()

		if not app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
			if self.wndBelt:
				self.wndBelt.RefreshSlot()

	if not app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
		def RefreshEquipSlotWindow(self):
			getItemVNum = ReprGetItemIndex(player.EQUIPMENT)
			getItemCount = ReprGetItemCount(player.EQUIPMENT)
			setItemVNum = self.wndEquip.SetItemSlot

			for i in xrange(player.WEAR_MAX): ## 32 ( 0 ~ 31 )
				itemCount = getItemCount(i)
				if itemCount <= 1:
					itemCount = 0
				setItemVNum(i, getItemVNum(i), itemCount)

				if app.ENABLE_CHANGE_LOOK_SYSTEM:
					if not player.GetChangeLookVnum(player.EQUIPMENT, i) == 0:
						self.wndEquip.SetSlotCoverImage(i, "icon/item/ingame_convert_Mark.tga")
					else:
						self.wndEquip.EnableSlotCoverImage(i,False)

			self.wndEquip.RefreshSlot()

			if self.wndCostume:
				self.wndCostume.RefreshCostumeSlot()

	def RefreshItemSlot(self):
		self.RefreshBagSlotWindow()
		if not app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
			self.RefreshEquipSlotWindow()

	if app.ENABLE_EXTEND_INVEN_SYSTEM:
		def __RefreshExinvenCoverSlot(self):
			cur_stage = player.GetExtendInvenStage()

			self.__HideAllExtendInvenButton()
			self.__ShowExtendInvenButton(cur_stage)

		def __RefreshExinvenSlotState(self):
			inventory_max = player.GetExtendInvenMax()

			for i in xrange(player.INVENTORY_PAGE_SIZE):
				slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)

				if slotNumber >= inventory_max:
					self.wndItem.LockSlot(i)
				else:
					self.wndItem.UnlockSlot(i)

	def RefreshStatus(self):
		money = player.GetElk()
		self.wndMoney.SetText(localeInfo.NumberToMoneyString(money))

		if app.ENABLE_CHEQUE_SYSTEM:
			cheque = player.GetCheque()
			self.wndCheque.SetText(str(cheque))

		if app.ENABLE_GEM_SYSTEM:
			if self.wndGem:
				gem = player.GetGem()
				self.wndGem.SetText(localeInfo.NumberToMoneyString(gem))

	def SetItemToolTip(self, tooltipItemvalue):
		from _weakref import proxy
		self.tooltipItem = proxy(tooltipItemvalue)

	def SellItem(self):
		if self.sellingSlotitemIndex == player.GetItemIndex(self.sellingSlotWindow, self.sellingSlotNumber):
			if self.sellingSlotitemCount == player.GetItemCount(self.sellingSlotWindow, self.sellingSlotNumber):
				## 용혼석도 팔리게 하는 기능 추가하면서 인자 type 추가
				net.SendShopSellPacketNew(self.sellingSlotNumber, self.questionDialog.count, self.sellingSlotWindow)
				snd.PlaySound("sound/ui/money.wav")
		self.OnCloseQuestionDialog()

	def OnDetachMetinFromItem(self):
		if None == self.questionDialog:
			return

		self.__SendUseItemToItemPacket(self.questionDialog.sourceWindow, self.questionDialog.sourcePos, self.questionDialog.targetWindow, self.questionDialog.targetPos)
		self.OnCloseQuestionDialog()

	def OnCloseQuestionDialog(self):
		if not self.questionDialog:
			return

		self.questionDialog.Close()
		self.questionDialog = None
		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	## Slot Event
	def SelectEmptySlot(self, selectedSlotPos, selectedSlotWindow):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return

		## 악세서리 창이 열려있으면
		## 아이템 이동 금지.
		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			if player.GetAcceRefineWindowOpen():
				return

		if app.ENABLE_CHANGE_LOOK_SYSTEM:
			if player.GetChangeLookWindowOpen():
				return

		if app.ENABLE_AURA_COSTUME_SYSTEM:
			if player.IsAuraWindowOpen():
				return

		if app.ENABLE_ATTR_6TH_7TH:
			if self.interface.wndAttr67Add.IsShow():
				return

		selectedSlotPos = self.__InventoryLocalSlotPosToGlobalSlotPos(selectedSlotPos, selectedSlotWindow)

		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedItemCount = mouseModule.mouseController.GetAttachedItemCount()
			attachedItemIndex = mouseModule.mouseController.GetAttachedItemIndex()

			if player.SLOT_TYPE_INVENTORY == attachedSlotType:
				if attachedSlotPos >= player.ITEM_SLOT_COUNT:
					mouseModule.mouseController.DeattachObject()
					return

				self.__SendMoveItemPacket(player.INVENTORY, attachedSlotPos, selectedSlotWindow, selectedSlotPos, attachedItemCount)

				if item.IsRefineScroll(attachedItemIndex):
					self.interface.SetUseItemMode(False)

			elif player.SLOT_TYPE_EQUIPMENT == attachedSlotType:
				if attachedSlotPos >= player.WEAR_MAX:
					mouseModule.mouseController.DeattachObject()
					return

				self.__SendMoveItemPacket(player.EQUIPMENT, attachedSlotPos, selectedSlotWindow, selectedSlotPos, attachedItemCount)

			elif player.SLOT_TYPE_BELT_INVENTORY == attachedSlotType:
				if attachedSlotPos >= item.BELT_INVENTORY_SLOT_COUNT:
					mouseModule.mouseController.DeattachObject()
					return

				self.__SendMoveItemPacket(player.BELT_INVENTORY, attachedSlotPos, selectedSlotWindow, selectedSlotPos, attachedItemCount)

			elif player.SLOT_TYPE_PRIVATE_SHOP == attachedSlotType:
				mouseModule.mouseController.RunCallBack("INVENTORY")

			elif player.SLOT_TYPE_SHOP == attachedSlotType:
				net.SendShopBuyPacket(attachedSlotPos)

			elif player.SLOT_TYPE_SAFEBOX == attachedSlotType:
				if player.ITEM_MONEY == attachedItemIndex:
					net.SendSafeboxWithdrawMoneyPacket(mouseModule.mouseController.GetAttachedItemCount())
					snd.PlaySound("sound/ui/money.wav")
				else:
					net.SendSafeboxCheckoutPacket(attachedSlotPos, selectedSlotWindow, selectedSlotPos)

			elif player.SLOT_TYPE_MALL == attachedSlotType:
				net.SendMallCheckoutPacket(attachedSlotPos, selectedSlotWindow, selectedSlotPos)

			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				if player.SLOT_TYPE_ACCE == attachedSlotType:
					#net.SendAcceRefineCheckOut(attachedSlotPos)
					pass

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				if player.SLOT_TYPE_CHANGE_LOOK == attachedSlotType:
					#net.SendChangeLookCheckOut(attachedSlotPos)
					pass

			if app.ENABLE_AURA_COSTUME_SYSTEM:
				if player.SLOT_TYPE_AURA == attachedSlotType:
					#net.SendAuraRefineCheckOut(attachedSlotPos)
					pass

			mouseModule.mouseController.DeattachObject()

	def SelectItemSlot(self, itemSlotIndex, window_type):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return

		itemSlotIndex = self.__InventoryLocalSlotPosToGlobalSlotPos(itemSlotIndex, window_type)

		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedItemVID = mouseModule.mouseController.GetAttachedItemIndex()

			if player.SLOT_TYPE_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_BELT_INVENTORY == attachedSlotType:
				if attachedSlotPos >= player.ITEM_SLOT_COUNT:
					mouseModule.mouseController.DeattachObject()
					return

				attachedWindow = player.SlotTypeToInvenType(attachedSlotType)
				self.__DropSrcItemToDestItemInInventory(attachedItemVID, attachedWindow, attachedSlotPos, window_type, itemSlotIndex)

			mouseModule.mouseController.DeattachObject()
		else:
			curCursorNum = app.GetCursor()
			if app.SELL == curCursorNum:
				self.__SellItem(window_type, itemSlotIndex)

			elif app.BUY == curCursorNum:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SHOP_BUY_INFO)

			elif app.IsPressed(app.DIK_LALT):
				link = player.GetItemLink(window_type, itemSlotIndex)
				ime.PasteString(link)

			elif app.IsPressed(app.DIK_LSHIFT):
				itemCount = player.GetItemCount(window_type, itemSlotIndex)

				if itemCount > 1:
					if app.ENABLE_CHEQUE_SYSTEM:
						self.dlgPickETC.SetTitleName(localeInfo.PICK_ITEM_TITLE)
						self.dlgPickETC.SetAcceptEvent(ui.__mem_func__(self.OnPickItem))
						self.dlgPickETC.Open(itemCount)
						self.dlgPickETC.itemGlobalSlotIndex = itemSlotIndex
						self.dlgPickETC.itemGlobalSlotWindow = window_type
					else:
						self.dlgPickMoney.SetTitleName(localeInfo.PICK_ITEM_TITLE)
						self.dlgPickMoney.SetAcceptEvent(ui.__mem_func__(self.OnPickItem))
						self.dlgPickMoney.Open(itemCount)
						self.dlgPickMoney.itemGlobalSlotIndex = itemSlotIndex
						self.dlgPickMoney.itemGlobalSlotWindow = window_type

			elif app.IsPressed(app.DIK_LCONTROL):
				itemIndex = player.GetItemIndex(window_type, itemSlotIndex)

				if True == item.CanAddToQuickSlotItem(itemIndex):
					if window_type == player.INVENTORY:
						player.RequestAddToEmptyLocalQuickSlot(player.SLOT_TYPE_INVENTORY, itemSlotIndex)
					elif window_type == player.BELT_INVENTORY:
						player.RequestAddToEmptyLocalQuickSlot(player.SLOT_TYPE_BELT_INVENTORY, itemSlotIndex)
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.QUICKSLOT_REGISTER_DISABLE_ITEM)

			else:
				selectedItemVNum = player.GetItemIndex(window_type, itemSlotIndex)
				itemCount = player.GetItemCount(window_type, itemSlotIndex)
				attachSlotType = player.WindowTypeToSlotType(window_type)

				mouseModule.mouseController.AttachObject(self, attachSlotType, itemSlotIndex, selectedItemVNum, itemCount)

				if self.__IsUsableItemToItem(selectedItemVNum, itemSlotIndex):
					self.interface.SetUseItemMode(True)
				else:
					self.interface.SetUseItemMode(False)

				snd.PlaySound("sound/ui/pick.wav")

	def __DropSrcItemToDestItemInInventory(self, srcItemVID, srcItemSlotWindow, srcItemSlotPos, dstItemSlotWindow, dstItemSlotPos):
		if srcItemSlotWindow == dstItemSlotWindow:
			if srcItemSlotPos == dstItemSlotPos:
				return

		## 악세서리 창이 열려있으면
		## 아이템 이동 금지.
		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			if player.GetAcceRefineWindowOpen():
				return

		if app.ENABLE_CHANGE_LOOK_SYSTEM:
			if player.GetChangeLookWindowOpen():
				return

		if app.ENABLE_AURA_COSTUME_SYSTEM:
			if player.IsAuraWindowOpen():
				return

		if app.ENABLE_ATTR_6TH_7TH:
			if self.interface.wndAttr67Add.IsShow():
				return

		## [MT-462]
		if player.GetItemIndex(srcItemSlotWindow, srcItemSlotPos) == player.GetItemIndex(dstItemSlotWindow, dstItemSlotPos) \
			and (player.GetItemFlags(srcItemSlotWindow, srcItemSlotPos) & item.ITEM_FLAG_STACKABLE) \
			and player.IsEquipmentSlot(dstItemSlotWindow, dstItemSlotPos) == False:
			self.__SendMoveItemPacket(srcItemSlotWindow, srcItemSlotPos, dstItemSlotWindow, dstItemSlotPos, 0)
			return

		if app.ENABLE_SOULBIND_SYSTEM:
			# cyh itemseal 2013 11 08
			if item.IsSealScroll(srcItemVID):
				if player.CanSealItem(srcItemVID, dstItemSlotWindow, dstItemSlotPos):
					self.__OpenQuestionDialog(srcItemSlotWindow, srcItemSlotPos, dstItemSlotWindow, dstItemSlotPos)

		useType = item.GetUseType(srcItemVID)

		if item.IsRefineScroll(srcItemVID):
			self.RefineItem(srcItemSlotWindow, srcItemSlotPos, dstItemSlotWindow, dstItemSlotPos)
			self.interface.SetUseItemMode(False)

		elif item.IsMetin(srcItemVID):
			self.AttachMetinToItem(srcItemSlotWindow, srcItemSlotPos, dstItemSlotWindow, dstItemSlotPos)

		elif item.IsDetachScroll(srcItemVID):
			self.DetachMetinFromItem(srcItemSlotWindow, srcItemSlotPos, dstItemSlotWindow, dstItemSlotPos)

		elif item.IsKey(srcItemVID):
			self.__SendUseItemToItemPacket(srcItemSlotWindow, srcItemSlotPos, dstItemSlotWindow, dstItemSlotPos)

		elif (player.GetItemFlags(srcItemSlotPos) & ITEM_FLAG_APPLICABLE) == ITEM_FLAG_APPLICABLE:
			self.__SendUseItemToItemPacket(srcItemSlotWindow, srcItemSlotPos, dstItemSlotWindow, dstItemSlotPos)

		elif useType in self.USE_TYPE_TUPLE:
			if app.ENABLE_REFINE_ELEMENT_SYSTEM:
				if "USE_ELEMENT_UPGRADE" == useType:
					if not uiRefine.RefineElementDialog.CanRefineElementUpgrade(srcItemSlotWindow, srcItemSlotPos, dstItemSlotWindow, dstItemSlotPos, True, False):
						return

				elif "USE_ELEMENT_DOWNGRADE" == useType:
					if not uiRefine.RefineElementDialog.CanRefineElementDowngrade(srcItemSlotWindow, srcItemSlotPos, dstItemSlotWindow, dstItemSlotPos, True, False):
						return

				elif "USE_ELEMENT_CHANGE" == useType:
					if not uiRefine.RefineElementChangeDialog.CanRefineElementChange(srcItemSlotWindow, srcItemSlotPos, dstItemSlotWindow, dstItemSlotPos, True, False):
						return

			self.__SendUseItemToItemPacket(srcItemSlotWindow, srcItemSlotPos, dstItemSlotWindow, dstItemSlotPos)

		else:
			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				if item.IsAcceScroll(srcItemVID):
					if player.CanAcceClearItem(srcItemVID, dstItemSlotWindow, dstItemSlotPos):
						self.__OpenQuestionDialog(srcItemSlotWindow, srcItemSlotPos, dstItemSlotWindow, dstItemSlotPos)
						return

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				if item.IsChangeLookClearScroll(srcItemVID):
					if dstItemSlotWindow != player.INVENTORY:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CHANGE_LOOK_DO_NOT_EQUIP_ITEM)
						return

					if player.CanChangeLookClearItem(srcItemVID, dstItemSlotWindow, dstItemSlotPos):
						self.__OpenQuestionDialog(srcItemSlotWindow, srcItemSlotPos, dstItemSlotWindow, dstItemSlotPos)
						return

			if app.ENABLE_AURA_COSTUME_SYSTEM:
				if srcItemVID == item.ITEM_VNUM_AURA_CLEAR:
					if player.CanAuraClearItem(srcItemVID, dstItemSlotWindow, dstItemSlotPos):
						self.__UseAuraClearItem(srcItemSlotWindow, srcItemSlotPos, dstItemSlotWindow, dstItemSlotPos)
						return

			if app.ENABLE_SET_ITEM:
				if item.IsSetClearScroll(srcItemVID):
					if player.CanSetItemClear(srcItemVID, dstItemSlotWindow, dstItemSlotPos):
						self.__OpenSetClearItemQuestionDialog(srcItemSlotWindow, srcItemSlotPos, dstItemSlotWindow, dstItemSlotPos)
						return

			if app.ENABLE_GEM_CONVERTER:
				if srcItemVID == item.ITEM_VNUM_GEM_CONVERTER:
					if self.__CanUseGemConverter(srcItemVID, srcItemSlotWindow, srcItemSlotPos, dstItemSlotWindow, dstItemSlotPos):
						self.__UseGemConverter(srcItemSlotWindow, srcItemSlotPos, dstItemSlotWindow, dstItemSlotPos)
						return

			## 이동시킨 곳이 장착 슬롯일 경우 아이템을 사용해서 장착 시킨다 - [levites]
			if player.IsEquipmentSlot(dstItemSlotWindow, dstItemSlotPos):
				## 들고 있는 아이템이 장비일때만
				if item.IsEquipmentVID(srcItemVID):
					self.__UseItem(srcItemSlotPos, srcItemSlotWindow)
			else:
				self.__SendMoveItemPacket(srcItemSlotWindow, srcItemSlotPos, dstItemSlotWindow, dstItemSlotPos, 0)

	def __OpenQuestionDialog(self, srcItemWindow, srcItemPos, dstItemWindow, dstItemPos):
		if self.interface.IsShowDlgQuestionWindow():
			self.interface.CloseDlgQuestionWindow()

		getItemVNum = player.GetItemIndex
		self.srcItemPos = srcItemPos
		self.dstItemPos = dstItemPos

		self.srcItemWindow = srcItemWindow
		self.dstItemWindow = dstItemWindow

		self.dlgQuestion.SetAcceptEvent(ui.__mem_func__(self.__Accept))
		self.dlgQuestion.SetCancelEvent(ui.__mem_func__(self.__Cancel))

		self.dlgQuestion.SetText1("%s" % item.GetItemName(getItemVNum(srcItemPos)) )
		self.dlgQuestion.SetText2(localeInfo.INVENTORY_REALLY_USE_ITEM)

		self.dlgQuestion.Open()

		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

	def __Accept(self):
		self.dlgQuestion.Close()
		self.__SendUseItemToItemPacket(self.srcItemWindow, self.srcItemPos, self.dstItemWindow, self.dstItemPos)
		self.srcItemPos = (0, 0)
		self.dstItemPos = (0, 0)

		self.srcItemWindow = None
		self.dstItemWindow = None

		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	def __Cancel(self):
		self.srcItemPos = (0, 0)
		self.dstItemPos = (0, 0)
		self.dlgQuestion.Close()

		self.srcItemWindow = None
		self.dstItemWindow = None

		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	if app.ENABLE_SET_ITEM:
		def __OpenSetClearItemQuestionDialog(self, srcItemWindow, srcItemPos, dstItemWindow, dstItemPos):
			if self.interface.IsShowDlgQuestionWindow():
				self.interface.CloseDlgQuestionWindow()

			getItemVNum = player.GetItemIndex
			self.srcItemPos = srcItemPos
			self.dstItemPos = dstItemPos

			self.srcItemWindow = srcItemWindow
			self.dstItemWindow = dstItemWindow

			self.dlgQuestion.SetAcceptEvent(ui.__mem_func__(self.__SetClearItemAccept))
			self.dlgQuestion.SetCancelEvent(ui.__mem_func__(self.__SetClearItemCancel))

			self.dlgQuestion.SetText1(localeInfo.SET_ITEM_CLEAR_CONFIRM_QUESTION)
			self.dlgQuestion.SetText2(localeInfo.SET_ITEM_CLEAR_CONFIRM_QUESTION_2)
			self.dlgQuestion.Open()

			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

		def __SetClearItemAccept(self):
			self.__Accept()

		def __SetClearItemCancel(self):
			self.__Cancel()

	def __SellItem(self, window_type, itemSlotPos):
		if not player.IsEquipmentSlot(window_type, itemSlotPos):
			itemIndex = player.GetItemIndex(window_type, itemSlotPos)
			itemCount = player.GetItemCount(window_type, itemSlotPos)

			self.sellingSlotWindow = window_type
			self.sellingSlotNumber = itemSlotPos
			self.sellingSlotitemIndex = itemIndex
			self.sellingSlotitemCount = itemCount

			item.SelectItem(itemIndex)
			## 안티 플레그 검사 빠져서 추가
			## 20140220
			if item.IsAntiFlag(item.ANTIFLAG_SELL):
				popup = uiCommon.PopupDialog()
				popup.SetText(localeInfo.SHOP_CANNOT_SELL_ITEM)
				popup.SetAcceptEvent(self.__OnClosePopupDialog)
				popup.Open()
				self.popup = popup
				return

			itemPrice = item.GetISellItemPrice()

			if item.Is1GoldItem():
				itemPrice = itemCount / itemPrice / 5
			else:
				itemPrice = itemPrice * itemCount / 5

			item.GetItemName(itemIndex)
			itemName = item.GetItemName()
			if app.ENABLE_SET_ITEM:
				itemName = item.GetItemNameBySetValue(player.GetItemSetValue(window_type, itemSlotPos))

			self.questionDialog = uiCommon.QuestionDialog()
			self.questionDialog.SetText(localeInfo.DO_YOU_SELL_ITEM(itemName, itemCount, itemPrice))
			self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.SellItem))
			self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
			self.questionDialog.Open()
			self.questionDialog.count = itemCount

			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

	def __OnClosePopupDialog(self):
		self.pop = None

	def RefineItem(self, scrollSlotWindow, scrollSlotPos, targetSlotWindow, targetSlotPos):
		scrollIndex = player.GetItemIndex(scrollSlotWindow, scrollSlotPos)
		targetIndex = player.GetItemIndex(targetSlotWindow, targetSlotPos)

		if player.REFINE_OK != player.CanRefine(scrollIndex, targetSlotWindow, targetSlotPos):
			return

		###########################################################
		self.__SendUseItemToItemPacket(scrollSlotWindow, scrollSlotPos, targetSlotWindow, targetSlotPos)
		#net.SendItemUseToItemPacket(scrollSlotPos, targetSlotPos)
		return
		###########################################################

		###########################################################
		#net.SendRequestRefineInfoPacket(targetSlotPos)
		#return
		###########################################################

		result = player.CanRefine(scrollIndex, targetSlotPos)

		if player.REFINE_ALREADY_MAX_SOCKET_COUNT == result:
			#snd.PlaySound("sound/ui/jaeryun_fail.wav")
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_NO_MORE_SOCKET)

		elif player.REFINE_NEED_MORE_GOOD_SCROLL == result:
			#snd.PlaySound("sound/ui/jaeryun_fail.wav")
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_NEED_BETTER_SCROLL)

		elif player.REFINE_CANT_MAKE_SOCKET_ITEM == result:
			#snd.PlaySound("sound/ui/jaeryun_fail.wav")
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_SOCKET_DISABLE_ITEM)

		elif player.REFINE_NOT_NEXT_GRADE_ITEM == result:
			#snd.PlaySound("sound/ui/jaeryun_fail.wav")
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_UPGRADE_DISABLE_ITEM)

		elif player.REFINE_CANT_REFINE_METIN_TO_EQUIPMENT == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_EQUIP_ITEM)

		if player.REFINE_OK != result:
			return

		self.refineDialog.Open(scrollSlotPos, targetSlotPos)

	def DetachMetinFromItem(self, scrollSlotWindow, scrollSlotPos, targetSlotWindow, targetSlotPos):
		scrollIndex = player.GetItemIndex(scrollSlotWindow, scrollSlotPos)
		targetIndex = player.GetItemIndex(targetSlotWindow, targetSlotPos)

		if not player.CanDetach(scrollIndex, targetSlotWindow, targetSlotPos):
			if app.ENABLE_ACCE_COSTUME_SYSTEM and item.IsAcceScroll(scrollIndex):
				return

			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_METIN_INSEPARABLE_ITEM)
			return

		self.questionDialog = uiCommon.QuestionDialog()
		if (app.ENABLE_ACCE_COSTUME_SYSTEM or app.ENABLE_AURA_COSTUME_SYSTEM) and item.IsAcceScroll(scrollIndex):
			self.questionDialog.SetText(localeInfo.AURA_CLEAR_QUESTION_WHEN_USE)
		elif app.ENABLE_CHANGE_LOOK_SYSTEM and item.IsChangeLookClearScroll(scrollIndex):
			self.questionDialog.SetText(localeInfo.CHANGE_LOOK_CHANGE_ITEM)
		else:
			self.questionDialog.SetText(localeInfo.REFINE_DO_YOU_SEPARATE_METIN)
		self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.OnDetachMetinFromItem))
		self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
		self.questionDialog.Open()
		self.questionDialog.sourcePos = scrollSlotPos
		self.questionDialog.targetPos = targetSlotPos

		self.questionDialog.sourceWindow = scrollSlotWindow
		self.questionDialog.targetWindow = targetSlotWindow

	def AttachMetinToItem(self, metinSlotWindow, metinSlotPos, targetSlotWindow, targetSlotPos):
		metinIndex = player.GetItemIndex(metinSlotWindow, metinSlotPos)
		targetIndex = player.GetItemIndex(targetSlotWindow, targetSlotPos)

		item.SelectItem(metinIndex)
		itemName = item.GetItemName()
		if app.ENABLE_SET_ITEM:
			itemName = item.GetItemNameBySetValue(player.GetItemSetValue(metinSlotWindow, metinSlotPos))

		result = player.CanAttachMetin(metinIndex, targetSlotWindow, targetSlotPos)

		if player.ATTACH_METIN_NOT_MATCHABLE_ITEM == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_CAN_NOT_ATTACH(itemName))

		if player.ATTACH_METIN_NO_MATCHABLE_SOCKET == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_NO_SOCKET(itemName))

		elif player.ATTACH_METIN_NOT_EXIST_GOLD_SOCKET == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_NO_GOLD_SOCKET(itemName))

		elif player.ATTACH_METIN_CANT_ATTACH_TO_EQUIPMENT == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_EQUIP_ITEM)

		if player.ATTACH_METIN_OK != result:
			return

		self.attachMetinDialog.Open(metinSlotWindow, metinSlotPos, targetSlotWindow, targetSlotPos)

	def OverOutItem(self):
		self.wndItem.SetUsableItem(False)
		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def OverInItem(self, overSlotPos, window_type):
		overInvenSlotPos = self.__InventoryLocalSlotPosToGlobalSlotPos(overSlotPos, window_type)
		self.wndItem.SetUsableItem(False)

		getItemVNum = ReprGetItemIndex(window_type)
		itemVnum = getItemVNum(overInvenSlotPos)

		if app.WJ_ENABLE_PICKUP_ITEM_EFFECT:
			if window_type == player.INVENTORY:
				self.DelHighlightSlot(overInvenSlotPos)

		if mouseModule.mouseController.isAttached():
			attachedItemType = mouseModule.mouseController.GetAttachedType()

			if player.SLOT_TYPE_INVENTORY == attachedItemType:
				attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
				attachedItemVNum = mouseModule.mouseController.GetAttachedItemIndex()

				if self.__CanUseSrcItemToDstItem(attachedItemVNum, player.INVENTORY, attachedSlotPos, window_type, overInvenSlotPos):
					self.wndItem.SetUsableItem(True)
					self.ShowToolTip(overInvenSlotPos, window_type)
					return

		self.ShowToolTip(overInvenSlotPos, window_type)

	def __IsUsableItemToItem(self, srcItemVNum, srcSlotPos):
		"다른 아이템에 사용할 수 있는 아이템인가?"

		if item.IsRefineScroll(srcItemVNum):
			return True
		elif app.ENABLE_SOULBIND_SYSTEM and item.IsSealScroll(srcItemVNum):
			return True
		elif item.IsMetin(srcItemVNum):
			return True
		elif item.IsDetachScroll(srcItemVNum):
			return True
		elif item.IsKey(srcItemVNum):
			return True
		elif item.IsItemUsedForDragonSoul(srcItemVNum):
			return True
		elif (player.GetItemFlags(srcSlotPos) & ITEM_FLAG_APPLICABLE) == ITEM_FLAG_APPLICABLE:
			return True
		else:
			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				if item.IsAcceScroll(srcItemVNum):
					return True

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				if item.IsChangeLookClearScroll(srcItemVNum):
					return True

			if app.ENABLE_AURA_COSTUME_SYSTEM:
				if item.ITEM_VNUM_AURA_CLEAR == srcItemVNum:
					return True

			if app.ENABLE_SET_ITEM:
				if item.IsSetClearScroll(srcItemVNum):
					return True

			if app.ENABLE_GEM_CONVERTER:
				if srcItemVNum == item.ITEM_VNUM_GEM_CONVERTER:
					return True

			if item.GetUseType(srcItemVNum) in self.USE_TYPE_TUPLE:
				return True

		return False

	def __CanUseSrcItemToDstItem(self, srcItemVNum, srcSlotWindow, srcSlotPos, dstSlotWindow, dstSlotPos):
		## 대상 아이템에 사용할 수 있는가?
		if srcSlotWindow == dstSlotWindow:
			if srcSlotPos == dstSlotPos:
				return False

		## [MT-462]: Stackable UseMode 이고..
		#if srcItemVNum == player.GetItemIndex(dstSlotWindow, dstSlotPos) and \
		#	(player.GetItemFlags(dstSlotWindow, dstSlotPos) & item.ITEM_FLAG_STACKABLE):
		#	return True

		if item.IsRefineScroll(srcItemVNum):
			if player.REFINE_OK == player.CanRefine(srcItemVNum, dstSlotWindow, dstSlotPos):
				return True

		elif app.ENABLE_SOULBIND_SYSTEM and item.IsSealScroll(srcItemVNum):
			if player.CanSealItem(srcItemVNum, dstSlotWindow, dstSlotPos):
				return True

		elif item.IsMetin(srcItemVNum):
			if player.ATTACH_METIN_OK == player.CanAttachMetin(srcItemVNum, dstSlotWindow, dstSlotPos):
				return True

		elif item.IsDetachScroll(srcItemVNum):
			if player.DETACH_METIN_OK == player.CanDetach(srcItemVNum, dstSlotWindow, dstSlotPos):
				return True

		elif item.IsKey(srcItemVNum):
			if player.CanUnlock(srcItemVNum, dstSlotWindow, dstSlotPos):
				return True

		elif (player.GetItemFlags(srcSlotWindow, srcSlotPos) & ITEM_FLAG_APPLICABLE) == ITEM_FLAG_APPLICABLE:
			return True

		else:
			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				if item.IsAcceScroll(srcItemVNum):
					if player.CanAcceClearItem(srcItemVNum, dstSlotWindow, dstSlotPos):
						return True

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				if item.IsChangeLookClearScroll(srcItemVNum):
					if player.CanChangeLookClearItem(srcItemVNum, dstSlotWindow, dstSlotPos):
						return True

			if app.ENABLE_AURA_COSTUME_SYSTEM:
				if item.ITEM_VNUM_AURA_CLEAR == srcItemVNum:
					if player.CanAuraClearItem(srcItemVNum, dstSlotWindow, dstSlotPos):
						return True

			if app.ENABLE_SET_ITEM:
				if item.IsSetClearScroll(srcItemVNum):
					if player.CanSetItemClear(srcItemVNum, dstSlotWindow, dstSlotPos):
						return True

			if app.ENABLE_GEM_CONVERTER:
				if srcItemVNum == item.ITEM_VNUM_GEM_CONVERTER:
					if self.__CanUseGemConverter(srcItemVNum, dstSlotWindow, srcSlotPos, dstSlotWindow, dstSlotPos):
						return True

			useType = item.GetUseType(srcItemVNum)

			if app.ENABLE_ATTR_6TH_7TH:
				if "USE_CHANGE_ATTRIBUTE2" == useType:
					if self.__CanChangeItemAttr67List(player.INVENTORY, dstSlotPos):
						return True

			if "USE_CLEAN_SOCKET" == useType:
				if self.__CanCleanBrokenMetinStone(dstSlotWindow, dstSlotPos):
					return True

			elif "USE_CHANGE_ATTRIBUTE" == useType:
				if self.__CanChangeItemAttrList(dstSlotWindow, dstSlotPos):
					return True

			elif "USE_ADD_ATTRIBUTE" == useType:
				if self.__CanAddItemAttr(dstSlotWindow, dstSlotPos):
					return True

			elif "USE_ADD_ATTRIBUTE2" == useType:
				if self.__CanAddItemAttr2(dstSlotWindow, dstSlotPos):
					return True

			elif "USE_ADD_ACCESSORY_SOCKET" == useType:
				if self.__CanAddAccessorySocket(dstSlotWindow, dstSlotPos):
					return True

			elif "USE_PUT_INTO_ACCESSORY_SOCKET" == useType:
				if self.__CanPutAccessorySocket(dstSlotWindow, dstSlotPos, srcItemVNum):
					return True

			elif "USE_PUT_INTO_BELT_SOCKET" == useType:
				dstItemVNum = player.GetItemIndex(dstSlotWindow, dstSlotPos)
				item.SelectItem(dstItemVNum)
				if item.ITEM_TYPE_BELT == item.GetItemType():
					return True

			elif app.ENABLE_MOVE_COSTUME_ATTR and "USE_CHANGE_COSTUME_ATTR" == useType:
				if self.__CanChangeCostumeAttrList(dstSlotWindow, dstSlotPos):
					return True

			elif app.ENABLE_MOVE_COSTUME_ATTR and "USE_RESET_COSTUME_ATTR" == useType:
				if self.__CanResetCostumeAttr(dstSlotWindow, dstSlotPos):
					return True

			else:
				if app.ENABLE_CHANGED_ATTR and "USE_SELECT_ATTRIBUTE" == useType:
					if self.__CanChangeItemAttrList(dstSlotWindow, dstSlotPos):
						return True

				if app.ENABLE_REFINE_ELEMENT_SYSTEM:
					if "USE_ELEMENT_UPGRADE" == useType:
						if uiRefine.RefineElementDialog.CanRefineElementUpgrade(srcSlotWindow, srcSlotPos, dstSlotWindow, dstSlotPos, False, False):
							return True

					if "USE_ELEMENT_DOWNGRADE" == useType:
						if uiRefine.RefineElementDialog.CanRefineElementDowngrade(srcSlotWindow, srcSlotPos, dstSlotWindow, dstSlotPos, False, False):
							return True

					if "USE_ELEMENT_CHANGE" == useType:
						if uiRefine.RefineElementChangeDialog.CanRefineElementChange(srcSlotWindow, srcSlotPos, dstSlotWindow, dstSlotPos, False, False):
							return True

		return False

	def __CanCleanBrokenMetinStone(self, dstSlotWindow, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotWindow, dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if item.ITEM_TYPE_WEAPON != item.GetItemType() and item.ITEM_TYPE_ARMOR != item.GetItemType():
			return False

		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			if player.GetItemMetinSocket(dstSlotWindow, dstSlotPos, i) == constInfo.ERROR_METIN_STONE:
				return True

		return False

	def __CanChangeItemAttrList(self, dstSlotWindow, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotWindow, dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if not item.GetItemType() in (item.ITEM_TYPE_WEAPON, item.ITEM_TYPE_ARMOR):
			return False

		# 재경비서
		if app.ENABLE_NEW_USER_CARE:
			if item.IsAntiFlag(item.ITEM_ANTIFLAG_ENCHANT):
				return False

		if app.ENABLE_SOULBIND_SYSTEM:
			if player.GetItemSealDate(dstSlotWindow, dstSlotPos) != item.E_SEAL_DATE_DEFAULT_TIMESTAMP:
				return False

		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			type, value = player.GetItemAttribute(dstSlotWindow, dstSlotPos, i)
			if type != 0:
				return True

		return False

	def __CanPutAccessorySocket(self, dstSlotWindow, dstSlotPos, mtrlVnum):
		dstItemVNum = player.GetItemIndex(dstSlotWindow, dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if item.GetItemType() != item.ITEM_TYPE_ARMOR:
			return False

		if not item.GetItemSubType() in (item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR):
			return False

		curCount = player.GetItemMetinSocket(dstSlotWindow, dstSlotPos, 0)
		maxCount = player.GetItemMetinSocket(dstSlotWindow, dstSlotPos, 1)

		if mtrlVnum != constInfo.GET_ACCESSORY_MATERIAL_VNUM(dstItemVNum, item.GetItemSubType()):
			return False

		if curCount >= maxCount:
			return False

		return True

	def __CanAddAccessorySocket(self, dstSlotWindow, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotWindow, dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if not item.GetItemType() in (item.ITEM_TYPE_ARMOR, item.ITEM_TYPE_BELT):
			return False

		if not item.GetItemSubType() in (item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR) and not item.GetItemType() == item.ITEM_TYPE_BELT:
			return False

		curCount = player.GetItemMetinSocket(dstSlotWindow, dstSlotPos, 0)
		maxCount = player.GetItemMetinSocket(dstSlotWindow, dstSlotPos, 1)

		ACCESSORY_SOCKET_MAX_SIZE = 3
		if maxCount >= ACCESSORY_SOCKET_MAX_SIZE:
			return False

		return True

	if app.ENABLE_ATTR_6TH_7TH:
		def __CanChangeItemAttr67List(self, dstSlotWindow, dstSlotPos):
			dstItemVNum = player.GetItemIndex(dstSlotWindow, dstSlotPos)
			if dstItemVNum == 0:
				return False

			item.SelectItem(dstItemVNum)

			if not item.GetItemType() in (item.ITEM_TYPE_WEAPON, item.ITEM_TYPE_ARMOR):
				return False

			if app.ENABLE_QUIVER_SYSTEM:
				if (item.GetItemType() == item.ITEM_TYPE_WEAPON and item.GetItemSubType() in (item.WEAPON_ARROW, item.WEAPON_QUIVER)):
					return False
			else:
				if (item.GetItemType() == item.ITEM_TYPE_WEAPON and item.GetItemSubType() == item.WEAPON_ARROW):
					return False

			# 재경비서
			if app.ENABLE_NEW_USER_CARE:
				if item.IsAntiFlag(item.ITEM_ANTIFLAG_ENCHANT):
					return False

			if app.ENABLE_SOULBIND_SYSTEM:
				if player.GetItemSealDate(dstSlotWindow, dstSlotPos) != item.E_SEAL_DATE_DEFAULT_TIMESTAMP:
					return False

			attrCount = 0
			for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
				type, value = player.GetItemAttribute(dstSlotWindow, dstSlotPos, i)
				if type != 0:
					attrCount += 1

			if attrCount > player.ATTRIBUTE_SLOT_MAX_NUM - 2 and attrCount <= player.ATTRIBUTE_SLOT_MAX_NUM:
				return True

			return False

	def __CanAddItemAttr(self, dstSlotWindow, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotWindow, dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if not item.GetItemType() in (item.ITEM_TYPE_WEAPON, item.ITEM_TYPE_ARMOR):
			return False

		# 재가비서
		if app.ENABLE_NEW_USER_CARE:
			if item.IsAntiFlag(item.ITEM_ANTIFLAG_REINFORCE):
				return False

		if app.ENABLE_SOULBIND_SYSTEM:
			if player.GetItemSealDate(dstSlotWindow, dstSlotPos) != item.E_SEAL_DATE_DEFAULT_TIMESTAMP:
				return False

		attrCount = 0
		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			type, value = player.GetItemAttribute(dstSlotWindow, dstSlotPos, i)
			if type != 0:
				attrCount += 1

		if attrCount < 4:
			return True

		return False

	def __CanAddItemAttr2(self, dstSlotWindow, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotWindow, dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if not item.GetItemType() in (item.ITEM_TYPE_WEAPON, item.ITEM_TYPE_ARMOR):
			return False

		# 재가비서
		if app.ENABLE_NEW_USER_CARE:
			if item.IsAntiFlag(item.ITEM_ANTIFLAG_REINFORCE):
				return False

		if app.ENABLE_SOULBIND_SYSTEM:
			if player.GetItemSealDate(dstSlotWindow, dstSlotPos) != item.E_SEAL_DATE_DEFAULT_TIMESTAMP:
				return False

		attrCount = 0
		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			type, value = player.GetItemAttribute(dstSlotWindow, dstSlotPos, i)
			if type != 0:
				attrCount += 1

		if attrCount == 4:
			return True

		return False

	if app.ENABLE_MOVE_COSTUME_ATTR:
		def __CanChangeCostumeAttrList(self, dstSlotWindow, dstSlotPos):
			dstItemVNum = player.GetItemIndex(dstSlotWindow, dstSlotPos)
			if dstItemVNum == 0:
				return False

			item.SelectItem(dstItemVNum)

			if item.GetItemType() != item.ITEM_TYPE_COSTUME:
				return False

			if app.ENABLE_MOUNT_COSTUME_SYSTEM:
				if item.GetItemSubType() == item.COSTUME_TYPE_MOUNT:
					return False

			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				if item.GetItemSubType() == item.COSTUME_TYPE_ACCE:
					return False

			if app.ENABLE_AURA_COSTUME_SYSTEM:
				if item.GetItemSubType() == item.COSTUME_TYPE_AURA:
					return False

			for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
				type, value = player.GetItemAttribute(dstSlotWindow, dstSlotPos, i)
				if type != 0:
					return True

			return False

		def __CanResetCostumeAttr(self, dstSlotWindow, dstSlotPos):
			dstItemVNum = player.GetItemIndex(dstSlotWindow, dstSlotPos)
			if dstItemVNum == 0:
				return False

			item.SelectItem(dstItemVNum)

			if item.GetItemType() != item.ITEM_TYPE_COSTUME:
				return False

			if app.ENABLE_MOUNT_COSTUME_SYSTEM:
				if item.GetItemSubType() == item.COSTUME_TYPE_MOUNT:
					return False

			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				if item.GetItemSubType() == item.COSTUME_TYPE_ACCE:
					return False

			if app.ENABLE_AURA_COSTUME_SYSTEM:
				if item.GetItemSubType() == item.COSTUME_TYPE_AURA:
					return False

			for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
				type, value = player.GetItemAttribute(dstSlotWindow, dstSlotPos, i)
				if type != 0:
					return True

			return False

	def ShowToolTip(self, slotIndex, window_type):
		if None != self.tooltipItem:
			self.tooltipItem.SetInventoryItem(slotIndex, window_type)

	def OnTop(self):
		if None != self.tooltipItem:
			self.tooltipItem.SetTop()

		if not app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
			if self.wndBelt:
				if self.IsClickBeltInventory == False:
					self.wndBelt.SetTop()
				else:
					self.IsClickBeltInventory = False

			if self.wndCostume:
				self.wndCostume.SetTop()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def UseItemSlot(self, slotIndex, window):
		curCursorNum = app.GetCursor()
		if app.SELL == curCursorNum:
			return

		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS():
			return

		slotIndex = self.__InventoryLocalSlotPosToGlobalSlotPos(slotIndex, window)

		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			if self.wndDragonSoulRefine.IsShow():
				self.wndDragonSoulRefine.AutoSetItem((window, slotIndex), 1)
				return

		self.__UseItem(slotIndex, window)
		mouseModule.mouseController.DeattachObject()
		self.OverOutItem()

	def __UseItem(self, slotIndex, slotWindow):
		ItemVNum = player.GetItemIndex(slotWindow, slotIndex)
		item.SelectItem(ItemVNum)

		## 악세서리 창이 열려있으면
		## 악세서리 아이템, 무기, 갑옷 외엔 아무 사용 안됨.
		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			if player.GetAcceRefineWindowOpen():
				self.__UseItemAcce(slotIndex, slotWindow)
				return

		if app.ENABLE_CHANGE_LOOK_SYSTEM:
			if player.GetChangeLookWindowOpen():
				return

		if app.ENABLE_AURA_COSTUME_SYSTEM:
			if player.IsAuraWindowOpen():
				self.__UseItemAura(slotIndex, slotWindow)
				return

		if app.WJ_ENABLE_TRADABLE_ICON or app.ENABLE_MOVE_COSTUME_ATTR:
			if self.interface.AttachInvenItemToOtherWindowSlot(slotIndex, slotWindow):
				return

		if app.ENABLE_FLOWER_EVENT:
			if 25121 == ItemVNum or 25122 == ItemVNum or 25123 == ItemVNum or 25124 == ItemVNum or 25125 == ItemVNum:
				value1 = item.GetValue(0)
				value2 = item.GetValue(1)
				value3 = item.GetApplyPoint(value2)

				if item.CheckAffect(value1, value3):
					pass
				else:
					if item.CheckAffect(value1, 0):
						self.questionDialog = uiCommon.QuestionDialog2()
						self.questionDialog.SetText1(localeInfo.FLOWER_EVENT_USE_ITEM_TEXT1)
						self.questionDialog.SetText2(localeInfo.FLOWER_EVENT_USE_ITEM_TEXT2)
						self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnAccept))
						self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnCancel))
						self.questionDialog.Open()
						self.questionDialog.slotIndex = slotIndex
						self.questionDialog.slotWindow = slotWindow
						constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)
						return

		if item.IsFlag(item.ITEM_FLAG_CONFIRM_WHEN_USE):
			self.questionDialog = uiCommon.QuestionDialog()
			self.questionDialog.SetText(localeInfo.INVENTORY_REALLY_USE_ITEM2)
			self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnAccept))
			self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnCancel))
			self.questionDialog.Open()
			self.questionDialog.slotIndex = slotIndex

			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)
		else:
			self.__SendUseItemPacket(slotIndex, slotWindow)

	def SetCanMouseEventSlot(self, inven_slot_index):
		if inven_slot_index >= player.INVENTORY_PAGE_SIZE:
			page = self.GetInventoryPageIndex() # range 0, 1, 2, 3
			inven_slot_index -= (page * player.INVENTORY_PAGE_SIZE)
		self.wndItem.SetCanMouseEventSlot(inven_slot_index)

	def SetCantMouseEventSlot(self, inven_slot_index):
		if inven_slot_index >= player.INVENTORY_PAGE_SIZE:
			page = self.GetInventoryPageIndex() # range 0, 1, 2, 3
			inven_slot_index -= (page * player.INVENTORY_PAGE_SIZE)
		self.wndItem.SetCantMouseEventSlot(inven_slot_index)

	if app.ENABLE_ACCE_COSTUME_SYSTEM:
		def __UseItemAcce(self, slotIndex, slotWindow):
			AcceSlot = player.FindMoveAcceItemSlot()

			if slotWindow != player.INVENTORY:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ACCE_NOT_USINGITEM)
				return

			UsingAcceSlot = player.FindActivatedAcceSlot(slotWindow, slotIndex)

			if app.ENABLE_SOULBIND_SYSTEM:
				## 봉인아이템 걸러냄
				if player.GetItemSealDate(player.INVENTORY, slotIndex) != item.E_SEAL_DATE_DEFAULT_TIMESTAMP:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ACCE_NOT_SEALITEM)
					return

			## 조합창
			if player.GetAcceRefineWindowType() == player.ACCE_SLOT_TYPE_COMBINE:
				if item.GetItemType() == item.ITEM_TYPE_COSTUME:
					if item.GetItemSubType() == item.COSTUME_TYPE_ACCE:
						if app.ENABLE_ACCE_SECOND_COSTUME_SYSTEM:
							## 전설등급 최고 흡수율 25%
							socketInDrainValue = player.GetItemMetinSocket(slotIndex, 1)
							if socketInDrainValue >= player.ACCE_MAX_DRAINRATE:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ACCE_MAX_DRAINRATE)
								return
						else:
							if item.GetRefinedVnum() == 0: ## 전선등급 아이템은 걸러냄.
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ACCE_MAXGRADE)
								return

						if UsingAcceSlot == player.ACCE_SLOT_MAX:
							if AcceSlot != player.ACCE_SLOT_MAX:
								if not player.FindUsingAcceSlot(AcceSlot) == player.NPOS():
									return

								if player.FindUsingAcceSlot(player.ACCE_SLOT_LEFT) == player.NPOS():
									if AcceSlot == player.ACCE_SLOT_RIGHT:
										return

							player.SetAcceActivatedItemSlot(AcceSlot, slotIndex)
							net.SendAcceRefineCheckIn(player.INVENTORY, slotIndex, AcceSlot, player.GetAcceRefineWindowType())
					else:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ACCE_POSSIBLE_ACCE)
						return
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ACCE_POSSIBLE_ACCE)
					return

			## 흡수창
			elif player.GetAcceRefineWindowType() == player.ACCE_SLOT_TYPE_ABSORB:
				isAbsorbItem = False
				if item.GetItemType() == item.ITEM_TYPE_COSTUME:
					if item.GetItemSubType() == item.COSTUME_TYPE_ACCE:
						if not player.GetItemMetinSocket(slotIndex, 0) == 0:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ACCE_NOTABSORBITEM)
							return

						if UsingAcceSlot == player.ACCE_SLOT_MAX:
							if not AcceSlot == player.ACCE_SLOT_LEFT: ## 악세서리아이템은 왼쪽 슬롯에만
								return

							player.SetAcceActivatedItemSlot(AcceSlot, slotIndex)
							net.SendAcceRefineCheckIn(player.INVENTORY, slotIndex, AcceSlot, player.GetAcceRefineWindowType())
					else:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ACCE_POSSIBLE_ACCEITEM)
						return

				elif item.GetItemType() == item.ITEM_TYPE_WEAPON: ## 무기류.
					isAbsorbItem = True

				elif item.GetItemType() == item.ITEM_TYPE_ARMOR: ## 갑옷류
					if item.GetItemSubType() == item.ARMOR_BODY:
						isAbsorbItem = True
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

				if isAbsorbItem:
					if UsingAcceSlot == player.ACCE_SLOT_MAX:
						if not AcceSlot == player.ACCE_SLOT_RIGHT: ## 흡수될 아이템은 오른쪽 슬롯에만
							if player.FindUsingAcceSlot(player.ACCE_SLOT_RIGHT) == player.NPOS():
								AcceSlot = player.ACCE_SLOT_RIGHT
							else:
								return

						popup = uiCommon.QuestionDialog()
						popup.SetText(localeInfo.ACCE_DEL_ABSORDITEM)
						popup.SetAcceptEvent(lambda arg1 = slotIndex, arg2 = AcceSlot: self.OnAcceAcceptEvent(arg1, arg2))
						popup.SetCancelEvent(self.OnAcceCloseEvent)
						popup.Open()
						self.pop = popup

			## 경고 메시지 띄우기.
			if not player.FindUsingAcceSlot(player.ACCE_SLOT_RIGHT) == player.NPOS() and not player.FindUsingAcceSlot(player.ACCE_SLOT_LEFT) == player.NPOS():
				if AcceSlot != player.ACCE_SLOT_MAX:
					popup = uiCommon.PopupDialog()
					if player.GetAcceRefineWindowType() == player.ACCE_SLOT_TYPE_COMBINE:

						if app.ENABLE_ACCE_SECOND_COSTUME_SYSTEM:
							socketInDrainValue = player.GetAcceItemMetinSocket(0, 1)
							socketInDrainValue2 = player.GetAcceItemMetinSocket(1, 1)
							socketInDrainValue3 = player.GetItemMetinSocket(slotIndex, 1)
							## 메인 서버 중. 등록된 아이템이 전설일때 경고 메시지 변경.
							if socketInDrainValue > 0 or socketInDrainValue2 > 0 or socketInDrainValue3 > 0:
								popup.SetText(localeInfo.ACCE_DEL_SERVEITEM2)
							else:
								popup.SetText(localeInfo.ACCE_DEL_SERVEITEM)
						else:
							popup.SetText(localeInfo.ACCE_DEL_SERVEITEM)

						popup.SetAcceptEvent(self.__OnClosePopupDialog)
						popup.Open()
						self.pop = popup

		## 아이템 흡수시 흡수될 아이템 할지 안할지 선택 팝업
		def OnAcceAcceptEvent(self, slotIndex, AcceSlot):
			self.pop.Close()
			self.pop = None

			player.SetAcceActivatedItemSlot(AcceSlot, slotIndex)
			net.SendAcceRefineCheckIn(player.INVENTORY, slotIndex, AcceSlot, player.GetAcceRefineWindowType())

		def OnAcceCloseEvent(self):
			self.pop.Close()
			self.pop = None

	if app.ENABLE_AURA_COSTUME_SYSTEM:
		def __UseItemAura(self, slotIndex, slotWindow):
			AuraSlot = player.FindMoveAuraItemSlot()

			if slotWindow != player.INVENTORY:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ACCE_NOT_USINGITEM)
				return

			UsingAuraSlot = player.FindActivatedAuraSlot(player.INVENTORY, slotIndex)

			if player.GetAuraCurrentItemSlotCount() >= player.AURA_SLOT_MAX <= UsingAuraSlot:
				return

			if app.ENABLE_SOULBIND_SYSTEM:
				## 봉인아이템 걸러냄
				if player.GetItemSealDate(player.INVENTORY, slotIndex) != item.E_SEAL_DATE_DEFAULT_TIMESTAMP:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_SEALITEM)
					return

			if player.GetAuraWindowType() == player.AURA_WINDOW_TYPE_ABSORB:
				isAbsorbItem = False
				if item.GetItemType() == item.ITEM_TYPE_COSTUME:
					if item.GetItemSubType() == item.COSTUME_TYPE_AURA:
						if not player.GetItemMetinSocket(slotIndex, item.ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM) == 0:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_ABSORBITEM)
							return

						if UsingAuraSlot == player.AURA_SLOT_MAX:
							if AuraSlot != player.AURA_SLOT_MAIN:
								return

							net.SendAuraRefineCheckIn(player.INVENTORY, slotIndex, AuraSlot, player.GetAuraWindowType())
					else:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_REFINEITEM)
						return

				elif item.GetItemType() == item.ITEM_TYPE_ARMOR:
					if item.GetItemSubType() in [item.ARMOR_SHIELD, item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR]:
						if player.FindUsingAuraSlot(player.AURA_SLOT_MAIN) == player.NPOS():
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_REGISTER_AURAITEM)
							return

						isAbsorbItem = True
					else:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_REFINEITEM)
						return
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_REFINEITEM)
					return

				if isAbsorbItem:
					if UsingAuraSlot == player.AURA_SLOT_MAX:
						if AuraSlot != player.AURA_SLOT_SUB:
							if player.FindUsingAuraSlot(player.AURA_SLOT_SUB) == player.NPOS():
								AuraSlot = player.AURA_SLOT_SUB
							else:
								return

						popup = uiCommon.QuestionDialog()
						popup.SetText(localeInfo.AURA_NOTICE_DEL_ABSORDITEM)
						popup.SetAcceptEvent(lambda arg1 = slotIndex, arg2 = AuraSlot: self.OnAuraAcceptEvent(arg1, arg2))
						popup.SetCancelEvent(self.OnAuraCloseEvent)
						popup.Open()
						self.pop = popup

			elif player.GetAuraWindowType() == player.AURA_WINDOW_TYPE_GROWTH:
				if UsingAuraSlot == player.AURA_SLOT_MAX:
					if AuraSlot == player.AURA_SLOT_MAIN:
						if item.GetItemType() == item.ITEM_TYPE_COSTUME and item.GetItemSubType() == item.COSTUME_TYPE_AURA:
							socketLevelValue = player.GetItemMetinSocket(slotIndex, item.ITEM_SOCKET_AURA_LEVEL_VALUE)
							curLevel = (socketLevelValue / 100000) - 1000
							curExp = socketLevelValue % 100000;
							if curLevel >= player.AURA_MAX_LEVEL:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_MAX_LEVEL)
								return

							#if curExp >= player.GetAuraRefineInfo(curLevel, item.AURA_REFINE_INFO_NEED_EXP):
							#	chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_GROWTHITEM)
							#	return

							net.SendAuraRefineCheckIn(player.INVENTORY, slotIndex, AuraSlot, player.GetAuraWindowType())
						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURAITEM)
							return

					elif AuraSlot == player.AURA_SLOT_SUB:
						if player.FindUsingAuraSlot(player.AURA_SLOT_MAIN) != player.NPOS():
							if item.GetItemType() == item.ITEM_TYPE_RESOURCE and item.GetItemSubType() == item.RESOURCE_AURA:
								if UsingAuraSlot == player.AURA_SLOT_MAX:
									if AuraSlot != player.AURA_SLOT_SUB:
										return

								net.SendAuraRefineCheckIn(player.INVENTORY, slotIndex, AuraSlot, player.GetAuraWindowType())
							#else:
							#	chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURARESOURCE)
							#	return
						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_REGISTER_AURAITEM)
							return

			elif player.GetAuraWindowType() == player.AURA_WINDOW_TYPE_EVOLVE:
				if UsingAuraSlot == player.AURA_SLOT_MAX:
					if AuraSlot == player.AURA_SLOT_MAIN:
						if item.GetItemType() == item.ITEM_TYPE_COSTUME and item.GetItemSubType() == item.COSTUME_TYPE_AURA:
							socketLevelValue = player.GetItemMetinSocket(slotIndex, item.ITEM_SOCKET_AURA_LEVEL_VALUE)
							curLevel = (socketLevelValue / 100000) - 1000
							curExp = socketLevelValue % 100000;
							if curLevel >= player.AURA_MAX_LEVEL:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_MAX_LEVEL)
								return

							if curLevel != player.GetAuraRefineInfo(curLevel, item.AURA_REFINE_INFO_LEVEL_MAX) or curExp < player.GetAuraRefineInfo(curLevel, item.AURA_REFINE_INFO_NEED_EXP):
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_EVOLUTION_ITEM)
								return

							if player.FindUsingAuraSlot(AuraSlot) != player.NPOS():
								return

							net.SendAuraRefineCheckIn(player.INVENTORY, slotIndex, AuraSlot, player.GetAuraWindowType())
						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURAITEM)
							return

					elif AuraSlot == player.AURA_SLOT_SUB:
						Cell = player.FindUsingAuraSlot(player.AURA_SLOT_MAIN)
						if Cell == player.NPOS():
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_REGISTER_AURAITEM)
							return

						socketLevelValue = player.GetItemMetinSocket(*(Cell + (item.ITEM_SOCKET_AURA_LEVEL_VALUE,)))
						curLevel = (socketLevelValue / 100000) - 1000
						curExp = socketLevelValue % 100000;
						if curLevel >= player.AURA_MAX_LEVEL:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_MAX_LEVEL)
							return

						if curExp < player.GetAuraRefineInfo(curLevel, item.AURA_REFINE_INFO_NEED_EXP):
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_EVOLUTION_ITEM)
							return

						if player.GetItemIndex(slotIndex) != player.GetAuraRefineInfo(curLevel, item.AURA_REFINE_INFO_MATERIAL_VNUM):
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_EVOLUTION_ITEM)
							return

						if player.GetItemCount(slotIndex) < player.GetAuraRefineInfo(curLevel, item.AURA_REFINE_INFO_MATERIAL_COUNT):
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_EVOLUTION_ITEMCOUNT)
							return

						if UsingAuraSlot == player.AURA_SLOT_MAX:
							if AuraSlot != player.AURA_SLOT_MAX:
								if player.FindUsingAuraSlot(AuraSlot) != player.NPOS():
									return

							net.SendAuraRefineCheckIn(player.INVENTORY, slotIndex, AuraSlot, player.GetAuraWindowType())

		def OnAuraAcceptEvent(self, slotIndex, AuraSlot):
			self.OnAuraCloseEvent()
			net.SendAuraRefineCheckIn(player.INVENTORY, slotIndex, AuraSlot, player.GetAuraWindowType())

		def OnAuraCloseEvent(self):
			self.pop.Close()
			self.pop = None

		def __UseAuraClearItem(self, src_item_window, src_item_pos, dst_item_window, dst_item_pos):
			if self.clear_aura_item_question_dialog:
				return

			if app.ENABLE_SOULBIND_SYSTEM:
				if player.GetItemSealDate(dst_item_window, dst_item_pos) != item.E_SEAL_DATE_DEFAULT_TIMESTAMP:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_CLEAR_CANNOT_USE_SEALED_ITEM)
					return

			question_dialog = uiCommon.QuestionDialog()
			question_dialog.SetText(localeInfo.AURA_CLEAR_QUESTION_WHEN_USE)
			question_dialog.SetAcceptEvent(ui.__mem_func__(self.__AuraClearItemAccept))
			question_dialog.SetCancelEvent(ui.__mem_func__(self.__AuraClearItemCancel))
			question_dialog.Open()
			question_dialog.srcItemWindow = src_item_window
			question_dialog.srcItemPos = src_item_pos
			question_dialog.dstItemWindow = dst_item_window
			question_dialog.dstItemPos = dst_item_pos
			self.clear_aura_item_question_dialog = question_dialog
			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

		def __AuraClearItemAccept(self):
			if self.clear_aura_item_question_dialog:
				self.__SendUseItemToItemPacket(\
					self.clear_aura_item_question_dialog.srcItemWindow,\
					self.clear_aura_item_question_dialog.srcItemPos,\
					self.clear_aura_item_question_dialog.dstItemWindow,\
					self.clear_aura_item_question_dialog.dstItemPos)

			self.__AuraClearQuestionDialogClose()

		def __AuraClearItemCancel(self):
			self.__AuraClearQuestionDialogClose()

		def __AuraClearQuestionDialogClose(self):
			if self.clear_aura_item_question_dialog:
				self.clear_aura_item_question_dialog.Close()
			self.clear_aura_item_question_dialog = None
			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	if app.ENABLE_GEM_CONVERTER:
		def __CanUseGemConverter(self, src_item_vnum, src_item_window, src_item_pos, dst_item_window, dst_item_pos):
			if src_item_window == dst_item_window and src_item_pos == dst_item_pos:
				return False

			if src_item_vnum != item.ITEM_VNUM_GEM_CONVERTER:
				return False

			item.SelectItem(src_item_vnum)

			gem_conv_count = player.GetItemCount(src_item_window, src_item_pos)
			gem_stone_vnum = player.GetItemIndex(dst_item_window, dst_item_pos)
			gem_stone_count = player.GetItemCount(dst_item_window, dst_item_pos)

			need_gem_stone_count = gem_conv_count * item.GetValue(0)

			if gem_stone_vnum != item.ITEM_VNUM_GEM_STONE:
				return False

			if gem_stone_count < need_gem_stone_count:
				return False

			return True

		def __UseGemConverter(self, src_item_window, src_item_pos, dst_item_window, dst_item_pos):
			if self.gem_converter_question_dialog:
				return

			self.gem_converter_item_pos = (src_item_pos, dst_item_pos)

			self.SetCantMouseEventSlot(src_item_pos)
			self.SetCantMouseEventSlot(dst_item_pos)

			question_dialog = uiCommon.QuestionDialog()
			question_dialog.SetText(localeInfo.GEM_CONVERTER_MESSAGE_USE_CONVERTER_QUESTION)
			question_dialog.SetAcceptEvent(ui.__mem_func__(self.__AcceptGemConverterUse))
			question_dialog.SetCancelEvent(ui.__mem_func__(self.__CancelGemConverterUse))
			question_dialog.Open()
			question_dialog.srcItemWindow = src_item_window
			question_dialog.srcItemPos = src_item_pos
			question_dialog.dstItemWindow = dst_item_window
			question_dialog.dstItemPos = dst_item_pos
			self.gem_converter_question_dialog = question_dialog
			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

		def __AcceptGemConverterUse(self):
			if not self.gem_converter_question_dialog:
				return

			gem_conv_vnum = player.GetItemIndex(self.gem_converter_question_dialog.srcItemWindow, self.gem_converter_question_dialog.srcItemPos)
			gem_conv_count = player.GetItemCount(self.gem_converter_question_dialog.srcItemWindow, self.gem_converter_question_dialog.srcItemPos)
			gem_stone_count = player.GetItemCount(self.gem_converter_question_dialog.dstItemWindow, self.gem_converter_question_dialog.dstItemPos)

			item.SelectItem(gem_conv_vnum)
			need_gem_stone_count = gem_conv_count * item.GetValue(0)
			conv_to_gem = need_gem_stone_count / item.GetValue(0)
			conv_cost = gem_conv_count * item.GetValue(1)

			if gem_stone_count < need_gem_stone_count:
				self.__CancelGemConverterUse()
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GEM_CONVERTER_MESSAGE_NOT_ENOUGH_GEMSTONE % localeInfo.NumberToMoneyString(need_gem_stone_count))
				return

			if player.GetElk() < conv_cost:
				self.__CancelGemConverterUse()
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GEM_CONVERTER_MESSAGE_NOT_ENOUGH_GOLD)
				return

			if player.GetGem() + conv_to_gem >= player.GEM_MAX:
				self.__CancelGemConverterUse()
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GEM_CONVERTER_MESSAGE_GEM_MAX)
				return

			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GEM_CONVERTER_MESSAGE_CONVERT_SUCCESS_GEM_STONE % gem_conv_count)
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GEM_CONVERTER_MESSAGE_CONVERT_SUCCESS_CONVERTER % need_gem_stone_count)

			self.__SendUseItemToItemPacket(\
				self.gem_converter_question_dialog.srcItemWindow,\
				self.gem_converter_question_dialog.srcItemPos,\
				self.gem_converter_question_dialog.dstItemWindow,\
				self.gem_converter_question_dialog.dstItemPos)

			self.__CancelGemConverterUse()

		def __CancelGemConverterUse(self):
			for i in self.gem_converter_item_pos:
				self.SetCanMouseEventSlot(i)
			self.gem_converter_item_pos = [-1, -1]

			self.__CloseGemConverterUseQuestionDialog()

		def __CloseGemConverterUseQuestionDialog(self):
			if self.gem_converter_question_dialog:
				self.gem_converter_question_dialog.Close()
			self.gem_converter_question_dialog = None
			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	def __UseItemQuestionDialog_OnCancel(self):
		self.OnCloseQuestionDialog()

	def __UseItemQuestionDialog_OnAccept(self):
		self.__SendUseItemPacket(self.questionDialog.slotIndex)
		self.OnCloseQuestionDialog()

	def __SendUseItemToItemPacket(self, srcSlotWindow, srcSlotPos, dstSlotWindow, dstSlotPos):
		# 개인상점 열고 있는 동안 아이템 사용 방지
		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_ITEM_FAILURE_PRIVATE_SHOP)
			return

		#if self.interface.IsShowDlgQuestionWindow():
		#	chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DONT_USE_ITEM_WHEN_SHOW_CONFIRM)
		#	return

		net.SendItemUseToItemPacket(srcSlotWindow, srcSlotPos, dstSlotWindow, dstSlotPos)

	def __SendUseItemPacket(self, slotPos, slotWindow = player.INVENTORY):
		# 개인상점 열고 있는 동안 아이템 사용 방지
		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_ITEM_FAILURE_PRIVATE_SHOP)
			return

		#if self.interface.IsShowDlgQuestionWindow():
		#	chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DONT_USE_ITEM_WHEN_SHOW_CONFIRM)
		#	return

		net.SendItemUsePacket(slotWindow, slotPos)

	def __SendMoveItemPacket(self, srcSlotWindow, srcSlotPos, dstSlotWindow, dstSlotPos, srcItemCount):
		# 개인상점 열고 있는 동안 아이템 사용 방지
		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOVE_ITEM_FAILURE_PRIVATE_SHOP)
			return

		#if self.interface.IsShowDlgQuestionWindow():
		#	chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DONT_USE_ITEM_WHEN_SHOW_CONFIRM)
		#	return

		net.SendItemMovePacket(srcSlotWindow, srcSlotPos, dstSlotWindow, dstSlotPos, srcItemCount)

	def SetDragonSoulRefineWindow(self, DragonSoulRefine):
		from _weakref import proxy
		self.wndDragonSoulRefine = proxy(DragonSoulRefine)

	def OnMoveWindow(self, x, y):
		if app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
			pass
		else:
			if self.wndBelt:
				self.wndBelt.AdjustPosition()

	# 악세서리 슬롯 테두리 추가, 제거.
	## HilightSlot Change
	def DeactivateSlot(self, slotindex, type):
		if type == wndMgr.HILIGHTSLOT_MAX:
			return

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			if type == wndMgr.HILIGHTSLOT_ACCE:
				self.__DelHighlightSlotAcce(slotindex)

		if app.ENABLE_CHANGE_LOOK_SYSTEM:
			if type == wndMgr.HILIGHTSLOT_CHANGE_LOOK:
				self.__DelHighlightSlotChangeLook(slotindex)

		if app.ENABLE_AURA_COSTUME_SYSTEM:
			if type == wndMgr.HILIGHTSLOT_AURA:
				self.__DelHighlightSlotAura(slotindex)

		if app.ENABLE_CUBE_RENEWAL:
			if type == wndMgr.HILIGHTSLOT_CUBE:
				self.__DelHighlightSlotCube(slotindex)

	## HilightSlot Change
	def ActivateSlot(self, slotindex, type):
		if type == wndMgr.HILIGHTSLOT_MAX:
			return

		#if app.WJ_ENABLE_PICKUP_ITEM_EFFECT:
		#	self.DelHighlightSlot(slotindex)

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			if type == wndMgr.HILIGHTSLOT_ACCE:
				self.__AddHighlightSlotAcce(slotindex)

		if app.ENABLE_CHANGE_LOOK_SYSTEM:
			if type == wndMgr.HILIGHTSLOT_CHANGE_LOOK:
				self.__AddHighlightSlotChangeLook(slotindex)

		if app.ENABLE_AURA_COSTUME_SYSTEM:
			if type == wndMgr.HILIGHTSLOT_AURA:
				self.__AddHighlightSlotAura(slotindex)

		if app.ENABLE_CUBE_RENEWAL:
			if type == wndMgr.HILIGHTSLOT_CUBE:
				self.__AddHighlightSlotCube(slotindex)

	if app.ENABLE_ACCE_COSTUME_SYSTEM:
		## 악세서리 용 하이라이트 list 추가.
		def __AddHighlightSlotAcce(self, slotIndex):
			if not slotIndex in self.listHighlightedAcceSlot:
				self.listHighlightedAcceSlot.append(slotIndex)

		## 악세서리 용 하이라이트 list 삭제.
		def __DelHighlightSlotAcce(self, slotIndex):
			if slotIndex in self.listHighlightedAcceSlot:
				if slotIndex >= player.INVENTORY_PAGE_SIZE:
					self.wndItem.DeactivateSlot(slotIndex - (self.inventoryPageIndex * player.INVENTORY_PAGE_SIZE))
					self.wndItem.SetCanMouseEventSlot(slotIndex - (self.inventoryPageIndex * player.INVENTORY_PAGE_SIZE))
				else:
					self.wndItem.DeactivateSlot(slotIndex)
					self.wndItem.SetCanMouseEventSlot(slotIndex)

				self.listHighlightedAcceSlot.remove(slotIndex)

	if app.ENABLE_CHANGE_LOOK_SYSTEM:
		def __AddHighlightSlotChangeLook(self, slotIndex):
			if not slotIndex in self.listHighlightedChangeLookSlot:
				self.listHighlightedChangeLookSlot.append(slotIndex)

		def __DelHighlightSlotChangeLook(self, slotIndex):
			if slotIndex in self.listHighlightedChangeLookSlot:
				if slotIndex >= player.INVENTORY_PAGE_SIZE:
					self.wndItem.DeactivateSlot(slotIndex - (self.inventoryPageIndex * player.INVENTORY_PAGE_SIZE))
					self.wndItem.SetCanMouseEventSlot(slotIndex - (self.inventoryPageIndex * player.INVENTORY_PAGE_SIZE))
				else:
					self.wndItem.DeactivateSlot(slotIndex)
					self.wndItem.SetCanMouseEventSlot(slotIndex)
				self.listHighlightedChangeLookSlot.remove(slotIndex)

	if app.ENABLE_CUBE_RENEWAL:
		def __AddHighlightSlotCube(self, slotIndex):
			if not slotIndex in self.listHighlightedCubeSlot:
				self.listHighlightedCubeSlot.append(slotIndex)

		def __DelHighlightSlotCube(self, slotIndex):
			if slotIndex in self.listHighlightedCubeSlot:
				if slotIndex >= player.INVENTORY_PAGE_SIZE:
					self.wndItem.DeactivateSlot(slotIndex - (self.inventoryPageIndex * player.INVENTORY_PAGE_SIZE))
				else:
					self.wndItem.DeactivateSlot(slotIndex)
				self.listHighlightedCubeSlot.remove(slotIndex)

	if app.ENABLE_AURA_COSTUME_SYSTEM:
		def __AddHighlightSlotAura(self, slotIndex):
			if not slotIndex in self.listHighlightedAuraSlot:
				self.listHighlightedAuraSlot.append(slotIndex)

		def __DelHighlightSlotAura(self, slotIndex):
			if slotIndex in self.listHighlightedAuraSlot:
				if slotIndex >= player.INVENTORY_PAGE_SIZE:
					self.wndItem.DeactivateSlot(slotIndex - (self.inventoryPageIndex * player.INVENTORY_PAGE_SIZE))
					self.wndItem.SetCanMouseEventSlot(slotIndex - (self.inventoryPageIndex * player.INVENTORY_PAGE_SIZE))
				else:
					self.wndItem.DeactivateSlot(slotIndex)
					self.wndItem.SetCanMouseEventSlot(slotIndex)
				self.listHighlightedAuraSlot.remove(slotIndex)

	## 하이라이트 리프레쉬.
	def __HighlightSlot_Refresh(self):
		## 악세서리.
		for i in xrange(self.wndItem.GetSlotCount()):
			slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)

			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				if slotNumber in self.listHighlightedAcceSlot:
					self.wndItem.ActivateSlot(i)
					self.wndItem.SetSlotDiffuseColor(i, wndMgr.COLOR_TYPE_GREEN)
					self.wndItem.SetCantMouseEventSlot(i)

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				if slotNumber in self.listHighlightedChangeLookSlot:
					self.wndItem.ActivateSlot(i)
					self.wndItem.SetSlotDiffuseColor(i, wndMgr.COLOR_TYPE_RED)
					self.wndItem.SetCantMouseEventSlot(i)

			if app.ENABLE_AURA_COSTUME_SYSTEM:
				if slotNumber in self.listHighlightedAuraSlot:
					self.wndItem.ActivateSlot(i)
					self.wndItem.SetSlotDiffuseColor(i, wndMgr.COLOR_TYPE_GREEN)
					self.wndItem.SetCantMouseEventSlot(i)

			if app.ENABLE_CUBE_RENEWAL:
				if slotNumber in self.listHighlightedCubeSlot:
					self.wndItem.ActivateSlot(i)
					self.wndItem.SetSlotDiffuseColor(i, wndMgr.COLOR_TYPE_RED)

			## HilightSlot Change
			if app.WJ_ENABLE_PICKUP_ITEM_EFFECT:
				if slotNumber in self.listHighlightedSlot:
					self.wndItem.ActivateSlot(i)
					self.wndItem.SetSlotDiffuseColor(i, wndMgr.COLOR_TYPE_MAX)

	## 악세서리 용 하이라이트 list 클리어.
	def __HighlightSlot_Clear(self):
		## 악세서리
		for i in xrange(self.wndItem.GetSlotCount()):
			slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)

			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				if slotNumber in self.listHighlightedAcceSlot:
					self.wndItem.DeactivateSlot(i)
					self.wndItem.SetCanMouseEventSlot(i)
					self.listHighlightedAcceSlot.remove(slotNumber)

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				if slotNumber in self.listHighlightedChangeLookSlot:
					self.wndItem.DeactivateSlot(i)
					self.wndItem.SetCanMouseEventSlot(i)
					self.listHighlightedChangeLookSlot.remove(slotNumber)

			if app.ENABLE_AURA_COSTUME_SYSTEM:
				if slotNumber in self.listHighlightedAuraSlot:
					self.wndItem.DeactivateSlot(i)
					self.wndItem.SetCanMouseEventSlot(i)
					self.listHighlightedAuraSlot.remove(slotNumber)

			if app.ENABLE_CUBE_RENEWAL:
				if slotNumber in self.listHighlightedCubeSlot:
					self.wndItem.DeactivateSlot(i)
					self.listHighlightedCubeSlot.remove(slotNumber)

			## HilightSlot Change
			if app.WJ_ENABLE_PICKUP_ITEM_EFFECT:
				if slotNumber in self.listHighlightedSlot:
					self.wndItem.DeactivateSlot(i)
					self.listHighlightedSlot.remove(slotNumber)

	if app.WJ_ENABLE_PICKUP_ITEM_EFFECT:
		# 슬롯 highlight 관련
		## 추가
		def HighlightSlot(self, slot):
			#slot값에 대한 예외처리.
			if slot > player.INVENTORY_PAGE_SIZE * player.INVENTORY_PAGE_COUNT:
				return

			if not slot in self.listHighlightedSlot:
				self.listHighlightedSlot.append(slot)

		## 삭제
		def DelHighlightSlot(self, inventorylocalslot):
			if inventorylocalslot in self.listHighlightedSlot:
				if inventorylocalslot >= player.INVENTORY_PAGE_SIZE:
					self.wndItem.DeactivateSlot(inventorylocalslot - (self.inventoryPageIndex * player.INVENTORY_PAGE_SIZE))
				else:
					self.wndItem.DeactivateSlot(inventorylocalslot)

				self.listHighlightedSlot.remove(inventorylocalslot)

		# 슬롯 highlight 관련 끝

	# wj.2014.12.2. 인벤토리와 DS인벤 간의 상태를 확인 및 설정하기 위한 함수.
	def IsDlgQuestionShow(self):
		if self.dlgQuestion.IsShow():
			return True
		else:
			return False

	def CancelDlgQuestion(self):
		self.__Cancel()

	def SetUseItemMode(self, bUse):
		self.wndItem.SetUseMode(bUse)

	if app.ENABLE_MOVE_COSTUME_ATTR or app.ENABLE_CHEQUE_SYSTEM or app.ENABLE_ATTR_6TH_7TH:
		def GetInventoryPageIndex(self):
			## 0 or 1
			return self.inventoryPageIndex

	if app.ENABLE_CHEQUE_SYSTEM:
		def OverInToolTip(self, arg) :
			arglen = len(str(arg))
			pos_x, pos_y = wndMgr.GetMousePosition()

			self.toolTip.ClearToolTip()
			self.toolTip.SetThinBoardSize(11 * arglen)
			self.toolTip.SetToolTipPosition(pos_x + 5, pos_y - 5)
			self.toolTip.AppendTextLine(arg, 0xffffff00)
			self.toolTip.Show()

		def OverOutToolTip(self) :
			self.toolTip.Hide()

		def EventProgress(self, event_type, idx):
			if "mouse_over_in" == str(event_type):
				if idx == 0:
					self.OverInToolTip(localeInfo.CHEQUE_SYSTEM_UNIT_YANG)
				elif idx == 1:
					self.OverInToolTip(localeInfo.CHEQUE_SYSTEM_UNIT_WON)
				elif app.ENABLE_GEM_SYSTEM and idx == 2:
					self.OverInToolTip(localeInfo.GEM_SYSTEM_NAME)
				else:
					return
			elif "mouse_over_out" == str(event_type):
				self.OverOutToolTip()
			else:
				return

	if app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
		def BindEquipmentWindow(self, equipment_window):
			self.wndEquipment = equipment_window

	def UseInventoryItemInQuickSlot(self, global_slot_index):
		self.__UseItem(global_slot_index, player.INVENTORY)

	if app.ENABLE_HIDE_COSTUME_SYSTEM:
		def SetHiddenCostumePart(self, part, hidden):
			if app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
				if self.wndEquipment:
					self.wndEquipment.SetHiddenCostumePart(part, hidden)
			else:
				if self.wndCostume:
					self.wndCostume.SetHiddenCostumePart(part, hidden)
