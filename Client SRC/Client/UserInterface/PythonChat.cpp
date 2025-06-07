#include "StdAfx.h"
#include "PythonChat.h"

#include "AbstractApplication.h"
#include "PythonCharacterManager.h"
#include "../EterBase/Timer.h"

int CPythonChat::TChatSet::ms_iChatModeSize = CHAT_TYPE_MAX_NUM;

const float c_fStartDisappearingTime = 5.0f;
const int c_iMaxLineCount = 5;

///////////////////////////////////////////////////////////////////////////////////////////////////

CDynamicPool<CPythonChat::SChatLine> CPythonChat::SChatLine::ms_kPool;

void CPythonChat::SetChatColor(UINT eType, UINT r, UINT g, UINT b)
{
	if (eType >= CHAT_TYPE_MAX_NUM)
		return;

	DWORD dwColor = (0xff000000) | (r << 16) | (g << 8) | (b);
	m_akD3DXClrChat[eType] = D3DXCOLOR(dwColor);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

CPythonChat::SChatLine* CPythonChat::SChatLine::New()
{
	return ms_kPool.Alloc();
}

void CPythonChat::SChatLine::Delete(CPythonChat::SChatLine* pkChatLine)
{
	pkChatLine->Instance.Destroy();
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
	if (pkChatLine->pCountryFlagImageInstance)
	{
		CGraphicExpandedImageInstance::Delete(pkChatLine->pCountryFlagImageInstance);
		pkChatLine->pCountryFlagImageInstance = NULL;
	}
#endif
	ms_kPool.Free(pkChatLine);
}

void CPythonChat::SChatLine::DestroySystem()
{
	ms_kPool.Destroy();
}

void CPythonChat::SChatLine::SetColor(DWORD dwID, DWORD dwColor)
{
	assert(dwID < CHAT_LINE_COLOR_ARRAY_MAX_NUM);

	if (dwID >= CHAT_LINE_COLOR_ARRAY_MAX_NUM)
		return;

	aColor[dwID] = dwColor;
}

void CPythonChat::SChatLine::SetColorAll(DWORD dwColor)
{
	for (int i = 0; i < CHAT_LINE_COLOR_ARRAY_MAX_NUM; ++i)
		aColor[i] = dwColor;
}

D3DXCOLOR& CPythonChat::SChatLine::GetColorRef(DWORD dwID)
{
	assert(dwID < CHAT_LINE_COLOR_ARRAY_MAX_NUM);

	if (dwID >= CHAT_LINE_COLOR_ARRAY_MAX_NUM)
	{
		static D3DXCOLOR color(1.0f, 0.0f, 0.0f, 1.0f);
		return color;
	}

	return aColor[dwID];
}

CPythonChat::SChatLine::SChatLine() : iType(0), fAppendedTime(0)
{
	for (int i = 0; i < CHAT_LINE_COLOR_ARRAY_MAX_NUM; ++i)
		aColor[i] = 0xff0000ff;

#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
	pCountryFlagImageInstance = NULL;
#endif
}

CPythonChat::SChatLine::~SChatLine()
{
	Instance.Destroy();

#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
	if (pCountryFlagImageInstance)
	{
		CGraphicExpandedImageInstance::Delete(pCountryFlagImageInstance);
		pCountryFlagImageInstance = NULL;
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int CPythonChat::CreateChatSet(DWORD dwID)
{
	m_ChatSetMap.emplace(std::make_pair(dwID, TChatSet()));
	return dwID;
}

void CPythonChat::UpdateViewMode(DWORD dwID)
{
	IAbstractApplication& rApp = IAbstractApplication::GetSingleton();

	float fcurTime = rApp.GetGlobalTime();
	//float felapsedTime = rApp.GetGlobalElapsedTime();

	TChatSet* pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

#if defined(ENABLE_CHAT_LOG_FIX)
	TChatLineList* pLineList = &(dwID == CHAT_SET_LOG_WINDOW ? pChatSet->m_ShowingChatLogLineList : pChatSet->m_ShowingChatLineList);
#else
	TChatLineList* pLineList = &(pChatSet->m_ShowingChatLineList);
#endif
	int iLineIndex = pLineList->size();
	int iHeight = -(int(pLineList->size() + 1) * pChatSet->m_iStep);

	TChatLineList::iterator itor;
	for (itor = pLineList->begin(); itor != pLineList->end();)
	{
		TChatLine* pChatLine = (*itor);

		D3DXCOLOR& rColor = pChatLine->GetColorRef(dwID);

		float fElapsedTime = (fcurTime - pChatLine->fAppendedTime);
		if (fElapsedTime >= c_fStartDisappearingTime || iLineIndex >= c_iMaxLineCount)
		{
			rColor.a -= rColor.a / 10.0f;

			if (rColor.a <= 0.1f)
			{
				itor = pLineList->erase(itor);
			}
			else
			{
				++itor;
			}
		}
		else
		{
			++itor;
		}

		/////

		iHeight += pChatSet->m_iStep;
		--iLineIndex;

#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
		int iWidth = pChatSet->m_ix;
#endif

#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
		if (pChatLine->pCountryFlagImageInstance && CPythonSystem::Instance().IsAlwaysShowCountry())
		{
			int iCountryFlagWidth = pChatLine->pCountryFlagImageInstance->GetWidth();
			if (GetDefaultCodePage() == CP_ARABIC)
			{
				pChatLine->pCountryFlagImageInstance->SetPosition(iWidth - iCountryFlagWidth + 6, pChatSet->m_iy + iHeight);
				iWidth -= iCountryFlagWidth;
			}
			else
			{
				pChatLine->pCountryFlagImageInstance->SetPosition(iWidth, pChatSet->m_iy + iHeight);
				iWidth += iCountryFlagWidth;
			}
		}
#endif

#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
		pChatLine->Instance.SetPosition(iWidth, pChatSet->m_iy + iHeight);
#else
		pChatLine->Instance.SetPosition(pChatSet->m_ix, pChatSet->m_iy + iHeight);
#endif
		pChatLine->Instance.SetColor(rColor);
		pChatLine->Instance.Update();
	}
}

void CPythonChat::UpdateEditMode(DWORD dwID)
{
	TChatSet* pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	const int c_iAlphaLine = max(0, GetVisibleLineCount(dwID) - GetEditableLineCount(dwID) + 2);

	int iLineIndex = 0;
	float fAlpha = 0.0f;
	float fAlphaStep = 0.0f;

	if (c_iAlphaLine > 0)
		fAlphaStep = 1.0f / float(c_iAlphaLine);

#if defined(ENABLE_CHAT_LOG_FIX)
	TChatLineList* pLineList = &(dwID == CHAT_SET_LOG_WINDOW ? pChatSet->m_ShowingChatLogLineList : pChatSet->m_ShowingChatLineList);
#else
	TChatLineList* pLineList = &(pChatSet->m_ShowingChatLineList);
#endif
	int iHeight = -(int(pLineList->size() + 1) * pChatSet->m_iStep);

	for (TChatLineList::iterator itor = pLineList->begin(); itor != pLineList->end(); ++itor)
	{
		TChatLine* pChatLine = (*itor);

		D3DXCOLOR& rColor = pChatLine->GetColorRef(dwID);

		if (iLineIndex < c_iAlphaLine)
		{
			rColor.a += (fAlpha - rColor.a) / 10.0f;
			fAlpha = fMIN(fAlpha + fAlphaStep, 1.0f);
		}
		else
		{
			rColor.a = fMIN(rColor.a + 0.05f, 1.0f);
		}

		iHeight += pChatSet->m_iStep;


#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
		int iWidth = pChatSet->m_ix;
#endif

#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
		if (pChatLine->pCountryFlagImageInstance && CPythonSystem::Instance().IsAlwaysShowCountry())
		{
			int iCountryFlagWidth = pChatLine->pCountryFlagImageInstance->GetWidth();
			if (GetDefaultCodePage() == CP_ARABIC)
			{
				pChatLine->pCountryFlagImageInstance->SetPosition(iWidth - iCountryFlagWidth + 6, pChatSet->m_iy + iHeight);
				iWidth -= iCountryFlagWidth;
			}
			else
			{
				pChatLine->pCountryFlagImageInstance->SetPosition(iWidth, pChatSet->m_iy + iHeight);
				iWidth += iCountryFlagWidth;
			}
		}
#endif

#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
		pChatLine->Instance.SetPosition(iWidth, pChatSet->m_iy + iHeight);
#else
		pChatLine->Instance.SetPosition(pChatSet->m_ix, pChatSet->m_iy + iHeight);
#endif
		pChatLine->Instance.SetColor(rColor);
		pChatLine->Instance.Update();
	}
}

void CPythonChat::UpdateLogMode(DWORD dwID)
{
	TChatSet* pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

#if defined(ENABLE_CHAT_LOG_FIX)
	TChatLineList* pLineList = &(dwID == CHAT_SET_LOG_WINDOW ? pChatSet->m_ShowingChatLogLineList : pChatSet->m_ShowingChatLineList);
#else
	TChatLineList* pLineList = &(pChatSet->m_ShowingChatLineList);
#endif
	int iHeight = 0;

	for (TChatLineList::reverse_iterator itor = pLineList->rbegin(); itor != pLineList->rend(); ++itor)
	{
		TChatLine* pChatLine = (*itor);

		iHeight -= pChatSet->m_iStep;

#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
		int iWidth = pChatSet->m_ix;
#endif

#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
		if (pChatLine->pCountryFlagImageInstance && CPythonSystem::Instance().IsAlwaysShowCountry())
		{
			int iCountryFlagWidth = pChatLine->pCountryFlagImageInstance->GetWidth();
			if (GetDefaultCodePage() == CP_ARABIC)
			{
				pChatLine->pCountryFlagImageInstance->SetPosition(iWidth - iCountryFlagWidth + 6, pChatSet->m_iy + iHeight);
				iWidth -= iCountryFlagWidth;
			}
			else
			{
				pChatLine->pCountryFlagImageInstance->SetPosition(iWidth, pChatSet->m_iy + iHeight);
				iWidth += iCountryFlagWidth;
			}
		}
#endif

#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
		pChatLine->Instance.SetPosition(iWidth, pChatSet->m_iy + iHeight);
#else
		pChatLine->Instance.SetPosition(pChatSet->m_ix, pChatSet->m_iy + iHeight);
#endif
		pChatLine->Instance.SetColor(pChatLine->GetColorRef(dwID));
		pChatLine->Instance.Update();
	}
}

void CPythonChat::Update(DWORD dwID)
{
	TChatSet* pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	switch (pChatSet->m_iBoardState)
	{
	case BOARD_STATE_VIEW:
		UpdateViewMode(dwID);
		break;
	case BOARD_STATE_EDIT:
		UpdateEditMode(dwID);
		break;
	case BOARD_STATE_LOG:
		UpdateLogMode(dwID);
		break;
	}

	DWORD dwcurTime = CTimer::Instance().GetCurrentMillisecond();
	for (TWaitChatList::iterator itor = m_WaitChatList.begin(); itor != m_WaitChatList.end();)
	{
		TWaitChat& rWaitChat = *itor;

		if (rWaitChat.dwAppendingTime < dwcurTime)
		{
			AppendChat(rWaitChat.iType, rWaitChat.strChat.c_str());

			itor = m_WaitChatList.erase(itor);
		}
		else
		{
			++itor;
		}
	}
}

void CPythonChat::Render(DWORD dwID)
{
	TChatLineList* pLineList = GetChatLineListPtr(dwID);
	if (!pLineList)
		return;

	for (TChatLineList::iterator itor = pLineList->begin(); itor != pLineList->end(); ++itor)
	{
		CGraphicTextInstance& rInstance = (*itor)->Instance;
		rInstance.Render();

#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
		CGraphicExpandedImageInstance* pkCountryFlagImageInstance = (*itor)->pCountryFlagImageInstance;
		if (pkCountryFlagImageInstance && CPythonSystem::Instance().IsAlwaysShowCountry())
			pkCountryFlagImageInstance->Render();
#endif
	}
}

void CPythonChat::SetBoardState(DWORD dwID, int iState)
{
	TChatSet* pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	pChatSet->m_iBoardState = iState;
	ArrangeShowingChat(dwID);
}

void CPythonChat::SetPosition(DWORD dwID, int ix, int iy)
{
	TChatSet* pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	pChatSet->m_ix = ix;
	pChatSet->m_iy = iy;
}

void CPythonChat::SetHeight(DWORD dwID, int iHeight)
{
	TChatSet* pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	pChatSet->m_iHeight = iHeight;
}

void CPythonChat::SetStep(DWORD dwID, int iStep)
{
	TChatSet* pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	pChatSet->m_iStep = iStep;
}

void CPythonChat::ToggleChatMode(DWORD dwID, int iMode)
{
	TChatSet* pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	pChatSet->m_iMode[iMode] = 1 - pChatSet->m_iMode[iMode];
	//Tracef("ToggleChatMode : %d\n", iMode);
	ArrangeShowingChat(dwID);
}

void CPythonChat::EnableChatMode(DWORD dwID, int iMode)
{
	TChatSet* pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	pChatSet->m_iMode[iMode] = TRUE;
	//Tracef("EnableChatMode : %d\n", iMode);
	ArrangeShowingChat(dwID);
}

void CPythonChat::DisableChatMode(DWORD dwID, int iMode)
{
	TChatSet* pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	pChatSet->m_iMode[iMode] = FALSE;
	//Tracef("DisableChatMode : %d\n", iMode);
	ArrangeShowingChat(dwID);
}

void CPythonChat::SetEndPos(DWORD dwID, float fPos)
{
	TChatSet* pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	fPos = max(0.0f, fPos);
	fPos = min(1.0f, fPos);
	if (pChatSet->m_fEndPos != fPos)
	{
		pChatSet->m_fEndPos = fPos;
		ArrangeShowingChat(dwID);
	}
}

int CPythonChat::GetVisibleLineCount(DWORD dwID)
{
	TChatLineList* pLineList = GetChatLineListPtr(dwID);
	if (!pLineList)
		return 0;

	return pLineList->size();
}

int CPythonChat::GetEditableLineCount(DWORD dwID)
{
	TChatSet* pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return 0;

	return pChatSet->m_iHeight / pChatSet->m_iStep + 1;
}

int CPythonChat::GetLineCount(DWORD dwID)
{
	TChatSet* pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return 0;

	int iCount = 0;
#if defined(ENABLE_CHAT_LOG_FIX)
	TChatLineDeque ChatLineDeque = (dwID == CHAT_SET_LOG_WINDOW ? m_ChatLogLineDeque : m_ChatLineDeque);
	for (DWORD i = 0; i < ChatLineDeque.size(); ++i)
	{
		if (!pChatSet->CheckMode(ChatLineDeque[i]->iType))
			continue;

		++iCount;
	}
#else
	for (DWORD i = 0; i < m_ChatLineDeque.size(); ++i)
	{
		if (!pChatSet->CheckMode(m_ChatLineDeque[i]->iType))
			continue;

		++iCount;
	}
#endif

	return iCount;
}

int CPythonChat::GetLineStep(DWORD dwID)
{
	TChatSet* pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return 0;

	return pChatSet->m_iStep;
}

CPythonChat::TChatLineList* CPythonChat::GetChatLineListPtr(DWORD dwID)
{
	TChatSetMap::iterator itor = m_ChatSetMap.find(dwID);
	if (m_ChatSetMap.end() == itor)
		return NULL;

	TChatSet& rChatSet = itor->second;
#if defined(ENABLE_CHAT_LOG_FIX)
	return &(dwID == CHAT_SET_LOG_WINDOW ? rChatSet.m_ShowingChatLogLineList : rChatSet.m_ShowingChatLineList);
#else
	return &(rChatSet.m_ShowingChatLineList);
#endif
}

CPythonChat::TChatSet* CPythonChat::GetChatSetPtr(DWORD dwID)
{
	TChatSetMap::iterator itor = m_ChatSetMap.find(dwID);
	if (m_ChatSetMap.end() == itor)
		return NULL;

	TChatSet& rChatSet = itor->second;
	return &rChatSet;
}

void CPythonChat::ArrangeShowingChat(DWORD dwID)
{
	TChatSet* pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	pChatSet->m_ShowingChatLineList.clear();

	TChatLineDeque TempChatLineDeque;
	for (TChatLineDeque::iterator itor = m_ChatLineDeque.begin(); itor != m_ChatLineDeque.end(); ++itor)
	{
		TChatLine* pChatLine = *itor;
		if (pChatSet->CheckMode(pChatLine->iType))
			TempChatLineDeque.emplace_back(pChatLine);
	}

	int icurLineCount = TempChatLineDeque.size();
	int iVisibleLineCount = min(icurLineCount, (pChatSet->m_iHeight + pChatSet->m_iStep) / pChatSet->m_iStep);
	int iEndLine = iVisibleLineCount + int(float(icurLineCount - iVisibleLineCount - 1) * pChatSet->m_fEndPos);

	/////

	int iHeight = 12;
	for (int i = min(icurLineCount - 1, iEndLine); i >= 0; --i)
	{
		if (!pChatSet->CheckMode(TempChatLineDeque[i]->iType))
			continue;

		if (pChatSet->m_iHeight + pChatSet->m_iStep <= iHeight)
			break;

		pChatSet->m_ShowingChatLineList.emplace_front(TempChatLineDeque[i]);
		iHeight += pChatSet->m_iStep;
	}

#if defined(ENABLE_CHAT_LOG_FIX)
	ArrangeShowingChatLog(dwID);
#endif
}

void CPythonChat::AppendChat(int iType, const char* c_szChat
#if defined(ENABLE_LOCALE_CLIENT)
	, BYTE bSpecialColorType
#endif
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
	, const char* c_szCountry
#endif
)
{
#if defined(ENABLE_OX_RENEWAL)
	if (strstr(c_szChat, "#start") || strstr(c_szChat, "#send") || strstr(c_szChat, "#end"))
		return;
#endif

	// DEFAULT_FONT
	//static CResource* s_pResource = CResourceManager::Instance().GetResourcePointer(g_strDefaultFontName.c_str());

	CGraphicText* pkDefaultFont = static_cast<CGraphicText*>(DefaultFont_GetResource());
	if (!pkDefaultFont)
	{
		TraceError("CPythonChat::AppendChat - CANNOT_FIND_DEFAULT_FONT");
		return;
	}
	// END_OF_DEFAULT_FONT

	IAbstractApplication& rApp = IAbstractApplication::GetSingleton();

	SChatLine* pChatLine = SChatLine::New();
	pChatLine->iType = iType;
	pChatLine->Instance.SetValue(c_szChat);
#if defined(WJ_MULTI_TEXTLINE)
	pChatLine->Instance.DisableEnterToken();
#endif

	// DEFAULT_FONT
	pChatLine->Instance.SetTextPointer(pkDefaultFont);
	// END_OF_DEFAULT_FONT

	pChatLine->fAppendedTime = rApp.GetGlobalTime();

#if defined(ENABLE_LOCALE_CLIENT)
	switch (bSpecialColorType)
	{
		case IAbstractChat::ESpecialColorType::CHAT_SPECIAL_COLOR_DICE_0:
			pChatLine->SetColorAll(D3DXCOLOR(1.0f, 0.97640002f, 0.6196f, 1.0f));
			break;
		case IAbstractChat::ESpecialColorType::CHAT_SPECIAL_COLOR_DICE_1:
			pChatLine->SetColorAll(D3DXCOLOR(1.0f, 0.1058f, 0.73720002f, 1.0f));
			break;
		default:
			pChatLine->SetColorAll(GetChatColor(iType));
			break;
	}
#else
	pChatLine->SetColorAll(GetChatColor(iType));
#endif

#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
	if (LocaleService_GetLocale(c_szCountry))
	{
		char szFileName[256];
		sprintf(szFileName, "d:/ymir work/ui/intro/login/server_flag_%s.sub",
			LocaleService_GetCountry(c_szCountry));

		if (CResourceManager::Instance().IsFileExist(szFileName))
		{
			CGraphicSubImage* pImage = (CGraphicSubImage*)CResourceManager::Instance().GetResourcePointer(szFileName);
			if (pImage)
			{
				if (NULL == pChatLine->pCountryFlagImageInstance)
					pChatLine->pCountryFlagImageInstance = CGraphicExpandedImageInstance::New();

				pChatLine->pCountryFlagImageInstance->SetImagePointer(pImage);
				pChatLine->pCountryFlagImageInstance->SetScale(0.8f, 0.8f);
			}
		}
	}
#endif

	m_ChatLineDeque.emplace_back(pChatLine);
	if (m_ChatLineDeque.size() > CHAT_LINE_MAX_NUM)
	{
		SChatLine* pChatLine = m_ChatLineDeque.front();
		SChatLine::Delete(pChatLine);
		m_ChatLineDeque.pop_front();
	}

	for (TChatSetMap::iterator itor = m_ChatSetMap.begin(); itor != m_ChatSetMap.end(); ++itor)
	{
		TChatSet* pChatSet = &(itor->second);
		//pChatLine->SetColor(itor->first, GetChatColor(iType));

		// Edit Mode 를 억지로 끼워 맞추기 위해 추가
		if (BOARD_STATE_EDIT == pChatSet->m_iBoardState)
		{
			ArrangeShowingChat(itor->first);
		}
		else //if (BOARD_STATE_VIEW == pChatSet->m_iBoardState)
		{
#if defined(ENABLE_CHATTING_WINDOW_RENEWAL)
			if (!pChatSet->CheckMode(pChatLine->iType))
				continue;
#endif

			pChatSet->m_ShowingChatLineList.emplace_back(pChatLine);
			if (pChatSet->m_ShowingChatLineList.size() > CHAT_LINE_MAX_NUM)
				pChatSet->m_ShowingChatLineList.pop_front();
		}
	}

#if defined(ENABLE_CHAT_LOG_FIX)
	AppendChatLog(iType, c_szChat
#if defined(ENABLE_LOCALE_CLIENT)
		, bSpecialColorType
#endif
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
		, c_szCountry
#endif
	);
#endif
}

void CPythonChat::AppendChatWithDelay(int iType, const char* c_szChat, int iDelay)
{
	TWaitChat WaitChat;
	WaitChat.iType = iType;
	WaitChat.strChat = c_szChat;
	WaitChat.dwAppendingTime = CTimer::Instance().GetCurrentMillisecond() + iDelay;
	m_WaitChatList.emplace_back(WaitChat);
}

#if defined(ENABLE_CHAT_LOG_FIX)
void CPythonChat::AppendChatLog(int iType, const char* c_szChat
#if defined(ENABLE_LOCALE_CLIENT)
	, BYTE bSpecialColorType
#endif
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
	, const char* c_szCountry
#endif
)
{
	// DEFAULT_FONT
	CGraphicText* pkDefaultFont = static_cast<CGraphicText*>(DefaultFont_GetResource());
	if (!pkDefaultFont)
	{
		TraceError("CPythonChat::AppendChatLog - CANNOT_FIND_DEFAULT_FONT");
		return;
	}
	// END_OF_DEFAULT_FONT

	IAbstractApplication& rApp = IAbstractApplication::GetSingleton();

	SChatLine* pChatLine = SChatLine::New();
	pChatLine->iType = iType;
	pChatLine->Instance.SetValue(c_szChat);
#if defined(WJ_MULTI_TEXTLINE)
	pChatLine->Instance.DisableEnterToken();
#endif

	// DEFAULT_FONT
	pChatLine->Instance.SetTextPointer(pkDefaultFont);
	// END_OF_DEFAULT_FONT

	pChatLine->fAppendedTime = rApp.GetGlobalTime();

#if defined(ENABLE_LOCALE_CLIENT)
	switch (bSpecialColorType)
	{
		case IAbstractChat::ESpecialColorType::CHAT_SPECIAL_COLOR_DICE_0:
			pChatLine->SetColorAll(D3DXCOLOR(1.0f, 0.97640002f, 0.6196f, 1.0f));
			break;
		case IAbstractChat::ESpecialColorType::CHAT_SPECIAL_COLOR_DICE_1:
			pChatLine->SetColorAll(D3DXCOLOR(1.0f, 0.1058f, 0.73720002f, 1.0f));
			break;
		default:
			pChatLine->SetColorAll(GetChatColor(iType));
			break;
	}
#else
	pChatLine->SetColorAll(GetChatColor(iType));
#endif

#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
	if (LocaleService_GetLocale(c_szCountry))
	{
		char szFileName[256];
		sprintf(szFileName, "d:/ymir work/ui/intro/login/server_flag_%s.sub",
			LocaleService_GetCountry(c_szCountry));

		if (CResourceManager::Instance().IsFileExist(szFileName))
		{
			CGraphicSubImage* pImage = (CGraphicSubImage*)CResourceManager::Instance().GetResourcePointer(szFileName);
			if (pImage)
			{
				if (NULL == pChatLine->pCountryFlagImageInstance)
					pChatLine->pCountryFlagImageInstance = CGraphicExpandedImageInstance::New();

				pChatLine->pCountryFlagImageInstance->SetImagePointer(pImage);
				pChatLine->pCountryFlagImageInstance->SetScale(0.8f, 0.8f);
			}
		}
	}
#endif

	m_ChatLogLineDeque.emplace_back(pChatLine);
	if (m_ChatLogLineDeque.size() > CHAT_LINE_MAX_NUM)
	{
		SChatLine* pChatLine = m_ChatLogLineDeque.front();
		SChatLine::Delete(pChatLine);
		m_ChatLogLineDeque.pop_front();
	}

	for (TChatSetMap::iterator itor = m_ChatSetMap.begin(); itor != m_ChatSetMap.end(); ++itor)
	{
		TChatSet* pChatSet = &(itor->second);
		pChatSet->m_ShowingChatLogLineList.emplace_back(pChatLine);
		if (pChatSet->m_ShowingChatLogLineList.size() > CHAT_LINE_MAX_NUM)
			pChatSet->m_ShowingChatLogLineList.pop_front();
	}
}

void CPythonChat::ArrangeShowingChatLog(DWORD dwID)
{
	TChatSet* pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	pChatSet->m_ShowingChatLogLineList.clear();

	TChatLineDeque TempChatLineDeque;
	for (TChatLineDeque::iterator itor = m_ChatLogLineDeque.begin(); itor != m_ChatLogLineDeque.end(); ++itor)
	{
		TChatLine* pChatLine = *itor;
		if (pChatSet->CheckMode(pChatLine->iType))
			TempChatLineDeque.emplace_back(pChatLine);
	}

	int icurLineCount = TempChatLineDeque.size();
	int iVisibleLineCount = min(icurLineCount, (pChatSet->m_iHeight + pChatSet->m_iStep) / pChatSet->m_iStep);
	int iEndLine = iVisibleLineCount + int(float(icurLineCount - iVisibleLineCount - 1) * pChatSet->m_fEndPos);

	/////

	int iHeight = 12;
	for (int i = min(icurLineCount - 1, iEndLine); i >= 0; --i)
	{
		if (!pChatSet->CheckMode(TempChatLineDeque[i]->iType))
			continue;

		if (pChatSet->m_iHeight + pChatSet->m_iStep <= iHeight)
			break;

		pChatSet->m_ShowingChatLogLineList.emplace_front(TempChatLineDeque[i]);
		iHeight += pChatSet->m_iStep;
	}
}
#endif

DWORD CPythonChat::GetChatColor(int iType)
{
	if (iType < CHAT_TYPE_MAX_NUM)
		return m_akD3DXClrChat[iType];

	return D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
}

void CPythonChat::IgnoreCharacter(const char* c_szName)
{
	TIgnoreCharacterSet::iterator itor = m_IgnoreCharacterSet.find(c_szName);
	// NOTE : 이미 차단 중이라면..
	if (m_IgnoreCharacterSet.end() != itor)
	{
		m_IgnoreCharacterSet.erase(itor);
	}
	// NOTE : 차단이 되지 않은 캐릭터라면..
	else
	{
		m_IgnoreCharacterSet.emplace(c_szName);
	}
}

BOOL CPythonChat::IsIgnoreCharacter(const char* c_szName)
{
	TIgnoreCharacterSet::iterator itor = m_IgnoreCharacterSet.find(c_szName);

	if (m_IgnoreCharacterSet.end() == itor)
		return FALSE;

	return TRUE;
}

CWhisper* CPythonChat::CreateWhisper(const char* c_szName)
{
	TWhisperMap::iterator itor = m_WhisperMap.find(c_szName);

	CWhisper* pWhisper;
	if (itor == m_WhisperMap.end())
	{
		pWhisper = CWhisper::New();
		m_WhisperMap.insert(TWhisperMap::value_type(c_szName, pWhisper));
	}
	else
	{
		pWhisper = itor->second;
	}

	return pWhisper;
}

void CPythonChat::AppendWhisper(int iType, const char* c_szName, const char* c_szChat
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
	, const char* c_szCountry
#endif
)
{
	TWhisperMap::iterator itor = m_WhisperMap.find(c_szName);

	CWhisper* pWhisper;
	if (itor == m_WhisperMap.end())
	{
		pWhisper = CreateWhisper(c_szName);
	}
	else
	{
		pWhisper = itor->second;
	}

#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
	pWhisper->AppendChat(iType, c_szChat, c_szCountry);
#else
	pWhisper->AppendChat(iType, c_szChat);
#endif
}

void CPythonChat::ClearWhisper(const char* c_szName)
{
	TWhisperMap::iterator itor = m_WhisperMap.find(c_szName);

	if (itor != m_WhisperMap.end())
	{
		CWhisper* pWhisper = itor->second;
		CWhisper::Delete(pWhisper);

		m_WhisperMap.erase(itor);
	}
}

BOOL CPythonChat::GetWhisper(const char* c_szName, CWhisper** ppWhisper)
{
	TWhisperMap::iterator itor = m_WhisperMap.find(c_szName);

	if (itor == m_WhisperMap.end())
		return FALSE;

	*ppWhisper = itor->second;

	return TRUE;
}

void CPythonChat::InitWhisper(PyObject* ppyObject)
{
	TWhisperMap::iterator itor = m_WhisperMap.begin();
	for (; itor != m_WhisperMap.end(); ++itor)
	{
		std::string strName = itor->first;
		PyCallClassMemberFunc(ppyObject, "MakeWhisperButton", Py_BuildValue("(s)", strName.c_str()));
	}
}

void CPythonChat::__DestroyWhisperMap()
{
	TWhisperMap::iterator itor = m_WhisperMap.begin();
	for (; itor != m_WhisperMap.end(); ++itor)
	{
		CWhisper::Delete(itor->second);
	}
	m_WhisperMap.clear();
}

void CPythonChat::Close()
{
	TChatSetMap::iterator itor = m_ChatSetMap.begin();
	for (; itor != m_ChatSetMap.end(); ++itor)
	{
		TChatSet& rChatSet = itor->second;
		TChatLineList* pLineList = &(rChatSet.m_ShowingChatLineList);
		for (TChatLineList::iterator itor = pLineList->begin(); itor != pLineList->end(); ++itor)
		{
			TChatLine* pChatLine = (*itor);
			pChatLine->fAppendedTime = 0.0f;
		}

#if defined(ENABLE_CHAT_LOG_FIX)
		pLineList = &(rChatSet.m_ShowingChatLogLineList);
		for (TChatLineList::iterator itor = pLineList->begin(); itor != pLineList->end(); ++itor)
		{
			TChatLine* pChatLine = (*itor);
			pChatLine->fAppendedTime = 0.0f;
		}
#endif
	}
}

void CPythonChat::Destroy()
{
	__DestroyWhisperMap();

	m_ShowingChatLineList.clear();
	m_ChatSetMap.clear();
	m_ChatLineDeque.clear();

#if defined(ENABLE_CHAT_LOG_FIX)
	m_ShowingChatLogLineList.clear();
	m_ChatLogLineDeque.clear();
#endif

	SChatLine::DestroySystem();
	CWhisper::DestroySystem();

	__Initialize();
}

void CPythonChat::__Initialize()
{
	m_akD3DXClrChat[CHAT_TYPE_TALKING] = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_akD3DXClrChat[CHAT_TYPE_INFO] = D3DXCOLOR(1.0f, 0.785f, 0.785f, 1.0f);
	m_akD3DXClrChat[CHAT_TYPE_NOTICE] = D3DXCOLOR(1.0f, 0.902f, 0.730f, 1.0f);
	m_akD3DXClrChat[CHAT_TYPE_PARTY] = D3DXCOLOR(0.542f, 1.0f, 0.949f, 1.0f);
	m_akD3DXClrChat[CHAT_TYPE_GUILD] = D3DXCOLOR(0.906f, 0.847f, 1.0f, 1.0f);
	m_akD3DXClrChat[CHAT_TYPE_COMMAND] = D3DXCOLOR(0.658f, 1.0f, 0.835f, 1.0f);
	m_akD3DXClrChat[CHAT_TYPE_SHOUT] = D3DXCOLOR(0.658f, 1.0f, 0.835f, 1.0f);
	m_akD3DXClrChat[CHAT_TYPE_WHISPER] = D3DXCOLOR(0xff4AE14A);
	m_akD3DXClrChat[CHAT_TYPE_BIG_NOTICE] = D3DXCOLOR(1.0f, 0.902f, 0.730f, 1.0f);
#if defined(ENABLE_OX_RENEWAL)
	m_akD3DXClrChat[CHAT_TYPE_BIG_CONTROL_NOTICE] = D3DXCOLOR(1.0f, 0.902f, 0.730f, 1.0f);
#endif
#if defined(ENABLE_DICE_SYSTEM)
	m_akD3DXClrChat[CHAT_TYPE_DICE_INFO] = D3DXCOLOR(0xFFcc00cc);
#endif
#if defined(ENABLE_CHATTING_WINDOW_RENEWAL)
	m_akD3DXClrChat[CHAT_TYPE_EXP_INFO] = D3DXCOLOR(1.0f, 0.785f, 0.785f, 1.0f);
	m_akD3DXClrChat[CHAT_TYPE_ITEM_INFO] = D3DXCOLOR(1.0f, 0.785f, 0.785f, 1.0f);
	m_akD3DXClrChat[CHAT_TYPE_MONEY_INFO] = D3DXCOLOR(1.0f, 0.785f, 0.785f, 1.0f);
#endif
}

CPythonChat::CPythonChat()
{
	__Initialize();
}

CPythonChat::~CPythonChat()
{
	assert(m_ChatLineDeque.empty());
	assert(m_ShowingChatLineList.empty());
	assert(m_ChatSetMap.empty());
	assert(m_WhisperMap.empty());
#if defined(ENABLE_CHAT_LOG_FIX)
	assert(m_ChatLogLineDeque.empty());
	assert(m_ShowingChatLogLineList.empty());
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
CDynamicPool<CWhisper> CWhisper::ms_kPool;

CWhisper* CWhisper::New()
{
	return ms_kPool.Alloc();
}

void CWhisper::Delete(CWhisper* pkWhisper)
{
	pkWhisper->Destroy();
	ms_kPool.Free(pkWhisper);
}

void CWhisper::DestroySystem()
{
	ms_kPool.Destroy();

	SChatLine::DestroySystem();
}

void CWhisper::SetPosition(float fPosition)
{
	m_fcurPosition = fPosition;
	__ArrangeChat();
}

void CWhisper::SetBoxSize(float fWidth, float fHeight)
{
	m_fWidth = fWidth;
	m_fHeight = fHeight;

	for (TChatLineDeque::iterator itor = m_ChatLineDeque.begin(); itor != m_ChatLineDeque.end(); ++itor)
	{
		TChatLine* pChatLine = *itor;
		pChatLine->Instance.SetLimitWidth(fWidth);
	}
}

void CWhisper::AppendChat(int iType, const char* c_szChat
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
	, const char* c_szCountry
#endif
)
{
	// DEFAULT_FONT
	//static CResource * s_pResource = CResourceManager::Instance().GetResourcePointer(g_strDefaultFontName.c_str());
#if defined(LOCALE_SERVICE_YMIR) || defined(LOCALE_SERVICE_JAPAN) || defined(LOCALE_SERVICE_HONGKONG) || defined(LOCALE_SERVICE_TAIWAN) || defined(LOCALE_SERVICE_NEWCIBN)
	CGraphicText* pkDefaultFont = static_cast<CGraphicText*>(DefaultFont_GetResource());
#else
	CGraphicText* pkDefaultFont = (iType == CPythonChat::WHISPER_TYPE_GM) ? static_cast<CGraphicText*>(DefaultBoldFont_GetResource()) : static_cast<CGraphicText*>(DefaultFont_GetResource());
#endif
	if (!pkDefaultFont)
	{
		TraceError("CWhisper::AppendChat - CANNOT_FIND_DEFAULT_FONT");
		return;
	}
	// END_OF_DEFAULT_FONT

	SChatLine* pChatLine = SChatLine::New();
	pChatLine->Instance.SetValue(c_szChat);

	// DEFAULT_FONT
	pChatLine->Instance.SetTextPointer(pkDefaultFont);
	// END_OF_DEFAULT_FONT

	pChatLine->Instance.SetLimitWidth(m_fWidth);
	pChatLine->Instance.SetMultiLine(true);
#if defined(WJ_MULTI_TEXTLINE)
	pChatLine->Instance.DisableEnterToken();
#endif

	switch (iType)
	{
		case CPythonChat::WHISPER_TYPE_SYSTEM:
			pChatLine->Instance.SetColor(D3DXCOLOR(1.0f, 0.785f, 0.785f, 1.0f));
			break;
		case CPythonChat::WHISPER_TYPE_GM:
			pChatLine->Instance.SetColor(D3DXCOLOR(1.0f, 0.632f, 0.0f, 1.0f));
			break;
		case CPythonChat::WHISPER_TYPE_CHAT:
		default:
			pChatLine->Instance.SetColor(0xffffffff);
			break;
	}

#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
	if (LocaleService_GetLocale(c_szCountry))
	{
		char szFileName[256];
		sprintf(szFileName, "d:/ymir work/ui/intro/login/server_flag_%s.sub",
			LocaleService_GetCountry(c_szCountry));

		if (CResourceManager::Instance().IsFileExist(szFileName))
		{
			CGraphicSubImage* pImage = (CGraphicSubImage*)CResourceManager::Instance().GetResourcePointer(szFileName);
			if (pImage)
			{
				if (NULL == pChatLine->pCountryFlagImageInstance)
					pChatLine->pCountryFlagImageInstance = CGraphicExpandedImageInstance::New();

				pChatLine->pCountryFlagImageInstance->SetImagePointer(pImage);
				pChatLine->pCountryFlagImageInstance->SetScale(0.8f, 0.8f);
			}
		}
	}
#endif

	m_ChatLineDeque.emplace_back(pChatLine);

	__ArrangeChat();
}

void CWhisper::__ArrangeChat()
{
	for (TChatLineDeque::iterator itor = m_ChatLineDeque.begin(); itor != m_ChatLineDeque.end(); ++itor)
	{
		TChatLine* pChatLine = *itor;
		pChatLine->Instance.Update();
	}
}

void CWhisper::Render(float fx, float fy)
{
	float fHeight = fy + m_fHeight;

	int iViewCount = int(m_fHeight / m_fLineStep) - 1;
	int iLineCount = int(m_ChatLineDeque.size());
	int iStartLine = -1;
	if (iLineCount > iViewCount)
	{
		iStartLine = int(float(iLineCount - iViewCount) * m_fcurPosition) + iViewCount - 1;
	}
	else if (!m_ChatLineDeque.empty())
	{
		iStartLine = iLineCount - 1;
	}

	RECT Rect = { fx, fy, fx + m_fWidth, fy + m_fHeight };

#if defined(ENABLE_CLIP_MASK)
	if (GetDefaultCodePage() == CP_ARABIC)
	{
		Rect.left = fx - m_fWidth;
		Rect.right = fx;
	}
#endif

	for (int i = iStartLine; i >= 0; --i)
	{
		assert(i >= 0 && i < int(m_ChatLineDeque.size()));
		TChatLine* pChatLine = m_ChatLineDeque[i];

		WORD wLineCount = pChatLine->Instance.GetTextLineCount();
		fHeight -= wLineCount * m_fLineStep;

#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
		float fWidth = 0.0f;
#endif

#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
		if (pChatLine->pCountryFlagImageInstance && CPythonSystem::Instance().IsAlwaysShowCountry())
		{
			int iCountryFlagWidth = pChatLine->pCountryFlagImageInstance->GetWidth();
			if (GetDefaultCodePage() == CP_ARABIC)
			{
				pChatLine->pCountryFlagImageInstance->SetPosition(fx + fWidth - iCountryFlagWidth, fHeight + 2.0f);
				fWidth -= iCountryFlagWidth + 3;
			}
			else
			{
				pChatLine->pCountryFlagImageInstance->SetPosition(fx + fWidth, fHeight + 2.0f);
				fWidth += iCountryFlagWidth + 3;
			}
			pChatLine->pCountryFlagImageInstance->Render(&Rect);
		}

		pChatLine->Instance.SetPosition(fx + fWidth, fHeight);
#else
		pChatLine->Instance.SetPosition(fx, fHeight);
#endif
		pChatLine->Instance.Render(&Rect);

		if (fHeight < fy)
			break;
	}
}

void CWhisper::__Initialize()
{
	m_fLineStep = 15.0f;
	m_fWidth = 300.0f;
	m_fHeight = 120.0f;
	m_fcurPosition = 1.0f;
}

void CWhisper::Destroy()
{
	std::for_each(m_ChatLineDeque.begin(), m_ChatLineDeque.end(), SChatLine::Delete);
	m_ChatLineDeque.clear();
	m_ShowingChatLineList.clear();
}

CWhisper::CWhisper()
{
	__Initialize();
}

CWhisper::~CWhisper()
{
	Destroy();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

CDynamicPool<CWhisper::SChatLine> CWhisper::SChatLine::ms_kPool;

CWhisper::SChatLine* CWhisper::SChatLine::New()
{
	return ms_kPool.Alloc();
}

void CWhisper::SChatLine::Delete(CWhisper::SChatLine* pkChatLine)
{
	pkChatLine->Instance.Destroy();
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
	if (pkChatLine->pCountryFlagImageInstance)
	{
		CGraphicExpandedImageInstance::Delete(pkChatLine->pCountryFlagImageInstance);
		pkChatLine->pCountryFlagImageInstance = NULL;
	}
#endif
	ms_kPool.Free(pkChatLine);
}

void CWhisper::SChatLine::DestroySystem()
{
	ms_kPool.Destroy();
}
