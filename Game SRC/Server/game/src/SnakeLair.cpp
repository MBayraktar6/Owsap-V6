/*
	Author: R4kan
	Date: 09-01-2022
	Version: 0.5
	Github: https://github.com/Rakancito/
*/

#include "stdafx.h"
#include "SnakeLair.h"

#include "item.h"
#include "char.h"
#include "utils.h"
#include "party.h"
#include "regen.h"
#include "config.h"
#include "packet.h"
#include "motion.h"
#include "item_manager.h"
#include "guild_manager.h"
#include "guild.h"
#include "start_position.h"
#include "locale_service.h"
#include "char_manager.h"
#include <boost/lexical_cast.hpp> 
#define itertype(container) __typeof__((container).begin())

namespace SnakeLair
{
	
#ifdef ENABLE_CONQUEROR_LEVEL
	int iSungmaAffect[4][8] =
	{
		{AFFECT_SUNGMA_STR, 35, 35, 38, 41, 44, 47, 50},
		{AFFECT_SUNGMA_HP, 25, 25, 28, 31, 34, 37, 40},
		{AFFECT_SUNGMA_MOVE, 35, 35, 38, 41, 44, 47, 50},
		{AFFECT_SUNGMA_INMUNE, 40, 40, 43, 46, 49, 52, 55},
	};
#endif

	int stoneKillSort[4][4] =
	{
		{8068, 8069, 8070, 8071}, //Fire, Ice, Wind, Earth
		{8069, 8071, 8068, 8070}, //Ice, Earth, Fire, Wind
		{8071, 8068, 8070, 8069}, //Earth, Fire, Wind, Ice
		{8070, 8071, 8069, 8068} //Wind, Earth, Ice, Fire
	};

	int EarthMonsterFloor1[15][3] =
	{
		{6850, 163, 156},
		{6850, 167, 159},
		{6850, 160, 160},
		{6850, 159, 162},
		{6850, 167, 164},
		{6850, 161, 167},
		{6850, 163, 163},
		{6850, 156, 156},
		{6850, 153, 162},
		{6850, 156, 169},
		{6850, 171, 167},
		{6850, 165, 164},
		{6850, 165, 159},
		{6850, 168, 161},
		{6850, 171, 161}
	};

	int FireMonsterFloor1[15][3] =
	{
		{6851, 164, 93},
		{6851, 167, 91},
		{6851, 163, 84},
		{6851, 159, 89},
		{6851, 155, 89},
		{6851, 161, 95},
		{6851, 159, 98},
		{6851, 161, 98},
		{6851, 166, 98},
		{6851, 167, 98},
		{6851, 169, 94},
		{6851, 165, 93},
		{6851, 168, 94},
		{6851, 153, 93},
		{6851, 159, 92}
	};

	int IceMonsterFloor1[15][3] =
	{
		{6852, 96, 88},
		{6852, 96, 93},
		{6852, 89, 92},
		{6852, 90, 88},
		{6852, 88, 92},
		{6852, 89, 97},
		{6852, 93, 96},
		{6852, 90, 95},
		{6852, 95, 95},
		{6852, 97, 98},
		{6852, 93, 101},
		{6852, 100, 93},
		{6852, 97, 88},
		{6852, 92, 91},
		{6852, 94, 91}
	};

	int WindMonsterFloor1[15][3] =
	{
		{6854, 96, 159},
		{6854, 96, 156},
		{6854, 89, 159},
		{6854, 90, 164},
		{6854, 88, 161},
		{6854, 89, 161},
		{6854, 93, 166},
		{6854, 90, 161},
		{6854, 95, 167},
		{6854, 97, 164},
		{6854, 93, 163},
		{6854, 100, 162},
		{6854, 97, 158},
		{6854, 92, 159},
		{6854, 94, 156}
	};

	int iSerpentStoneCoord[6][2] =
	{
		{674, 93},
		{674, 127},
		{674, 162},
		{605, 93},
		{605, 127},
		{605, 162}
	};

	int iSortStoneCoord[6][4] =
	{
		{0, 1, 2, 3},
		{1, 2, 3, 4},
		{2, 3, 4, 5},
		{3, 4, 5, 0},
		{4, 5, 0, 1},
		{5, 0, 1, 2}
	};

	int iSerpentFinalBoss[28][3] =
	{
		{6850, 148, 618},
		{6850, 133, 609},
		{6851, 130, 617},
		{6851, 110, 612},
		{6852, 107, 619},
		{6852, 97, 629},
		{6853, 101, 638},
		{6853, 94, 649},
		{6854, 105, 656},
		{6854, 102, 659},
		{6850, 129, 664},
		{6850, 145, 654},
		{6851, 160, 654},
		{6851, 155, 628},
		{6852, 151, 638},
		{6852, 165, 639},
		{6853, 159, 631},
		{6853, 169, 638},
		{6854, 161, 624},
		{6854, 171, 629},
		{6850, 167, 649},
		{6850, 104, 643},
		{6851, 97, 646},
		{6851, 92, 636},
		{6852, 90, 623},
		{6852, 97, 621},
		{6853, 120, 609}
	};

	struct FPartyCHCollector
	{
		std::vector <DWORD> pPlayerID;
		FPartyCHCollector()
		{
		}
		void operator () (LPCHARACTER ch)
		{
			pPlayerID.push_back(ch->GetPlayerID());
		}
	};

