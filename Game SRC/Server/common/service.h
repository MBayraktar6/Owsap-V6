#if !defined(__INC_COMMON_SERVICE_H__)
#define __INC_COMMON_SERVICE_H__

#define __LOCALE_SERVICE_EUROPE__
#define __LOCALE_SERVICE_OWSAP__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////// Farques
#define ENABLE_QUEEN_NETHIS																						/// < Kraliçe Nethis Zindanı








////////////////////////////////////////////////////////////////////////////////
// Game Related
#define __MAILBOX__ // Mail Box System
#define __QUEST_RENEWAL__ // Quest Page Renewal
#if defined(__QUEST_RENEWAL__)
#	define __QUEST_EVENT_DAMAGE__ // Damage Quest Event
#	define __QUEST_EVENT_DEAD__ // Dead Quest Event
#	define __QUEST_EVENT_FISH__ // Fishing Quest Event
#	define __QUEST_EVENT_MINE__ // Mining Quest Event
#	define __QUEST_EVENT_BUY_SELL__ // NPC Buy/Sell Quest Event
#	define __QUEST_EVENT_CRAFT__ // Craft Quest Event
#	define __QUEST_EVENT_EMOTION__ // Emotion Quest Event
#	define __QUEST_EVENT_RESTART_HERE__ // Restart Here Quest Event
#	define __QUEST_REQUEST_EVENT__ // Request Quest Event
#endif
#define __CHATTING_WINDOW_RENEWAL__ // Chatting Window Renewal (Mini Version)
#define __CUBE_RENEWAL__ // Cube Renewal
#define __RANKING_SYSTEM__ // Ranking System
#define __ELEMENT_SYSTEM__ // Element System
#define __SEND_TARGET_INFO__ // Monster Information & Drops
#define __PVP_COUNTDOWN__ // PvP Duel Countdown
#define __REFINE_MSG_ADD__ // Extended refine fail message
#define __PVP_BALANCE_IMPROVING__ // PvP Balance Improving

////////////////////////////////////////////////////////////////////////////////
// Map & Dungeon Related
#define __SNOW_DUNGEON__ // Snow Dungeon
#define __DUNGEON_RENEWAL__ // Extended Dungeon Functions
#define __BLUE_DRAGON_RENEWAL__ // Blue Dragon Rework
#define __MT_THUNDER_DUNGEON__ // Ochao Temple
#define __DAWNMIST_DUNGEON__ // Erebus Dungeon
#define __DEFENSE_WAVE__ // Defense Wave
#define __CLIENT_TIMER__ // Client Timer (Used for instances)
#define __LABYRINTH_DUNGEON__ // Labyrinth Dungeon (Utils)
#define __ELEMENTAL_DUNGEON__ // Elemental Dungeon
#define __GUILD_DRAGONLAIR_SYSTEM__ // Guild Dragon Lair
#if defined(__GUILD_DRAGONLAIR_SYSTEM__)
#	define __GUILD_DRAGONLAIR_PARTY_SYSTEM__ // Guild Dragon Lair Party
#endif

////////////////////////////////////////////////////////////////////////////////
// Mini-Game Related
#define __MINI_GAME_RUMI__ // Mini-Game Rumi
#if defined(__MINI_GAME_RUMI__)
#	define __OKEY_EVENT_FLAG_RENEWAL__
#endif
#define __MINI_GAME_YUTNORI__ // Mini-Game Yutnori
#if defined(__MINI_GAME_YUTNORI__)
#	define __YUTNORI_EVENT_FLAG_RENEWAL__
#endif
#define __MINI_GAME_CATCH_KING__ // Mini-Game Catch King
#if defined(__MINI_GAME_CATCH_KING__)
#	define __CATCH_KING_EVENT_FLAG_RENEWAL__
#endif
#define __FISHING_GAME__ // Fishing Game
#define __SUMMER_EVENT_ROULETTE__ // Mini-Game Roulette (Late Summer Event)

////////////////////////////////////////////////////////////////////////////////
// Event Related
#define __EASTER_EVENT__ // Easter Event 2011
#define __XMAS_EVENT_2008__ // Christmas Event 2008
#define __XMAS_EVENT_2012__ // Christmas Event 2012
#define __2016_VALENTINE_EVENT__ // Valentine Event 2016~2024
#define __HALLOWEEN_EVENT_2014__ // Halloween Event 2011~2014 (Halloween Hair)
#define __OX_RENEWAL__ // OX Renewal
#define __EVENT_BANNER_FLAG__ // Event Banner Flags
#define __METINSTONE_SWAP__ // Swap Stone Shape
#define __RACE_SWAP__ // Swap Race Shape
#define __SNOWFLAKE_STICK_EVENT__ // Snowflake Stick Event
#define __INGAME_EVENT_MANAGER__ // InGame Event Manager
#define __FLOWER_EVENT__ // Flower Event

