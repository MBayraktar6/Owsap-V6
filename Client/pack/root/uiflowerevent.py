#
# Filename: uiFlowerEvent.py
# Author: Owsap
# Description: Flower Event
#

import app
import event
import wndMgr
import ui
import uiCommon
import uiToolTip
import uiScriptLocale
import chat
import net
import player
import localeInfo
import uiMiniGame
#import ingameEventSystem

class FlowerEventUtil:
	FLOWER_EVENT_LOCA = {
		player.FLOWER_EVENT_CHAT_TYPE_NOT_ENOUGH_SHOOT_COUNT : localeInfo.FLOWER_EVENT_NOT_ENOUGH_SHOOT_COUNT,
		player.FLOWER_EVENT_CHAT_TYPE_NOT_ENOUGH_EVENTORY_SPACE : localeInfo.FLOWER_EVENT_NOT_ENOUGH_EVENTORY_SPACE,
		player.FLOWER_EVENT_CHAT_TYPE_NOT_ENOUGH_SHOOT_ENVELOPE : localeInfo.FLOWER_EVENT_NOT_ENOUGH_SHOOT_ENVELOPE,
		player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_ENVELOPE : localeInfo.FLOWER_EVENT_GET_SHOOT_ENVELOPE,
		player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_CHRYSANTHEMUM : localeInfo.FLOWER_EVENT_GET_SHOOT_CHRYSANTHEMUM,
		player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_MAY_BELL : localeInfo.FLOWER_EVENT_GET_SHOOT_MAY_BELL,
		player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_DAFFODIL : localeInfo.FLOWER_EVENT_GET_SHOOT_DAFFODIL,
		player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_LILY : localeInfo.FLOWER_EVENT_GET_SHOOT_LILY,
		player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_SUNFLOWER : localeInfo.FLOWER_EVENT_GET_SHOOT_SUNFLOWER,
		player.FLOWER_EVENT_CHAT_TYPE_ITEM_FULL_AND_NOT_USE : localeInfo.FLOWER_EVENT_ITEM_FULL_AND_NOT_USE,
		player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_CHRYSANTHEMUM_COUNT : localeInfo.FLOWER_EVENT_GET_SHOOT_CHRYSANTHEMUM_COUNT,
		player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_MAY_BELL_COUNT : localeInfo.FLOWER_EVENT_GET_SHOOT_MAY_BELL_COUNT,
		player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_DAFFODIL_COUNT : localeInfo.FLOWER_EVENT_GET_SHOOT_DAFFODIL_COUNT,
		player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_LILY_COUNT : localeInfo.FLOWER_EVENT_GET_SHOOT_LILY_COUNT,
		player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_SUNFLOWER_COUNT : localeInfo.FLOWER_EVENT_GET_SHOOT_SUNFLOWER_COUNT,
		player.FLOWER_EVENT_CHAT_TYPE_ENVELOPE_MAX : localeInfo.FLOWER_EVENT_ENVELOPE_MAX
	}

	GET_SHOOT_COUNT_MESSAGE_TYPE = {
		player.SHOOT_CHRYSANTHEMUM : player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_CHRYSANTHEMUM_COUNT,
		player.SHOOT_MAY_BELL : player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_MAY_BELL_COUNT,
		player.SHOOT_DAFFODIL : player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_DAFFODIL_COUNT,
		player.SHOOT_LILY : player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_LILY_COUNT,
		player.SHOOT_SUNFLOWER : player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_SUNFLOWER_COUNT
	}

	GET_SHOOT_MESSAGE_TYPE = {
		player.SHOOT_ENVELOPE : player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_ENVELOPE,
		player.SHOOT_CHRYSANTHEMUM : player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_CHRYSANTHEMUM,
		player.SHOOT_MAY_BELL : player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_MAY_BELL,
		player.SHOOT_DAFFODIL : player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_DAFFODIL,
		player.SHOOT_LILY : player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_LILY,
		player.SHOOT_SUNFLOWER : player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_SUNFLOWER
	}

	EXCHANGE_COUNT_TYPE_TEXT = ('1', '10', '50', '100')

	@classmethod
	def InitializeLoca(cls):
		cls.FLOWER_EVENT_LOCA = {
			player.FLOWER_EVENT_CHAT_TYPE_NOT_ENOUGH_SHOOT_COUNT : localeInfo.FLOWER_EVENT_NOT_ENOUGH_SHOOT_COUNT,
			player.FLOWER_EVENT_CHAT_TYPE_NOT_ENOUGH_EVENTORY_SPACE : localeInfo.FLOWER_EVENT_NOT_ENOUGH_EVENTORY_SPACE,
			player.FLOWER_EVENT_CHAT_TYPE_NOT_ENOUGH_SHOOT_ENVELOPE : localeInfo.FLOWER_EVENT_NOT_ENOUGH_SHOOT_ENVELOPE,
			player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_ENVELOPE : localeInfo.FLOWER_EVENT_GET_SHOOT_ENVELOPE,
			player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_CHRYSANTHEMUM : localeInfo.FLOWER_EVENT_GET_SHOOT_CHRYSANTHEMUM,
			player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_MAY_BELL : localeInfo.FLOWER_EVENT_GET_SHOOT_MAY_BELL,
			player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_DAFFODIL : localeInfo.FLOWER_EVENT_GET_SHOOT_DAFFODIL,
			player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_LILY : localeInfo.FLOWER_EVENT_GET_SHOOT_LILY,
			player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_SUNFLOWER : localeInfo.FLOWER_EVENT_GET_SHOOT_SUNFLOWER,
			player.FLOWER_EVENT_CHAT_TYPE_ITEM_FULL_AND_NOT_USE : localeInfo.FLOWER_EVENT_ITEM_FULL_AND_NOT_USE,
			player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_CHRYSANTHEMUM_COUNT : localeInfo.FLOWER_EVENT_GET_SHOOT_CHRYSANTHEMUM_COUNT,
			player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_MAY_BELL_COUNT : localeInfo.FLOWER_EVENT_GET_SHOOT_MAY_BELL_COUNT,
			player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_DAFFODIL_COUNT : localeInfo.FLOWER_EVENT_GET_SHOOT_DAFFODIL_COUNT,
			player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_LILY_COUNT : localeInfo.FLOWER_EVENT_GET_SHOOT_LILY_COUNT,
			player.FLOWER_EVENT_CHAT_TYPE_GET_SHOOT_SUNFLOWER_COUNT : localeInfo.FLOWER_EVENT_GET_SHOOT_SUNFLOWER_COUNT,
			player.FLOWER_EVENT_CHAT_TYPE_ENVELOPE_MAX : localeInfo.FLOWER_EVENT_ENVELOPE_MAX
		}

	@classmethod
	def IsFlowerEventEnd(cls):
		return player.GetFlowerEventEnable()

	@classmethod
	def FlowerEventGetItemMessage(cls, shoot_type, shoot_count):
		if shoot_type in cls.GET_SHOOT_COUNT_MESSAGE_TYPE and shoot_count > 1:
			cls.FlowerEventMessage(cls.GET_SHOOT_COUNT_MESSAGE_TYPE[shoot_type], shoot_count)
		elif shoot_type in cls.GET_SHOOT_MESSAGE_TYPE:
			cls.FlowerEventMessage(cls.GET_SHOOT_MESSAGE_TYPE[shoot_type])
		else:
			raise ValueError("Unknown shoot_type %d" % shoot_type)

	@classmethod
	def FlowerEventMessage(cls, chat_type, data = None):
		if chat_type in cls.FLOWER_EVENT_LOCA:
			message = cls.FLOWER_EVENT_LOCA[chat_type]
			if data is not None:
				chat.AppendChat(chat.CHAT_TYPE_INFO, message % data)
			else:
				chat.AppendChat(chat.CHAT_TYPE_INFO, message)

