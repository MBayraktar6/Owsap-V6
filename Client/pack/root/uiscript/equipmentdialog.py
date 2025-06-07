import app

if app.ENABLE_ADDITIONAL_EQUIPMENT_PAGE:
	import uiScriptLocale
	import item
	BOARD_WIDTH		= 180
	BOARD_HEIGHT	= 290

	window = {
		"name" : "EquipmentDialog",
		"style" : ("movable", "float",),
		
		"x" : SCREEN_WIDTH - 180,
		"y" : 0,

		"width" : BOARD_WIDTH,
		"height" : BOARD_HEIGHT,

		"children" :
		(
			{
				"name" : "board",
				"type" : "board_with_titlebar",

				"x" : 0,
				"y" : 0,

				"width" : BOARD_WIDTH,
				"height" : BOARD_HEIGHT,

				"title" : "Character Name",

				"children" :
				(
					## base_tab
					{
						"name" : "base_tab",
						"type" : "image",

						"x" : 12,
						"y" : 33,

						"width" : BOARD_WIDTH,
						"height" : 25,

						"image" : "d:/ymir work/ui/equipment_bg_without_ring_tab00.tga",
					},
					## Tab Area
					{
						"name" : "additional_tab",
						"type" : "window",

						"x" : 12,
						"y" : 33,

						"width" : BOARD_WIDTH,
						"height" : 26,

						"children" :
						[
							{
								"name" : "tab_img_01",
								"type" : "image",

								"x" : 0,
								"y" : 0,

								"width" : BOARD_WIDTH,
								"height" : 25,

								"image" : "d:/ymir work/ui/equipment_bg_without_ring_tab01.tga",
							},
							{
								"name" : "tab_img_02",
								"type" : "image",

								"x" : 0,
								"y" : 0,

								"width" : BOARD_WIDTH,
								"height" : 25,

								"image" : "d:/ymir work/ui/equipment_bg_without_ring_tab02.tga",
							},
							{
								"name" : "tab_btn_01",
								"type" : "button",

								"x" : 0,
								"y" : 0,

								"width" : 39,
								"height" : 25,
								"tooltip_text" : uiScriptLocale.EQUIPMENT_SLOT_1,
							},
							{
								"name" : "tab_btn_02",
								"type" : "button",

								"x" : 39,
								"y" : 0,

								"width" : 39,
								"height" : 25,
								"tooltip_text" : uiScriptLocale.EQUIPMENT_SLOT_2,
							},
						],
					},
					## equip area
					{
						"name" : "equipment_window",
						"type" : "window",

						"x" : 0,
						"y" : 56,

						"width" : BOARD_WIDTH,
						"height" : BOARD_HEIGHT,

						"children" :
						[							
							## equipment slots
							{
								"name":"equipment_base_image",
								"type":"image",
								"style" : ("attach",),

								"x" : 0,
								"y" : 0,
								"horizontal_align" : "center",
								"image" : "d:/ymir work/ui/equipment_bg_without_ring.tga",

								"children" :
								(
									{
										"name" : "equipment_slot",
										"type" : "slot",

										"x" : 3,
										"y" : 3,

										"width" : 150,
										"height" : 140,

										"slot" : 
										(
											## ARMOR_BODY
											{"index":item.EQUIPMENT_BODY, "x":39, "y":37, "width":32, "height":64},
											## ARMOR_HEAD
											{"index":item.EQUIPMENT_HEAD, "x":39, "y":2, "width":32, "height":32},
											## ARMOR_FOOTS
											{"index":item.EQUIPMENT_SHOES, "x":4, "y":99, "width":32, "height":32},
											## ARMOR_WRIST
											{"index":item.EQUIPMENT_WRIST, "x":75, "y":67, "width":32, "height":32},
											## ITEM_WEAPON
											{"index":item.EQUIPMENT_WEAPON, "x":3, "y":3, "width":32, "height":96},
											## ARMOR_NECK
											{"index":item.EQUIPMENT_NECK, "x":114, "y":67, "width":32, "height":32},
											## ARMOR_EAR
											{"index":item.EQUIPMENT_EAR, "x":114, "y":35, "width":32, "height":32},
											## WEAPON_QUIVER
											{"index":item.EQUIPMENT_ARROW, "x":114, "y":2, "width":32, "height":32},
											## ARMOR_SHIELD
											{"index":item.EQUIPMENT_SHIELD, "x":75, "y":35, "width":32, "height":32},
											## ITEM_BELT
											{"index":item.EQUIPMENT_BELT, "x":39, "y":99, "width":32, "height":32},
											## ARMOR_PENDANT
											{"index":item.EQUIPMENT_PENDANT, "x":75, "y":2, "width":32, "height":32},
											## ARMOR_GLOVE
											{"index":item.EQUIPMENT_GLOVE, "x":75, "y":99, "width":32, "height":32},
										),
									},
									{
										"name" : "unique_slot",
										"type" : "slot",

										"x" : 3,
										"y" : 140,

										"width" : 150,
										"height" : 36,

										"slot" : 
										(
											## ITEM_UNIQUE_LEFT
											{"index":item.EQUIPMENT_UNIQUE1, "x":2, "y":7, "width":32, "height":32},
											## ITEM_UNIQUE_RIGHT
											{"index":item.EQUIPMENT_UNIQUE2, "x":39, "y":7, "width":32, "height":32},
										),
									},
								),
							},
							## dragon_soul_button
							{
								"name" : "dragon_soul_button",
								"type" : "button",

								"x" : 18,
								"y" : 186,

								"tooltip_text" : uiScriptLocale.TASKBAR_DRAGON_SOUL,

								"default_image" : "d:/ymir work/ui/dragonsoul/DragonSoul_Button_01.tga",
								"over_image" : "d:/ymir work/ui/dragonsoul/DragonSoul_Button_02.tga",
								"down_image" : "d:/ymir work/ui/dragonsoul/DragonSoul_Button_03.tga",
							},
							## mall_button
							{
								"name" : "mall_button",
								"type" : "button",

								"x" : 53,
								"y" : 186,

								"tooltip_text" : uiScriptLocale.MALL_TITLE,
						
								"default_image" : "d:/ymir work/ui/game/taskbar/Mall_Button_01.tga",
								"over_image" : "d:/ymir work/ui/game/taskbar/Mall_Button_02.tga",
								"down_image" : "d:/ymir work/ui/game/taskbar/Mall_Button_03.tga",
							},
							## premium_private_shop_button
							{
								"name" : "premium_private_shop_button",
								"type" : "button",

								"x" : 91,
								"y" : 186,

								"tooltip_text" : uiScriptLocale.PREMIUM_PRIVATE_SHOP,
						
								"default_image" : "icon/item/private_button_01.tga",
								"over_image" : "icon/item/private_button_02.tga",
								"down_image" : "icon/item/private_button_03.tga",
							},
							## costume_button
							{
								"name" : "costume_button",
								"type" : "button",

								"x" : 132,
								"y" : 186,

								"tooltip_text" : uiScriptLocale.COSTUME_TITLE,

								"default_image" : "d:/ymir work/ui/game/taskbar/costume_Button_01.tga",
								"over_image" : "d:/ymir work/ui/game/taskbar/costume_Button_02.tga",
								"down_image" : "d:/ymir work/ui/game/taskbar/costume_Button_03.tga",
							},
						],
					},
				),
			},
		),
	}
