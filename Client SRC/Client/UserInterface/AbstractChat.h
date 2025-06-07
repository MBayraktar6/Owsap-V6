#pragma once

#include "AbstractSingleton.h"

class IAbstractChat : public TAbstractSingleton<IAbstractChat>
{
public:
	IAbstractChat() {}
	virtual ~IAbstractChat() {}

#if defined(ENABLE_LOCALE_CLIENT)
	enum ESpecialColorType
	{
		CHAT_SPECIAL_COLOR_NORMAL,
		CHAT_SPECIAL_COLOR_DICE_0,
		CHAT_SPECIAL_COLOR_DICE_1,
	};

	virtual void AppendChat(int iType, const char* c_szChat, BYTE bSpecialColorType = ESpecialColorType::CHAT_SPECIAL_COLOR_NORMAL
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
		, const char* c_szCountry = NULL
#endif
	) = 0;
#else
	virtual void AppendChat(int iType, const char* c_szChat
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
		, const char* c_szCountry = NULL
#endif
	) = 0;
#endif
};
