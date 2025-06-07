#ifndef __INC_PACKET_HEADER_INFO_H__
#define __INC_PACKET_HEADER_INFO_H__

#include "packet.h"

typedef struct SPacketElement
{
	int iSize;
	std::string stName;
	int iCalled;
	DWORD dwLoad;
#if defined(__SEND_SEQUENCE__)
	bool bSequencePacket;
#endif
} TPacketElement;

typedef std::map<int, TPacketElement*> PacketMap;

class CPacketInfo
{
public:
	CPacketInfo();
	virtual ~CPacketInfo();

	void Set(int header, int size, const char* c_pszName, bool bSeq);
	bool Get(int header, int* size, const char** c_ppszName);

	void Start();
	void End();

	void Log(const char* c_pszFileName);

#if defined(__SEND_SEQUENCE__)
	bool IsSequence(int header);
	void SetSequence(int header, bool bSeq);
#endif

private:
	TPacketElement* GetElement(int header);

protected:
	PacketMap m_pPacketMap;
	TPacketElement* m_pCurrentPacket;
	DWORD m_dwStartTime;
};

class CPacketInfoCG : public CPacketInfo
{
public:
	CPacketInfoCG();
	virtual ~CPacketInfoCG();
};

// PacketInfo P2P 
class CPacketInfoGG : public CPacketInfo
{
public:
	CPacketInfoGG();
	virtual ~CPacketInfoGG();
};

/// Implemented in input_udp.cpp
class CPacketInfoUDP : public CPacketInfo
{
public:
	CPacketInfoUDP();
	virtual ~CPacketInfoUDP();
};

#endif // __INC_PACKET_HEADER_INFO_H__
