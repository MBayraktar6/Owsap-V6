import uiScriptLocale
import app

ROOT_PATH = "d:/ymir work/ui/public/"

TEMPORARY_X = +13
BUTTON_TEMPORARY_X = 5
PVP_X = -10

LINE_LABEL_X = 30
LINE_DATA_X = 90
LINE_BEGIN = 40
LINE_STEP = 25

SMALL_BUTTON_WIDTH = 45
MIDDLE_BUTTON_WIDTH = 65

window = {
	"name" : "GameOptionDialog",
	"style" : ["movable", "float",],

	"x" : 0,
	"y" : 0,

	"width" : 300,
	"height" : LINE_BEGIN + 12,

	"children" :
	[
		{
			"name" : "board",
			"type" : "board",

			"x" : 0,
			"y" : 0,

			"width" : 300,
			"height" : LINE_BEGIN + 12,

			"children" :
			[
				## Title
				{
					"name" : "titlebar",
					"type" : "titlebar",
					"style" : ["attach",],

					"x" : 8,
					"y" : 8,

					"width" : 284,
					"color" : "gray",

					"children":
					[
						{
							"name" : "titlename",
							"type" : "text",

							"x" : 0,
							"y" : 3,

							"text" : uiScriptLocale.GAMEOPTION_TITLE,

							"horizontal_align" : "center",
							"text_horizontal_align" : "center",
						},
					],
				},
			],
		},
	],
}

LINE_NUMBER = 0
window["height"] = window["height"] + LINE_STEP
window["children"][0]["height"] = window["children"][0]["height"] + LINE_STEP
window["children"][0]["children"] = [
	## 이름색생
	{
		"name" : "name_color",
		"type" : "text",

		"x" : LINE_LABEL_X,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER + 2,

		"text" : uiScriptLocale.OPTION_NAME_COLOR,
	},
	{
		"name" : "name_color_normal",
		"type" : "radio_button",

		"x" : LINE_DATA_X + MIDDLE_BUTTON_WIDTH * 0,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,

		"text" : uiScriptLocale.OPTION_NAME_COLOR_NORMAL,

		"default_image" : ROOT_PATH + "Middle_Button_01.sub",
		"over_image" : ROOT_PATH + "Middle_Button_02.sub",
		"down_image" : ROOT_PATH + "Middle_Button_03.sub",
	},
	{
		"name" : "name_color_empire",
		"type" : "radio_button",

		"x" : LINE_DATA_X + MIDDLE_BUTTON_WIDTH * 1,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,

		"text" : uiScriptLocale.OPTION_NAME_COLOR_EMPIRE,

		"default_image" : ROOT_PATH + "Middle_Button_01.sub",
		"over_image" : ROOT_PATH + "Middle_Button_02.sub",
		"down_image" : ROOT_PATH + "Middle_Button_03.sub",
	},
] + window["children"][0]["children"]

LINE_NUMBER += 1
window["height"] = window["height"] + LINE_STEP
window["children"][0]["height"] = window["children"][0]["height"] + LINE_STEP
window["children"][0]["children"] = [
	## 적 메뉴
	{
		"name" : "target_board",
		"type" : "text",

		"x" : LINE_LABEL_X,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER + 2,

		"text" : uiScriptLocale.OPTION_TARGET_BOARD,
	},
	{
		"name" : "target_board_no_view",
		"type" : "radio_button",

		"x" : LINE_DATA_X + MIDDLE_BUTTON_WIDTH * 0,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,

		"text" : uiScriptLocale.OPTION_TARGET_BOARD_NO_VIEW,

		"default_image" : ROOT_PATH + "Middle_Button_01.sub",
		"over_image" : ROOT_PATH + "Middle_Button_02.sub",
		"down_image" : ROOT_PATH + "Middle_Button_03.sub",
	},
	{
		"name" : "target_board_view",
		"type" : "radio_button",

		"x" : LINE_DATA_X + MIDDLE_BUTTON_WIDTH * 1,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,

		"text" : uiScriptLocale.OPTION_TARGET_BOARD_VIEW,

		"default_image" : ROOT_PATH + "Middle_Button_01.sub",
		"over_image" : ROOT_PATH + "Middle_Button_02.sub",
		"down_image" : ROOT_PATH + "Middle_Button_03.sub",
	},

] + window["children"][0]["children"]

