#pragma once

#include "Packet.h"
#include "AbstractChat.h"
#include "PythonSystem.h"

class CWhisper
{
public:
	using TChatLine = struct SChatLine
	{
		CGraphicTextInstance Instance;
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
		CGraphicExpandedImageInstance* pCountryFlagImageInstance;
#endif

		SChatLine()
		{
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
			pCountryFlagImageInstance = NULL;
#endif
		}

		~SChatLine()
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

		static void DestroySystem();

		static SChatLine* New();
		static void Delete(SChatLine* pkChatLine);

		static CDynamicPool<SChatLine> ms_kPool;
	};

	using TChatLineDeque = std::deque<TChatLine*>;
	using TChatLineList = std::list<TChatLine*>;

public:
	CWhisper();
	~CWhisper();

	void Destroy();

	void SetPosition(float fPosition);
	void SetBoxSize(float fWidth, float fHeight);
	void AppendChat(int iType, const char* c_szChat
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
		, const char* c_szCountry = NULL
#endif
	);
	void Render(float fx, float fy);

protected:
	void __Initialize();
	void __ArrangeChat();

protected:
	float m_fLineStep;
	float m_fWidth;
	float m_fHeight;
	float m_fcurPosition;

	TChatLineDeque m_ChatLineDeque;
	TChatLineList m_ShowingChatLineList;

public:
	static void DestroySystem();

	static CWhisper* New();
	static void Delete(CWhisper* pkWhisper);

	static CDynamicPool<CWhisper> ms_kPool;
};

class CPythonChat : public CSingleton<CPythonChat>, public IAbstractChat
{
public:
	enum EWhisperType
	{
		WHISPER_TYPE_CHAT = 0,
		WHISPER_TYPE_NOT_EXIST = 1,
		WHISPER_TYPE_TARGET_BLOCKED = 2,
		WHISPER_TYPE_SENDER_BLOCKED = 3,
		WHISPER_TYPE_ERROR = 4,
		WHISPER_TYPE_GM = 5,
		WHISPER_TYPE_SYSTEM = 0xFF
	};

	enum EBoardState
	{
		BOARD_STATE_VIEW,
		BOARD_STATE_EDIT,
		BOARD_STATE_LOG,
	};

#if defined(ENABLE_CHAT_LOG_FIX)
	enum EChatWindow
	{
		CHAT_SET_CHAT_WINDOW,
		CHAT_SET_LOG_WINDOW
	};
#endif

	enum
	{
		CHAT_LINE_MAX_NUM = 300,
		CHAT_LINE_COLOR_ARRAY_MAX_NUM = 3,
	};

