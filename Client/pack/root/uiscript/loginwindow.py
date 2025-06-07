import uiScriptLocale
import localeInfo

ROOT_PATH = "d:/ymir work/ui/public/"
LOGIN_IMAGES_PATH = "d:/ymir work/ui/login/"
LOGIN_IMAGES_PATH_COMMON = "d:/ymir work/ui/login/"

ACCOUNT_WINDOW_WIDTH = 244
ACCOUNT_WINDOW_HEIGHT = 450

LOGIN_WINDOW_WIDTH = 276
LOGIN_WINDOW_HEIGHT = 450

CHANNEL_WINDOW_WIDTH = 244
CHANNEL_WINDOW_HEIGHT = 450

LANGUAGE_BOARD_WIDTH = 281
LANGUAGE_BOARD_HEIGHT = 50

SCALE_BY_SCREEN = (float(SCREEN_WIDTH) / 2560.0 + float(SCREEN_HEIGHT) / 1440.0) / 2


window = {
	"name" : "LoginWindow",
	"sytle" : ("movable","float",),
	"x" : 0,
	"y" : 65,
	"width" : SCREEN_WIDTH,
	"height" : SCREEN_HEIGHT,

	"children" :
	[

		## Board

		{
			"name" : "BackGround",
			"type" : "expanded_image",
			"x" : 0,
			"y" : -65,
			"x_scale" : float(SCREEN_WIDTH) / 1920.0,
			"y_scale" : float(SCREEN_HEIGHT) / 1080.0,
			"image" : "d:/ymir work/ui/login/login_screen_2.jpg",
		},
		
		{
			"name" : "AccountBoard",
			"type" : "listboxex",

			"x" : 0,
			"y" : 0,
			
			"width" : 208,
			"height" : 190,
			"itemstep" : 38,
			"viewcount" : 12,

		},
		## LoginBoard
		{
			"name" : "LoginBoard",


			"x" : (SCREEN_WIDTH - LOGIN_WINDOW_WIDTH) / 2,
			"y" : (SCREEN_HEIGHT - LOGIN_WINDOW_HEIGHT) / 2,

			"width" : LOGIN_WINDOW_WIDTH,
			"height" : LOGIN_WINDOW_HEIGHT,

			"children" :
			(
				{
					"name" : "LoginBg",
					"type" : "image",
					
					"x" : 0,
					"y" : 80,
					
					"horizontal_align" : "center",

					"image" : "d:/ymir work/ui/login/button/board.png",
				},
				{
					"name" : "ID_BACKGROUND",
					"type" : "radio_button",
					
					"x" : 0,
					"y" : 180,
					
					"horizontal_align" : "center",
					
					"default_image" : "d:/ymir work/ui/login/turkey/loginwindow/login_account_input.png",
					"over_image" : "d:/ymir work/ui/login/turkey/loginwindow/login_account_input.png",
					"down_image" : "d:/ymir work/ui/login/turkey/loginwindow/login_account_input.png",
					
					"children" :
					(
						{
							"name" : "ID_EditLine",
							"type": "editline",
							"info_msg":"Kullanýcý adý",
							"x" : 45,
							"y" : 10,
							
							"width" : 228,
							"height" : 35,
							"input_limit" : 16,
							"enable_codepage" : 0,
							
							"r" : 0.705,
							"g" : 0.494,
							"b" : 0.447,
							"a" : 1.0,
						},
					),
				},
				{
					"name" : "PW_BACKGROUND",
					"type" : "radio_button",
					
					"x" : 0,
					"y" : 225,
					
					"horizontal_align" : "center",
					
					"default_image" : "d:/ymir work/ui/login/turkey/loginwindow/login_password_input.png",
					"over_image" : "d:/ymir work/ui/login/turkey/loginwindow/login_password_input.png",
					"down_image" : "d:/ymir work/ui/login/turkey/loginwindow/login_password_input.png",
					
					"children" :
					(
						{
							"name" : "Password_EditLine",
							"type" : "editline",
							"info_msg":"Þifre",

							"x" : 45,
							"y" : 10,
							
							"width" : 228,
							"height" : 35,
							"input_limit" : 16,
							"secret_flag" : 1,
							"enable_codepage" : 0,
							
							"r" : 0.705,
							"g" : 0.494,
							"b" : 0.447,
							"a" : 1.0,
						},
					),
				},

						{
							"name" : "site",
							"type" : "button",

							"x" : -95,
							"y" : 388, 			
							"horizontal_align" : "center",

							"default_image" : "d:/ymir work/ui/login/medya/homepage.png",
							"over_image" : "d:/ymir work/ui/login/medya/homepage2.png",
							"down_image" : "d:/ymir work/ui/login/medya/homepage.png",
						},

						{
							"name" : "discord",
							"type" : "button",

							"x" : 95,
							"y" : 388,
							"horizontal_align" : "center",

							"default_image" : "d:/ymir work/ui/login/medya/discord.png",
							"over_image" : "d:/ymir work/ui/login/medya/discord2.png",
							"down_image" : "d:/ymir work/ui/login/medya/discord.png",
						},

						{
							"name" : "register",
							"type" : "button",

							"x" : 0,
							"y" : 388,
							"horizontal_align" : "center",

							"default_image" : "d:/ymir work/ui/login/medya/register.png",
							"over_image" : "d:/ymir work/ui/login/medya/register2.png",
							"down_image" : "d:/ymir work/ui/login/medya/register.png",
						},

				{
					"name" : "LoginButton",
					"type" : "button",
					
					"x" : 0,
					"y" : 300 - 20,
					
					"horizontal_align" : "center",
					
					"default_image" : "d:/ymir work/ui/login/turkey/loginwindow/login_button_down.png",
					"over_image" : "d:/ymir work/ui/login/turkey/loginwindow/login_button_hover.png",
					"down_image" : "d:/ymir work/ui/login/turkey/loginwindow/login_button_normal.png",
				},

				#exit button
				{
					"name" : "LoginExitButton",
					"type" : "button",
					
					"x" : 0,
					"y" : 360 - 15,
					
					"horizontal_align" : "center",
					
					"default_image" : "d:/ymir work/ui/login/turkey/loginwindow/exit_normal.png",
					"over_image" : "d:/ymir work/ui/login/turkey/loginwindow/exit_hover.png",
					"down_image" : "d:/ymir work/ui/login/turkey/loginwindow/exit_down.png",
				},
				{
					"name" : "ScrollBar",
					"type" : "newscrollbar",

					"x" : 210 ,
					"y" : 240,
					"size" : 118,
				},
					)
				},
			{
			"name" : "AccountBoardMain",

			"x" : (SCREEN_WIDTH - CHANNEL_WINDOW_WIDTH) / 2 + ( LOGIN_WINDOW_WIDTH / 2) + (CHANNEL_WINDOW_WIDTH / 2),
			"y" : (SCREEN_HEIGHT - CHANNEL_WINDOW_HEIGHT) / 2,
			
			"width" : ACCOUNT_WINDOW_WIDTH,
			"height" : ACCOUNT_WINDOW_HEIGHT,

			"children" :
			(
				{
					"name" : "AccountBG",
					"type" : "image",
					
					"x" : 0,
					"y" : 80,
					
					"horizontal_align" : "center",
					
					# "image" : LOGIN_IMAGES_PATH + "select_account.png",
				},
{
							"name" : "registerButton1",
							"type" : "button",

							"x" : 160 / 2,
							"y" : 135,
							"default_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_hover.png",
							"over_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_normal.png",
							"down_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_down.png",
							"fontsize" : "LARGE",

							"text" : "",
							"children":
							(
								{
									"name" : "deleteButton1",
									"type" : "button",

									"x" : 60,
									"y" : 5,

									"default_image" : "d:/ymir work/ui/login/button/delete3.png",
									"over_image" : "d:/ymir work/ui/login/button/delete2.png",
									"down_image" : "d:/ymir work/ui/login/button/delete.png",
								},
								{
									"name" : "F1Button",
									"type" : "image",
									"x" : -35,
									"y" : 0,
									"image" : "d:/ymir work/ui/login/button/f1.png",
								},
							),
						},
						
						{
							"name" : "registerButton2",
							"type" : "button",

							"x" : 160 / 2,
							"y" : 175,
							"default_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_hover.png",
							"over_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_normal.png",
							"down_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_down.png",
							"fontsize" : "LARGE",
							"text" : "",
							"children":
							(
								{
									"name" : "deleteButton2",
									"type" : "button",

									"x" : 60,
									"y" : 5,

									"default_image" : "d:/ymir work/ui/login/button/delete3.png",
									"over_image" : "d:/ymir work/ui/login/button/delete2.png",
									"down_image" : "d:/ymir work/ui/login/button/delete.png",
								},
								{
									"name" : "F2Button",
									"type" : "image",
									"x" : -35,
									"y" : 0,
									"image" : "d:/ymir work/ui/login/button/f2.png",
								},
							),
						},
						
						{
							"name" : "registerButton3",
							"type" : "button",

							"x" : 160 / 2,
							"y" : 215,
							"default_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_hover.png",
							"over_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_normal.png",
							"down_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_down.png",
							"fontsize" : "LARGE",

							"text" : "",
							"children":
							(
								{
									"name" : "deleteButton3",
									"type" : "button",

									"x" : 60,
									"y" : 5,

									"default_image" : "d:/ymir work/ui/login/button/delete3.png",
									"over_image" : "d:/ymir work/ui/login/button/delete2.png",
									"down_image" : "d:/ymir work/ui/login/button/delete.png",
								},
								{
									"name" : "F3Button",
									"type" : "image",
									"x" : -35,
									"y" : 0,
									"image" : "d:/ymir work/ui/login/button/f3.png",
								},
							),
						},
						
						{
							"name" : "registerButton4",
							"type" : "button",

							"x" : 160 / 2,
							"y" : 255,
							"default_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_hover.png",
							"over_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_normal.png",
							"down_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_down.png",
							"fontsize" : "LARGE",

							"text" : "",
							"children":
							(
								{
									"name" : "deleteButton4",
									"type" : "button",

									"x" : 60,
									"y" : 5,
									"default_image" : "d:/ymir work/ui/login/button/delete3.png",
									"over_image" : "d:/ymir work/ui/login/button/delete2.png",
									"down_image" : "d:/ymir work/ui/login/button/delete.png",

								},
								{
									"name" : "F4Button",
									"type" : "image",
									"x" : -35,
									"y" : 0,
									"image" : "d:/ymir work/ui/login/button/f4.png",
								},
							),
						},
						{
							"name" : "registerButton5",
							"type" : "button",

							"x" : 160 / 2,
							"y" : 295,
							"default_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_hover.png",
							"over_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_normal.png",
							"down_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_down.png",
							"fontsize" : "LARGE",

							"text" : "",
							"children":
							(
								{
									"name" : "deleteButton5",
									"type" : "button",

									"x" : 60,
									"y" : 5,

									"default_image" : "d:/ymir work/ui/login/button/delete3.png",
									"over_image" : "d:/ymir work/ui/login/button/delete2.png",
									"down_image" : "d:/ymir work/ui/login/button/delete.png",
								},
								{
									"name" : "F5Button",
									"type" : "image",
									"x" : -35,
									"y" : 0,
									"image" : "d:/ymir work/ui/login/button/f5.png",
								},
							),
						},
						{
							"name" : "registerButton6",
							"type" : "button",

							"x" : 160 / 2,
							"y" : 335,
							"default_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_hover.png",
							"over_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_normal.png",
							"down_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_down.png",
							"fontsize" : "LARGE",

							"text" : "",
							"children":
							(
								{
									"name" : "deleteButton6",
									"type" : "button",

									"x" : 60,
									"y" : 2,

									"default_image" : "d:/ymir work/ui/login/button/delete3.png",
									"over_image" : "d:/ymir work/ui/login/button/delete2.png",
									"down_image" : "d:/ymir work/ui/login/button/delete.png",
								},
								{
									"name" : "F6Button",
									"type" : "image",
									"x" : -35,
									"y" : 0,
									"image" : "d:/ymir work/ui/login/button/f6.png",
								},
							),
						},
			),
		},
		# ChannelWindow
		{
			"name" : "Login_back",

			"x" : (SCREEN_WIDTH - CHANNEL_WINDOW_WIDTH) / 2 - LOGIN_WINDOW_WIDTH + (( LOGIN_WINDOW_WIDTH - CHANNEL_WINDOW_WIDTH ) /2),
			"y" : (SCREEN_HEIGHT - CHANNEL_WINDOW_HEIGHT) / 2,
			
			"width" : CHANNEL_WINDOW_WIDTH,
			"height" : CHANNEL_WINDOW_HEIGHT,
			
			"children" :
			(
				{
					"name" : "ChannelBG",
					"type" : "image",
					
					"x" : 0,
					"y" : 80,
					
					"horizontal_align" : "center",
					
					# "image" : LOGIN_IMAGES_PATH + "selectchannel.png",
				},
						
						
				{
					"name" : "ChannelButton1",
					"type" : "radio_button",
					
					"x" : 80,
					"y" : 135,
					
					"default_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_hover.png",
					"over_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_normal.png",
					"down_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_down.png",
					
					"children":
					(
						{
							"name" : "ChannelButtonName",
							"type" : "text",
							"x" : 0,
							"y" : -17,
											
							"text_horizontal_align" : "center",
							"horizontal_align" : "center",
											
							"text" : "Kanal 1",
						},
						{
							"name" : "ChannelButtonTooltip1",
							"type" : "text",
							"x" : 0,
							"y" : 7,
											
							"text_horizontal_align" : "center",
							"horizontal_align" : "center",
											
							"text" : "OFF",
							"color" : 0xffb77643,
						},
					),
				},
				{
					"name" : "ChannelButton2",
					"type" : "radio_button",
					
					"x" : 80,
					"y" : 180,
					
					"default_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_hover.png",
					"over_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_normal.png",
					"down_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_down.png",
					
					
					"children":
					(
						{
							"name" : "ChannelButtonName",
							"type" : "text",
							"x" : 0,
							"y" : -15,
											
							"text_horizontal_align" : "center",
							"horizontal_align" : "center",
											
							"text" : "Kanal 2",
						},
						{
							"name" : "ChannelButtonTooltip2",
							"type" : "text",
							"x" : 0,
							"y" : 7,
											
							"text_horizontal_align" : "center",
							"horizontal_align" : "center",
											
							"text" : "OFF",
							"color" : 0xffb77643,
						},
					),
				},
				{
					"name" : "ChannelButton3",
					"type" : "radio_button",
					
					"x" : 80,
					"y" : 225,
					
					"default_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_hover.png",
					"over_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_normal.png",
					"down_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_down.png",
					
					"children":
					(
						{
							"name" : "ChannelButtonName",
							"type" : "text",
							"x" : 0,
							"y" : -15,
											
							"text_horizontal_align" : "center",
							"horizontal_align" : "center",
											
							"text" : "Kanal 3",
						},
						{
							"name" : "ChannelButtonTooltip3",
							"type" : "text",
							"x" : 0,
							"y" : 7,
											
							"text_horizontal_align" : "center",
							"horizontal_align" : "center",
											
							"text" : "OFF",
							"color" : 0xffb77643,
						},
					),
				},
				{
					"name" : "ChannelButton4",
					"type" : "radio_button",
					
					"x" : 80,
					"y" : 270,
					
					"default_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_hover.png",
					"over_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_normal.png",
					"down_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_down.png",
					
					
					"children":
					(
						{
							"name" : "ChannelButtonName",
							"type" : "text",
							"x" : 0,
							"y" : -15,
											
							"text_horizontal_align" : "center",
							"horizontal_align" : "center",
											
							"text" : "Kanal 4",
						},
						{
							"name" : "ChannelButtonTooltip4",
							"type" : "text",
							"x" : 0,
							"y" : 7,
											
							"text_horizontal_align" : "center",
							"horizontal_align" : "center",
											
							"text" : "OFF",
							"color" : 0xffb77643,
						},
					),
				},
{
					"name" : "ChannelButton5",
					"type" : "radio_button",
					
					"x" : 80,
					"y" : 315,
					
					"default_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_hover.png",
					"over_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_normal.png",
					"down_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_down.png",
					
					
					"children":
					(
						{
							"name" : "ChannelButtonName",
							"type" : "text",
							"x" : 0,
							"y" : -15,
											
							"text_horizontal_align" : "center",
							"horizontal_align" : "center",
											
							"text" : "Kanal 5",
						},
						{
							"name" : "ChannelButtonTooltip5",
							"type" : "text",
							"x" : 0,
							"y" : 7,
											
							"text_horizontal_align" : "center",
							"horizontal_align" : "center",
											
							"text" : "OFF",
							"color" : 0xffb77643,
						},
					),
				},
{
					"name" : "ChannelButton6",
					"type" : "radio_button",
					
					"x" : 80,
					"y" : 360,
					
					"default_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_hover.png",
					"over_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_normal.png",
					"down_image" : "d:/ymir work/ui/login/turkey/loginwindow/account_save_down.png",
					
					"children":
					(
						{
							"name" : "ChannelButtonName",
							"type" : "text",
							"x" : 0,
							"y" : -15,
											
							"text_horizontal_align" : "center",
							"horizontal_align" : "center",
											
							"text" : "Kanal 6",
						},
						{
							"name" : "ChannelButtonTooltip6",
							"type" : "text",
							"x" : 0,
							"y" : 7,
											
							"text_horizontal_align" : "center",
							"horizontal_align" : "center",
											
							"text" : "OFF",
							"color" : 0xffb77643,
						},
					),
				},
						{
							"name" : "ServerList",
							"type" : "listbox",

							"x" : 30,
							"y" : 210,
							#"width" : 100,
							#"height" : SERVER_LIST_HEIGHT,
							"row_count" : 15,
							"item_align" : 0,
						},
					),
				},
	],
}