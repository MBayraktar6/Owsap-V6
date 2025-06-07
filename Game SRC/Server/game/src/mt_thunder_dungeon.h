/**
* Filename: mt_thunder_dungeon.h
* Author: Owsap
**/

#if !defined(__INC_MT_THUNDER_DUNGEON__) && defined(__MT_THUNDER_DUNGEON__)
#define __INC_MT_THUNDER_DUNGEON__

/*
* NOTE : The map looks like a dungeon, but it doesn't function like one.
* There will only be one guardian who spawns a portal to the next map.
* This guardian will change positions in the rooms in a loop of 3 minutes.
* However, if the guardian takes damage, the timer is reset to 10 minutes
* so that players have time to defeat him.
*
* The portal will disappear 1 minute after being created,
* and the guardian will respawn again.
*/

class CMTThunderDungeon : public singleton<CMTThunderDungeon>
{
public:
	static constexpr long MAP_INDEX = 354;
	static constexpr long MAP_ROOMS = 11;
	static constexpr PIXEL_POSITION MAP_ROOM_POS[MAP_ROOMS] = {
		{ 193, 145, 0 },
		{ 123, 216, 0 },
		{ 224, 383, 0 },
		{ 348, 708, 0 },
		{ 375, 608, 0 },
		{ 430, 516, 0 },
		{ 444, 382, 0 },
		{ 388, 195, 0 },
		{ 446, 247, 0 },
		{ 592, 139, 0 },
		{ 646, 152, 0 }
	};

	static constexpr DWORD GUARDIAN_VNUM = 6405;
	static constexpr DWORD PORTAL_VNUM = 20415;

	static constexpr time_t GUARDIAN_MOVE_SEC = 180;
	static constexpr time_t GUARDIAN_BATTLE_SEC = 600;
	static constexpr int GUARDIAN_CHECK_PULSE = 3;

	static constexpr time_t PORTAL_OPEN_SEC = 60;

public:
	CMTThunderDungeon();
	virtual ~CMTThunderDungeon();

public:
	void Initialize();
	void Destroy();

	void SpawnGuardian();
	void MoveGuardian();

	void SpawnPortal(const LPCHARACTER pkGuardian);

public:
	DWORD GetGuardianVID() const { return m_dwGuardianVID; }
	DWORD GetPortalVID() const { return m_dwPortalVID; }

private:
	std::vector<PIXEL_POSITION> m_vGuardianPos;

	DWORD m_dwGuardianVID;
	LPEVENT m_pGuardianMoveEvent;

	DWORD m_dwPortalVID;
	LPEVENT m_pPortalEvent;
};
#endif
