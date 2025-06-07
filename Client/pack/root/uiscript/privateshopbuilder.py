import uiScriptLocale

BOARD_WIDTH = 184
BOARD_HEIGHT = 380
window = {
	"name" : "PrivateShopBuilder",

	"x" : 0,
	"y" : 0,

	"style" : ("movable", "float",),

	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : BOARD_WIDTH,
			"height" : BOARD_HEIGHT,

			"children" :
			(
				## Title
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 8,
					"y" : 8,

					"width" : 169,
					"color" : "gray",

					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":84, "y":4, "text":uiScriptLocale.PRIVATE_SHOP_TITLE, "text_horizontal_align":"center" },
					),
				},

				## Name_Static
				#{
				#	"name" : "Name_Static", "type" : "text", "x" : 15, "y" : 35 + 3, "text" : uiScriptLocale.PRIVATE_SHOP_NAME,
				#},
				## Name
				{
					"name" : "NameSlot",
					"type" : "slotbar",
					"x" : 13,
					"y" : 35,
					"width" : 90 + 67,
					"height" : 18,

					"children" :
					(
						{
							"name" : "NameLine",
							"type" : "text",
							"x" : 3,
							"y" : 3,
							"width" : 157,
							"height" : 15,
							"input_limit" : 25,
							"text" : "1234567890123456789012345",
						},
					),
				},

				## Item Slot
				{
					"name" : "ItemSlot",
					"type" : "grid_table",

					"x" : 12,
					"y" : 34 + 26,

					"start_index" : 0,
					"x_count" : 5,
					"y_count" : 8,
					"x_step" : 32,
					"y_step" : 32,

					"image" : "d:/ymir work/ui/public/Slot_Base.sub",
				},

				## 1st
				{
					"name" : "1st",
					"type" : "radio_button",

					"x" : BOARD_WIDTH/2 - 32,
					"y" : BOARD_HEIGHT - 57,

					#"width" : 61,
					#"height" : 21,

					"text" : "I",

					"default_image" : "d:/ymir work/ui/game/windows/tab_button_small_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/tab_button_small_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/tab_button_small_03.sub",
				},

				## 2st
				{
					"name" : "2nd",
					"type" : "radio_button",

					"x" : BOARD_WIDTH/2 + 1,
					"y" : BOARD_HEIGHT - 57,

					#"width" : 61,
					#"height" : 21,

					"text" : "II",

					"default_image" : "d:/ymir work/ui/game/windows/tab_button_small_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/tab_button_small_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/tab_button_small_03.sub",
				},

				## Ok
				{
					"name" : "OkButton",
					"type" : "button",

					"x" : 21,
					"y" : BOARD_HEIGHT - 33,

					"width" : 61,
					"height" : 21,

					"text" : uiScriptLocale.OK,

					"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
				},

				## Close
				{
					"name" : "CloseButton",
					"type" : "button",

					"x" : 104,
					"y" : BOARD_HEIGHT - 33,

					"width" : 61,
					"height" : 21,

					"text" : uiScriptLocale.CLOSE,

					"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
				},
			),
		},
	),
}
