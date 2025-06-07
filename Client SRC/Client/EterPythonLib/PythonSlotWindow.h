#pragma once

#include "PythonWindow.h"

namespace UI
{
	enum
	{
		ITEM_WIDTH = 32,
		ITEM_HEIGHT = 32,

		ITEM_MAX_SIZE = 3,
#if defined(WJ_ENABLE_PICKUP_ITEM_EFFECT)
		SLOT_ACTIVE_EFFECT_COUNT = 3,
#endif

		SLOT_NUMBER_NONE = 0xffffffff,
	};

	enum ESlotStyle
	{
		SLOT_STYLE_NONE,
		SLOT_STYLE_PICK_UP,
		SLOT_STYLE_SELECT,
	};

#if defined(WJ_ENABLE_PICKUP_ITEM_EFFECT)
	enum ESlotColorType
	{
		COLOR_TYPE_ORANGE,
		COLOR_TYPE_WHITE,
		COLOR_TYPE_RED,
		COLOR_TYPE_GREEN,
		COLOR_TYPE_YELLOW,
		COLOR_TYPE_SKY,
		COLOR_TYPE_PINK,
		COLOR_TYPE_MAX
	};

	enum ESlotHilight
	{
#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
		HILIGHTSLOT_ACCE,
#endif
#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
		HILIGHTSLOT_CHANGE_LOOK,
#endif
#if defined(ENABLE_AURA_COSTUME_SYSTEM)
		HILIGHTSLOT_AURA,
#endif
#if defined(ENABLE_CUBE_RENEWAL)
		HILIGHTSLOT_CUBE,
#endif
		HILIGHTSLOT_MAX
	};
#endif

	enum ESlotState
	{
		SLOT_STATE_LOCK = (1 << 0),
		SLOT_STATE_CANT_USE = (1 << 1),
		SLOT_STATE_DISABLE = (1 << 2),
		SLOT_STATE_ALWAYS_RENDER_COVER = (1 << 3),
#if defined(WJ_ENABLE_TRADABLE_ICON)
		SLOT_STATE_CANT_MOUSE_EVENT = (1 << 4),
		SLOT_STATE_UNUSABLE_ON_TOP_WND = (1 << 5),
#endif
		SLOT_STATE_HIGHLIGHT_GREEN = (1 << 6),
	};

	class CSlotWindow : public CWindow
	{
	public:
		static DWORD Type();

	public:
		class CSlotButton;
		class CCoverButton;
		class CCoolTimeFinishEffect;
#if defined(ENABLE_HIGH_LIGHT_IMAGE)
		class CHighLightImage;
#endif

		friend class CSlotButton;
		friend class CCoverButton;

		typedef struct SSlot
		{
			DWORD dwState;
			DWORD dwSlotNumber;
			DWORD dwCenterSlotNumber; // NOTE : 사이즈가 큰 아이템의 경우 아이템의 실제 위치 번호
			DWORD dwRealSlotNumber;
			DWORD dwRealCenterSlotNumber;
			DWORD dwItemIndex; // NOTE : 여기서 사용되는 Item이라는 단어는 좁은 개념의 것이 아닌,
			BOOL isItem; // "슬롯의 내용물"이라는 포괄적인 개념어. 더 좋은 것이 있을까? - [levites]

			// CoolTime
			float fCoolTime;
			float fStartCoolTime;

			// Toggle
			BOOL bActive;

			int ixPosition;
			int iyPosition;

			int ixCellSize;
			int iyCellSize;

			BYTE byxPlacedItemSize;
			BYTE byyPlacedItemSize;

			CGraphicImageInstance* pInstance;
			CNumberLine* pNumberLine;

			bool bRenderBaseSlotImage;
			CCoverButton* pCoverButton;
			CSlotButton* pSlotButton;
			CImageBox* pSignImage;
			CAniImageBox* pFinishCoolTimeEffect;
#if defined(WJ_ENABLE_PICKUP_ITEM_EFFECT)
			D3DXCOLOR d3Color;
#endif
			bool bCoolTimeInverse;
			D3DXCOLOR d3CoolTimeColor;

#if defined(ENABLE_SLOT_SCALE)
			int i116;
#endif
			std::array<std::shared_ptr<CAniImageBox>, 3> pSlotEffect;
			bool bRenderCoverImage;
			bool bRenderSecondCoverImage;
#if defined(ENABLE_HIGH_LIGHT_IMAGE)
			BOOL bEnableHighLight;
			CHighLightImage* pHighLightImage;
#endif
			D3DXCOLOR sDiffuseColor;
		} TSlot;

		typedef std::list<TSlot> TSlotList;
		typedef TSlotList::iterator TSlotListIterator;
		struct SStoreCoolDown
		{
			float fCoolTime;
			float fElapsedTime;
			bool bActive;
		};

	public:
		CSlotWindow(PyObject* ppyObject);
		virtual ~CSlotWindow();

