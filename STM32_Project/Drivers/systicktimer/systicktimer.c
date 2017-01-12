/* This one for:
* 1. delay function
* 
* 2. current time in millis from system startup
* 
*
* Author: Alexandr Antonov (Bismuth208)
* Created:  11.9.2015
*/

#include <stm32f10x.h>

#include <stdlib.h>
#include "systicktimer.h"

static __IO uint32_t systemCurrentMilis = 0;

#if USE_USER_FUNCTION
void (*pUserRunTimeFunc)(void) = NULL;
static __IO uint8_t usrTimeOutCheck =0;
#endif

//==========================================//
void _delayMS(__IO uint32_t delayTime)
{
  uint32_t duty = _uptime();
  while((_uptime() - duty) < delayTime);
}

uint32_t _uptime(void)
{
  return systemCurrentMilis;
}

#if USE_USER_FUNCTION
void SysTimerSetRuntine(void (*pFunc)(void))
{
  pUserRunTimeFunc = pFunc;
}
#endif
 
void SysTick_Handler(void)
{
  ++systemCurrentMilis;

#if USE_USER_FUNCTION
  if(pUserRunTimeFunc != NULL) {
    if((++usrTimeOutCheck) >= PERIOD_CHECK_USER_FUNC) {
      usrTimeOutCheck=0;
      pUserRunTimeFunc();
    } 
  }
#endif
}

void initSysTickTimer(void)
{
  // 0.001 s = 1/1000 s = 1ms
  // 0.01 s = 1/100 s = 10ms
  if (SysTick_Config(SystemCoreClock / 100)){
    /* if not 0 - error */
    while (1);
  }
  systemCurrentMilis =0;
}
//==========================================//