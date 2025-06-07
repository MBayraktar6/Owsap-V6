#if !defined(__INC_LOCALE_INC_H__)
#define __INC_LOCALE_INC_H__

#include "Version.h"

#define LOCALE_SERVICE_EUROPE
#define LOCALE_SERVICE_OWSAP

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////// Farques
#define ENABLE_QUEEN_NETHIS																						/// < Kraliçe Nethis Zindanı



////////////////////////////////////////////////////////////////////////////////
// Game Related
#define ENABLE_MAILBOX // Mail Box System
#define ENABLE_QUEST_RENEWAL // Quest Page Renewal
#define ENABLE_CHATTING_WINDOW_RENEWAL // Chatting Window Renewal (Mini Version)
#define ENABLE_CUBE_RENEWAL // Cube Renewal
#define ENABLE_RANKING_SYSTEM // Ranking System
#define ENABLE_ELEMENT_ADD // Monster Element
#define ENABLE_SEND_TARGET_INFO // Monster Information & Drops
#define ENABLE_REFINE_MSG_ADD // Extended Refine Fail Message
#define ENABLE_BALANCE_IMPROVING // PvP Balance Improving
#define ENABLE_QUEST_REQUEST_EVENT // Request Event Quest

////////////////////////////////////////////////////////////////////////////////
// Map & Dungeon Related
#define ENABLE_DEFENSE_WAVE // Defense Wave (Hydra Dungeon)
#define ENABLE_CLIENT_TIMER // Client Timer (Used for instances)
#define ENABLE_ELEMENTAL_DUNGEON // Elemental Dungeon
#define ENABLE_GUILD_DRAGONLAIR_SYSTEM // Guild Dragon Lair
#if defined(ENABLE_GUILD_DRAGONLAIR_SYSTEM)
#	define ENABLE_GUILD_DRAGONLAIR_PARTY_SYSTEM // Guild Dragon Lair Party
#endif

////////////////////////////////////////////////////////////////////////////////
// Mini-Game Related
#define ENABLE_MINI_GAME_RUMI // Mini-Game Rumi (Okey Card)
#if defined(ENABLE_MINI_GAME_RUMI)
#	define ENABLE_OKEY_EVENT_FLAG_RENEWAL
#endif
#define ENABLE_MINI_GAME_YUTNORI // Mini-Game Yutnori
#if defined(ENABLE_MINI_GAME_YUTNORI)
#	define ENABLE_YUTNORI_EVENT_FLAG_RENEWAL
#endif
#define ENABLE_MINI_GAME_CATCH_KING // Mini-Game Catch King
#if defined(ENABLE_MINI_GAME_CATCH_KING)
#	define ENABLE_CATCH_KING_EVENT_FLAG_RENEWAL
#endif
#define ENABLE_FISHING_GAME // Fishing Game
#define ENABLE_SUMMER_EVENT_ROULETTE // Summer Event Roulette
#if defined(ENABLE_SUMMER_EVENT_ROULETTE)
//#	define ENABLE_LATE_SUMMER_EVENT_SKYBOX
#endif

////////////////////////////////////////////////////////////////////////////////
// Event Related
#define ENABLE_OX_RENEWAL // OX Renewal
#define ENABLE_EVENT_BANNER_FLAG // Event Banner Flags
#define ENABLE_METINSTONE_SWAP // Swap Metin Stone Shape
#define ENABLE_RACE_SWAP // Swap Race Shape
#define ENABLE_SNOWFLAKE_STICK_EVENT // Snowflake Stick Event
#define ENABLE_EVENT_BANNER // Event Banner
#define ENABLE_EVENT_BANNER_REWARD_LIST_RENEWAL // Event Banner Reward List Renewal
#define ENABLE_FLOWER_EVENT // Flower Event

////////////////////////////////////////////////////////////////////////////////
// Currency Related
#define ENABLE_CHEQUE_SYSTEM // Cheque (Won) System
#define ENABLE_GEM_SYSTEM // Gem (Gaya) System

