#ifndef __INC_ETER_BASE_RANDOM_H__
#define __INC_ETER_BASE_RANDOM_H__

extern void srandom(unsigned long seed);
extern unsigned long random();
extern float frandom(float flLow, float flHigh);
extern long random_range(long from, long to);

#endif // __INC_ETER_BASE_RANDOM_H__
