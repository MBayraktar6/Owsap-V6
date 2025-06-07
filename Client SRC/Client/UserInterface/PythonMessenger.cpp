#include "StdAfx.h"
#include "PythonMessenger.h"

void CPythonMessenger::RemoveFriend(const char* c_szName)
{
	m_FriendNameMap.erase(c_szName);
}

void CPythonMessenger::OnFriendLogin(const char* c_szName
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM) && defined(ENABLE_MESSENGER_DETAILS)
	, const char* c_szCountry
#endif
)
{
	m_FriendNameMap.insert(c_szName);

	if (m_poMessengerHandler)
	{
#if defined(ENABLE_MESSENGER_DETAILS)
		PyCallClassMemberFunc(m_poMessengerHandler, "OnLogin", Py_BuildValue("(isis)", MESSENGER_GRUOP_INDEX_FRIEND, c_szName, 0
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
			, c_szCountry
#endif
		));
#else
		PyCallClassMemberFunc(m_poMessengerHandler, "OnLogin", Py_BuildValue("(is)", MESSENGER_GRUOP_INDEX_FRIEND, c_szName));
#endif
	}
}

void CPythonMessenger::OnFriendLogout(const char* c_szName
#if defined(ENABLE_MESSENGER_DETAILS)
	, const DWORD c_dwLastPlayTime
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
	, const char* c_szCountry
#endif
#endif
)
{
	m_FriendNameMap.insert(c_szName);

	if (m_poMessengerHandler)
	{
#if defined(ENABLE_MESSENGER_DETAILS)
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
		PyCallClassMemberFunc(m_poMessengerHandler, "OnLogout", Py_BuildValue("(isis)", MESSENGER_GRUOP_INDEX_FRIEND, c_szName, c_dwLastPlayTime, LocaleService_GetCountry(c_szCountry)));
#else
		PyCallClassMemberFunc(m_poMessengerHandler, "OnLogout", Py_BuildValue("(isi)", MESSENGER_GRUOP_INDEX_FRIEND, c_szName, c_dwLastPlayTime));
#endif
#else
		PyCallClassMemberFunc(m_poMessengerHandler, "OnLogout", Py_BuildValue("(is)", MESSENGER_GRUOP_INDEX_FRIEND, c_szName));
#endif
	}
}

#if defined(ENABLE_MESSENGER_GM)
void CPythonMessenger::OnGMLogin(const char* c_szName
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM) && defined(ENABLE_MESSENGER_DETAILS)
	, const char* c_szCountry
#endif
)
{
	m_GMNameMap.insert(c_szName);

	if (m_poMessengerHandler)
	{
#if defined(ENABLE_MESSENGER_DETAILS)
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
		PyCallClassMemberFunc(m_poMessengerHandler, "OnLogin", Py_BuildValue("(isis)", MESSENGER_GROUP_INDEX_GM, c_szName, 0, LocaleService_GetCountry(c_szCountry)));
#else
		PyCallClassMemberFunc(m_poMessengerHandler, "OnLogin", Py_BuildValue("(isi)", MESSENGER_GROUP_INDEX_GM, c_szName, 0));
#endif
#else
		PyCallClassMemberFunc(m_poMessengerHandler, "OnLogin", Py_BuildValue("(is)", MESSENGER_GROUP_INDEX_GM, c_szName));
#endif
	}
}

void CPythonMessenger::OnGMLogout(const char* c_szName
#if defined(ENABLE_MESSENGER_DETAILS)
	, const DWORD c_dwLastPlayTime
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
	, const char* c_szCountry
#endif
#endif
)
{
	m_GMNameMap.insert(c_szName);

	if (m_poMessengerHandler)
	{
#if defined(ENABLE_MESSENGER_DETAILS)
#if defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
		PyCallClassMemberFunc(m_poMessengerHandler, "OnLogout", Py_BuildValue("(isis)", MESSENGER_GROUP_INDEX_GM, c_szName, c_dwLastPlayTime, LocaleService_GetCountry(c_szCountry)));
#else
		PyCallClassMemberFunc(m_poMessengerHandler, "OnLogout", Py_BuildValue("(isi)", MESSENGER_GROUP_INDEX_GM, c_szName, c_dwLastPlayTime));
#endif
#else
		PyCallClassMemberFunc(m_poMessengerHandler, "OnLogout", Py_BuildValue("(is)", MESSENGER_GROUP_INDEX_GM, c_szName));
#endif
	}
}
#endif

