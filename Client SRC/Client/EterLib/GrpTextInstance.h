#ifndef __INC_ETER_LIB_GRP_TEXT_INSTANCE_H__
#define __INC_ETER_LIB_GRP_TEXT_INSTANCE_H__

#include "Pool.h"
#include "GrpText.h"

#include "../UserInterface/Locale_inc.h"
#if defined(ENABLE_TEXT_IMAGE_LINE)
#include "GrpImageInstance.h"
#endif

class CGraphicTextInstance
{
public:
	typedef CDynamicPool<CGraphicTextInstance> TPool;

public:
	enum EHorizontalAlign
	{
		HORIZONTAL_ALIGN_LEFT = 0x01,
		HORIZONTAL_ALIGN_CENTER = 0x02,
		HORIZONTAL_ALIGN_RIGHT = 0x03,
	};

	enum EVerticalAlign
	{
		VERTICAL_ALIGN_TOP = 0x10,
		VERTICAL_ALIGN_CENTER = 0x20,
		VERTICAL_ALIGN_BOTTOM = 0x30
	};

public:
	static void Hyperlink_UpdateMousePos(int x, int y);
	static int  Hyperlink_GetText(char* buf, int len);

public:
	CGraphicTextInstance();
	virtual ~CGraphicTextInstance();

	void Destroy();

	void Update();
	void Render(RECT* pClipRect = NULL);

	void ShowCursor();
	void HideCursor();

	void ShowOutLine();
	void HideOutLine();

	void SetColor(float r, float g, float b, float a = 1.0f);
	void SetColor(DWORD color);

	void SetOutlineColor(float r, float g, float b, float a = 1.0f);
	void SetOutlineColor(DWORD color);

	void SetHorizonalAlign(int hAlign);
	void SetVerticalAlign(int vAlign);
	void SetMax(int iMax);
	void SetTextPointer(CGraphicText* pText);
	void SetValueString(const std::string& c_stValue);
	void SetValue(const char* c_szValue, size_t len = -1);
	void SetPosition(float fx, float fy, float fz = 0.0f);
	void SetSecret(bool Value);
	void SetOutline(bool Value);
	void SetFeather(bool Value);
	void SetMultiLine(bool Value);
	void SetLimitWidth(float fWidth);

	void GetTextSize(int* pRetWidth, int* pRetHeight);
	const std::string& GetValueStringReference();
	WORD GetTextLineCount();
	WORD GetLineHeight() { return m_textLineHeight; }
	void SetLineHeight(const int c_iLineHeight) { m_textLineHeight = c_iLineHeight; }

#if defined(WJ_MULTI_TEXTLINE)
	void CheckMultiLine();
	void DisableEnterToken() { m_bDisableEnterToken = true; }
#endif

	int PixelPositionToCharacterPosition(int iPixelPosition);
	int GetHorizontalAlign();

protected:
	void __Initialize();
	int  __DrawCharacter(CGraphicFontTexture* pFontTexture, WORD codePage, wchar_t text, DWORD dwColor);
	void __GetTextPos(DWORD index, float* x, float* y);
	int __GetTextTag(const wchar_t* src, int maxLen, int& tagLen, std::wstring& extraInfo);

protected:
	struct SHyperlink
	{
		short sx;
		short ex;
		std::wstring text;

		SHyperlink() : sx(0), ex(0) { }
	};

#if defined(ENABLE_TEXT_IMAGE_LINE)
	struct SImage
	{
		short x;
		CGraphicImageInstance* pInstance;

		SImage() : x(0)
		{
			pInstance = NULL;
		}
	};
#endif

protected:
	DWORD m_dwTextColor;
	DWORD m_dwOutlineColor;

	WORD m_textWidth;
	WORD m_textHeight;
	WORD m_textLineHeight;

	BYTE m_hAlign;
	BYTE m_vAlign;

	WORD m_iMax;
	float m_fLimitWidth;

	bool m_isCursor;
	bool m_isSecret;
	bool m_isMultiLine;

	bool m_isOutline;
	float m_fFontFeather;

	/////

	std::string m_stText;
	D3DXVECTOR3 m_v3Position;

private:
	bool m_isUpdate;
	bool m_isUpdateFontTexture;

	CGraphicText::TRef m_roText;
	CGraphicFontTexture::TPCharacterInfomationVector m_pCharInfoVector;
	std::vector<DWORD> m_dwColorInfoVector;
	std::vector<SHyperlink> m_hyperlinkVector;

#if defined(WJ_MULTI_TEXTLINE)
	std::vector<std::shared_ptr<CGraphicTextInstance>> m_vMultiTextLine;
	bool m_bDisableEnterToken;
#endif

#if defined(ENABLE_TEXT_IMAGE_LINE)
	std::vector<SImage> m_imageVector;
#endif

public:
	static void CreateSystem(UINT uCapacity);
	static void DestroySystem();

	static CGraphicTextInstance* New();
	static void Delete(CGraphicTextInstance* pkInst);

	static CDynamicPool<CGraphicTextInstance> ms_kPool;
};

extern const char* FindToken(const char* begin, const char* end);
extern int ReadToken(const char* token);

#endif // __INC_ETER_LIB_GRP_TEXT_INSTANCE_H__
