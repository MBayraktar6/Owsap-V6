import uiScriptLocale
import app

SMALL_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_01.sub"
LARGE_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_03.sub"
XLARGE_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_04.sub"

window = {
	"name" : "InputDialog",

	"x" : 0,
	"y" : 0,

	"style" : ("float",),

	"width" : 160,
	"height" : 120,

	"children" :
	(
		{
			"name" : "Mainboard",
			"type" : "board",

			"x" : 0,
			"y" : 0,

			"width" :160,
			"height" : 120,

			"children" :
			(
				## TitleBar
				{
					"name" : "Board",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 7,
					"y" : 7,

					"width" : 144,
					"color" : "yellow",

					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":75, "y":4, "text":uiScriptLocale.GUILD_WAR_BATTLE_TYPE, "text_horizontal_align":"center" },
					),
				},
				{
					"name" : "Defense_Button",
					"type" : "button",
					"x" : 18 + 65,
					"y" : 35 + 25 + 25,
					"text" : uiScriptLocale.GUILD_WAR_TYPE_DEFENSE,
					"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
				},
				{
					"name" : "Time_Button",
					"type" : "button",
					"x" : 18,
					"y" : 35 + 25 + 25,
					"text" : uiScriptLocale.GUILD_WAR_TYPE_TIME,
					"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
				},
				{
					"name" : "King_Button",
					"type" : "button",
					"x" : 18,
					"y" : 35,
					"text" : uiScriptLocale.GUILD_WAR_TYPE_NORMAL,
					"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
				},
				{
					"name" : "Die_Button",
					"type" : "button",
					"x" : 18 + 65,
					"y" : 35,
					"text" : uiScriptLocale.GUILD_WAR_TYPE_DIE,
					"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
				},
				{
					"name" : "Protect_Button",
					"type" : "button",
					"x" : 18,
					"y" : 35 + 25,
					"text" : uiScriptLocale.GUILD_WAR_TYPE_FLAG,
					"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
				},
				{
					"name" : "Tiger_Button",
					"type" : "button",
					"x" : 18 + 65,
					"y" : 35 + 25,
					"text" : uiScriptLocale.GUILD_WAR_TYPE_TIGER,
					"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
				},
			),
		},
	),
}