	enum EHyperLinkType
	{
		HYPER_LINK_ITEM_KEYWORD,
		HYPER_LINK_ITEM_VNUM,
#if defined(ENABLE_SET_ITEM)
		HYPER_LINK_ITEM_PRE_SET_VALUE,
#endif
		HYPER_LINK_ITEM_FLAGS,
		HYPER_LINK_ITEM_SOCKET0,
		HYPER_LINK_ITEM_SOCKET1,
		HYPER_LINK_ITEM_SOCKET2,
#if defined(ENABLE_ITEM_SOCKET6)
		HYPER_LINK_ITEM_SOCKET3,
		HYPER_LINK_ITEM_SOCKET4,
		HYPER_LINK_ITEM_SOCKET5,
#endif
#if defined(ENABLE_CHANGE_LOOK_SYSTEM)
		HYPER_LINK_ITEM_CHANGE_ITEM_VNUM,
#endif
#if defined(ENABLE_REFINE_ELEMENT_SYSTEM)
		HYPER_LINK_ITEM_REFINE_ELEMENT_APPLY_TYPE,
		HYPER_LINK_ITEM_REFINE_ELEMENT_GRADE,
		HYPER_LINK_ITEM_REFINE_ELEMENT_VALUE0,
		HYPER_LINK_ITEM_REFINE_ELEMENT_VALUE1,
		HYPER_LINK_ITEM_REFINE_ELEMENT_VALUE2,
		HYPER_LINK_ITEM_REFINE_ELEMENT_BONUS_VALUE0,
		HYPER_LINK_ITEM_REFINE_ELEMENT_BONUS_VALUE1,
		HYPER_LINK_ITEM_REFINE_ELEMENT_BONUS_VALUE2,
#endif
#if defined(ENABLE_APPLY_RANDOM)
		HYPER_LINK_ITEM_APPLY_RANDOM_TYPE0,
		HYPER_LINK_ITEM_APPLY_RANDOM_VALUE0,
		HYPER_LINK_ITEM_APPLY_RANDOM_TYPE1,
		HYPER_LINK_ITEM_APPLY_RANDOM_VALUE1,
		HYPER_LINK_ITEM_APPLY_RANDOM_TYPE2,
		HYPER_LINK_ITEM_APPLY_RANDOM_VALUE2,
		HYPER_LINK_ITEM_APPLY_RANDOM_TYPE3,
		HYPER_LINK_ITEM_APPLY_RANDOM_VALUE3,
#endif
		HYPER_LINK_ITEM_ATTRIBUTE_TYPE0,
		HYPER_LINK_ITEM_ATTRIBUTE_VALUE0,
		HYPER_LINK_ITEM_ATTRIBUTE_TYPE1,
		HYPER_LINK_ITEM_ATTRIBUTE_VALUE1,
		HYPER_LINK_ITEM_ATTRIBUTE_TYPE2,
		HYPER_LINK_ITEM_ATTRIBUTE_VALUE2,
		HYPER_LINK_ITEM_ATTRIBUTE_TYPE3,
		HYPER_LINK_ITEM_ATTRIBUTE_VALUE3,
		HYPER_LINK_ITEM_ATTRIBUTE_TYPE4,
		HYPER_LINK_ITEM_ATTRIBUTE_VALUE4,
		HYPER_LINK_ITEM_ATTRIBUTE_TYPE5,
		HYPER_LINK_ITEM_ATTRIBUTE_VALUE5,
		HYPER_LINK_ITEM_ATTRIBUTE_TYPE6,
		HYPER_LINK_ITEM_ATTRIBUTE_VALUE6,
		HYPER_LINK_ITEM_MAX,
	};

	typedef struct SChatLine
	{
		int iType;
		float fAppendedTime;
		D3DXCOLOR aColor[CHAT_LINE_COLOR_ARRAY_MAX_NUM];
		CGraphicTextInstance Instance;
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
		CGraphicExpandedImageInstance* pCountryFlagImageInstance;
#endif

		SChatLine();
		virtual ~SChatLine();

		void SetColor(DWORD dwID, DWORD dwColor);
		void SetColorAll(DWORD dwColor);
		D3DXCOLOR& GetColorRef(DWORD dwID);
		static void DestroySystem();

		static SChatLine* New();
		static void Delete(SChatLine* pkChatLine);

		static CDynamicPool<SChatLine> ms_kPool;
	} TChatLine;

	using TWaitChat = struct SWaitChat
	{
		int iType;
		std::string strChat;
		DWORD dwAppendingTime;
	};

	using TChatLineDeque = std::deque<TChatLine*>;
	using TChatLineList = std::list<TChatLine*>;

	using TWhisperMap = std::map<std::string, CWhisper*>;
	using TIgnoreCharacterSet = std::set<std::string>;
	using TWaitChatList = std::list<TWaitChat>;

	using TChatSet = struct SChatSet
	{
		int m_ix;
		int m_iy;
		int m_iHeight;
		int m_iStep;
		float m_fEndPos;

		int m_iBoardState;
		std::vector<int> m_iMode;

		TChatLineList m_ShowingChatLineList;
#if defined(ENABLE_CHAT_LOG_FIX)
		TChatLineList m_ShowingChatLogLineList;
#endif

		bool CheckMode(DWORD dwType)
		{
			if (dwType >= m_iMode.size())
				return false;

			return m_iMode[dwType] ? true : false;
		}

		SChatSet()
		{
			m_iBoardState = BOARD_STATE_VIEW;

			m_ix = 0;
			m_iy = 0;
			m_fEndPos = 1.0f;
			m_iHeight = 0;
			m_iStep = 15;

			m_iMode.clear();
			m_iMode.resize(ms_iChatModeSize, 1);
		}

		static int ms_iChatModeSize;
	};

