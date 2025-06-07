import app

AUTOBAN_QUIZ_ANSWER = "ANSWER"
AUTOBAN_QUIZ_REFRESH = "REFRESH"
AUTOBAN_QUIZ_REST_TIME = "REST_TIME"

CODEPAGE = str(app.GetDefaultCodePage())

#CUBE_TITLE = "Cube Window"

if app.ENABLE_LOCALE_CLIENT:
	alsoExportToCharset = "windows-1250"
	localeDict = {}

	def LoadLocaleFile(srcFileName):
		global localeDict

		localeDict["CUBE_INFO_TITLE"] = "Recipe"
		localeDict["CUBE_REQUIRE_MATERIAL"] = "Requirements"
		localeDict["CUBE_REQUIRE_MATERIAL_OR"] = "or"

		try:
			lines = open(srcFileName, "r").readlines()
		except IOError:
			import dbg
			dbg.LogBox("LoadUIScriptLocaleError(%(srcFileName)s)" % locals())
			app.Abort()

		for line in lines:
			tokens = line[:-1].split("\t")

			if len(tokens) >= 2:
				localeDict[tokens[0]] = tokens[1]
			else:
				print(len(tokens), lines.index(line), line)

		globals().update(localeDict)

	def ReloadLocaleFile():
		global localeDict
		localeDict.clear()

		global CODEPAGE
		CODEPAGE = str(app.GetDefaultCodePage())

		if "HONGKONG" == app.GetLocaleServiceName():
			name = "locale/hongkong"
		elif "JAPAN" == app.GetLocaleServiceName():
			name = "locale/japan"
		elif "TAIWAN" == app.GetLocaleServiceName():
			name = "locale/taiwan"
		elif "NEWCIBN" == app.GetLocaleServiceName():
			name = "locale/newcibn"
		elif "EUROPE" == app.GetLocaleServiceName():
			name = app.GetLocalePath()
		else:
			name = "locale/ymir"

		global LOCALE_UISCRIPT_PATH
		LOCALE_UISCRIPT_PATH = "%s/ui/" % (name)

		global LOGIN_PATH, EMPIRE_PATH, GUILD_PATH, SELECT_PATH, WINDOWS_PATH, MAPNAME_PATH
		LOGIN_PATH = "locale/common/ui/login/"
		EMPIRE_PATH = "%s/ui/empire/" % (name)
		GUILD_PATH = "locale/common/ui/guild/"
		SELECT_PATH = "%s/ui/select/" % (name)
		WINDOWS_PATH = "%s/ui/windows/" % (name)
		MAPNAME_PATH = "%s/ui/mapname/" % (name)

		global JOBDESC_WARRIOR_PATH, JOBDESC_ASSASSIN_PATH, JOBDESC_SURA_PATH, JOBDESC_SHAMAN_PATH, JOBDESC_WOLFMAN_PATH
		JOBDESC_WARRIOR_PATH = "%s/jobdesc_warrior.txt" % (name)
		JOBDESC_ASSASSIN_PATH = "%s/jobdesc_assassin.txt" % (name)
		JOBDESC_SURA_PATH = "%s/jobdesc_sura.txt" % (name)
		JOBDESC_SHAMAN_PATH = "%s/jobdesc_shaman.txt" % (name)
		JOBDESC_WOLFMAN_PATH = "%s/jobdesc_wolfman.txt" % (name)

		global EMPIREDESC_A, EMPIREDESC_B, EMPIREDESC_C
		EMPIREDESC_A = "%s/empiredesc_a.txt" % (name)
		EMPIREDESC_B = "%s/empiredesc_b.txt" % (name)
		EMPIREDESC_C = "%s/empiredesc_c.txt" % (name)

		global LOCALE_INTERFACE_FILE_NAME, NEW_LOCALE_INTERFACE_FILE_NAME
		LOCALE_INTERFACE_FILE_NAME = "%s/locale_interface.txt" % (name)
		NEW_LOCALE_INTERFACE_FILE_NAME = "%s/new_locale_interface.txt" % (name)

		if app.ENABLE_LOADING_TIP:
			global LOADING_TIP_LIST
			LOADING_TIP_LIST = "%s/loading_tip_vnum.txt" % (name)

		if app.ENABLE_MINI_GAME_RUMI:
			global MINIGAME_RUMI_DESC
			MINIGAME_RUMI_DESC = "%s/mini_game_okey_desc.txt" % (name)

		if app.ENABLE_MINI_GAME_YUTNORI:
			global YUTNORI_EVENT_DESC
			YUTNORI_EVENT_DESC = "%s/yutnori_event_desc.txt" % (name)

		if app.ENABLE_MINI_GAME_CATCH_KING:
			global MINIGAME_CATCH_KING_DESC, MINIGAME_CATCH_KING_SIMPLE_DESC
			MINIGAME_CATCH_KING_DESC = "%s/catchking_event_desc.txt" % (name)
			MINIGAME_CATCH_KING_SIMPLE_DESC = "%s/catchking_event_simple_desc.txt" % (name)

		if app.ENABLE_SNOWFLAKE_STICK_EVENT:
			global SNOWFLAKE_STICK_EVENT_DESC_FILE
			SNOWFLAKE_STICK_EVENT_DESC_FILE = "%s/snowflake_stick_event_desc.txt" % (name)

		LoadLocaleFile(LOCALE_INTERFACE_FILE_NAME)
		LoadLocaleFile(NEW_LOCALE_INTERFACE_FILE_NAME)
