#
# Filename: uiCubeRenewal.py
# Copyright (C) 2023 Owsap Development
#
# Author: Owsap
# Website: https://owsap.dev/
#

import app
import ime
import grp
import snd
import mouseModule
import wndMgr
import localeInfo
import ui
import uiCommon
import uiToolTip
import net
import player
import item
import chat

from collections import OrderedDict
import re

ROOT_PATH = "d:/ymir work/ui/game/cube/"

MAX_MAKE_QUANTITY = player.CUBE_MAX_MAKE_QUANTITY
MAX_MATERIAL_QUANTITY = player.CUBE_MAX_MATERIAL_QUANTITY
MAX_MATERIAL_COUNT = player.CUBE_MAX_MATERIAL_COUNT

COLOR_DEFAULT = 4291348680L
COLOR_SELECT = 4293121989L

BELT_IMPROVE_ITEM_VNUM = (79605,)
MAX_BELT_IMPROVE_ITEM_QUANTITY = 40

class ListItem(ui.Window):
	LIST_TAB_FIRST = 1
	LIST_TAB_SECCOND = 2
	LIST_TAB_THIRD = 3
	LIST_TAB_COUNT = 11
	LIST_SCROLL_STEP = 1.0
	LIST_BOARD_HEIGHT = 227

	def __init__(self, parent):
		ui.Window.__init__(self)
		self.parent = parent

		self.SetParent(parent.item_list_board)
		self.SetSize(305, 17)
		self.SetTab(0)

		self.button = ui.ImageBox()
		self.button.SetParent(self)
		self.button.SetEvent(ui.__mem_func__(self.SelectItem), "mouse_click")
		self.button.Show()

		self.image = ui.ImageBox()
		self.image.AddFlag("not_pick")
		self.image.SetParent(self)
		if localeInfo.IsARABIC():
			self.image.SetWindowHorizontalAlignRight()
			self.image.SetPosition(15, 3)
		else:
			self.image.SetPosition(7, 3)
		self.image.Show()

		self.text = ui.TextLine()
		self.text.SetParent(self)
		if localeInfo.IsARABIC():
			self.text.SetWindowHorizontalAlignRight()
			self.text.SetPosition(40, 1)
		else:
			self.text.SetPosition(40, 2)
		self.text.SetOutline(True)
		self.text.Show()

	def __del__(self):
		ui.Window.__del__(self)

	def SetName(self, name):
		self.name = name
		self.text.SetText(name)

	def GetName(self):
		return self.name

	def SetTab(self, type):
		self.type = type

	def GetTab(self):
		return self.type

	def SelectItem(self):
		self.parent.OnSelectItem(self)

class ItemCategoryGroup(ListItem):
	def __init__(self, parent):
		ListItem.__init__(self, parent)
		self.SetTab(ListItem.LIST_TAB_FIRST)

		self.is_open = False
		self.detail_group_list = []
		self.detail_group_name_dict = {}

		self.button.LoadImage(ROOT_PATH + "cube_menu_tab1.sub")
		if localeInfo.IsARABIC():
			self.button.LeftRightReverse()

	def __del__(self):
		ListItem.__del__(self)

	def Destroy(self):
		for detail_group in self.detail_group_list:
			detail_group.Destroy()

		for detail_group in self.detail_group_name_dict.values():
			detail_group.Destroy()

		self.detail_group_list = []
		self.detail_group_name_dict = {}

	def AddItem(self, name):
		detail_group = ItemDetailGroup(self.parent)
		detail_group.SetName(name)
		detail_group.SelectItem()

		self.detail_group_list.append(detail_group)
		self.detail_group_name_dict[name] = detail_group

		return self.detail_group_list[len(self.detail_group_list) - 1]

	def GetDetailGroupList(self):
		return self.detail_group_list

	def GetDetailGroupName(self, name):
		return name in self.detail_group_name_dict

	def GetDetailGroupByName(self, name):
		if name in self.detail_group_name_dict:
			return self.detail_group_name_dict[name]
		return None

	def ShowOpenedImg(self):
		self.image.LoadImage(ROOT_PATH + "cube_menu_tab1_minus.sub")
		self.is_open = True

	def ShowClosedImg(self):
		self.image.LoadImage(ROOT_PATH + "cube_menu_tab1_plus.sub")
		self.is_open = False
		map(ui.Window.Hide, self.detail_group_list)

	def IsOpen(self):
		return self.is_open

	def SelectItem(self):
		if self.is_open:
			self.ShowClosedImg()
		else:
			self.ShowOpenedImg()

		for detail_group in self.GetDetailGroupList():
			detail_group.SelectItem()

		self.parent.RefreshSelectItem()

	def RefreshPossibleMakeQuantity(self):
		for detail_group in self.GetDetailGroupList():
			detail_group.RefreshPossibleMakeQuantity()