////////////////////////////////////////////////////////////////////////////////
// Shop Related
#define ENABLE_SHOPEX_RENEWAL // ShopEX Renewal
#if defined(ENABLE_SHOPEX_RENEWAL)
#	define ENABLE_SHOPEX_TAB4 // ShopEx 4 Tabs
#endif
#define ENABLE_MYSHOP_DECO // Private Shop Decoration
#define ENABLE_MYSHOP_EXPANSION // Additional Private Shop Tab
#define ENABLE_PRIVATESHOP_SEARCH_SYSTEM // Private Shop Search System

////////////////////////////////////////////////////////////////////////////////
// Dragon Soul Related
#define ENABLE_DRAGON_SOUL_SYSTEM // Dragon Soul System
#if defined(ENABLE_DRAGON_SOUL_SYSTEM)
	#define ENABLE_DS_GRADE_MYTH // Dragon Soul Mythical Grade
	#define ENABLE_DS_SET // Dragon Soul Table Handling
	#define ENABLE_DS_CHANGE_ATTR // Dragon Soul Change Attribute
	#define ENABLE_DS_7_SLOT // Dragon Soul 7th Slot
#endif

////////////////////////////////////////////////////////////////////////////////
// Character Related
#define ENABLE_PLAYER_PER_ACCOUNT5 // Players Per Account (5)
#define ENABLE_WOLFMAN_CHARACTER // Wolfman Character
#if defined(ENABLE_WOLFMAN_CHARACTER)
//#	define DISABLE_WOLFMAN_CREATION // Disable Wolfman Creation
#endif
#define ENABLE_VIEW_TARGET_DECIMAL_HP // View Target HP
#if defined(ENABLE_VIEW_TARGET_DECIMAL_HP)
#	define ENABLE_VIEW_TARGET_PLAYER_HP // View Player Target HP
#endif
#define ENABLE_IMPROVED_LOGOUT_POINTS // Improved Logout Points
#define ENABLE_MULTI_LANGUAGE_SYSTEM // Multi Language System
#define ENABLE_EXPRESSING_EMOTION // Special Actions
#define ENABLE_CONQUEROR_LEVEL // Conqueror Level
#define ENABLE_DELETE_FAILURE_TYPE // Delete Character Failure Type
#if defined(ENABLE_DELETE_FAILURE_TYPE)
#	define ENABLE_DELETE_FAILURE_TYPE_ADD // Additional Failure Types
#endif
#define ENABLE_LEFT_SEAT // Left Seat (AFK)
#define ENABLE_AFFECT_RENEWAL // Affect Renewal

////////////////////////////////////////////////////////////////////////////////
// Skill Related
#define ENABLE_678TH_SKILL // 6th & 7th Skills
#define ENABLE_SKILL_COOLTIME_UPDATE // Refresh Skill Cooldown After Death
#define ENABLE_PARTY_PROFICY // Party Proficiency Passive Skill
#define ENABLE_PARTY_INSIGHT // Party InSight Passive Skill

////////////////////////////////////////////////////////////////////////////////
// Party & Guild Related
#define ENABLE_DICE_SYSTEM // New Dice System (Party)
#define WJ_SHOW_PARTY_ON_MINIMAP // Party Member Atlas (Map)
#define ENABLE_PARTY_CHANNEL_FIX // Party Channel Fix
#define ENABLE_GUILD_LEADER_GRADE_NAME // Guild Leader Grade Name (TextTail)
#define ENABLE_GUILD_BUILDING_OBJ_PLACEMENT_EFFECT // Guild Building Object Placement Effect

////////////////////////////////////////////////////////////////////////////////
// Messenger Related
#define ENABLE_MESSENGER_LOCK_FIX // Messenger List Lock Fix
#define ENABLE_MESSENGER_BLOCK // Messenger Block System
#define ENABLE_MESSENGER_GM // Messenger GM List
#define ENABLE_MESSENGER_DETAILS // Messenger Details

////////////////////////////////////////////////////////////////////////////////
// Inventory Related
#define ENABLE_NEW_EQUIPMENT_SYSTEM // New Equipment System (Belt)
#define ENABLE_EXTEND_INVEN_SYSTEM // Extended Inventory Pages (4)
#define ENABLE_EXTEND_MALLBOX // Extended Mallbox
#define ENABLE_SAFEBOX_IMPROVING // Safebox Improving

