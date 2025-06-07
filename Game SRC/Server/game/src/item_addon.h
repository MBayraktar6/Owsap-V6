#ifndef __INC_ITEM_ADDON_H__
#define __INC_ITEM_ADDON_H__

class CItemAddonManager : public singleton<CItemAddonManager>
{
public:
	CItemAddonManager();
	virtual ~CItemAddonManager();

	void ApplyAddonTo(int iAddonType, LPITEM pItem);
};

#endif // __INC_ITEM_ADDON_H__
