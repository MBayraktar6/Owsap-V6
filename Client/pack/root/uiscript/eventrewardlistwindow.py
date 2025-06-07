import uiScriptLocale
import localeInfo
import app

if app.ENABLE_EVENT_BANNER_REWARD_LIST_RENEWAL:
	WINDOW_X						= 0
	WINDOW_Y						= 65
	WINDOW_WIDTH					= 290
	WINDOW_HEIGHT					= 40
	
	BACKGROUND_IMAGE_X				= 15
	BACKGROUND_IMAGE_Y				= 0
	
	TITLE_BUTTON_X					= 19
	TITLE_BUTTON_Y					= 3
	
	REWARD_LIST_BUTTON_X			= 202
	REWARD_LIST_BUTTON_Y			= 2
	REWARD_LIST_BUTTON_WIDTH		= 102
	REWARD_LIST_BUTTON_HEIGHT		= 40
	
	EVENT_DURATION_WINDOW_X			= 19
	EVENT_DURATION_WINDOW_Y			= 25
	EVENT_DURATION_WINDOW_WIDTH		= 174
	EVENT_DURATION_WINDOW_HEIGHT	= 16
	
	window = {
		"name" : "EventRewardListWindow",
		"type" : "window",
	
		"x" : WINDOW_X,
		"y" : WINDOW_Y,
		
		"width" : WINDOW_WIDTH,
		"height" : WINDOW_HEIGHT,
	
		"children" :
		[
			## background image
			{ 
				"name" : "back_board_img", 
				"type" : "expanded_image", 
				"style" : ("attach",), 
	
				"x" : BACKGROUND_IMAGE_X,
				"y" : BACKGROUND_IMAGE_Y, 
	
				"image" : "d:/ymir work/ui/event/bg_list_tab.sub" 
			},
			
			## title button
			{
				"name" : "title_button",
				"type" : "button", 
	
				"x" : TITLE_BUTTON_X,
				"y" : TITLE_BUTTON_Y, 
	
				"text" : " ",
				"default_image" : "d:/ymir work/ui/event/long_button_01.sub",
				"over_image" : "d:/ymir work/ui/event/long_button_02.sub",
				"down_image" : "d:/ymir work/ui/event/long_button_03.sub",
			},
			
			## reward list button
			{
				"name" : "reward_list_button",
				"type" : "button", 
	
				"x" : REWARD_LIST_BUTTON_X,
				"y" : REWARD_LIST_BUTTON_Y, 
	
				"width" : REWARD_LIST_BUTTON_WIDTH,
				"height" : REWARD_LIST_BUTTON_HEIGHT,
	
				"text" : uiScriptLocale.EVENT_REWARD_CHECK,
				"all_align" : "center",
				"default_image" : "d:/ymir work/ui/event/reward_list_button_default.sub",
				"over_image" : "d:/ymir work/ui/event/reward_list_button_over.sub",
				"down_image" : "d:/ymir work/ui/event/reward_list_button_down.sub",
			},
			
			## event duration
			{
				"name" : "event_duration_window", 
				"type" : "window", 
				
				"x" : EVENT_DURATION_WINDOW_X, 
				"y" : EVENT_DURATION_WINDOW_Y, 
				
				"width" : EVENT_DURATION_WINDOW_WIDTH, 
				"height" : EVENT_DURATION_WINDOW_HEIGHT, 
				
				"style" : ("attach",),
				
				"children" :
				(
					{"name":"EventDuration", "type":"text", "x":0, "y":0, "text": " ", "r" : 0.58, "g" : 0.56, "b" : 0.51, "a" : 1.0},
				),
			},
		],
	}
else:
	IN_GAME_UI_WIDTH	= 290
	REWARD_UI_HEIGHT	= 40
	
	SLOT_WIDTH = 32
	SLOT_HEIGHT = 32
	
	window = {
		"name" : "EventRewardListWindow",
		"type" : "window",
	
		"x" : 0,
		"y" : 65,
		
		"width" : IN_GAME_UI_WIDTH,
		"height" : REWARD_UI_HEIGHT,
	
		"children" :
		[
			{ 
				"name" : "back_board_img", 
				"type" : "expanded_image", 
				"style" : ("attach",), 
				"x" : 15,
				"y" : 0, 
				"image" : "d:/ymir work/ui/event/bg_list_tab.sub" 
			},
		
			{
				"name" : "EventButton",
				"type" : "button", 
				"x" : 19,
				"y" : 3, 
				"text" : " ",
				"default_image" : "d:/ymir work/ui/event/long_button_01.sub",
				"over_image" : "d:/ymir work/ui/event/long_button_02.sub",
				"down_image" : "d:/ymir work/ui/event/long_button_03.sub",
			},
			{
				"name" : "reward_item_slot",
				"type" : "slot",
				
				"x" : 202,
				"y" : 7,
				
				"width" : SLOT_WIDTH*3,
				"height" : SLOT_HEIGHT,
				
				"image" : "d:/ymir work/ui/public/Slot_Base.sub",
				
				"slot" : 
				(
					{"index":0, "x":0, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
					{"index":1, "x":SLOT_WIDTH * 1, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
					{"index":2, "x":SLOT_WIDTH * 2, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
				),
			},
			{
				"name" : "event_duration_window", 
				"type" : "window", 
				
				"x" : 19, 
				"y" : 25, 
				
				"width" : 174, 
				"height" : 16, 
				
				"style" : ("attach",),
				
				"children" :
				(
					{"name":"EventDuration", "type":"text", "x":0, "y":0, "text": " ", "r" : 0.58, "g" : 0.56, "b" : 0.51, "a" : 1.0},
				),
			},
		],
	}