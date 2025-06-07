#
# Filename: uiSnowFlakeStickEvent.py
# Author: Owsap
# Description: Flower Event
#

import app
import grp
import event
import exception
import ui
import uiCommon
import uiScriptLocale
import uiToolTip
import uiMiniGame
import localeInfo
import chat
import player
import item
import net
#import snowflakeStickEvent
#import ingameEventSystem

class SnowflakeStickEventUtil:
	RANK_BUFF_TYPE_LOCA = {
		0 : localeInfo.TARGET_LEVEL_S_PAWN,
		1 : localeInfo.TARGET_LEVEL_KNIGHT,
		2 : localeInfo.TARGET_LEVEL_S_KNIGHT,
		3 : localeInfo.SNOWFLAKE_STICK_EVENT_RANK_BUFF_TYPE_STONE
	}

	@classmethod
	def InitializeLoca(cls):
		cls.RANK_BUFF_TYPE_LOCA = {
			0 : localeInfo.TARGET_LEVEL_S_PAWN,
			1 : localeInfo.TARGET_LEVEL_KNIGHT,
			2 : localeInfo.TARGET_LEVEL_S_KNIGHT,
			3 : localeInfo.SNOWFLAKE_STICK_EVENT_RANK_BUFF_TYPE_STONE
		}

	@classmethod
	def AppendChat(cls, chat_type, message, data = None, interface = None):
		chat.AppendChat(chat_type, message)

