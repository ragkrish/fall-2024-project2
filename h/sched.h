/* sched.h - EXPDISTSCHED, LINUXSCHED, setschedclass, setschedclass */

#ifndef _SCHED_H_
#define _SCHED_H_

// Scheduling class constants
#define DEFAULTSCHED 0
#define EXPDISTSCHED 1
#define LINUXSCHED 2

// Function prototypes
void setschedclass(int sched_class);
int getschedclass(void);

#endif // _SCHEDCLASS_H_