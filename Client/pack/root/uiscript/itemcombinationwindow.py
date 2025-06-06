import uiScriptLocale
import item
import app

window = {
	"name" : "Item_Combination_Window",

	"x" : 0,
	"y" : 0,

	"style" : ("movable", "float",),

	"width" : 205,
	"height" : 380,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : 205,
			"height" : 380,
		
			"children" :
			(
				## Title
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 6,
					"y" : 6,

					"width" : 190,
					"color" : "yellow",

					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":95, "y":3, "text" : uiScriptLocale.COMB_TITLE, "text_horizontal_align":"center" },
					),
				},
				
				## Background Image
				{
					"name" : "Item_Comb_Background_Image",
					"type" : "image",
					
					"x" : 8,
					"y" : 30,
					
					"image" : uiScriptLocale.LOCALE_COMMON_UISCRIPT_PATH + "combination/comb1.tga",
				},
				
				## Slot
				{
					"name" : "MediumSlot",
					"type" : "slot",
					
					"image" : "d:/ymir work/ui/public/slot_base.sub",
			
					"x" : 8,
					"y" : 30,
			
					"width" : 190,
					"height" : 40,
			
					"slot" : (
						{"index":0, "x":78, "y":12, "width":31, "height":31},  # 스크롤
					),
				},
				
				{
					"name" : "CombSlot", # 스크롤을 장착했을 경우에만 아래 슬롯이 보이도록 하기위해서 Scroll 슬롯과 따로 작성하였다.
					"type" : "slot",
			
					"x" : 8,
					"y" : 90,
			
					"width" : 190,
					"height" : 300,
			
					"slot" : (
						{"index":1, "x":28, "y":8, "width":31, "height":96},  # 메인코스튬
						{"index":2, "x":129, "y":8, "width":31, "height":96}, # 재료코스튬
						{"index":3, "x":79, "y":125, "width":31, "height":96}, # 결과코스튬
					),
					
					"children" :
					(
						{ "name":"Slot1_Name", "type":"text", "x":43, "y":110, "text" : uiScriptLocale.COMB_APPEARANCE, "text_horizontal_align":"center" },
						{ "name":"Slot2_Name", "type":"text", "x":143, "y":110, "text" : uiScriptLocale.COMB_ATTRIBUTE, "text_horizontal_align":"center" },
					),
				},
				
				## Button
				{
					"name" : "AcceptButton",
					"type" : "button",

					"x" : 40,
					"y" : 345,

					"text" : uiScriptLocale.OK,

					"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
				},
				{
					"name" : "CancelButton",
					"type" : "button",

					"x" : 114,
					"y" : 345,

					"text" : uiScriptLocale.CANCEL,

					"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
				},				
			),
		},
	),
}

