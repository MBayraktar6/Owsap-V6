/**
* File : block_country.h
* Date : 2007.05.31
* Author : mhh
* Description :
**/

#ifndef __INC_BLOCK_COUNTRY_H__
#define __INC_BLOCK_COUNTRY_H__

void add_blocked_country_ip(TPacketBlockCountryIp* data);
void block_exception(TPacketBlockException* data);
bool is_blocked_country_ip(const char* user_ip);
bool is_block_exception(const char* login);

#endif // __INC_BLOCK_COUNTRY_H__
