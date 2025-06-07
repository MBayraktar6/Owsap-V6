#ifndef __INC_LIB_SQL_TELLWAIT_H__
#define __INC_LIB_SQL_TELLWAIT_H__

#ifndef __WIN32__

extern void TELL_WAIT();

extern void WAIT_CHILD();
extern void TELL_CHILD(pid_t pid);

extern void WAIT_PARENT();
extern void TELL_PARENT(pid_t pid);

#endif

#endif // __INC_LIB_SQL_TELLWAIT_H__
