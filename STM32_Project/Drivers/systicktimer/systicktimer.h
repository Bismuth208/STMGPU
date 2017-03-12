#ifndef _SYSTICKTIMER_H
#define _SYSTICKTIMER_H

#define USE_USER_FUNCTION 1
#define PERIOD_CHECK_USER_FUNC  1      // every 10 msec
 
void _delayMS(volatile uint32_t duty);
void initSysTickTimer(void);
uint32_t _uptime(void);

#if USE_USER_FUNCTION
void setSysTimerRuntine(void (*pFunc)(void));
#endif

#endif /* _SYSTICKTIMER_H */