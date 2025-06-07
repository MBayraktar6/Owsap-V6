import app
import net
import player
import ui
import localeInfo

class PopupNoticeWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.title_bar = None
		self.title_name = None
		self.checkbox = None
		self.check_image = None
		self.web_box = None

		self.url = None
		self.popup_event_flag_value = 0
		self.is_not_show_checked = 0
		self.is_hide_when_quest_open = 0

		self.prev_pos = None

		self.interface = None
		self.game_button_window = None

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def LoadWindow(self):
		try:
			PythonScriptLoader = ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self, "UIScript/PopupNoticeWindow.py")
		except:
			import exception
			exception.Abort("PopupNoticeWindow.LoadWindow.LoadObject")

		try:
			GetObject = self.GetChild
			self.title_bar = GetObject("TitleBar")
			self.title_name = GetObject("TitleName")
			self.checkbox = GetObject("checkbox")
			self.check_image = GetObject("checkbox_bg")
			self.web_box = GetObject("webBox")
		except:
			import exception
			exception.Abort("PopupNoticeWindow.LoadWindow.BindObject")

		self.title_bar.SetCloseEvent(ui.__mem_func__(self.__OnCloseButtonClick))
		self.check_image.SetEvent(ui.__mem_func__(self.__OnClickCheckBox), "mouse_click")
		self.checkbox.Hide()

	def Destroy(self):
		app.HideWebPage(app.BROWSER_TYPE_POPUP_NOTICE)

		self.ClearDictionary()
		self.title_bar = None
		self.title_name = None
		self.checkbox = None
		self.check_image = None
		self.web_box = None

		self.prev_pos = None

		self.interface = None
		self.game_button_window = None

	def Refresh(self):
		pass

	def __OnCloseButtonClick(self):
		self.Close()

	def Close(self):
		self.is_hide_when_quest_open = 0
		net.SendCommandPacket("/popup_notice_check " + str(self.is_not_show_checked))

		app.HideWebPage(app.BROWSER_TYPE_POPUP_NOTICE)
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def BindInterface(self, interface):
		self.interface = interface

	def BindGameButtonWindow(self, game_button_window):
		self.game_button_window = game_button_window

	def InitWebPage(self):
		pass

	def __OnClickCheckBox(self, event_type):
		if "mouse_click" == event_type :
			if not self.is_not_show_checked:
				self.is_not_show_checked = 1
				self.checkbox.Show()
			else:
				self.is_not_show_checked = 0
				self.checkbox.Hide()

	def SetTitle(self, title):
		self.title_name.SetText(title)

	def SetPopupNoticeEventFlag(self, flag_value):
		self.popup_event_flag_value = flag_value

		if self.game_button_window:
			self.game_button_window.CheckGameButton()

	def GetPopupNoticeEventFlagValue(self):
		return self.popup_event_flag_value

	def PopupNoticeProcess(self, is_checked, url):
		self.is_not_show_checked = is_checked
		self.url = url

		if is_checked != True:
			self.Open(url, is_checked)

	def Open(self, url, is_checked):
		self.Refresh()
		self.Show()
		self.SetCenterPosition()

		if not self.web_box:
			return

		x, y = self.web_box.GetGlobalPosition()
		ex, ey = x + self.web_box.GetWidth() + 1, y + self.web_box.GetHeight() + 1
		app.ShowWebPage(app.BROWSER_TYPE_POPUP_NOTICE, url, (x, y, ex, ey))

		if is_checked:
			self.checkbox.Show()
		else:
			self.checkbox.Hide()

		self.is_hide_when_quest_open = 1

	def OnUpdate(self):
		if not self.web_box:
			return

		new_pos = self.web_box.GetGlobalPosition()
		if new_pos == self.prev_pos:
			return

		self.prev_pos = new_pos

		x, y = new_pos
		ex, ey = x + self.web_box.GetWidth() + 1, y + self.web_box.GetHeight() + 1
		app.MoveWebPage(app.BROWSER_TYPE_POPUP_NOTICE, (x, y, ex, ey))

	def OnClickPopupNoticeUIOpen(self):
		self.Open(self.url, self.is_not_show_checked)

	def CloseWhenOpenQuest(self):
		app.HideWebPage(app.BROWSER_TYPE_POPUP_NOTICE)
		self.Hide()

	def OpenWhenOpenQuest(self):
		if self.is_hide_when_quest_open:
			self.OnClickPopupNoticeUIOpen()
