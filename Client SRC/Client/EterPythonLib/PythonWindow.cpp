#include "StdAfx.h"
#include "../EterBase/CRC32.h"
#include "PythonWindow.h"
#include "PythonSlotWindow.h"
#include "PythonWindowManager.h"
#if defined(RENDER_TARGET)
#	include "../EterLib/RenderTargetManager.h"
#endif

BOOL g_bOutlineBoxEnable = FALSE;

namespace UI
{
	CWindow::CWindow(PyObject* ppyObject) :
		m_x(0),
		m_y(0),
		m_lWidth(0),
		m_lHeight(0),
		m_poHandler(ppyObject),
		m_bShow(false),
		m_pParent(NULL),
		m_dwFlag(0),
		m_isUpdatingChildren(FALSE)
	{
#ifdef _DEBUG
		static DWORD DEBUG_dwGlobalCounter = 0;
		DEBUG_dwCounter = DEBUG_dwGlobalCounter++;

		m_strName = "!!debug";
#endif
		m_bWindowType = WINDOW_TYPE_WINDOW;
		//assert(m_poHandler != NULL);
		m_HorizontalAlign = HORIZONTAL_ALIGN_LEFT;
		m_VerticalAlign = VERTICAL_ALIGN_TOP;
		m_rect.bottom = m_rect.left = m_rect.right = m_rect.top = 0;
		m_limitBiasRect.bottom = m_limitBiasRect.left = m_limitBiasRect.right = m_limitBiasRect.top = 0;
#if defined(ENABLE_CLIP_MASK)
		m_bEnableMask = false;
		m_pMaskWindow = nullptr;
		m_rMaskRect = {};
#endif
	}

	CWindow::~CWindow()
	{
	}

	DWORD CWindow::Type()
	{
		static DWORD s_dwType = GetCRC32("CWindow", strlen("CWindow"));
		return (s_dwType);
	}

	BOOL CWindow::IsType(DWORD dwType)
	{
		return OnIsType(dwType);
	}

	BOOL CWindow::OnIsType(DWORD dwType)
	{
		if (CWindow::Type() == dwType)
			return TRUE;

		return FALSE;
	}

	struct FClear
	{
		void operator () (CWindow* pWin)
		{
			pWin->Clear();
		}
	};

	void CWindow::Clear()
	{
		// FIXME : Children을 즉시 Delete하지는 않는다.
		// 어차피 Python쪽에서 Destroy가 하나씩 다시 호출 될 것이므로..
		// 하지만 만약을 위해 링크는 끊어 놓는다.
		// 더 좋은 형태는 있는가? - [levites]
		std::for_each(m_pChildList.begin(), m_pChildList.end(), FClear());
		m_pChildList.clear();

		m_pParent = NULL;
#if defined(ENABLE_CLIP_MASK)
		m_pMaskWindow = nullptr;
#endif

		DestroyHandle();
		Hide();
	}

	void CWindow::DestroyHandle()
	{
		m_poHandler = NULL;
	}

	void CWindow::Show()
	{
		m_bShow = true;
	}

	void CWindow::Hide()
	{
		m_bShow = false;
	}

#if defined(ENABLE_MOUSE_WHEEL_TOP_WINDOW)
	bool CWindow::OnMouseWheelButtonUp()
	{
		bool bValue = false;
		if (PyCallClassMemberFunc(m_poHandler, "OnMouseWheelButtonUp", BuildEmptyTuple(), &bValue))
			return bValue;

		return false;
	}

	bool CWindow::OnMouseWheelButtonDown()
	{
		bool bValue = false;
		if (PyCallClassMemberFunc(m_poHandler, "OnMouseWheelButtonDown", BuildEmptyTuple(), &bValue))
			return bValue;

		return false;
	}
#endif

	// NOTE : IsShow는 "자신이 보이는가?" 이지만, __IsShowing은 "자신이 그려지고 있는가?" 를 체크한다
	// 자신은 Show 지만 Tree 위쪽의 Parent 중 하나는 Hide 일 수 있으므로.. - [levites]
	bool CWindow::IsRendering()
	{
		if (!IsShow())
			return false;

		if (!m_pParent)
			return true;

		return m_pParent->IsRendering();
	}

	void CWindow::__RemoveReserveChildren()
	{
		if (m_pReserveChildList.empty())
			return;

		TWindowContainer::iterator it;
		for (it = m_pReserveChildList.begin(); it != m_pReserveChildList.end(); ++it)
		{
			m_pChildList.remove(*it);
		}
		m_pReserveChildList.clear();
	}

	void CWindow::Update()
	{
		if (!IsShow())
			return;

		__RemoveReserveChildren();

		OnUpdate();

		m_isUpdatingChildren = TRUE;
		TWindowContainer::iterator it = m_pChildList.begin();
		while (it != m_pChildList.end())
		{
			if ((*it))
			{
				(*it)->Update();
				++it;
			}
			else
			{
				it = m_pChildList.erase(it);
			}
		}
		m_isUpdatingChildren = FALSE;
	}

	void CWindow::Render()
	{
		if (!IsShow())
			return;

		OnRender();

		if (g_bOutlineBoxEnable)
		{
			CPythonGraphic::Instance().SetDiffuseColor(1.0f, 1.0f, 1.0f);
			CPythonGraphic::Instance().RenderBox2d(m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);
		}

		std::for_each(m_pChildList.begin(), m_pChildList.end(), std::mem_fn(&CWindow::Render));
	}

	void CWindow::OnUpdate()
	{
		if (!m_poHandler)
			return;

		if (!IsShow())
			return;

		static PyObject* poFuncName_OnUpdate = PyString_InternFromString("OnUpdate");

		PyCallClassMemberFunc(m_poHandler, "OnUpdate", BuildEmptyTuple());
		//PyCallClassMemberFunc_ByPyString(m_poHandler, poFuncName_OnUpdate, BuildEmptyTuple());

	}

	void CWindow::OnRender()
	{
		if (!m_poHandler)
			return;

		if (!IsShow())
			return;

		PyCallClassMemberFunc(m_poHandler, "OnRender", BuildEmptyTuple());
	}

	void CWindow::SetName(const char* c_szName)
	{
		m_strName = c_szName;
	}

	void CWindow::SetSize(long width, long height)
	{
		m_lWidth = width;
		m_lHeight = height;

		m_rect.right = m_rect.left + m_lWidth;
		m_rect.bottom = m_rect.top + m_lHeight;
	}

	void CWindow::SetHorizontalAlign(DWORD dwAlign)
	{
		m_HorizontalAlign = (EHorizontalAlign)dwAlign;
		UpdateRect();
	}

	void CWindow::SetVerticalAlign(DWORD dwAlign)
	{
		m_VerticalAlign = (EVerticalAlign)dwAlign;
		UpdateRect();
	}

	void CWindow::SetPosition(long x, long y)
	{
		m_x = x;
		m_y = y;

		UpdateRect();
	}

	void CWindow::GetPosition(long* plx, long* ply)
	{
		*plx = m_x;
		*ply = m_y;
	}

	long CWindow::UpdateRect()
	{
		m_rect.top = m_y;
		if (m_pParent)
		{
			switch (m_VerticalAlign)
			{
			case VERTICAL_ALIGN_BOTTOM:
				m_rect.top = m_pParent->GetHeight() - m_rect.top;
				break;
			case VERTICAL_ALIGN_CENTER:
				m_rect.top = (m_pParent->GetHeight() - GetHeight()) / 2 + m_rect.top;
				break;
			}
			m_rect.top += m_pParent->m_rect.top;
		}
		m_rect.bottom = m_rect.top + m_lHeight;

#if defined( _USE_CPP_RTL_FLIP )
		if (m_pParent == NULL) {
			m_rect.left = m_x;
			m_rect.right = m_rect.left + m_lWidth;
		}
		else {
			if (m_pParent->IsFlag(UI::CWindow::FLAG_RTL) == true) {
				m_rect.left = m_pParent->GetWidth() - m_lWidth - m_x;
				switch (m_HorizontalAlign)
				{
				case HORIZONTAL_ALIGN_RIGHT:
					m_rect.left = -m_x;
					break;
				case HORIZONTAL_ALIGN_CENTER:
					m_rect.left = m_pParent->GetWidth() / 2 - GetWidth() - m_x;
					break;
				}
				m_rect.left += m_pParent->m_rect.left;
				m_rect.right = m_rect.left + m_lWidth;
			}
			else {
				m_rect.left = m_x;
				switch (m_HorizontalAlign)
				{
				case HORIZONTAL_ALIGN_RIGHT:
					m_rect.left = m_pParent->GetWidth() - m_rect.left;
					break;
				case HORIZONTAL_ALIGN_CENTER:
					m_rect.left = (m_pParent->GetWidth() - GetWidth()) / 2 + m_rect.left;
					break;
				}
				m_rect.left += m_pParent->m_rect.left;
				m_rect.right = m_rect.left + m_lWidth;
			}
		}
#else
		m_rect.left = m_x;
		if (m_pParent)
		{
			switch (m_HorizontalAlign)
			{
			case HORIZONTAL_ALIGN_RIGHT:
				m_rect.left = ::abs(m_pParent->GetWidth()) - m_rect.left;
				break;
			case HORIZONTAL_ALIGN_CENTER:
				m_rect.left = m_pParent->GetWidth() / 2 - GetWidth() / 2 + m_rect.left;
				break;
			}
			m_rect.left += 0L < m_pParent->GetWidth() ? m_pParent->m_rect.left : m_pParent->m_rect.right + ::abs(m_pParent->GetWidth());
		}
		m_rect.right = m_rect.left + m_lWidth;
#endif
		std::for_each(m_pChildList.begin(), m_pChildList.end(), std::mem_fn(&CWindow::UpdateRect));

		OnChangePosition();

		return 1;
	}

	void CWindow::GetLocalPosition(long& rlx, long& rly)
	{
		rlx = rlx - m_rect.left;
		rly = rly - m_rect.top;
	}

	void CWindow::GetMouseLocalPosition(long& rlx, long& rly)
	{
		CWindowManager::Instance().GetMousePosition(rlx, rly);

		if (GetDefaultCodePage() == CP_ARABIC && m_bWindowType == WINDOW_TYPE_EX_IMAGE)
		{
			rlx = m_rect.right - rlx;
			rly -= m_rect.top;
		}
		else
		{
			rlx = rlx - m_rect.left;
			rly = rly - m_rect.top;
		}
	}

	void CWindow::AddChild(CWindow* pWin)
	{
		m_pChildList.push_back(pWin);
		pWin->m_pParent = this;
	}

	CWindow* CWindow::GetRoot()
	{
		if (m_pParent)
			if (m_pParent->IsWindow())
				return m_pParent->GetRoot();

		return this;
	}

	CWindow* CWindow::GetParent()
	{
		return m_pParent;
	}

	bool CWindow::IsChild(CWindow* pWin)
	{
		std::list<CWindow*>::iterator itor = m_pChildList.begin();

		while (itor != m_pChildList.end())
		{
			if (*itor == pWin)
				return true;

			++itor;
		}

		return false;
	}

