#
# Filename: uiMiniGame.py
# Copyright (C) 2024 Owsap Development
#
# Author: Owsap
# Website: https://owsap.dev/
#

import app
import wndMgr
import ui
import uiCommon
import uiScriptLocale
import chat
import player
import background
import net
import systemSetting
import constInfo
import localeInfo
import ingameEventSystem

from _weakref import proxy

button_gap = 47
button_height = 60

if app.ENABLE_MINI_GAME_RUMI:
	import uiMiniGameRumi
	MINIGAME_RUMI = player.MINIGAME_RUMI

if app.ENABLE_MINI_GAME_YUTNORI:
	import uiMiniGameYutnori
	MINIGAME_YUTNORI = player.MINIGAME_YUTNORI

if app.ENABLE_FLOWER_EVENT:
	import uiFlowerEvent
	FLOWER_EVENT = player.FLOWER_EVENT

if app.ENABLE_MINI_GAME_CATCH_KING:
	import uiMiniGameCatchKing
	MINIGAME_CATCHKING = player.MINIGAME_CATCHKING

if app.ENABLE_SUMMER_EVENT_ROULETTE:
	import uiMiniGameRoulette
	MINIGAME_ROULETTE = player.MINIGAME_ROULETTE

if app.ENABLE_SNOWFLAKE_STICK_EVENT:
	import uiSnowflakeStickEvent
	SNOWFLAKE_STICK_EVENT = player.SNOWFLAKE_STICK_EVENT

MINIGAME_TYPE_MAX = player.MINIGAME_TYPE_MAX

class InGameRewardDialog(ui.ScriptWindow):
	def __init__(self, parent, event_name, event_func, event_end_time, event_type):
		ui.ScriptWindow.__init__(self)

		self.parent = parent
		self.event_name = event_name
		self.event_func = event_func
		self.event_end_time = event_end_time
		self.event_type = event_type

		self.SetParent(proxy(self.parent))

		self.event_button = None
		self.event_duration = None
		if app.ENABLE_EVENT_BANNER_REWARD_LIST_RENEWAL:
			self.event_reward_list_button = None
			self.reward_item_slot = None
		else:
			self.reward_item_slot = None

		self.__LoadWindow(event_name, event_func, event_end_time, event_type)
		ui.ScriptWindow.Show(self)

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadWindow(self, event_name, event_func, event_end_time, event_type):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/EventRewardListWindow.py")
		except:
			import exception
			exception.Abort("InGameRewardDialog.LoadWindow.LoadObject")

		try:
			if localeInfo.IsARABIC():
				self.GetChild("back_board_img").LeftRightReverse()

			if app.ENABLE_EVENT_BANNER_REWARD_LIST_RENEWAL:
				self.event_button = self.GetChild("title_button")
			else:
				self.event_button = self.GetChild("EventButton")

			if event_name:
				self.event_button.SetText(event_name)

			if event_func != None:
				self.event_button.SetEvent(event_func)

			self.event_duration = self.GetChild("EventDuration")
			left_sec = max(0, event_end_time - app.GetGlobalTimeStamp())
			if left_sec == 0:
				self.event_duration.SetText(uiScriptLocale.END_TIME)
			else:
				self.event_duration.SetText("(" + localeInfo.LEFT_TIME + " " + localeInfo.SecondToDHM(left_sec) + ")")

			if app.ENABLE_EVENT_BANNER_REWARD_LIST_RENEWAL:
				self.event_reward_list_button = self.GetChild("reward_list_button")
				self.event_reward_list_button.SetEvent(ui.__mem_func__(self.ShowRewardList), event_type)
				self.event_reward_list_button.Hide()

				if localeInfo.IsARABIC():
					event_reward_list_button_width = self.event_reward_list_button.GetWidth()
					(event_reward_list_button_x, event_reward_list_button_y) = self.event_reward_list_button.GetLocalPosition()
					self.event_reward_list_button.SetPosition(event_reward_list_button_x + event_reward_list_button_width, event_reward_list_button_y)

				self.reward_item_slot = ui.SlotWindow()
				self.reward_item_slot.SetParent(self)
				self.reward_item_slot.SetPosition(202, 7)
				if localeInfo.IsARABIC():
					self.reward_item_slot.SetPosition(0, 7)
				self.reward_item_slot.SetSize(32 * ingameEventSystem.INGAME_EVENT_REWARD_LIST_MIN, 32)
				self.reward_item_slot.SetSlotBaseImage("d:/ymir work/ui/public/Slot_Base.sub", 1.0, 1.0, 1.0, 1.0)
				for slot in range(ingameEventSystem.INGAME_EVENT_REWARD_LIST_MIN):
					self.reward_item_slot.AppendSlot(slot, slot * 32, 0, 32, 32)
				self.reward_item_slot.SetOverInItemEvent(ui.__mem_func__(self.__SlotOverInItem))
				self.reward_item_slot.SetOverOutItemEvent(ui.__mem_func__(self.__SlotOverOutItem))
				self.reward_item_slot.Show()
			else:
				self.reward_item_slot = self.GetChild("reward_item_slot")
				self.reward_item_slot.SetOverInItemEvent(ui.__mem_func__(self.__SlotOverInItem))
				self.reward_item_slot.SetOverOutItemEvent(ui.__mem_func__(self.__SlotOverOutItem))
				if localeInfo.IsARABIC():
					self.reward_item_slot.SetPosition(0, 7)
		except:
			import exception
			exception.Abort("InGameRewardDialog.LoadWindow.BindObject")

		for i in range(self.reward_item_slot.GetSlotCount()):
			(item_vnum, item_count) = ingameEventSystem.GetInGameEventRewardData(event_type, i)

			if item_vnum != 0 and item_count != 0:
				self.reward_item_slot.SetItemSlot(i, item_vnum, item_count)
			else:
				self.reward_item_slot.ClearSlot(i)
				self.reward_item_slot.SetCoverButton(i,\
					"d:/ymir work/ui/pet/skill_button/skill_enable_button.sub",\
					"d:/ymir work/ui/pet/skill_button/skill_enable_button.sub",\
					"d:/ymir work/ui/pet/skill_button/skill_enable_button.sub",\
					"d:/ymir work/ui/pet/skill_button/skill_enable_button.sub", False, False)
				self.reward_item_slot.SetAlwaysRenderCoverButton(i)

		reward_item_count = ingameEventSystem.GetInGameEventRewardCount(event_type)
		if reward_item_count > ingameEventSystem.INGAME_EVENT_REWARD_LIST_MIN:
			self.event_reward_list_button.Show()
			self.reward_item_slot.Hide()
		else:
			self.event_reward_list_button.Hide()
			self.reward_item_slot.Show()

	if app.ENABLE_EVENT_BANNER_REWARD_LIST_RENEWAL:
		def ShowRewardList(self, event_type):
			if self.parent:
				self.parent.OpenEventRewardList(event_type)

	def __SlotOverInItem(self, slot_index):
		if not self.parent:
			return

		if self.parent.tooltip_item == None:
			return

		(item_vnum, item_count) = ingameEventSystem.GetInGameEventRewardData(self.event_type, slot_index)
		if item_vnum != 0 and item_count != 0:
			item_socket_list = [0 for i in xrange(player.METIN_SOCKET_MAX_NUM)]
			item_attr_list = [(0, 0) for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]

			self.parent.tooltip_item.ClearToolTip()
			self.parent.tooltip_item.AddItemData(item_vnum, item_socket_list, item_attr_list)

	def __SlotOverOutItem(self):
		if self.parent and self.parent.tooltip_item != None:
			self.parent.tooltip_item.HideToolTip()

	def Destroy(self):
		ui.ScriptWindow.Hide(self)

		self.ClearDictionary()
		self.event_func = None

	def OnUpdate(self):
		if not self.event_duration:
			return

		left_sec = max(0, self.event_end_time - app.GetGlobalTimeStamp())
		if left_sec <= 0:
			self.event_duration.SetText(uiScriptLocale.END_TIME)
		else:
			self.event_duration.SetText("(" + localeInfo.LEFT_TIME + " " + localeInfo.SecondToDHM(left_sec) + ")")

