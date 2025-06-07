#if !defined(__INC_DRAGON_SOUL_H__) && defined(__DRAGON_SOUL_SYSTEM__)
#define __INC_DRAGON_SOUL_H__

#include "../../common/length.h"

class CHARACTER;
class CItem;

class DragonSoulTable;

class DSManager : public singleton<DSManager>
{
public:
	enum
	{
		GEMSTONE = 30270,
		REWARD_BOX = 50255,
		GEMSTONE_NEED_COUNT = 10
	};

public:
	DSManager();
	~DSManager();
	bool ReadDragonSoulTableFile(const char* c_pszFileName);

	void GetDragonSoulInfo(DWORD dwVnum, OUT BYTE& bType, OUT BYTE& bGrade, OUT BYTE& bStep, OUT BYTE& bRefine) const;
	// fixme : titempos로
	WORD GetBasePosition(const LPITEM pItem) const;
	bool IsValidCellForThisItem(const LPITEM pItem, const TItemPos& Cell) const;
	int GetDuration(const LPITEM pItem) const;

	// 용혼석을 받아서 특정 용심을 추출하는 함수
	bool ExtractDragonHeart(LPCHARACTER ch, LPITEM pItem, LPITEM pExtractor = NULL);

	// 특정 용혼석(pItem)을 장비창에서 제거할 때에 성공 여부를 결정하고, 
	// 실패시 부산물을 주는 함수.(부산물은 dragon_soul_table.txt에 정의)
	// DestCell에 invalid한 값을 넣으면 성공 시, 용혼석을 빈 공간에 자동 추가.
	// 실패 시, 용혼석(pItem)은 delete됨.
	// 추출아이템이 있다면 추출 성공 확률이 pExtractor->GetValue(0)%만큼 증가함.
	// 부산물은 언제나 자동 추가.
	bool PullOut(LPCHARACTER ch, TItemPos DestCell, IN OUT LPITEM& pItem, LPITEM pExtractor = NULL);

	// 용혼석 업그레이드 함수
	bool DoRefineGrade(LPCHARACTER ch, TItemPos(&aItemPoses)[DRAGON_SOUL_REFINE_GRID_SIZE]);
	bool DoRefineStep(LPCHARACTER ch, TItemPos(&aItemPoses)[DRAGON_SOUL_REFINE_GRID_SIZE]);
	bool DoRefineStrength(LPCHARACTER ch, TItemPos(&aItemPoses)[DRAGON_SOUL_REFINE_GRID_SIZE]);
#if defined(__DS_CHANGE_ATTR__)
	bool DoChangeAttribute(LPCHARACTER lpCh, TItemPos(&arItemPos)[DRAGON_SOUL_REFINE_GRID_SIZE]);
#endif

	bool DragonSoulItemInitialize(LPITEM pItem);
	bool HasActivedAllSlotsByPage(const LPCHARACTER ch, const BYTE bPageIndex = DRAGON_SOUL_DECK_0) const;

	bool IsTimeLeftDragonSoul(LPITEM pItem) const;
	int LeftTime(LPITEM pItem) const;
	bool ActivateDragonSoul(LPITEM pItem);
	bool DeactivateDragonSoul(LPITEM pItem, bool bSkipRefreshOwnerActiveState = false);
	bool IsActiveDragonSoul(LPITEM pItem) const;

#if defined(__DS_SET__)
	float GetWeight(DWORD dwVnum);
	int GetApplyCount(DWORD dwVnum);
	int GetBasicApplyValue(DWORD dwVnum, int iType, bool bAttr = false);
	int GetAdditionalApplyValue(DWORD dwVnum, int iType, bool bAttr = false);
#endif

private:
	void SendRefineResultPacket(LPCHARACTER ch, BYTE bSubHeader, const TItemPos& pos);

	// 캐릭터의 용혼석 덱을 살펴보고, 활성화 된 용혼석이 없다면, 캐릭터의 용혼석 활성 상태를 off 시키는 함수.
	void RefreshDragonSoulState(LPCHARACTER ch);

	DWORD MakeDragonSoulVnum(BYTE bType, BYTE grade, BYTE step, BYTE refine);
	bool PutAttributes(LPITEM pDS);
	bool RefreshItemAttributes(LPITEM pItem);

	DragonSoulTable* m_pTable;
};

#endif // __INC_DRAGON_SOUL_H__
