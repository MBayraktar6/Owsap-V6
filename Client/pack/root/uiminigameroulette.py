#
# Filename: uiMiniGameRoulette.py
# Author: Owsap
# Description: Mini-Game Roulette (Late Summer Event)
#

import app
import ui
import uiCommon
import uiScriptLocale
import mouseModule
import localeInfo
import net
import chat
import player

RESPONSE_TIME_OUT = 10000
ROULETTE_SLOT_MAX = 20

spin_count_before_request = ((20, 5), (20, 15, 5))
spin_time_before_request = ((40, 60), (20, 40, 60))
beginning_spin_time_after_request = (60, 120)
middle_spin_time_after_request = ((160, 200, 240, 280, 320, 340, 350, 360, 370, 380), (160, 200, 240, 280, 320, 340, 340, 340, 340, 340))
last_spin_time_after_request = (400, 600)
edge_effect_pos_tuple = ((25, 46), (69, 46), (113, 46), (157, 46), (201, 46), (245, 46), (245, 90), (245, 134), (245, 178), (245, 222), (245, 266), (201, 266), (157, 266), (113, 266), (69, 266), (25, 266), (25, 222), (25, 178), (25, 134), (25, 90))

class RouletteWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.start_pos_x = 0
		self.start_pos_y = 0

		self.spin_expire_time = 0
		self.item_data_tuple = ()

		self.tooltip = None
		self.popup = None

		self.__SpinInitialize()
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/MiniGameRouletteWindow.py")
		except:
			import exception
			exception.Abort("RouletteWindow.__LoadWindow.UIScript/MiniGameRouletteWindow.py")

		try:
			self.board = self.GetChild("board")
			self.item_slot = self.GetChild("item_slot")
			self.spin_button = self.GetChild("spin_button")
			self.spin_button_text1 = self.GetChild("spin_button_text1")
			self.spin_button_text2 = self.GetChild("spin_button_text2")
			self.spin_button_text3 = self.GetChild("spin_button_text3")
			self.slot_edge_effect = self.GetChild("slot_edge_effect")
		except:
			import exception
			exception.Abort("RouletteWindow.__LoadWindow")

		self.board.SetCloseEvent(ui.__mem_func__(self.Close))

		self.spin_button.SetEvent(ui.__mem_func__(self.ClickSpinButton))
		self.item_slot.SetOverInItemEvent(ui.__mem_func__(self.OverInSlot))
		self.item_slot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutSlot))

		self.popup = uiCommon.PopupDialog()
		self.popup.SetText(localeInfo.ROULETTE_REWARD_TEXT)
		self.popup.Close()

	def __TitleUpdate(self):
		left_time = max(0, self.spin_expire_time - app.GetGlobalTimeStamp())
		if left_time > 0:
			self.board.SetTitleName(uiScriptLocale.ROULETTE_SPECIAL_TITLE % localeInfo.SecondToColonTypeMS(left_time))
		else:
			self.board.SetTitleName(uiScriptLocale.ROULETTE_TITLE)

	def __Open(self, item_info):
		(self.start_pos_x, self.start_pos_y, z) = player.GetMainCharacterPosition()
		self.__SetItemList(item_info)
		self.Show()

	def __SetItemList(self, item_data_tuple):
		self.spin_expire_time = item_data_tuple[0]
		self.item_data_tuple = item_data_tuple[1]

		for i in xrange(ROULETTE_SLOT_MAX):
			item_vnum = self.item_data_tuple[i][0]
			item_count = self.item_data_tuple[i][1]
			self.item_slot.SetItemSlot(i, item_vnum, item_count)

	def __SpinInitialize(self):
		self.is_spinning = False
		self.spin_index = 0
		self.spin_result = ROULETTE_SLOT_MAX

		self.spin_rotation_count = 10
		self.last_spin_time = 0
		self.spin_time = 0

	def __StartSpin(self, result):
		if self.is_spinning:
			return

		if self.spin_button:
			self.spin_button.Disable()

		self.is_spinning = True
		self.spin_result = result

	def __MoveNext(self, cur_time, pivot_time):
		if cur_time - pivot_time < self.spin_time:
			return

		self.last_spin_time = app.GetGlobalTime()

		if self.spin_rotation_count > 0:
			self.__SpinFix((self.spin_index + 1) % ROULETTE_SLOT_MAX)

			if self.spin_index in (0, 5, 10, 15):
				self.spin_rotation_count -= 1

			self.spin_time = { 5: 0, 4: 10, 3: 25, 2: 50, 1: 100 }.get(self.spin_rotation_count, self.spin_time)
		else:
			if self.__RangeCheck() <= 5:
				self.spin_time = 600

			if self.spin_index == self.spin_result:
				self.__SpinEnd(self.spin_result)
			else:
				self.__SpinFix((self.spin_index + 1) % ROULETTE_SLOT_MAX)

	def __SpinFix(self, index):
		self.spin_index = index

		if self.slot_edge_effect:
			self.slot_edge_effect.SetPosition(edge_effect_pos_tuple[index][0], edge_effect_pos_tuple[index][1])

	def __SpinEnd(self, result):
		self.__SpinInitialize()

		self.spin_index = result
		self.spin_result = result

		if self.slot_edge_effect:
			self.slot_edge_effect.SetPosition(edge_effect_pos_tuple[result][0], edge_effect_pos_tuple[result][1])

		if self.spin_button:
			self.spin_button.Enable()

		if self.popup:
			self.popup.Open()

		net.SendMiniGameRouletteEnd()

	def __RangeCheck(self):
		if self.spin_result >= self.spin_index:
			return self.spin_result - self.spin_index
		else:
			return (ROULETTE_SLOT_MAX - self.spin_index) + self.spin_result

	def __RouletteUpdateBeforeRequest(self, cur_time): pass
	def __RouletteUpdateAfterRequest(self, cur_time): pass

	def __RouletteUpdate(self):
		if not self.is_spinning:
			return

		self.__MoveNext(app.GetGlobalTime(), self.last_spin_time)

	def __SlotIndexGenerator(self):
		return app.GetRandom(0, ROULETTE_SLOT_MAX - 1)

	def ClickSpinButton(self):
		if self.popup.IsShow():
			return

		net.SendMiniGameRouletteStart()

	def OverInSlot(self, slot_index):
		if self.tooltip and self.item_data_tuple:
			self.tooltip.SetRouletteItem(self.item_data_tuple[slot_index][0])

	def OverOutSlot(self):
		if self.tooltip:
			self.tooltip.HideToolTip()

	def RouletteProcess(self, type, data):
		if type == player.ROULETTE_GC_OPEN:
			self.__Open(data)

		elif type == player.ROULETTE_GC_START:
			self.__StartSpin(data)

		elif type == player.ROULETTE_GC_REQUEST:
			pass

		elif type == player.ROULETTE_GC_END:
			self.__SpinEnd(data)

		elif type == player.ROULETTE_GC_CLOSE:
			self.Close(True)

	def SetItemToolTip(self, tooltip):
		self.tooltip = tooltip

	def Show(self):
		net.SendMiniGameRouletteRequest()

		self.SetCenterPosition()
		self.SetTop()

		ui.ScriptWindow.Show(self)

	def Close(self, is_force = False, is_send_packet = True):
		if self.is_spinning and not is_force:
			return

		if is_send_packet:
			net.SendMiniGameRouletteClose()

		if self.popup:
			self.popup.Close()

		self.Hide()

	def Destroy(self):
		self.start_pos_x = 0
		self.start_pos_y = 0

		self.spin_expire_time = 0
		self.item_data_tuple = ()

		self.__SpinInitialize()

		self.ClearDictionary()
		self.Close(True, False)

		self.tooltip = None
		self.popup = None

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def OnUpdate(self):
		self.__TitleUpdate()
		self.__RouletteUpdate()

		(x, y, z) = player.GetMainCharacterPosition()
		if abs(x - self.start_pos_x) > player.SHOW_UI_WINDOW_LIMIT_RANGE or abs(y - self.start_pos_y) > player.SHOW_UI_WINDOW_LIMIT_RANGE:
			self.Close()