LINE_NUMBER += 1
window["height"] = window["height"] + LINE_STEP
window["children"][0]["height"] = window["children"][0]["height"] + LINE_STEP
window["children"][0]["children"] = [
	## PvP 모드
	{
		"name" : "pvp_mode",
		"type" : "text",

		"x" : LINE_LABEL_X,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER + 2,

		"text" : uiScriptLocale.OPTION_PVPMODE,
	},
	{
		"name" : "pvp_peace",
		"type" : "radio_button",

		"x" : LINE_DATA_X + SMALL_BUTTON_WIDTH * 0,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,

		"text" : uiScriptLocale.OPTION_PVPMODE_PEACE,

		"default_image" : ROOT_PATH + "small_Button_01.sub",
		"over_image" : ROOT_PATH + "small_Button_02.sub",
		"down_image" : ROOT_PATH + "small_Button_03.sub",
	},
	{
		"name" : "pvp_revenge",
		"type" : "radio_button",

		"x" : LINE_DATA_X + SMALL_BUTTON_WIDTH * 1,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,

		"text" : uiScriptLocale.OPTION_PVPMODE_REVENGE,

		"default_image" : ROOT_PATH + "small_Button_01.sub",
		"over_image" : ROOT_PATH + "small_Button_02.sub",
		"down_image" : ROOT_PATH + "small_Button_03.sub",
	},
	{
		"name" : "pvp_guild",
		"type" : "radio_button",

		"x" : LINE_DATA_X + SMALL_BUTTON_WIDTH * 2,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,

		"text" : uiScriptLocale.OPTION_PVPMODE_GUILD,

		"default_image" : ROOT_PATH + "small_Button_01.sub",
		"over_image" : ROOT_PATH + "small_Button_02.sub",
		"down_image" : ROOT_PATH + "small_Button_03.sub",
	},
	{
		"name" : "pvp_free",
		"type" : "radio_button",

		"x" : LINE_DATA_X + SMALL_BUTTON_WIDTH * 3,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,

		"text" : uiScriptLocale.OPTION_PVPMODE_FREE,

		"default_image" : ROOT_PATH + "small_Button_01.sub",
		"over_image" : ROOT_PATH + "small_Button_02.sub",
		"down_image" : ROOT_PATH + "small_Button_03.sub",
	},
] + window["children"][0]["children"]

LINE_NUMBER += 1
window["height"] = window["height"] + LINE_STEP
window["children"][0]["height"] = window["children"][0]["height"] + LINE_STEP
window["children"][0]["children"] = [
	## 차단
	{
		"name" : "block",
		"type" : "text",

		"x" : LINE_LABEL_X,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER + 2,

		"text" : uiScriptLocale.OPTION_BLOCK,
	},
	{
		"name" : "block_exchange_button",
		"type" : "toggle_button",

		"x" : LINE_DATA_X + MIDDLE_BUTTON_WIDTH * 0,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,

		"text" : uiScriptLocale.OPTION_BLOCK_EXCHANGE,

		"default_image" : ROOT_PATH + "middle_button_01.sub",
		"over_image" : ROOT_PATH + "middle_button_02.sub",
		"down_image" : ROOT_PATH + "middle_button_03.sub",
	},
	{
		"name" : "block_party_button",
		"type" : "toggle_button",

		"x" : LINE_DATA_X + MIDDLE_BUTTON_WIDTH * 1,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,

		"text" : uiScriptLocale.OPTION_BLOCK_PARTY,

		"default_image" : ROOT_PATH + "middle_button_01.sub",
		"over_image" : ROOT_PATH + "middle_button_02.sub",
		"down_image" : ROOT_PATH + "middle_button_03.sub",
	},
	{
		"name" : "block_guild_button",
		"type" : "toggle_button",

		"x" : LINE_DATA_X + MIDDLE_BUTTON_WIDTH * 2,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,

		"text" : uiScriptLocale.OPTION_BLOCK_GUILD,

		"default_image" : ROOT_PATH + "middle_button_01.sub",
		"over_image" : ROOT_PATH + "middle_button_02.sub",
		"down_image" : ROOT_PATH + "middle_button_03.sub",
	},
] + window["children"][0]["children"]