	struct FExitAndGoTo
	{
		FExitAndGoTo() {};
		void operator()(LPENTITY ent)
		{
			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER pkChar = (LPCHARACTER) ent;
				if (pkChar && pkChar->IsPC())
				{
					PIXEL_POSITION posSub = CSnk::instance().GetSubXYZ();
					if (!posSub.x)
						pkChar->WarpSet(EMPIRE_START_X(pkChar->GetEmpire()), EMPIRE_START_Y(pkChar->GetEmpire()));
					else
						pkChar->WarpSet(posSub.x, posSub.y);
				}
			}
		}
	};

	struct FSkillQueenNethis
	{
		FSkillQueenNethis() {};
		void operator()(LPENTITY ent)
		{
			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER pkChar = (LPCHARACTER) ent;
				if (pkChar && pkChar->IsPC())
				{
					if (!pkChar->IsDead())
					{
						pkChar->EffectPacket(SE_EFFECT_SNAKE_REGEN);
						pkChar->ComputeSnakeSkill(273, pkChar, 1);
					}
				}
			}
		}
	};

	struct FWarpMap
	{
		FWarpMap(int x, int y, long lMapIndex) : m_x(x), m_y(y), m_lMapIndex(lMapIndex) {};
		void operator()(LPENTITY ent)
		{
			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER pkChar = (LPCHARACTER) ent;
				if (pkChar->IsPC())
				{
					PIXEL_POSITION pos = CSnk::instance().GetXYZ();
					if (!pos.x)
						return;
					else
						pkChar->WarpSet(pos.x+m_x, pos.y+m_y, m_lMapIndex);
				}
			}
		}

		int m_x, m_y;
		long m_lMapIndex;
	};

	struct FPurgeSectree
	{
		void operator () (LPENTITY ent)
		{
			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER ch = (LPCHARACTER) ent;

#ifdef NEW_PET_SYSTEM
				if (ch && !ch->IsPC() && !ch->IsPet() && !ch->IsNewPet())
#else
				if (ch && !ch->IsPC() && !ch->IsPet())
#endif
				{
					if (ch->IsNPC())
						M2_DESTROY_CHARACTER(ch);
					else
						ch->Dead();
				}
			}
		}
	};

	EVENTINFO(r_snakespawn_info)
	{
		CSnkMap*	pMap;
		BYTE	bStep;
		BYTE	bSubStep;
	};
	
	EVENTFUNC(r_snakespawn_event)
	{
		r_snakespawn_info* pEventInfo = dynamic_cast<r_snakespawn_info*>(event->info);
		if (!pEventInfo)
			return 0;
		
		CSnkMap* pMap = pEventInfo->pMap;
		if (!pMap)
			return 0;
		
		if (!pMap->GetMapSectree())
			return 0;
		
		if (pEventInfo->bStep == 2)
		{
			if (pEventInfo->bSubStep == 1)
			{
				if (pMap->GetElementSort() == 0)
					for(BYTE b = 0; b < 15; b++)
						pMap->Spawn((DWORD)(FireMonsterFloor1[b][0]), FireMonsterFloor1[b][1], FireMonsterFloor1[b][2], 0);
				else if (pMap->GetElementSort() == 1)
					for(BYTE b = 0; b < 15; b++)
						pMap->Spawn((DWORD)(IceMonsterFloor1[b][0]), IceMonsterFloor1[b][1], IceMonsterFloor1[b][2], 0);
				else if (pMap->GetElementSort() == 2)
					for(BYTE b = 0; b < 15; b++)
						pMap->Spawn((DWORD)(EarthMonsterFloor1[b][0]), EarthMonsterFloor1[b][1], EarthMonsterFloor1[b][2], 0);
				else if (pMap->GetElementSort() == 3)
					for(BYTE b = 0; b < 15; b++)
						pMap->Spawn((DWORD)(WindMonsterFloor1[b][0]), WindMonsterFloor1[b][1], WindMonsterFloor1[b][2], 0);

				pEventInfo->bSubStep = 2;

				return PASSES_PER_SEC(2);
			}
			else if (pEventInfo->bSubStep == 2)
			{
				if (pMap->GetElementSort() == 0)
					for(BYTE b = 0; b < 15; b++)
						pMap->Spawn((DWORD)(IceMonsterFloor1[b][0]), IceMonsterFloor1[b][1], IceMonsterFloor1[b][2], 0);
				else if (pMap->GetElementSort() == 1)
					for(BYTE b = 0; b < 15; b++)
						pMap->Spawn((DWORD)(EarthMonsterFloor1[b][0]), EarthMonsterFloor1[b][1], EarthMonsterFloor1[b][2], 0);
				else if (pMap->GetElementSort() == 2)
					for(BYTE b = 0; b < 15; b++)
						pMap->Spawn((DWORD)(FireMonsterFloor1[b][0]), FireMonsterFloor1[b][1], FireMonsterFloor1[b][2], 0);
				else if (pMap->GetElementSort() == 3)
					for(BYTE b = 0; b < 15; b++)
						pMap->Spawn((DWORD)(EarthMonsterFloor1[b][0]), EarthMonsterFloor1[b][1], EarthMonsterFloor1[b][2], 0);

				pEventInfo->bSubStep = 3;
				return PASSES_PER_SEC(2);

			}
			else if (pEventInfo->bSubStep == 3)
			{
				if (pMap->GetElementSort() == 0)
					for(BYTE b = 0; b < 15; b++)
						pMap->Spawn((DWORD)(WindMonsterFloor1[b][0]), WindMonsterFloor1[b][1], WindMonsterFloor1[b][2], 0);
				else if (pMap->GetElementSort() == 1)
					for(BYTE b = 0; b < 15; b++)
						pMap->Spawn((DWORD)(FireMonsterFloor1[b][0]), FireMonsterFloor1[b][1], FireMonsterFloor1[b][2], 0);
				if (pMap->GetElementSort() == 2)
					for(BYTE b = 0; b < 15; b++)
						pMap->Spawn((DWORD)(WindMonsterFloor1[b][0]), WindMonsterFloor1[b][1], WindMonsterFloor1[b][2], 0);
				if (pMap->GetElementSort() == 3)
					for(BYTE b = 0; b < 15; b++)
						pMap->Spawn((DWORD)(IceMonsterFloor1[b][0]), IceMonsterFloor1[b][1], IceMonsterFloor1[b][2], 0);

				pEventInfo->bSubStep = 4;
				return PASSES_PER_SEC(2);

			}
			if (pEventInfo->bSubStep == 4)
			{
				if (pMap->GetElementSort() == 0)
					for(BYTE b = 0; b < 15; b++)
						pMap->Spawn((DWORD)(EarthMonsterFloor1[b][0]), EarthMonsterFloor1[b][1], EarthMonsterFloor1[b][2], 0);
				else if (pMap->GetElementSort() == 1)
					for(BYTE b = 0; b < 15; b++)
						pMap->Spawn((DWORD)(WindMonsterFloor1[b][0]), WindMonsterFloor1[b][1], WindMonsterFloor1[b][2], 0);
				else if (pMap->GetElementSort() == 2)
					for(BYTE b = 0; b < 15; b++)
						pMap->Spawn((DWORD)(IceMonsterFloor1[b][0]), IceMonsterFloor1[b][1], IceMonsterFloor1[b][2], 0);
				if (pMap->GetElementSort() == 3)
					for(BYTE b = 0; b < 15; b++)
						pMap->Spawn((DWORD)(FireMonsterFloor1[b][0]), FireMonsterFloor1[b][1], FireMonsterFloor1[b][2], 0);

				return 0;
			}
		}

		if (pEventInfo->bStep == 3)
		{
			if (pEventInfo->bSubStep == 1)
			{
				FWarpMap f((254*100), (60*100), pMap->GetMapIndex());
				pMap->GetMapSectree()->for_each(f);
			}
			
			pMap->Spawn(8068, 418, 162, 8);
			pMap->Spawn(8069, 349, 162, 8);
			pMap->Spawn(8070, 349, 93, 8);
			pMap->Spawn(8071, 418, 93, 8);
			
			return 0;
		}
		if (pEventInfo->bStep == 4)
		{
			if (pEventInfo->bSubStep == 1)
			{
				FWarpMap f((510*100), (60*100), pMap->GetMapIndex());
				pMap->GetMapSectree()->for_each(f);

				pEventInfo->bSubStep = 2;
				return PASSES_PER_SEC(5);
			}
			if (pEventInfo->bSubStep == 2)
			{
				pMap->Spawn(8075, iSerpentStoneCoord[iSortStoneCoord[0][pMap->GetSubElementSort()]][0], iSerpentStoneCoord[iSortStoneCoord[0][pMap->GetSubElementSort()]][1], 0);
				pEventInfo->bSubStep = 3;
				return PASSES_PER_SEC(5);
			}

			if (pEventInfo->bSubStep == 3)
			{
				pMap->Spawn(8075, iSerpentStoneCoord[iSortStoneCoord[1][pMap->GetSubElementSort()]][0], iSerpentStoneCoord[iSortStoneCoord[1][pMap->GetSubElementSort()]][1], 0);
				pEventInfo->bSubStep = 4;
				return PASSES_PER_SEC(5);
			}
			if (pEventInfo->bSubStep == 4)
			{
				pMap->Spawn(8075, iSerpentStoneCoord[iSortStoneCoord[2][pMap->GetSubElementSort()]][0], iSerpentStoneCoord[iSortStoneCoord[2][pMap->GetSubElementSort()]][1], 0);
				pEventInfo->bSubStep = 5;
				return PASSES_PER_SEC(5);
			}
			if (pEventInfo->bSubStep == 5)
			{
				pMap->Spawn(8075, iSerpentStoneCoord[iSortStoneCoord[3][pMap->GetSubElementSort()]][0], iSerpentStoneCoord[iSortStoneCoord[3][pMap->GetSubElementSort()]][1], 0);
				pEventInfo->bSubStep = 6;
				return PASSES_PER_SEC(5);
			}
			if (pEventInfo->bSubStep == 6)
			{
				pMap->Spawn(8075, iSerpentStoneCoord[iSortStoneCoord[4][pMap->GetSubElementSort()]][0], iSerpentStoneCoord[iSortStoneCoord[4][pMap->GetSubElementSort()]][1], 0);
				pEventInfo->bSubStep = 7;
				return PASSES_PER_SEC(5);
			}
			if (pEventInfo->bSubStep == 7)
			{
				pMap->Spawn(8075, iSerpentStoneCoord[iSortStoneCoord[5][pMap->GetSubElementSort()]][0], iSerpentStoneCoord[iSortStoneCoord[5][pMap->GetSubElementSort()]][1], 0);
				pEventInfo->bSubStep = 8;
				return 0;
			}
			if (pEventInfo->bSubStep == 8)
			{
				for(BYTE b = 0; b < 15; b++)
					pMap->Spawn((DWORD)(FireMonsterFloor1[b][0]), FireMonsterFloor1[b][1]+511, FireMonsterFloor1[b][2], 0);
				for(BYTE b = 0; b < 15; b++)
					pMap->Spawn((DWORD)(IceMonsterFloor1[b][0]), IceMonsterFloor1[b][1]+511, IceMonsterFloor1[b][2], 0);
				for(BYTE b = 0; b < 15; b++)
					pMap->Spawn((DWORD)(EarthMonsterFloor1[b][0]), EarthMonsterFloor1[b][1]+511, EarthMonsterFloor1[b][2], 0);
				for(BYTE b = 0; b < 15; b++)
					pMap->Spawn((DWORD)(WindMonsterFloor1[b][0]), WindMonsterFloor1[b][1]+511, WindMonsterFloor1[b][2], 0);

				return 0;
			}
			if (pEventInfo->bSubStep == 9)
			{
				pMap->Spawn(6755, 605, 92, 1);
			}
			return 0;
		}
		if (pEventInfo->bStep == 5)
		{
			if (pEventInfo->bSubStep == 1)
			{
				FWarpMap f((507*100), (310*100), pMap->GetMapIndex());
				pMap->GetMapSectree()->for_each(f);

				pEventInfo->bSubStep = 2;
				pMap->SetSnakePilar1(pMap->Spawn((DWORD)(PILAR_STEP_4), iSerpentStoneCoord[iSortStoneCoord[0][pMap->GetSubElementSort()]][0], iSerpentStoneCoord[iSortStoneCoord[0][pMap->GetSubElementSort()]][1]+255, 0));								
				return PASSES_PER_SEC(1);
			}
			if (pEventInfo->bSubStep == 2)
			{
				pMap->SetSnakePilar2(pMap->Spawn((DWORD)(PILAR_STEP_4), iSerpentStoneCoord[iSortStoneCoord[1][pMap->GetSubElementSort()]][0], iSerpentStoneCoord[iSortStoneCoord[1][pMap->GetSubElementSort()]][1]+255, 0));
				pEventInfo->bSubStep = 3;
				return PASSES_PER_SEC(1);
			}
			if (pEventInfo->bSubStep == 3)
			{
				pMap->SetSnakePilar3(pMap->Spawn((DWORD)(PILAR_STEP_4), iSerpentStoneCoord[iSortStoneCoord[2][pMap->GetSubElementSort()]][0], iSerpentStoneCoord[iSortStoneCoord[2][pMap->GetSubElementSort()]][1]+255, 0));
				pEventInfo->bSubStep = 4;
				return PASSES_PER_SEC(1);
			}
			if (pEventInfo->bSubStep == 4)
			{
				pMap->SetSnakePilar4(pMap->Spawn((DWORD)(PILAR_STEP_4), iSerpentStoneCoord[iSortStoneCoord[3][pMap->GetSubElementSort()]][0], iSerpentStoneCoord[iSortStoneCoord[3][pMap->GetSubElementSort()]][1]+255, 0));
				pEventInfo->bSubStep = 5;
				return PASSES_PER_SEC(1);
			}
			if (pEventInfo->bSubStep == 5)
			{
				pMap->SetSnakePilar5(pMap->Spawn((DWORD)(PILAR_STEP_4), iSerpentStoneCoord[iSortStoneCoord[4][pMap->GetSubElementSort()]][0], iSerpentStoneCoord[iSortStoneCoord[4][pMap->GetSubElementSort()]][1]+255, 0));
				pEventInfo->bSubStep = 6;
				return PASSES_PER_SEC(1);
			}
			if (pEventInfo->bSubStep == 6)
			{
				pMap->SetSnakePilar6(pMap->Spawn((DWORD)(PILAR_STEP_4), iSerpentStoneCoord[iSortStoneCoord[5][pMap->GetSubElementSort()]][0], iSerpentStoneCoord[iSortStoneCoord[5][pMap->GetSubElementSort()]][1]+255, 0));
				pEventInfo->bSubStep = 7;
			}
			if (pEventInfo->bSubStep == 7)
			{
				if((SECTREE_MANAGER::instance().GetMonsterCountInMap(pMap->GetMapIndex(), 6750) < 1) && 
				(SECTREE_MANAGER::instance().GetMonsterCountInMap(pMap->GetMapIndex(), 6751) < 1))
				{
					for(BYTE bCount = 0; bCount < 2; bCount++)
					{
						pMap->Spawn(6750, 642, 435, 8);
						pMap->Spawn(6750, 629, 429, 8);
						pMap->Spawn(6751, 635, 420, 8);
						pMap->Spawn(6751, 638, 420, 8);
						pMap->Spawn(6751, 635, 435, 8);
						pMap->Spawn(6750, 629, 435, 8);
						pMap->Spawn(6751, 642, 425, 8);
						pMap->Spawn(6750, 635, 435, 8);
						pMap->Spawn(6751, 642, 320, 8);
						pMap->Spawn(6751, 629, 333, 8);
						pMap->Spawn(6750, 635, 342, 8);
						pMap->Spawn(6751, 638, 346, 8);
						pMap->Spawn(6750, 635, 325, 8);
						pMap->Spawn(6751, 629, 336, 8);
						pMap->Spawn(6750, 642, 326, 8);
						pMap->Spawn(6751, 635, 340, 8);
					}
				}
			}
		}
		if (pEventInfo->bStep == 6)
		{
			if (pEventInfo->bSubStep == 1)
			{
				FWarpMap f((253*100), (188*100), pMap->GetMapIndex());
				pMap->GetMapSectree()->for_each(f);

				pMap->SetSnakeBlackSmith1(pMap->Spawn((DWORD)(BLACKSMITH_5), 418, 348, 8));
				pMap->SetSnakeBlackSmith2(pMap->Spawn((DWORD)(BLACKSMITH_5), 418, 418, 8));
				pMap->SetSnakeBlackSmith3(pMap->Spawn((DWORD)(BLACKSMITH_5), 348, 418, 8));
				pMap->SetSnakeBlackSmith4(pMap->Spawn((DWORD)(BLACKSMITH_5), 348, 348, 8));

				pEventInfo->bSubStep = 2;
			}
			if (pEventInfo->bSubStep == 2)
			{
				if ((SECTREE_MANAGER::instance().GetMonsterCountInMap(pMap->GetMapIndex(), 6750) < 1) && 
				(SECTREE_MANAGER::instance().GetMonsterCountInMap(pMap->GetMapIndex(), 6753) < 1) &&
				(SECTREE_MANAGER::instance().GetMonsterCountInMap(pMap->GetMapIndex(), 6754) < 1))
				{
					pMap->Spawn(6750, 353, 388, 8);
					pMap->Spawn(6753, 351, 387, 8);
					pMap->Spawn(6754, 349, 381, 8);
					pMap->Spawn(6754, 354, 378, 8);
					pMap->Spawn(6753, 348, 378, 8);
					pMap->Spawn(6750, 347, 375, 8);
					pMap->Spawn(6750, 358, 388, 8);
					pMap->Spawn(6753, 354, 393, 8);
					pMap->Spawn(6754, 344, 387, 8);
					pMap->Spawn(6753, 338, 375, 8);

					pMap->Spawn(6750, 409, 379, 8);
					pMap->Spawn(6753, 418, 379, 8);
					pMap->Spawn(6754, 421, 384, 8);
					pMap->Spawn(6754, 413, 386, 8);
					pMap->Spawn(6753, 406, 388, 8);
					pMap->Spawn(6750, 406, 378, 8);
					pMap->Spawn(6750, 414, 390, 8);
					pMap->Spawn(6753, 423, 393, 8);
					pMap->Spawn(6754, 431, 387, 8);
					pMap->Spawn(6753, 425, 403, 8);	
				}
			}
			if (pEventInfo->bSubStep == 3)
			{
				pMap->SetBlackSmithSerpent(pMap->Spawn((DWORD)(VNUM_BLACKSMITH_SERPENT), 383, 438, 8));
				return 0;
			}
		}

		if (pEventInfo->bStep == 7)
		{
			if (pEventInfo->bSubStep == 1)
			{
				FWarpMap f((-200), (560*100), pMap->GetMapIndex());
				pMap->GetMapSectree()->for_each(f);

				if (pMap->GetElementSort() == 0)
				{
					pMap->SetSnakeStatue1(pMap->Spawn((DWORD)(SNAKE_STATUE1), 152, 664, 1));
					pMap->SetSnakeStatue2(pMap->Spawn((DWORD)(SNAKE_STATUE2), 102, 664, 1));
					pMap->SetSnakeStatue3(pMap->Spawn((DWORD)(SNAKE_STATUE3), 153, 614, 1));
					pMap->SetSnakeStatue4(pMap->Spawn((DWORD)(SNAKE_STATUE4), 102, 614, 1));
				}
				else if (pMap->GetElementSort() == 1)
				{
					pMap->SetSnakeStatue1(pMap->Spawn((DWORD)(SNAKE_STATUE2), 152, 664, 1));
					pMap->SetSnakeStatue2(pMap->Spawn((DWORD)(SNAKE_STATUE4), 102, 664, 1));
					pMap->SetSnakeStatue3(pMap->Spawn((DWORD)(SNAKE_STATUE1), 153, 614, 1));
					pMap->SetSnakeStatue4(pMap->Spawn((DWORD)(SNAKE_STATUE3), 102, 614, 1));
				}
				else if (pMap->GetElementSort() == 2)
				{
					pMap->SetSnakeStatue1(pMap->Spawn((DWORD)(SNAKE_STATUE4), 152, 664, 1));
					pMap->SetSnakeStatue2(pMap->Spawn((DWORD)(SNAKE_STATUE1), 102, 664, 1));
					pMap->SetSnakeStatue3(pMap->Spawn((DWORD)(SNAKE_STATUE3), 153, 614, 1));
					pMap->SetSnakeStatue4(pMap->Spawn((DWORD)(SNAKE_STATUE2), 102, 614, 1));
				}
				else if (pMap->GetElementSort() == 3)
				{
					pMap->SetSnakeStatue1(pMap->Spawn((DWORD)(SNAKE_STATUE3), 152, 664, 1));
					pMap->SetSnakeStatue2(pMap->Spawn((DWORD)(SNAKE_STATUE4), 102, 664, 1));
					pMap->SetSnakeStatue3(pMap->Spawn((DWORD)(SNAKE_STATUE2), 153, 614, 1));
					pMap->SetSnakeStatue4(pMap->Spawn((DWORD)(SNAKE_STATUE1), 102, 614, 1));
				}

				pMap->SetRotationStatuesCount(0);

				pEventInfo->bSubStep = 2;
			}

			if (pEventInfo->bSubStep == 2)
			{
				if ((SECTREE_MANAGER::instance().GetMonsterCountInMap(pMap->GetMapIndex(), 6850) < 1) && 
					(SECTREE_MANAGER::instance().GetMonsterCountInMap(pMap->GetMapIndex(), 6851) < 1) &&
					(SECTREE_MANAGER::instance().GetMonsterCountInMap(pMap->GetMapIndex(), 6852) < 1) &&
					(SECTREE_MANAGER::instance().GetMonsterCountInMap(pMap->GetMapIndex(), 6853) < 1) &&
					(SECTREE_MANAGER::instance().GetMonsterCountInMap(pMap->GetMapIndex(), 6854) < 1))
				{
					for(BYTE b = 0; b < 28; b++)
					{
						pMap->Spawn(iSerpentFinalBoss[b][0], iSerpentFinalBoss[b][1], iSerpentFinalBoss[b][2], 1);
					}
				}
			}

			if (pEventInfo->bSubStep == 3)
			{
				FPurgeSectree f;
				pMap->GetMapSectree()->for_each(f);

				pMap->Spawn(6756, 128, 640, 1);
				return 0;
			}

			if (pEventInfo->bSubStep == 4)
			{
				pMap->EndDungeonWarp();
				return 0;
			}
		}
		return PASSES_PER_SEC(5);
	}

	EVENTINFO(r_snakelimit_info)
	{
		CSnkMap*	pMap;
	};
	
	EVENTFUNC(r_snakelimit_event)
	{
		r_snakelimit_info* pEventInfo = dynamic_cast<r_snakelimit_info*>(event->info);
		if (pEventInfo)
		{
			CSnkMap* pMap = pEventInfo->pMap;
			if (pMap)
			{
				pMap->EndDungeonWarp();
			}
		}
		
		return 0;
	}

	EVENTINFO(r_snakeskill_info)
	{
		CSnkMap*	pMap;
	};
	
	EVENTFUNC(r_snakeskill_event)
	{
		r_snakeskill_info* pEventInfo = dynamic_cast<r_snakeskill_info*>(event->info);
		if (pEventInfo)
		{
			CSnkMap* pMap = pEventInfo->pMap;
			if (pMap)
			{
				FSkillQueenNethis f;
				pMap->GetMapSectree()->for_each(f);
			}
		}
		
		return PASSES_PER_SEC(25);
	}

	CSnkMap::CSnkMap(long lMapIndex)
	{
		if (e_SpawnEvent != NULL)
			event_cancel(&e_SpawnEvent);
		e_SpawnEvent = NULL;

		if (e_pEndEvent != NULL)
			event_cancel(&e_pEndEvent);
		e_pEndEvent = NULL;

		if (e_pSkillEvent != NULL)
			event_cancel(&e_pSkillEvent);
		e_pSkillEvent = NULL;

#ifdef ENABLE_CONQUEROR_LEVEL
		m_vec_mapSungmaAffectQueen.clear();
		LoadSungmaAttr();
#endif
		SetDungeonStep(1);
		SetMapIndex(lMapIndex);
		SetPortalNPC(NULL);

		SetSnakeStatue1(NULL);
		SetSnakeStatue2(NULL);
		SetSnakeStatue3(NULL);
		SetSnakeStatue4(NULL);

		SetMapSectree(SECTREE_MANAGER::instance().GetMap(map_index));
		SetElementSort(0);
		SetSubElementSort(0);
		SetKillCountPilars(0);
		SetKillCountStones(0);
		pPilar1 = NULL, pPilar2 = NULL, pPilar3 = NULL, pPilar4 = NULL;
		pkBlackSmithSerpent = NULL;

		SetBlockSnake1(false);
		SetBlockSnake2(false);
		SetBlockSnake3(false);
		SetBlockSnake4(false);

		Start();
	}

	CSnkMap::~CSnkMap()
	{
		if (e_SpawnEvent != NULL)
			event_cancel(&e_SpawnEvent);
		e_SpawnEvent = NULL;

		if (e_pEndEvent != NULL)
			event_cancel(&e_pEndEvent);
		e_pEndEvent = NULL;

		if (e_pSkillEvent != NULL)
			event_cancel(&e_pSkillEvent);
		e_pSkillEvent = NULL;
	}

	void CSnkMap::Destroy()
	{
		if (e_SpawnEvent != NULL)
			event_cancel(&e_SpawnEvent);
		e_SpawnEvent = NULL;

		if (e_pEndEvent != NULL)
			event_cancel(&e_pEndEvent);
		e_pEndEvent = NULL;

		if (e_pSkillEvent != NULL)
			event_cancel(&e_pSkillEvent);
		e_pSkillEvent = NULL;

#ifdef ENABLE_CONQUEROR_LEVEL
		m_vec_mapSungmaAffectQueen.clear();
#endif
		SetDungeonStep(1);
		SetMapIndex(0);

		SetPortalNPC(NULL);
		SetParty(NULL);

		SetSnakeStatue1(NULL);
		SetSnakeStatue2(NULL);
		SetSnakeStatue3(NULL);
		SetSnakeStatue4(NULL);

		SetMapSectree(NULL);
		SetElementSort(0);
		SetSubElementSort(0);
		SetKillCountPilars(0);
		SetKillCountStones(0);
		pPilar1 = NULL, pPilar2 = NULL, pPilar3 = NULL, pPilar4 = NULL;
		pkBlackSmithSerpent = NULL;
	}

	void CSnkMap::StartDungeon(LPCHARACTER pkChar)
	{
		LPPARTY pParty = pkChar->GetParty();

		if (!pParty)
			return;

		SetParty(pParty);

		if (!GetParty())
			return;

		if(GetParty()->GetLeaderPID() != pkChar->GetPlayerID())
		{
			pkChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("Only the party leader can start the dungeon."));
			return;
		}

		char szNotice[512];
		snprintf(szNotice, sizeof(szNotice), LC_STRING("The dungeon will be available for %d minutes."), DWORD(TIME_LIMIT_DUNGEON)/60);
		SendNoticeMap(szNotice, pkChar->GetMapIndex(), true);

		pkChar->SetQuestNPCID(0);

		GetPortalNPC()->Dead();
		SetPortalNPC(NULL);

		int iElementSort = number(0,3);
		SetElementSort(iElementSort);
		SetSubElementSort(iElementSort);
		SetDungeonStep(2);
	}

	void CSnkMap::SetDungeonStep(BYTE bStep)
	{
		dungeon_step = bStep;

		if (e_SpawnEvent != NULL)
			event_cancel(&e_SpawnEvent);
		e_SpawnEvent = NULL;

		if (bStep == 2)
		{
			char szNotice[512];
			snprintf(szNotice, sizeof(szNotice), LC_STRING("Defeat all the monsters."));
			SendNoticeMap(szNotice, GetMapIndex(), true);

			SetKillCountMonsters(0);

			r_snakespawn_info* pEventInfo = AllocEventInfo<r_snakespawn_info>();
			pEventInfo->pMap = this;
			pEventInfo->bStep = bStep;
			pEventInfo->bSubStep = 1;
			e_SpawnEvent = event_create(r_snakespawn_event, pEventInfo, PASSES_PER_SEC(1));

			if (e_pSkillEvent != NULL)
				event_cancel(&e_pSkillEvent);
			e_pSkillEvent = NULL;

			r_snakeskill_info* pEventInfo_ = AllocEventInfo<r_snakeskill_info>();
			pEventInfo_->pMap = this;
			e_pSkillEvent = event_create(r_snakeskill_event, pEventInfo_, PASSES_PER_SEC(2));
			

		}
		if (bStep == 3)
		{
			char szNotice[512];
			snprintf(szNotice, sizeof(szNotice), LC_STRING("All members of the group advance to the next room."));
			SendNoticeMap(szNotice, GetMapIndex(), true);

			r_snakespawn_info* pEventInfo = AllocEventInfo<r_snakespawn_info>();
			pEventInfo->pMap = this;
			pEventInfo->bStep = bStep;
			pEventInfo->bSubStep = 1;
			e_SpawnEvent = event_create(r_snakespawn_event, pEventInfo, PASSES_PER_SEC(5));
		}
		if (bStep == 4)
		{
			char szNotice[512];
			snprintf(szNotice, sizeof(szNotice), LC_STRING("All members of the group advance to the next room."));
			SendNoticeMap(szNotice, GetMapIndex(), true);

			r_snakespawn_info* pEventInfo = AllocEventInfo<r_snakespawn_info>();
			pEventInfo->pMap = this;
			pEventInfo->bStep = bStep;
			pEventInfo->bSubStep = 1;
			e_SpawnEvent = event_create(r_snakespawn_event, pEventInfo, PASSES_PER_SEC(5));

		}
		if (bStep == 5)
		{
			char szNotice[512];
			snprintf(szNotice, sizeof(szNotice), LC_STRING("All members of the group advance to the next room."));
			SendNoticeMap(szNotice, GetMapIndex(), true);

			r_snakespawn_info* pEventInfo = AllocEventInfo<r_snakespawn_info>();
			pEventInfo->pMap = this;
			pEventInfo->bStep = bStep;
			pEventInfo->bSubStep = 1;
			e_SpawnEvent = event_create(r_snakespawn_event, pEventInfo, PASSES_PER_SEC(5));

		}
		if (bStep == 6)
		{
			char szNotice[512];
			snprintf(szNotice, sizeof(szNotice), LC_STRING("All members of the group advance to the next room."));
			SendNoticeMap(szNotice, GetMapIndex(), true);

			r_snakespawn_info* pEventInfo = AllocEventInfo<r_snakespawn_info>();
			pEventInfo->pMap = this;
			pEventInfo->bStep = bStep;
			pEventInfo->bSubStep = 1;
			e_SpawnEvent = event_create(r_snakespawn_event, pEventInfo, PASSES_PER_SEC(5));
		}

		if (bStep == 7)
		{
			FPurgeSectree f;
			GetMapSectree()->for_each(f);

			SetBlackSmithSerpent(NULL);

			char szNotice[512];
			snprintf(szNotice, sizeof(szNotice), LC_STRING("All members of the group advance to the next room."));
			SendNoticeMap(szNotice, GetMapIndex(), true);

			r_snakespawn_info* pEventInfo = AllocEventInfo<r_snakespawn_info>();
			pEventInfo->pMap = this;
			pEventInfo->bStep = bStep;
			pEventInfo->bSubStep = 1;
			e_SpawnEvent = event_create(r_snakespawn_event, pEventInfo, PASSES_PER_SEC(5));
		}

	}
	void CSnkMap::Start()
	{
		if (!GetMapSectree())
			EndDungeonWarp();
		else
		{
			SetPortalNPC(Spawn((DWORD)(PORTAL_VNUM), 126, 105, 8));

			if (e_pEndEvent != NULL)
				event_cancel(&e_pEndEvent);
			e_pEndEvent = NULL;

			r_snakelimit_info* pEventInfo = AllocEventInfo<r_snakelimit_info>();
			pEventInfo->pMap = this;
			e_pEndEvent = event_create(r_snakelimit_event, pEventInfo, PASSES_PER_SEC(3600));
		}

	}

	void CSnkMap::OnKill(LPCHARACTER pkMonster, LPCHARACTER pKiller)
	{
		BYTE bStep = dungeon_step;

		if (!bStep)
			return;

		if (!GetMapSectree())
			return;

		if (((bStep == 2) && (pkMonster->GetMobTable().dwVnum >= 6850 && pkMonster->GetMobTable().dwVnum <= 6854)))
		{
			SetKillCountMonsters(GetKillCountMonsters() + 1);
			if(GetKillCountMonsters() >= 60)
			{
				SetKillCountMonsters(0);
				FPurgeSectree f;
				GetMapSectree()->for_each(f);
				SetDungeonStep(3);
			}
		}
		if ((bStep == 3) && (pkMonster->GetMobTable().dwVnum == 8068 || pkMonster->GetMobTable().dwVnum == 8069 || pkMonster->GetMobTable().dwVnum == 8070 || pkMonster->GetMobTable().dwVnum == 8071))
		{
			if(stoneKillSort[GetElementSort()][GetKillCountMonsters()] == pkMonster->GetMobTable().dwVnum)
			{
				SetKillCountMonsters(GetKillCountMonsters() + 1);
				if((4 - GetKillCountMonsters()) != 0)
				{
					char szNotice[512];
					snprintf(szNotice, sizeof(szNotice), LC_STRING("Destroyed one correct Metin, destroy %d more"), (4 - GetKillCountMonsters()));
					SendNoticeMap(szNotice, GetMapIndex(), true);
				}
			}
			else
			{
				char szNotice[512];
				snprintf(szNotice, sizeof(szNotice), LC_STRING("Wrong Metin order, this Floor will reset."));
				SendNoticeMap(szNotice, GetMapIndex(), true);
				
				FPurgeSectree f;
				GetMapSectree()->for_each(f);
				SetKillCountMonsters(0);

				if (e_SpawnEvent != NULL)
					event_cancel(&e_SpawnEvent);
				e_SpawnEvent = NULL;

				r_snakespawn_info* pEventInfo = AllocEventInfo<r_snakespawn_info>();
				pEventInfo->pMap = this;
				pEventInfo->bStep = bStep;
				pEventInfo->bSubStep = 2;
				e_SpawnEvent = event_create(r_snakespawn_event, pEventInfo, PASSES_PER_SEC(1));


			}
			if(GetKillCountMonsters() >= 4)
			{
				SetKillCountMonsters(0);
				SetDungeonStep(4);
			}
		}

		if (bStep == 4)
		{
			if(pkMonster->GetMobTable().dwVnum == 8075)
			{
				SetKillCountStones(GetKillCountStones() + 1);
				if(GetKillCountStones() >= 6)
				{
					char szNotice[512];
					snprintf(szNotice, sizeof(szNotice), LC_STRING("Defeat the Boss group."));
					SendNoticeMap(szNotice, GetMapIndex(), true);

					SetKillCountMonsters(0);
					SetKillCountStones(0);

					if (e_SpawnEvent != NULL)
						event_cancel(&e_SpawnEvent);
					e_SpawnEvent = NULL;

					r_snakespawn_info* pEventInfo = AllocEventInfo<r_snakespawn_info>();
					pEventInfo->pMap = this;
					pEventInfo->bStep = bStep;
					pEventInfo->bSubStep = 8;
					e_SpawnEvent = event_create(r_snakespawn_event, pEventInfo, PASSES_PER_SEC(1));
				}
				return;
			}
			else if(pkMonster->GetMobTable().dwVnum >= 6850 && pkMonster->GetMobTable().dwVnum <= 6854)
			{
				SetKillCountMonsters(GetKillCountMonsters() + 1);
				if(GetKillCountMonsters() >= 60)
				{
					char szNotice[512];
					snprintf(szNotice, sizeof(szNotice), LC_STRING("Defeat the Boss."));
					SendNoticeMap(szNotice, GetMapIndex(), true);
					SetKillCountMonsters(0);
					SetKillCountStones(0);

					if (e_SpawnEvent != NULL)
						event_cancel(&e_SpawnEvent);
					e_SpawnEvent = NULL;

					FPurgeSectree f;
					GetMapSectree()->for_each(f);

					r_snakespawn_info* pEventInfo = AllocEventInfo<r_snakespawn_info>();
					pEventInfo->pMap = this;
					pEventInfo->bStep = bStep;
					pEventInfo->bSubStep = 9;
					e_SpawnEvent = event_create(r_snakespawn_event, pEventInfo, PASSES_PER_SEC(1));
				}
				return;
			}
			else if (pkMonster->GetMobTable().dwVnum == 6755)
			{
				FPurgeSectree f;
				GetMapSectree()->for_each(f);
				SetKillCountMonsters(0);
				SetKillCountStones(0);
				int iChangeElement = number(0,1);
				if (iChangeElement)
				{
					if (e_SpawnEvent != NULL)
						event_cancel(&e_SpawnEvent);
					e_SpawnEvent = NULL;

					if (GetElementSort() == 3)
						SetElementSort(0);
					else
						SetElementSort(GetElementSort()+1);

					r_snakespawn_info* pEventInfo = AllocEventInfo<r_snakespawn_info>();
					pEventInfo->pMap = this;
					pEventInfo->bStep = bStep;
					pEventInfo->bSubStep = 2;
					e_SpawnEvent = event_create(r_snakespawn_event, pEventInfo, PASSES_PER_SEC(1));

					return;
				}
				SetDungeonStep(5);
				return;
			}
		}

		if (bStep == 5)
		{
			if (pkMonster->GetMobTable().dwVnum == 6750 || pkMonster->GetMobTable().dwVnum == 6751)
			{
				if(!pKiller)
					return;

				int iChance = number(1, 100);
				if (iChance < 5)
				{
					LPITEM pkItem = ITEM_MANAGER::instance().CreateItem(VNUM_KILL_PILAR);
					if (!pkItem)
						return;

					PIXEL_POSITION mPos;
					mPos.x = pkMonster->GetX();
					mPos.y = pkMonster->GetY();

					pkItem->AddToGround(GetMapIndex(), mPos);
					pkItem->StartDestroyEvent();
					pkItem->SetOwnership(pKiller, 60);
				}
			}
		}

		if (bStep == 6)
		{
			if (pkMonster->GetMobTable().dwVnum == 6750 || pkMonster->GetMobTable().dwVnum == 6753 || pkMonster->GetMobTable().dwVnum == 6754)
			{
				if(!pKiller)
					return;

				int iChance = number(1, 100);
				if (iChance < 5)
				{
					LPITEM pkItem = ITEM_MANAGER::instance().CreateItem(VNUM_KILL_BLACKSMITH);
					if (!pkItem)
						return;

					PIXEL_POSITION mPos;
					mPos.x = pkMonster->GetX();
					mPos.y = pkMonster->GetY();

					pkItem->AddToGround(GetMapIndex(), mPos);
					pkItem->StartDestroyEvent();
					pkItem->SetOwnership(pKiller, 60);
				}
			}
		}

		if (bStep == 7)
		{
			if (pkMonster->GetMobTable().dwVnum >= 6850 && pkMonster->GetMobTable().dwVnum <= 6854)
			{
				if(!pKiller)
					return;

				int iChance = number(1, 100);
				if (iChance < 5)
				{
					int iDropItem = number(VNUM_ITEM_SNAKE_FIRE, VNUM_ITEM_SNAKE_GROUND);
					LPITEM pkItem = ITEM_MANAGER::instance().CreateItem(iDropItem);
					if (!pkItem)
						return;

					PIXEL_POSITION mPos;
					mPos.x = pkMonster->GetX();
					mPos.y = pkMonster->GetY();

					pkItem->AddToGround(GetMapIndex(), mPos);
					pkItem->StartDestroyEvent();
					pkItem->SetOwnership(pKiller, 60);
				}
				return;
			}
			if (pkMonster->GetMobTable().dwVnum == 6756)
			{
				char szNotice[512];
				snprintf(szNotice, sizeof(szNotice), LC_STRING("All partecipants will be teleport in 60 seconds."));
				SendNoticeMap(szNotice, GetMapIndex(), true);

				if (e_SpawnEvent != NULL)
					event_cancel(&e_SpawnEvent);
				e_SpawnEvent = NULL;

				FPurgeSectree f;
				GetMapSectree()->for_each(f);

				if (e_pSkillEvent != NULL)
					event_cancel(&e_pSkillEvent);
				e_pSkillEvent = NULL;

				r_snakespawn_info* pEventInfo = AllocEventInfo<r_snakespawn_info>();
				pEventInfo->pMap = this;
				pEventInfo->bStep = GetDungeonStep();
				pEventInfo->bSubStep = 4;
				e_SpawnEvent = event_create(r_snakespawn_event, pEventInfo, PASSES_PER_SEC(60));


				return;
			}
		}
	}

	void CSnkMap::EndDungeonWarp()
	{
		if (GetMapSectree())
		{
			FExitAndGoTo f;
			GetMapSectree()->for_each(f);
		}

		long lMapIndex = GetMapIndex();
		SECTREE_MANAGER::instance().DestroyPrivateMap(GetMapIndex());
		Destroy();
		CSnk::instance().Remove(lMapIndex);
		M2_DELETE(this);

	}

	void CSnkMap::OnKillBlackSmith(LPITEM pkItem, LPCHARACTER pkChar, LPCHARACTER pkBlackSmith)
	{
		if ((!pkItem) || (!pkChar) || (!pkBlackSmith))
			return;

		if (!GetMapSectree())
			return;

		if(GetSnakeBLackSmith1())
			if(GetSnakeBLackSmith1()->GetVID() == pkBlackSmith->GetVID())
				GetSnakeBLackSmith1()->Dead();
		if(GetSnakeBLackSmith2())
			if(GetSnakeBLackSmith2()->GetVID() == pkBlackSmith->GetVID())
				GetSnakeBLackSmith2()->Dead();
		if(GetSnakeBLackSmith3())
			if(GetSnakeBLackSmith3()->GetVID() == pkBlackSmith->GetVID())
				GetSnakeBLackSmith3()->Dead();
		if(GetSnakeBLackSmith4())
			if(GetSnakeBLackSmith4()->GetVID() == pkBlackSmith->GetVID())
				GetSnakeBLackSmith4()->Dead();

		ITEM_MANAGER::instance().RemoveItem(pkItem);

		SetKillCountPilars(GetKillCountPilars()+1);
		if((4-GetKillCountPilars()) != 0)
		{
			char szNotice[512];
			snprintf(szNotice, sizeof(szNotice), LC_STRING("A pillar has been destroyed, destroy %d more pillars."), 4-GetKillCountPilars());
			SendNoticeMap(szNotice, GetMapIndex(), true);
		}
		
		if(GetKillCountPilars() >= 4)
		{
			if (e_SpawnEvent != NULL)
				event_cancel(&e_SpawnEvent);
			e_SpawnEvent = NULL;

			FPurgeSectree f;
			GetMapSectree()->for_each(f);

			char szNotice[512];
			snprintf(szNotice, sizeof(szNotice), LC_STRING("Go with the Serpent BlackSmith."));
			SendNoticeMap(szNotice, GetMapIndex(), true);

			r_snakespawn_info* pEventInfo = AllocEventInfo<r_snakespawn_info>();
			pEventInfo->pMap = this;
			pEventInfo->bStep = GetDungeonStep();
			pEventInfo->bSubStep = 3;
			e_SpawnEvent = event_create(r_snakespawn_event, pEventInfo, PASSES_PER_SEC(1));
		}
	}

	void CSnkMap::OnKillPilar(LPITEM pkItem, LPCHARACTER pkChar, LPCHARACTER pkPilar)
	{
		if ((!pkItem) || (!pkChar) || (!pkPilar))
			return;

		if (!GetMapSectree())
			return;

		ITEM_MANAGER::instance().RemoveItem(pkItem);
		
		if(GetSnakePilar1())
		{
			if(GetSnakePilar1()->GetVID() == pkPilar->GetVID())
			{
				GetSnakePilar1()->Dead();
				SetKillCountPilars(GetKillCountPilars()+1);
			}
		}
		if(GetSnakePilar2())
		{
			if(GetSnakePilar2()->GetVID() == pkPilar->GetVID())
			{
				if (GetKillCountPilars() < 1 || GetKillCountPilars() > 1)
				{
					char szNotice[512];
					snprintf(szNotice, sizeof(szNotice), LC_STRING("Wrong order on kill Pilar"));
					SendNoticeMap(szNotice, GetMapIndex(), true);
					return;
				}
				GetSnakePilar2()->Dead();
				SetKillCountPilars(GetKillCountPilars()+1);
			}
		}
		if(GetSnakePilar3())
		{
			if(GetSnakePilar3()->GetVID() == pkPilar->GetVID())
			{
				if (GetKillCountPilars() < 2 || GetKillCountPilars() > 2)
				{
					char szNotice[512];
					snprintf(szNotice, sizeof(szNotice), LC_STRING("Wrong order on kill Pilar"));
					SendNoticeMap(szNotice, GetMapIndex(), true);
					return;
				}

				GetSnakePilar3()->Dead();
				SetKillCountPilars(GetKillCountPilars()+1);
			}
		}
		if(GetSnakePilar4())
		{
			if(GetSnakePilar4()->GetVID() == pkPilar->GetVID())
			{
				if (GetKillCountPilars() < 3 || GetKillCountPilars() > 3)
				{
					char szNotice[512];
					snprintf(szNotice, sizeof(szNotice), LC_TEXT("Wrong order on kill Pilar"));
					SendNoticeMap(szNotice, GetMapIndex(), true);
					return;
				}

				GetSnakePilar4()->Dead();
				SetKillCountPilars(GetKillCountPilars()+1);
			}
		}
		if(GetSnakePilar5())
		{
			if(GetSnakePilar5()->GetVID() == pkPilar->GetVID())
			{
				if (GetKillCountPilars() < 4 || GetKillCountPilars() > 4)
				{
					char szNotice[512];
					snprintf(szNotice, sizeof(szNotice), LC_STRING("Wrong order on kill Pilar"));
					SendNoticeMap(szNotice, GetMapIndex(), true);
					return;
				}

				GetSnakePilar5()->Dead();
				SetKillCountPilars(GetKillCountPilars()+1);	
			}
		}	
		if(GetSnakePilar6())
		{
			if(GetSnakePilar6()->GetVID() == pkPilar->GetVID())
			{
				if (GetKillCountPilars() < 5 || GetKillCountPilars() > 5)
				{
					char szNotice[512];
					snprintf(szNotice, sizeof(szNotice), LC_STRING("Wrong order on kill Pilar"));
					SendNoticeMap(szNotice, GetMapIndex(), true);
					return;
				}

				GetSnakePilar6()->Dead();
				SetKillCountPilars(GetKillCountPilars()+1);
			}
		}
			
		if((6-GetKillCountPilars()) != 0)
		{
			char szNotice[512];
			snprintf(szNotice, sizeof(szNotice), LC_STRING("A pillar has been destroyed, destroy %d more pillars."), 6-GetKillCountPilars());
			SendNoticeMap(szNotice, GetMapIndex(), true);
		}
		
		if(GetKillCountPilars() >= 6)
		{
			FPurgeSectree f;
			GetMapSectree()->for_each(f);
			SetKillCountPilars(0);
			SetDungeonStep(6);
		}
	}

	void CSnkMap::OnStatueSetRotation(LPITEM pkItem, LPCHARACTER pkChar, LPCHARACTER pkStatue)
	{
		if ((!pkItem) || (!pkChar) || (!pkStatue))
			return;

		if (GetDungeonStep() != 7)
			return;
			
		if (GetMapSectree())
		{
			PIXEL_POSITION pos = CSnk::instance().GetXYZ();
			if(!pos.x)
			{
				pkChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("Error!! creating the coordinates, report with Staff."));
				return;
			}

			ITEM_MANAGER::instance().RemoveItem(pkItem);

			if (GetSnakeStatue1())
			{
				if (GetSnakeStatue1()->GetVID() == pkStatue->GetVID())
				{
					if (GetBlockSnake1())
						return;

					if (GetSnakeStatue1()->GetMobTable().dwVnum == (int)(SNAKE_STATUE1))
						if (pkItem->GetVnum() != (int)(VNUM_ITEM_SNAKE_FIRE))
							return;
					if (GetSnakeStatue1()->GetMobTable().dwVnum == (int)(SNAKE_STATUE2))
						if (pkItem->GetVnum() != (int)(VNUM_ITEM_SNAKE_ICE))
							return;
					if (GetSnakeStatue1()->GetMobTable().dwVnum == (int)(SNAKE_STATUE3))
						if (pkItem->GetVnum() != (int)(VNUM_ITEM_SNAKE_WIND))
							return;
					if (GetSnakeStatue1()->GetMobTable().dwVnum == (int)(SNAKE_STATUE4))
						if (pkItem->GetVnum() != (int)(VNUM_ITEM_SNAKE_GROUND))
							return;

					pos.x = GetMapSectree()->m_setting.iBaseX + 127 * 100;
					pos.y = GetMapSectree()->m_setting.iBaseY + 640 * 100;
					pos.z = 0;
					SetRotationStatuesCount(GetRotationStatuesCount()+1);
					GetSnakeStatue1()->SetRotationToXY(pos.x, pos.y);
					GetSnakeStatue1()->SendMovePacket(FUNC_WAIT, 0, GetSnakeStatue1()->GetX(), GetSnakeStatue1()->GetY(), 0, 0);
					SetBlockSnake1(true);
				}
			}

			if (GetSnakeStatue2())
			{
				if (GetSnakeStatue2()->GetVID() == pkStatue->GetVID())
				{

					if (GetBlockSnake2())
						return;

					if (GetSnakeStatue2()->GetMobTable().dwVnum == (int)(SNAKE_STATUE1))
						if (pkItem->GetVnum() != (int)(VNUM_ITEM_SNAKE_FIRE))
							return;
					if (GetSnakeStatue2()->GetMobTable().dwVnum == (int)(SNAKE_STATUE2))
						if (pkItem->GetVnum() != (int)(VNUM_ITEM_SNAKE_ICE))
							return;
					if (GetSnakeStatue2()->GetMobTable().dwVnum == (int)(SNAKE_STATUE3))
						if (pkItem->GetVnum() != (int)(VNUM_ITEM_SNAKE_WIND))
							return;
					if (GetSnakeStatue2()->GetMobTable().dwVnum == (int)(SNAKE_STATUE4))
						if (pkItem->GetVnum() != (int)(VNUM_ITEM_SNAKE_GROUND))
							return;

					pos.x = GetMapSectree()->m_setting.iBaseX + 127 * 100;
					pos.y = GetMapSectree()->m_setting.iBaseY + 640 * 100;
					pos.z = 0;
					SetRotationStatuesCount(GetRotationStatuesCount()+1);
					GetSnakeStatue2()->SetRotationToXY(pos.x, pos.y);
					GetSnakeStatue2()->SendMovePacket(FUNC_WAIT, 0, GetSnakeStatue2()->GetX(), GetSnakeStatue2()->GetY(), 0, 0);
					SetBlockSnake2(true);
				}
			}

			if (GetSnakeStatue3())
			{
				if (GetSnakeStatue3()->GetVID() == pkStatue->GetVID())
				{
					if (GetBlockSnake3())
						return;

					if (GetSnakeStatue3()->GetMobTable().dwVnum == (int)(SNAKE_STATUE1))
						if (pkItem->GetVnum() != (int)(VNUM_ITEM_SNAKE_FIRE))
							return;
					if (GetSnakeStatue3()->GetMobTable().dwVnum == (int)(SNAKE_STATUE2))
						if (pkItem->GetVnum() != (int)(VNUM_ITEM_SNAKE_ICE))
							return;
					if (GetSnakeStatue3()->GetMobTable().dwVnum == (int)(SNAKE_STATUE3))
						if (pkItem->GetVnum() != (int)(VNUM_ITEM_SNAKE_WIND))
							return;
					if (GetSnakeStatue3()->GetMobTable().dwVnum == (int)(SNAKE_STATUE4))
						if (pkItem->GetVnum() != (int)(VNUM_ITEM_SNAKE_GROUND))
							return;

					pos.x = GetMapSectree()->m_setting.iBaseX + 127 * 100;
					pos.y = GetMapSectree()->m_setting.iBaseY + 640 * 100;
					pos.z = 0;
					SetRotationStatuesCount(GetRotationStatuesCount()+1);
					GetSnakeStatue3()->SetRotationToXY(pos.x, pos.y);
					GetSnakeStatue3()->SendMovePacket(FUNC_WAIT, 0, GetSnakeStatue3()->GetX(), GetSnakeStatue3()->GetY(), 0, 0);
					SetBlockSnake3(true);
				}
			}

			if (GetSnakeStatue4())
			{
				if (GetSnakeStatue4()->GetVID() == pkStatue->GetVID())
				{
					if (GetBlockSnake4())
						return;

					if (GetSnakeStatue4()->GetMobTable().dwVnum == (int)(SNAKE_STATUE1))
						if (pkItem->GetVnum() != (int)(VNUM_ITEM_SNAKE_FIRE))
							return;
					if (GetSnakeStatue4()->GetMobTable().dwVnum == (int)(SNAKE_STATUE2))
						if (pkItem->GetVnum() != (int)(VNUM_ITEM_SNAKE_ICE))
							return;
					if (GetSnakeStatue4()->GetMobTable().dwVnum == (int)(SNAKE_STATUE3))
						if (pkItem->GetVnum() != (int)(VNUM_ITEM_SNAKE_WIND))
							return;
					if (GetSnakeStatue4()->GetMobTable().dwVnum == (int)(SNAKE_STATUE4))
						if (pkItem->GetVnum() != (int)(VNUM_ITEM_SNAKE_GROUND))
							return;


					pos.x = GetMapSectree()->m_setting.iBaseX + 127 * 100;
					pos.y = GetMapSectree()->m_setting.iBaseY + 640 * 100;
					pos.z = 0;
					SetRotationStatuesCount(GetRotationStatuesCount()+1);
					GetSnakeStatue4()->SetRotationToXY(pos.x, pos.y);
					GetSnakeStatue4()->SendMovePacket(FUNC_WAIT, 0, GetSnakeStatue4()->GetX(), GetSnakeStatue4()->GetY(), 0, 0);
					SetBlockSnake4(true);
				}
			}

			if (GetRotationStatuesCount() >= 4)
			{
				if(GetSnakeStatue1())
					GetSnakeStatue1()->SendMovePacket(FUNC_MOB_SKILL, 0, GetSnakeStatue1()->GetX(), GetSnakeStatue1()->GetY(), 0, 0);
				if(GetSnakeStatue2())
					GetSnakeStatue2()->SendMovePacket(FUNC_MOB_SKILL, 0, GetSnakeStatue2()->GetX(), GetSnakeStatue2()->GetY(), 0, 0);
				if(GetSnakeStatue3())
					GetSnakeStatue3()->SendMovePacket(FUNC_MOB_SKILL, 0, GetSnakeStatue3()->GetX(), GetSnakeStatue3()->GetY(), 0, 0);
				if(GetSnakeStatue4())
					GetSnakeStatue4()->SendMovePacket(FUNC_MOB_SKILL, 0, GetSnakeStatue4()->GetX(), GetSnakeStatue4()->GetY(), 0, 0);

				if (e_SpawnEvent != NULL)
					event_cancel(&e_SpawnEvent);
				e_SpawnEvent = NULL;

				r_snakespawn_info* pEventInfo = AllocEventInfo<r_snakespawn_info>();
				pEventInfo->pMap = this;
				pEventInfo->bStep = GetDungeonStep();
				pEventInfo->bSubStep = 3;
				e_SpawnEvent = event_create(r_snakespawn_event, pEventInfo, PASSES_PER_SEC(5));

			}

		}
	}

	LPCHARACTER CSnkMap::Spawn(DWORD dwVnum, int iX, int iY, int iDir, bool bSpawnMotion)
	{
		if (dwVnum == 0)
			return NULL;

		if (!GetMapSectree())
			return NULL;

		LPCHARACTER pkMob = CHARACTER_MANAGER::instance().SpawnMob(dwVnum, GetMapIndex(), GetMapSectree()->m_setting.iBaseX + iX * 100, GetMapSectree()->m_setting.iBaseY + iY * 100, 0, bSpawnMotion, iDir == 0 ? -1 : (iDir - 1) * 45);
		if (pkMob)
			sys_log(0, "<SnakeLair> Spawn: %s (map index: %d). x: %d y: %d", pkMob->GetName(), GetMapIndex(), (GetMapSectree()->m_setting.iBaseX + iX * 100), (GetMapSectree()->m_setting.iBaseY + iY * 100));

		return pkMob;
	}
