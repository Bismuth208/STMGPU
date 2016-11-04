#ifndef _SYSTICKTIMER_H
#define _SYSTICKTIMER_H

#define USE_USER_FUNCTION 0
#define PERIOD_CHECK_USER_FUNC  10      // every 10 msec

#ifdef __cplusplus
 extern "C" {
#endif
 
void _delayMS(volatile uint32_t duty);
void initSysTickTimer(void);
uint32_t _uptime(void);

#if USE_USER_FUNCTION
void SysTimerSetRuntine(void (*pFunc)(void));
#endif

#ifdef __cplusplus
 extern "C" {
#endif

#endif /* _SYSTICKTIMER_H */