LINE_NUMBER += 1
window["height"] = window["height"] + LINE_STEP
window["children"][0]["height"] = window["children"][0]["height"] + LINE_STEP
window["children"][0]["children"] = [
	## 채팅창
	{
		"name" : "block_whisper_button",
		"type" : "toggle_button",

		"x" : LINE_DATA_X + MIDDLE_BUTTON_WIDTH * 0,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,

		"text" : uiScriptLocale.OPTION_BLOCK_WHISPER,

		"default_image" : ROOT_PATH + "middle_button_01.sub",
		"over_image" : ROOT_PATH + "middle_button_02.sub",
		"down_image" : ROOT_PATH + "middle_button_03.sub",
	},
	{
		"name" : "block_friend_button",
		"type" : "toggle_button",

		"x" : LINE_DATA_X + MIDDLE_BUTTON_WIDTH * 1,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,

		"text" : uiScriptLocale.OPTION_BLOCK_FRIEND,

		"default_image" : ROOT_PATH + "middle_button_01.sub",
		"over_image" : ROOT_PATH + "middle_button_02.sub",
		"down_image" : ROOT_PATH + "middle_button_03.sub",
	},
	{
		"name" : "block_party_request_button",
		"type" : "toggle_button",

		"x" : LINE_DATA_X + MIDDLE_BUTTON_WIDTH * 2,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,

		"text" : uiScriptLocale.OPTION_BLOCK_PARTY_REQUEST,

		"default_image" : ROOT_PATH + "middle_button_01.sub",
		"over_image" : ROOT_PATH + "middle_button_02.sub",
		"down_image" : ROOT_PATH + "middle_button_03.sub",
	},
] + window["children"][0]["children"]

LINE_NUMBER += 1
window["height"] = window["height"] + LINE_STEP
window["children"][0]["height"] = window["children"][0]["height"] + LINE_STEP
window["children"][0]["children"] = [
	## 채팅창
	{
		"name" : "chat",
		"type" : "text",

		"x" : LINE_LABEL_X,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER + 2,

		"text" : uiScriptLocale.OPTION_VIEW_CHAT,
	},
	{
		"name" : "view_chat_on_button",
		"type" : "radio_button",

		"x" : LINE_DATA_X,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,

		"text" : uiScriptLocale.OPTION_VIEW_CHAT_ON,

		"default_image" : ROOT_PATH + "middle_button_01.sub",
		"over_image" : ROOT_PATH + "middle_button_02.sub",
		"down_image" : ROOT_PATH + "middle_button_03.sub",
	},
	{
		"name" : "view_chat_off_button",
		"type" : "radio_button",

		"x" : LINE_DATA_X + MIDDLE_BUTTON_WIDTH,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,

		"text" : uiScriptLocale.OPTION_VIEW_CHAT_OFF,

		"default_image" : ROOT_PATH + "middle_button_01.sub",
		"over_image" : ROOT_PATH + "middle_button_02.sub",
		"down_image" : ROOT_PATH + "middle_button_03.sub",
	},
] + window["children"][0]["children"]

LINE_NUMBER += 1
window["height"] = window["height"] + LINE_STEP
window["children"][0]["height"] = window["children"][0]["height"] + LINE_STEP
window["children"][0]["children"] = [
	## 이름보기
	{
		"name" : "always_show_name",
		"type" : "text",

		"x" : LINE_LABEL_X,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER + 2,

		"text" : uiScriptLocale.OPTION_ALWAYS_SHOW_NAME,
	},
	{
		"name" : "always_show_name_on_button",
		"type" : "radio_button",

		"x" : LINE_DATA_X,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,

		"text" : uiScriptLocale.OPTION_ALWAYS_SHOW_NAME_ON,

		"default_image" : ROOT_PATH + "middle_button_01.sub",
		"over_image" : ROOT_PATH + "middle_button_02.sub",
		"down_image" : ROOT_PATH + "middle_button_03.sub",
	},
	{
		"name" : "always_show_name_off_button",
		"type" : "radio_button",

		"x" : LINE_DATA_X + MIDDLE_BUTTON_WIDTH,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,

		"text" : uiScriptLocale.OPTION_ALWAYS_SHOW_NAME_OFF,

		"default_image" : ROOT_PATH + "middle_button_01.sub",
		"over_image" : ROOT_PATH + "middle_button_02.sub",
		"down_image" : ROOT_PATH + "middle_button_03.sub",
	},
] + window["children"][0]["children"]