	void CWindow::DeleteChild(CWindow* pWin)
	{
		if (m_isUpdatingChildren)
		{
			m_pReserveChildList.push_back(pWin);
		}
		else
		{
			m_pChildList.remove(pWin);
		}
	}

	void CWindow::SetTop(CWindow* pWin)
	{
		if (!pWin->IsFlag(CWindow::FLAG_FLOAT))
			return;

		TWindowContainer::iterator itor = std::find(m_pChildList.begin(), m_pChildList.end(), pWin);
		if (m_pChildList.end() != itor)
		{
			m_pChildList.push_back(*itor);
			m_pChildList.erase(itor);

			pWin->OnTop();
		}
		else
		{
			TraceError(" CWindow::SetTop - Failed to find child window\n");
		}
	}

	void CWindow::OnMouseDrag(long lx, long ly)
	{
		PyCallClassMemberFunc(m_poHandler, "OnMouseDrag", Py_BuildValue("(ii)", lx, ly));
	}

	void CWindow::OnMoveWindow(long lx, long ly)
	{
		PyCallClassMemberFunc(m_poHandler, "OnMoveWindow", Py_BuildValue("(ii)", lx, ly));
	}

	void CWindow::OnSetFocus()
	{
		//PyCallClassMemberFunc(m_poHandler, "OnSetFocus", BuildEmptyTuple());
		PyCallClassMemberFunc(m_poHandler, "OnSetFocus", BuildEmptyTuple());
	}

	void CWindow::OnKillFocus()
	{
		PyCallClassMemberFunc(m_poHandler, "OnKillFocus", BuildEmptyTuple());
	}

	void CWindow::OnMouseOverIn()
	{
		PyCallClassMemberFunc(m_poHandler, "OnMouseOverIn", BuildEmptyTuple());
	}

	void CWindow::OnMouseOverOut()
	{
		PyCallClassMemberFunc(m_poHandler, "OnMouseOverOut", BuildEmptyTuple());
	}

	void CWindow::OnMouseOver()
	{
	}

	void CWindow::OnDrop()
	{
		PyCallClassMemberFunc(m_poHandler, "OnDrop", BuildEmptyTuple());
	}

	void CWindow::OnTop()
	{
		PyCallClassMemberFunc(m_poHandler, "OnTop", BuildEmptyTuple());
	}

	void CWindow::OnIMEUpdate()
	{
		PyCallClassMemberFunc(m_poHandler, "OnIMEUpdate", BuildEmptyTuple());
	}

#if defined(ENABLE_CLIP_MASK)
	void CWindow::SetClippingMaskRect(const RECT& rMaskRect)
	{
		m_bEnableMask = true;
		m_rMaskRect = rMaskRect;

		TWindowContainer::const_reverse_iterator itor;
		for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)
		{
			CWindow* pWindow = *itor;

			if (pWindow)
				pWindow->SetClippingMaskRect(rMaskRect);
		}
	}

	void CWindow::SetClippingMaskWindow(CWindow* pMaskWindow)
	{
		if (!pMaskWindow)
			return;

		m_bEnableMask = true;
		m_pMaskWindow = pMaskWindow;

		TWindowContainer::const_reverse_iterator itor;
		for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)
		{
			CWindow* pWindow = *itor;

			if (pWindow)
				pWindow->SetClippingMaskWindow(pMaskWindow);
		}
	}