		void Destroy();

		// Manage Slot
		void SetSlotType(DWORD dwType);
		void SetSlotStyle(DWORD dwStyle);

		void HideSlotButton(DWORD dwSlotIndex);

		void AppendSlot(DWORD dwIndex, int ixPosition, int iyPosition, int ixCellSize, int iyCellSize);
		void SetCoverButton(DWORD dwIndex, const char* c_szUpImageName, const char* c_szOverImageName, const char* c_szDownImageName, const char* c_szDisableImageName, BOOL bLeftButtonEnable, BOOL bRightButtonEnable);
		void SetSlotBaseImage(const char* c_szFileName, float fr, float fg, float fb, float fa);
		void SetBaseImageScale(float fx, float fy);
		void AppendSlotButton(const char* c_szUpImageName, const char* c_szOverImageName, const char* c_szDownImageName);
		void AppendRequirementSignImage(const char* c_szImageName);
		void SetCorverButtonScale(DWORD dwSlotIndex, float fx, float fy);

		void EnableCoverButton(DWORD dwIndex);
		void DisableCoverButton(DWORD dwIndex);
		void SetAlwaysRenderCoverButton(DWORD dwIndex, bool bAlwaysRender = false);

		void ShowSlotBaseImage(DWORD dwIndex);
		void HideSlotBaseImage(DWORD dwIndex);
		BOOL IsDisableCoverButton(DWORD dwIndex);
		BOOL HasSlot(DWORD dwIndex);

		void ClearAllSlot();
		void ClearSlot(DWORD dwIndex);
		void SetSlot(DWORD dwIndex, DWORD dwVirtualNumber, BYTE byWidth, BYTE byHeight, CGraphicImage* pImage, D3DXCOLOR& diffuseColor);
		void SetSlotCount(DWORD dwIndex, DWORD dwCount);
		void SetSlotCountNew(DWORD dwIndex, DWORD dwGrade, DWORD dwCount);
		void SetRealSlotNumber(DWORD dwIndex, DWORD dwID);
		void SetSlotCoolTime(DWORD dwIndex, float fCoolTime, float fElapsedTime = 0.0f);

#if defined(WJ_ENABLE_PICKUP_ITEM_EFFECT)
		void SetSlotDiffuseColor(DWORD dwIndex, int iColorType);
#endif

#if defined(ENABLE_HIGH_LIGHT_IMAGE)
		void AppendHighLightImage(DWORD dwIndex, const char* c_sImageName, float fAlphaSpeed = 0.0f, float fRotationSpeed = 0.0f, float fCurAlpha = 1.0f, DWORD dwDiffuse = DWORD(-1));
		void EnableHighLightImage(DWORD dwIndex);
		void DisableHighLightImage(DWORD dwIndex);
#endif

#if defined(ENABLE_SLOT_COOLTIME_REFRESH_FIX)
		void StoreSlotCoolTime(DWORD dwKey, DWORD dwSlotIndex, float fCoolTime, float fElapsedTime = .0f);
		void RestoreSlotCoolTime(DWORD dwKey);
#endif

		void ActivateSlot(const DWORD dwSlotIndex);
		void DeactivateSlot(const DWORD dwSlotIndex);
		void RefreshSlot();

		DWORD GetSlotCount();
		bool IsActiveSlot(DWORD dwIndex);

		void LockSlot(DWORD dwIndex);
		void UnlockSlot(DWORD dwIndex);
		BOOL IsLockSlot(DWORD dwIndex);
		void SetCantUseSlot(DWORD dwIndex);
		void SetUseSlot(DWORD dwIndex);
		BOOL IsCantUseSlot(DWORD dwIndex);
		void EnableSlot(DWORD dwIndex);
		void DisableSlot(DWORD dwIndex);
		BOOL IsEnableSlot(DWORD dwIndex);

#if defined(WJ_ENABLE_TRADABLE_ICON)
		void SetCantMouseEventSlot(DWORD dwIndex);
		void SetCanMouseEventSlot(DWORD dwIndex);
		void SetUnusableSlotOnTopWnd(DWORD dwIndex);
		void SetUsableSlotOnTopWnd(DWORD dwIndex);
#endif

		// Select
		void ClearSelected();
		void SelectSlot(DWORD dwSelectingIndex);
		BOOL isSelectedSlot(DWORD dwIndex);
		DWORD GetSelectedSlotCount();
		DWORD GetSelectedSlotNumber(DWORD dwIndex);

		// Slot Button
		void ShowSlotButton(DWORD dwSlotNumber);
		void HideAllSlotButton();
		void OnPressedSlotButton(DWORD dwType, DWORD dwSlotNumber, BOOL isLeft = TRUE);

#if defined(ENABLE_SLOT_SCALE)
		void SetSlotImage(DWORD dwSlotIndex, const char* c_szFileName);
		void SetScale(float fx, float fy);
#endif

