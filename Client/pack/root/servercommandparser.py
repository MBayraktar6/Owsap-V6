import net
import background
import stringCommander
import constInfo
import app

class ServerCommandParser(object):
	def __init__(self):
		net.SetServerCommandParserWindow(self)
		self.__ServerCommand_Build()

	def __ServerCommand_Build(self):
		serverCommandList = {
			"DayMode" : self.__DayMode_Update,
			"xmas_snow" : self.__XMasSnow_Enable,
			"xmas_boom" : self.__XMasBoom_Enable,
			"xmas_tree" : self.__XMasTree_Enable,
			"newyear_boom" : self.__XMasBoom_Enable,
			"item_mall" : self.__ItemMall_Open,
		}

		if app.ENABLE_MINI_GAME_RUMI:
			serverCommandList["mini_game_okey"] = self.__MiniGameOkeyEvent
			serverCommandList["mini_game_okey_normal"] = self.__MiniGameOkeyNormalEvent

		if app.ENABLE_POPUP_NOTICE:
			serverCommandList["PopupNoticeEventFlag"] = self.__SetPopupNoticeEventFlag
			serverCommandList["PopupNoticeProcess"] = self.__PopupNoticeProcess

		if app.ENABLE_MINI_GAME_YUTNORI:
			serverCommandList["mini_game_yutnori"] = self.__MiniGameYutnori

		if app.ENABLE_SUMMER_EVENT_ROULETTE:
			serverCommandList["e_late_summer"] = self.__LateSummerEvent

		if app.ENABLE_SNOWFLAKE_STICK_EVENT:
			serverCommandList["snowflake_stick_event"] = self.__SnowflakeStickEvent

		if app.ENABLE_FLOWER_EVENT:
			serverCommandList["e_flower_drop"] = self.__FlowerEvent

		self.serverCommander = stringCommander.Analyzer()
		for serverCommandItem in serverCommandList.items():
			self.serverCommander.SAFE_RegisterCallBack(
				serverCommandItem[0], serverCommandItem[1]
			)

	def BINARY_ServerCommand_Run(self, line):
		try:
			print " BINARY_ServerCommand_Reserve", line
			return self.serverCommander.Run(line)
		except RuntimeError, msg:
			import dbg
			dbg.TraceError(msg)
			return 0

	def __PreserveCommand(self, line):
		net.PreserveServerCommand(line)

	if app.ENABLE_POPUP_NOTICE:
		def __SetPopupNoticeEventFlag(self, flag_value):
			self.__PreserveCommand("PopupNoticeEventFlag " + flag_value)

		def __PopupNoticeProcess(self, is_checked, url):
			self.__PreserveCommand("PopupNoticeProcess %d %s" % (int(is_checked), url))

	def __DayMode_Update(self, mode):
		self.__PreserveCommand("PRESERVE_DayMode " + mode)

	def __ItemMall_Open(self):
		self.__PreserveCommand("item_mall")

	## юс╫ц
	def __XMasBoom_Enable(self, mode):
		if "1" == mode:
			self.__PreserveCommand("PRESERVE_DayMode dark")
		else:
			self.__PreserveCommand("PRESERVE_DayMode light")

	def __XMasSnow_Enable(self, mode):
		self.__PreserveCommand("xmas_snow " + mode)

	def __XMasTree_Enable(self, grade):
		self.__PreserveCommand("xmas_tree " + grade)

	if app.ENABLE_MINI_GAME_RUMI:
		def __MiniGameOkeyEvent(self, enable):
			self.__PreserveCommand("mini_game_okey " + enable)

		def __MiniGameOkeyNormalEvent(self, enable):
			self.__PreserveCommand("mini_game_okey_normal " + enable)

	if app.ENABLE_MINI_GAME_YUTNORI:
		def __MiniGameYutnori(self, enable):
			self.__PreserveCommand("mini_game_yutnori " + enable)

	if app.ENABLE_SUMMER_EVENT_ROULETTE:
		def __LateSummerEvent(self, enable):
			self.__PreserveCommand("e_late_summer " + enable)

	if app.ENABLE_SNOWFLAKE_STICK_EVENT:
		def __SnowflakeStickEvent(self, end_time):
			self.__PreserveCommand("snowflake_stick_event " + end_time)

	if app.ENABLE_FLOWER_EVENT:
		def __FlowerEvent(self, enable):
			self.__PreserveCommand("e_flower_drop " + enable)

parserWnd = ServerCommandParser()
