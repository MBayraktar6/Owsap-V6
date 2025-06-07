#include "StdAfx.h"
#include "FrameController.h"

void CFrameController::Update(float fElapsedTime)
{
#if defined(__FRAME_UPDATE_OPTIMIZATION__)
	if (m_fFrameTime == 0.0f || m_dwMaxFrame == 0)
		return;

	m_fLastFrameTime += fElapsedTime;

	DWORD dwFrameCount = static_cast<DWORD>(m_fLastFrameTime / m_fFrameTime);
	if (dwFrameCount == 0)
		return;

	m_fLastFrameTime -= dwFrameCount * m_fFrameTime;
	m_dwcurFrame += dwFrameCount;

	if (m_dwcurFrame >= m_dwMaxFrame)
	{
		DWORD dwLoopCount = m_dwcurFrame / m_dwMaxFrame;
		if (!m_isLoop || (m_iLoopCount > 0 && dwLoopCount >= m_iLoopCount))
		{
			m_fLastFrameTime = 0.0f;
			m_dwcurFrame = 0;
			m_iLoopCount = 1;
			m_isActive = FALSE;
			return;
		}

		m_dwcurFrame %= m_dwMaxFrame;
		m_iLoopCount -= dwLoopCount;
	}
#else
	m_fLastFrameTime -= fElapsedTime;

	for (int i = 0; i < 20; ++i)
	{
		if (m_fLastFrameTime < 0.0f)
		{
			m_fLastFrameTime += m_fFrameTime;
			++m_dwcurFrame;

			if (m_dwcurFrame >= m_dwMaxFrame)
			{
				if (m_isLoop && --m_iLoopCount != 0)
				{
					if (m_iLoopCount < 0)
						m_iLoopCount = 0;
					m_dwcurFrame = 0;
				}
				else
				{
					m_iLoopCount = 1;
					m_dwcurFrame = 0;
					m_isActive = FALSE;
					return;
				}
			}
		}
		else
		{
			break;
		}
	}
#endif
}

void CFrameController::SetCurrentFrame(DWORD dwFrame)
{
	m_dwcurFrame = dwFrame;
}

BYTE CFrameController::GetCurrentFrame()
{
	return m_dwcurFrame;
}

void CFrameController::SetMaxFrame(DWORD dwMaxFrame)
{
	m_dwMaxFrame = dwMaxFrame;
}

void CFrameController::SetFrameTime(float fTime)
{
	m_fFrameTime = fTime;
#if defined(__FRAME_UPDATE_OPTIMIZATION__)
	m_fLastFrameTime = 0.0f;
#else
	m_fLastFrameTime = fTime;
#endif
}

void CFrameController::SetStartFrame(DWORD dwStartFrame)
{
	m_dwStartFrame = dwStartFrame;
}

void CFrameController::SetLoopFlag(BOOL bFlag)
{
	m_isLoop = bFlag;
}

void CFrameController::SetLoopCount(int iLoopCount)
{
	m_iLoopCount = iLoopCount;
}

void CFrameController::SetActive(BOOL bFlag)
{
	m_isActive = bFlag;
}

BOOL CFrameController::isActive(DWORD dwMainFrame)
{
	if (dwMainFrame < m_dwStartFrame)
		return FALSE;

	return m_isActive;
}

void CFrameController::Clear()
{
	m_isActive = TRUE;
	m_dwcurFrame = 0;
	m_fLastFrameTime = 0.0f;
#if defined(__FRAME_UPDATE_OPTIMIZATION__)
	m_iLoopCount = 0;
#endif
}

CFrameController::CFrameController()
{
	m_isActive = TRUE;
	m_dwcurFrame = 0;
	m_fLastFrameTime = 0.0f;

	m_isLoop = FALSE;
#if defined(__FRAME_UPDATE_OPTIMIZATION__)
	m_iLoopCount = 0;
#endif

	m_dwMaxFrame = 0;
	m_fFrameTime = 0.0f;
	m_dwStartFrame = 0;
}

CFrameController::~CFrameController() = default;
