#ifndef __INC_BAN_IP_H__
#define __INC_BAN_IP_H__

extern bool LoadBanIP(const char* filename);
extern bool IsBanIP(struct in_addr in);

#endif // __INC_BAN_IP_H__
