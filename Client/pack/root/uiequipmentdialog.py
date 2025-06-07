import ui
import chr
import player
import app

TYPE_INDEX_ITEM_VNUM = 0
TYPE_INDEX_ITEM_COUNT = 1
TYPE_INDEX_SOCKET = 2
TYPE_INDEX_ATTR = 3
TYPE_INDEX_RARE_ATTR = 4
TYPE_INDEX_CHANGE_LOOK = 5
TYPE_INDEX_REFINE_ELEMENT = 6
TYPE_INDEX_APPLY_RANDOM = 7
TYPE_INDEX_SET_VALUE = 8

class CostumeWindow(ui.ScriptWindow):
	def __init__(self, wnd_equip):
		ui.ScriptWindow.__init__(self)
		self.wnd_equip = wnd_equip
		self.costume_slot = None
		self.is_loaded = False

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadWindow(self):
		if self.is_loaded == True:
			return

		self.is_loaded = True

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/CostumeWindow.py")

			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
			self.board = self.GetChild("board")
			self.costume_slot = self.GetChild("CostumeSlot")
		except:
			import exception
			exception.Abort("CostumeWindow.LoadWindow.LoadObject")

		self.costume_slot.SetOverInItemEvent(ui.__mem_func__(self.wnd_equip.OverInItem), player.EQUIPMENT)
		self.costume_slot.SetOverOutItemEvent(ui.__mem_func__(self.wnd_equip.OverOutItem))

	def Show(self):
		self.__LoadWindow()
		ui.ScriptWindow.Show(self)

	def Close(self):
		self.Hide()

	def SetCostumeSlot(self, slot_index, item_vnum, change_look_vnum):
		self.costume_slot.SetItemSlot(slot_index, item_vnum, 0)
		if app.ENABLE_CHANGE_LOOK_SYSTEM:
			if not change_look_vnum == 0:
				self.costume_slot.SetSlotCoverImage(slot_index, "icon/item/ingame_convert_Mark.tga")
			else:
				self.costume_slot.EnableSlotCoverImage(slot_index, False)

	def AdjustPosition(self, x, y):
		self.SetPosition(x - self.board.GetWidth(), y)

class EquipmentDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.__Initialize()
		self.__LoadDialog()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __Initialize(self):
		self.vid = 0
		self.event_close = None
		self.item_data_dict = {}
		self.tooltip_item = None
		self.wnd_costume = None

	def __LoadDialog(self):
		try:
			PythonScriptLoader = ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self, "UIScript/EquipmentDialog.py")

			self.board = self.GetChild("board")
			self.equipment_slot = self.GetChild("equipment_slot")
			self.unique_slot = self.GetChild("unique_slot")

			self.GetChild("tab_img_01").Hide()
			self.GetChild("tab_img_02").Hide()
			self.GetChild("tab_btn_01").Hide()
			self.GetChild("tab_btn_02").Hide()

			self.GetChild("dragon_soul_button").Hide()
			self.GetChild("mall_button").Hide()
			self.GetChild("premium_private_shop_button").Hide()
			self.GetChild("costume_button").SetEvent(ui.__mem_func__(self.__ShowCostumeInventory))
		except:
			import exception
			exception.Abort("EquipmentDialog.LoadDialog.BindObject")

		self.board.SetCloseEvent(ui.__mem_func__(self.Close))

		self.equipment_slot.SetSlotType(player.SLOT_TYPE_EQUIPMENT)
		self.equipment_slot.SetOverInItemEvent(ui.__mem_func__(self.OverInItem), player.EQUIPMENT)
		self.equipment_slot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

		self.unique_slot.SetSlotType(player.SLOT_TYPE_EQUIPMENT)
		self.unique_slot.SetOverInItemEvent(ui.__mem_func__(self.OverInItem), player.EQUIPMENT)
		self.unique_slot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

		self.__CreateCostumeWindow()

	def __CreateCostumeWindow(self):
		self.wnd_costume = CostumeWindow(self)

	def __EquipmentPageTabSetting(self, page_index):
		pass

	def __RefreshEquipment(self):
		for slot_index, item in self.item_data_dict.items():
			vnum = item[TYPE_INDEX_ITEM_VNUM]
			if vnum == 0:
				continue

			count = item[TYPE_INDEX_ITEM_COUNT]
			if count <= 0:
				continue

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				change_look_vnum = item[TYPE_INDEX_CHANGE_LOOK]

			self.equipment_slot.SetItemSlot(slot_index, vnum)
			self.unique_slot.SetItemSlot(slot_index, vnum)

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				if not change_look_vnum == 0:
					self.equipment_slot.SetSlotCoverImage(slot_index, "icon/item/ingame_convert_Mark.tga")
				else:
					self.equipment_slot.EnableSlotCoverImage(slot_index, False)

			if self.wnd_costume:
				self.wnd_costume.SetCostumeSlot(slot_index, vnum, change_look_vnum)

	def __ShowCostumeInventory(self):
		if self.wnd_costume:
			if not self.wnd_costume.IsShow():
				self.wnd_costume.Show()
			else:
				self.wnd_costume.Hide()
		else:
			self.wnd_costume = CostumeWindow(self)
			self.wnd_costume.Show()

	def Open(self, vid):
		self.vid = vid
		self.item_data_dict = {}

		name = chr.GetNameByVID(vid)
		self.board.SetTitleName(name)

		self.SetCenterPosition()
		self.SetTop()
		self.Show()

		(x, y) = self.GetLocalPosition()
		if self.wnd_costume:
			self.wnd_costume.AdjustPosition(x, y)

	def Close(self):
		self.item_data_dict = {}
		self.tooltip_item = None
		self.Hide()

		if self.wnd_costume:
			self.wnd_costume.Close()

		if self.event_close:
			self.event_close(self.vid)

	def Destroy(self):
		self.event_close = None

		self.Close()
		self.ClearDictionary()

		self.board = None
		self.equipment_slot = None

	def SetEquipmentDialogItem(self, window, slot_index, vnum, count, change_look_vnum, refine_element, apply_random_list, set_value):
		empty_socket_list = [0 for i in xrange(player.METIN_SOCKET_MAX_NUM)]
		empty_attr_list = [(0, 0) for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]

		self.item_data_dict[slot_index] = (vnum, count, empty_socket_list, empty_attr_list, 0, change_look_vnum, refine_element, apply_random_list, set_value)

		self.__RefreshEquipment()

	def SetEquipmentDialogSocket(self, window, slot_index, socket_index, value):
		if not slot_index in self.item_data_dict:
			return

		if socket_index < 0 or socket_index > player.METIN_SOCKET_MAX_NUM:
			return

		self.item_data_dict[slot_index][TYPE_INDEX_SOCKET][socket_index] = value

	def SetEquipmentDialogAttr(self, window, slot_index, attr_index, type, value):
		if not slot_index in self.item_data_dict:
			return

		if attr_index < 0 or attr_index > player.ATTRIBUTE_SLOT_MAX_NUM:
			return

		self.item_data_dict[slot_index][TYPE_INDEX_ATTR][attr_index] = (type, value)

	def SetItemToolTip(self, tooltip_item):
		self.tooltip_item = tooltip_item

	def SetCloseEvent(self, event):
		self.event_close = event

	def OverInItem(self, slot_index, window):
		if None == self.tooltip_item:
			return

		if not slot_index in self.item_data_dict:
			return

		item_vnum = self.item_data_dict[slot_index][TYPE_INDEX_ITEM_VNUM]
		if 0 == item_vnum:
			return

		self.tooltip_item.ClearToolTip()

		socket_list = self.item_data_dict[slot_index][TYPE_INDEX_SOCKET]
		attr_list = self.item_data_dict[slot_index][TYPE_INDEX_ATTR]
		refine_element = self.item_data_dict[slot_index][TYPE_INDEX_REFINE_ELEMENT]
		apply_random_list = self.item_data_dict[slot_index][TYPE_INDEX_APPLY_RANDOM]
		set_value = self.item_data_dict[slot_index][TYPE_INDEX_SET_VALUE]

		self.tooltip_item.AddItemData(item_vnum, socket_list, attr_list, None, 0, 0, window, -1, refine_element, apply_random_list, set_value)
		if app.ENABLE_CHANGE_LOOK_SYSTEM:
			self.tooltip_item.AppendChangeLookInfoItemVnum(self.item_data_dict[slot_index][TYPE_INDEX_CHANGE_LOOK])

		self.tooltip_item.ShowToolTip()

	def OverOutItem(self):
		if self.tooltip_item:
			self.tooltip_item.HideToolTip()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def OnMoveWindow(self, x, y):
		if self.wnd_costume:
			self.wnd_costume.AdjustPosition(x, y)