	using TChatSetMap = std::map<int, TChatSet>;

public:
	CPythonChat();
	virtual ~CPythonChat();

	void SetChatColor(UINT eType, UINT r, UINT g, UINT b);

	void Destroy();
	void Close();

	int CreateChatSet(DWORD dwID);
	void Update(DWORD dwID);
	void Render(DWORD dwID);
	void RenderWhisper(const char* c_szName, float fx, float fy);

	void SetBoardState(DWORD dwID, int iState);
	void SetPosition(DWORD dwID, int ix, int iy);
	void SetHeight(DWORD dwID, int iHeight);
	void SetStep(DWORD dwID, int iStep);
	void ToggleChatMode(DWORD dwID, int iMode);
	void EnableChatMode(DWORD dwID, int iMode);
	void DisableChatMode(DWORD dwID, int iMode);
	void SetEndPos(DWORD dwID, float fPos);

	int GetVisibleLineCount(DWORD dwID);
	int GetEditableLineCount(DWORD dwID);
	int GetLineCount(DWORD dwID);
	int GetLineStep(DWORD dwID);

	// Chat
	void AppendChat(int iType, const char* c_szChat
#if defined(ENABLE_LOCALE_CLIENT)
		, BYTE bSpecialColorType = IAbstractChat::ESpecialColorType::CHAT_SPECIAL_COLOR_NORMAL
#endif
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
		, const char* c_szCountry = NULL
#endif
	);
	void AppendChatWithDelay(int iType, const char* c_szChat, int iDelay);
	void ArrangeShowingChat(DWORD dwID);

#if defined(ENABLE_CHAT_LOG_FIX)
	// Chat-log
	void AppendChatLog(int iType, const char* c_szChat
#if defined(ENABLE_LOCALE_CLIENT)
		, BYTE bSpecialColorType = IAbstractChat::ESpecialColorType::CHAT_SPECIAL_COLOR_NORMAL
#endif
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
		, const char* c_szCountry = NULL
#endif
	);
	void ArrangeShowingChatLog(DWORD dwID);
#endif

	// Ignore
	void IgnoreCharacter(const char* c_szName);
	BOOL IsIgnoreCharacter(const char* c_szName);

	// Whisper
	CWhisper* CreateWhisper(const char* c_szName);
	void AppendWhisper(int iType, const char* c_szName, const char* c_szChat
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
		, const char* c_szCountry = NULL
#endif
	);
	void ClearWhisper(const char* c_szName);
	BOOL GetWhisper(const char* c_szName, CWhisper** ppWhisper);
	void InitWhisper(PyObject* ppyObject);

protected:
	void __Initialize();
	void __DestroyWhisperMap();

	TChatLineList* GetChatLineListPtr(DWORD dwID);
	TChatSet* GetChatSetPtr(DWORD dwID);

	void UpdateViewMode(DWORD dwID);
	void UpdateEditMode(DWORD dwID);
	void UpdateLogMode(DWORD dwID);

	DWORD GetChatColor(int iType);

protected:
	TChatLineDeque m_ChatLineDeque;
	TChatLineList m_ShowingChatLineList;
	TChatSetMap m_ChatSetMap;
	TWhisperMap m_WhisperMap;
	TIgnoreCharacterSet m_IgnoreCharacterSet;
	TWaitChatList m_WaitChatList;

#if defined(ENABLE_CHAT_LOG_FIX)
	TChatLineDeque m_ChatLogLineDeque;
	TChatLineList m_ShowingChatLogLineList;
#endif

	D3DXCOLOR m_akD3DXClrChat[CHAT_TYPE_MAX_NUM];
};