if app.ENABLE_OPTIMIZATION:
	LINE_NUMBER += 1
	window["height"] = window["height"] + LINE_STEP
	window["children"][0]["height"] = window["children"][0]["height"] + LINE_STEP
	window["children"][0]["children"] = [
		## 타격 값
		{
			"name" : "effect_on_off",
			"type" : "text",

			"x" : LINE_LABEL_X,
			"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER + 2,

			"text" : uiScriptLocale.OPTION_EFFECT,
		},
		{
			"name" : "show_damage_on_button",
			"type" : "radio_button",

			"x" : LINE_DATA_X,
			"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,

			"text" : uiScriptLocale.OPTION_VIEW_CHAT_ON,

			"default_image" : ROOT_PATH + "middle_button_01.sub",
			"over_image" : ROOT_PATH + "middle_button_02.sub",
			"down_image" : ROOT_PATH + "middle_button_03.sub",
		},
		{
			"name" : "show_damage_off_button",
			"type" : "radio_button",

			"x" : LINE_DATA_X + MIDDLE_BUTTON_WIDTH,
			"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,

			"text" : uiScriptLocale.OPTION_VIEW_CHAT_OFF,

			"default_image" : ROOT_PATH + "middle_button_01.sub",
			"over_image" : ROOT_PATH + "middle_button_02.sub",
			"down_image" : ROOT_PATH + "middle_button_03.sub",
		},
	] + window["children"][0]["children"]

	LINE_NUMBER += 1
	window["height"] = window["height"] + LINE_STEP
	window["children"][0]["height"] = window["children"][0]["height"] + LINE_STEP
	window["children"][0]["children"] = [
		## 피격 모션
		{
			"name" : "other_char_attacked",
			"type" : "text",

			"x" : LINE_LABEL_X,
			"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER + 2,

			"text" : uiScriptLocale.OPTION_ATTACKED_MOTION,
		},
		{
			"name" : "other_char_attacked_all",
			"type" : "radio_button",

			"x" : LINE_DATA_X,
			"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,

			"text" : uiScriptLocale.OPTION_ATTACKED_MOTION_ALL,

			"default_image" : ROOT_PATH + "middle_button_01.sub",
			"over_image" : ROOT_PATH + "middle_button_02.sub",
			"down_image" : ROOT_PATH + "middle_button_03.sub",
		},
		{
			"name" : "other_char_attacked_self",
			"type" : "radio_button",

			"x" : LINE_DATA_X + MIDDLE_BUTTON_WIDTH,
			"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,

			"text" : uiScriptLocale.OPTION_ATTACKED_MOTION_SELF,

			"default_image" : ROOT_PATH + "middle_button_01.sub",
			"over_image" : ROOT_PATH + "middle_button_02.sub",
			"down_image" : ROOT_PATH + "middle_button_03.sub",
		},
	] + window["children"][0]["children"]

LINE_NUMBER += 1
window["height"] = window["height"] + LINE_STEP
window["children"][0]["height"] = window["children"][0]["height"] + LINE_STEP
window["children"][0]["children"] = [
	## 판매문구
	{
		"name" : "salestext_on_off",
		"type" : "text",

		"x" : LINE_LABEL_X,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER + 2,

		"text" : uiScriptLocale.OPTION_SALESTEXT,
	},
	{
		"name" : "salestext_on_button",
		"type" : "radio_button",

		"x" : LINE_DATA_X,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,

		"text" : uiScriptLocale.OPTION_SALESTEXT_VIEW_ON,

		"default_image" : ROOT_PATH + "middle_button_01.sub",
		"over_image" : ROOT_PATH + "middle_button_02.sub",
		"down_image" : ROOT_PATH + "middle_button_03.sub",
	},
	{
		"name" : "salestext_off_button",
		"type" : "radio_button",

		"x" : LINE_DATA_X + MIDDLE_BUTTON_WIDTH,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,

		"text" : uiScriptLocale.OPTION_SALESTEXT_VIEW_OFF,

		"default_image" : ROOT_PATH + "middle_button_01.sub",
		"over_image" : ROOT_PATH + "middle_button_02.sub",
		"down_image" : ROOT_PATH + "middle_button_03.sub",
	},
] + window["children"][0]["children"]