class ItemDetailGroup(ListItem):
	def __init__(self, parent):
		ListItem.__init__(self, parent)
		self.SetTab(ListItem.LIST_TAB_SECCOND)

		self.is_open = False
		self.detail_object_list = []

		self.__CreateObject()

	def __del__(self):
		ListItem.__del__(self)

	def Destroy(self):
		for detail_object in self.detail_object_list:
			detail_object.Destroy()

		self.detail_object_list = []

	def __CreateObject(self):
		self.button.SetEvent(ui.__mem_func__(self.ClickFunc), "mouse_click")
		self.button.LoadImage(ROOT_PATH + "cube_menu_tab2.sub")
		if localeInfo.IsARABIC():
			self.button.LeftRightReverse()

		if localeInfo.IsARABIC():
			self.image.SetPosition(44, 4)
			self.text.SetPosition(52, 1)
		else:
			self.image.SetPosition(7, 4)
			self.text.SetPosition(25, 2)

		self.text.SetPackedFontColor(0xffc2a046)

	def AddItem(self, name, cube_item):
		detail_object = ItemDetailObject(self, self.parent, cube_item)
		detail_object.SetName(name)
		self.detail_object_list.append(detail_object)

	def ShowOpenedImg(self):
		self.image.LoadImage(ROOT_PATH + "cube_menu_tab2_minus.sub")
		self.is_open = True

	def ShowClosedImg(self):
		self.image.LoadImage(ROOT_PATH + "cube_menu_tab2_plus.sub")
		self.is_open = False
		map(ui.Window.Hide, self.detail_object_list)

	def IsOpen(self):
		return self.is_open

	def GetDetailObjectList(self):
		return self.detail_object_list

	def SelectItem(self):
		if self.is_open:
			self.ShowClosedImg()
		else:
			self.ShowOpenedImg()

	def ClickFunc(self):
		self.SelectItem()
		self.parent.RefreshSelectItem()

	def SetOneTextLine(self, item_text):
		self.text.SetText(item_text)

	def SetTwoTextLine(self, quantity_text, item_text):
		if localeInfo.IsARABIC():
			self.text.SetText("%s|cff89b88d%s|h|r" % (quantity_text, item_text))
		else:
			self.text.SetText("|cff89b88d%s|h|r %s" % (quantity_text, item_text))

	def RefreshPossibleMakeQuantity(self):
		for detail_object in self.detail_object_list:
			detail_object.RefreshPossibleMakeQuantity()

class ItemDetailObject(ListItem):
	def __init__(self, group, parent, cube_item):
		ListItem.__init__(self, parent)
		self.SetTab(ListItem.LIST_TAB_THIRD)

		self.is_toggle = False
		self.group = group
		self.cube_item = cube_item
		self.make_count = float("inf")

		self.__CreateObject()

	def __del__(self):
		ListItem.__del__(self)
		self.Destroy()

	def Destroy(self):
		self.group = None
		self.cube_item = {}

	def __CreateObject(self):
		if localeInfo.IsARABIC():
			self.text.SetPosition(64, 1)
		else:
			self.text.SetPosition(17, 2)

		self.button.SetEvent(ui.__mem_func__(self.__OnMouseLeftButtonUp), "mouse_click")
		self.button.SetEvent(ui.__mem_func__(self.__OnMouseOverIn), "mouse_over_in")
		self.button.SetEvent(ui.__mem_func__(self.__OnMouseOverOut), "mouse_over_out")
		self.button.LoadImage(ROOT_PATH + "cube_menu_tab3_default.sub")
		if localeInfo.IsARABIC():
			self.button.LeftRightReverse()

	def EnableItem(self, select):
		if not select:
			self.ShowDefaultImg()
			self.is_toggle = False
		else:
			self.ShowSelectedImg()
			self.is_toggle = True

	def __OnMouseLeftButtonUp(self):
		if self.cube_item and not self.is_toggle:
			self.parent.SelectItem(self, self.cube_item)
		else:
			self.parent.ClearCubeData()

	def __OnMouseOverIn(self):
		if not self.is_toggle:
			self.ShowSelectedImg()

	def __OnMouseOverOut(self):
		if not self.is_toggle:
			self.ShowDefaultImg()

	def ShowDefaultImg(self):
		self.button.LoadImage(ROOT_PATH + "cube_menu_tab3_default.sub")
		if localeInfo.IsARABIC():
			self.button.LeftRightReverse()

	def ShowSelectedImg(self):
		self.button.LoadImage(ROOT_PATH + "cube_menu_tab3_select.sub")
		if localeInfo.IsARABIC():
			self.button.LeftRightReverse()

	def SetOneTextLine(self, item_text):
		if self.is_toggle and self.parent.pct_increase_item_pos != -1:
			item_text += "|cff89b88d+%d%%|h|r"% self.parent.pct_increase_item_count

		self.text.SetText(item_text)

	def SetTwoTextLine(self, quantity_text, item_text):
		if self.is_toggle and self.parent.pct_increase_item_pos != -1:
			item_text += "|cff89b88d+%d%%|h|r" % self.parent.pct_increase_item_count

		if localeInfo.IsARABIC():
			self.text.SetText("%s|cff89b88d%s|h|r" % (quantity_text, item_text))
		else:
			self.text.SetText("|cff89b88d%s|h|r %s" % (quantity_text, item_text))

	def RefreshPossibleMakeQuantity(self):
		self.make_count = float("inf")

		for item in self.cube_item["item"]:
			material_vnum = item["vnum"]
			material_count = item["count"]

			if app.ENABLE_SET_ITEM:
				player_count = player.GetItemCountByVnum(material_vnum, True)
			else:
				player_count = player.GetItemCountByVnum(material_vnum)

			if player_count == 0:
				self.make_count = 0
				break

			if material_count > 0:
				craftable_quantity = player_count // material_count
				self.make_count = min(self.make_count, craftable_quantity)

				if self.make_count * material_count >= MAX_MATERIAL_QUANTITY:
					self.make_count = MAX_MATERIAL_QUANTITY // material_count
				else:
					self.make_count = self.cube_item["reward"]["count"] * self.make_count

		if self.make_count > MAX_MAKE_QUANTITY:
			self.make_count = int(MAX_MAKE_QUANTITY / self.cube_item["reward"]["count"]) * self.cube_item["reward"]["count"]

		if self.make_count:
			self.group.SetTwoTextLine(localeInfo.CUBE_LIST_TEXT_MAKE_QUANTITY % self.make_count, self.group.name)
			self.SetTwoTextLine(localeInfo.CUBE_LIST_TEXT_MAKE_QUANTITY % self.make_count, self.name)
		else:
			self.group.SetOneTextLine(self.group.name)
			self.SetOneTextLine(self.name)

	def GetPossibleMakeQuantity(self):
		if app.ENABLE_SET_ITEM:
			if player.IsCubeSetAddCategory(self.cube_item["category"]) and self.cube_item["set_value"]:
				if self.make_count > 0:
					return 1

		return self.make_count

