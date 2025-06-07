/**
* File : auth_brazil.h
* Date : 2010.4.7
* Author : mhh
* Description :
**/

#ifndef __INC_AUTH_BRAZIL_H__
#define __INC_AUTH_BRAZIL_H__

#define AUTH_BRAZIL_SERVER_ERR 0
#define AUTH_BRAZIL_SUCC 1
#define AUTH_BRAZIL_NOID 2
#define AUTH_BRAZIL_WRONGPWD 3
#define AUTH_BRAZIL_FLASHUSER 4

int auth_brazil(const char* login, const char* pwd);

int auth_brazil_inc_query_count();
void auth_brazil_log();

#endif // __INC_AUTH_BRAZIL_H__