LINE_NUMBER += 1
window["height"] = window["height"] + LINE_STEP
window["children"][0]["height"] = window["children"][0]["height"] + LINE_STEP
window["children"][0]["children"] = [
	## 몹정보
	{
		"name" : "show_mob_info",
		"type" : "text",

		"multi_line" : 1,

		"x" : LINE_LABEL_X,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER + 2,

		"text" : uiScriptLocale.OPTION_MOB_INFO,
	},
	{
		"name" : "show_mob_level_button",
		"type" : "toggle_button",

		"x" : LINE_DATA_X,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,

		"text" : uiScriptLocale.OPTION_MOB_INFO_LEVEL,

		"default_image" : ROOT_PATH + "middle_button_01.sub",
		"over_image" : ROOT_PATH + "middle_button_02.sub",
		"down_image" : ROOT_PATH + "middle_button_03.sub",
	},
	{
		"name" : "show_mob_AI_flag_button",
		"type" : "toggle_button",

		"x" : LINE_DATA_X + MIDDLE_BUTTON_WIDTH,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,

		"text" : uiScriptLocale.OPTION_MOB_INFO_AGGR,

		"default_image" : ROOT_PATH + "middle_button_01.sub",
		"over_image" : ROOT_PATH + "middle_button_02.sub",
		"down_image" : ROOT_PATH + "middle_button_03.sub",
	},
] + window["children"][0]["children"]

if app.ENABLE_KEYCHANGE_SYSTEM:
	LINE_NUMBER += 1
	window["height"] = window["height"] + LINE_STEP
	window["children"][0]["height"] = window["children"][0]["height"] + LINE_STEP
	window["children"][0]["children"] = [
		## 키설정
		{
			"name" : "key_setting_show",
			"type" : "text",

			"x" : LINE_LABEL_X,
			"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER + 2,

			"text" : uiScriptLocale.OPTION_KEY_SETTING,
		},
		{
			"name" : "key_setting_show_button",
			"type" : "button",

			"x" : LINE_DATA_X,
			"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,

			"text" : uiScriptLocale.OPTION_SETTING,

			"default_image" : ROOT_PATH + "middle_button_01.sub",
			"over_image" : ROOT_PATH + "middle_button_02.sub",
			"down_image" : ROOT_PATH + "middle_button_03.sub",
		},
	] + window["children"][0]["children"]

if app.ENABLE_LOOTING_SYSTEM:
	LINE_NUMBER += 1
	window["height"] = window["height"] + LINE_STEP
	window["children"][0]["height"] = window["children"][0]["height"] + LINE_STEP
	window["children"][0]["children"] = [
		## 루팅
		{
			"name" : "looting_system_text",
			"type" : "text",

			"x" : LINE_LABEL_X,
			"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER + 2,

			"text" : uiScriptLocale.OPTION_LOOTING_SETTING,
		},
		{
			"name" : "looting_system_button",
			"type" : "button",

			"x" : LINE_DATA_X,
			"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,

			"text" : uiScriptLocale.OPTION_SETTING,

			"default_image" : ROOT_PATH + "middle_button_01.sub",
			"over_image" : ROOT_PATH + "middle_button_02.sub",
			"down_image" : ROOT_PATH + "middle_button_03.sub",
		},
	] + window["children"][0]["children"]

"""
LINE_NUMBER += 1
window["height"] = window["height"] + LINE_STEP
window["children"][0]["height"] = window["children"][0]["height"] + LINE_STEP
window["children"][0]["children"] = [
	## 숨기기
	{
		"name" : "structure_view_mode",
		"type" : "text",

		"multi_line" : 1,

		"x" : LINE_LABEL_X,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER + 2,

		"text" : uiScriptLocale.STRUCTURE_VIEW_MODE,
	},
	{
		"name" : "structure_view_mode_button",
		"type" : "toggle_button",

		"x" : LINE_DATA_X,
		"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,

		"text" : uiScriptLocale.STRUCTURE_VIEW_TYPE_STRUCTURE,

		"default_image" : ROOT_PATH + "middle_button_01.sub",
		"over_image" : ROOT_PATH + "middle_button_02.sub",
		"down_image" : ROOT_PATH + "middle_button_03.sub",
	},
] + window["children"][0]["children"]
"""

