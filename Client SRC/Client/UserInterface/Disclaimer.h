#pragma once

// Attribution Disclaimer for Uncredited Features and Systems

/*
* Some features and systems in this project may not have an identified author or contributor.
* This could be due to various reasons, such as the feature being a quick and simple addition,
* or the use of a collection of features without individual credits.
*
* If you believe you are the author of an uncredited feature used in this project,
* please contact admin@owsap.dev to rectify the situation and ensure proper attribution.
*
* Our aim is to maintain transparency and acknowledge the contributions of all developers.
*
* ENABLE_GEM_SYSTEM .......................... Owsap
* ENABLE_MINI_GAME_RUMI ...................... Owsap
* ENABLE_MINI_GAME_CATCH_KING ................ ZeNu
* ENABLE_DS_CHANGE_ATTR ...................... Owsap
* ENABLE_MOVE_COSTUME_ATTR ................... blackdragonx61 / Mali
* ENABLE_EXTEND_INVEN_SYSTEM ................. Owsap
* ENABLE_ADDITIONAL_EQUIPMENT_PAGE ........... Owsap
* ENABLE_MESSENGER_DETAILS ................... Owsap
* ENABLE_SHOPEX_RENEWAL ...................... blackdragonx61 / Mali, Owsap
* ENABLE_SHOPEX_TAB4 ......................... blackdragonx61 / Mali
* ENABLE_MYSHOP_DECO ......................... blackdragonx61 / Mali, Owsap
* ENABLE_PRIVATESHOP_SEARCH_SYSTEM ........... blackdragonx61 / Mali
* ENABLE_IMPROVED_LOGOUT_POINTS .............. blackdragonx61 / Mali
* ENABLE_SKILL_COOLTIME_UPDATE ............... blackdragonx61 / Mali
* ENABLE_EXPRESSING_EMOTION .................. Owsap
* ENABLE_CONQUEROR_LEVEL ..................... Owsap
* ENABLE_SKILLBOOK_COMB_SYSTEM ............... Owsap
* ENABLE_ELEMENT_ADD ......................... Owsap
* ENABLE_CHANGE_LOOK_SYSTEM .................. blackdragonx61 / Mali
* WJ_SHOW_PARTY_ON_MINIMAP ................... blackdragonx61 / Mali, Owsap
* ENABLE_EVENT_BANNER_FLAG ................... Owsap
* ENABLE_MAILBOX ............................. blackdragonx61 / Mali
* ENABLE_RANKING_SYSTEM ...................... Owsap
* ENABLE_LOCALE_CLIENT ....................... blackdragonx61 / Mali, Owsap
* ENABLE_LOOTING_SYSTEM ...................... blackdragonx61 / Mali, Owsap
* ENABLE_POPUP_NOTICE ........................ Owsap
* ENABLE_MULTI_LANGUAGE_SYSTEM ............... Owsap
* ENABLE_FISHING_GAME ........................ Owsap
* ENABLE_CLIENT_TIMER ........................ Owsap
* ENABLE_DEFENSE_WAVE ........................ Owsap
* ENABLE_ITEM_DROP_RENEWAL ................... xP3NG3Rx
* ENABLE_SOUL_SYSTEM ......................... Owsap
* ENABLE_ITEM_VALUE10 ........................ Owsap
* ENABLE_APPLY_RANDOM ........................ Owsap
* ENABLE_CHANGED_ATTR ........................ blackdragonx61 / Mali
* ENABLE_LUCKY_BOX ........................... blackdragonx61 / Mali
* ENABLE_ATTR_6TH_7TH ........................ Owsap
* ENABLE_SET_ITEM ............................ Owsap
* ENABLE_NEW_DROP_DIALOG ..................... Owsap
* WJ_ENABLE_PICKUP_ITEM_EFFECT ............... blackdragonx61 / Mali, Owsap
* WJ_ENABLE_TRADABLE_ICON .................... xP3NG3Rx
* WJ_SHOW_MOB_INFO ........................... xP3NG3Rx
* WJ_MULTI_TEXTLINE .......................... blackdragonx61 / Mali, Owsap
* ENABLE_TEXT_IMAGE_LINE ..................... xP3NG3Rx
* ENABLE_HIGH_LIGHT_IMAGE .................... xP3NG3Rx
* ENABLE_SLOT_SCALE .......................... blackdragonx61 / Mali
* ENABLE_SLOT_COOLTIME_REFRESH_FIX ........... blackdragonx61 / Mali
* ENABLE_CLIP_MASK ........................... blackdragonx61 / Mali
* ENABLE_MOUSE_WHEEL_TOP_WINDOW .............. blackdragonx61 / Mali
* ENABLE_ATLAS_MARK_INFO ..................... xP3NG3Rx
* ENABLE_QUEST_RENEWAL ....................... Owsap
* ENABLE_CUBE_RENEWAL ........................ Owsap
* ENABLE_KEYCHANGE_SYSTEM .................... Owsap
* ENABLE_DETAILS_UI .......................... Owsap
* ENABLE_CHATTING_WINDOW_RENEWAL ............. Owsap
* ENABLE_OX_RENEWAL .......................... Owsap
* ENABLE_HELP_RENEWAL ........................ Owsap
* ENABLE_QUEST_WIDTH_EXPANSION ............... Owsap
* ENABLE_MINIMAP_TEXTURE_PATTERN ............. Owsap
* ENABLE_HIDE_ATTACHING_EFFECT_ON_ACTING ..... Owsap
* ENABLE_LOADING_TIP ......................... blackdragonx61 / Mali
* ENABLE_TAB_NEXT_TARGET ..................... Vegas
* ENABLE_FOG_FIX ............................. Owsap
* ENABLE_ENVIRONMENT_EFFECT_OPTION ........... blackdragonx61 / Mali
* ENABLE_FOV_OPTION .......................... Owsap
* ENABLE_DISABLE_SOFTWARE_TILING ............. Owsap
* ENABLE_GRAPHIC_ON_OFF ...................... blackdragonx61 / Mali
* ENABLE_SHADOW_RENDER_QUALITY_OPTION ........ blackdragonx61 / Mali
* ENABLE_METINSTONE_SWAP ..................... blackdragonx61 / Mali
* ENABLE_OPTIMIZATION ........................ blackdragonx61 / Mali
* ENABLE_ETER_PACK_OBSCURING ................. Owsap
* ENABLE_CEF_BROWSER ......................... Owsap
* ENABLE_CHAR_CONFIG ......................... blackdragonx61 / Mali
* ENABLE_ACCE_COSTUME_SYSTEM ................. Owsap
* ENABLE_AURA_COSTUME_SYSTEM ................. Owsap
* ENABLE_DELETE_FAILURE_TYPE ................. Owsap
* ENABLE_SNOWFLAKE_STICK_EVENT ............... Owsap
* ENABLE_GAME_OPTION_ESCAPE .................. Owsap
* ENABLE_REFINE_ELEMENT_SYSTEM ............... Owsap
* ENABLE_HIDE_COSTUME_SYSTEM ................. Owsap
* ENABLE_PARTY_PROFICY ....................... Owsap
* ENABLE_PARTY_INSIGHT ....................... Owsap
* ENABLE_PVP_BALANCE_IMPROVING ............... Owsap
* ENABLE_SAFEBOX_IMPROVING ................... Owsap
* ENABLE_ATLAS_GM_GOTO ....................... Owsap
* ENABLE_EVENT_BANNER ........................ Owsap
* ENABLE_EVENT_BANNER_REWARD_LIST_RENEWAL .... Owsap
* ENABLE_MINI_GAME_YUTNORI ................... Owsap
* ENABLE_LEFT_SEAT ........................... Owsap
* ENABLE_ELEMENTAL_DUNGEON ................... Owsap
* ENABLE_GUILD_DRAGONLAIR_SYSTEM ............. Owsap
* ENABLE_GUILD_DRAGONLAIR_PARTY_SYSTEM ....... Owsap
* ENABLE_SUMMER_EVENT_ROULETTE ............... Owsap
* ENABLE_FLOWER_EVENT ........................ Owsap
*/
