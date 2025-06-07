import uiScriptLocale
import localeInfo
import app

ROOT = "d:/ymir work/ui/game/"

BOARD_WIDTH = 290
BOARD_HEIGHT = 189

window = {
	"name" : "ExchangeDialog",

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
 
					"width" : 276,
					"color" : "gray",

					"children" :
					(
						{ "name" : "TitleName", "type" : "text", "x" : 133, "y" : 3, "text" : uiScriptLocale.EXCHANGE_TITLE, "text_horizontal_align" : "center" },
					),
				},

				## MiddleBar
				{
					"name" : "Middle_Bar",
					"type" : "expanded_image",

					"x" : (BOARD_WIDTH / 2) - 3,
					"y" : 31,
					"x_scale" : 1.0, 
					"y_scale" : 1.16,
					"image" : ROOT + "windows/middlebar.sub",
				},

				## Owner
				{
					"name" : "Owner",
					"type" : "window",

					"x" : 150,
					"y" : 33,

					"width" : 130,
					"height" : 146,

					"children" :
					(
						{
							"name" : "Owner_Slot",
							"type" : "grid_table",

							"start_index" : 0,

							"x" : 0,
							"y" : 0,

							"x_count" : 4,
							"y_count" : 3,
							"x_step" : 32,
							"y_step" : 32,
							"x_blank" : 0,
							"y_blank" : 0,

							"image" : "d:/ymir work/ui/public/slot_base.sub",
						},
						{
							"name" : "Cheque_Icon",
							"type" : "image",

							"x" : 0,
							"y" : 102,

							"image" : "d:/ymir work/ui/game/windows/cheque_icon.sub",
						},
						{
							"name" : "Owner_Cheque",
							"type" : "button",

							"x" : 20,
							"y" : 102,

							"default_image" : "d:/ymir work/ui/public/parameter_slot_02.sub",
							"over_image" : "d:/ymir work/ui/public/parameter_slot_02.sub",
							"down_image" : "d:/ymir work/ui/public/parameter_slot_02.sub",

							"children" :
							(
								{
									"name" : "Owner_Cheque_Value",
									"type" : "text",

									"x" : 62,
									"y" : 3,

									"text" : "1234567",

									"text_horizontal_align" : "right",
								},
							),
						},
						{
							"name" : "Money_Icon",
							"type" : "image",

							"x" : 0,
							"y" : 124,

							"image" : "d:/ymir work/ui/game/windows/money_icon.sub",
						},
						{
							"name" : "Owner_Money",
							"type" : "button",

							"x" : 20,
							"y" : 124,

							"default_image" : "d:/ymir work/ui/public/parameter_slot_02.sub",
							"over_image" : "d:/ymir work/ui/public/parameter_slot_02.sub",
							"down_image" : "d:/ymir work/ui/public/parameter_slot_02.sub",

							"children" :
							(
								{
									"name" : "Owner_Money_Value",
									"type" : "text",

									"x" : 62,
									"y" : 3,

									"text" : "1234567",

									"text_horizontal_align" : "right",
								},
							),
						},
						{
							"name" : "Owner_Accept_Light",
							"type" : "button",

							"x" : 100,
							"y" : 102,

							"default_image" : "d:/ymir work/ui/game/windows/accept_button_off.sub",
							"over_image" : "d:/ymir work/ui/game/windows/accept_button_off.sub",
							"down_image" : "d:/ymir work/ui/game/windows/accept_button_on.sub",
						},
						{
							"name" : "Owner_Accept_Button",
							"type" : "toggle_button",

							"x" : 90,
							"y" : 124,

							"text" : uiScriptLocale.EXCHANGE_ACCEPT,

							"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
						},
					),
				},

				## Target
				{
					"name" : "Target",
					"type" : "window",

					"x" : 10,
					"y" : 33,

					"width" : 130,
					"height" : 130,

					"children" :
					(
						{
							"name" : "Target_Slot",
							"type" : "grid_table",

							"start_index" : 0,

							"x" : 0,
							"y" : 0,

							"x_count" : 4,
							"y_count" : 3,
							"x_step" : 32,
							"y_step" : 32,
							"x_blank" : 0,
							"y_blank" : 0,

							"image" : "d:/ymir work/ui/public/slot_base.sub",
						},
						{
							"name" : "Cheque_Icon",
							"type" : "image",

							"x" : 0,
							"y" : 102,

							"image" : "d:/ymir work/ui/game/windows/cheque_icon.sub",
						},
						{
							"name" : "Target_Cheque",
							"type" : "image",

							"x" : 20,
							"y" : 102,

							"image" : "d:/ymir work/ui/public/parameter_slot_02.sub",

							"children" :
							(
								{
									"name" : "Target_Cheque_Value",
									"type" : "text",

									"x" : 62,
									"y" : 3,

									"text" : "1234567",

									"text_horizontal_align" : "right",
								},
							),
						},
						{
							"name" : "Money_Icon",
							"type" : "image",

							"x" : 0,
							"y" : 124,

							"image" : "d:/ymir work/ui/game/windows/money_icon.sub",
						},
						{
							"name" : "Target_Money",
							"type" : "image",

							"x" : 20,
							"y" : 124,

							"image" : "d:/ymir work/ui/public/parameter_slot_02.sub",

							"children" :
							(
								{
									"name" : "Target_Money_Value",
									"type" : "text",

									"x" : 62,
									"y" : 3,

									"text" : "1234567",

									"text_horizontal_align" : "right",
								},
							),
						},
						{
							"name" : "Target_Accept_Light",
							"type" : "button",

							"x" : 100,
							"y" : 102,

							"default_image" : "d:/ymir work/ui/game/windows/accept_button_off.sub",
							"over_image" : "d:/ymir work/ui/game/windows/accept_button_off.sub",
							"down_image" : "d:/ymir work/ui/game/windows/accept_button_on.sub",
						},
					),
				},
			),
		},
	),
}