#ifdef ENABLE_CONQUEROR_LEVEL
	void CSnkMap::LoadSungmaAttr()
	{
		TSungmaAffectQueen aMap;

		for (BYTE by = 0; by < 4; by++)
		{
			for (BYTE bx = 1; bx < 8; bx++)
			{
				aMap.bDungeonFloor = bx;
				aMap.iAffecType = iSungmaAffect[by][0];
				aMap.iValue = iSungmaAffect[by][bx];
				m_vec_mapSungmaAffectQueen.push_back(aMap);
			}
		}
	}

	int CSnkMap::GetSungmaValueAffect(int iAffect)
	{
		if (!m_vec_mapSungmaAffectQueen.empty())
		{
			std::vector<TSungmaAffectQueen>::iterator it = m_vec_mapSungmaAffectQueen.begin();
			while (it != m_vec_mapSungmaAffectQueen.end())
			{
				TSungmaAffectQueen & rRegion = *(it++);
				if (rRegion.bDungeonFloor == GetDungeonStep())
					if(rRegion.iAffecType == iAffect)
						return rRegion.iValue;
			}
		}
	}

#endif

	void CSnk::Initialize()
	{
		SetXYZ(0, 0, 0);
		SetSubXYZ(0, 0, 0);
		m_dwRegGroups.clear();
	}

	void CSnk::Destroy()
	{

		itertype(m_dwRegGroups) iter = m_dwRegGroups.begin();
		for (; iter != m_dwRegGroups.end(); ++iter)
		{
			CSnkMap* pMap = iter->second;
			SECTREE_MANAGER::instance().DestroyPrivateMap(pMap->GetMapIndex());
			pMap->Destroy();
			M2_DELETE(pMap);
		}

		SetXYZ(0, 0, 0);
		SetSubXYZ(0, 0, 0);
		m_dwRegGroups.clear();
	}

	void CSnk::OnKill(LPCHARACTER pkMonster, LPCHARACTER pKiller)
	{
		if ((!pkMonster) || (!pKiller))
			return;

		LPPARTY pParty = pKiller->GetParty();
		
		if(!pParty)
			return;

		long lMapIndex = pKiller->GetMapIndex();

		if (lMapIndex < 1000)
			return;

		CSnkMap* pMap = m_dwRegGroups.find(lMapIndex)->second;
		if (!pMap)
			return;

		pMap->OnKill(pkMonster, pKiller);

		return;
	}

	void CSnk::OnKillPilar(LPITEM pkItem, LPCHARACTER pkChar, LPCHARACTER pkPilar)
	{
		if ((!pkItem) || (!pkChar) || (!pkPilar))
			return;

		LPPARTY pParty = pkChar->GetParty();
		if (!pParty)
			return;

		if (pkItem->GetOriginalVnum() != (DWORD)(VNUM_KILL_PILAR))
			return;

		long lMapIndex = pkChar->GetMapIndex();

		if (lMapIndex < 1000)
			return;

		CSnkMap* pMap = m_dwRegGroups.find(lMapIndex)->second;

		if (!pMap)
			return;

		pMap->OnKillPilar(pkItem, pkChar, pkPilar);

		return;
	}

	void CSnk::OnKillBlackSmith(LPITEM pkItem, LPCHARACTER pkChar, LPCHARACTER pkBlackSmith)
	{
		if ((!pkItem) || (!pkChar) || (!pkBlackSmith))
			return;

		LPPARTY pParty = pkChar->GetParty();
		if (!pParty)
			return;

		if (pkItem->GetOriginalVnum() != (DWORD)(VNUM_KILL_BLACKSMITH))
			return;

		long lMapIndex = pkChar->GetMapIndex();

		if (lMapIndex < 1000)
			return;

		CSnkMap* pMap = m_dwRegGroups.find(lMapIndex)->second;

		if (!pMap)
			return;

		pMap->OnKillBlackSmith(pkItem, pkChar, pkBlackSmith);
		
		return;
	}

	void CSnk::OnStatueSetRotation(LPITEM pkItem, LPCHARACTER pkChar, LPCHARACTER pkStatue)
	{
		if ((!pkItem) || (!pkChar) || (!pkStatue))
			return;

		LPPARTY pParty = pkChar->GetParty();
		if (!pParty)
			return;

		if (pkItem->GetOriginalVnum() < (DWORD)(SNAKE_STATUE1) && pkItem->GetOriginalVnum() > (DWORD)(SNAKE_STATUE4))
			return;

		long lMapIndex = pkChar->GetMapIndex();

		if (lMapIndex < 1000)
			return;

		CSnkMap* pMap = m_dwRegGroups.find(lMapIndex)->second;

		if (!pMap)
			return;

		pMap->OnStatueSetRotation(pkItem, pkChar, pkStatue);

		return;

	}

	void CSnk::SetDungeonStep(LPCHARACTER pkChar)
	{
		if (!pkChar)
			return;

		LPPARTY pParty = pkChar->GetParty();

		if (!pParty)
			return;

		if(pParty->GetLeaderPID() != pkChar->GetPlayerID())
		{
			pkChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("Only the party leader can create a new Room."));
			return;
		}

		long lMapIndex = pkChar->GetMapIndex();

		if (lMapIndex < 1000)
			return;

		itertype(m_dwRegGroups) iter = m_dwRegGroups.find(lMapIndex), iterEnd = m_dwRegGroups.end();
		if (iter == iterEnd)
			return;

		CSnkMap* pMap = m_dwRegGroups.find(lMapIndex)->second;
		if (pMap)
		{
			if(pMap->GetParty()->GetLeaderPID() != pkChar->GetPlayerID())
			{
				pkChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("Wow WoW Wow, you are in other Group Register."));
				return;
			}
			pMap->SetDungeonStep(7);
		}
	}

	bool CSnk::IsSnakeMap(long lMapIndex)
	{
		long lMinIndex = (long)(MAP_INDEX) * 10000, lMaxIndex = (long)(MAP_INDEX) * 10000 + 10000;
		if (((lMapIndex >= lMinIndex) && (lMapIndex <= lMaxIndex)) || (lMapIndex == (long)(MAP_INDEX)))
			return true;

		return false;
	}

	void CSnk::Remove(long lMapIndex)
	{
		itertype(m_dwRegGroups) iter = m_dwRegGroups.find(lMapIndex);

		if (iter != m_dwRegGroups.end())
		{
			m_dwRegGroups.erase(iter);
		}
		return;
	}

