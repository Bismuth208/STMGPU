#ifndef _FSMCDRV_H
#define _FSMCDRV_H

void sendCMD8_FSMC(uint8_t c);
void sendData8_FSMC(uint8_t c);
void sendData8_Arr_FSMC(uint8_t *arr, uint16_t size);
void sendData16_FSMC(uint16_t data);
void sendData16_Arr_FSMC(uint16_t *arr, uint16_t size);
void repeatData16_Arr_FSMC(uint16_t data, uint32_t size);
void sendData32_FSMC(uint16_t data1, uint16_t data2);

void initFSMC_GPIO(void);
void initFSMC(void);


#endif /* FSMCDRV_H */