else:
	if app.ENABLE_SET_ITEM:
		window = {
			"name" : "EquipmentDialog",
			"style" : ("movable", "float",),

			"x" : 0,
			"y" : 0,

			"width" : 180,
			"height" : 230,

			"children" :
			(
				{
					"name" : "Board",
					"type" : "board_with_titlebar",

					"x" : 0,
					"y" : 0,

					"width" : 180,
					"height" : 230,

					"title" : "Character Name",

					"children" :
					(
						{
							"name":"EquipmentBaseImage",
							"type":"image",
							"style" : ("attach",),

							"x" : 0,
							"y" : 9,
							"horizontal_align" : "center",
							"vertical_align" : "center",

							"image" : "d:/ymir work/ui/equipment_bg_without_ring.tga",

							"children" :
							(

								{
									"name" : "EquipmentSlot",
									"type" : "slot",

									"x" : 3,
									"y" : 3,

									"width" : 150,
									"height" : 182,

									"slot" : (
												{"index":0, "x":39, "y":37, "width":32, "height":64},
												{"index":1, "x":39, "y":2, "width":32, "height":32},
												{"index":2, "x":39, "y":145, "width":32, "height":32},
												{"index":3, "x":75, "y":67, "width":32, "height":32},
												{"index":4, "x":3, "y":3, "width":32, "height":96},
												{"index":5, "x":114, "y":67, "width":32, "height":32},
												{"index":6, "x":114, "y":35, "width":32, "height":32},
												{"index":7, "x":2, "y":145, "width":32, "height":32},
												{"index":8, "x":75, "y":145, "width":32, "height":32},
												{"index":9, "x":114, "y":2, "width":32, "height":32},
												{"index":10, "x":75, "y":35, "width":32, "height":32},
												## 새 반지1
												##{"index":21, "x":2, "y":106, "width":32, "height":32},
												## 새 반지2
												##{"index":22, "x":75, "y":106, "width":32, "height":32},
												## 새 벨트
												{"index":23, "x":39, "y":106, "width":32, "height":32},
												## 팬던트
												{"index":27, "x":3, "y":106, "width":32, "height":32},
												## 장갑
												{"index":29, "x":75, "y":106, "width":32, "height":32},
											),
								},

							),

						},
					),
				},
			),
		}
	else:
		window = {
			"name" : "EquipmentDialog",
			"style" : ("movable", "float",),

			"x" : 0,
			"y" : 0,

			"width" : 180,
			"height" : 230,

			"children" :
			(
				{
					"name" : "Board",
					"type" : "board_with_titlebar",

					"x" : 0,
					"y" : 0,

					"width" : 180,
					"height" : 230,

					"title" : "Character Name",

					"children" :
					(
						{
							"name":"EquipmentBaseImage",
							"type":"image",
							"style" : ("attach",),

							"x" : 0,
							"y" : 9,
							"horizontal_align" : "center",
							"vertical_align" : "center",

							"image" : "d:/ymir work/ui/equipment_bg_without_ring.tga",

							"children" :
							(

								{
									"name" : "EquipmentSlot",
									"type" : "slot",

									"x" : 3,
									"y" : 3,

									"width" : 150,
									"height" : 182,

									"slot" : (
												{"index":0, "x":39, "y":37, "width":32, "height":64},
												{"index":1, "x":39, "y":2, "width":32, "height":32},
												{"index":2, "x":39, "y":145, "width":32, "height":32},
												{"index":3, "x":75, "y":67, "width":32, "height":32},
												{"index":4, "x":3, "y":3, "width":32, "height":96},
												{"index":5, "x":114, "y":67, "width":32, "height":32},
												{"index":6, "x":114, "y":35, "width":32, "height":32},
												{"index":7, "x":2, "y":145, "width":32, "height":32},
												{"index":8, "x":75, "y":145, "width":32, "height":32},
												{"index":9, "x":114, "y":2, "width":32, "height":32},
												{"index":10, "x":75, "y":35, "width":32, "height":32},
												## 새 반지1
												##{"index":21, "x":2, "y":106, "width":32, "height":32},
												## 새 반지2
												##{"index":22, "x":75, "y":106, "width":32, "height":32},
												## 새 벨트
												{"index":23, "x":39, "y":106, "width":32, "height":32},
												## 장갑
												{"index":29, "x":75, "y":106, "width":32, "height":32},
											),
								},

							),

						},
					),
				},
			),
		}