#endif

	BOOL CWindow::RunIMETabEvent()
	{
		if (!IsRendering())
			return FALSE;

		if (OnIMETabEvent())
			return TRUE;

		TWindowContainer::reverse_iterator itor;
		for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)
		{
			CWindow* pWindow = *itor;

			if (pWindow->RunIMETabEvent())
				return TRUE;
		}

		return FALSE;
	}

	BOOL CWindow::RunIMEReturnEvent()
	{
		if (!IsRendering())
			return FALSE;

		if (OnIMEReturnEvent())
			return TRUE;

		TWindowContainer::reverse_iterator itor;
		for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)
		{
			CWindow* pWindow = *itor;

			if (pWindow->RunIMEReturnEvent())
				return TRUE;
		}

		return FALSE;
	}

	BOOL CWindow::RunIMEKeyDownEvent(int ikey)
	{
		if (!IsRendering())
			return FALSE;

		if (OnIMEKeyDownEvent(ikey))
			return TRUE;

		TWindowContainer::reverse_iterator itor;
		for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)
		{
			CWindow* pWindow = *itor;

			if (pWindow->RunIMEKeyDownEvent(ikey))
				return TRUE;
		}

		return FALSE;
	}

	CWindow* CWindow::RunKeyDownEvent(int ikey)
	{
		if (OnKeyDown(ikey))
			return this;

		TWindowContainer::reverse_iterator itor;
		for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)
		{
			CWindow* pWindow = *itor;

			if (pWindow->IsShow())
			{
				CWindow* pProcessedWindow = pWindow->RunKeyDownEvent(ikey);
				if (NULL != pProcessedWindow)
				{
					return pProcessedWindow;
				}
			}
		}

		return NULL;
	}

	BOOL CWindow::RunKeyUpEvent(int ikey)
	{
		if (OnKeyUp(ikey))
			return TRUE;

		TWindowContainer::reverse_iterator itor;
		for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)
		{
			CWindow* pWindow = *itor;

			if (pWindow->IsShow())
				if (pWindow->RunKeyUpEvent(ikey))
					return TRUE;
		}

		return FALSE;
	}

	BOOL CWindow::RunPressEscapeKeyEvent()
	{
		TWindowContainer::reverse_iterator itor;
		for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)
		{
			CWindow* pWindow = *itor;

			if (pWindow->IsShow())
				if (pWindow->RunPressEscapeKeyEvent())
					return TRUE;
		}

		if (OnPressEscapeKey())
			return TRUE;

		return FALSE;
	}

	BOOL CWindow::RunPressExitKeyEvent()
	{
		TWindowContainer::reverse_iterator itor;
		for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)
		{
			CWindow* pWindow = *itor;

			if (pWindow->RunPressExitKeyEvent())
				return TRUE;

			if (pWindow->IsShow())
				if (pWindow->OnPressExitKey())
					return TRUE;
		}

		return FALSE;
	}

	BOOL CWindow::OnMouseLeftButtonDown()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnMouseLeftButtonDown", BuildEmptyTuple(), &lValue))
			if (0 != lValue)
				return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnMouseLeftButtonUp()
	{
		PyCallClassMemberFunc(m_poHandler, "OnMouseLeftButtonUp", BuildEmptyTuple());
		return TRUE; // NOTE : ButtonUp은 예외로 무조건 TRUE
	}

	BOOL CWindow::OnMouseLeftButtonDoubleClick()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnMouseLeftButtonDoubleClick", BuildEmptyTuple(), &lValue))
			if (0 != lValue)
				return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnMouseRightButtonDown()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnMouseRightButtonDown", BuildEmptyTuple(), &lValue))
			if (0 != lValue)
				return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnMouseRightButtonUp()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnMouseRightButtonUp", BuildEmptyTuple(), &lValue))
			if (0 != lValue)
				return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnMouseRightButtonDoubleClick()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnMouseRightButtonDoubleClick", BuildEmptyTuple(), &lValue))
			if (0 != lValue)
				return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnMouseMiddleButtonDown()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnMouseMiddleButtonDown", BuildEmptyTuple(), &lValue))
			if (0 != lValue)
				return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnMouseMiddleButtonUp()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnMouseMiddleButtonUp", BuildEmptyTuple(), &lValue))
			if (0 != lValue)
				return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnIMETabEvent()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnIMETab", BuildEmptyTuple(), &lValue))
			if (0 != lValue)
				return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnIMEReturnEvent()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnIMEReturn", BuildEmptyTuple(), &lValue))
			if (0 != lValue)
				return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnIMEKeyDownEvent(int ikey)
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnIMEKeyDown", Py_BuildValue("(i)", ikey), &lValue))
			if (0 != lValue)
				return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnIMEChangeCodePage()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnIMEChangeCodePage", BuildEmptyTuple(), &lValue))
			if (0 != lValue)
				return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnIMEOpenCandidateListEvent()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnIMEOpenCandidateList", BuildEmptyTuple(), &lValue))
			if (0 != lValue)
				return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnIMECloseCandidateListEvent()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnIMECloseCandidateList", BuildEmptyTuple(), &lValue))
			if (0 != lValue)
				return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnIMEOpenReadingWndEvent()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnIMEOpenReadingWnd", BuildEmptyTuple(), &lValue))
			if (0 != lValue)
				return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnIMECloseReadingWndEvent()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnIMECloseReadingWnd", BuildEmptyTuple(), &lValue))
			if (0 != lValue)
				return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnKeyDown(int ikey)
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnKeyDown", Py_BuildValue("(i)", ikey), &lValue))
			if (0 != lValue)
				return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnKeyUp(int ikey)
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnKeyUp", Py_BuildValue("(i)", ikey), &lValue))
			if (0 != lValue)
				return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnPressEscapeKey()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnPressEscapeKey", BuildEmptyTuple(), &lValue))
			if (0 != lValue)
				return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnPressExitKey()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnPressExitKey", BuildEmptyTuple(), &lValue))
			if (0 != lValue)
				return TRUE;

		return FALSE;
	}

	/////

	bool CWindow::IsIn(long x, long y)
	{
		if (x >= m_rect.left && x <= m_rect.right)
			if (y >= m_rect.top && y <= m_rect.bottom)
				return true;

		return false;
	}

	bool CWindow::IsIn()
	{
		long lx, ly;
		UI::CWindowManager::Instance().GetMousePosition(lx, ly);

		return IsIn(lx, ly);
	}

	CWindow* CWindow::PickWindow(long x, long y)
	{
		std::list<CWindow*>::reverse_iterator ritor = m_pChildList.rbegin();
		for (; ritor != m_pChildList.rend(); ++ritor)
		{
			CWindow* pWin = *ritor;
			if (pWin->IsShow())
			{
				if (!pWin->IsFlag(CWindow::FLAG_IGNORE_SIZE))
				{
					if (!pWin->IsIn(x, y)) {
						if (0L <= pWin->GetWidth()) {
							continue;
						}
					}
				}

				CWindow* pResult = pWin->PickWindow(x, y);
				if (pResult)
					return pResult;
			}
		}

		if (IsFlag(CWindow::FLAG_NOT_PICK))
			return NULL;

		return (this);
	}

	CWindow* CWindow::PickTopWindow(long x, long y)
	{
		std::list<CWindow*>::reverse_iterator ritor = m_pChildList.rbegin();
		for (; ritor != m_pChildList.rend(); ++ritor)
		{
			CWindow* pWin = *ritor;
			if (pWin->IsShow())
				if (pWin->IsIn(x, y))
					if (!pWin->IsFlag(CWindow::FLAG_NOT_PICK))
						return pWin;
		}

		return NULL;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	CBox::CBox(PyObject* ppyObject) : CWindow(ppyObject), m_dwColor(0xff000000)
	{
	}
	CBox::~CBox()
	{
	}

	void CBox::SetColor(DWORD dwColor)
	{
		m_dwColor = dwColor;
	}

	void CBox::OnRender()
	{
		CPythonGraphic::Instance().SetDiffuseColor(m_dwColor);
		CPythonGraphic::Instance().RenderBox2d(m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	CBar::CBar(PyObject* ppyObject) : CWindow(ppyObject), m_dwColor(0xff000000)
	{
	}
	CBar::~CBar()
	{
	}

	void CBar::SetColor(DWORD dwColor)
	{
		m_dwColor = dwColor;
	}

#if defined(ENABLE_CLIP_MASK)
	void CBar::OnUpdate()
	{
		if (!IsShow())
			return;

		if (m_bEnableMask && m_pMaskWindow)
			m_rMaskRect = m_pMaskWindow->GetRect();

		CWindow::OnUpdate();
	}
#endif

	void CBar::OnRender()
	{
		CPythonGraphic::Instance().SetDiffuseColor(m_dwColor);
#if defined(ENABLE_CLIP_MASK)
		if (m_bEnableMask)
		{
			CPythonGraphic::Instance().RenderBar2d(MINMAX(m_rMaskRect.left, m_rect.left, m_rMaskRect.right),
				MINMAX(m_rMaskRect.top, m_rect.top, m_rMaskRect.bottom),
				MINMAX(m_rMaskRect.left, m_rect.right, m_rMaskRect.right),
				MINMAX(m_rMaskRect.top, m_rect.bottom, m_rMaskRect.bottom));
		}
		else
		{
			CPythonGraphic::Instance().RenderBar2d(m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);
		}
#else
		CPythonGraphic::Instance().RenderBar2d(m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);
#endif
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	CLine::CLine(PyObject* ppyObject) : CWindow(ppyObject), m_dwColor(0xff000000)
	{
	}
	CLine::~CLine()
	{
	}

	void CLine::SetColor(DWORD dwColor)
	{
		m_dwColor = dwColor;
	}

	void CLine::OnRender()
	{
		CPythonGraphic& rkpyGraphic = CPythonGraphic::Instance();
		rkpyGraphic.SetDiffuseColor(m_dwColor);
		rkpyGraphic.RenderLine2d(m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	DWORD CBar3D::Type()
	{
		static DWORD s_dwType = GetCRC32("CBar3D", strlen("CBar3D"));
		return (s_dwType);
	}

	CBar3D::CBar3D(PyObject* ppyObject) : CWindow(ppyObject)
	{
		m_dwLeftColor = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
		m_dwRightColor = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f);
		m_dwCenterColor = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	}
	CBar3D::~CBar3D()
	{
	}

	BOOL CBar3D::OnIsType(DWORD dwType)
	{
		if (CBar3D::Type() == dwType)
			return TRUE;

		return FALSE;
	}
	void CBar3D::SetColor(DWORD dwLeft, DWORD dwRight, DWORD dwCenter)
	{
		m_dwLeftColor = dwLeft;
		m_dwRightColor = dwRight;
		m_dwCenterColor = dwCenter;
	}

#if defined(ENABLE_CLIP_MASK)
	void CBar3D::OnUpdate()
	{
		if (!IsShow())
			return;

		if (m_bEnableMask && m_pMaskWindow)
			m_rMaskRect = m_pMaskWindow->GetRect();

		CWindow::OnUpdate();
	}
#endif

	void CBar3D::OnRender()
	{
		CPythonGraphic& rkpyGraphic = CPythonGraphic::Instance();

#if defined(ENABLE_CLIP_MASK)
		if (m_bEnableMask)
		{
			if (m_rMaskRect.left >= m_rect.right || m_rMaskRect.right <= m_rect.left ||
				m_rMaskRect.top >= m_rect.bottom || m_rMaskRect.bottom <= m_rect.top)
				return;

			rkpyGraphic.SetDiffuseColor(m_dwCenterColor);
			rkpyGraphic.RenderBar2d(MAX(m_rect.left, m_rMaskRect.left),
				MAX(m_rect.top, m_rMaskRect.top),
				MIN(m_rect.right, m_rMaskRect.right),
				MIN(m_rect.bottom, m_rMaskRect.bottom));

			rkpyGraphic.SetDiffuseColor(m_dwLeftColor);
			if (m_rMaskRect.top <= m_rect.top && m_rMaskRect.bottom > m_rect.top)
				rkpyGraphic.RenderLine2d(MAX(m_rect.left, m_rMaskRect.left),
					m_rect.top,
					MIN(m_rect.right, m_rMaskRect.right),
					m_rect.top);

			if (m_rMaskRect.left <= m_rect.left && m_rMaskRect.right > m_rect.left)
				rkpyGraphic.RenderLine2d(m_rect.left,
					MAX(m_rect.top, m_rMaskRect.top),
					m_rect.left,
					MIN(m_rect.bottom, m_rMaskRect.bottom));

			rkpyGraphic.SetDiffuseColor(m_dwRightColor);
			if (m_rMaskRect.bottom > m_rect.bottom && m_rMaskRect.top <= m_rect.bottom)
				rkpyGraphic.RenderLine2d(MAX(m_rect.left, m_rMaskRect.left),
					m_rect.bottom,
					MIN(m_rect.right, m_rMaskRect.right),
					m_rect.bottom);

			if (m_rMaskRect.right > m_rect.right && m_rMaskRect.left <= m_rect.right)
				rkpyGraphic.RenderLine2d(m_rect.right,
					MAX(m_rect.top, m_rMaskRect.top),
					m_rect.right,
					MIN(m_rect.bottom, m_rMaskRect.bottom));
		}
		else
		{
			rkpyGraphic.SetDiffuseColor(m_dwCenterColor);
			rkpyGraphic.RenderBar2d(m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);

			rkpyGraphic.SetDiffuseColor(m_dwLeftColor);
			rkpyGraphic.RenderLine2d(m_rect.left, m_rect.top, m_rect.right, m_rect.top);
			rkpyGraphic.RenderLine2d(m_rect.left, m_rect.top, m_rect.left, m_rect.bottom);

			rkpyGraphic.SetDiffuseColor(m_dwRightColor);
			rkpyGraphic.RenderLine2d(m_rect.left, m_rect.bottom, m_rect.right, m_rect.bottom);
			rkpyGraphic.RenderLine2d(m_rect.right, m_rect.top, m_rect.right, m_rect.bottom);
		}
#else
		rkpyGraphic.SetDiffuseColor(m_dwCenterColor);
		rkpyGraphic.RenderBar2d(m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);

		rkpyGraphic.SetDiffuseColor(m_dwLeftColor);
		rkpyGraphic.RenderLine2d(m_rect.left, m_rect.top, m_rect.right, m_rect.top);
		rkpyGraphic.RenderLine2d(m_rect.left, m_rect.top, m_rect.left, m_rect.bottom);

		rkpyGraphic.SetDiffuseColor(m_dwRightColor);
		rkpyGraphic.RenderLine2d(m_rect.left, m_rect.bottom, m_rect.right, m_rect.bottom);
		rkpyGraphic.RenderLine2d(m_rect.right, m_rect.top, m_rect.right, m_rect.bottom);
#endif
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	CTextLine::CTextLine(PyObject* ppyObject) : CWindow(ppyObject)
	{
		m_bWindowType = WINDOW_TYPE_TEXTLINE;
		m_TextInstance.SetColor(0.78f, 0.78f, 0.78f);
		m_TextInstance.SetHorizonalAlign(CGraphicTextInstance::HORIZONTAL_ALIGN_LEFT);
		m_TextInstance.SetVerticalAlign(CGraphicTextInstance::VERTICAL_ALIGN_TOP);
	}
	CTextLine::~CTextLine()
	{
		m_TextInstance.Destroy();
	}

	void CTextLine::SetMax(int iMax)
	{
		m_TextInstance.SetMax(iMax);
	}
	void CTextLine::SetHorizontalAlign(int iType)
	{
		m_TextInstance.SetHorizonalAlign(iType);
	}
	void CTextLine::SetVerticalAlign(int iType)
	{
		m_TextInstance.SetVerticalAlign(iType);
	}
	void CTextLine::SetSecret(BOOL bFlag)
	{
		m_TextInstance.SetSecret(bFlag ? true : false);
	}
	void CTextLine::SetOutline(BOOL bFlag)
	{
		m_TextInstance.SetOutline(bFlag ? true : false);
	}
	void CTextLine::SetOutlineColor(float fRed, float fGreen, float fBlue, float fAlpha)
	{
		m_TextInstance.SetOutlineColor(fRed, fGreen, fBlue, fAlpha);
	}
	void CTextLine::SetOutlineColor(DWORD dwColor)
	{
		m_TextInstance.SetOutlineColor(dwColor);
	}
	void CTextLine::SetFeather(BOOL bFlag)
	{
		m_TextInstance.SetFeather(bFlag ? true : false);
	}
	void CTextLine::SetMultiLine(BOOL bFlag)
	{
		m_TextInstance.SetMultiLine(bFlag ? true : false);
	}
	WORD CTextLine::GetTextLineCount()
	{
		return m_TextInstance.GetTextLineCount();
	}
	WORD CTextLine::GetLineHeight()
	{
		return m_TextInstance.GetLineHeight();
	}
	void CTextLine::SetLineHeight(int iHeight)
	{
		m_TextInstance.SetLineHeight(iHeight);
		m_TextInstance.Update();
	}
#if defined(WJ_MULTI_TEXTLINE)
	void CTextLine::DisableEnterToken()
	{
		m_TextInstance.DisableEnterToken();
	}
#endif
	void CTextLine::SetFontName(const char* c_szFontName)
	{
		std::string stFontName = c_szFontName;
		stFontName += ".fnt";

		CResourceManager& rkResMgr = CResourceManager::Instance();
		CResource* pkRes = rkResMgr.GetTypeResourcePointer(stFontName.c_str());
		CGraphicText* pkResFont = static_cast<CGraphicText*>(pkRes);
		m_TextInstance.SetTextPointer(pkResFont);
	}
	void CTextLine::SetFontColor(DWORD dwColor)
	{
		m_TextInstance.SetColor(dwColor);
	}
	void CTextLine::SetLimitWidth(float fWidth)
	{
		m_TextInstance.SetLimitWidth(fWidth);
	}
	void CTextLine::SetText(const char* c_szText)
	{
		OnSetText(c_szText);
	}
	void CTextLine::GetTextSize(int* pnWidth, int* pnHeight)
	{
		m_TextInstance.GetTextSize(pnWidth, pnHeight);
	}
	const char* CTextLine::GetText()
	{
		return m_TextInstance.GetValueStringReference().c_str();
	}
	void CTextLine::ShowCursor()
	{
		m_TextInstance.ShowCursor();
	}
	void CTextLine::HideCursor()
	{
		m_TextInstance.HideCursor();
	}
	int CTextLine::GetCursorPosition()
	{
		long lx, ly;
		CWindow::GetMouseLocalPosition(lx, ly);
		return m_TextInstance.PixelPositionToCharacterPosition(lx);
	}

	void CTextLine::OnSetText(const char* c_szText)
	{
		m_TextInstance.SetValue(c_szText);
		m_TextInstance.Update();
	}

#if defined(ENABLE_CLIP_MASK)
	void CTextLine::OnUpdate()
	{
		if (!IsShow())
			return;

		if (m_bEnableMask && m_pMaskWindow)
			m_rMaskRect = m_pMaskWindow->GetRect();

		m_TextInstance.Update();
	}
	void CTextLine::OnRender()
	{
		if (IsShow())
			m_TextInstance.Render(m_bEnableMask ? &m_rMaskRect : nullptr);
	}
#else
	void CTextLine::OnUpdate()
	{
		if (IsShow())
			m_TextInstance.Update();
	}
	void CTextLine::OnRender()
	{
		if (IsShow())
			m_TextInstance.Render();
	}
#endif

	void CTextLine::OnChangePosition()
	{
		// FOR_ARABIC_ALIGN
		//if (m_TextInstance.GetHorizontalAlign() == CGraphicTextInstance::HORIZONTAL_ALIGN_ARABIC)
		if (GetDefaultCodePage() == CP_ARABIC)
		{
			m_TextInstance.SetPosition(m_rect.right, m_rect.top);
		}
		else
		{
			m_TextInstance.SetPosition(m_rect.left, m_rect.top);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	CNumberLine::CNumberLine(PyObject* ppyObject) : CWindow(ppyObject)
	{
		m_strPath = "d:/ymir work/ui/game/taskbar/";
		m_iHorizontalAlign = HORIZONTAL_ALIGN_LEFT;
		m_dwWidthSummary = 0;
	}
	CNumberLine::CNumberLine(CWindow* pParent) : CWindow(NULL)
	{
		m_strPath = "d:/ymir work/ui/game/taskbar/";
		m_iHorizontalAlign = HORIZONTAL_ALIGN_LEFT;
		m_dwWidthSummary = 0;

		m_pParent = pParent;
	}
	CNumberLine::~CNumberLine()
	{
		ClearNumber();
	}

	void CNumberLine::SetPath(const char* c_szPath)
	{
		m_strPath = c_szPath;
	}
	void CNumberLine::SetHorizontalAlign(int iType)
	{
		m_iHorizontalAlign = iType;
	}
	void CNumberLine::SetNumber(const char* c_szNumber)
	{
		if (0 == m_strNumber.compare(c_szNumber))
			return;

		ClearNumber();

		m_strNumber = c_szNumber;

		for (DWORD i = 0; i < m_strNumber.size(); ++i)
		{
			char cChar = m_strNumber[i];
			std::string strImageFileName;

			if (':' == cChar)
			{
				strImageFileName = m_strPath + "colon.sub";
			}
			else if ('?' == cChar)
			{
				strImageFileName = m_strPath + "questionmark.sub";
			}
			else if ('/' == cChar)
			{
				strImageFileName = m_strPath + "slash.sub";
			}
			else if ('%' == cChar)
			{
				strImageFileName = m_strPath + "percent.sub";
			}
			else if ('+' == cChar)
			{
				strImageFileName = m_strPath + "plus.sub";
			}
			else if ('m' == cChar)
			{
				strImageFileName = m_strPath + "m.sub";
			}
			else if ('g' == cChar)
			{
				strImageFileName = m_strPath + "g.sub";
			}
			else if ('p' == cChar)
			{
				strImageFileName = m_strPath + "p.sub";
			}
			else if (cChar >= '0' && cChar <= '9')
			{
				strImageFileName = m_strPath;
				strImageFileName += cChar;
				strImageFileName += ".sub";
			}
			else
				continue;

			if (!CResourceManager::Instance().IsFileExist(strImageFileName.c_str()))
				continue;

			CGraphicImage* pImage = (CGraphicImage*)CResourceManager::Instance().GetResourcePointer(strImageFileName.c_str());

			CGraphicImageInstance* pInstance = CGraphicImageInstance::New();
			pInstance->SetImagePointer(pImage);
			m_ImageInstanceVector.push_back(pInstance);

			m_dwWidthSummary += pInstance->GetWidth();
		}
	}

	void CNumberLine::ClearNumber()
	{
		m_ImageInstanceVector.clear();
		m_dwWidthSummary = 0;
		m_strNumber = "";
	}

#if defined(ENABLE_CLIP_MASK)
	void CNumberLine::OnUpdate()
	{
		if (!IsShow())
			return;

		if (m_bEnableMask && m_pMaskWindow)
			m_rMaskRect = m_pMaskWindow->GetRect();

		CWindow::OnUpdate();
	}
#endif

	void CNumberLine::OnRender()
	{
		for (DWORD i = 0; i < m_ImageInstanceVector.size(); ++i)
		{
#if defined(ENABLE_CLIP_MASK)
			m_ImageInstanceVector[i]->Render(m_bEnableMask ? &m_rMaskRect : NULL);
#else
			m_ImageInstanceVector[i]->Render();
#endif
		}
	}

	void CNumberLine::OnChangePosition()
	{
		int ix = m_x;
		int iy = m_y;

		if (m_pParent)
		{
			ix = m_rect.left;
			iy = m_rect.top;
		}

		switch (m_iHorizontalAlign)
		{
		case HORIZONTAL_ALIGN_LEFT:
			break;
		case HORIZONTAL_ALIGN_CENTER:
			ix -= int(m_dwWidthSummary) / 2;
			break;
		case HORIZONTAL_ALIGN_RIGHT:
			ix -= int(m_dwWidthSummary);
			break;
		}

		for (DWORD i = 0; i < m_ImageInstanceVector.size(); ++i)
		{
			m_ImageInstanceVector[i]->SetPosition(ix, iy);
			ix += m_ImageInstanceVector[i]->GetWidth();
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	CImageBox::CImageBox(PyObject* ppyObject) :
		CWindow(ppyObject),
		m_fCoolTime(0.0f),
		m_fCoolTimeStart(0.0f)
	{
		m_pImageInstance = NULL;
	}
	CImageBox::~CImageBox()
	{
		OnDestroyInstance();
	}

	void CImageBox::OnCreateInstance()
	{
		OnDestroyInstance();

		m_pImageInstance = CGraphicImageInstance::New();
	}
	void CImageBox::OnDestroyInstance()
	{
		if (m_pImageInstance)
		{
			CGraphicImageInstance::Delete(m_pImageInstance);
			m_pImageInstance = NULL;
		}
	}

	BOOL CImageBox::LoadImage(const char* c_szFileName)
	{
		if (!c_szFileName[0])
			return FALSE;

		OnCreateInstance();

		CResource* pResource = CResourceManager::Instance().GetResourcePointer(c_szFileName);
		if (!pResource)
			return FALSE;
		if (!pResource->IsType(CGraphicImage::Type()))
			return FALSE;

		m_pImageInstance->SetImagePointer(static_cast<CGraphicImage*>(pResource));
		if (m_pImageInstance->IsEmpty())
			return FALSE;

		SetSize(m_pImageInstance->GetWidth(), m_pImageInstance->GetHeight());
		UpdateRect();

		return TRUE;
	}

	void CImageBox::SetDiffuseColor(float fr, float fg, float fb, float fa)
	{
		if (!m_pImageInstance)
			return;

		m_pImageInstance->SetDiffuseColor(fr, fg, fb, fa);
	}

	int CImageBox::GetWidth()
	{
		if (!m_pImageInstance)
			return 0;

		return m_pImageInstance->GetWidth();
	}

	int CImageBox::GetHeight()
	{
		if (!m_pImageInstance)
			return 0;

		return m_pImageInstance->GetHeight();
	}

	void CImageBox::SetScale(float fx, float fy)
	{
		if (!m_pImageInstance)
			return;

		m_pImageInstance->SetScale(fx, fy);
		CWindow::SetSize(long(float(GetWidth()) * fx), long(float(GetHeight()) * fy));
	}

#if defined(ENABLE_CLIP_MASK)
	void CImageBox::OnUpdate()
	{
		if (!m_pImageInstance)
			return;

		if (!IsShow())
			return;

		if (!m_bEnableMask)
			return;

		if (!m_pMaskWindow)
			return;

		m_rMaskRect = m_pMaskWindow->GetRect();
	}
	void CImageBox::OnRender()
	{
		if (!m_pImageInstance)
			return;

		if (IsShow())
			m_pImageInstance->Render(m_bEnableMask ? &m_rMaskRect : nullptr);
	}
#else
	void CImageBox::OnUpdate()
	{
	}
	void CImageBox::OnRender()
	{
		if (!m_pImageInstance)
			return;

		if (IsShow())
		{
			if (m_fCoolTime == 0.0f)
				m_pImageInstance->Render();
			else
				m_pImageInstance->RenderCoolTime((CTimer::Instance().GetCurrentSecond() - m_fCoolTimeStart) / m_fCoolTime);
		}
	}
#endif

	void CImageBox::OnChangePosition()
	{
		if (!m_pImageInstance)
			return;

		m_pImageInstance->SetPosition(m_rect.left, m_rect.top);
	}

	void CImageBox::SetCoolTime(float fCoolTime)
	{
		m_fCoolTime = fCoolTime;
	}

	void CImageBox::SetCoolTimeStart(float fCoolTimeStart)
	{
		m_fCoolTimeStart = fCoolTimeStart;
	}

	void CImageBox::LeftRightReverse()
	{
		if (!m_pImageInstance)
			return;

		m_pImageInstance->LeftRightReverse();
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	// MarkBox - 마크 출력용 UI 윈도우
	///////////////////////////////////////////////////////////////////////////////////////////////
	CMarkBox::CMarkBox(PyObject* ppyObject) : CWindow(ppyObject)
	{
		m_pMarkInstance = NULL;
	}

	CMarkBox::~CMarkBox()
	{
		OnDestroyInstance();
	}

	void CMarkBox::OnCreateInstance()
	{
		OnDestroyInstance();
		m_pMarkInstance = CGraphicMarkInstance::New();
	}

	void CMarkBox::OnDestroyInstance()
	{
		if (m_pMarkInstance)
		{
			CGraphicMarkInstance::Delete(m_pMarkInstance);
			m_pMarkInstance = NULL;
		}
	}

	void CMarkBox::LoadImage(const char* c_szFilename)
	{
		OnCreateInstance();

		m_pMarkInstance->SetImageFileName(c_szFilename);
		m_pMarkInstance->Load();
		SetSize(m_pMarkInstance->GetWidth(), m_pMarkInstance->GetHeight());

		UpdateRect();
	}

	void CMarkBox::SetScale(FLOAT fScale)
	{
		if (!m_pMarkInstance)
			return;

		m_pMarkInstance->SetScale(fScale);
	}

	void CMarkBox::SetIndex(UINT uIndex)
	{
		if (!m_pMarkInstance)
			return;

		m_pMarkInstance->SetIndex(uIndex);
	}

	void CMarkBox::SetDiffuseColor(float fr, float fg, float fb, float fa)
	{
		if (!m_pMarkInstance)
			return;

		m_pMarkInstance->SetDiffuseColor(fr, fg, fb, fa);
	}

#if defined(ENABLE_CLIP_MASK)
	void CMarkBox::OnUpdate()
	{
		if (!m_pMarkInstance)
			return;

		if (!IsShow())
			return;

		if (!m_bEnableMask)
			return;

		if (!m_pMaskWindow)
			return;

		m_rMaskRect = m_pMaskWindow->GetRect();
	}

	void CMarkBox::OnRender()
	{
		if (!m_pMarkInstance)
			return;

		if (IsShow())
			m_pMarkInstance->Render(m_bEnableMask ? &m_rMaskRect : NULL);
	}
#else
	void CMarkBox::OnUpdate()
	{
	}

	void CMarkBox::OnRender()
	{
		if (!m_pMarkInstance)
			return;

		if (IsShow())
			m_pMarkInstance->Render();
	}
#endif

	void CMarkBox::OnChangePosition()
	{
		if (!m_pMarkInstance)
			return;

		m_pMarkInstance->SetPosition(m_rect.left, m_rect.top);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	DWORD CExpandedImageBox::Type()
	{
		static DWORD s_dwType = GetCRC32("CExpandedImageBox", strlen("CExpandedImageBox"));
		return (s_dwType);
	}

	BOOL CExpandedImageBox::OnIsType(DWORD dwType)
	{
		if (CExpandedImageBox::Type() == dwType)
			return TRUE;

		return FALSE;
	}

	CExpandedImageBox::CExpandedImageBox(PyObject* ppyObject) : CImageBox(ppyObject)
	{
		m_bWindowType = WINDOW_TYPE_EX_IMAGE;
	}
	CExpandedImageBox::~CExpandedImageBox()
	{
		OnDestroyInstance();
	}

	void CExpandedImageBox::OnCreateInstance()
	{
		OnDestroyInstance();

		m_pImageInstance = CGraphicExpandedImageInstance::New();
	}
	void CExpandedImageBox::OnDestroyInstance()
	{
		if (m_pImageInstance)
		{
			CGraphicExpandedImageInstance::Delete((CGraphicExpandedImageInstance*)m_pImageInstance);
			m_pImageInstance = NULL;
		}
	}

	void CExpandedImageBox::SetScale(float fx, float fy)
	{
		if (!m_pImageInstance)
			return;

		((CGraphicExpandedImageInstance*)m_pImageInstance)->SetScale(fx, fy);
		CWindow::SetSize(long(float(GetWidth()) * fx), long(float(GetHeight()) * fy));
	}
	void CExpandedImageBox::SetOrigin(float fx, float fy)
	{
		if (!m_pImageInstance)
			return;

		((CGraphicExpandedImageInstance*)m_pImageInstance)->SetOrigin(fx, fy);
	}
	void CExpandedImageBox::SetRotation(float fRotation)
	{
		if (!m_pImageInstance)
			return;

		((CGraphicExpandedImageInstance*)m_pImageInstance)->SetRotation(fRotation);
	}
	void CExpandedImageBox::SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom)
	{
		if (!m_pImageInstance)
			return;

		((CGraphicExpandedImageInstance*)m_pImageInstance)->SetRenderingRect(fLeft, fTop, fRight, fBottom);
	}
	void CExpandedImageBox::SetRenderingRectWithScale(float fLeft, float fTop, float fRight, float fBottom)
	{
		if (!m_pImageInstance)
			return;

		((CGraphicExpandedImageInstance*)m_pImageInstance)->SetRenderingRectWithScale(fLeft, fTop, fRight, fBottom);
	}

	void CExpandedImageBox::SetRenderingMode(int iMode)
	{
		((CGraphicExpandedImageInstance*)m_pImageInstance)->SetRenderingMode(iMode);
	}

#if defined(ENABLE_CLIP_MASK)
	void CExpandedImageBox::OnUpdate()
	{
		if (!m_pImageInstance)
			return;

		if (!IsShow())
			return;

		if (!m_bEnableMask)
			return;

		if (!m_pMaskWindow)
			return;

		m_rMaskRect = m_pMaskWindow->GetRect();
	}
	void CExpandedImageBox::OnRender()
	{
		if (!m_pImageInstance)
			return;

		if (IsShow())
			m_pImageInstance->Render(m_bEnableMask ? &m_rMaskRect : nullptr);
	}
#else
	void CExpandedImageBox::OnUpdate()
	{
	}
	void CExpandedImageBox::OnRender()
	{
		if (!m_pImageInstance)
			return;

		if (IsShow())
		{
			if (m_fCoolTime == 0.0f)
				m_pImageInstance->Render();
			else
				((CGraphicExpandedImageInstance*)m_pImageInstance)->RenderCoolTime((CTimer::Instance().GetCurrentSecond() - m_fCoolTimeStart) / m_fCoolTime);
		}
	}
#endif

	void CExpandedImageBox::LeftRightReverse()
	{
		((CGraphicExpandedImageInstance*)m_pImageInstance)->LeftRightReverse();
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	DWORD CAniImageBox::Type()
	{
		static DWORD s_dwType = GetCRC32("CAniImageBox", strlen("CAniImageBox"));
		return (s_dwType);
	}

	BOOL CAniImageBox::OnIsType(DWORD dwType)
	{
		if (CAniImageBox::Type() == dwType)
			return TRUE;

		return FALSE;
	}

	CAniImageBox::CAniImageBox(PyObject* ppyObject)
		: CWindow(ppyObject),
		m_bycurDelay(0),
		m_byDelay(4),
		m_bycurIndex(0)
	{
		m_ImageVector.clear();
	}
	CAniImageBox::~CAniImageBox()
	{
		for_each(m_ImageVector.begin(), m_ImageVector.end(), CGraphicExpandedImageInstance::DeleteExpandedImageInstance);
	}

	void CAniImageBox::SetDelay(int iDelay)
	{
		m_byDelay = iDelay;
	}

#if defined(WJ_ENABLE_PICKUP_ITEM_EFFECT)
	void CAniImageBox::SetDiffuseColor(float r, float g, float b, float a)
	{
		for (CGraphicExpandedImageInstance* pImageInstance : m_ImageVector)
			pImageInstance->SetDiffuseColor(r, g, b, a);
	}
#endif

	void CAniImageBox::AppendImage(const char* c_szFileName)
	{
#if defined(__ANI_IMAGE_PERFORMANCE__)
		m_ImageFileNames.push(c_szFileName);
#else
		CResource* pResource = CResourceManager::Instance().GetResourcePointer(c_szFileName);
		if (!pResource->IsType(CGraphicImage::Type()))
			return;

		CGraphicExpandedImageInstance* pImageInstance = CGraphicExpandedImageInstance::New();
		pImageInstance->SetImagePointer(static_cast<CGraphicImage*>(pResource));
		if (pImageInstance->IsEmpty())
		{
			CGraphicExpandedImageInstance::Delete(pImageInstance);
			return;
		}

		m_ImageVector.push_back(pImageInstance);
		m_bycurIndex = rand() % static_cast<BYTE>(m_ImageVector.size());
#endif
	}

#if defined(__ANI_IMAGE_PERFORMANCE__)
	void CAniImageBox::SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom)
	{
		m_SetRenderingRect = std::bind(&CGraphicExpandedImageInstance::SetRenderingRect, std::placeholders::_1, fLeft, fTop, fRight, fBottom);
		std::for_each(m_ImageVector.begin(), m_ImageVector.end(), m_SetRenderingRect);
	}

	void CAniImageBox::SetRenderingRectWithScale(float fLeft, float fTop, float fRight, float fBottom)
	{
		m_SetRenderingRectWithScale = std::bind(&CGraphicExpandedImageInstance::SetRenderingRectWithScale, std::placeholders::_1, fLeft, fTop, fRight, fBottom);
		std::for_each(m_ImageVector.begin(), m_ImageVector.end(), m_SetRenderingRectWithScale);
	}
#else
	struct FSetRenderingRect
	{
		float fLeft, fTop, fRight, fBottom;
		void operator () (CGraphicExpandedImageInstance* pInstance)
		{
			pInstance->SetRenderingRect(fLeft, fTop, fRight, fBottom);
		}
	};
	void CAniImageBox::SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom)
	{
		FSetRenderingRect setRenderingRect;
		setRenderingRect.fLeft = fLeft;
		setRenderingRect.fTop = fTop;
		setRenderingRect.fRight = fRight;
		setRenderingRect.fBottom = fBottom;
		for_each(m_ImageVector.begin(), m_ImageVector.end(), setRenderingRect);
	}

	struct FSetRenderingRectWithScale
	{
		float fLeft, fTop, fRight, fBottom;
		void operator () (CGraphicExpandedImageInstance* pInstance)
		{
			pInstance->SetRenderingRectWithScale(fLeft, fTop, fRight, fBottom);
		}
	};

	void CAniImageBox::SetRenderingRectWithScale(float fLeft, float fTop, float fRight, float fBottom)
	{
		FSetRenderingRectWithScale setRenderingRect;
		setRenderingRect.fLeft = fLeft;
		setRenderingRect.fTop = fTop;
		setRenderingRect.fRight = fRight;
		setRenderingRect.fBottom = fBottom;
		for_each(m_ImageVector.begin(), m_ImageVector.end(), setRenderingRect);
	}
#endif

#if defined(__ANI_IMAGE_PERFORMANCE__)
	void CAniImageBox::SetRenderingMode(int iMode)
	{
		m_SetRenderingMode = std::bind(&CGraphicExpandedImageInstance::SetRenderingMode, std::placeholders::_1, iMode);
		std::for_each(m_ImageVector.begin(), m_ImageVector.end(), m_SetRenderingMode);
	}
#else
	struct FSetRenderingMode
	{
		int iMode;
		void operator () (CGraphicExpandedImageInstance* pInstance)
		{
			pInstance->SetRenderingMode(iMode);
		}
	};
	void CAniImageBox::SetRenderingMode(int iMode)
	{
		FSetRenderingMode setRenderingMode;
		setRenderingMode.iMode = iMode;
		for_each(m_ImageVector.begin(), m_ImageVector.end(), setRenderingMode);
	}
#endif

	void CAniImageBox::SetScale(float fx, float fy)
	{
		for (CGraphicExpandedImageInstance* pInstance : m_ImageVector)
			pInstance->SetScale(fx, fy);
	}

#if defined(ENABLE_FISHING_GAME)
	void CAniImageBox::SetRotation(float fRotation)
	{
		m_SetRotation = std::bind(&CGraphicExpandedImageInstance::SetRotation, std::placeholders::_1, fRotation);
		std::for_each(m_ImageVector.begin(), m_ImageVector.end(), m_SetRotation);
	}
#endif

	void CAniImageBox::ResetFrame()
	{
		m_bycurIndex = 0;
	}

	void CAniImageBox::OnUpdate()
	{
#if defined(ENABLE_CLIP_MASK)
		if (m_bEnableMask && m_pMaskWindow)
			m_rMaskRect = m_pMaskWindow->GetRect();
#endif

#if defined(__ANI_IMAGE_PERFORMANCE__)
		if (!m_ImageFileNames.empty())
		{
			const std::string& stFileName = m_ImageFileNames.front();
			CResource* pResource = CResourceManager::Instance().GetResourcePointer(stFileName.c_str());
			if (pResource->IsType(CGraphicImage::Type()))
			{
				CGraphicExpandedImageInstance* pImageInstance = CGraphicExpandedImageInstance::New();
				pImageInstance->SetImagePointer(static_cast<CGraphicImage*>(pResource));
				if (pImageInstance->IsEmpty())
				{
					CGraphicExpandedImageInstance::Delete(pImageInstance);
				}
				else
				{
					pImageInstance->SetPosition(m_rect.left, m_rect.top);

					if (m_SetRenderingRect)
						m_SetRenderingRect(pImageInstance);

					if (m_SetRenderingMode)
						m_SetRenderingMode(pImageInstance);

					if (m_SetDiffuseColor)
						m_SetDiffuseColor(pImageInstance);

#if defined(ENABLE_FISHING_GAME)
					if (m_SetRotation)
						m_SetRotation(pImageInstance);
#endif

					m_ImageVector.push_back(pImageInstance);
				}
			}
			m_ImageFileNames.pop();
		}
#endif

		++m_bycurDelay;
		if (m_bycurDelay < m_byDelay)
			return;

		m_bycurDelay = 0;
#if defined(ENABLE_MINI_GAME_RUMI)
		OnKeyFrame(m_bycurIndex);
#endif

		++m_bycurIndex;
		if (m_bycurIndex >= m_ImageVector.size())
		{
			m_bycurIndex = 0;

			OnEndFrame();
		}
	}

	void CAniImageBox::OnRender()
	{
		if (m_bycurIndex < m_ImageVector.size())
		{
			CGraphicExpandedImageInstance* pImage = m_ImageVector[m_bycurIndex];
#if defined(ENABLE_CLIP_MASK)
			pImage->Render(m_bEnableMask ? &m_rMaskRect : NULL);
#else
			pImage->Render();
#endif
		}
	}

#if defined(__ANI_IMAGE_PERFORMANCE__)
	void CAniImageBox::OnChangePosition()
	{
		std::for_each(m_ImageVector.begin(), m_ImageVector.end(),
			std::bind(&CGraphicExpandedImageInstance::SetPosition, std::placeholders::_1, m_rect.left, m_rect.top));
	}
#else
	struct FChangePosition
	{
		float fx, fy;
		void operator () (CGraphicExpandedImageInstance* pInstance)
		{
			pInstance->SetPosition(fx, fy);
		}
	};
	void CAniImageBox::OnChangePosition()
	{
		FChangePosition changePosition;
		changePosition.fx = m_rect.left;
		changePosition.fy = m_rect.top;
		for_each(m_ImageVector.begin(), m_ImageVector.end(), changePosition);
	}
#endif

	void CAniImageBox::OnEndFrame()
	{
		PyCallClassMemberFunc(m_poHandler, "OnEndFrame", BuildEmptyTuple());
	}
	
#if defined(ENABLE_MINI_GAME_RUMI)
	void CAniImageBox::OnKeyFrame(int iCurFrame)
	{
		PyCallClassMemberFunc(m_poHandler, "OnKeyFrame", Py_BuildValue("(i)", iCurFrame));
	}
#endif

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	CButton::CButton(PyObject* ppyObject) : CWindow(ppyObject),
		m_pcurVisual(NULL),
		m_bEnable(TRUE),
		m_isPressed(FALSE),
		m_isFlash(FALSE),
		m_isFlashing(FALSE),
		m_pFlashVisual(NULL),
		m_bAlwaysTooltip(false)
	{
		CWindow::AddFlag(CWindow::FLAG_NOT_CAPTURE);
	}
	CButton::~CButton() {}

	BOOL CButton::SetUpVisual(const char* c_szFileName)
	{
		CResource* pResource = CResourceManager::Instance().GetResourcePointer(c_szFileName);
		if (!pResource->IsType(CGraphicImage::Type()))
			return FALSE;

		m_upVisual.SetImagePointer(static_cast<CGraphicImage*>(pResource));
		if (m_upVisual.IsEmpty())
			return FALSE;

		SetSize(m_upVisual.GetWidth(), m_upVisual.GetHeight());
		//
		SetCurrentVisual(&m_upVisual);
		//

		return TRUE;
	}

	BOOL CButton::SetOverVisual(const char* c_szFileName)
	{
		CResource* pResource = CResourceManager::Instance().GetResourcePointer(c_szFileName);
		if (!pResource->IsType(CGraphicImage::Type()))
			return FALSE;

		m_overVisual.SetImagePointer(static_cast<CGraphicImage*>(pResource));
		if (m_overVisual.IsEmpty())
			return FALSE;

		SetSize(m_overVisual.GetWidth(), m_overVisual.GetHeight());

		return TRUE;
	}

	BOOL CButton::SetDownVisual(const char* c_szFileName)
	{
		CResource* pResource = CResourceManager::Instance().GetResourcePointer(c_szFileName);
		if (!pResource->IsType(CGraphicImage::Type()))
			return FALSE;

		m_downVisual.SetImagePointer(static_cast<CGraphicImage*>(pResource));
		if (m_downVisual.IsEmpty())
			return FALSE;

		SetSize(m_downVisual.GetWidth(), m_downVisual.GetHeight());

		return TRUE;
	}

	BOOL CButton::SetDisableVisual(const char* c_szFileName)
	{
		CResource* pResource = CResourceManager::Instance().GetResourcePointer(c_szFileName);
		if (!pResource->IsType(CGraphicImage::Type()))
			return FALSE;

		m_disableVisual.SetImagePointer(static_cast<CGraphicImage*>(pResource));
		if (m_downVisual.IsEmpty())
			return FALSE;

		SetSize(m_disableVisual.GetWidth(), m_disableVisual.GetHeight());

		return TRUE;
	}

	const char* CButton::GetUpVisualFileName()
	{
		return m_upVisual.GetGraphicImagePointer()->GetFileName();
	}
	const char* CButton::GetOverVisualFileName()
	{
		return m_overVisual.GetGraphicImagePointer()->GetFileName();
	}
	const char* CButton::GetDownVisualFileName()
	{
		return m_downVisual.GetGraphicImagePointer()->GetFileName();
	}

	int CButton::GetButtonImageWidth() const
	{
		if (m_pcurVisual)
			return m_pcurVisual->GetWidth();

		return 0;
	}

	int CButton::GetButtonImageHeight() const
	{
		if (m_pcurVisual)
			return m_pcurVisual->GetHeight();

		return 0;
	}

	void CButton::SetAlwaysToolTip(bool bVal)
	{
		m_bAlwaysTooltip = bVal;
	}

	void CButton::Flash()
	{
		m_isFlash = TRUE;
	}

	void CButton::EnableFlash()
	{
		m_isFlash = TRUE;
		SetFlashVisual(&m_overVisual);
	}

	void CButton::DisableFlash()
	{
		m_isFlash = FALSE;
		SetFlashVisual(NULL);
	}

	void CButton::SetFlashVisual(CGraphicImageInstance* pVisual)
	{
		m_pFlashVisual = pVisual;
		if (!m_pFlashVisual)
			return;

		m_pFlashVisual->SetPosition(m_rect.left, m_rect.top);
	}

	BOOL CButton::IsEnable()
	{
		return m_bEnable;
	}
	void CButton::Enable()
	{
		SetUp();
		m_bEnable = TRUE;
	}

	void CButton::Disable()
	{
		m_bEnable = FALSE;
		if (!m_disableVisual.IsEmpty())
			SetCurrentVisual(&m_disableVisual);
	}

	BOOL CButton::IsDisable()
	{
		return m_bEnable;
	}

	BOOL CButton::IsPressed()
	{
		return m_isPressed;
	}

	void CButton::SetUp()
	{
		SetCurrentVisual(&m_upVisual);
		m_isPressed = FALSE;
	}

	void CButton::Up()
	{
		if (IsIn())
			SetCurrentVisual(&m_overVisual);
		else
			SetCurrentVisual(&m_upVisual);

		PyCallClassMemberFunc(m_poHandler, "CallEvent", BuildEmptyTuple());
	}

	void CButton::Over()
	{
		SetCurrentVisual(&m_overVisual);
	}

	void CButton::Down()
	{
		m_isPressed = TRUE;
		SetCurrentVisual(&m_downVisual);
		PyCallClassMemberFunc(m_poHandler, "DownEvent", BuildEmptyTuple());
	}

#if defined(ENABLE_CLIP_MASK)
	void CButton::OnUpdate()
	{
		if (!m_bEnableMask)
			return;

		if (!m_pMaskWindow)
			return;

		m_rMaskRect = m_pMaskWindow->GetRect();
	}
	void CButton::OnRender()
	{
		if (!IsShow())
			return;

		if (m_pcurVisual)
		{
			if (m_isFlash)
			{
				if (!IsIn())
				{
					if (int(timeGetTime() / 500) % 2)
					{
						if (m_pFlashVisual)
							m_pFlashVisual->Render();
						return;
					}
				}
			}
			m_pcurVisual->Render(m_bEnableMask ? &m_rMaskRect : nullptr);
		}

		PyCallClassMemberFunc(m_poHandler, "OnRender", BuildEmptyTuple());
	}
#else
	void CButton::OnUpdate()
	{
	}
	void CButton::OnRender()
	{
		if (!IsShow())
			return;

		if (m_pcurVisual)
		{
			if ((m_isFlash || m_isFlashing) && !IsIn() && !IsPressed())
			{
				if (int(timeGetTime() / 500) % 2)
				{
					if (m_isFlashing && !m_overVisual.IsEmpty())
						SetCurrentVisual(&m_overVisual);
					else
					{
						if (m_pFlashVisual)
							m_pFlashVisual->Render();
						return;
					}
				}
				else
				{
					if (m_isFlashing && !m_upVisual.IsEmpty())
						SetCurrentVisual(&m_upVisual);
				}
			}

			m_pcurVisual->Render();
		}

		PyCallClassMemberFunc(m_poHandler, "OnRender", BuildEmptyTuple());
	}
#endif

	void CButton::OnChangePosition()
	{
		if (m_pcurVisual)
			m_pcurVisual->SetPosition(m_rect.left, m_rect.top);

		if (m_pFlashVisual)
			m_pFlashVisual->SetPosition(m_rect.left, m_rect.top);
	}

	BOOL CButton::OnMouseLeftButtonDown()
	{
		if (!IsEnable())
			return TRUE;

		m_isPressed = TRUE;
		Down();

		return TRUE;
	}

	BOOL CButton::OnMouseLeftButtonDoubleClick()
	{
		if (!IsEnable())
			return TRUE;

		OnMouseLeftButtonDown();

		return TRUE;
	}

	BOOL CButton::OnMouseLeftButtonUp()
	{
		if (!IsEnable())
			return TRUE;
		if (!IsPressed())
			return TRUE;

		m_isPressed = FALSE;
		Up();

		return TRUE;
	}

	void CButton::OnMouseOverIn()
	{
		const BOOL bEnable = IsEnable();
		if (m_bAlwaysTooltip)
		{
			if (bEnable)
				Over();

			PyCallClassMemberFunc(m_poHandler, "ShowToolTip", BuildEmptyTuple());
#if defined(ENABLE_DETAILS_UI)
			PyCallClassMemberFunc(m_poHandler, "OnMouseOverIn", BuildEmptyTuple());
#endif
		}
		else if (bEnable)
		{
			Over();
			PyCallClassMemberFunc(m_poHandler, "ShowToolTip", BuildEmptyTuple());
#if defined(ENABLE_DETAILS_UI)
			PyCallClassMemberFunc(m_poHandler, "OnMouseOverIn", BuildEmptyTuple());
#endif
		}
	}

	void CButton::OnMouseOverOut()
	{
		const BOOL bEnable = IsEnable();
		if (m_bAlwaysTooltip)
		{
			if (bEnable)
				SetUp();

			PyCallClassMemberFunc(m_poHandler, "HideToolTip", BuildEmptyTuple());
#if defined(ENABLE_DETAILS_UI)
			PyCallClassMemberFunc(m_poHandler, "OnMouseOverOut", BuildEmptyTuple());
#endif
		}
		else if (bEnable)
		{
			SetUp();
			PyCallClassMemberFunc(m_poHandler, "HideToolTip", BuildEmptyTuple());
#if defined(ENABLE_DETAILS_UI)
			PyCallClassMemberFunc(m_poHandler, "OnMouseOverOut", BuildEmptyTuple());
#endif
		}
	}

	void CButton::SetCurrentVisual(CGraphicImageInstance* pVisual)
	{
		m_pcurVisual = pVisual;
		m_pcurVisual->SetPosition(m_rect.left, m_rect.top);
	}

	void CButton::SetScale(float fx, float fy)
	{
		if (m_pcurVisual)
			m_pcurVisual->SetScale(fx, fy);

		if (!m_upVisual.IsEmpty())
			m_upVisual.SetScale(fx, fy);

		if (!m_overVisual.IsEmpty())
			m_overVisual.SetScale(fx, fy);

		if (!m_downVisual.IsEmpty())
			m_downVisual.SetScale(fx, fy);

		if (!m_disableVisual.IsEmpty())
			m_disableVisual.SetScale(fx, fy);
	}

	void CButton::SetDiffuseColor(float fr, float fg, float fb, float fa)
	{
		m_upVisual.SetDiffuseColor(fr, fg, fb, fa);
		m_overVisual.SetDiffuseColor(fr, fg, fb, fa);
		m_downVisual.SetDiffuseColor(fr, fg, fb, fa);
		m_disableVisual.SetDiffuseColor(fr, fg, fb, fa);
	}

	void CButton::LeftRightReverse()
	{
		m_upVisual.LeftRightReverse();
		m_overVisual.LeftRightReverse();
		m_downVisual.LeftRightReverse();
		m_disableVisual.LeftRightReverse();
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	CRadioButton::CRadioButton(PyObject* ppyObject) : CButton(ppyObject)
	{
	}
	CRadioButton::~CRadioButton()
	{
	}

	BOOL CRadioButton::OnMouseLeftButtonDown()
	{
		if (!IsEnable())
			return TRUE;

		if (!m_isPressed)
		{
			Down();
			PyCallClassMemberFunc(m_poHandler, "CallEvent", BuildEmptyTuple());
		}

		return TRUE;
	}
	BOOL CRadioButton::OnMouseLeftButtonUp()
	{
		return TRUE;
	}
	void CRadioButton::OnMouseOverIn()
	{
		if (!IsEnable())
			return;

		if (!m_isPressed)
		{
			SetCurrentVisual(&m_overVisual);
		}

		PyCallClassMemberFunc(m_poHandler, "ShowToolTip", BuildEmptyTuple());
	}
	void CRadioButton::OnMouseOverOut()
	{
		if (!IsEnable())
			return;

		if (!m_isPressed)
		{
			SetCurrentVisual(&m_upVisual);
		}

		PyCallClassMemberFunc(m_poHandler, "HideToolTip", BuildEmptyTuple());
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	CToggleButton::CToggleButton(PyObject* ppyObject) : CButton(ppyObject)
	{
	}
	CToggleButton::~CToggleButton()
	{
	}

	BOOL CToggleButton::OnMouseLeftButtonDown()
	{
		if (!IsEnable())
			return TRUE;

		if (m_isPressed)
		{
			SetUp();
			if (IsIn())
				SetCurrentVisual(&m_overVisual);
			else
				SetCurrentVisual(&m_upVisual);
			PyCallClassMemberFunc(m_poHandler, "OnToggleUp", BuildEmptyTuple());
		}
		else
		{
			Down();
			PyCallClassMemberFunc(m_poHandler, "OnToggleDown", BuildEmptyTuple());
		}

		return TRUE;
	}
	BOOL CToggleButton::OnMouseLeftButtonUp()
	{
		return TRUE;
	}

	void CToggleButton::OnMouseOverIn()
	{
		if (!IsEnable())
			return;

		if (!m_isPressed)
		{
			SetCurrentVisual(&m_overVisual);
		}

		PyCallClassMemberFunc(m_poHandler, "ShowToolTip", BuildEmptyTuple());
	}
	void CToggleButton::OnMouseOverOut()
	{
		if (!IsEnable())
			return;

		if (!m_isPressed)
		{
			SetCurrentVisual(&m_upVisual);
		}

		PyCallClassMemberFunc(m_poHandler, "HideToolTip", BuildEmptyTuple());
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	CDragButton::CDragButton(PyObject* ppyObject) : CButton(ppyObject)
	{
		CWindow::RemoveFlag(CWindow::FLAG_NOT_CAPTURE);
		m_restrictArea.left = 0;
		m_restrictArea.top = 0;
		m_restrictArea.right = CWindowManager::Instance().GetScreenWidth();
		m_restrictArea.bottom = CWindowManager::Instance().GetScreenHeight();
	}
	CDragButton::~CDragButton()
	{
	}

	void CDragButton::SetRestrictMovementArea(int ix, int iy, int iwidth, int iheight)
	{
		m_restrictArea.left = ix;
		m_restrictArea.top = iy;
		m_restrictArea.right = ix + iwidth;
		m_restrictArea.bottom = iy + iheight;
	}

	void CDragButton::OnChangePosition()
	{
		m_x = max(m_x, m_restrictArea.left);
		m_y = max(m_y, m_restrictArea.top);
		m_x = min(m_x, max(0, m_restrictArea.right - m_lWidth));
		m_y = min(m_y, max(0, m_restrictArea.bottom - m_lHeight));

		m_rect.left = m_x;
		m_rect.top = m_y;

		if (m_pParent)
		{
			const RECT& c_rRect = m_pParent->GetRect();
			m_rect.left += c_rRect.left;
			m_rect.top += c_rRect.top;
		}

		m_rect.right = m_rect.left + m_lWidth;
		m_rect.bottom = m_rect.top + m_lHeight;

		std::for_each(m_pChildList.begin(), m_pChildList.end(), std::mem_fn(&CWindow::UpdateRect));

		if (m_pcurVisual)
			m_pcurVisual->SetPosition(m_rect.left, m_rect.top);

		if (IsPressed())
			PyCallClassMemberFunc(m_poHandler, "OnMove", BuildEmptyTuple());
	}

	void CDragButton::OnMouseOverIn()
	{
		if (!IsEnable())

			return;

		CButton::OnMouseOverIn();
		PyCallClassMemberFunc(m_poHandler, "OnMouseOverIn", BuildEmptyTuple());
	}

	void CDragButton::OnMouseOverOut()
	{
		if (!IsEnable())
			return;

		CButton::OnMouseOverIn();
		PyCallClassMemberFunc(m_poHandler, "OnMouseOverOut", BuildEmptyTuple());
	}

#if defined(RENDER_TARGET)
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	CRenderTarget::CRenderTarget(PyObject* ppyObject) :
		CWindow(ppyObject),
		m_pRenderTargetTexture(nullptr),
		m_iIndex(-1)
	{
	}
	CRenderTarget::~CRenderTarget() {};

	void CRenderTarget::OnUpdate()
	{
		if (!m_pRenderTargetTexture)
			return;

		if (!IsShow())
			return;

		m_pRenderTargetTexture->SetRenderingRect(&m_rect);

#if defined(ENABLE_CLIP_MASK)
		if (m_bEnableMask && m_pMaskWindow)
			m_rMaskRect = m_pMaskWindow->GetRect();
#endif
	}

	void CRenderTarget::OnRender()
	{
		if (!m_pRenderTargetTexture)
			return;

		if (!IsShow())
			return;

#if defined(ENABLE_CLIP_MASK)
		m_pRenderTargetTexture->Render(m_bEnableMask ? &m_rMaskRect : NULL);
#else
		m_pRenderTargetTexture->Render();
#endif
	}

	void CRenderTarget::SetRenderTarget(int index)
	{
		m_iIndex = index;
		m_pRenderTargetTexture = CRenderTargetManager::Instance().GetRenderTargetTexture(index);
	}
#endif

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	CMoveTextLine::CMoveTextLine(PyObject* ppyObject) :
		CTextLine(ppyObject),
		m_v2SrcPos(0.0f, 0.0f),
		m_v2DstPos(0.0f, 0.0f),
		m_v2NextPos(0.0f, 0.0f),
		m_v2Direction(0.0f, 0.0f),
		m_v2NextDistance(0.0f, 0.0f),
		m_fDistance(0.0f),
		m_fMoveSpeed(10.0f),
		m_bIsMove(false)
	{
	}

	CMoveTextLine::~CMoveTextLine()
	{
		m_TextInstance.Destroy();
	}

	DWORD CMoveTextLine::Type()
	{
		static DWORD s_dwType = GetCRC32("CMoveTextLine", strlen("CMoveTextLine"));
		return (s_dwType);
	}

	BOOL CMoveTextLine::OnIsType(DWORD dwType)
	{
		if (CMoveTextLine::Type() == dwType)
			return TRUE;

		return FALSE;
	}

	void CMoveTextLine::SetMoveSpeed(float fSpeed)
	{
		m_fMoveSpeed = fSpeed;
	}

	bool CMoveTextLine::GetMove()
	{
		return m_bIsMove;
	}

	void CMoveTextLine::MoveStart()
	{
		m_bIsMove = true;
		m_v2NextPos = m_v2SrcPos;
	}

	void CMoveTextLine::MoveStop()
	{
		m_bIsMove = false;
	}

	void CMoveTextLine::OnEndMove()
	{
		PyCallClassMemberFunc(m_poHandler, "OnEndMove", BuildEmptyTuple());
	}

	void CMoveTextLine::OnChangePosition()
	{
		m_TextInstance.SetPosition((GetDefaultCodePage() == CP_1256) ? m_rect.right : m_rect.left, m_rect.top);
	}

	void CMoveTextLine::SetMovePosition(float fDstX, float fDstY)
	{
		if (fDstX != m_v2DstPos.x || fDstY != m_v2DstPos.y || m_rect.left != m_v2SrcPos.x || m_rect.top != m_v2SrcPos.y)
		{
			m_v2SrcPos.x = m_rect.left;
			m_v2SrcPos.y = m_rect.top;

			m_v2DstPos.x = fDstX;
			m_v2DstPos.y = fDstY;

			D3DXVec2Subtract(&m_v2Direction, &m_v2DstPos, &m_v2SrcPos);

			m_fDistance = (m_v2Direction.y * m_v2Direction.y + m_v2Direction.x * m_v2Direction.x);
			D3DXVec2Normalize(&m_v2Direction, &m_v2Direction);

			if (m_v2SrcPos != m_v2NextPos)
			{
				float fDist = sqrtf(m_v2NextDistance.x * m_v2NextDistance.x + m_v2NextDistance.y * m_v2NextDistance.y);
				m_v2NextPos = m_v2Direction * fDist;
				m_TextInstance.SetPosition(m_v2NextPos.x, m_v2NextPos.y);
			}
		}
	}

	void CMoveTextLine::OnUpdate()
	{
		if (IsShow() && GetMove())
		{
			D3DXVec2Add(&m_v2NextPos, &m_v2NextPos, &(m_v2Direction * m_fMoveSpeed));
			D3DXVec2Subtract(&m_v2NextDistance, &m_v2NextPos, &m_v2SrcPos);

			float fNextDistance = m_v2NextDistance.y * m_v2NextDistance.y + m_v2NextDistance.x * m_v2NextDistance.x;
			if (fNextDistance >= m_fDistance)
			{
				m_v2NextPos = m_v2DstPos;
				MoveStop();
				OnEndMove();
			}

			m_TextInstance.SetPosition(m_v2NextPos.x, m_v2NextPos.y);
			m_TextInstance.Update();
		}
	}

	void CMoveTextLine::OnRender()
	{
		if (IsShow())
			m_TextInstance.Render();
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	CMoveImageBox::CMoveImageBox(PyObject* ppyObject) :
		CImageBox(ppyObject),
		m_v2SrcPos(0.0f, 0.0f),
		m_v2DstPos(0.0f, 0.0f),
		m_v2NextPos(0.0f, 0.0f),
		m_v2Direction(0.0f, 0.0f),
		m_v2NextDistance(0.0f, 0.0f),
		m_fDistance(0.0f),
		m_fMoveSpeed(10.0f),
		m_bIsMove(false)
	{
	}

	CMoveImageBox::~CMoveImageBox()
	{
		OnDestroyInstance();
	}

	void CMoveImageBox::OnCreateInstance()
	{
		OnDestroyInstance();

		m_pImageInstance = CGraphicImageInstance::New();
	}

	void CMoveImageBox::OnDestroyInstance()
	{
		if (m_pImageInstance)
		{
			CGraphicImageInstance::Delete(m_pImageInstance);
			m_pImageInstance = NULL;
		}
	}

	DWORD CMoveImageBox::Type()
	{
		static DWORD s_dwType = GetCRC32("CMoveImageBox", strlen("CMoveImageBox"));
		return (s_dwType);
	}

	BOOL CMoveImageBox::OnIsType(DWORD dwType)
	{
		if (CMoveImageBox::Type() == dwType)
			return TRUE;

		return FALSE;
	}

	void CMoveImageBox::OnEndMove()
	{
		PyCallClassMemberFunc(m_poHandler, "OnEndMove", BuildEmptyTuple());
	}

	void CMoveImageBox::SetMovePosition(float fDstX, float fDstY)
	{
		if (fDstX != m_v2DstPos.x || fDstY != m_v2DstPos.y || m_rect.left != m_v2SrcPos.x || m_rect.top != m_v2SrcPos.y)
		{
			m_v2SrcPos.x = m_rect.left;
			m_v2SrcPos.y = m_rect.top;

			m_v2DstPos.x = fDstX;
			m_v2DstPos.y = fDstY;

			D3DXVec2Subtract(&m_v2Direction, &m_v2DstPos, &m_v2SrcPos);

			m_fDistance = (m_v2Direction.x * m_v2Direction.x + m_v2Direction.y * m_v2Direction.y);

			D3DXVec2Normalize(&m_v2Direction, &m_v2Direction);

			if (m_pImageInstance && m_v2SrcPos != m_v2NextPos)
			{
				float fDist = sqrtf(m_v2NextDistance.x * m_v2NextDistance.x + m_v2NextDistance.y * m_v2NextDistance.y);

				m_v2NextPos = m_v2Direction * fDist;
				m_pImageInstance->SetPosition(m_v2NextPos.x, m_v2NextPos.y);
			}
		}
	}

	void CMoveImageBox::SetMoveSpeed(float fSpeed)
	{
		m_fMoveSpeed = fSpeed;
	}

	void CMoveImageBox::MoveStart()
	{
		m_bIsMove = true;
		m_v2NextPos = m_v2SrcPos;
	}

	void CMoveImageBox::MoveStop()
	{
		m_bIsMove = false;
	}

	bool CMoveImageBox::GetMove()
	{
		return m_bIsMove;
	}

	void CMoveImageBox::OnUpdate()
	{
		if (!m_pImageInstance)
			return;

		if (IsShow() && GetMove())
		{
			D3DXVec2Add(&m_v2NextPos, &m_v2NextPos, &(m_v2Direction * m_fMoveSpeed));
			D3DXVec2Subtract(&m_v2NextDistance, &m_v2NextPos, &m_v2SrcPos);

			float fNextDistance = (m_v2NextDistance.x * m_v2NextDistance.x + m_v2NextDistance.y * m_v2NextDistance.y);
			if (fNextDistance >= m_fDistance)
			{
				m_v2NextPos = m_v2DstPos;
				MoveStop();
				OnEndMove();
			}

			m_pImageInstance->SetPosition(m_v2NextPos.x, m_v2NextPos.y);
		}
	}

	void CMoveImageBox::OnRender()
	{
		if (!m_pImageInstance)
			return;

		if (IsShow())
			m_pImageInstance->Render();
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	CMoveScaleImageBox::CMoveScaleImageBox(PyObject* ppyObject) :
		CMoveImageBox(ppyObject),
		m_fMaxScale(1.0f),
		m_fMaxScaleRate(1.0f),
		m_fScaleDistance(0.0f),
		m_fAdditionalScale(0.0f),
		m_v2CurScale(1.0f, 1.0f)
	{
	}

	CMoveScaleImageBox::~CMoveScaleImageBox()
	{
		OnDestroyInstance();
	}

	void CMoveScaleImageBox::OnCreateInstance()
	{
		OnDestroyInstance();

		m_pImageInstance = CGraphicImageInstance::New();
	}

	void CMoveScaleImageBox::OnDestroyInstance()
	{
		if (m_pImageInstance)
		{
			CGraphicImageInstance::Delete(m_pImageInstance);
			m_pImageInstance = NULL;
		}
	}

	DWORD CMoveScaleImageBox::Type()
	{
		static DWORD s_dwType = GetCRC32("CMoveScaleImageBox", strlen("CMoveScaleImageBox"));
		return (s_dwType);
	}

	BOOL CMoveScaleImageBox::OnIsType(DWORD dwType)
	{
		if (CMoveScaleImageBox::Type() == dwType)
			return TRUE;

		return FALSE;
	}

	void CMoveScaleImageBox::SetMaxScale(float fMaxScale)
	{
		m_fMaxScale = fMaxScale;
	}

	void CMoveScaleImageBox::SetMaxScaleRate(float fMaxScaleRate)
	{
		m_fMaxScaleRate = fMaxScaleRate;
		float fDistanceRate = m_fDistance * fMaxScaleRate;
		m_fScaleDistance = fDistanceRate;
		m_v2CurScale = m_pImageInstance->GetScale();
		float fDiffScale = m_fMaxScale - m_v2CurScale.x;
		m_fAdditionalScale = fDiffScale / (sqrtf(fDistanceRate) / m_fMoveSpeed);
	}

	void CMoveScaleImageBox::SetScalePivotCenter(bool bScalePivotCenter)
	{
		if (m_pImageInstance)
			m_pImageInstance->SetScalePivotCenter(bScalePivotCenter);
	}

	void CMoveScaleImageBox::OnUpdate()
	{
		if (!m_pImageInstance)
			return;

		if (IsShow() && GetMove())
		{
			D3DXVec2Add(&m_v2NextPos, &m_v2NextPos, &(m_v2Direction * m_fMoveSpeed));
			D3DXVec2Subtract(&m_v2NextDistance, &m_v2NextPos, &m_v2SrcPos);

			float fNextDistance = (m_v2NextDistance.x * m_v2NextDistance.x + m_v2NextDistance.y * m_v2NextDistance.y);
			if (m_fScaleDistance < fNextDistance)
				m_fAdditionalScale *= -1.0f;

			D3DXVECTOR2 v2NewScale;
			D3DXVec2Add(&v2NewScale, &m_pImageInstance->GetScale(), &D3DXVECTOR2(m_fAdditionalScale, m_fAdditionalScale));
			if (m_fMaxScale < v2NewScale.x)
				v2NewScale = D3DXVECTOR2(m_fMaxScale, m_fMaxScale);

			if (m_v2CurScale.x > v2NewScale.x)
				v2NewScale = m_v2CurScale;

			m_pImageInstance->SetScale(v2NewScale);

			if (fNextDistance >= m_fDistance)
			{
				m_pImageInstance->SetScale(m_v2CurScale);
				m_v2NextPos = m_v2DstPos;
				MoveStop();
				OnEndMove();
			}

			m_pImageInstance->SetPosition(m_v2NextPos.x, m_v2NextPos.y);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	CCircle::CCircle(PyObject* ppyObject) :
		CWindow(ppyObject)
	{
		m_dwColor = 0xFF000000;
	}

	CCircle::~CCircle()
	{
	}

	void CCircle::SetColor(DWORD dwColor)
	{
		m_dwColor = dwColor;
	}

	void CCircle::OnRender()
	{
		if (!IsShow())
			return;

		CPythonGraphic& rkpyGraphic = CPythonGraphic::Instance();
		rkpyGraphic.SetDiffuseColor(m_dwColor);
		const float c_fRadius = m_lWidth * m_v2Scale.x * 0.5f;
		rkpyGraphic.RenderCircle2d(c_fRadius + m_rect.left,
			c_fRadius + m_rect.top,
			0.0f,
			c_fRadius);
	}
};
