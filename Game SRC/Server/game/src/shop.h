#ifndef __INC_SHOP_H__
#define __INC_SHOP_H__

enum
{
	SHOP_MAX_DISTANCE = 1000,
};

class CGrid;

/* ---------------------------------------------------------------------------------- */
class CShop
{
public:
	enum EShopGrid
	{
		SHOP_GRID_WIDTH = 5,
		SHOP_GRID_HEIGHT = 9,
	};

	typedef struct shop_item
	{
		DWORD vnum; // 아이템 번호
		long price; // 가격
#if defined(__CHEQUE_SYSTEM__)
		long cheque;
#endif
		DWORD count; // 아이템 개수

		LPITEM pkItem;
		int itemid; // 아이템 고유아이디

#if defined(__SHOPEX_RENEWAL__)
		BYTE bPriceType;
		DWORD dwPriceVnum;
		long alSockets[ITEM_SOCKET_MAX_NUM];
#endif

		shop_item()
		{
			vnum = 0;
			price = 0;
#if defined(__CHEQUE_SYSTEM__)
			cheque = 0;
#endif
			count = 0;
			itemid = 0;
			pkItem = NULL;

#if defined(__SHOPEX_RENEWAL__)
			bPriceType = SHOP_COIN_TYPE_GOLD;
			dwPriceVnum = 0;
			memset(&alSockets, 0, sizeof(alSockets));
#endif
		}
	} SHOP_ITEM;

	CShop();
	virtual ~CShop();

	bool Create(DWORD dwVnum, DWORD dwNPCVnum, TShopItemTable* pItemTable);
	void SetShopItems(TShopItemTable* pItemTable, BYTE bItemCount);

	virtual void SetPCShop(LPCHARACTER ch);
	virtual bool IsPCShop() { return m_pkPC ? true : false; }
#if defined(__SHOPEX_RENEWAL__)
	virtual bool IsShopEx() const { return false; };
#endif

	// 게스트 추가/삭제
	virtual bool AddGuest(LPCHARACTER ch, DWORD owner_vid, bool bOtherEmpire);
	void RemoveGuest(LPCHARACTER ch);

	// 물건 구입
	virtual int Buy(LPCHARACTER ch, BYTE pos
#if defined(__PRIVATESHOP_SEARCH_SYSTEM__)
		, bool bIsShopSearch = false
#endif
	);

	// 게스트에게 패킷을 보냄
	void BroadcastUpdateItem(BYTE pos);

	// 판매중인 아이템의 갯수를 알려준다.
	int GetNumberByVnum(DWORD dwVnum);

	// 아이템이 상점에 등록되어 있는지 알려준다.
	virtual bool IsSellingItem(DWORD itemID);

	DWORD GetVnum() { return m_dwVnum; }
	DWORD GetNPCVnum() { return m_dwNPCVnum; }

#if defined(__PRIVATESHOP_SEARCH_SYSTEM__)
	const std::vector<SHOP_ITEM>& GetItemVector() const { return m_itemVector; }
	LPCHARACTER GetShopOwner() { return m_pkPC; }
#endif

protected:
	void Broadcast(const void* data, int bytes);

protected:
	DWORD m_dwVnum;
	DWORD m_dwNPCVnum;

#if defined(__MYSHOP_EXPANSION__)
	std::unique_ptr<CGrid> m_pGrid;
	std::array<std::unique_ptr<CGrid>, MYSHOP_MAX_TABS> m_apTabGrid;
#else
	CGrid* m_pGrid;
#endif

	typedef std::unordered_map<LPCHARACTER, bool> GuestMapType;
	GuestMapType m_map_guest;
	std::vector<SHOP_ITEM> m_itemVector; // 이 상점에서 취급하는 물건들

	LPCHARACTER m_pkPC;
};

#endif // __INC_SHOP_H__