else:
	def LoadLocaleFile(srcFileName, localeDict):
		localeDict["CUBE_INFO_TITLE"] = "Recipe"
		localeDict["CUBE_REQUIRE_MATERIAL"] = "Requirements"
		localeDict["CUBE_REQUIRE_MATERIAL_OR"] = "or"

		try:
			lines = open(srcFileName, "r").readlines()
		except IOError:
			import dbg
			dbg.LogBox("LoadUIScriptLocaleError(%(srcFileName)s)" % locals())
			app.Abort()

		for line in lines:
			tokens = line[:-1].split("\t")

			if len(tokens) >= 2:
				localeDict[tokens[0]] = tokens[1]
			else:
				print(len(tokens), lines.index(line), line)

if "locale/ymir" == app.GetLocalePath():
	LOCALE_COMMON_UISCRIPT_PATH = "locale/ymir_ui/"

	WINDOWS_PATH = "d:/ymir work/ui/game/949_windows/"
	SELECT_PATH = "d:/ymir work/ui/intro/949_select/"
	GUILD_PATH = "d:/ymir work/ui/game/949_guild/"
	EMPIRE_PATH = "d:/ymir work/ui/intro/949_empire/"
	MAPNAME_PATH = "locale/ymir_ui/mapname/"
	LOGIN_PATH = "d:/ymir work/ui/intro/949_login/"

	JOBDESC_WARRIOR_PATH = "locale/ymir/desc_warrior.txt"
	JOBDESC_ASSASSIN_PATH = "locale/ymir/desc_assassin.txt"
	JOBDESC_SURA_PATH = "locale/ymir/desc_sura.txt"
	JOBDESC_SHAMAN_PATH = "locale/ymir/desc_shaman.txt"
	JOBDESC_WOLFMAN_PATH = "locale/ymir/desc_wolfman.txt"

	EMPIREDESC_A = "locale/ymir/desc_empire_a.txt"
	EMPIREDESC_B = "locale/ymir/desc_empire_b.txt"
	EMPIREDESC_C = "locale/ymir/desc_empire_c.txt"

	LOCALE_INTERFACE_FILE_NAME = "locale/ymir/locale_interface.txt"
	NEW_LOCALE_INTERFACE_FILE_NAME = "locale/ymir/new_locale_interface.txt"
else:
	if "HONGKONG" == app.GetLocaleServiceName():
		name = "locale/hongkong"
	elif "JAPAN" == app.GetLocaleServiceName():
		name = "locale/japan"
	elif "TAIWAN" == app.GetLocaleServiceName():
		name = "locale/taiwan"
	elif "NEWCIBN" == app.GetLocaleServiceName():
		name = "locale/newcibn"
	elif "EUROPE" == app.GetLocaleServiceName():
		name = app.GetLocalePath()
	else:
		name = "locale/ymir"

	LOCALE_COMMON_UISCRIPT_PATH = "locale/common/ui/"
	LOCALE_UISCRIPT_PATH = "%s/ui/" % (name)

	LOGIN_PATH = "locale/common/ui/login/"
	EMPIRE_PATH = "%s/ui/empire/" % (name)
	GUILD_PATH = "locale/common/ui/guild/"
	SELECT_PATH = "%s/ui/select/" % (name)
	WINDOWS_PATH = "%s/ui/windows/" % (name)
	MAPNAME_PATH = "%s/ui/mapname/" % (name)

	JOBDESC_WARRIOR_PATH = "%s/jobdesc_warrior.txt" % (name)
	JOBDESC_ASSASSIN_PATH = "%s/jobdesc_assassin.txt" % (name)
	JOBDESC_SURA_PATH = "%s/jobdesc_sura.txt" % (name)
	JOBDESC_SHAMAN_PATH = "%s/jobdesc_shaman.txt" % (name)
	JOBDESC_WOLFMAN_PATH = "%s/jobdesc_wolfman.txt" % (name)

	EMPIREDESC_A = "%s/empiredesc_a.txt" % (name)
	EMPIREDESC_B = "%s/empiredesc_b.txt" % (name)
	EMPIREDESC_C = "%s/empiredesc_c.txt" % (name)

	LOCALE_INTERFACE_FILE_NAME = "%s/locale_interface.txt" % (name)
	NEW_LOCALE_INTERFACE_FILE_NAME = "%s/new_locale_interface.txt" % (name)

	if app.ENABLE_LOADING_TIP:
		LOADING_TIP_LIST = "locale/common/loading_tip_list.txt"
		LOADING_TIP_VNUM = "%s/loading_tip_vnum.txt" % (name)

	if app.ENABLE_MINI_GAME_RUMI:
		MINIGAME_RUMI_DESC = "%s/mini_game_okey_desc.txt" % (name)

	if app.ENABLE_MINI_GAME_YUTNORI:
		YUTNORI_EVENT_DESC = "%s/yutnori_event_desc.txt" % (name)

	if app.ENABLE_MINI_GAME_CATCH_KING:
		MINIGAME_CATCH_KING_DESC = "%s/catchking_event_desc.txt" % (name)
		MINIGAME_CATCH_KING_SIMPLE_DESC = "%s/catchking_event_simple_desc.txt" % (name)

	if app.ENABLE_SNOWFLAKE_STICK_EVENT:
		SNOWFLAKE_STICK_EVENT_DESC_FILE = "%s/snowflake_stick_event_desc.txt" % (name)

if app.ENABLE_LOCALE_CLIENT:
	LoadLocaleFile(LOCALE_INTERFACE_FILE_NAME)
	LoadLocaleFile(NEW_LOCALE_INTERFACE_FILE_NAME)
else:
	LoadLocaleFile(LOCALE_INTERFACE_FILE_NAME, locals())
	LoadLocaleFile(NEW_LOCALE_INTERFACE_FILE_NAME, locals())