////////////////////////////////////////////////////////////////////////////////
// Equipment Related
#define ENABLE_QUIVER_SYSTEM // Quiver Equipment
#define ENABLE_PENDANT // Talisman Elements
#define ENABLE_GLOVE_SYSTEM // Glove Equipment
#define ENABLE_ADDITIONAL_EQUIPMENT_PAGE // Additional Equipment Page

////////////////////////////////////////////////////////////////////////////////
// Costume Related
#define ENABLE_COSTUME_SYSTEM // Costume System
#define ENABLE_MOUNT_COSTUME_SYSTEM // Mount Costume System
#define ENABLE_ACCE_COSTUME_SYSTEM // Acce (Sash) Costume System
#define ENABLE_ACCE_SECOND_COSTUME_SYSTEM // Acce (Sash) Costume System
#define ENABLE_AURA_COSTUME_SYSTEM // Aura Costume System
#define ENABLE_WEAPON_COSTUME_SYSTEM // Weapon Costume System
#define ENABLE_MOVE_COSTUME_ATTR // Move Costume Attribute (Item Combination)
#define ENABLE_HIDE_COSTUME_SYSTEM // Hide Costume System

////////////////////////////////////////////////////////////////////////////////
// Item Related
#define ENABLE_ENERGY_SYSTEM // Energy System
#define ENABLE_MAGIC_REDUCTION // Magic Reduction Item
#define ENABLE_STONE_OF_BLESS // Stone of Bless (Refinement Item)
#define ENABLE_SOULBIND_SYSTEM // Soul Bind System (Protect Items)
#if defined(ENABLE_SOULBIND_SYSTEM)
#	define ENABLE_DRAGON_SOUL_SEAL // Dragon Soul Seal
#	define ENABLE_UN_SEAL_SCROLL_PLUS // Un-Seal Scroll Plus
#endif
//#define ENABLE_SOUL_SYSTEM // Soul System
#define ENABLE_ITEM_APPLY4 // Extended Apply Bonus (4)
#define ENABLE_ITEM_SOCKET6 // Extended Item Sockets (6)
#define ENABLE_ITEM_VALUE10 // Extended Item Values
#define ENABLE_APPLY_RANDOM // Apply Random Bonus (Base Bonus)
#define ENABLE_CHANGED_ATTR // Change / Select Attribute
#define ENABLE_ATTR_6TH_7TH // 6th and 7th Attribute
#define ENABLE_SKILLBOOK_COMB_SYSTEM // Skill Book Combination
#define ENABLE_CHANGE_LOOK_SYSTEM // Change Look System (Item Look)
#define ENABLE_LOOTING_SYSTEM // Looting System (Filter)
#if defined(ENABLE_LOOTING_SYSTEM)
#	define ENABLE_PREMIUM_LOOTING // Enable Premium Usage of the Loot Filter System
#endif
#define ENABLE_GACHA // Boxes (Open x Times)
#define ENABLE_LUCKY_BOX // Lucky Box
#define ENABLE_SET_ITEM // Set Item Bonus
#define ENABLE_GEM_CONVERTER // Gem Converter
#define ENABLE_REFINE_ELEMENT_SYSTEM // Refine Element