class CubeWindow(ui.ScriptWindow):
	def __init__(self):
		self.CATEGORY_NAME_DICT = {
			player.CUBE_ARMOR : localeInfo.CUBE_CATEGORY_ARMOR,
			player.CUBE_WEAPON : localeInfo.CUBE_CATEGORY_WEAPON,
			player.CUBE_ACCESSORY : localeInfo.CUBE_CATEGORY_ACCESSORY,
			player.CUBE_BELT : localeInfo.CUBE_CATEGORY_BELT,
			player.CUBE_EVENT : localeInfo.CUBE_CATEGORY_EVENT,
			player.CUBE_ETC : localeInfo.CUBE_CATEGORY_ETC,
			player.CUBE_JOB : localeInfo.CUBE_CATEGORY_JOB,
			player.CUBE_SETADD_WEAPON : localeInfo.CUBE_CATEGORY_SETADD_WEAPON,
			player.CUBE_SETADD_ARMOR_BODY : localeInfo.CUBE_CATEGORY_SETADD_ARMOR,
			player.CUBE_SETADD_ARMOR_HELMET : localeInfo.CUBE_CATEGORY_SETADD_HELMET,
			player.CUBE_PET : localeInfo.CUBE_CATEGORY_PET,
			player.CUBE_SKILL_BOOK : localeInfo.CUBE_CATEGOR_SKILL_BOOK,
			player.CUBE_ARMOR_GLOVE : localeInfo.CUBE_CATEGORY_ARMOR_GLOVE,
			player.CUBE_ALCHEMY : localeInfo.CUBE_CATEGORY_ALCHEMY
		}

		ui.ScriptWindow.__init__(self)

		self.__Initialize()
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__init__(self)
		self.__Initialize()

	def __Initialize(self):
		self.xCubeWindowStart = 0
		self.yCubeLookWindowStart = 0

		self.inventory = None
		self.dlg_question = None
		self.item_tooltip = None

		self.category_group_list = []
		self.category_showing_list = []
		self.scroll_diff = 0
		self.scroll_pos = 0
		self.selected_obj = None
		self.cube_item = {}
		self.multiplier = 1
		self.has_attr = 0

		self.pct_increase_item_vnum = 0
		self.pct_increase_item_count = 0
		self.pct_increase_item_pos = -1

	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/CubeRenewalWindow.py")
		except:
			import exception
			exception.Abort("CubeWindow.__LoadWindow.LoadScriptFile")

		try:
			self.item_list_board = self.GetChild("item_list_board")
			self.cube_list_scroll_bar = self.GetChild("cube_list_scroll_bar")
			self.item_slot = self.GetChild("item_slot")
			self.result_qty = self.GetChild("result_qty")
			self.qty_sub_button = self.GetChild("qty_sub_button")
			self.qty_add_button = self.GetChild("qty_add_button")
			self.yang_text = self.GetChild("yang_text")
			self.gem_text = self.GetChild("gem_text")
			self.imporve_slot = self.GetChild("imporve_slot")
			self.button_ok = self.GetChild("button_ok")
			self.button_cancel = self.GetChild("button_cancel")
			self.material_qty_dict = {}
			for i in xrange(MAX_MATERIAL_COUNT):
				self.material_qty_dict[i] = self.GetChild("material_qty_text_%d" % (i + 1))

			if localeInfo.IsARABIC():
				self.yang_text.SetWindowHorizontalAlignLeft()
				self.yang_text.SetPosition(4, 0)

				self.gem_text.SetWindowHorizontalAlignLeft()
				self.gem_text.SetPosition(4, 0)
		except:
			import exception
			exception.Abort("CubeWindow.__LoadWindow.BindObject")

		try:
			self.GetChild("board").SetCloseEvent(ui.__mem_func__(self.Close))

			self.material_slot = ui.SlotWindow()
			self.material_slot.SetParent(self.item_slot)
			self.material_slot.SetPosition(25, 13)
			self.material_slot.SetSize(32 * 9, 32 * 3)
			self.material_slot.SetOverInItemEvent(ui.__mem_func__(self.__OverInCubeMaterialSlot))
			self.material_slot.SetOverOutItemEvent(ui.__mem_func__(self.__OverOutCubeMaterialSlot))

			max_material_count = MAX_MATERIAL_COUNT + 1
			for row in range(3):
				for col in range(max_material_count):
					slot = row * max_material_count + col
					x = 0
					if col > 0:
						x = 16 + (46 * col)

					y = row * 32
					self.material_slot.AppendSlot(slot, x, y, 32, 32)
					self.material_slot.SetCantMouseEventSlot(slot)

			self.material_slot.RefreshSlot()
			self.material_slot.Show()

			self.pct_increase_item_slot = ui.SlotWindow()
			self.pct_increase_item_slot.SetParent(self.imporve_slot)
			self.pct_increase_item_slot.SetSize(32, 32)
			self.pct_increase_item_slot.SetPosition(6, 5)
			self.pct_increase_item_slot.SetSelectItemSlotEvent(ui.__mem_func__(self.__SelectItemSlot))
			self.pct_increase_item_slot.SetSelectEmptySlotEvent(ui.__mem_func__(self.__SelectEmptySlot))
			self.pct_increase_item_slot.SetOverInItemEvent(ui.__mem_func__(self.__OverInItem))
			self.pct_increase_item_slot.SetOverOutItemEvent(ui.__mem_func__(self.__OverOutItem))
			self.pct_increase_item_slot.AppendSlot(0, 0, 0, 32, 32)
			self.pct_increase_item_slot.Show()

			self.result_qty.SetReturnEvent(ui.__mem_func__(self.__OnResultQuantityChange))
			self.result_qty.SetEscapeEvent(ui.__mem_func__(self.Close))

			self.qty_sub_button.SetEvent(ui.__mem_func__(self.__OnResultQuantitySub))
			self.qty_add_button.SetEvent(ui.__mem_func__(self.__OnResultQuantityAdd))

			self.button_ok.SetEvent(ui.__mem_func__(self.__OnClickMakeButton))
			self.button_cancel.SetEvent(ui.__mem_func__(self.__OnClickCancelButton))

			self.cube_list_scroll_bar.SetScrollEvent(ui.__mem_func__(self.OnScroll))
			self.cube_list_scroll_bar.SetUpScrollButtonEvent(ui.__mem_func__(self.OnScrollUp))
			self.cube_list_scroll_bar.SetDownScrollButtonEvent(ui.__mem_func__(self.OnScrollDown))
			self.cube_list_scroll_bar.SetPos(0)

			self.item_tooltip = uiToolTip.ItemToolTip()
			self.item_tooltip.Hide()
		except:
			import exception
			exception.Abort("CubeWindow.__LoadWindow.BindEvent")

	def __UseItemSlot(self):
		self.ClearPctIncreaseItemSlot()

	def __SelectItemSlot(self):
		if mouseModule.mouseController.isAttached():
			mouseModule.mouseController.DeattachObject()

		self.ClearPctIncreaseItemSlot()

	def __SelectEmptySlot(self):
		if mouseModule.mouseController.isAttached():
			attached_slot_type = mouseModule.mouseController.GetAttachedType()
			attached_slot_pos = mouseModule.mouseController.GetAttachedSlotNumber()
			mouseModule.mouseController.DeattachObject()

			if not self.cube_item:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CUBE_SELECT_ITEM_FIRST)
				return

			if self.cube_item["percent"] >= 100:
				return

			item_vnum = player.GetItemIndex(attached_slot_pos)
			item_count = player.GetItemCount(attached_slot_pos)

			if item_vnum == 0:
				return

			item.SelectItem(item_vnum)
			if item.GetItemType() == item.ITEM_TYPE_ALCHEMY and item.GetItemSubType() == self.cube_item["category"]:
				self.pct_increase_item_slot.SetItemSlot(0, item_vnum, item_count)
				self.pct_increase_item_vnum = item_vnum
				self.pct_increase_item_count = item.GetValue(0)
				self.pct_increase_item_pos = attached_slot_pos

				self.__OnResultQuantityChange()
			else:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CUBE_DO_NOT_SAME_ALCHEMY_KINDS)
				return
		else:
			if not self.cube_item:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CUBE_SELECT_ITEM_FIRST)
				return

	def __OverInItem(self):
		if not self.item_tooltip:
			return

		if self.pct_increase_item_pos == -1:
			return

		metin_slot = [0 for i in xrange(player.METIN_SOCKET_MAX_NUM)]
		attr_slot = [(0, 0) for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]

		self.item_tooltip.ClearToolTip()
		self.item_tooltip.AddItemData(self.pct_increase_item_vnum, metin_slot, attr_slot)
		self.item_tooltip.Show()

	def __OverOutItem(self):
		if self.item_tooltip:
			self.item_tooltip.Hide()

	def __OverInCubeRewardSlot(self):
		inven_pos = -1
		reward_item = self.cube_item["reward"]

		if app.ENABLE_SET_ITEM:
			if player.IsCubeSetAddCategory(self.cube_item["category"]) and self.cube_item["set_value"]:
				for i in xrange(player.INVENTORY_SLOT_COUNT):
					if player.GetItemIndex(i) != reward_item["vnum"]:
						continue

					if player.GetItemSetValue(i):
						continue

					inven_pos = i
					break

		if inven_pos != -1:
			metin_slot = [player.GetItemMetinSocket(inven_pos, i) for i in xrange(player.METIN_SOCKET_MAX_NUM)]
			attr_slot = [player.GetItemAttribute(inven_pos, i) for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]
			refine_element = player.GetItemRefineElement(inven_pos) if app.ENABLE_REFINE_ELEMENT_SYSTEM else None
			apply_random_list = [player.GetItemApplyRandom(inven_pos, i) for i in range(player.APPLY_RANDOM_SLOT_MAX_NUM)] if app.ENABLE_APPLY_RANDOM else []
			set_value = player.GetItemSetValue(inven_pos) if app.ENABLE_SET_ITEM else 0
		else:
			metin_slot = [0 for i in xrange(player.METIN_SOCKET_MAX_NUM)]
			attr_slot = [(0, 0) for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]
			refine_element = None
			apply_random_list = [(0, 0) for i in range(player.APPLY_RANDOM_SLOT_MAX_NUM)] if app.ENABLE_APPLY_RANDOM else []
			set_value = 0

		if app.ENABLE_SET_ITEM:
			if self.cube_item["set_value"]:
				set_value = self.cube_item["set_value"]

		self.item_tooltip.ClearToolTip()
		self.item_tooltip.AddItemData(reward_item["vnum"], metin_slot, attr_slot, None, 0, 0, player.INVENTORY, inven_pos, refine_element, apply_random_list, set_value)
		self.item_tooltip.Show()

	def __OverOutCubeRewardSlot(self):
		if self.item_tooltip:
			self.item_tooltip.Hide()

	def __OverInCubeMaterialSlot(self, material_idx):
		if material_idx == 0:
			self.__OverInCubeRewardSlot()
			return

		material_item = self.cube_item["item"][material_idx - 1]
		reward_item = self.cube_item["reward"]

		inven_pos = -1
		if app.ENABLE_SET_ITEM:
			if player.IsCubeSetAddCategory(self.cube_item["category"]) and self.cube_item["set_value"]:
				if material_item["vnum"] == reward_item["vnum"]:
					for i in xrange(player.INVENTORY_SLOT_COUNT):
						if player.GetItemIndex(i) != reward_item["vnum"]:
							continue

						if player.GetItemSetValue(i):
							continue

						inven_pos = i
						break

		if inven_pos != -1:
			metin_slot = [player.GetItemMetinSocket(inven_pos, i) for i in xrange(player.METIN_SOCKET_MAX_NUM)]
			attr_slot = [player.GetItemAttribute(inven_pos, i) for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]
			refine_element = player.GetItemRefineElement(inven_pos) if app.ENABLE_REFINE_ELEMENT_SYSTEM else None
			apply_random_list = [player.GetItemApplyRandom(inven_pos, i) for i in range(player.APPLY_RANDOM_SLOT_MAX_NUM)] if app.ENABLE_APPLY_RANDOM else []
			set_value = player.GetItemSetValue(inven_pos) if app.ENABLE_SET_ITEM else 0
		else:
			metin_slot = [0 for i in xrange(player.METIN_SOCKET_MAX_NUM)]
			attr_slot = [(0, 0) for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]
			refine_element = None
			apply_random_list = [(0, 0) for i in range(player.APPLY_RANDOM_SLOT_MAX_NUM)] if app.ENABLE_APPLY_RANDOM else []
			set_value = 0

		self.item_tooltip.ClearToolTip()
		self.item_tooltip.AddItemData(material_item["vnum"], metin_slot, attr_slot, None, 0, 0, player.INVENTORY, inven_pos, refine_element, apply_random_list, set_value)
		self.item_tooltip.Show()

	def __OverOutCubeMaterialSlot(self):
		if self.item_tooltip:
			self.item_tooltip.Hide()

	def __OnResultQuantitySub(self):
		if not self.selected_obj or not self.cube_item:
			return

		self.RefreshPossibleMakeQuantity()

		self.multiplier = 1
		self.result_qty.SetText(str(self.cube_item["reward"]["count"] * self.multiplier))

		self.RefreshCubeWindow()
		self.RefreshRewardQuantity()

	def __OnResultQuantityAdd(self):
		if not self.selected_obj or not self.cube_item:
			return

		self.RefreshPossibleMakeQuantity()

		self.multiplier = max(1, self.GetPossibleMakeQuantity() / self.cube_item["reward"]["count"])
		self.result_qty.SetText(str(self.cube_item["reward"]["count"] * self.multiplier))

		self.RefreshCubeWindow()
		self.RefreshRewardQuantity()

	def __OnResultQuantityChange(self):
		if not self.result_qty.GetText():
			return

		if not self.selected_obj or not self.cube_item:
			return

		self.RefreshPossibleMakeQuantity()

		value = int(self.result_qty.GetText())
		multiplier = max(1, self.GetPossibleMakeQuantity() / self.cube_item["reward"]["count"])
		item_count = self.cube_item["reward"]["count"] * multiplier

		if value > item_count:
			self.multiplier = max(1, item_count / self.cube_item["reward"]["count"])
		else:
			self.multiplier = max(1, value / self.cube_item["reward"]["count"])

		self.result_qty.SetText(str(self.cube_item["reward"]["count"] * self.multiplier))

		self.RefreshCubeWindow()
		self.RefreshRewardQuantity()

	def __OnClickResultQuantity(self): pass

	def __OnClickMakeButton(self):
		if not self.selected_obj or not self.cube_item:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CUBE_PLEASE_SELECT)
			return

		self.RefreshPossibleMakeQuantity()
		self.RefreshCubeWindow()
		self.RefreshRewardQuantity()

		if not self.GetPossibleMakeQuantity():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CUBE_NOT_ENOUGH_MATERIAL)
			return

		if player.GetElk() < self.cube_item["gold"] * self.multiplier:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CUBE_NOT_ENOUGH_GOLD)
			return

		if player.GetGem() < self.cube_item["gem"] * self.multiplier:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CUBE_NOT_ENOUGH_GEM)
			return

		if self.pct_increase_item_pos != -1:
			item.SelectItem(player.GetItemIndex(self.pct_increase_item_pos))
			if player.GetItemCount(self.pct_increase_item_pos) < self.multiplier:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CUBE_NOT_ENOUGHT_ALCHEMY_COUNT % item.GetItemName())
				return

		if self.has_attr and self.cube_item["set_value"] == 0:
			self.OpenCubemakeQuestionDialog()
		else:
			self.OnAcceptCubeMake()

	def __OnClickCancelButton(self):
		self.Close()

	def OpenCubemakeQuestionDialog(self):
		if self.dlg_question:
			del self.dlg_question
			self.dlg_question = None

		self.dlg_question = uiCommon.QuestionDialog2()
		self.dlg_question.SetText1(localeInfo.CUBE_MAKE_CONFIRM_QUESTION_1 % (player.CUBE_MIN_ITEM_ATTR_WARNING))
		self.dlg_question.SetText2(localeInfo.CUBE_MAKE_CONFIRM_QUESTION_2)
		self.dlg_question.SetAcceptEvent(ui.__mem_func__(self.OnAcceptCubeMake))
		self.dlg_question.SetCancelEvent(ui.__mem_func__(self.dlg_question.Close))
		self.dlg_question.SetWidth(self.GetWidth() + app.GetTextLength(localeInfo.CUBE_MAKE_CONFIRM_QUESTION_1))
		self.dlg_question.Open()

	def OnAcceptCubeMake(self):
		if self.dlg_question:
			self.dlg_question.Close()

		if not self.cube_item:
			return

		net.SendCubeMake(self.cube_item["index"], self.multiplier, self.pct_increase_item_pos)

	def Open(self):
		ui.ScriptWindow.Show(self)
		if app.ENABLE_MOUSE_WHEEL_TOP_WINDOW:
			self.SetTop()
			wndMgr.SetWheelTopWindow(self.hWnd)

		(self.xCubeWindowStart, self.yCubeLookWindowStart, z) = player.GetMainCharacterPosition()
		player.SetCubeWindowOpen(True)

	def Close(self):
		self.ClearCubeData()

		for category_group in self.category_group_list:
			category_group.ShowClosedImg()

			for detail_group in category_group.GetDetailGroupList():
				detail_group.ShowClosedImg()

				for detail_object in detail_group.GetDetailObjectList():
					detail_object.EnableItem(False)

		for category_group in self.category_group_list:
			category_group.Destroy()
		self.category_group_list = []

		self.category_showing_list = []
		self.scroll_diff = 0
		self.scroll_pos = 0

		if self.dlg_question:
			self.dlg_question.Close()

		if self.item_tooltip:
			self.item_tooltip.Hide()

		player.SetCubeWindowOpen(False)
		net.SendCubeClose()

		ui.ScriptWindow.Hide(self)
		if app.ENABLE_MOUSE_WHEEL_TOP_WINDOW:
			wndMgr.ClearWheelTopWindow(self.hWnd)

	def CloseWithServerNotice(self): pass
	def Destroy(self):
		self.ClearDictionary()

		self.cube_item = {}
		self.selected_obj = None
		self.category_group_list = []
		self.category_showing_list = []

		ui.ScriptWindow.Hide(self)

	def SetInven(self, inventory):
		self.inventory = inventory

	def OnUpdate(self):
		(x, y, z) = player.GetMainCharacterPosition()
		if abs(x - self.xCubeWindowStart) > player.SHOW_UI_WINDOW_LIMIT_RANGE or abs(y - self.yCubeLookWindowStart) > player.SHOW_UI_WINDOW_LIMIT_RANGE:
			self.Close()

	def OnScrollUp(self):
		if self.scroll_pos > 0:
			self.scroll_pos -= 1

			pos = float(self.scroll_pos) / self.scroll_diff
			self.cube_list_scroll_bar.curPos = pos
			self.cube_list_scroll_bar.SetPos(pos, False)

		self.Refresh()

	def OnScrollDown(self):
		if self.scroll_pos < len(self.category_showing_list) - ListItem.LIST_TAB_COUNT:
			self.scroll_pos += 1

			pos = float(self.scroll_pos) / self.scroll_diff
			self.cube_list_scroll_bar.curPos = pos
			self.cube_list_scroll_bar.SetPos(pos, False)

		self.Refresh()

	def OnScroll(self):
		self.scroll_pos = int(self.cube_list_scroll_bar.GetPos() * self.scroll_diff)
		self.Refresh()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	if app.ENABLE_MOUSE_WHEEL_TOP_WINDOW:
		def OnMouseWheelButtonUp(self):
			self.OnScrollUp()
		def OnMouseWheelButtonDown(self):
			self.OnScrollDown()

	def SelectItem(self, selected_obj, cube_item):
		self.ClearCubeData()
		self.cube_item = cube_item

		self.selected_obj = selected_obj
		for category_group in self.category_group_list:
			for detail_group in category_group.GetDetailGroupList():
				for detail_object in detail_group.GetDetailObjectList():
					detail_object.EnableItem(False)
		selected_obj.EnableItem(True)

		self.RefreshPossibleMakeQuantity()
		self.RefreshCubeWindow()
		self.RefreshRewardQuantity()

	def Refresh(self):
		if ListItem.LIST_TAB_COUNT >= len(self.category_showing_list):
			self.scroll_pos = 0
			self.scroll_diff = 0
			self.cube_list_scroll_bar.Hide()
		else:
			self.scroll_diff = len(self.category_showing_list) - ListItem.LIST_TAB_COUNT
			if self.scroll_diff > 0:
				step_size = 1.0 / self.scroll_diff
				self.cube_list_scroll_bar.SetScrollStep(step_size)
			self.cube_list_scroll_bar.Show()

		map(ui.Window.Hide, self.category_showing_list)

		yShowingList = 5
		for category_group in self.category_showing_list[self.scroll_pos:]:
			xShowingList = 0

			if category_group.GetTab() == ListItem.LIST_TAB_FIRST:
				yShowingList += 5
			if category_group.GetTab() == ListItem.LIST_TAB_SECCOND:
				xShowingList += 15
			if category_group.GetTab() == ListItem.LIST_TAB_THIRD:
				yShowingList -= 2
				if localeInfo.IsARABIC():
					xShowingList += 15
				else:
					xShowingList += 35

			category_group.SetPosition(3 + xShowingList, yShowingList)
			category_group.SetTop()
			category_group.Show()

			yShowingList += 20
			if yShowingList > ListItem.LIST_BOARD_HEIGHT:
				break

	def RefreshSelectItem(self):
		self.category_showing_list = []
		for category_group in self.category_group_list:
			self.category_showing_list.append(category_group)
			if not category_group.IsOpen():
				continue

			for detail_group in category_group.GetDetailGroupList():
				self.category_showing_list.append(detail_group)
				if not detail_group.IsOpen():
					continue

				for detail_object in detail_group.GetDetailObjectList():
					self.category_showing_list.append(detail_object)

		self.Refresh()
		self.RefreshPossibleMakeQuantity()
		self.RefreshCubeWindow()
		self.RefreshRewardQuantity()

	def RefreshCubeWindow(self):
		if not self.cube_item:
			return

		self.ClearActivatedSlots()

		for i, item in enumerate(self.cube_item["item"]):
			slot = i + 1

			material_vnum = item["vnum"]
			material_count = max(1, min(MAX_MATERIAL_QUANTITY, item["count"] * self.multiplier))

			if material_vnum == 0:
				self.material_slot.ClearSlot(slot)
				continue

			self.material_slot.SetItemSlot(slot, item["vnum"], material_count)

			if app.ENABLE_SET_ITEM:
				if player.IsCubeSetAddCategory(self.cube_item["category"]) and self.cube_item["set_value"]:
					item_count = player.GetItemCountByVnum(item["vnum"], True)
				else:
					item_count = player.GetItemCountByVnum(item["vnum"])
			else:
				item_count = player.GetItemCountByVnum(item["vnum"])
			self.material_qty_dict[i].SetText("%d/%d" % (item_count, material_count))
			if item_count >= item["count"]:
				self.material_qty_dict[i].SetPackedFontColor(grp.GenerateColor(0.5411, 0.7254, 0.5568, 1.0))
			else:
				self.material_qty_dict[i].SetPackedFontColor(grp.GenerateColor(0.9, 0.4745, 0.4627, 1.0))

			self.ActivateItemSlot(material_vnum, material_count)

		self.yang_text.SetText(localeInfo.NumberToMoneyString(self.cube_item["gold"] * self.multiplier))
		if player.GetElk() >= self.cube_item["gold"] * self.multiplier:
			self.yang_text.SetPackedFontColor(grp.GenerateColor(0.7607, 0.7607, 0.7607, 1.0))
		else:
			self.yang_text.SetPackedFontColor(grp.GenerateColor(0.9, 0.4745, 0.4627, 1.0))

		self.gem_text.SetText(localeInfo.NumberToMoneyString(self.cube_item["gem"] * self.multiplier))
		if player.GetGem() >= self.cube_item["gem"] * self.multiplier:
			self.gem_text.SetPackedFontColor(grp.GenerateColor(0.7607, 0.7607, 0.7607, 1.0))
		else:
			self.gem_text.SetPackedFontColor(grp.GenerateColor(0.9, 0.4745, 0.4627, 1.0))

		if self.pct_increase_item_pos != -1:
			self.ActivateSlot(self.pct_increase_item_pos)

	def RefreshPossibleMakeQuantity(self):
		for category_group in self.category_group_list:
			if category_group.IsOpen():
				category_group.RefreshPossibleMakeQuantity()

	def RefreshRewardQuantity(self):
		if not self.cube_item:
			return

		reward_item = self.cube_item["reward"]
		if reward_item:
			self.material_slot.SetItemSlot(0, reward_item["vnum"], reward_item["count"] * self.multiplier)
			self.result_qty.SetText(str(reward_item["count"] * self.multiplier))

	def LoadCubeData(self, npc_vnum):
		self.ClearCubeData()

		category_data_dict = OrderedDict()
		for cube_item in player.GetCubeItem(npc_vnum):
			category = cube_item["category"]
			if category in self.CATEGORY_NAME_DICT:
				if category not in category_data_dict:
					category_data_dict[category] = []
				category_data_dict[category].append(cube_item)

		for category_key, category_data in category_data_dict.items():
			if not category_data:
				continue

			category_group = ItemCategoryGroup(self)
			category_group.SetName(self.CATEGORY_NAME_DICT[category_key])
			category_group.ShowClosedImg()

			for cube_item in category_data:
				item.SelectItem(cube_item["reward"]["vnum"])
				if app.ENABLE_SET_ITEM:
					if player.IsCubeSetAddCategory(cube_item["category"]):
						item_name = item.GetItemNameBySetValue(cube_item["set_value"])
					else:
						item_name = item.GetItemName()
				else:
					item_name = item.GetItemName()

				group_name = re.sub(r"^\d+\*|\+\d+$", "", item_name)

				detail_group = category_group.GetDetailGroupByName(group_name)
				if not category_group.GetDetailGroupName(group_name):
					detail_group = category_group.AddItem(group_name)

				if detail_group:
					detail_group.AddItem(localeInfo.CUBE_LIST_TEXT_ITEM_INFO % (item_name, cube_item["percent"]), cube_item)

			self.category_group_list.append(category_group)

		self.RefreshSelectItem()

	def CubeResult(self, is_success):
		if not is_success:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CUBE_FAIL)

		self.__OnResultQuantityChange()
		self.ClearPctIncreaseItemSlot()

	def GetPossibleMakeQuantity(self):
		if self.selected_obj:
			return self.selected_obj.GetPossibleMakeQuantity()
		return 0

	def ActivateSlot(self, slot):
		if self.inventory and slot != -1:
			self.inventory.ActivateSlot(slot, wndMgr.HILIGHTSLOT_CUBE)
			self.inventory.RefreshBagSlotWindow()

	def ActivateItemSlot(self, item_vnum, item_quantity):
		if not self.cube_item:
			return

		total_count = 0
		inven_pos_list = []
		for i in xrange(player.INVENTORY_SLOT_COUNT):
			if total_count >= item_quantity:
				break

			vnum = player.GetItemIndex(i)
			count = player.GetItemCount(i)

			if vnum != item_vnum:
				continue

			item.SelectItem(vnum)

			if app.ENABLE_SOULBIND_SYSTEM:
				if player.GetItemSealDate(player.INVENTORY, i) != item.E_SEAL_DATE_DEFAULT_TIMESTAMP:
					continue

			if app.ENABLE_SET_ITEM:
				if player.IsCubeSetAddCategory(self.cube_item["category"]) and self.cube_item["set_value"]:
					if player.GetItemSetValue(i):
						continue

			if count > 0:
				if i not in inven_pos_list:
					total_count += count
					inven_pos_list.append(i)

					if not self.has_attr:
						attr_count = sum(1 for j in range(player.ATTRIBUTE_SLOT_MAX_NUM) if player.GetItemAttribute(i, j)[0] != 0)
						self.has_attr = attr_count > player.CUBE_MIN_ITEM_ATTR_WARNING

			if total_count >= item_quantity:
				for i in inven_pos_list:
					self.ActivateSlot(i)

	def ClearCubeData(self):
		self.cube_item = {}
		self.multiplier = 1

		self.selected_obj = None
		for category_group in self.category_group_list:
			for detail_group in category_group.GetDetailGroupList():
				for detail_object in detail_group.GetDetailObjectList():
					detail_object.EnableItem(False)

		for i in xrange(self.material_slot.GetSlotCount()):
			self.material_slot.ClearSlot(i)

		self.result_qty.SetText("")
		for i in xrange(MAX_MATERIAL_COUNT):
			self.material_qty_dict[i].SetText("")

		self.yang_text.SetText("")
		self.gem_text.SetText("")

		if self.dlg_question:
			self.dlg_question.Close()

		if self.item_tooltip:
			self.item_tooltip.Hide()

		self.ClearActivatedSlots()
		self.ClearPctIncreaseItemSlot()

	def ClearActivatedSlots(self):
		if self.inventory and self.inventory.wndItem:
			for i in xrange(player.INVENTORY_SLOT_COUNT):
				self.inventory.DeactivateSlot(i, wndMgr.HILIGHTSLOT_CUBE)
			self.inventory.RefreshBagSlotWindow()

		self.has_attr = False

	def ClearPctIncreaseItemSlot(self):
		if self.pct_increase_item_slot:
			self.pct_increase_item_slot.ClearSlot(0)

		if self.inventory and self.pct_increase_item_pos != -1:
			self.inventory.DeactivateSlot(self.pct_increase_item_pos, wndMgr.HILIGHTSLOT_CUBE)
			self.inventory.RefreshBagSlotWindow()

		self.pct_increase_item_vnum = 0
		self.pct_increase_item_count = 0
		self.pct_increase_item_pos = -1

		self.RefreshPossibleMakeQuantity()