if app.ENABLE_LEFT_SEAT:
	LINE_NUMBER += 1
	window["height"] = window["height"] + LINE_STEP
	window["children"][0]["height"] = window["children"][0]["height"] + LINE_STEP
	window["children"][0]["children"] = [
		## 자리비움 대기시간
		{ "name" : "left_seat_time_bar_text", "type" : "text", "x" : LINE_LABEL_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "text" : uiScriptLocale.LEFT_SEAT,},
		{ "name" : "left_seat_time_list_button", "type" : "button", "x" : LINE_DATA_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "text" : uiScriptLocale.LEFT_SEAT_10_MIN, "default_image" : "d:/ymir work/ui/game/party_match/button_one.sub", "over_image" : "d:/ymir work/ui/game/party_match/button_one.sub", "down_image" : "d:/ymir work/ui/game/party_match/button_one.sub", },
		{
			"name" : "left_seat_time_list_window", "type" : "window", "x" : LINE_DATA_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER+16, "width" : 130, "height" : 80,
			"children":
			(
				{ "name" : "left_seat_time_10_min", "type" : "button", "x" : 0, "y" : 0, "text" : uiScriptLocale.LEFT_SEAT_10_MIN, "default_image" : "d:/ymir work/ui/game/party_match/button_top.sub", "over_image" : "d:/ymir work/ui/game/party_match/button_top.sub", "down_image" : "d:/ymir work/ui/game/party_match/button_top.sub", },
				{ "name" : "left_seat_time_30_min", "type" : "button", "x" : 0, "y" : 16, "text" : uiScriptLocale.LEFT_SEAT_30_MIN, "default_image" : "d:/ymir work/ui/game/party_match/button_middle.sub", "over_image" : "d:/ymir work/ui/game/party_match/button_middle.sub", "down_image" : "d:/ymir work/ui/game/party_match/button_middle.sub", },
				{ "name" : "left_seat_time_90_min", "type" : "button", "x" : 0, "y" : 32, "text" : uiScriptLocale.LEFT_SEAT_90_MIN, "default_image" : "d:/ymir work/ui/game/party_match/button_bottom.sub", "over_image" : "d:/ymir work/ui/game/party_match/button_bottom.sub", "down_image" : "d:/ymir work/ui/game/party_match/button_bottom.sub", },
			),
		},
		{ "name" : "left_seat_list_time_arrow_button", "type" : "button", "x" : LINE_DATA_X + 130, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "default_image" : "d:/ymir work/ui/game/party_match/arrow_default.sub", "over_image" : "d:/ymir work/ui/game/party_match/arrow_over.sub", "down_image" : "d:/ymir work/ui/game/party_match/arrow_down.sub", },
		{ "name" : "left_seat_list_time_mouse_over_image", "type" : "expanded_image", "style" : ("not_pick",), "x" : LINE_DATA_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "image" : "d:/ymir work/ui/game/party_match/button_over.sub", },
	] + window["children"][0]["children"]

	LINE_NUMBER += 1
	window["height"] = window["height"] + (LINE_STEP * 3)
	window["children"][0]["height"] = window["children"][0]["height"] + (LINE_STEP * 3)
	window["children"][0]["children"] = [
		## 자리비움 로그아웃 대기시간
		{ "name" : "left_seat_logout_bar_text", "type" : "text", "x" : LINE_LABEL_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "text" : uiScriptLocale.LEFT_SEAT_LOGOUT,},
		{ "name" : "left_seat_logout_list_button", "type" : "button", "x" : LINE_DATA_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "text" : uiScriptLocale.LEFT_SEAT_180_MIN, "default_image" : "d:/ymir work/ui/game/party_match/button_one.sub", "over_image" : "d:/ymir work/ui/game/party_match/button_one.sub", "down_image" : "d:/ymir work/ui/game/party_match/button_one.sub", },
		{
			"name" : "left_seat_logout_list_window", "type" : "window", "x" : LINE_DATA_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER+16, "width" : 130, "height" : 80,
			"children":
			(
				{ "name" : "left_seat_logout_30_min", "type" : "button", "x" : 0, "y" : 0, "text" : uiScriptLocale.LEFT_SEAT_30_MIN, "default_image" : "d:/ymir work/ui/game/party_match/button_top.sub", "over_image" : "d:/ymir work/ui/game/party_match/button_top.sub", "down_image" : "d:/ymir work/ui/game/party_match/button_top.sub", },
				{ "name" : "left_seat_logout_60_min", "type" : "button", "x" : 0, "y" : 16, "text" : uiScriptLocale.LEFT_SEAT_60_MIN, "default_image" : "d:/ymir work/ui/game/party_match/button_middle.sub", "over_image" : "d:/ymir work/ui/game/party_match/button_middle.sub", "down_image" : "d:/ymir work/ui/game/party_match/button_middle.sub", },
				{ "name" : "left_seat_logout_120_min", "type" : "button", "x" : 0, "y" : 32, "text" : uiScriptLocale.LEFT_SEAT_120_MIN, "default_image" : "d:/ymir work/ui/game/party_match/button_middle.sub", "over_image" : "d:/ymir work/ui/game/party_match/button_middle.sub", "down_image" : "d:/ymir work/ui/game/party_match/button_middle.sub", },
				{ "name" : "left_seat_logout_180_min", "type" : "button", "x" : 0, "y" : 48, "text" : uiScriptLocale.LEFT_SEAT_180_MIN, "default_image" : "d:/ymir work/ui/game/party_match/button_middle.sub", "over_image" : "d:/ymir work/ui/game/party_match/button_middle.sub", "down_image" : "d:/ymir work/ui/game/party_match/button_middle.sub", },
				{ "name" : "left_seat_logout_off", "type" : "button", "x" : 0, "y" : 64, "text" : uiScriptLocale.LEFT_SEAT_OFF, "default_image" : "d:/ymir work/ui/game/party_match/button_bottom.sub", "over_image" : "d:/ymir work/ui/game/party_match/button_bottom.sub", "down_image" : "d:/ymir work/ui/game/party_match/button_bottom.sub", },
			),
		},
		{ "name" : "left_seat_logout_list_arrow_button", "type" : "button", "x" : LINE_DATA_X + 130, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "default_image" : "d:/ymir work/ui/game/party_match/arrow_default.sub", "over_image" : "d:/ymir work/ui/game/party_match/arrow_over.sub", "down_image" : "d:/ymir work/ui/game/party_match/arrow_down.sub", },
		{ "name" : "left_seat_logout_list_mouse_over_image", "type" : "expanded_image", "style" : ("not_pick",), "x" : LINE_DATA_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "image" : "d:/ymir work/ui/game/party_match/button_over.sub", },
	] + window["children"][0]["children"]

