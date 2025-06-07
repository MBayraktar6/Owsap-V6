/*
* Filename: changelook.h
* Author: blackdragonx61 (aka. Mali)
*/

#pragma once

#if defined(__CHANGE_LOOK_SYSTEM__)
class CChangeLook
{
public:
	CChangeLook(const LPCHARACTER c_lpCh, const BYTE c_bType);
	~CChangeLook();

	static void Open(const LPCHARACTER c_lpCh, const BYTE c_bType);

	void ItemCheckIn(const TItemPos& c_rItemPos, const BYTE c_bSlotIndex);
	void ItemCheckOut(const BYTE c_bSlotIndex);

	void FreeItemCheckIn(const TItemPos& c_rItemPos);
	void FreeItemCheckOut();

	void Accept();
	void Clear();

	bool IsTypeItem() const;
	bool IsTypeMount() const;

private:
	bool CanAddItem(const LPITEM c_lpItem) const;
	bool CheckItem(const LPITEM c_lpItem) const;
	bool CanAddPassItem(const LPITEM c_lpItem) const;

	LPITEM GetLeftItem() const;
	LPITEM GetRightItem() const;
	LPITEM GetPassItem() const;

	DWORD GetPrice() const;

private:
	enum class EChangeLookType : BYTE
	{
		CHANGE_LOOK_TYPE_ITEM,
		CHANGE_LOOK_TYPE_MOUNT
	};

	enum class EChangeLookSlots : BYTE
	{
		CHANGE_LOOK_SLOT_LEFT,
		CHANGE_LOOK_SLOT_RIGHT,
		CHANGE_LOOK_SLOT_MAX
	};

	enum class EChangeLookPrice : DWORD
	{
		CHANGE_LOOK_ITEM_PRICE = 50000000, // 50M
		CHANGE_LOOK_MOUNT_PRICE = 30000000, // 30M
	};

private:
	LPCHARACTER m_lpCh;
	EChangeLookType m_bType;
	std::array<LPITEM, static_cast<BYTE>(EChangeLookSlots::CHANGE_LOOK_SLOT_MAX)> m_aItem;
	LPITEM m_lpPassItem;
};
#endif
