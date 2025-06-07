import uiScriptLocale
import app

window = {
	"name" : "GameWindow",
	"style" : ("not_pick",),

	"x" : 0,
	"y" : 0,

	"width" : SCREEN_WIDTH,
	"height" : SCREEN_HEIGHT,

	"children" :
	[
		{ 
			"name":"HelpButton", 
			"type":"button", 
			"x" : 50,
			"y" : SCREEN_HEIGHT-170,
			"default_image" : "d:/ymir work/ui/game/windows/btn_bigplus_up.sub",
			"over_image" : "d:/ymir work/ui/game/windows/btn_bigplus_over.sub",
			"down_image" : "d:/ymir work/ui/game/windows/btn_bigplus_down.sub",

			"children" : 
			(
				{ 
					"name":"HelpButtonLabel", 
					"type":"text", 
					"x": 16, 
					"y": 40, 
					"text":uiScriptLocale.GAME_HELP, 
					"r":1.0, "g":1.0, "b":1.0, "a":1.0, 
					"text_horizontal_align":"center" 
				},
			),
		},
		{ 
			"name":"QuestButton", 
			"type":"button", 
			"x" : SCREEN_WIDTH-50-32,
			"y" : SCREEN_HEIGHT-170,
			"default_image" : "d:/ymir work/ui/game/windows/btn_bigplus_up.sub",
			"over_image" : "d:/ymir work/ui/game/windows/btn_bigplus_over.sub",
			"down_image" : "d:/ymir work/ui/game/windows/btn_bigplus_down.sub",

			"children" : 
			(
				{ 
					"name":"QuestButtonLabel", 
					"type":"text", 
					"x": 16, 
					"y": 40, 
					"text":uiScriptLocale.GAME_QUEST, 
					"r":1.0, "g":1.0, "b":1.0, "a":1.0, 
					"text_horizontal_align":"center" 
				},
			),
		},
		{ 
			"name":"StatusPlusButton", 
			"type" : "button", 
			"x" : 68, 
			"y" : SCREEN_HEIGHT-100, 
			"default_image" : "d:/ymir work/ui/game/windows/btn_bigplus_up.sub",
			"over_image" : "d:/ymir work/ui/game/windows/btn_bigplus_over.sub",
			"down_image" : "d:/ymir work/ui/game/windows/btn_bigplus_down.sub",

			"children" :
			(
				{ 
					"name":"StatusPlusLabel", 
					"type":"text", 
					"x": 16, 
					"y": 40, 
					"text":uiScriptLocale.GAME_STAT_UP, 
					"r":1.0, "g":1.0, "b":1.0, "a":1.0, 
					"text_horizontal_align":"center" 
				},		
			),
		},			
		{ 
			"name":"SkillPlusButton", 
			"type" : "button", 
			"x" : SCREEN_WIDTH-50-32,
			"y" : SCREEN_HEIGHT-100,
			"default_image" : "d:/ymir work/ui/game/windows/btn_bigplus_up.sub",
			"over_image" : "d:/ymir work/ui/game/windows/btn_bigplus_over.sub",
			"down_image" : "d:/ymir work/ui/game/windows/btn_bigplus_down.sub",

			"children" : 
			(
				{ 
					"name":"SkillPlusLabel", 
					"type":"text", 
					"x": 16, 
					"y": 40, 
					"text":uiScriptLocale.GAME_SKILL_UP, 
					"r":1.0, "g":1.0, "b":1.0, "a":1.0, 
					"text_horizontal_align":"center" 
				},	
			),
		},			
		{ 
			"name":"ExitObserver", 
			"type" : "button", 
			"x" : SCREEN_WIDTH-50-32,
			"y" : SCREEN_HEIGHT-170,
			"default_image" : "d:/ymir work/ui/game/windows/btn_bigplus_up.sub",
			"over_image" : "d:/ymir work/ui/game/windows/btn_bigplus_over.sub",
			"down_image" : "d:/ymir work/ui/game/windows/btn_bigplus_down.sub",

			"children" : 
			(
				{ 
					"name":"ExitObserverButtonName", 
					"type":"text", 
					"x": 16, 
					"y": 40, 
					"text": uiScriptLocale.GAME_EXIT_OBSERVER, 
					"r":1.0, "g":1.0, "b":1.0, "a":1.0, 
					"text_horizontal_align":"center" 
				},	
			),
		},
		{ 
			"name":"BuildGuildBuilding",
			"type" : "button",
			"x" : SCREEN_WIDTH-50-32,
			"y" : SCREEN_HEIGHT-170,
			"default_image" : "d:/ymir work/ui/game/windows/btn_bigplus_up.sub",
			"over_image" : "d:/ymir work/ui/game/windows/btn_bigplus_over.sub",
			"down_image" : "d:/ymir work/ui/game/windows/btn_bigplus_down.sub",

			"children" : 
			(
				{ 
					"name":"BuildGuildBuildingButtonName",
					"type":"text",
					"x": 16,
					"y": 40,
					"text": uiScriptLocale.GUILD_BUILDING_TITLE,
					"r":1.0, "g":1.0, "b":1.0, "a":1.0,
					"text_horizontal_align":"center"
				},	
			),
		},
	],
}

