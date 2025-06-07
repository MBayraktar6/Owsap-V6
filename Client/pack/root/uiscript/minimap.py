import app

ROOT = "d:/ymir work/ui/minimap/"

window = {
	"name" : "MiniMap",

	"x" : SCREEN_WIDTH - 136,
	"y" : 0,

	"width" : 136,
	"height" : 137,

	"children" :
	[
		## OpenWindow
		{
			"name" : "OpenWindow",
			"type" : "window",

			"x" : 0,
			"y" : 0,

			"width" : 136,
			"height" : 137,

			"children" :
			[
				{
					"name" : "OpenWindowBGI",
					"type" : "image",
					"x" : 0,
					"y" : 0,
					"image" : ROOT + "minimap.sub",
				},
				## MiniMapWindow
				{
					"name" : "MiniMapWindow",
					"type" : "window",

					"x" : 4,
					"y" : 5,

					"width" : 128,
					"height" : 128,
				},
				## ScaleUpButton
				{
					"name" : "ScaleUpButton",
					"type" : "button",

					"x" : 101,
					"y" : 116,

					"default_image" : ROOT + "minimap_scaleup_default.sub",
					"over_image" : ROOT + "minimap_scaleup_over.sub",
					"down_image" : ROOT + "minimap_scaleup_down.sub",
				},
				## ScaleDownButton
				{
					"name" : "ScaleDownButton",
					"type" : "button",

					"x" : 115,
					"y" : 103,

					"default_image" : ROOT + "minimap_scaledown_default.sub",
					"over_image" : ROOT + "minimap_scaledown_over.sub",
					"down_image" : ROOT + "minimap_scaledown_down.sub",
				},
				## MiniMapHideButton
				{
					"name" : "MiniMapHideButton",
					"type" : "button",

					"x" : 111,
					"y" : 6,

					"default_image" : ROOT + "minimap_close_default.sub",
					"over_image" : ROOT + "minimap_close_over.sub",
					"down_image" : ROOT + "minimap_close_down.sub",
				},
				## AtlasShowButton
				{
					"name" : "AtlasShowButton",
					"type" : "button",

					"x" : 12,
					"y" : 12,

					"default_image" : ROOT + "atlas_open_default.sub",
					"over_image" : ROOT + "atlas_open_over.sub",
					"down_image" : ROOT + "atlas_open_down.sub",
				},
				## ServerInfo
				{
					"name" : "ServerInfo",
					"type" : "text",

					"text_horizontal_align" : "center",

					"outline" : 1,

					"x" : 70,
					"y" : 140,

					"text" : "",
				},
				## PositionInfo
				{
					"name" : "PositionInfo",
					"type" : "text",

					"text_horizontal_align" : "center",

					"outline" : 1,

					"x" : 70,
					"y" : 160,

					"text" : "",
				},
				## ObserverCount
				{
					"name" : "ObserverCount",
					"type" : "text",

					"text_horizontal_align" : "center",

					"outline" : 1,

					"x" : 70,
					"y" : 180,

					"text" : "",
				},
			],
		},
		{
			"name" : "CloseWindow",
			"type" : "window",

			"x" : 0,
			"y" : 0,

			"width" : 132,
			"height" : 48,

			"children" :
			[
				## ShowButton
				{
					"name" : "MiniMapShowButton",
					"type" : "button",

					"x" : 100,
					"y" : 4,

					"default_image" : ROOT + "minimap_open_default.sub",
					"over_image" : ROOT + "minimap_open_default.sub",
					"down_image" : ROOT + "minimap_open_default.sub",
				},
			],
		},
	],
}

window["x"] = window["x"] - 15
window["width"] = window["width"] + 15
window["children"][0]["width"] = window["children"][0]["width"] + 15
window["children"][1]["width"] = window["children"][1]["width"] + 15

for idx in xrange(len(window["children"][0]["children"])):
	window["children"][0]["children"][idx]["x"] = window["children"][0]["children"][idx]["x"] + 15

window["children"][1]["children"][0]["x"] = window["children"][1]["children"][0]["x"] + 15
window["children"][1]["children"][0]["y"] = 2

window["children"][0]["children"][5]["x"] = 0
window["children"][0]["children"][5]["y"] = 57

if app.ENABLE_GUILD_DRAGONLAIR_PARTY_SYSTEM:
	window["children"][0]["children"] = window["children"][0]["children"] + [
		## GuildDragonlairFirstGuildText
		{
			"name" : "GuildDragonlairFirstGuildText",
			"type" : "text",

			"text_horizontal_align" : "center",

			"outline" : 1,

			"x" : 60,
			"y" : 160,

			"text" : "1st",
		},
		## GuildDragonlairFirstGuildSecond
		{
			"name" : "GuildDragonlairFirstGuildSecond",
			"type" : "text",

			"text_horizontal_align" : "center",

			"outline" : 1,

			"x" : 100,
			"y" : 160,

			"text" : "None",
		},
	]

if app.ENABLE_MAILBOX:
	window["children"][0]["children"] = window["children"][0]["children"] + [
		## MailBox
		{
			"name" : "MailBoxButton",
			"type" : "button",

			"x" : 126,
			"y" : 30,

			"default_image" : "d:/ymir work/ui/game/mailbox/post_minimap.sub",
			"over_image" : "d:/ymir work/ui/game/mailbox/post_minimap.sub",
			"down_image" : "d:/ymir work/ui/game/mailbox/post_minimap.sub",
		},
		{
			"name" : "MailBoxEffect",
			"type" : "ani_image",

			"x" : 126,
			"y" : 30,

			"delay" : 6,

			"images" :
			(
				"d:/ymir work/ui/game/mailbox/minimap_flash/2.sub",
				"d:/ymir work/ui/game/mailbox/minimap_flash/3.sub",
				"d:/ymir work/ui/game/mailbox/minimap_flash/4.sub",
				"d:/ymir work/ui/game/mailbox/minimap_flash/5.sub",
				"d:/ymir work/ui/game/mailbox/minimap_flash/4.sub",
				"d:/ymir work/ui/game/mailbox/minimap_flash/3.sub",
				"d:/ymir work/ui/game/mailbox/minimap_flash/2.sub",
				"d:/ymir work/ui/game/mailbox/minimap_flash/1.sub",
				"d:/ymir work/ui/game/mailbox/minimap_flash/1.sub",
				"d:/ymir work/ui/game/mailbox/minimap_flash/1.sub",
				"d:/ymir work/ui/game/mailbox/minimap_flash/1.sub",
				"d:/ymir work/ui/game/mailbox/minimap_flash/1.sub",
			),
		},
	]

if app.ENABLE_EVENT_BANNER:
	window["children"][0]["children"] = window["children"][0]["children"] + [
		## InGameEventButton
		{
			"name" : "InGameEventButton",
			"type" : "button",

			"x" : 2,
			"y" : 28,

			"default_image" : "d:/ymir work/ui/minimap/E_open_default.tga",
			"over_image" : "d:/ymir work/ui/minimap/E_open_over.tga",
			"down_image" : "d:/ymir work/ui/minimap/E_open_down.tga",
		},
	]

if app.ENABLE_CLIENT_TIMER:
	window["children"][0]["children"] = window["children"][0]["children"] + [
		{
			"name" : "ClientTimerText",
			"type" : "text",

			"text_horizontal_align" : "center",
			"outline" : 1,

			"x" : 80,
			"y" : 160,
		},
	]
