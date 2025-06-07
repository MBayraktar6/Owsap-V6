#ifndef __INC_LZO_MANAGER_H__
#define __INC_LZO_MANAGER_H__

#include "minilzo.h"

class LZOManager : public singleton<LZOManager>
{
public:
	LZOManager();
	virtual ~LZOManager();

	bool Compress(const BYTE* src, size_t srcsize, BYTE* dest, lzo_uint* puiDestSize);
	bool Decompress(const BYTE* src, size_t srcsize, BYTE* dest, lzo_uint* puiDestSize);
	size_t GetMaxCompressedSize(size_t original);

	BYTE* GetWorkMemory() { return m_workmem; }

private:
	BYTE* m_workmem;
};

#endif // __INC_LZO_MANAGER_H__
