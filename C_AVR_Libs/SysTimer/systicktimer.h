#ifndef _SYSTICKTIMER_H
#define _SYSTICKTIMER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif
    
uint32_t uptime(void);
uint32_t micros(void);

void _delayMS(uint32_t timetoloop);
void _delayMicroseconds(uint16_t us);
void initSysTickTimer(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