class InGameEventDialog(ui.ScriptWindow):

	REWARD_LIST_MAX_COUNT = 5
	REWARD_LIST_POS_X = 330

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.cur_event_reward_list_index = 0
		self.event_reward_dict_info = {}

		self.scroll_pos = 0
		self.scroll_diff = 0

		self.tooltip_item = None
		self.interface = None

		if app.ENABLE_EVENT_BANNER_REWARD_LIST_RENEWAL:
			self.reward_list_wnd = None

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/InGameEventDialog.py")
		except:
			import exception
			exception.Abort("InGameEventDialog.LoadWindow.LoadObject")

		try:
			self.board = self.GetChild("board")

			self.titlebar = self.GetChild("Event_Title_Bar")
			self.titlebar.SetCloseEvent(ui.__mem_func__(self.Close))

			self.calendar_button = self.GetChild("Event_Calendar_Button")
			self.calendar_button.SetEvent(ui.__mem_func__(self.ClickCalendarButton))

			self.scroll_bar = self.GetChild("Event_Scroll_Bar")
			self.scroll_bar.SetScrollEvent(ui.__mem_func__(self.__OnScrollEventList))
			self.scroll_bar.Hide()
		except:
			import exception
			exception.Abort("InGameEventDialog.LoadWindow.BindObject")

		self.__LoadRewardListWindow()

	def __RefreshEventRewardDict(self):
		self.scroll_pos = int(self.scroll_bar.GetPos() * self.scroll_diff)

		event_reward_list = list(self.event_reward_dict_info.values())
		total_len = len(event_reward_list)

		for index in xrange(total_len):
			cur_scroll_index = index + self.scroll_pos
			if cur_scroll_index < total_len:
				reward_dialog = event_reward_list[cur_scroll_index]
				self.__RefreshEventRewardListYPosition(index, reward_dialog)

	def __RefreshEventRewardListYPosition(self, index, reward_dialog):
		if localeInfo.IsARABIC():
			reward_dialog.SetPosition(35, button_height + index * button_gap)
		else:
			reward_dialog.SetPosition(0, button_height + index * button_gap)

		if index < self.REWARD_LIST_MAX_COUNT:
			reward_dialog.Show()
		else:
			reward_dialog.Hide()

	def __OnScrollEventList(self):
		self.__RefreshEventRewardDict()

	def Show(self):
		ui.ScriptWindow.Show(self)

		if app.ENABLE_MOUSE_WHEEL_TOP_WINDOW:
			self.SetTop()
			wndMgr.SetWheelTopWindow(self.hWnd)

	def Close(self):
		if app.ENABLE_EVENT_BANNER_REWARD_LIST_RENEWAL:
			self.CloseEventRewardList()

		if app.ENABLE_MOUSE_WHEEL_TOP_WINDOW:
			wndMgr.ClearWheelTopWindow(self.hWnd)

		self.Hide()

	def Destroy(self):
		self.ClearDictionary()

		self.cur_event_reward_list_index = 0

		self.DeleteAllButton()

		self.scroll_pos = 0
		self.scroll_diff = 0

		self.tooltip_item = None
		self.interface = None

		if app.ENABLE_EVENT_BANNER_REWARD_LIST_RENEWAL:
			self.reward_list_wnd = None

	def AppendButton(self, name, func, event_end_time, event_type):
		if event_type in self.event_reward_dict_info:
			return

		reward_dialog = InGameRewardDialog(self, name, func, event_end_time, event_type)
		reward_dialog.SetSize(self.REWARD_LIST_POS_X - 30, 43)

		self.event_reward_dict_info[event_type] = reward_dialog

	def DeleteAllButton(self):
		for reward_dialog in self.event_reward_dict_info.values():
			reward_dialog.Destroy()

		self.event_reward_dict_info = {}

	def RefreshDialog(self):
		total_len = len(self.event_reward_dict_info)

		height_adjust = 12
		width_adjust = 5

		max_board_height = button_height + (button_gap * self.REWARD_LIST_MAX_COUNT) + height_adjust
		board_height = min(max_board_height, button_height + (button_gap * total_len + 1) + height_adjust)

		new_width = self.REWARD_LIST_POS_X + width_adjust
		self.board.SetSize(new_width, board_height)
		if localeInfo.IsARABIC():
			self.titlebar.SetWidth(new_width - 15)
		else:
			self.titlebar.SetWidth(new_width - 4)
		self.SetSize(new_width, board_height)

		(x, y) = self.GetLocalPosition()
		self.SetPosition(x, y)

		if total_len > self.REWARD_LIST_MAX_COUNT:
			self.scroll_diff = total_len - self.REWARD_LIST_MAX_COUNT

			if self.scroll_diff > 0:
				scroll_step_size = 1.0 / self.scroll_diff
				self.scroll_bar.SetScrollStep(scroll_step_size)
				self.scroll_pos = 0

			self.scroll_bar.Show()
		else:
			self.scroll_pos = 0
			self.scroll_diff = 0
			self.scroll_bar.Hide()

		self.__RefreshEventRewardDict()

	if app.ENABLE_EVENT_BANNER_REWARD_LIST_RENEWAL:
		def __LoadRewardListWindow(self):
			self.reward_list_wnd = uiCommon.RewardListDialog()
			self.reward_list_wnd.SetCalledWindow(self)
			self.reward_list_wnd.SetItemToolTip(self.tooltip_item)
			self.reward_list_wnd.Hide()

		def __MoveRewardList(self, x, y):
			if not self.reward_list_wnd:
				return

			parent_x, parent_y = self.GetLocalPosition()
			if localeInfo.IsARABIC():
				self.reward_list_wnd.SetPosition(parent_x - self.reward_list_wnd.GetWidth(), parent_y)
			else:
				self.reward_list_wnd.SetPosition(parent_x + self.REWARD_LIST_POS_X, parent_y)

		def OnMoveWindow(self, x, y):
			self.__MoveRewardList(x, y)

		def RefreshRewardList(self):
			if not self.reward_list_wnd:
				return

			reward_item_dict = {}
			for i in xrange(ingameEventSystem.GetInGameEventRewardCount(self.cur_event_reward_list_index)):
				(item_vnum, item_count) = ingameEventSystem.GetInGameEventRewardData(self.cur_event_reward_list_index, i)
				reward_item_dict.update({ i : (item_vnum, item_count)})

			self.reward_list_wnd.ClearRewardData()
			self.reward_list_wnd.SetRewardInfo(reward_item_dict)
			self.reward_list_wnd.RefreshRewardList()

		def OpenEventRewardList(self, event_type):
			self.cur_event_reward_list_index = event_type

			if self.reward_list_wnd:
				self.reward_list_wnd.Show()

			x, y = self.GetGlobalPosition()
			self.__MoveRewardList(x, y)

			self.RefreshRewardList()

		def CloseEventRewardList(self):
			if self.reward_list_wnd:
				self.reward_list_wnd.Hide()

	def ClickCalendarButton(self):
		return

	def SetItemToolTip(self, tooltip):
		self.tooltip_item = tooltip

		if self.reward_list_wnd:
			self.reward_list_wnd.SetItemToolTip(tooltip)

	def BindInterface(self, interface):
		self.interface = interface

	def OnPressEscapeKey(self):
		self.Close()
		return True

	if app.ENABLE_MOUSE_WHEEL_TOP_WINDOW:
		def OnMouseWheelButtonUp(self):
			return self.scroll_bar.OnUp() if self.scroll_bar else False

		def OnMouseWheelButtonDown(self):
			return self.scroll_bar.OnDown() if self.scroll_bar else False

