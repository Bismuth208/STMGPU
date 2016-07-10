#ifndef SYSTICKTIMER_H
#define SYSTICKTIMER_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stm32f10x.h>
   
#define PERIOD_CHECK_USER_FUNC  10      // every 10 msec
 
void _delayMS(__IO uint32_t duty);
void initSysTickTimer(void);
uint32_t _uptime(void);

void SysTimerSetRuntine(void (*pFunc)(void));

#ifdef __cplusplus
 extern "C" {
#endif

#endif /* SYSTICKTIMER_H */