#if defined(ENABLE_MESSENGER_BLOCK)
void CPythonMessenger::OnBlockLogin(const char* c_szName)
{
	m_BlockNameMap.insert(c_szName);

	//if (m_poMessengerHandler)
	// Note : In order to keep the blocked player online status, both player must have "account, companion" in the database table.
	//PyCallClassMemberFunc(m_poMessengerHandler, "OnLogin", Py_BuildValue("(is)", MESSENGER_GROUP_INDEX_BLOCK, c_szKey));

	// NOTE : Blocked players should not be shown as an online status.
	PyCallClassMemberFunc(m_poMessengerHandler, "OnLogout", Py_BuildValue("(is)", MESSENGER_GROUP_INDEX_BLOCK, c_szName));
}

void CPythonMessenger::OnBlockLogout(const char* c_szName)
{
	m_BlockNameMap.insert(c_szName);

	if (m_poMessengerHandler)
		PyCallClassMemberFunc(m_poMessengerHandler, "OnLogout", Py_BuildValue("(is)", MESSENGER_GROUP_INDEX_BLOCK, c_szName));
}

BOOL CPythonMessenger::IsBlockFriendByName(const char* c_szName)
{
	return (m_BlockNameMap.end() != m_BlockNameMap.find(c_szName));
}

void CPythonMessenger::RemoveBlock(const char* c_szName)
{
	m_BlockNameMap.erase(c_szName);
	PyCallClassMemberFunc(m_poMessengerHandler, "OnRemoveList", Py_BuildValue("(is)", MESSENGER_GROUP_INDEX_BLOCK, c_szName));
}
#endif

BOOL CPythonMessenger::IsFriendByName(const char* c_szName)
{
	return m_FriendNameMap.end() != m_FriendNameMap.find(c_szName);
}

void CPythonMessenger::AppendGuildMember(const char* c_szName)
{
	if (m_GuildMemberStateMap.end() != m_GuildMemberStateMap.find(c_szName))
		return;

	LogoutGuildMember(c_szName);
}

void CPythonMessenger::RemoveGuildMember(const char* c_szName)
{
	m_GuildMemberStateMap.erase(c_szName);

	if (m_poMessengerHandler)
		PyCallClassMemberFunc(m_poMessengerHandler, "OnRemoveList", Py_BuildValue("(is)", MESSENGER_GRUOP_INDEX_GUILD, c_szName));
}

void CPythonMessenger::RemoveAllGuildMember()
{
	m_GuildMemberStateMap.clear();

	if (m_poMessengerHandler)
		PyCallClassMemberFunc(m_poMessengerHandler, "OnRemoveAllList", Py_BuildValue("(i)", MESSENGER_GRUOP_INDEX_GUILD));
}

void CPythonMessenger::LoginGuildMember(const char* c_szName)
{
	m_GuildMemberStateMap[c_szName] = 1;
	if (m_poMessengerHandler)
		PyCallClassMemberFunc(m_poMessengerHandler, "OnLogin", Py_BuildValue("(is)", MESSENGER_GRUOP_INDEX_GUILD, c_szName));
}

void CPythonMessenger::LogoutGuildMember(const char* c_szName)
{
	m_GuildMemberStateMap[c_szName] = 0;
	if (m_poMessengerHandler)
		PyCallClassMemberFunc(m_poMessengerHandler, "OnLogout", Py_BuildValue("(is)", MESSENGER_GRUOP_INDEX_GUILD, c_szName));
}