class MiniGameWindow(ui.ScriptWindow):
	def __init__(self):
		ingameEventSystem.SetIngameEventHandler(self)

		self.isLoaded = 0

		self.interface = None
		self.tooltip_item = None
		self.inven = None

		self.mini_game_dialog = None
		self.isshow_mini_game_dialog = False
		self.game_type = MINIGAME_TYPE_MAX

		self.ingame_event_list = {
			ingameEventSystem.INGAME_EVENT_TYPE_MYSTERY_BOX1 : (uiScriptLocale.BANNER_MYSTERY_BOX_BUTTON, self.__ClickNoWorkButton),
			ingameEventSystem.INGAME_EVENT_TYPE_MYSTERY_BOX2 : (uiScriptLocale.BANNER_MYSTERY_BOX_BUTTON_2, self.__ClickNoWorkButton),
			ingameEventSystem.INGAME_EVENT_TYPE_NEW_XMAS_EVENT : (uiScriptLocale.BANNER_NEW_XMAS_BUTTON, self.__ClickNoWorkButton),
			ingameEventSystem.INGAME_EVENT_TYPE_EASTER_EVENT : (uiScriptLocale.BANNER_EASTER_BUTTON, self.__ClickNoWorkButton),
			ingameEventSystem.INGAME_EVENT_TYPE_ICECREAM_EVENT : (uiScriptLocale.BANNER_SUMMER_BUTTON, self.__ClickNoWorkButton),
			ingameEventSystem.INGAME_EVENT_TYPE_RAMADAN_EVENT : (uiScriptLocale.BANNER_RAMADAN_BUTTON, self.__ClickNoWorkButton),
			ingameEventSystem.INGAME_EVENT_TYPE_HALLOWEEN_EVENT : (uiScriptLocale.BANNER_HALLOWEEN_BUTTON, self.__ClickNoWorkButton),
			ingameEventSystem.INGAME_EVENT_TYPE_FOOTBALL_EVENT : (uiScriptLocale.BANNER_FOOTBALL_BUTTON, self.__ClickNoWorkButton),
			ingameEventSystem.INGAME_EVENT_TYPE_OLYMPIC_EVENT : (uiScriptLocale.BANNER_OLYMPIC_BUTTON, self.__ClickNoWorkButton),
			ingameEventSystem.INGAME_EVENT_TYPE_VALENTINE_DAY_EVENT : (uiScriptLocale.BANNER_VALENTINE_BUTTON, self.__ClickNoWorkButton),
		}

		if app.ENABLE_MINI_GAME_RUMI:
			self.ingame_event_list.update({ ingameEventSystem.INGAME_EVENT_TYPE_OKEY : (uiScriptLocale.BANNER_OKEY_BUTTON, self.__ClickRumiButton) })
			self.ingame_event_list.update({ ingameEventSystem.INGAME_EVENT_TYPE_OKEY_NORMAL : (uiScriptLocale.BANNER_OKEY_BUTTON, self.__ClickRumiButton) })
			self.rumi_game = None

		if app.ENABLE_MINI_GAME_YUTNORI:
			self.ingame_event_list.update({ ingameEventSystem.INGAME_EVENT_TYPE_YUTNORI : (uiScriptLocale.BANNER_YUTNORI_BUTTON, self.__ClickYutnoriButton) })
			self.yutnori_game = None

		if app.ENABLE_FLOWER_EVENT:
			self.ingame_event_list.update({ ingameEventSystem.INGAME_EVENT_TYPE_FLOWER_EVENT : (uiScriptLocale.BANNER_FLOWER_BUTTON, self.__ClickFlowerEventButton) })
			self.flower_event = None

		if app.ENABLE_MINI_GAME_CATCH_KING:
			self.ingame_event_list.update({ ingameEventSystem.INGAME_EVENT_TYPE_CATCHKING : (uiScriptLocale.BANNER_CATCHKING_BUTTON, self.__ClickCatchKingButton) })
			self.catch_king_game = None

		if app.ENABLE_SUMMER_EVENT_ROULETTE:
			self.ingame_event_list.update({ ingameEventSystem.INGAME_EVENT_TYPE_ROULETTE : (uiScriptLocale.BANNER_ROULETTE_BUTTON, self.__ClickNoWorkButton) })
			self.roulette_game = None

		if app.ENABLE_SNOWFLAKE_STICK_EVENT:
			self.ingame_event_list.update({ ingameEventSystem.INGAME_EVENT_TYPE_SNOWFLAKE_STICK_EVENT : (uiScriptLocale.BANNER_SNOWFLAKE_STICK_EVENT_BUTTON, self.__ClickSnowflakeStickEventButton) })
			self.snowflake_stick_event = None

		ui.ScriptWindow.__init__(self)
		self.__LoadWindow()

	def __del__(self):
		ingameEventSystem.DestroyInGameEventHandler()
		ui.ScriptWindow.__del__(self)

	def __LoadScript(self, fileName):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, fileName)

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			self.mini_game_dialog = InGameEventDialog()
			self.mini_game_dialog.BindInterface(self.interface)
			self.mini_game_dialog.SetItemToolTip(self.tooltip_item)
			self.mini_game_dialog.Hide()
		except:
			import exception
			exception.Abort("MiniGameWindow.LoadWindow.InGameEventDialog")

		ui.ScriptWindow.Show(self)

	def __CloseAll(self, except_game = MINIGAME_TYPE_MAX):
		if except_game == MINIGAME_TYPE_MAX:
			return

		if app.ENABLE_MINI_GAME_RUMI:
			if self.rumi_game and except_game != MINIGAME_RUMI:
				self.rumi_game.Close()

		if app.ENABLE_MINI_GAME_YUTNORI:
			if self.yutnori_game and except_game != MINIGAME_YUTNORI:
				self.yutnori_game.Close()

		if app.ENABLE_FLOWER_EVENT:
			if self.flower_event and except_game != FLOWER_EVENT:
				self.flower_event.Close()

		if app.ENABLE_MINI_GAME_CATCH_KING:
			if self.catch_king_game and except_game != MINIGAME_CATCHKING:
				self.catch_king_game.Close()

		if app.ENABLE_SUMMER_EVENT_ROULETTE:
			if self.roulette_game and except_game != MINIGAME_ROULETTE:
				self.roulette_game.Close()

		if app.ENABLE_SNOWFLAKE_STICK_EVENT:
			if self.snowflake_stick_event and except_game != SNOWFLAKE_STICK_EVENT:
				self.snowflake_stick_event.Close()

	def __ClickNoWorkButton(self):
		return

	def Show(self):
		self.__LoadWindow()
		ui.ScriptWindow.Show(self)

		if self.mini_game_dialog and self.isshow_mini_game_dialog:
			self.mini_game_dialog.Show()

	def Close(self):
		self.Hide()

	def Hide(self):
		if self.mini_game_dialog:
			self.isshow_mini_game_dialog = self.mini_game_dialog.IsShow()
			self.mini_game_dialog.Hide()

		wndMgr.Hide(self.hWnd)

	def Destroy(self):
		self.isLoaded = 0

		self.interface = None
		self.tooltip_item = None
		self.inven = None

		if self.mini_game_dialog:
			self.mini_game_dialog.Destroy()
			self.mini_game_dialog = None

		self.isshow_mini_game_dialog = False
		self.game_type = MINIGAME_TYPE_MAX

		if app.ENABLE_MINI_GAME_RUMI:
			if self.rumi_game:
				self.rumi_game.Destroy()
				self.rumi_game = None

		if app.ENABLE_MINI_GAME_YUTNORI:
			if self.yutnori_game:
				self.yutnori_game.Destroy()
				self.yutnori_game = None

		if app.ENABLE_FLOWER_EVENT:
			if self.flower_event:
				self.flower_event.Close()
				self.flower_event.Destroy()
				self.flower_event = None

		if app.ENABLE_MINI_GAME_CATCH_KING:
			if self.catch_king_game:
				self.catch_king_game.Destroy()
				self.catch_king_game = None

		if app.ENABLE_SUMMER_EVENT_ROULETTE:
			if self.roulette_game:
				self.roulette_game.Destroy()
				self.roulette_game = None

		if app.ENABLE_SNOWFLAKE_STICK_EVENT:
			if self.snowflake_stick_event:
				self.snowflake_stick_event.Close()
				self.snowflake_stick_event.Destroy()
				self.snowflake_stick_event = None

	def AddInGameEvent(self, ingame_event_type):
		if not ingame_event_type in self.ingame_event_list:
			return

		if not self.mini_game_dialog:
			return

		(event_enable, event_start_time, event_end_time) = ingameEventSystem.GetInGameEventData(ingame_event_type)
		if event_enable:
			(event_name, event_func) = self.ingame_event_list[ingame_event_type]
			self.mini_game_dialog.AppendButton(event_name, event_func, event_end_time, ingame_event_type)

	# NOTE : Webzen processes some events in this method, but we will handle each one with its own method.
	def InGameEventProcess(self, ingame_event_type, subheader_type, data = None):
		pass

	def CheckRemoveEventWindow(self, name):
		pass

	def OnGameOver(self): pass

	def ShowInGameEvent(self):
		if not self.mini_game_dialog:
			return

		if ingameEventSystem.GetInGameEventCount() == 0:
			if self.mini_game_dialog.IsShow():
				self.mini_game_dialog.Close()
			return

		if self.mini_game_dialog.IsShow():
			self.mini_game_dialog.Close()
		else:
			self.mini_game_dialog.Show()

	def RefreshInGameEvent(self, IsRefresh):
		if IsRefresh and self.mini_game_dialog:
			self.mini_game_dialog.DeleteAllButton()

			if app.ENABLE_MINI_GAME_RUMI:
				if not ingameEventSystem.GetInGameEventEnable(ingameEventSystem.INGAME_EVENT_TYPE_OKEY) and\
					not ingameEventSystem.GetInGameEventEnable(ingameEventSystem.INGAME_EVENT_TYPE_OKEY_NORMAL):

					if self.rumi_game:
						self.rumi_game.Destroy()
						self.rumi_game = None

			if app.ENABLE_MINI_GAME_YUTNORI:
				if not ingameEventSystem.GetInGameEventEnable(ingameEventSystem.INGAME_EVENT_TYPE_YUTNORI):
					if self.yutnori_game:
						self.yutnori_game.Destroy()
						self.yutnori_game = None

			if app.ENABLE_FLOWER_EVENT:
				if not ingameEventSystem.GetInGameEventEnable(ingameEventSystem.INGAME_EVENT_TYPE_FLOWER_EVENT):
					if self.flower_event:
						self.flower_event.Destroy()
						self.flower_event = None

			if app.ENABLE_MINI_GAME_CATCH_KING:
				if not ingameEventSystem.GetInGameEventEnable(ingameEventSystem.INGAME_EVENT_TYPE_CATCHKING):
					if self.catch_king_game:
						self.catch_king_game.Destroy()
						self.catch_king_game = None

			if app.ENABLE_SUMMER_EVENT_ROULETTE:
				if not ingameEventSystem.GetInGameEventEnable(ingameEventSystem.INGAME_EVENT_TYPE_ROULETTE):
					if self.roulette_game:
						self.roulette_game.Destroy()
						self.roulette_game = None

			if app.ENABLE_SNOWFLAKE_STICK_EVENT:
				if not ingameEventSystem.GetInGameEventEnable(ingameEventSystem.INGAME_EVENT_TYPE_SNOWFLAKE_STICK_EVENT):
					if self.snowflake_stick_event:
						self.snowflake_stick_event.Destroy()
						self.snowflake_stick_event = None

			if ingameEventSystem.GetInGameEventCount() == 0:
				if self.interface:
					self.interface.HideMiniMapInGameEventButton()

				self.mini_game_dialog.Close()
			else:
				if self.interface:
					self.interface.ShowMiniMapInGameEventButton()

				for ingame_event_type in self.ingame_event_list.keys():
					(event_enable, event_start_time, event_end_time) = ingameEventSystem.GetInGameEventData(ingame_event_type)

					if event_enable:
						self.AddInGameEvent(ingame_event_type)

				self.mini_game_dialog.RefreshDialog()

	def RefreshStatus(self):
		if ingameEventSystem.GetInGameEventCount() == 0:
			self.Close()

	def BindInterface(self, interface):
		#from _weakref import proxy
		#self.interface = proxy(interface)
		self.interface = interface

		if self.mini_game_dialog:
			self.mini_game_dialog.BindInterface(interface)

	def SetItemToolTip(self, tooltip):
		self.tooltip_item = tooltip

		if self.mini_game_dialog:
			self.mini_game_dialog.SetItemToolTip(tooltip)

	def SetInven(self, inven):
		self.inven = inven

	def show_mini_game_dialog(self):
		if self.mini_game_dialog:
			if not self.mini_game_dialog.IsShow():
				self.mini_game_dialog.Show()

	def hide_mini_game_dialog(self):
		if self.mini_game_dialog:
			if self.mini_game_dialog.IsShow():
				self.mini_game_dialog.Hide()

	if app.ENABLE_MINI_GAME_RUMI:
		def __ClickRumiButton(self):
			self.__CloseAll(MINIGAME_RUMI)

			if self.mini_game_dialog:
				self.mini_game_dialog.Close()

			if not self.rumi_game:
				self.rumi_game = uiMiniGameRumi.MiniGameRumi()
				if app.ENABLE_OKEY_EVENT_FLAG_RENEWAL:
					if self.tooltip_item:
						self.rumi_game.SetItemToolTip(self.tooltip_item)

			self.game_type = MINIGAME_RUMI
			self.main_game = self.rumi_game
			self.main_game.Open()

		def MiniGameOkeyEvent(self, enable):
			pass

		def MiniGameRumiStart(self):
			if self.rumi_game:
				self.rumi_game.GameStart()

		def MiniGameRumiEnd(self):
			if self.rumi_game:
				self.rumi_game.GameEnd()

		def MiniGameRumiMoveCard(self, srcCard, dstCard):
			if MINIGAME_RUMI != self.game_type:
				return

			if self.rumi_game:
				self.rumi_game.RumiMoveCard(srcCard, dstCard)

		def MiniGameRumiSetDeckCount(self, deck_card_count):
			if MINIGAME_RUMI != self.game_type:
				return

			if self.rumi_game:
				self.rumi_game.SetDeckCount(deck_card_count)

		def MiniGameRumiIncreaseScore(self, score, total_score):
			if MINIGAME_RUMI != self.game_type:
				return

			if self.rumi_game:
				self.rumi_game.RumiIncreaseScore(score, total_score)

		if app.ENABLE_OKEY_EVENT_FLAG_RENEWAL:
			def MiniGameRumiFlagProcess(self, process_type, data):
				if player.RUMI_GC_SUBHEADER_SET_CARD_PIECE_FLAG == process_type:
					if app.ENABLE_CHATTING_WINDOW_RENEWAL:
						chat.AppendChat(chat.CHAT_TYPE_ITEM_INFO, localeInfo.OKEY_EVENT_MESSAGE_CARD_PEICE_GAIN)
					else:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OKEY_EVENT_MESSAGE_CARD_PEICE_GAIN)

				if player.RUMI_GC_SUBHEADER_SET_CARD_FLAG == process_type:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OKEY_EVENT_MESSAGE_CARD_GAIN)

				if player.RUMI_GC_SUBHEADER_NO_MORE_GAIN == process_type:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OKEY_EVENT_MESSAGE_NO_MORE_GAIN)

				if MINIGAME_RUMI != self.game_type:
					return

				if self.rumi_game:
					self.rumi_game.MiniGameRumiFlagProcess(process_type, data)

		def SetOkeyNormalBG(self):
			if self.rumi_game:
				self.rumi_game.SetOkeyNormalBG()

	if app.ENABLE_MINI_GAME_YUTNORI:
		def __ClickYutnoriButton(self):
			self.__CloseAll(MINIGAME_YUTNORI)

			if self.mini_game_dialog:
				self.mini_game_dialog.Close()

			if not self.yutnori_game:
				self.yutnori_game = uiMiniGameYutnori.MiniGameYutnori()
				if app.ENABLE_YUTNORI_EVENT_FLAG_RENEWAL:
					if self.tooltip_item:
						self.yutnori_game.SetItemToolTip(self.tooltip_item)

			self.game_type = MINIGAME_YUTNORI
			self.main_game = self.yutnori_game
			self.main_game.Open()

		def YutnoriProcess(self, type, data):
			if MINIGAME_YUTNORI != self.game_type:
				return

			if not self.yutnori_game:
				return

			self.yutnori_game.YutnoriProcess(type, data)

		if app.ENABLE_YUTNORI_EVENT_FLAG_RENEWAL:
			def YutnoriFlagProcess(self, type, data):
				if player.YUTNORI_GC_SUBHEADER_SET_YUT_PIECE_FLAG == type:
					if app.ENABLE_CHATTING_WINDOW_RENEWAL:
						chat.AppendChat(chat.CHAT_TYPE_ITEM_INFO, localeInfo.YUTNORI_EVENT_MESSAGE_CARD_PEICE_GAIN)
					else:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.YUTNORI_EVENT_MESSAGE_CARD_PEICE_GAIN)

				elif player.YUTNORI_GC_SUBHEADER_SET_YUT_BOARD_FLAG == type:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.YUTNORI_EVENT_MESSAGE_CARD_GAIN)

				elif player.YUTNORI_GC_SUBHEADER_NO_MORE_GAIN == type:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.YUTNORI_EVENT_MESSAGE_NO_MORE_GAIN)

				if not self.yutnori_game:
					return

				self.yutnori_game.YutnoriFlagProcess(type, data)

	if app.ENABLE_MINI_GAME_CATCH_KING:
		def __ClickCatchKingButton(self):
			self.__CloseAll(MINIGAME_CATCHKING)

			if self.mini_game_dialog:
				self.mini_game_dialog.Close()

			if not self.catch_king_game:
				self.catch_king_game = uiMiniGameCatchKing.MiniGameCatchKing()
				if app.ENABLE_CATCH_KING_EVENT_FLAG_RENEWAL:
					if self.tooltip_item:
						self.catch_king_game.SetItemToolTip(self.tooltip_item)

			self.game_type = MINIGAME_CATCHKING
			self.main_game = self.catch_king_game
			self.main_game.Open()

		def MiniGameCatchKingEventStart(self, bigScore):
			if self.catch_king_game:
				self.catch_king_game.GameStart(bigScore)

		def MiniGameCatchKingSetHandCard(self, cardNumber):
			if self.catch_king_game:
				self.catch_king_game.CatchKingSetHandCard(cardNumber)

		def MiniGameCatchKingResultField(self, score, rowType, cardPos, cardValue, keepFieldCard, destroyHandCard, getReward, isFiveNear):
			if self.catch_king_game:
				self.catch_king_game.CatchKingResultField(score, rowType, cardPos, cardValue, keepFieldCard, destroyHandCard, getReward, isFiveNear)

		def MiniGameCatchKingSetEndCard(self, cardPos, cardNumber):
			if self.catch_king_game:
				self.catch_king_game.CatchKingSetEndCard(cardPos, cardNumber)

		def MiniGameCatchKingReward(self, rewardCode):
			if self.catch_king_game:
				self.catch_king_game.CatchKingReward(rewardCode)

		if app.ENABLE_CATCH_KING_EVENT_FLAG_RENEWAL:
			def CatchKingFlagProcess(self, type, data):
				if player.CATCHKING_GC_SET_CARD_PIECE_FLAG == type:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CATCHKING_EVENT_MESSAGE_CARD_PEICE_GAIN)

				if player.CATCHKING_GC_SET_CARD_FLAG == type:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CATCHKING_EVENT_MESSAGE_CARD_GAIN)

				if player.CATCHKING_GC_NO_MORE_GAIN == type:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CATCHKING_EVENT_MESSAGE_NO_MORE_GAIN)

				if MINIGAME_CATCHKING != self.game_type:
					return

				if self.catch_king_game:
					self.catch_king_game.CatchKingFlagProcess(type, data)

	if app.ENABLE_SNOWFLAKE_STICK_EVENT:
		def __ClickSnowflakeStickEventButton(self):
			self.__CloseAll(SNOWFLAKE_STICK_EVENT)

			if self.mini_game_dialog:
				self.mini_game_dialog.Close()

			if player.GetLevel() < 60:
				uiSnowflakeStickEvent.SnowflakeStickEventUtil.AppendChat(self, chat.CHAT_TYPE_INFO, localeInfo.SNOWFLAKE_STICK_EVENT_MESSAGE_NOT_ENOUGH_LEVEL)
				return

			if not self.snowflake_stick_event:
				self.snowflake_stick_event = uiSnowflakeStickEvent.SnowflakeStickEvent()
				if self.tooltip_item:
					self.snowflake_stick_event.SetItemToolTip(self.tooltip_item)

			self.game_type = SNOWFLAKE_STICK_EVENT
			self.main_game = self.snowflake_stick_event
			self.main_game.Open()

		def SnowflakeStickEventProcess(self, type, data):
			if type == net.SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_ADD_SNOW_BALL:
				uiSnowflakeStickEvent.SnowflakeStickEventUtil.AppendChat(self, chat.CHAT_TYPE_INFO, localeInfo.SNOWFLAKE_STICK_EVENT_MESSAGE_ADD_SNOW_BALL)

			elif type == net.SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_ADD_TREE_BRANCH:
				uiSnowflakeStickEvent.SnowflakeStickEventUtil.AppendChat(self, chat.CHAT_TYPE_INFO, localeInfo.SNOWFLAKE_STICK_EVENT_MESSAGE_ADD_TREE_BRANCH)

			elif type == net.SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_MESSAGE_SNOW_BALL_MAX:
				uiSnowflakeStickEvent.SnowflakeStickEventUtil.AppendChat(self, chat.CHAT_TYPE_INFO, localeInfo.SNOWFLAKE_STICK_EVENT_MESSAGE_SNOW_BALL_MAX)

			elif type == net.SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_MESSAGE_TREE_BRANCH_MAX:
				uiSnowflakeStickEvent.SnowflakeStickEventUtil.AppendChat(self, chat.CHAT_TYPE_INFO, localeInfo.SNOWFLAKE_STICK_EVENT_MESSAGE_TREE_BRANCH_MAX)

			elif type == net.SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_MESSAGE_USE_STICK_FAILED:
				if data:
					uiSnowflakeStickEvent.SnowflakeStickEventUtil.AppendChat(self, chat.CHAT_TYPE_INFO, localeInfo.SNOWFLAKE_STICK_EVENT_MESSAGE_CANNOT_USE_STICK_COOLTIME % max(0, (data - app.GetGlobalTimeStamp()) // 60))
				else:
					uiSnowflakeStickEvent.SnowflakeStickEventUtil.AppendChat(self, chat.CHAT_TYPE_INFO, localeInfo.SNOWFLAKE_STICK_EVENT_MESSAGE_USE_STICK_FAILED_1)
					uiSnowflakeStickEvent.SnowflakeStickEventUtil.AppendChat(self, chat.CHAT_TYPE_INFO, localeInfo.SNOWFLAKE_STICK_EVENT_MESSAGE_USE_STICK_FAILED_2)

			elif type == net.SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_MESSAGE_GET_RANK_BUFF:
				uiSnowflakeStickEvent.SnowflakeStickEventUtil.AppendChat(self, chat.CHAT_TYPE_INFO, localeInfo.SNOWFLAKE_STICK_EVENT_MESSAGE_RANK_BUFF_GET)

			elif type == net.SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_MESSAGE_GET_SNOWFLAKE_BUFF:
				uiSnowflakeStickEvent.SnowflakeStickEventUtil.AppendChat(self, chat.CHAT_TYPE_INFO, localeInfo.SNOWFLAKE_STICK_EVENT_MESSAGE_SNOWFLAKE_BUFF_GET)

			elif type == net.SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_ENABLE:
				if data:
					uiSnowflakeStickEvent.SnowflakeStickEventUtil.AppendChat(self, chat.CHAT_TYPE_NOTICE, localeInfo.SNOWFLAKE_STICK_EVENT_MESSAGE_EVENT_START)
				else:
					uiSnowflakeStickEvent.SnowflakeStickEventUtil.AppendChat(self, chat.CHAT_TYPE_NOTICE, localeInfo.SNOWFLAKE_STICK_EVENT_MESSAGE_EVENT_END)

			if SNOWFLAKE_STICK_EVENT != self.game_type:
				return

			if not self.snowflake_stick_event:
				return

			self.snowflake_stick_event.InGameEventProcess(type, data)

	if app.ENABLE_SUMMER_EVENT_ROULETTE:
		def RouletteProcess(self, type, data):
			if not self.roulette_game:
				self.roulette_game = uiMiniGameRoulette.RouletteWindow()
				if self.tooltip_item:
					self.roulette_game.SetItemToolTip(self.tooltip_item)

			if type == player.ROULETTE_GC_OPEN:
				self.__CloseAll(MINIGAME_ROULETTE)

				if self.mini_game_dialog:
					self.mini_game_dialog.Close()

				self.game_type = MINIGAME_ROULETTE
				self.main_game = self.roulette_game

			self.roulette_game.RouletteProcess(type, data)

		def IsLateSummerRouletteGameOpen(self):
			return self.roulette_game.IsShow() if self.roulette_game else False

	if app.ENABLE_FLOWER_EVENT:
		def __ClickFlowerEventButton(self):
			self.__CloseAll(FLOWER_EVENT)

			if self.mini_game_dialog:
				self.mini_game_dialog.Close()

			if not self.flower_event:
				self.flower_event = uiFlowerEvent.FlowerEvent()

			self.game_type = FLOWER_EVENT
			self.main_game = self.flower_event
			self.main_game.Open()

		def FlowerEventProcess(self, type, data):
			if type == net.FLOWER_EVENT_SUBHEADER_GC_GET_INFO:
				if isinstance(data, tuple):
					uiFlowerEvent.FlowerEventUtil.FlowerEventGetItemMessage(*data)
				else:
					uiFlowerEvent.FlowerEventUtil.FlowerEventMessage(data)

			if self.flower_event:
				self.flower_event.FlowerEventProcess(type, data)