#ifdef ENABLE_CONQUEROR_LEVEL
	int CSnk::GetSungmaValueAffectDungeon(LPCHARACTER pkChar, int iAffect)
	{
		if (!pkChar)
			return 0;

		if (!pkChar->IsPC())
			return 0;

		LPPARTY pParty = pkChar->GetParty();

		if (!pParty)
			return 0;

		long lMapIndex = pkChar->GetMapIndex();
		
		if (lMapIndex < 1000)
			return 0;

		itertype(m_dwRegGroups) iter = m_dwRegGroups.find(lMapIndex), iterEnd = m_dwRegGroups.end();
		if (iter == iterEnd)
			return 0;

		CSnkMap* pMap = m_dwRegGroups.find(lMapIndex)->second;

		if (pMap)
			return pMap->GetSungmaValueAffect(iAffect);

		return 0;
	}
#endif

	void CSnk::LeaveParty(long lMapIndex)
	{
		Remove(lMapIndex);
		return;
	}

	void CSnk::Leave(LPCHARACTER pChar)
	{
		if (!pChar)
			return;

		PIXEL_POSITION posSub = GetSubXYZ();
		if (!posSub.x)
			pChar->WarpSet(EMPIRE_START_X(pChar->GetEmpire()), EMPIRE_START_Y(pChar->GetEmpire()));
		else
			pChar->WarpSet(posSub.x, posSub.y);

		return;
	}

	bool CSnk::Access(LPCHARACTER pChar)
	{
		if (pChar)
		{

			LPPARTY pParty = pChar->GetParty();

			if(!pParty)
			{
				pChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("You don't belong to any group."));
				return false;
			}

			long lNormalMapIndex = (long)(MAP_INDEX);
			PIXEL_POSITION pos = GetXYZ(), posSub = GetSubXYZ();
			if (!pos.x)
			{
				LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap((long)(SnakeLair::MAP_INDEX));
				if (pkSectreeMap)
					SetXYZ(pkSectreeMap->m_setting.iBaseX + 130 * 100, pkSectreeMap->m_setting.iBaseY + 130 * 100, 0);
			}
			
			if (!posSub.x)
			{
				LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap((long)(SnakeLair::SUBMAP_INDEX));
				if (pkSectreeMap)
					SetSubXYZ(pkSectreeMap->m_setting.iBaseX + 428 * 100, pkSectreeMap->m_setting.iBaseY + 364 * 100, 0);
			}

			if(pParty->GetLeaderPID() != pChar->GetPlayerID())
			{
				pChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("Only the party leader can create a new Dungeon."));
				return false;
			}

			long lMapIndex = SECTREE_MANAGER::instance().CreatePrivateMap(lNormalMapIndex);
			if (!lMapIndex)
			{
				pChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("An error occurred during map creation."));
				return false;
			}

			CSnkMap *pMap = M2_NEW CSnkMap(lMapIndex);

			if (pMap)
			{
				m_dwRegGroups.insert(std::make_pair(lMapIndex, pMap));

				PIXEL_POSITION mPos;
				if (!SECTREE_MANAGER::instance().GetRecallPositionByEmpire((int)(MAP_INDEX), 0, mPos))
				{
					pChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("Sectree Error get recall position."));
					return true;

				}

				pMap->SetDungeonStep(1);

				FPartyCHCollector f;
				pChar->GetParty()->ForEachOnMapMember(f, pChar->GetMapIndex());
				std::vector <DWORD>::iterator it;
				for (it = f.pPlayerID.begin(); it != f.pPlayerID.end(); it++)
				{
					LPCHARACTER pkChr = CHARACTER_MANAGER::instance().FindByPID(*it);
					if(pkChr)
					{
						pkChr->SaveExitLocation();
						pkChr->WarpSet(mPos.x, mPos.y, pMap->GetMapIndex());
					}
				}
			}
			return true;
		}
		return false;
	}
	void CSnk::Start(LPCHARACTER pkChar)
	{
		if (!pkChar)
			return;

		LPPARTY pParty = pkChar->GetParty();

		if(!pParty)
		{
			pkChar->ChatPacket(CHAT_TYPE_INFO, LC_STRING("Your group has disappeared. What are you doing here?"));
			return;
		}

		long lMapIndex = pkChar->GetMapIndex();

		if (lMapIndex < 1000)
			return;

		CSnkMap* pMap = m_dwRegGroups.find(lMapIndex)->second;
		if (pMap)
			pMap->StartDungeon(pkChar);
	}

};