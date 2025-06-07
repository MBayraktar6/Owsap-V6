#pragma once

#include "Packet.h"

/*
* 교환 창 관련
*/
class CPythonExchange : public CSingleton<CPythonExchange>
{
public:
	enum
	{
		EXCHANGE_ITEM_MAX_NUM = 12,
		EXCHANGE_YANG_MAX = 99999999,
#if defined(ENABLE_CHEQUE_SYSTEM)
		EXCHANGE_CHEQUE_MAX = 999,
#endif
	};

	typedef struct SExchangeData
	{
		char szName[CHARACTER_NAME_MAX_LEN + 1];
		BYTE bLevel;
		DWORD dwElk;
#if defined(ENABLE_CHEQUE_SYSTEM)
		DWORD dwCheque;
#endif
		BYTE bAccept;

		DWORD dwItemVnum[EXCHANGE_ITEM_MAX_NUM];
		WORD wItemCount[EXCHANGE_ITEM_MAX_NUM];
		long aItemSocket[EXCHANGE_ITEM_MAX_NUM][ITEM_SOCKET_SLOT_MAX_NUM];
		TPlayerItemAttribute aItemAttr[EXCHANGE_ITEM_MAX_NUM][ITEM_ATTRIBUTE_SLOT_MAX_NUM];
#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
		DWORD dwTransmutationVnum[EXCHANGE_ITEM_MAX_NUM];
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
		TPlayerItemRefineElement sItemRefineElement[EXCHANGE_ITEM_MAX_NUM];
#endif
#if defined(ENABLE_APPLY_RANDOM)
		TPlayerItemAttribute aItemApplyRandom[EXCHANGE_ITEM_MAX_NUM][ITEM_APPLY_RANDOM_SLOT_MAX_NUM];
#endif
#if defined(ENABLE_SET_ITEM)
		BYTE bSetValue[EXCHANGE_ITEM_MAX_NUM];
#endif
	} TExchangeData;

public:
	CPythonExchange();
	virtual ~CPythonExchange();

	void Clear();

	void Start();
	void End();
	bool IsTrading();

	// Interface
public:
	void SetAcceptToSelf(BYTE bAccept);
	bool GetAcceptFromSelf();

	void SetAcceptToTarget(BYTE bAccept);
	bool GetAcceptFromTarget();

	void SetElkMode(bool bValue);
	bool GetElkMode();

	// Name
	void SetSelfName(const char* c_szName);
	const char* GetNameFromSelf();

	void SetTargetName(const char* c_szName);
	const char* GetNameFromTarget();

	// Level
	void SetLevelToTarget(BYTE bLevel);
	BYTE GetLevelFromTarget() const;

	// Elk
	void SetElkToSelf(DWORD dwElk);
	DWORD GetElkFromSelf() const;

	void SetElkToTarget(DWORD dwElk);
	DWORD GetElkFromTarget() const;

#if defined(ENABLE_CHEQUE_SYSTEM)
	// Cheque
	void SetChequeToSelf(DWORD dwCheque);
	DWORD GetChequeFromSelf() const;

	void SetChequeToTarget(DWORD dwCheque);
	DWORD GetChequeFromTarget() const;
#endif

	// Item
	void SetItemToSelf(BYTE bPos, DWORD dwVnum, WORD wCount);
	void SetItemToTarget(BYTE bPos, DWORD dwVnum, WORD wCount);

	void DelItemOfTarget(BYTE bPos);
	void DelItemOfSelf(BYTE bPos);

	// Item Vnum
	DWORD GetItemVnumFromSelf(BYTE bPos) const;
	DWORD GetItemVnumFromTarget(BYTE bPos) const;

	// Item Count
	WORD GetItemCountFromSelf(BYTE bPos) const;
	WORD GetItemCountFromTarget(BYTE bPos) const;

	// Item Sockets
	void SetItemMetinSocketToSelf(BYTE bPos, BYTE bSocketPos, long lValue);
	long GetItemMetinSocketFromSelf(BYTE bPos, BYTE bSocketPos) const;

	void SetItemMetinSocketToTarget(BYTE bPos, BYTE bSocketPos, long lValue);
	long GetItemMetinSocketFromTarget(BYTE bPos, BYTE bSocketPos) const;

	// Item Attributes
	void SetItemAttributeToSelf(BYTE bPos, BYTE bAttrPos, POINT_TYPE wType, POINT_VALUE lValue);
	void GetItemAttributeFromSelf(BYTE bPos, BYTE bAttrPos, POINT_TYPE* pwType, POINT_VALUE* plValue);

	void SetItemAttributeToTarget(BYTE bPos, BYTE bAttrPos, POINT_TYPE wType, POINT_VALUE lValue);
	void GetItemAttributeFromTarget(BYTE bPos, BYTE bAttrPos, POINT_TYPE* pwType, POINT_VALUE* plValue);

#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
	// Item Change Look
	void SetItemChangeLookVnumToSelf(BYTE bPos, DWORD dwVnum);
	DWORD GetItemChangeLookVnumToSelf(BYTE bPos) const;

	void SetItemChangeLookVnumToTarget(BYTE bPos, DWORD dwVnum);
	DWORD GetItemChangeLookVnumToTarget(BYTE bPos) const;
#endif

#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
	// Item Refine Element
	void SetItemRefineElementToSelf(BYTE bPos, const TPlayerItemRefineElement& rkTable);
	const TPlayerItemRefineElement* const GetItemRefineElementFromSelf(BYTE bPos);

	void SetItemRefineElementToTarget(BYTE bPos, const TPlayerItemRefineElement& rkTable);
	const TPlayerItemRefineElement* const GetItemRefineElementFromTarget(BYTE bPos);
#endif

#if defined(ENABLE_APPLY_RANDOM)
	// Item Apply Random
	void SetItemApplyRandomToSelf(BYTE bPos, BYTE bApplyPos, POINT_TYPE wType, POINT_VALUE lValue);
	void GetItemApplyRandomFromSelf(BYTE bPos, BYTE bApplyPos, POINT_TYPE* pwType, POINT_VALUE* plValue);

	void SetItemApplyRandomToTarget(BYTE bPos, BYTE bApplyPos, POINT_TYPE wType, POINT_VALUE lValue);
	void GetItemApplyRandomFromTarget(BYTE bPos, BYTE bApplyPos, POINT_TYPE* pwType, POINT_VALUE* plValue);
#endif

#if defined(ENABLE_SET_ITEM)
	// Item Set Value
	void SetItemSetValueToSelf(BYTE bPos, BYTE bSetValue);
	BYTE GetItemSetValueFromSelf(BYTE bPos) const;

	void SetItemSetValueToTarget(BYTE bPos, const BYTE bSetValue);
	BYTE GetItemSetValueFromTarget(BYTE bPos) const;
#endif

protected:
	bool m_bIsTrading;
	bool m_bElkMode; // 엘크를 클릭해서 교환했을때를 위한 변종임.
	TExchangeData m_kSelf;
	TExchangeData m_kVictim;
};