class SnowflakeStickEvent(ui.ScriptWindow):

	DESC_X = 7
	DESC_Y = 5
	DESC_LINE_COUNT_MAX = 12
	DESC_WIDTH = 60

	class DescriptionBox(ui.Window):
		def __init__(self):
			ui.Window.__init__(self)
			self.desc_index = -1

		def __del__(self):
			ui.Window.__del__(self)

		def SetIndex(self, index):
			self.desc_index = index

		def OnRender(self):
			event.RenderEventSet(self.desc_index)

	EXCHANGE_STICK_NEED_MATERIAL_COUNT = 12
	EXCHANGE_STICK_COUNT_INIT_COOLTIME = 86400

	EXCHANGE_PET_NEED_MATERIAL_COUNT = 15
	EXCHANGE_MOUNT_NEED_MATERIAL_COUNT = 15

	EXCHANGE_STICK_COUNT_MAX = 5
	EXCHANGE_PET_COUNT_MAX = 3
	EXCHANGE_MOUNT_COUNT_MAX = 3

	EMPTY_METIN_SLOT = [0, 0, 0, 0, 0, 0]
	EMPTY_ATTR_SLOT = [(0, 0), (0, 0), (0, 0), (0, 0), (0, 0), (0, 0), (0, 0)]

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__Initialize()
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __Initialize(self):
		self.snow_ball_count_text = None
		self.tree_branch_count_text = None
		self.exchange_stick_count_text = None
		self.exchange_pet_count_text = None
		self.exchange_mount_count_text = None

		self.exchange_stick_slot = None
		self.exchange_pet_slot = None
		self.exchange_pet_stick_slot = None
		self.exchange_mount_slot = None
		self.exchange_mount_stick_slot = None

		self.exchange_reward_button = None
		self.help_button = None
		self.snowflake_stick_exchange_button = None
		self.reward_pet_exchange_button = None
		self.reward_mount_exchange_button = None

		self.right_arrow_img = None
		self.reward_pet_right_arrow = None
		self.reward_mount_right_arrow = None

		self.snow_ball_img = None
		self.tree_branch_img = None

		self.prev_button = None
		self.next_button = None

		self.desc_board = None

		self.snow_ball_count = 0
		self.tree_branch_count = 0
		self.exchange_stick_count = 0
		self.exchange_stick_time = 0
		self.exchange_pet_count = 0
		self.exchange_mount_count = 0

		self.cur_page = 0
		self.prev_exchange_time = 0
		self.is_data_requested = False

		self.tooltip_item = None
		self.interface = None

		self.desc_box = None
		self.desc_index = -1
		self.desc_y = self.DESC_Y

		self.tooltip = uiToolTip.ToolTip()

	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/SnowflakeStickEventWindow.py")
		except:
			import exception
			exception.Abort("SnowflakeStickEvent.__LoadWindow.LoadScriptFile")

		try:
			self.GetChild("board").SetCloseEvent(ui.__mem_func__(self.Close))

			## Pages
			self.desc_page = self.GetChild("page_desc_window")
			self.exchange_reward_page = self.GetChild("page_exchange_reward_window")
			self.exchange_reward_page.Hide()

			## Exchange Texts
			self.snow_ball_count_text = self.GetChild("snow_ball_count_text")
			self.tree_branch_count_text = self.GetChild("tree_branch_count_text")
			self.exchange_stick_count_text = self.GetChild("snowflake_stick_exchange_count_text")
			self.exchange_pet_count_text = self.GetChild("reward_pet_exchange_count_text")
			self.exchange_mount_count_text = self.GetChild("reward_mount_exchange_count_text")

			## Slots
			self.exchange_stick_slot = self.GetChild("snowflake_stick_slot")
			self.exchange_pet_slot = self.GetChild("reward_pet_slot")
			self.exchange_pet_stick_slot = self.GetChild("reward_pet_stick_slot")
			self.exchange_mount_slot = self.GetChild("reward_mount_slot")
			self.exchange_mount_stick_slot = self.GetChild("reward_mount_stick_slot")

			## Buttons
			self.exchange_reward_button = self.GetChild("exchange_reward_button")
			self.help_button = self.GetChild("help_button")
			self.snowflake_stick_exchange_button = self.GetChild("snowflake_stick_exchange_button")
			self.reward_pet_exchange_button = self.GetChild("reward_pet_exchange_button")
			self.reward_mount_exchange_button = self.GetChild("reward_mount_exchange_button")

			## Arrows
			self.right_arrow_img = self.GetChild("right_arrow_img")
			self.reward_pet_right_arrow = self.GetChild("reward_pet_right_arrow")
			self.reward_mount_right_arrow = self.GetChild("reward_mount_right_arrow")

			## Snowball & Tree Branch Images
			self.snow_ball_img = self.GetChild("snow_ball_img")
			self.tree_branch_img = self.GetChild("tree_branch_img")

			## Description Box
			self.desc_board = self.GetChild("desc_window_background")
			self.desc_box = self.DescriptionBox()
			self.desc_box.SetParent(self.desc_board)
			self.desc_box.Hide()

			## Description Arrows
			prev_button = ui.Button()
			prev_button.SetParent(self.desc_board)
			if localeInfo.IsARABIC():
				prev_button.SetPosition(30, 21)
			else:
				prev_button.SetPosition(self.desc_board.GetWidth() - 25 - 30, 21)
			prev_button.SetWindowVerticalAlignBottom()
			prev_button.SetUpVisual("d:/ymir work/ui/public/public_intro_btn/prev_btn_01.sub")
			prev_button.SetOverVisual("d:/ymir work/ui/public/public_intro_btn/prev_btn_02.sub")
			prev_button.SetDownVisual("d:/ymir work/ui/public/public_intro_btn/prev_btn_01.sub")
			prev_button.SetEvent(ui.__mem_func__(self.__ClickPrevButton))
			prev_button.Show()
			self.prev_button = prev_button

			next_button = ui.Button()
			next_button.SetParent(self.desc_board)
			if localeInfo.IsARABIC():
				next_button.SetPosition(30 + 30, 21)
			else:
				next_button.SetPosition(self.desc_board.GetWidth() - 25, 21)
			next_button.SetWindowVerticalAlignBottom()
			next_button.SetUpVisual("d:/ymir work/ui/public/public_intro_btn/next_btn_01.sub")
			next_button.SetOverVisual("d:/ymir work/ui/public/public_intro_btn/next_btn_02.sub")
			next_button.SetDownVisual("d:/ymir work/ui/public/public_intro_btn/next_btn_01.sub")
			next_button.SetEvent(ui.__mem_func__(self.__ClickNextButton))
			next_button.Show()
			self.next_button = next_button
		except:
			import exception
			exception.Abort("SnowflakeStickEvent.__LoadWindow.BindObject")

		## Exchange Buttons
		self.exchange_reward_button.SetEvent(ui.__mem_func__(self.__ClickExchangeRewardButton))
		self.snowflake_stick_exchange_button.SetEvent(ui.__mem_func__(self.__ClickExchangeStickButton))
		self.reward_pet_exchange_button.SetEvent(ui.__mem_func__(self.__ClickExchangePetButton))
		self.reward_mount_exchange_button.SetEvent(ui.__mem_func__(self.__ClickExchangeMountButton))

		## Help Button
		self.help_button.SetEvent(ui.__mem_func__(self.__ClickExchangeRewardButton))
		self.help_button.SetOverEvent(ui.__mem_func__(self.__OverInHelpButton))
		self.help_button.SetOverOutEvent(ui.__mem_func__(self.__OnImageMouseEvent), "mouse_over_out")

		## Snowball Branch Image
		self.snow_ball_img.SetEvent(ui.__mem_func__(self.__OnImageMouseEvent), "mouse_over_in", (uiScriptLocale.SNOWFLAKE_STICK_EVENT_TOOLTIP_TITLE_SNOW_BALL, uiScriptLocale.SNOWFLAKE_STICK_EVENT_TOOLTIP_SNOW_BALL, True))
		self.snow_ball_img.SetEvent(ui.__mem_func__(self.__OnImageMouseEvent), "mouse_over_out")

		## Tree Branch Image
		self.tree_branch_img.SetEvent(ui.__mem_func__(self.__OnImageMouseEvent), "mouse_over_in", (uiScriptLocale.SNOWFLAKE_STICK_EVENT_TOOLTIP_TITLE_TREE_BRANCH, uiScriptLocale.SNOWFLAKE_STICK_EVENT_TOOLTIP_TREE_BRANCH, True))
		self.tree_branch_img.SetEvent(ui.__mem_func__(self.__OnImageMouseEvent), "mouse_over_out")

		## Right Arrow
		self.right_arrow_img.SetEvent(ui.__mem_func__(self.__OnImageMouseEvent), "mouse_over_in", ("", uiScriptLocale.SNOWFLAKE_STICK_EVENT_TOOLTIP_RIGHT_ARROW, False))
		self.right_arrow_img.SetEvent(ui.__mem_func__(self.__OnImageMouseEvent), "mouse_over_out")

		## Reward Pet Right Arrow
		self.reward_pet_right_arrow.SetOverEvent(ui.__mem_func__(self.__OnImageMouseEvent), "mouse_over_in", ("", uiScriptLocale.SNOWFLAKE_STICK_EVENT_TOOLTIP_RIGHT_ARROW_PET, False))
		self.reward_pet_right_arrow.SetOverOutEvent(ui.__mem_func__(self.__OnImageMouseEvent), "mouse_over_out")

		## Reward Mount Right Arrow
		self.reward_mount_right_arrow.SetOverEvent(ui.__mem_func__(self.__OnImageMouseEvent), "mouse_over_in", ("", uiScriptLocale.SNOWFLAKE_STICK_EVENT_TOOLTIP_RIGHT_ARROW_MOUNT, False))
		self.reward_mount_right_arrow.SetOverOutEvent(ui.__mem_func__(self.__OnImageMouseEvent), "mouse_over_out")

		## Exchange Snow Stick Slot
		self.exchange_stick_slot.SetItemSlot(0, item.ITEM_VNUM_SNOWFLAKE_STICK, 0)
		self.exchange_stick_slot.SetOverInItemEvent(ui.__mem_func__(self.__OverInSlotItem), 0, item.ITEM_VNUM_SNOWFLAKE_STICK, True)
		self.exchange_stick_slot.SetOverOutItemEvent(ui.__mem_func__(self.__OverOut))

		## Exchange Pet Slot
		self.exchange_pet_slot.SetItemSlot(0, item.ITEM_VNUM_SNOWFLAKE_STICK_EVENT_PET, 0)
		self.exchange_pet_slot.SetOverInItemEvent(ui.__mem_func__(self.__OverInSlotItem), 0, item.ITEM_VNUM_SNOWFLAKE_STICK_EVENT_PET, False)
		self.exchange_pet_slot.SetOverOutItemEvent(ui.__mem_func__(self.__OverOut))

		self.exchange_pet_stick_slot.SetItemSlot(0, item.ITEM_VNUM_SNOWFLAKE_STICK, 0)
		self.exchange_pet_stick_slot.SetOverInItemEvent(ui.__mem_func__(self.__OverInSlotItem), 0, item.ITEM_VNUM_SNOWFLAKE_STICK, True)
		self.exchange_pet_stick_slot.SetOverOutItemEvent(ui.__mem_func__(self.__OverOut))

		## Exchange Mount Slot
		self.exchange_mount_slot.SetItemSlot(0, item.ITEM_VNUM_SNOWFLAKE_STICK_EVENT_MOUNT, 0)
		self.exchange_mount_slot.SetOverInItemEvent(ui.__mem_func__(self.__OverInSlotItem), 0, item.ITEM_VNUM_SNOWFLAKE_STICK_EVENT_MOUNT, False)
		self.exchange_mount_slot.SetOverOutItemEvent(ui.__mem_func__(self.__OverOut))

		self.exchange_mount_stick_slot.SetItemSlot(0, item.ITEM_VNUM_SNOWFLAKE_STICK, 0)
		self.exchange_mount_stick_slot.SetOverInItemEvent(ui.__mem_func__(self.__OverInSlotItem), 0, item.ITEM_VNUM_SNOWFLAKE_STICK, True)
		self.exchange_mount_stick_slot.SetOverOutItemEvent(ui.__mem_func__(self.__OverOut))

	def __ClickPrevButton(self):
		line_height = event.GetLineHeight(self.desc_index) + 4
		cur_start_line = event.GetVisibleStartLine(self.desc_index)

		decrease_count = self.DESC_LINE_COUNT_MAX

		if cur_start_line - decrease_count < 0:
			return

		event.SetVisibleStartLine(self.desc_index, cur_start_line - decrease_count)
		self.desc_y += (line_height * decrease_count)

	def __ClickNextButton(self):
		line_height = event.GetLineHeight(self.desc_index) + 4

		total_line_count = event.GetProcessedLineCount(self.desc_index)
		cur_start_line = event.GetVisibleStartLine(self.desc_index)

		increase_count = self.DESC_LINE_COUNT_MAX

		if cur_start_line + increase_count >= total_line_count:
			increase_count = total_line_count - cur_start_line

		if increase_count < 0 or cur_start_line + increase_count >= total_line_count:
			return

		event.SetVisibleStartLine(self.desc_index, cur_start_line + increase_count)
		self.desc_y -= (line_height * increase_count)

	def __RefreshUI(self):
		self.snow_ball_count_text.SetText(str(self.snow_ball_count))
		self.tree_branch_count_text.SetText(str(self.tree_branch_count))

		self.exchange_stick_count_text.SetText("%d/%d" % (max(0, self.EXCHANGE_STICK_COUNT_MAX - self.exchange_stick_count), self.EXCHANGE_STICK_COUNT_MAX))
		self.exchange_pet_count_text.SetText("%d/%d" % (max(0, self.EXCHANGE_PET_COUNT_MAX - self.exchange_pet_count), self.EXCHANGE_PET_COUNT_MAX))
		self.exchange_mount_count_text.SetText("%d/%d" % (max(0, self.EXCHANGE_MOUNT_COUNT_MAX - self.exchange_mount_count), self.EXCHANGE_MOUNT_COUNT_MAX))

	def __OverInSlotItem(self, item_vnum, is_show_event_remain_time):
		if self.tooltip_item:
			self.tooltip_item.ClearToolTip()
			self.tooltip_item.AddItemData(item_vnum, self.EMPTY_METIN_SLOT, self.EMPTY_ATTR_SLOT)

			if is_show_event_remain_time:
				event_remain_time = localeInfo.SecondToDHM(player.GetSnowflakeStickEvent() - app.GetGlobalTimeStamp())
				self.tooltip_item.AppendTextLine("%s : %s" % (localeInfo.LEFT_TIME, event_remain_time))

			self.tooltip_item.ShowToolTip()

	def __OverOut(self):
		if self.tooltip_item:
			self.tooltip_item.HideToolTip()

	def __OverInHelpButton(self):
		if self.exchange_stick_time > app.GetGlobalTimeStamp():
			exchange_time = localeInfo.SecondToDHM(self.exchange_stick_time - app.GetGlobalTimeStamp())
			self.__OnImageMouseEvent("mouse_over_in", ("", uiScriptLocale.SNOWFLAKE_STICK_EVENT_TOOLTIP_HELP_BUTTON_1 + " " + exchange_time, False))
		else:
			self.__OnImageMouseEvent("mouse_over_in", ("", uiScriptLocale.SNOWFLAKE_STICK_EVENT_TOOLTIP_HELP_BUTTON_2, False))

	def __OnImageMouseEvent(self, event_type, tooltip_data = None):
		self.tooltip.ClearToolTip()
		if event_type == "mouse_over_in":

			(title, desc, is_show_event_remain_time) = tooltip_data

			self.tooltip.SetThinBoardSize(app.GetTextLength(str(desc)))

			if title:
				self.tooltip.AutoAppendTextLine(title, self.tooltip.TITLE_COLOR)

			self.tooltip.AutoAppendTextLine(desc)

			if is_show_event_remain_time:
				event_remain_time = localeInfo.SecondToDHM(player.GetSnowflakeStickEvent() - app.GetGlobalTimeStamp())
				self.tooltip.AutoAppendTextLine("%s : %s" % (localeInfo.LEFT_TIME, event_remain_time))

			self.tooltip.AlignHorizonalCenter()
			self.tooltip.ShowToolTip()
		else:
			self.tooltip.HideToolTip()

	def __ClickExchangeRewardButton(self):
		if self.cur_page == 0:
			self.desc_page.Hide()
			self.exchange_reward_page.Show()
			self.cur_page = 1

		elif self.cur_page == 1:
			self.desc_page.Show()
			self.exchange_reward_page.Hide()
			self.cur_page = 0

	def __ClickExchangeStickButton(self):
		if app.GetGlobalTimeStamp() > player.GetSnowflakeStickEvent():
			SnowflakeStickEventUtil.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SNOWFLAKE_STICK_EVENT_MESSAGE_EXCHANGE_EVENT_DURATION_EXPIRED)
			return

		if self.snow_ball_count < self.EXCHANGE_STICK_NEED_MATERIAL_COUNT or self.tree_branch_count < self.EXCHANGE_STICK_NEED_MATERIAL_COUNT:
			SnowflakeStickEventUtil.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SNOWFLAKE_STICK_EVENT_MESSAGE_EXCHANGE_STICK_NOT_ENOUGH_MATERIAL)
			return

		if self.tree_branch_count < self.EXCHANGE_STICK_NEED_MATERIAL_COUNT:
			SnowflakeStickEventUtil.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SNOWFLAKE_STICK_EVENT_MESSAGE_EXCHANGE_STICK_NOT_ENOUGH_MATERIAL)
			return

		if self.exchange_stick_time > app.GetGlobalTimeStamp():
			exchange_time = localeInfo.SecondToDHM(self.exchange_stick_time - app.GetGlobalTimeStamp())
			SnowflakeStickEventUtil.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SNOWFLAKE_STICK_EVENT_MESSAGE_EXCHANGE_STICK_MAX + " " + exchange_time)
			return

		net.SendSnowflakeStickEventRequestExchangeStick()
		self.prev_exchange_time = app.GetGlobalTimeStamp()

	def __ClickExchangePetButton(self):
		if app.GetGlobalTimeStamp() > player.GetSnowflakeStickEvent():
			SnowflakeStickEventUtil.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SNOWFLAKE_STICK_EVENT_MESSAGE_EXCHANGE_EVENT_DURATION_EXPIRED)
			return

		if player.GetItemCountByVnum(item.ITEM_VNUM_SNOWFLAKE_STICK) <= self.EXCHANGE_PET_NEED_MATERIAL_COUNT:
			SnowflakeStickEventUtil.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SNOWFLAKE_STICK_EVENT_MESSAGE_EXCHANGE_PET_NOT_ENOUGH_MATERIAL)
			return

		if self.exchange_pet_count >= self.EXCHANGE_PET_COUNT_MAX:
			SnowflakeStickEventUtil.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SNOWFLAKE_STICK_EVENT_MESSAGE_EXCHANGE_PET_MAX)
			return

		net.SendSnowflakeStickEventRequestExchangePet()
		self.prev_exchange_time = app.GetGlobalTimeStamp()

	def __ClickExchangeMountButton(self):
		if app.GetGlobalTimeStamp() > player.GetSnowflakeStickEvent():
			SnowflakeStickEventUtil.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SNOWFLAKE_STICK_EVENT_MESSAGE_EXCHANGE_EVENT_DURATION_EXPIRED)
			return

		if player.GetItemCountByVnum(item.ITEM_VNUM_SNOWFLAKE_STICK) <= self.EXCHANGE_MOUNT_NEED_MATERIAL_COUNT:
			SnowflakeStickEventUtil.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SNOWFLAKE_STICK_EVENT_MESSAGE_EXCHANGE_MOUNT_NOT_ENOUGH_MATERIAL)
			return

		if self.exchange_mount_count >= self.EXCHANGE_MOUNT_COUNT_MAX:
			SnowflakeStickEventUtil.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SNOWFLAKE_STICK_EVENT_MESSAGE_EXCHANGE_MOUNT_MAX)
			return

		net.SendSnowflakeStickEventRequestExchangeMount()
		self.prev_exchange_time = app.GetGlobalTimeStamp()

	def Open(self):
		#if not self.is_data_requested:
		net.SendSnowflakeStickEventRequestInfo()
		#self.is_data_requested = True

		ui.ScriptWindow.Show(self)

		event.ClearEventSet(self.desc_index)
		self.desc_index = event.RegisterEventSet(uiScriptLocale.SNOWFLAKE_STICK_EVENT_DESC_FILE)
		event.SetFontColor(self.desc_index, 0.7843, 0.7843, 0.7843)
		event.SetVisibleLineCount(self.desc_index, self.DESC_LINE_COUNT_MAX)
		total_line = event.GetTotalLineCount(self.desc_index)

		if localeInfo.IsARABIC():
			event.SetEventSetWidth(self.desc_index, self.desc_board.GetWidth() - 20)

		event.SetRestrictedCount(self.desc_index, self.DESC_WIDTH)

		if self.desc_box:
			self.desc_box.Show()

		self.__RefreshUI()

	def Close(self):
		self.Hide()

		event.ClearEventSet(self.desc_index)
		self.desc_index = -1
		self.desc_y = self.DESC_Y

		if self.desc_box:
			self.desc_box.Hide()

	def Destroy(self):
		self.snow_ball_count_text = None
		self.tree_branch_count_text = None
		self.exchange_stick_count_text = None
		self.exchange_pet_count_text = None
		self.exchange_mount_count_text = None

		self.exchange_stick_slot = None
		self.exchange_pet_slot = None
		self.exchange_pet_stick_slot = None
		self.exchange_mount_slot = None
		self.exchange_mount_stick_slot = None

		self.exchange_reward_button = None
		self.help_button = None
		self.snowflake_stick_exchange_button = None
		self.reward_pet_exchange_button = None
		self.reward_mount_exchange_button = None

		self.right_arrow_img = None
		self.reward_pet_right_arrow = None
		self.reward_mount_right_arrow = None

		self.snow_ball_img = None
		self.tree_branch_img = None

		self.prev_button = None
		self.next_button = None

		self.desc_board = None

		self.snow_ball_count = 0
		self.tree_branch_count = 0
		self.exchange_stick_count = 0
		self.exchange_stick_time = 0
		self.exchange_pet_count = 0
		self.exchange_mount_count = 0

		self.cur_page = 0
		self.prev_exchange_time = 0
		self.is_data_requested = False

		self.tooltip_item = None
		self.interface = None

		self.desc_box = None
		self.desc_index = -1
		self.desc_y = self.DESC_Y

	def BindInterface(self, interface):
		self.interface = interface

	def SetItemToolTip(self, tooltip):
		self.tooltip_item = tooltip

	def InGameEventProcess(self, type, data = None):
		if type == net.SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_EVENT_INFO:
			(snow_ball_count, tree_branch_count, exchange_stick_count, exchange_stick_time, exchange_pet_count, exchange_mount_count) = data

			self.snow_ball_count = snow_ball_count
			self.tree_branch_count = tree_branch_count
			self.exchange_stick_count = exchange_stick_count
			self.exchange_stick_time = exchange_stick_time
			self.exchange_pet_count = exchange_pet_count
			self.exchange_mount_count = exchange_mount_count

		elif type == net.SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_ADD_SNOW_BALL:
			self.snow_ball_count += 1

		elif type == net.SNOWFLAKE_STICK_EVENT_GC_SUBHEADER_ADD_TREE_BRANCH:
			self.tree_branch_count += 1

		else:
			pass

		self.__RefreshUI()

	def OnPressEscapeKey(self):
		self.Close()

	def OnUpdate(self):
		(x, y) = self.desc_board.GetGlobalPosition()
		event.UpdateEventSet(self.desc_index, x + self.DESC_X, -(y + self.desc_y))
		self.desc_box.SetIndex(self.desc_index)
