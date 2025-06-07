#ifndef __INC_EXCHANGE_H__
#define __INC_EXCHANGE_H__

class CGrid;

enum EExchangeValues
{
	EXCHANGE_ITEM_MAX_NUM = 12,
	EXCHANGE_MAX_DISTANCE = 1000,
	EXCHANGE_YANG_MAX = 99999999,
#if defined(__CHEQUE_SYSTEM__)
	EXCHANGE_CHEQUE_MAX = 999,
#endif
};

class CExchange
{
public:
	CExchange(LPCHARACTER pOwner);
	~CExchange();

	bool Accept(bool bIsAccept = true);
	void Cancel();

	bool AddGold(long lGold);
#if defined(__CHEQUE_SYSTEM__)
	bool AddCheque(long lCheque);
#endif
	bool AddItem(TItemPos item_pos, BYTE display_pos);
	bool RemoveItem(BYTE pos);

	LPCHARACTER GetOwner() { return m_pOwner; }
	CExchange* GetCompany() { return m_pCompany; }

	bool GetAcceptStatus() { return m_bAccept; }

	void SetCompany(CExchange* pExchange) { m_pCompany = pExchange; }

	LPITEM GetItemByPosition(int i) const { return m_apItems[i]; }

private:
	bool Done();
	bool Check(int* piItemCount);
	bool CheckSpace();

private:
	CExchange* m_pCompany; // 상대방의 CExchange 포인터

	LPCHARACTER m_pOwner;

	TItemPos m_aItemPos[EXCHANGE_ITEM_MAX_NUM];
	LPITEM m_apItems[EXCHANGE_ITEM_MAX_NUM];
	BYTE m_abItemDisplayPos[EXCHANGE_ITEM_MAX_NUM];

	bool m_bAccept;
	long m_lGold;
#if defined(__CHEQUE_SYSTEM__)
	long m_lCheque;
#endif

	CGrid* m_pGrid;

};

#endif // __INC_EXCHANGE_H__
