#ifndef __INC_SHOP_MANAGER_H__
#define __INC_SHOP_MANAGER_H__

class CShop;
typedef class CShop* LPSHOP;

class CShopManager : public singleton<CShopManager>
{
public:
	typedef std::map<DWORD, CShop*> TShopMap;

public:
	CShopManager();
	virtual ~CShopManager();

	bool Initialize(TShopTable* table, int size);
	void Destroy();

	LPSHOP Get(DWORD dwVnum);
	LPSHOP GetByNPCVnum(DWORD dwVnum);

	bool StartShopping(LPCHARACTER pkChr, LPCHARACTER pkShopKeeper, int iShopVnum = 0);
	void StopShopping(LPCHARACTER ch);

#if defined(__PRIVATESHOP_SEARCH_SYSTEM__)
	void ShopSearchBuy(LPCHARACTER ch, const TPacketCGPrivateShopSearchBuyItem* p);
	void ShopSearchProcess(LPCHARACTER ch, const TPacketCGPrivateShopSearch* p);
#endif

	void Buy(LPCHARACTER ch, BYTE pos);
	void Sell(LPCHARACTER ch, WORD wCell, WORD wCount = 0, BYTE bType = 0);

	LPSHOP CreatePCShop(LPCHARACTER ch, TShopItemTable* pTable, BYTE bItemCount);
	LPSHOP FindPCShop(DWORD dwVID);
	void DestroyPCShop(LPCHARACTER ch);
#if defined(__SHOPEX_RENEWAL__)
	bool ReadShopTableEx(const char* stFileName);
#endif

private:
	TShopMap m_map_pkShop;
	TShopMap m_map_pkShopByNPCVnum;
	TShopMap m_map_pkShopByPC;

#if !defined(__SHOPEX_RENEWAL__)
	bool ReadShopTableEx(const char* stFileName);
#endif
};

#endif // __INC_SHOP_MANAGER_H__
