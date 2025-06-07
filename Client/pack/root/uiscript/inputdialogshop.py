import uiScriptLocale
ROOT_PATH = "d:/ymir work/ui/public/"
window = {
	"name" : "InputDialogShop",
	"x" : 0,
	"y" : 0,
	"style" : ("movable", "float",),
	"width" : 170,
	"height" : 90 + 30,
	"children" :
	(
		{
			"name" : "Board",
			"type" : "board_with_titlebar",
			"x" : 0,
			"y" : 0,
			"width" : 170,
			"height" : 90 + 30,
			"title" : "",
			"children" :
			(
				## Input Slot
				{
					"name" : "InputSlot",
					"type" : "slotbar",
					"x" : 0,
					"y" : 34,
					"width" : 90,
					"height" : 18,
					"horizontal_align" : "center",
					"children" :
					(
						{
							"name" : "InputValue",
							"type" : "editline",
							"x" : 3,
							"y" : 3,
							"width" : 90,
							"height" : 18,
							"input_limit" : 12,
						},
					),
				},
				{
					"name" : "InputTimeText",
					"type" : "text",
					"x" : 15,
					"y" : 63,
					"text" : uiScriptLocale.OFFLINE_SHOP_STYLE,
				},
				{
					"name" : "YellowBar",
					"type" : "image",
					"x" : 6,
					"y" : 60,
					"image" : "locale/tr/offlineshop/yellow_bar.tga",
				},
				{
					"name" : "InputStyle0",
					"type" : "radio_button",
					"x" : 55,
					"y" : 60,
					"tooltip_text" : uiScriptLocale.OFFLINE_SHOP_STYLE_0,
					"default_image" : "locale/tr/offlineshop/default_image.tga",
					"over_image" : "locale/tr/offlineshop/over_image.tga",
					"down_image" : "locale/tr/offlineshop/down_image.tga",
				},
				## Button
				{
					"name" : "AcceptButton",
					"type" : "button",
					"x" : - 61 - 5 + 30,
					"y" : 58 + 30,
					"horizontal_align" : "center",
					"text" : uiScriptLocale.OK,
					"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
				},
				{
					"name" : "CancelButton",
					"type" : "button",
					"x" : 5 + 30,
					"y" : 58 + 30,
					"horizontal_align" : "center",
					"text" : uiScriptLocale.CANCEL,
					"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
				},
			),
		},
	),
}