class FlowerEvent(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.Initialize()
		self.LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Initialize(self):
		self.shoot_list_exchange_button = [None] * player.SHOOT_TYPE_MAX
		self.shoot_list_count_text = [None] * player.SHOOT_TYPE_MAX

		self.exchange_window_drop_down = None
		self.exchange_button_drop_down = None
		self.exchange_count_drop_down = 0

		self.is_data_requested = False
		self.last_exchange_time = 0.0

		self.tooltip = None

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/FlowerEventWindow.py")
		except:
			import exception
			exception.Abort("FlowerEvent.LoadWindow.LoadScript")

		try:
			## Board
			self.board = self.GetChild("board")

			## Slot Image
			self.slot_image = self.GetChild("slot_image")

			## Exchange Button
			self.shoot_list_exchange_button[player.SHOOT_ENVELOPE] = self.GetChild("main_exchange_button")
			self.shoot_list_exchange_button[player.SHOOT_CHRYSANTHEMUM] = self.GetChild("shoot_list_exchange_button_1")
			self.shoot_list_exchange_button[player.SHOOT_MAY_BELL] = self.GetChild("shoot_list_exchange_button_2")
			self.shoot_list_exchange_button[player.SHOOT_DAFFODIL] = self.GetChild("shoot_list_exchange_button_3")
			self.shoot_list_exchange_button[player.SHOOT_LILY] = self.GetChild("shoot_list_exchange_button_4")
			self.shoot_list_exchange_button[player.SHOOT_SUNFLOWER] = self.GetChild("shoot_list_exchange_button_5")

			## Count Text
			self.shoot_list_count_text[player.SHOOT_ENVELOPE] = self.GetChild("main_shoot_count_text")
			self.shoot_list_count_text[player.SHOOT_CHRYSANTHEMUM] = self.GetChild("shoot_count_text_1")
			self.shoot_list_count_text[player.SHOOT_MAY_BELL] = self.GetChild("shoot_count_text_2")
			self.shoot_list_count_text[player.SHOOT_DAFFODIL] = self.GetChild("shoot_count_text_3")
			self.shoot_list_count_text[player.SHOOT_LILY] = self.GetChild("shoot_count_text_4")
			self.shoot_list_count_text[player.SHOOT_SUNFLOWER] = self.GetChild("shoot_count_text_5")

		except:
			import exception
			exception.Abort("FlowerEvent.LoadWindow.BindObject")

		## Board
		self.board.SetCloseEvent(ui.__mem_func__(self.Close))

		## Slot Image
		self.slot_image.SetEvent(ui.__mem_func__(self.__ImgOverIn), "mouse_over_in")
		self.slot_image.SetEvent(ui.__mem_func__(self.__ImgOverOut), "mouse_over_out")

		## Exchange Button
		self.shoot_list_exchange_button[player.SHOOT_ENVELOPE].SetEvent(ui.__mem_func__(self.__OnClickListExchangeButton), player.SHOOT_ENVELOPE)
		self.shoot_list_exchange_button[player.SHOOT_CHRYSANTHEMUM].SetEvent(ui.__mem_func__(self.__OnClickListExchangeButton), player.SHOOT_CHRYSANTHEMUM)
		self.shoot_list_exchange_button[player.SHOOT_MAY_BELL].SetEvent(ui.__mem_func__(self.__OnClickListExchangeButton), player.SHOOT_MAY_BELL)
		self.shoot_list_exchange_button[player.SHOOT_DAFFODIL].SetEvent(ui.__mem_func__(self.__OnClickListExchangeButton), player.SHOOT_DAFFODIL)
		self.shoot_list_exchange_button[player.SHOOT_LILY].SetEvent(ui.__mem_func__(self.__OnClickListExchangeButton), player.SHOOT_LILY)
		self.shoot_list_exchange_button[player.SHOOT_SUNFLOWER].SetEvent(ui.__mem_func__(self.__OnClickListExchangeButton), player.SHOOT_SUNFLOWER)

		## ToolTip
		self.tooltip = uiToolTip.ToolTip()
		self.tooltip.ClearToolTip()

		## Exchange Button Drop Down
		self.exchange_window_drop_down = ui.ComboBoxImage(self, "d:/ymir work/ui/public/cheque_slot.sub", 214, 91)
		self.exchange_window_drop_down.SetEvent(lambda key, parent = self : parent.ExchangeCountDropDown(key))
		for key, value in enumerate(FlowerEventUtil.EXCHANGE_COUNT_TYPE_TEXT):
			self.exchange_window_drop_down.InsertItem(key, value)
		self.exchange_window_drop_down.SetCurrentItem(FlowerEventUtil.EXCHANGE_COUNT_TYPE_TEXT[0])
		self.exchange_window_drop_down.Show()

		if self.exchange_window_drop_down:
			## Exchange Button Drop Down
			self.exchange_button_drop_down = ui.Button()
			self.exchange_button_drop_down.SetParent(self)
			self.exchange_button_drop_down.SetPosition(250, 93)
			self.exchange_button_drop_down.SetUpVisual("d:/ymir work/ui/minigame/flower_event/drop_down_btn_default.sub")
			self.exchange_button_drop_down.SetOverVisual("d:/ymir work/ui/minigame/flower_event/drop_down_btn_over.sub")
			self.exchange_button_drop_down.SetDownVisual("d:/ymir work/ui/minigame/flower_event/drop_down_btn_down.sub")
			self.exchange_button_drop_down.SetEvent(ui.__mem_func__(self.exchange_window_drop_down.ToggleOpenCloseListBox))
			self.exchange_button_drop_down.Show()

	def Open(self):
		if not self.is_data_requested:
			net.SendFlowerEventRequestInfo()

		self.is_data_requested = True
		ui.ScriptWindow.Show(self)

	def Close(self):
		self.Hide()

		if self.tooltip:
			self.tooltip.Hide()

	def Destroy(self):
		self.ClearDictionary()

		self.board = None
		self.slot_image = None
		self.shoot_list_count_text = []
		self.shoot_list_exchange_button = []
		self.exchange_window_drop_down = None
		self.exchange_button_drop_down = None
		self.exchange_count_drop_down = 0

		self.is_data_requested = False
		self.last_exchange_time = 0.0

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def ExchangeCountDropDown(self, key):
		self.exchange_window_drop_down.SetCurrentItem(FlowerEventUtil.EXCHANGE_COUNT_TYPE_TEXT[key])
		self.exchange_count_drop_down = key

	## InGameEventProcess
	def FlowerEventProcess(self, type, data):
		if type == net.FLOWER_EVENT_SUBHEADER_GC_INFO_ALL:
			for shoot_type, shoot_count in enumerate(data):
				self.__SetFlowerEventInfo(shoot_type, shoot_count)
		elif type == net.FLOWER_EVENT_SUBHEADER_GC_UPDATE_INFO:
			self.__SetFlowerEventInfo(*data)
		else:
			pass

	def __SetFlowerEventInfo(self, shoot_type, shoot_count):
		if self.shoot_list_count_text:
			self.shoot_list_count_text[shoot_type].SetText(str(shoot_count))

	def __OnClickListExchangeButton(self, shoot_type):
		if app.GetTime() - self.last_exchange_time < float(player.FLOWER_EVENT_EXCHANGE_COOLTIME_SEC):
			return

		self.last_exchange_time = app.GetTime()
		net.SendFlowerEventExchange(shoot_type, self.exchange_count_drop_down)

	def __ImgOverIn(self):
		if self.tooltip:
			self.tooltip.ClearToolTip()
			self.tooltip.AppendTextLine(localeInfo.TOOLTIP_FLOWER_EVENT_SHOOT_ENVELOPE)
			self.tooltip.Show()

	def __ImgOverOut(self):
		if self.tooltip:
			self.tooltip.Hide()
