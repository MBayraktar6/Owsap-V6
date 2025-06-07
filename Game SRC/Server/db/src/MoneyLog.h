#ifndef __INC_MONEY_LOG
#define __INC_MONEY_LOG

#include <map>

typedef struct SMoneyLog
{
	int iGold;
#if defined(__CHEQUE_SYSTEM__)
	int iCheque;
#endif
} TMoneyLog;

typedef std::map<DWORD, TMoneyLog> MoneyLogMap;

class CMoneyLog : public singleton<CMoneyLog>
{
public:
	CMoneyLog();
	virtual ~CMoneyLog();

	void Save();
	void AddLog(BYTE bType, DWORD dwVnum, int iGold
#if defined(__CHEQUE_SYSTEM__)
		, int iCheque
#endif
	);

private:
	MoneyLogMap m_MoneyLogContainer[MONEY_LOG_TYPE_MAX_NUM];
};

#endif