		void SetSlotCoverImage(DWORD dwSlotIndex, const char* c_szFileName);
		void EnableSlotCoverImage(DWORD dwSlotIndex, bool bOnOff);

		void __CreateSlotCoverImage(const char* c_szFileName);
		void __DestroySlotCoverImage();

		void SetSecondSlotCoverImage(DWORD dwSlotIndex, const char* c_szFileName);
		void EnableSecondSlotCoverImage(DWORD dwSlotIndex, bool bOnOff);
		void __CreateSecondSlotCoverImage(const char* c_szFileName);
		void __DestroySecondSlotCoverImage();

		void SetSlotCoolTimeInverse(DWORD dwIndex, float fCoolTime, float fElapsedTime = 0.0f);
		void SetSlotCoolTimeColor(DWORD dwSlotIndex, float fr, float fg, float fb, float fa);

		// Requirement Sign
		void ShowRequirementSign(DWORD dwSlotNumber);
		void HideRequirementSign(DWORD dwSlotNumber);

		// OverInSlot
		BOOL OnOverIn(DWORD dwSlotNumber);
		void OnOverOut();

		// ToolTip
		BOOL OnOverInItem(DWORD dwSlotNumber);
		void OnOverOutItem();

		// For Usable Item
		void SetUseMode(BOOL bFlag);
		void SetUsableItem(BOOL bFlag);

		// CallBack
		void ReserveDestroyCoolTimeFinishEffect(DWORD dwSlotIndex);

		void GetSlotGlobalPosition(DWORD dwSlotIndex, long* x, long* y);
		void GetSlotLocalPosition(DWORD dwSlotIndex, long* x, long* y);

	protected:
		void __Initialize();
		void __CreateToggleSlotImage();
#if defined(WJ_ENABLE_PICKUP_ITEM_EFFECT)
		void __CreateSlotEnableEffect(int iIndex);
#else
		void __CreateSlotEnableEffect();
#endif
		void __CreateFinishCoolTimeEffect(TSlot* pSlot);
		void __CreateBaseImage(const char* c_szFileName, float fr, float fg, float fb, float fa);

		void __DestroyToggleSlotImage();
#if defined(WJ_ENABLE_PICKUP_ITEM_EFFECT)
		void __DestroySlotEnableEffect(int iIndex);
#else
		void __DestroySlotEnableEffect();
#endif
		void __DestroyFinishCoolTimeEffect(TSlot* pSlot);
		void __DestroyBaseImage();

		// Event
		void OnUpdate();
		void OnRender();
		BOOL OnMouseLeftButtonDown();
		BOOL OnMouseLeftButtonUp();
		BOOL OnMouseRightButtonDown();
		BOOL OnMouseLeftButtonDoubleClick();
		void OnMouseOverOut();
		void OnMouseOver();
		void RenderSlotBaseImage();
		void RenderLockedSlot();
		virtual void OnRenderPickingSlot();
		virtual void OnRenderSelectedSlot();

		// Select
		void OnSelectEmptySlot(int iSlotNumber);
		void OnSelectItemSlot(int iSlotNumber);
		void OnUnselectEmptySlot(int iSlotNumber);
		void OnUnselectItemSlot(int iSlotNumber);
		void OnUseSlot();

		// Manage Slot
		BOOL GetSlotPointer(DWORD dwIndex, TSlot** ppSlot);
		BOOL GetSelectedSlotPointer(TSlot** ppSlot);
		virtual BOOL GetPickedSlotPointer(TSlot** ppSlot);
		void ClearSlot(TSlot* pSlot);
		virtual void OnRefreshSlot();

		// ETC
		BOOL OnIsType(DWORD dwType);

	protected:
		DWORD m_dwSlotType;
		DWORD m_dwSlotStyle;
		std::list<DWORD> m_dwSelectedSlotIndexList;
		TSlotList m_SlotList;
		DWORD m_dwOverInSlotNumber;
		DWORD m_dwToolTipSlotNumber;
#if defined(ENABLE_SLOT_COOLTIME_REFRESH_FIX)
		std::map<DWORD, std::map<DWORD, SStoreCoolDown>> m_CoolDownStore;
#endif

		BOOL m_isUseMode;
		BOOL m_isUsableItem;
#if defined(ENABLE_SLOT_SCALE)
		D3DXVECTOR2 m_v2Scale;
#endif

		CGraphicImageInstance* m_pBaseImageInstance;
		CImageBox* m_pToggleSlotImage;
		CImageBox* m_pSlotCoverImage;
		CImageBox* m_pSecondSlotCoverImage;
#if defined(WJ_ENABLE_PICKUP_ITEM_EFFECT)
		CAniImageBox* m_pSlotActiveEffect[SLOT_ACTIVE_EFFECT_COUNT];
#else
		CAniImageBox* m_pSlotActiveEffect;
#endif
		std::deque<DWORD> m_ReserveDestroyEffectDeque;

	};
};
