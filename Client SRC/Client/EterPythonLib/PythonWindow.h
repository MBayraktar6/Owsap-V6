#pragma once

#include "../EterBase/Utils.h"
#include "../UserInterface/Locale_inc.h"
#if defined(RENDER_TARGET)
#include "../EterLib/GrpRenderTargetTexture.h"
#endif

namespace UI
{
	class CWindow
	{
	public:
		typedef std::list<CWindow*> TWindowContainer;

		static DWORD Type();
		BOOL IsType(DWORD dwType);

		enum EHorizontalAlign
		{
			HORIZONTAL_ALIGN_LEFT = 0,
			HORIZONTAL_ALIGN_CENTER = 1,
			HORIZONTAL_ALIGN_RIGHT = 2,
		};

		enum EVerticalAlign
		{
			VERTICAL_ALIGN_TOP = 0,
			VERTICAL_ALIGN_CENTER = 1,
			VERTICAL_ALIGN_BOTTOM = 2,
		};

		enum EFlags
		{
			FLAG_MOVABLE = (1 << 0),	// 움직일 수 있는 창
			FLAG_LIMIT = (1 << 1),	// 창이 화면을 벗어나지 않음
			FLAG_SNAP = (1 << 2),	// 스냅 될 수 있는 창
			FLAG_DRAGABLE = (1 << 3),
			FLAG_ATTACH = (1 << 4),	// 완전히 부모에 붙어 있는 창 (For Drag / ex. ScriptWindow)
			FLAG_RESTRICT_X = (1 << 5),	// 좌우 이동 제한
			FLAG_RESTRICT_Y = (1 << 6),	// 상하 이동 제한
			FLAG_NOT_CAPTURE = (1 << 7),
			FLAG_FLOAT = (1 << 8),	// 공중에 떠있어서 순서 재배치가 되는 창
			FLAG_NOT_PICK = (1 << 9),	// 마우스에 의해 Pick되지 않는 창
			FLAG_IGNORE_SIZE = (1 << 10),
			FLAG_RTL = (1 << 11),	// Right-to-left
		};

		enum WindowTypes
		{
			WINDOW_TYPE_WINDOW,
			WINDOW_TYPE_TEXTLINE,
			WINDOW_TYPE_EX_IMAGE,
			WINDOW_TYPE_MAX_NUM
		};

	public:
		CWindow(PyObject* ppyObject);
		virtual ~CWindow();

		void AddChild(CWindow* pWin);

		void Clear();
		void DestroyHandle();
		void Update();
		void Render();

		void SetName(const char* c_szName);
		const char* GetName() { return m_strName.c_str(); }
		void SetSize(long width, long height);
		long GetWidth() { return m_lWidth; }
		long GetHeight() { return m_lHeight; }

		void SetHorizontalAlign(DWORD dwAlign);
		void SetVerticalAlign(DWORD dwAlign);
		void SetPosition(long x, long y);
		void GetPosition(long* plx, long* ply);
		long GetPositionX(void) const { return m_x; }
		long GetPositionY(void) const { return m_y; }
		RECT& GetRect() { return m_rect; }
		void GetLocalPosition(long& rlx, long& rly);
		void GetMouseLocalPosition(long& rlx, long& rly);
		long UpdateRect();

		RECT& GetLimitBias() { return m_limitBiasRect; }
		void SetLimitBias(long l, long r, long t, long b) { m_limitBiasRect.left = l, m_limitBiasRect.right = r, m_limitBiasRect.top = t, m_limitBiasRect.bottom = b; }

		void Show();
		void Hide();
		bool IsShow() { return m_bShow; }
		bool IsRendering();

		bool HasParent() { return m_pParent ? true : false; }
		bool HasChild() { return m_pChildList.empty() ? false : true; }
		int GetChildCount() { return m_pChildList.size(); }

		CWindow* GetRoot();
		CWindow* GetParent();
		bool IsChild(CWindow* pWin);
		void DeleteChild(CWindow* pWin);
		void SetTop(CWindow* pWin);

		bool IsIn(long x, long y);
		bool IsIn();
		CWindow* PickWindow(long x, long y);
		CWindow* PickTopWindow(long x, long y);	// NOTE : Children으로 내려가지 않고 상위에서만 
		// 체크 하는 특화된 함수

		void __RemoveReserveChildren();

