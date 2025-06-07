#ifndef __INC_UNIQUE_MOB_H__
#define __INC_UNIQUE_MOB_H__

enum EUniqueMob
{
	THUNDER_BOSS = 6192,
	THUNDER_HEALER = 6409,

	LAND_AGENT = 20040,

#if defined(__XMAS_EVENT_2012__)
	// Bambi
	MOB_XMAS_REINDEER = 53002,
	MOB_XMAS_REINDEER_MALL = 53007,
#endif

#if defined(__SNOWFLAKE_STICK_EVENT__)
	MOB_XMAS_SNOWMAN = 33010,
#endif
};

#endif /* __INC_UNIQUE_MOB_H__ */