////////////////////////////////////////////////////////////////////////////////
// Currency Related
#define __CHEQUE_SYSTEM__ // Cheque (Won) System
#define __GEM_SYSTEM__ // Gem (Gaya) System

////////////////////////////////////////////////////////////////////////////////
// Shop Related
#define __SHOPEX_RENEWAL__ // ShopEX Renewal
#if defined(__SHOPEX_RENEWAL__)
#	define __SHOPEX_TAB4__ // ShopEx 4 Tabs
#endif
//#define __SHOPEX_EMPIRE_TAX__ // ShopEx Tax (Triple Price)
#define __MYSHOP_DECO__ // Private Shop Decoration
#define __MYSHOP_EXPANSION__ // Additional Private Shop Tab
#define __PRIVATESHOP_SEARCH_SYSTEM__ // Private Shop Search System
#if defined(__GEM_SYSTEM__)
#	define __GEM_SHOP__ // Gem (Gaya) Shop
#endif

////////////////////////////////////////////////////////////////////////////////
// Dragon Soul Related
#define __DRAGON_SOUL_SYSTEM__ // Dragon Soul System
#if defined(__DRAGON_SOUL_SYSTEM__)
	#define __DS_GRADE_MYTH__ // Dragon Soul Mythical Grade
	#define __DS_SET__ // Dragon Soul Table Handling
	#define __DS_CHANGE_ATTR__ // Dragon Soul Change Attribute
	#define __DS_7_SLOT__ // Dragon Soul 7th Slot
#endif

////////////////////////////////////////////////////////////////////////////////
// Pet Related
#define __PET_SYSTEM__ // Pet System
#define __PET_LOOT__ // Pet Loot
//#define __PET_LOOT_AI__ // Pet Loot AI (Goes and picks items)

////////////////////////////////////////////////////////////////////////////////
// Character Related
#define __PLAYER_PER_ACCOUNT5__ // Players Per Account (5)
#define __WOLFMAN_CHARACTER__ // Wolfman Character
#if defined(__WOLFMAN_CHARACTER__)
//#	define __DISABLE_WOLFMAN_CREATION__ // Disable Wolfman Creation
#endif
#define __VIEW_TARGET_HP__ // View Target HP
#if defined(__VIEW_TARGET_HP__)
#	define __VIEW_TARGET_PLAYER_HP__ // View Player Target HP
#endif
#define __IMPROVED_LOGOUT_POINTS__ // Improved Logout Points
#define __EXPRESSING_EMOTIONS__ // Special Actions
#define __CONQUEROR_LEVEL__ // Conqueror Level
#define __MULTI_LANGUAGE_SYSTEM__ // Multi Language System
#define __DELETE_FAILURE_TYPE__ // Delete Character Failure Type
#define __LEFT_SEAT__ // Left Seat (AFK)
#define __AFFECT_RENEWAL__ // Affect Renewal

////////////////////////////////////////////////////////////////////////////////
// Skill Related
#define __7AND8TH_SKILLS__ // 7th, 8th Passive Skills
#define __SKILL_COOLTIME_UPDATE__ // Refresh Skill Cooldown After Death
#define __9TH_SKILL__ // 9th Player Skill
#define __PARTY_PROFICY__ // Party Proficiency Passive Skill
#define __PARTY_INSIGHT__ // Party InSight Passive Skill

////////////////////////////////////////////////////////////////////////////////
// Party & Guild Related
#define __DICE_SYSTEM__ // New Dice System (Party)
#define __WJ_SHOW_PARTY_ON_MINIMAP__ // Party Member Atlas (Map)
#define __PARTY_CHANNEL_FIX__ // Party Channel Fix
#define __GUILD_LEADER_GRADE_NAME__ // Guild Leader Grade Name (TextTail)
#define __PARTY_KILL_RENEWAL__ // All kill events count towards the party.
#define __GUILD_WAR_AUTO_JOIN_LEADER__ // Join the Guild War automatically as the Leader.
#define __GUILD_EVENT_FLAG__ // Guild Event Flag

////////////////////////////////////////////////////////////////////////////////
// Messenger Related
#define __MESSENGER_BLOCK_SYSTEM__ // Messenger Block System
#define __MESSENGER_GM__ // Messenger GM List
#define __MESSENGER_DETAILS__ // Messenger Details

////////////////////////////////////////////////////////////////////////////////
// Inventory Related
#define __EXTEND_INVEN_SYSTEM__ // Extended Inventory System
#define __EXTEND_MALLBOX__ // Extended Mallbox
#define __SAFEBOX_IMPROVING__ // Safebox Improving

////////////////////////////////////////////////////////////////////////////////
// Equipment Related
#define __QUIVER_SYSTEM__ // Quiver Equipment
#define __PENDANT_SYSTEM__ // Talisman Elements
#define __GLOVE_SYSTEM__ // Glove Equipment