		void AddFlag(DWORD flag) { SET_BIT(m_dwFlag, flag); }
		void RemoveFlag(DWORD flag) { REMOVE_BIT(m_dwFlag, flag); }
		bool IsFlag(DWORD flag) { return (m_dwFlag & flag) ? true : false; }
		/////////////////////////////////////

		virtual void OnRender();
		virtual void OnUpdate();
		virtual void OnChangePosition() {}

		virtual void OnSetFocus();
		virtual void OnKillFocus();

		virtual void OnMouseDrag(long lx, long ly);
		virtual void OnMouseOverIn();
		virtual void OnMouseOverOut();
		virtual void OnMouseOver();
		virtual void OnDrop();
		virtual void OnTop();
		virtual void OnIMEUpdate();

		virtual void OnMoveWindow(long x, long y);

		///////////////////////////////////////

		BOOL RunIMETabEvent();
		BOOL RunIMEReturnEvent();
		BOOL RunIMEKeyDownEvent(int ikey);

		CWindow* RunKeyDownEvent(int ikey);
		BOOL RunKeyUpEvent(int ikey);
		BOOL RunPressEscapeKeyEvent();
		BOOL RunPressExitKeyEvent();

		virtual BOOL OnIMETabEvent();
		virtual BOOL OnIMEReturnEvent();
		virtual BOOL OnIMEKeyDownEvent(int ikey);

		virtual BOOL OnIMEChangeCodePage();
		virtual BOOL OnIMEOpenCandidateListEvent();
		virtual BOOL OnIMECloseCandidateListEvent();
		virtual BOOL OnIMEOpenReadingWndEvent();
		virtual BOOL OnIMECloseReadingWndEvent();

		virtual BOOL OnMouseLeftButtonDown();
		virtual BOOL OnMouseLeftButtonUp();
		virtual BOOL OnMouseLeftButtonDoubleClick();
		virtual BOOL OnMouseRightButtonDown();
		virtual BOOL OnMouseRightButtonUp();
		virtual BOOL OnMouseRightButtonDoubleClick();
		virtual BOOL OnMouseMiddleButtonDown();
		virtual BOOL OnMouseMiddleButtonUp();

		virtual BOOL OnKeyDown(int ikey);
		virtual BOOL OnKeyUp(int ikey);
		virtual BOOL OnPressEscapeKey();
		virtual BOOL OnPressExitKey();
		///////////////////////////////////////

		virtual void SetColor(DWORD dwColor) {}
		virtual BOOL OnIsType(DWORD dwType);
		/////////////////////////////////////

		virtual BOOL IsWindow() { return TRUE; }
		/////////////////////////////////////

#if defined(ENABLE_CLIP_MASK)
		virtual void SetClippingMaskRect(const RECT& rMaskRect);
		virtual void SetClippingMaskWindow(CWindow* pMaskWindow);
#endif

#if defined(ENABLE_MOUSE_WHEEL_TOP_WINDOW)
		virtual bool OnMouseWheelButtonUp();
		virtual bool OnMouseWheelButtonDown();
#endif

	protected:
		std::string m_strName;
		BYTE m_bWindowType;

		EHorizontalAlign m_HorizontalAlign;
		EVerticalAlign m_VerticalAlign;
		long m_x, m_y; // X,Y 상대좌표
		long m_lWidth, m_lHeight; // 크기
		RECT m_rect; // Global 좌표
		RECT m_limitBiasRect; // limit bias 값

		bool m_bMovable;
		bool m_bShow;

		DWORD m_dwFlag;

		PyObject* m_poHandler;

		CWindow* m_pParent;
		TWindowContainer m_pChildList;

		BOOL m_isUpdatingChildren;
		TWindowContainer m_pReserveChildList;

#if defined(ENABLE_CLIP_MASK)
		bool m_bEnableMask;
		CWindow* m_pMaskWindow;
		RECT m_rMaskRect;
#endif

#ifdef _DEBUG
	public:
		DWORD DEBUG_dwCounter;
#endif
	};

	class CLayer : public CWindow
	{
	public:
		CLayer(PyObject* ppyObject) : CWindow(ppyObject) {}
		virtual ~CLayer() {}

		BOOL IsWindow() { return FALSE; }
	};

	class CBox : public CWindow
	{
	public:
		CBox(PyObject* ppyObject);
		virtual ~CBox();

		void SetColor(DWORD dwColor);

	protected:
		void OnRender();

	protected:
		DWORD m_dwColor;
	};

	class CBar : public CWindow
	{
	public:
		CBar(PyObject* ppyObject);
		virtual ~CBar();

