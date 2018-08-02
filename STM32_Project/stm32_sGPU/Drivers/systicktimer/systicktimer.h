#ifndef __SYSTICKTIMER_H
#define __SYSTICKTIMER_H

#ifdef __cplusplus
 extern "C" {
#endif

#define USE_USER_FUNCTION 1
#define PERIOD_CHECK_USER_FUNC  1      // every 10 msec
 
void _delayMS(volatile uint32_t duty);
void initSysTickTimer(void);
uint32_t _uptime(void);

#if USE_USER_FUNCTION
void setSysTimerRuntine(void (*pFunc)(void));
#endif

#ifdef __cplusplus
}
#endif

#endif /* __SYSTICKTIMER_H */