#window["children"] = window["children"] + [
#		{ 
#			"name":"GuildWarButton", 
#			"type" : "button", 
#			"x" : SCREEN_WIDTH-50-32,
#			"y" : SCREEN_HEIGHT-240,
#			"default_image" : "d:/ymir work/ui/game/windows/btn_bigplus_up.sub",
#			"over_image" : "d:/ymir work/ui/game/windows/btn_bigplus_over.sub",
#			"down_image" : "d:/ymir work/ui/game/windows/btn_bigplus_down.sub",
#
#			"children" : 
#			(
#				{ 
#					"name":"GuildWarButtonName", 
#					"type":"text", 
#					"x": 16, 
#					"y": 40, 
#					"text": uiScriptLocale.GAME_GUILD_WAR_JOIN, 
#					"r":1.0, "g":1.0, "b":1.0, "a":1.0, 
#					"text_horizontal_align":"center" 
#				},	
#			),
#		},]

#window["children"] = window["children"] + [
#	{
#		"name" : "MistsIslandWarpButton", "type" : "button",
#		"x" : SCREEN_WIDTH - 82, "y" : SCREEN_HEIGHT - 190,
#		"default_image"	: "d:/ymir work/ui/game/windows/btn_bigplus_up.sub",
#		"over_image"	: "d:/ymir work/ui/game/windows/btn_bigplus_over.sub",
#		"down_image"	: "d:/ymir work/ui/game/windows/btn_bigplus_down.sub",
#		"children" :
#		(
#			{
#				"name" : "MistIslandWarpLabel", "type" : "text",
#				"x" : 16, "y" : 40,
#				"text" : uiScriptLocale.WARP_MI_LABEL,
#				"r" : 1.0, "g" : 1.0, "b" : 1.0, "a" : 1.0,
#				"text_horizontal_align" : "center",
#			},
#		),
#	},
#]

if app.ENABLE_POPUP_NOTICE:
	window["children"] = window["children"] + [
		# PopupNoticeButton
		{
			"name" :	"PopupNoticeButton", 
			"type" :	"button",
			"x" :		SCREEN_WIDTH-32-5,
			"y" :		SCREEN_HEIGHT-125,
			"width" :	32,
			"height" :	32,

			"default_image"	: "d:/ymir work/ui/game/windows/btn_popup_notice_up.sub",
			"over_image"	: "d:/ymir work/ui/game/windows/btn_popup_notice_over.sub",
			"down_image"	: "d:/ymir work/ui/game/windows/btn_popup_notice_down.sub",

		},

		## PopupNotice animation image
		{
			"name" :		"PopupNoticeAnimImage",
			"type" :		"ani_image",
			
			"x" :			SCREEN_WIDTH-32-5,
			"y" :			SCREEN_HEIGHT-125,
			"width" :		32,
			"height" :		32,
			"delay" :		10,
			
			"images" :
			(
				"d:/ymir work/ui/game/windows/btn_popup_notice_over.sub",
				"d:/ymir work/ui/game/windows/btn_popup_notice_up.sub",
				"d:/ymir work/ui/game/windows/btn_popup_notice_over.sub",

			),
		},
	]
