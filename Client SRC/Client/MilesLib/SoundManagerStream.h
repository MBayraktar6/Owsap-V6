#ifndef __INC_MILES_LIB_SOUND_MANAGER_STREAM_H__
#define __INC_MILES_LIB_SOUND_MANAGER_STREAM_H__

#include "SoundBase.h"
#include "SoundInstance.h"

class CSoundManagerStream : public CSoundBase
{
public:
	enum
	{
		MUSIC_INSTANCE_MAX_NUM = 3,
	};

public:
	CSoundManagerStream();
	virtual ~CSoundManagerStream();

	bool Initialize();
	void Destroy();

	bool SetInstance(DWORD dwIndex, const char* filename);
	CSoundInstanceStream* GetInstance(DWORD dwIndex);

protected:
	bool CheckInstanceIndex(DWORD dwIndex);

protected:
	CSoundInstanceStream m_Instances[MUSIC_INSTANCE_MAX_NUM];

};

#endif // __INC_MILES_LIB_SOUND_MANAGER_STREAM_H__