		void SetColor(DWORD dwColor);

	protected:
#if defined(ENABLE_CLIP_MASK)
		void OnUpdate();
#endif
		void OnRender();

	protected:
		DWORD m_dwColor;
	};

	class CLine : public CWindow
	{
	public:
		CLine(PyObject* ppyObject);
		virtual ~CLine();

		void SetColor(DWORD dwColor);

	protected:
		void OnRender();

	protected:
		DWORD m_dwColor;
	};

	class CBar3D : public CWindow
	{
	public:
		static DWORD Type();
		BOOL OnIsType(DWORD dwType) override;

	public:
		CBar3D(PyObject* ppyObject);
		virtual ~CBar3D();

		void SetColor(DWORD dwLeft, DWORD dwRight, DWORD dwCenter);

	protected:
#if defined(ENABLE_CLIP_MASK)
		void OnUpdate();
#endif
		void OnRender();

	protected:
		DWORD m_dwLeftColor;
		DWORD m_dwRightColor;
		DWORD m_dwCenterColor;
	};

	// Text
	class CTextLine : public CWindow
	{
	public:
		CTextLine(PyObject* ppyObject);
		virtual ~CTextLine();

		void SetMax(int iMax);
		void SetHorizontalAlign(int iType);
		void SetVerticalAlign(int iType);
		void SetSecret(BOOL bFlag);
		void SetOutline(BOOL bFlag);
		void SetOutlineColor(float fRed, float fGreen, float fBlue, float fAlpha = 1.0f);
		void SetOutlineColor(DWORD dwColor);
		void SetFeather(BOOL bFlag);
		void SetMultiLine(BOOL bFlag);
		void SetFontName(const char* c_szFontName);
		void SetFontColor(DWORD dwColor);
		void SetLimitWidth(float fWidth);

		void ShowCursor();
		void HideCursor();
		int GetCursorPosition();

		void SetText(const char* c_szText);
		const char* GetText();

		void GetTextSize(int* pnWidth, int* pnHeight);

		WORD GetTextLineCount();
		WORD GetLineHeight();
		void SetLineHeight(int iHeight);

#if defined(WJ_MULTI_TEXTLINE)
		void DisableEnterToken();
#endif

	protected:
		void OnUpdate();
		void OnRender();
		void OnChangePosition();

		virtual void OnSetText(const char* c_szText);

	protected:
		CGraphicTextInstance m_TextInstance;
	};

	class CNumberLine : public CWindow
	{
	public:
		CNumberLine(PyObject* ppyObject);
		CNumberLine(CWindow* pParent);
		virtual ~CNumberLine();

		void SetPath(const char* c_szPath);
		void SetHorizontalAlign(int iType);
		void SetNumber(const char* c_szNumber);

	protected:
		void ClearNumber();
#if defined(ENABLE_CLIP_MASK)
		void OnUpdate();
#endif
		void OnRender();
		void OnChangePosition();

	protected:
		std::string m_strPath;
		std::string m_strNumber;
		std::vector<CGraphicImageInstance*> m_ImageInstanceVector;

		int m_iHorizontalAlign;
		DWORD m_dwWidthSummary;
	};

	// Image
	class CImageBox : public CWindow
	{
	public:
		CImageBox(PyObject* ppyObject);
		virtual ~CImageBox();

		BOOL LoadImage(const char* c_szFileName);
		void SetDiffuseColor(float fr, float fg, float fb, float fa);

		int GetWidth();
		int GetHeight();

		virtual void SetScale(float fx, float fy);

		void SetCoolTime(float fCoolTime);
		void SetCoolTimeStart(float fCoolTimeStart);

		void LeftRightReverse();

	protected:
		virtual void OnCreateInstance();
		virtual void OnDestroyInstance();

		virtual void OnUpdate();
		virtual void OnRender();
		void OnChangePosition();

	protected:
		CGraphicImageInstance* m_pImageInstance;
		float m_fCoolTime;
		float m_fCoolTimeStart;
	};

	class CMarkBox : public CWindow
	{
	public:
		CMarkBox(PyObject* ppyObject);
		virtual ~CMarkBox();

		void LoadImage(const char* c_szFilename);
		void SetDiffuseColor(float fr, float fg, float fb, float fa);
		void SetIndex(UINT uIndex);
		void SetScale(FLOAT fScale);

	protected:
		virtual void OnCreateInstance();
		virtual void OnDestroyInstance();