////////////////////////////////////////////////////////////////////////////////
// UI Related
#define ENABLE_SELECT_RENEWAL // New Select UI
#define ENABLE_CREATE_RENEWAL // New Create UI
#define ENABLE_SERVER_SELECT_RENEWAL // Server List Renewal
#define ENABLE_CHANNEL_LIST // Channel List Renewal
#define LOGIN_COUNT_DOWN_UI_MODIFY // Login Count Down
#define ENABLE_NEW_USER_CARE // User Care (Control)
#define WJ_SHOW_MOB_INFO // Monsters Level & Aggressive Flag
#define WJ_ENABLE_PICKUP_ITEM_EFFECT // Picking Item Effect
#define WJ_ENABLE_TRADABLE_ICON // Block Untradable Items
#define WJ_MULTI_TEXTLINE // Multi TextLine
#define ENABLE_ATLAS_MARK_INFO // Atlas Mark Info Load
#define ENABLE_POPUP_NOTICE // Pop-up notification
#define ENABLE_NEW_DROP_DIALOG // New Drop Dialog w/ Delete Item Option
#define ENABLE_ITEM_DROP_RENEWAL // Item Drop Renewal w/ Color Effect
#define ENABLE_TEXT_IMAGE_LINE // Image TextLine (Emoji)
#define ENABLE_HIGH_LIGHT_IMAGE // High Light Image
#define ENABLE_SLOT_SCALE // Slot Scale
#define ENABLE_SLOT_COOLTIME_REFRESH_FIX // Slot Cooltime Refresh
#define ENABLE_CLIP_MASK // Window Clip Masking
#define ENABLE_MOUSE_WHEEL_TOP_WINDOW // Mouse Scroll Wheel
#define RENDER_TARGET // Render Target Model
#define ENABLE_KEYCHANGE_SYSTEM // Keyboard Customization System
#if defined(ENABLE_KEYCHANGE_SYSTEM)
#	define ENABLE_DSS_KEY_SELECT
#endif
#define ENABLE_DETAILS_UI // Character Details (Bonus Page)
#define ENABLE_CHAT_LOG_FIX // Chat-log fix
#define ENABLE_INPUT_CANCEL // Quest Input Cancel
#define ENABLE_HELP_RENEWAL // Help Renewal
#define ENABLE_MOVE_WND_SCREEN_LIMIT // Move Window Screen Limit
#define ENABLE_QUEST_WIDTH_EXPANSION // Quest Window Expansion
#define ENABLE_MINIMAP_TEXTURE_PATTERN // MiniMap Texture Pattern
#define ENABLE_HIDE_ATTACHING_EFFECT_ON_ACTING // Hide attaching effects on acting
#define ENABLE_LOADING_TIP // Loading Tips
#define ENABLE_TAB_NEXT_TARGET // Next Target Tab Key
#define ENABLE_GAME_OPTION_ESCAPE // Game Option (Escape)
#define ENABLE_ATLAS_GM_GOTO // Atlas Game Master Goto

// Environment & Render Related
#define REMOVE_SAFEZONE_COLLISION // Remove Safe Zone NPC Collision 
#define REMOVE_SHOP_COLLISION // Remove Shop Collision
#define REMOVE_HORSE_COLLISION // Remove Horse Collision
#define REMOVE_PET_COLLISION // Remove Pet Collision
#define REMOVE_OX_PLAYER_COLLISION // Remove OX Map Player Collision
#define REMOVE_BAZAR_COLLISION // Remove Bazar Collision
#define ENABLE_FOG_FIX // Fog Fix
#define ENABLE_ENVIRONMENT_EFFECT_OPTION // Environment Effect Options
#define ENABLE_FOV_OPTION // Field of View (FoV) Game Option
#define ENABLE_DISABLE_SOFTWARE_TILING // Disable Software Tiling
#define ENABLE_GRAPHIC_ON_OFF // Graphic System (On/Off)
#define ENABLE_SHADOW_RENDER_QUALITY_OPTION // Shadow Render Quality
#define ENABLE_OPTIMIZATION // Model Animation Optimization

////////////////////////////////////////////////////////////////////////////////
// Miscellaneous
#define ENABLE_LOCALE_CLIENT // Locale Client Strings
#define ENABLE_EMOTION_HIDE_WEAPON // Hide Weapon While Acting

////////////////////////////////////////////////////////////////////////////////
// Application Related
#define DISABLE_INDEX_FILE // Built In Pack Index
#define ENABLE_ETER_PACK_OBSCURING // EterPack Key Obscuring
#define ENABLE_DISCORD_RPC // DiscordRPC
#define ENABLE_CEF_BROWSER // CEF Browser
#define ENABLE_CHAR_CONFIG // Character Config
#if defined(_DEBUG)
#	define ENABLE_CONSOLE_WINDOW // Console Window
#else
#	define ENABLE_CHECK_DLL_MD5 // Check File MD5
#endif

////////////////////////////////////////////////////////////////////////////////
// Network Related
#define ENABLE_MOVE_CHANNEL // Move Game Channel

#include "Disclaimer.h"
#endif // __INC_LOCALE_INC_H__
