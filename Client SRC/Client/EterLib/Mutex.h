#ifndef __INC_ETER_LIB_MUTEX_H__
#define __INC_ETER_LIB_MUTEX_H__

class Mutex
{
public:
	Mutex();
	~Mutex();

	void Lock();
	void Unlock();
	bool Trylock();

private:
	CRITICAL_SECTION lock;

};

#endif // __INC_ETER_LIB_MUTEX_H__