if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
	LINE_NUMBER += 1
	window["height"] = window["height"] + LINE_STEP
	window["children"][0]["height"] = window["children"][0]["height"] + LINE_STEP
	window["children"][0]["children"] = [
		{
			"name" : "always_show_country",
			"type" : "text",

			"x" : LINE_LABEL_X,
			"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER + 2,

			"text" : uiScriptLocale.OPTION_ALWAYS_SHOW_COUNTRY,
		},
		{
			"name" : "always_show_country_on_button",
			"type" : "radio_button",

			"x" : LINE_DATA_X,
			"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,

			"text" : uiScriptLocale.OPTION_VIEW_CHAT_ON,

			"default_image" : ROOT_PATH + "middle_button_01.sub",
			"over_image" : ROOT_PATH + "middle_button_02.sub",
			"down_image" : ROOT_PATH + "middle_button_03.sub",
		},
		{
			"name" : "always_show_country_off_button",
			"type" : "radio_button",

			"x" : LINE_DATA_X + MIDDLE_BUTTON_WIDTH,
			"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,

			"text" : uiScriptLocale.OPTION_VIEW_CHAT_OFF,

			"default_image" : ROOT_PATH + "middle_button_01.sub",
			"over_image" : ROOT_PATH + "middle_button_02.sub",
			"down_image" : ROOT_PATH + "middle_button_03.sub",
		},
	] + window["children"][0]["children"]
