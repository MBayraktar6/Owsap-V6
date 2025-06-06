#pragma once

#include "../EterBase/Singleton.h"

/*
* 따라다니는 텍스트 처리
*/
class CPythonTextTail : public CSingleton<CPythonTextTail>
{
public:
	typedef struct STextTail
	{
		CGraphicTextInstance* pTextInstance;
		CGraphicTextInstance* pOwnerTextInstance;

		CGraphicMarkInstance* pMarkInstance;
		CGraphicTextInstance* pGuildNameTextInstance;
#if defined(ENABLE_LEFT_SEAT)
		CGraphicTextInstance* pLeftSeatTextInstance;
#endif

		CGraphicTextInstance* pTitleTextInstance;
		CGraphicTextInstance* pLevelTextInstance;
#if defined(WJ_SHOW_MOB_INFO)
		CGraphicTextInstance* pAIFlagTextInstance;
#endif
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
		CGraphicExpandedImageInstance* pCountryFlagImageInstance;
#endif

		// Todo : 이건 전부 VID로 바꾸도록 한다.
		// 도중 캐릭터가 없어질 경우 튕길 가능성이 있음
		CGraphicObjectInstance* pOwner;

		DWORD dwVirtualID;

		float x, y, z;
		float fDistanceFromPlayer;
		D3DXCOLOR Color;
		BOOL bNameFlag; // 이름도 함께 켤것인지의 플래그

		float xStart, yStart;
		float xEnd, yEnd;

		DWORD LivingTime;

		float fHeight;

#if defined(WJ_SHOW_MOB_INFO)
		BOOL bIsPC;
#endif

		STextTail() {}
		virtual ~STextTail() {}
	} TTextTail;

	typedef std::map<DWORD, TTextTail*> TTextTailMap;
	typedef std::list<TTextTail*> TTextTailList;
	typedef TTextTailMap TChatTailMap;

public:
	CPythonTextTail(void);
	virtual ~CPythonTextTail(void);

	void GetInfo(std::string* pstInfo);

	void Initialize();
	void Destroy();
	void Clear();

	void UpdateAllTextTail();
	void UpdateShowingTextTail();
	void Render();

	void ArrangeTextTail();
	void HideAllTextTail();
	void ShowAllTextTail();
	void ShowCharacterTextTail(DWORD VirtualID);
	void ShowItemTextTail(DWORD VirtualID);

	void RegisterCharacterTextTail(DWORD dwGuildID
#if defined(ENABLE_GUILD_LEADER_GRADE_NAME)
		, const char* c_szGuildLeaderGradeName
#endif
		, DWORD dwVirtualID, const D3DXCOLOR& c_rColor, float fAddHeight = 10.0f
	);
	void RegisterItemTextTail(DWORD VirtualID, const char* c_szText, CGraphicObjectInstance* pOwner
#if defined(ENABLE_ITEM_DROP_RENEWAL)
		, bool bHasAttr = false
#endif
	);

	void RegisterChatTail(DWORD VirtualID, const char* c_szChat);
	void RegisterInfoTail(DWORD VirtualID, const char* c_szChat);
	void SetCharacterTextTailColor(DWORD VirtualID, const D3DXCOLOR& c_rColor);
	void SetItemTextTailOwner(DWORD dwVID, const char* c_szName);
	void DeleteCharacterTextTail(DWORD VirtualID);
	void DeleteItemTextTail(DWORD VirtualID);

	int Pick(int ixMouse, int iyMouse);
	void SelectItemName(DWORD dwVirtualID);

	bool GetTextTailPosition(DWORD dwVID, float* px, float* py, float* pz);
	bool IsChatTextTail(DWORD dwVID);

	void EnablePKTitle(BOOL bFlag);
	void AttachTitle(DWORD dwVID, const char* c_szName, const D3DXCOLOR& c_rColor);
	void DetachTitle(DWORD dwVID);

	void AttachLevel(DWORD dwVID, const char* c_szText, const D3DXCOLOR& c_rColor);
#if defined(ENABLE_CONQUEROR_LEVEL)
	void AttachConquerorLevel(DWORD dwVID, const char* c_szText);
#endif
	void DetachLevel(DWORD dwVID);

#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
	void AttachCountryFlag(DWORD dwVID, const std::string& c_rstrCountry);
	void DetachCountryFlag(DWORD dwVID);
#endif

#if defined(ENABLE_LEFT_SEAT)
	void AttachLeftSeatText(DWORD dwVID, const std::string& c_rstrText);
	void DetachLeftSeatText(DWORD dwVID);
#endif

protected:
	TTextTail* RegisterTextTail(DWORD dwVirtualID, const char* c_szText, CGraphicObjectInstance* pOwner, float fHeight, const D3DXCOLOR& c_rColor);
	void DeleteTextTail(TTextTail* pTextTail);

	void UpdateTextTail(TTextTail* pTextTail);
	void RenderTextTailBox(TTextTail* pTextTail);
	void RenderTextTailName(TTextTail* pTextTail);
	void UpdateDistance(const TPixelPosition& c_rCenterPosition, TTextTail* pTextTail);

	bool isIn(TTextTail* pSource, TTextTail* pTarget);

protected:
	TTextTailMap m_CharacterTextTailMap;
	TTextTailMap m_ItemTextTailMap;
	TChatTailMap m_ChatTailMap;

	TTextTailList m_CharacterTextTailList;
	TTextTailList m_ItemTextTailList;

private:
	CDynamicPool<STextTail> m_TextTailPool;
};
