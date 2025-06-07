import app
import uiScriptLocale
import localeInfo

ROOT = "d:/ymir work/ui/public/"
VERSION_ADD_Y = 10

window = {
	"name" : "SystemDialog",
	"style" : ("float",),

	"x" : (SCREEN_WIDTH  - 200) / 2,
	"y" : 0,

	"width" : 200,
	"height" : 40 + VERSION_ADD_Y,

	"children" :
	[
		{
			"name" : "board",
			"type" : "thinboard",

			"x" : 0,
			"y" : 0,

			"width" : 200,
			"height" : 40 + VERSION_ADD_Y,

			"children" :
			[
			],
		},
	],
}

# µµ¿ò¸»
ADD_Y = 17
window["height"] = window["height"] + 17
window["children"][0]["height"] = window["children"][0]["height"] + 17
window["children"][0]["children"] = window["children"][0]["children"] + [
	{
		"name" : "help_button",
		"type" : "button",

		"x" : 10,
		"y" : ADD_Y,

		"text" : uiScriptLocale.SYSTEM_HELP,

		"default_image" : ROOT + "XLarge_Button_01.sub",
		"over_image" : ROOT + "XLarge_Button_02.sub",
		"down_image" : ROOT + "XLarge_Button_03.sub",
	},
]

ADD_Y += 30
window["height"] = window["height"] + 30
window["children"][0]["height"] = window["children"][0]["height"] + 30
window["children"][0]["children"] = window["children"][0]["children"] + [
	{
		"name" : "version",
		"type" : "text",

		"x" : 0,
		"y" : ADD_Y,

		"horizontal_align" : "center",
		"text_horizontal_align" : "center",

		"text" : "Ver. " + app.VERSION,
	},
]

# ½Ã½ºÅÛ ¿É¼Ç
ADD_Y += VERSION_ADD_Y
window["height"] = window["height"] + VERSION_ADD_Y
window["children"][0]["height"] = window["children"][0]["height"] + VERSION_ADD_Y
window["children"][0]["children"] = window["children"][0]["children"] + [
	{
		"name" : "system_option_button",
		"type" : "button",

		"x" : 10,
		"y" : ADD_Y + VERSION_ADD_Y,

		"text" : uiScriptLocale.SYSTEMOPTION_TITLE,

		"default_image" : ROOT + "XLarge_Button_01.sub",
		"over_image" : ROOT + "XLarge_Button_02.sub",
		"down_image" : ROOT + "XLarge_Button_03.sub",
	},
]

# °ÔÀÓ¿É¼Ç
ADD_Y += 30
window["height"] = window["height"] + 30
window["children"][0]["height"] = window["children"][0]["height"] + 30
window["children"][0]["children"] = window["children"][0]["children"] + [
	{
		"name" : "game_option_button",
		"type" : "button",

		"x" : 10,
		"y" : ADD_Y + VERSION_ADD_Y,

		"text" : uiScriptLocale.GAMEOPTION_TITLE,

		"default_image" : ROOT + "XLarge_Button_01.sub",
		"over_image" : ROOT + "XLarge_Button_02.sub",
		"down_image" : ROOT + "XLarge_Button_03.sub",
	},
]

if app.ENABLE_GAME_OPTION_ESCAPE:
	# Á¶ÀÛ ºÒ°¡ Å»ÃâÇÏ±â
	ADD_Y += 30
	window["height"] = window["height"] + 30
	window["children"][0]["height"] = window["children"][0]["height"] + 30
	window["children"][0]["children"] = window["children"][0]["children"] + [
		{
			"name" : "escape_button",
			"type" : "button",

			"x" : 10,
			"y" : ADD_Y + VERSION_ADD_Y,

			"text" : uiScriptLocale.SYSTEM_CANNOT_CONTROL_AND_ESCAPTE,

			"default_image" : ROOT + "XLarge_Button_01.sub",
			"over_image" : ROOT + "XLarge_Button_02.sub",
			"down_image" : ROOT + "XLarge_Button_03.sub",
		},
	]

# Ä³¸¯ÅÍ ÀüÈ¯ÇÏ±â
ADD_Y += 30
window["height"] = window["height"] + 30
window["children"][0]["height"] = window["children"][0]["height"] + 30
window["children"][0]["children"] = window["children"][0]["children"] + [
	{
		"name" : "change_button",
		"type" : "button",

		"x" : 10,
		"y" : ADD_Y + VERSION_ADD_Y,

		"text" : uiScriptLocale.SYSTEM_CHANGE,

		"default_image" : ROOT + "XLarge_Button_01.sub",
		"over_image" : ROOT + "XLarge_Button_02.sub",
		"down_image" : ROOT + "XLarge_Button_03.sub",
	},
]

if app.ENABLE_MOVE_CHANNEL:
	ADD_Y += 30
	window["height"] = window["height"] + 30
	window["children"][0]["height"] = window["children"][0]["height"] + 30
	window["children"][0]["children"] = window["children"][0]["children"] + [
		{
			"name" : "movechannel_button",
			"type" : "button",

			"x" : 10,
			"y" : ADD_Y + VERSION_ADD_Y,

			"text" : uiScriptLocale.SYSTEM_MOVE_CHANNEL,

			"default_image" : ROOT + "XLarge_Button_01.sub",
			"over_image" : ROOT + "XLarge_Button_02.sub",
			"down_image" : ROOT + "XLarge_Button_03.sub",
		},
	]

# ·Î±× ¾Æ¿ô
ADD_Y += 30
window["height"] = window["height"] + 30
window["children"][0]["height"] = window["children"][0]["height"] + 30
window["children"][0]["children"] = window["children"][0]["children"] + [
	{
		"name" : "logout_button",
		"type" : "button",

		"x" : 10,
		"y" : ADD_Y + VERSION_ADD_Y,

		"text" : uiScriptLocale.SYSTEM_LOGOUT,

		"default_image" : ROOT + "XLarge_Button_01.sub",
		"over_image" : ROOT + "XLarge_Button_02.sub",
		"down_image" : ROOT + "XLarge_Button_03.sub",
	},
]

ADD_Y += 40
window["height"] = window["height"] + 40
window["children"][0]["height"] = window["children"][0]["height"] + 40
window["children"][0]["children"] = window["children"][0]["children"] + [
	{
		"name" : "exit_button",
		"type" : "button",

		"x" : 10,
		"y" : ADD_Y + VERSION_ADD_Y,

		"text" : uiScriptLocale.SYSTEM_EXIT,

		"default_image" : ROOT + "XLarge_Button_01.sub",
		"over_image" : ROOT + "XLarge_Button_02.sub",
		"down_image" : ROOT + "XLarge_Button_03.sub",
	},
]

ADD_Y += 30
window["height"] = window["height"] + 30
window["children"][0]["height"] = window["children"][0]["height"] + 30
window["children"][0]["children"] = window["children"][0]["children"] + [
	{
		"name" : "cancel_button",
		"type" : "button",

		"x" : 10,
		"y" : ADD_Y + VERSION_ADD_Y,

		"text" : uiScriptLocale.CANCEL,

		"default_image" : ROOT + "XLarge_Button_01.sub",
		"over_image" : ROOT + "XLarge_Button_02.sub",
		"down_image" : ROOT + "XLarge_Button_03.sub",
	},
]

window["y"] = (SCREEN_HEIGHT - window["height"]) / 2
