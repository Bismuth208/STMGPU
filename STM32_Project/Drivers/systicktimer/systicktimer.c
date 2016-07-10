/* This one for:
* 1. delay function
* 
* 2. current time in millis from system startup
* 
*
* Author: Alexandr Antonov (Bismuth208)
* Created:  11.9.2015
*/
#include <stdlib.h>
#include "systicktimer.h"

static __IO uint32_t systemCurrentMilis = 0;

void (*pUserRunTimeFunc)(void) = NULL;
static __IO uint8_t usrTimeOutCheck =0;

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

void SysTimerSetRuntine(void (*pFunc)(void))
{
  pUserRunTimeFunc = pFunc;
}
 
void SysTick_Handler(void)
{
  ++systemCurrentMilis;
  
  if(pUserRunTimeFunc != NULL) {
    if((++usrTimeOutCheck) >= PERIOD_CHECK_USER_FUNC) {
      usrTimeOutCheck=0;
      pUserRunTimeFunc();
    } 
  }
}

void initSysTickTimer(void)
{
  // 0.001 s = 1/1000 s = 1ms
  if (SysTick_Config(SystemCoreClock / 1000)){
    /* if not 0 - error */
    while (1);
  }
  systemCurrentMilis =0;
}
//==========================================//