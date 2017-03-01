#ifndef _FSMCDRV_H
#define _FSMCDRV_H

// Use 1st bank of FSMC
// adreses for 8 bit mode, in 16 bit mode addr will be: 0x60020000 !
#define LCD_FSMC_CMD 	(*((volatile uint16_t*) 0x60000000))
#define LCD_FSMC_DATA	(*((volatile uint16_t*) 0x60020000))

#define FSMC_SEND_DATA(a)      LCD_FSMC_DATA=a
#define FSMC_SEND_DATA32(a,b)  LCD_FSMC_DATA=a;LCD_FSMC_DATA=b
#define FSMC_SEND_CMD(a)       LCD_FSMC_CMD=a

#define LCD_WRITE_REGISTER(reg, data)   LCD_FSMC_CMD=reg;LCD_FSMC_DATA=data;
//#define LCD_BEGIN_RAM_WRITE             LCD_FSMC_CMD=CTR_WRITE_DATA;
//#define LCD_WRITE_RAM(data)             LCD_FSMC_DATA=data



void sendRegData(uint16_t reg, uint16_t data);
void sendCMD8_FSMC(uint16_t c);
void sendData8_FSMC(uint16_t c);
void sendData8_Arr_FSMC(uint8_t *arr, uint16_t size);
void sendData16_FSMC(uint16_t data);
void sendData16_Arr_FSMC(uint16_t *arr, uint16_t size);
void repeatData16_Arr_FSMC(uint16_t data, uint32_t size);
void sendData32_FSMC(uint16_t data1, uint16_t data2);

void initFSMC_GPIO(void);
void initFSMC(void);


#endif /* FSMCDRV_H */