void CPythonMessenger::RefreshGuildMember()
{
	for (TGuildMemberStateMap::iterator itor = m_GuildMemberStateMap.begin(); itor != m_GuildMemberStateMap.end(); ++itor)
	{
		if (itor->second)
			PyCallClassMemberFunc(m_poMessengerHandler, "OnLogin", Py_BuildValue("(is)", MESSENGER_GRUOP_INDEX_GUILD, (itor->first).c_str()));
		else
			PyCallClassMemberFunc(m_poMessengerHandler, "OnLogout", Py_BuildValue("(is)", MESSENGER_GRUOP_INDEX_GUILD, (itor->first).c_str()));
	}
}

void CPythonMessenger::Destroy()
{
	m_FriendNameMap.clear();
	m_GuildMemberStateMap.clear();
}

void CPythonMessenger::SetMessengerHandler(PyObject* poHandler)
{
	m_poMessengerHandler = poHandler;
}

CPythonMessenger::CPythonMessenger()
	: m_poMessengerHandler(NULL)
{
}

CPythonMessenger::~CPythonMessenger()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////

PyObject* messengerRemoveFriend(PyObject* poSelf, PyObject* poArgs)
{
	char* c_szName;
	if (!PyTuple_GetString(poArgs, 0, &c_szName))
		return Py_BuildException();

	CPythonMessenger::Instance().RemoveFriend(c_szName);
	return Py_BuildNone();
}

PyObject* messengerIsFriendByName(PyObject* poSelf, PyObject* poArgs)
{
	char* szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonMessenger::Instance().IsFriendByName(szName));
}

#if defined(ENABLE_MESSENGER_BLOCK)
PyObject* messengerIsBlockFriendByName(PyObject* poSelf, PyObject* poArgs)
{
	char* szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonMessenger::Instance().IsBlockFriendByName(szName));
}

PyObject* messengerRemoveBlock(PyObject* poSelf, PyObject* poArgs)
{
	char* szKey;
	if (!PyTuple_GetString(poArgs, 0, &szKey))
		return Py_BuildException();

	CPythonMessenger::Instance().RemoveBlock(szKey);
	return Py_BuildNone();
}
#endif

PyObject* messengerDestroy(PyObject* poSelf, PyObject* poArgs)
{
	CPythonMessenger::Instance().Destroy();
	return Py_BuildNone();
}

PyObject* messengerRefreshGuildMember(PyObject* poSelf, PyObject* poArgs)
{
	CPythonMessenger::Instance().RefreshGuildMember();
	return Py_BuildNone();
}

PyObject* messengerSetMessengerHandler(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* poEventHandler;
	if (!PyTuple_GetObject(poArgs, 0, &poEventHandler))
		return Py_BuildException();

	CPythonMessenger::Instance().SetMessengerHandler(poEventHandler);
	return Py_BuildNone();
}

#if defined(ENABLE_MAILBOX)
PyObject* messengerGetFriendNames(PyObject* poSelf, PyObject* poArgs)
{
	const CPythonMessenger& messenger = CPythonMessenger::Instance();
	PyObject* Tuple = PyTuple_New(messenger.m_FriendNameMap.size());

	uint16_t i = 0;
	for (const std::string& sFriendName : messenger.m_FriendNameMap)
		PyTuple_SetItem(Tuple, i++, PyString_FromString(sFriendName.c_str()));

	return Tuple;
}
#endif

void initMessenger()
{
	static PyMethodDef s_methods[] =
	{
		{ "RemoveFriend", messengerRemoveFriend, METH_VARARGS },
		{ "IsFriendByName", messengerIsFriendByName, METH_VARARGS },
		{ "Destroy", messengerDestroy, METH_VARARGS },
		{ "RefreshGuildMember", messengerRefreshGuildMember, METH_VARARGS },
		{ "SetMessengerHandler", messengerSetMessengerHandler, METH_VARARGS },
#if defined(ENABLE_MESSENGER_BLOCK)
		{ "IsBlockFriendByName", messengerIsBlockFriendByName, METH_VARARGS },
		{ "RemoveBlock", messengerRemoveBlock, METH_VARARGS },
#endif
#if defined(ENABLE_MAILBOX)
		{ "GetFriendNames", messengerGetFriendNames, METH_VARARGS },
#endif
		{ NULL, NULL, NULL },
	};

	Py_InitModule("messenger", s_methods);
}