		virtual void OnUpdate();
		virtual void OnRender();
		void OnChangePosition();
	protected:
		CGraphicMarkInstance* m_pMarkInstance;
	};
	class CExpandedImageBox : public CImageBox
	{
	public:
		static DWORD Type();

	public:
		CExpandedImageBox(PyObject* ppyObject);
		virtual ~CExpandedImageBox();

		void SetScale(float fx, float fy);
		void SetOrigin(float fx, float fy);
		void SetRotation(float fRotation);
		void SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom);
		void SetRenderingRectWithScale(float fLeft, float fTop, float fRight, float fBottom);
		void SetRenderingMode(int iMode);
		void LeftRightReverse();

	protected:
		void OnCreateInstance();
		void OnDestroyInstance();

		virtual void OnUpdate();
		virtual void OnRender();

		BOOL OnIsType(DWORD dwType);
	};

	class CAniImageBox : public CWindow
	{
	public:
		static DWORD Type();

	public:
		CAniImageBox(PyObject* ppyObject);
		virtual ~CAniImageBox();

		void SetDelay(int iDelay);
#if defined(WJ_ENABLE_PICKUP_ITEM_EFFECT)
		void SetDiffuseColor(float r, float g, float b, float a);
#endif
		void AppendImage(const char* c_szFileName);
		void SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom);
		void SetRenderingRectWithScale(float fLeft, float fTop, float fRight, float fBottom);
		void SetRenderingMode(int iMode);
#if defined(ENABLE_FISHING_GAME)
		void SetRotation(float fRotation);
#endif

		void SetScale(float fx, float fy);
		void ResetFrame();

	protected:
		void OnUpdate();
		void OnRender();
		void OnChangePosition();
		virtual void OnEndFrame();
#if defined(ENABLE_MINI_GAME_RUMI)
		virtual void OnKeyFrame(int iCurFrame);
#endif

		BOOL OnIsType(DWORD dwType);

	protected:
		BYTE m_bycurDelay;
		BYTE m_byDelay;
		BYTE m_bycurIndex;
		BYTE m_bKeyFrame;
		std::vector<CGraphicExpandedImageInstance*> m_ImageVector;
#if defined(ENABLE_FISHING_GAME)
		std::function<void(CGraphicExpandedImageInstance*)> m_SetRotation;
#endif
#if defined(__ANI_IMAGE_PERFORMANCE__)
		std::queue<std::string> m_ImageFileNames;
		std::function<void(CGraphicExpandedImageInstance*)> m_SetRenderingRect, m_SetRenderingRectWithScale, m_SetRenderingMode, m_SetDiffuseColor;
#endif
	};

	// Button
	class CButton : public CWindow
	{
	public:
		CButton(PyObject* ppyObject);
		virtual ~CButton();

		BOOL SetUpVisual(const char* c_szFileName);
		BOOL SetOverVisual(const char* c_szFileName);
		BOOL SetDownVisual(const char* c_szFileName);
		BOOL SetDisableVisual(const char* c_szFileName);

		const char* GetUpVisualFileName();
		const char* GetOverVisualFileName();
		const char* GetDownVisualFileName();

		int GetButtonImageWidth() const;
		int	GetButtonImageHeight() const;

		void SetAlwaysToolTip(bool bVal);

		void Flash();
		void EnableFlash();
		void DisableFlash();
		void SetFlashVisual(CGraphicImageInstance* pVisual);

		void Enable();
		void Disable();

		BOOL IsDisable();
		BOOL IsPressed();

		void SetUp();
		void Up();
		void Over();
		void Down();

		void SetScale(float fx, float fy);
		void SetDiffuseColor(float fr, float fg, float fb, float fa);

		void LeftRightReverse();

	protected:
		void OnUpdate();
		void OnRender();
		void OnChangePosition();

		BOOL OnMouseLeftButtonDown();
		BOOL OnMouseLeftButtonDoubleClick();
		BOOL OnMouseLeftButtonUp();
		void OnMouseOverIn();
		void OnMouseOverOut();

		BOOL IsEnable();

		void SetCurrentVisual(CGraphicImageInstance* pVisual);

	protected:
		BOOL m_bEnable;
		BOOL m_isPressed;
		BOOL m_isFlash;
		BOOL m_isFlashing;

		CGraphicImageInstance* m_pcurVisual;
		CGraphicImageInstance m_upVisual;
		CGraphicImageInstance m_overVisual;
		CGraphicImageInstance m_downVisual;
		CGraphicImageInstance m_disableVisual;
		CGraphicImageInstance* m_pFlashVisual;
		bool m_bAlwaysTooltip;
	};
	class CRadioButton : public CButton
	{
	public:
		CRadioButton(PyObject* ppyObject);
		virtual ~CRadioButton();

	protected:
		BOOL OnMouseLeftButtonDown();
		BOOL OnMouseLeftButtonUp();
		void OnMouseOverIn();
		void OnMouseOverOut();
	};
	class CToggleButton : public CButton
	{
	public:
		CToggleButton(PyObject* ppyObject);
		virtual ~CToggleButton();

	protected:
		BOOL OnMouseLeftButtonDown();
		BOOL OnMouseLeftButtonUp();
		void OnMouseOverIn();
		void OnMouseOverOut();
	};
	class CDragButton : public CButton
	{
	public:
		CDragButton(PyObject* ppyObject);
		virtual ~CDragButton();

		void SetRestrictMovementArea(int ix, int iy, int iwidth, int iheight);

	protected:
		void OnChangePosition();
		void OnMouseOverIn();
		void OnMouseOverOut();

	protected:
		RECT m_restrictArea;
	};

