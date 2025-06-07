import uiScriptLocale
import app

window = {
	"name" : "GuildBanklogWindow",
	"style": ("movable","float"),

	"x" : 280,
	"y" : 20,

	"width"  : 305,
	"height" : 356,

	"children":
	(
		{
			"name" : "Board",
			"type" : "board",

			"x" : 0,
			"y" : 0,

			"width" : 305,
			"height": 356,
			
			"children":
			(
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),
					"x" : 8,
					"y" : 7,
				
					"width" : 285,
					"color" : "yellow",
						"children" :
						(
							{ "name":"TitleName", "type":"text", "x":140, "y":3, "text":uiScriptLocale.GUILD_BANKINFO_TITLE, "text_horizontal_align":"center" },
						),
				},
				## ScrollBar
				{
					"name" : "ScrollBar",
					"type" : "scrollbar",

					"x" : 280,
					"y" : 50,
					"scrollbar_type" : "normal",
					"size" : 260,
				},
				## TextLine
				{
					"name" : "IndexName", "type" : "text", "x" : 48, "y" : 40, "text" : uiScriptLocale.GUILD_BANKINFO_NAME,
				},
				{
					"name" : "IndexGrade", "type" : "text", "x" : 118, "y" : 40, "text" : uiScriptLocale.GUILD_BANKINFO_TYPE,
				},
				{
					"name" : "IndexJob", "type" : "text", "x" : 233, "y" : 40, "text" : uiScriptLocale.GUILD_BANKINFO_USETYPE,
				},
				## Button
				{
					"name" : "ExitButton",
					"type" : "button",
					"x" : 200,
					"y" : 325,
					"text" : uiScriptLocale.GUILD_BANKINFO_CLOSE,
					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",						
				},
			),
		},
	),
}
