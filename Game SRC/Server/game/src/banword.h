#ifndef __INC_BANWORD_MANAGER_H__
#define __INC_BANWORD_MANAGER_H__

class CBanwordManager : public singleton<CBanwordManager>
{
public:
	CBanwordManager();
	virtual ~CBanwordManager();

	bool Initialize(TBanwordTable* p, WORD wSize);
	bool Find(const char* c_pszString);
	bool CheckString(const char* c_pszString, size_t _len);
	void ConvertString(char* c_pszString, size_t _len);

protected:
	typedef std::unordered_map<std::string, bool> TBanwordHashmap;
	TBanwordHashmap m_hashmap_words;
};

#endif // __INC_BANWORD_MANAGER_H__