#if defined(RENDER_TARGET)
	class CRenderTarget : public CWindow
	{
	public:
		CRenderTarget(PyObject* ppyObject);
		virtual ~CRenderTarget();

		void SetRenderTarget(int index);

	protected:
		void OnUpdate();
		void OnRender();

	private:
		int m_iIndex;
		CGraphicRenderTargetTexture* m_pRenderTargetTexture;
	};
#endif

	// MoveTextLine
	class CMoveTextLine : public CTextLine
	{
	public:
		CMoveTextLine(PyObject* ppyObject);
		virtual ~CMoveTextLine();

	public:
		static DWORD Type();

		void SetMoveSpeed(float fSpeed);
		void SetMovePosition(float fDstX, float fDstY);
		bool GetMove();
		void MoveStart();
		void MoveStop();

	protected:
		void OnUpdate();
		void OnRender();
		void OnEndMove();
		void OnChangePosition();

		BOOL OnIsType(DWORD dwType);

		D3DXVECTOR2 m_v2SrcPos, m_v2DstPos, m_v2NextPos, m_v2Direction, m_v2NextDistance;
		float m_fDistance, m_fMoveSpeed;
		bool m_bIsMove;
	};
	class CMoveImageBox : public CImageBox
	{
	public:
		CMoveImageBox(PyObject* ppyObject);
		virtual ~CMoveImageBox();

		static DWORD Type();

		void SetMoveSpeed(float fSpeed);
		void SetMovePosition(float fDstX, float fDstY);
		bool GetMove();
		void MoveStart();
		void MoveStop();

	protected:
		virtual void OnCreateInstance();
		virtual void OnDestroyInstance();

		virtual void OnUpdate();
		virtual void OnRender();
		virtual void OnEndMove();

		BOOL OnIsType(DWORD dwType);

		D3DXVECTOR2 m_v2SrcPos, m_v2DstPos, m_v2NextPos, m_v2Direction, m_v2NextDistance;
		float m_fDistance, m_fMoveSpeed;
		bool m_bIsMove;
	};
	class CMoveScaleImageBox : public CMoveImageBox
	{
	public:
		CMoveScaleImageBox(PyObject* ppyObject);
		virtual ~CMoveScaleImageBox();

		static DWORD Type();

		void SetMaxScale(float fMaxScale);
		void SetMaxScaleRate(float fMaxScaleRate);
		void SetScalePivotCenter(bool bScalePivotCenter);

	protected:
		virtual void OnCreateInstance();
		virtual void OnDestroyInstance();

		virtual void OnUpdate();

		BOOL OnIsType(DWORD dwType);

		float m_fMaxScale, m_fMaxScaleRate, m_fScaleDistance, m_fAdditionalScale;
		D3DXVECTOR2 m_v2CurScale;
	};

	class CCircle : public CWindow
	{
	public:
		CCircle(PyObject* ppyObject);
		virtual ~CCircle();

		void SetColor(DWORD dwColor);
		//void SetRadius(float fRadius);

	protected:
		void OnRender();

	protected:
		DWORD m_dwColor;
		//float m_fRadius;
		D3DXVECTOR2 m_v2Scale;
	};

};

extern BOOL g_bOutlineBoxEnable;
