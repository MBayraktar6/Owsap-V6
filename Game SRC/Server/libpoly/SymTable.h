#ifndef __INC_POLY_SYMTABLE_H__
#define __INC_POLY_SYMTABLE_H__

#include <string>

class CSymTable
{
public:
	CSymTable(int aTok, std::string aStr);
	virtual ~CSymTable();

	double dVal;
	int token;
	std::string strlex;
};

#endif // __INC_POLY_SYMTABLE_H__