////////////////////////////////////////////////////////////////////////////////
// Costume Related
#define __COSTUME_SYSTEM__ // Costume System
#define __MOUNT_COSTUME_SYSTEM__ // Mount Costume System
#define __ACCE_COSTUME_SYSTEM__ // Acce (Sash) Costume System
#define __AURA_COSTUME_SYSTEM__ // Aura Costume System
#define __WEAPON_COSTUME_SYSTEM__ // Weapon Costume System
#if defined(__WEAPON_COSTUME_SYSTEM__)
#	define __HIDE_WEAPON_COSTUME_WITH_NO_MAIN_WEAPON__
#endif
#define __MOVE_COSTUME_ATTR__ // Move Costume Attribute (Item Combination)
#define __HIDE_COSTUME_SYSTEM__ // Hide Costume System

////////////////////////////////////////////////////////////////////////////////
// Item Related
#define __MAGIC_REDUCTION__ // Magic Reduction Item
#define __STONE_OF_BLESS__ // Stone of Bless (Refinement Item)
#define __REFINE_PICKAXE_RENEWAL__ // Refine Pickaxe Renewal
#define __REFINE_FISHINGROD_RENEWAL__ // Refine Fishing Rod Renewal
#define __SOUL_BIND_SYSTEM__ // Seal Scroll System
#if defined(__SOUL_BIND_SYSTEM__)
#	define __DRAGON_SOUL_SEAL__ // Dragon Soul Seal
#	define __UN_SEAL_SCROLL_PLUS__ // Unseal Scroll Plus
#endif
//#define __SOUL_SYSTEM__ // Soul System
#if defined(__SOUL_SYSTEM__)
#	define __SOUL_SYSTEM_CALC_FINAL_DAMAGE__
#endif
#define __ITEM_APPLY4__ // Extended Apply Bonus (4)
#define __ITEM_SOCKET6__ // Extended Item Sockets (6)
#define __ITEM_VALUE10__ // Extended Item Values
#define __ITEM_APPLY_RANDOM__ // Apply Random Bonus (Base Bonus)
#define __CHANGED_ATTR__ // Change / Select Attribute
#define __ATTR_6TH_7TH__ // 6th and 7th Attribute
#define __SKILLBOOK_COMB_SYSTEM__ // Skill Book Combination
#define __CHANGE_LOOK_SYSTEM__ // Change Look System (Item Look)
#define __LOOT_FILTER_SYSTEM__ // Looting System
#if defined(__LOOT_FILTER_SYSTEM__)
#	define __PREMIUM_LOOT_FILTER__ // Enable Premium Usage of the Loot Filter System
#endif
#define __GACHA_SYSTEM__ // Boss (Gacha) Boxes (Open x Times)
#define __LUCKY_BOX__ // Lucky Box
#define __SET_ITEM__ // Set Item Bonus
#define __GEM_CONVERTER__ // Gem Converter
#define __REFINE_ELEMENT_SYSTEM__ // Refine Element System
#define __USE_NEXT_AUTO_POTION__ // When the automatic potion runs out, use the next one.
#define __REFINE_STACK_FIX__ // Refine Stack Fix (Used for stones)

////////////////////////////////////////////////////////////////////////////////
// UI Related
#define __WJ_SHOW_MOB_INFO__ // Monsters Level & Aggressive Flag
#define __WJ_PICKUP_ITEM_EFFECT__ // Picking Item Effect
#define __NEW_USER_CARE__ // User Care (Control)
#define __BINARY_ATLAS_MARK_INFO__ // Atlas Mark Info Load
#define __POPUP_NOTICE__ // Pop-up notification
#define __NEW_DROP_DIALOG__ // New Drop Dialog w/ Delete Item Option
#define __ITEM_DROP_RENEWAL__ // Item Drop Renewal w/ Color Effect
#define __GAME_OPTION_ESCAPE__ // Game Option (Escape)

////////////////////////////////////////////////////////////////////////////////
// Miscellaneous
#define __EXTENDED_RELOAD__ // Extended GM Reload Commands (For drops)
#define __EXTENDED_ITEM_AWARD__ // Extended Item Award
#define __ENVIRONMENT_SYSTEM__ // Environment System
#define __LOCALE_CLIENT__ // Locale Client

////////////////////////////////////////////////////////////////////////////////
// Network Related
//#define __IMPROVED_PACKET_ENCRYPTION__ // Improved Packet Encryption
//#define __SEND_SEQUENCE__ // Sequence Matching
#define __UDP_BLOCK__ // UDP Block
//#define __ALLOW_EXTERNAL_PEER__ // Allow External Peer API
#define __PROXY_IP__ // Proxy IP
#define __MOVE_CHANNEL__ // Move Game Channel
#define __CHECK_PORT_STATUS__ // Check Port Status
#define __AUTO_DETECT_INTERNAL_IP__ // Detect Internal IP

#include "disclaimer.h"
#endif // __INC_COMMON_SERVICE_H__
