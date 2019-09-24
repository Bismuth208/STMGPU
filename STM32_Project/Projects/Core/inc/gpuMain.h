// -------------  TO DO: ---------- //
/*
*
*
*
*
*       MAKE SOME MAGIC
*            HERE
*
*
*
*/
// -------------------------------- //

#ifndef _GPUMAIN_H
#define _GPUMAIN_H

//===========================================================================//

#define MAX_TEXT_SIZE   192

//===========================================================================//
#define MAX_FILL_BUF    40      // in percent, warning if buffer overflowed
#define MIN_FILL_BUF    1

#define CALC_BUF_FILL(a)   ((SERIAL_BUFFER_SIZE/100)*a)

#define CALC_MAX_FILL_SIZE      CALC_BUF_FILL(MAX_FILL_BUF)
#define CALC_MIN_FILL_SIZE      CALC_BUF_FILL(MIN_FILL_BUF)

// Buffer bsy indication, CPU MUST check this pin EVERYTIME before send any command!
// othervice, undefined behavor, can happen evething (buffer overflow, wrong commands)
//#define USE_BSY_PROTECTION      1

// sGPU communication interface definitions (if you need another one)
#define GPU_INTERFACE_SEND_DATA_8(data)            sendData8_UART1(data)
#define GPU_INTERFACE_SEND_ARR_DATA_8(data, size)  sendArrData8_UART1(data, size)

#define GPU_INTERFACE_GET_AVALIABLE_DATA()   dataAvailable_UART1()
#define GPU_INTERFACE_GET_DATA_8()           waitCutByte_UART1()
#define GPU_INTERFACE_GET_DATA_16()          waitCutWord_UART1()

#define GPU_INTERFACE_GET_P_BUFFER_DATA(size)          waitCutpBuf_UART1(size)
#define GPU_INTERFACE_GET_BUFFER_DATA(ptr, size)       waitCutBuf_UART1(ptr, size)

#define GPU_INTERFACE_FFLUSH()                fflush_UART1()

//===========================================================================//
#define T_GPU_VERSION   "Build: " __DATE__" " __TIME__ " \n"

#define T_SELECT_WAY    "Selected interface: "
#define T_UART_WAY      "UART_1\n"
#define T_INIT          "Init... "
#define T_WAIT_SYNC     "Waiting for sync... "
#define T_TFT_SIZE      "Sending TFT size to host... "
#define T_GPU_START     "Start GPU...\n"

#define T_SOFT_BSY      "Software "
#define T_HARD_BSY      "Hardware "
#define T_BSY_STATE     "BSY is set by default.\n"

#define T_BAUD_SPEED    "UART speed: "
#define T_BAUD_9600     "9600 Bd/s\n"
#define T_BAUD_57K      "57600 Bd/s\n"
#define T_BAUD_115K     "115200 Bd/s\n"
#define T_BAUD_1M       "1M Bd/s\n"
#define T_DAUD_DEFAULT  "Default (57600)\n"

#define T_OK            "ok.\n"
#define T_FAIL          "fail.\n"

#define T_AUTHOR_ME     "Powered by Bismuth208"

//===========================================================================//

#include "sGPU_registers.h"

//===========================================================================//


typedef union {
  uint8_t data[14];
  struct {
    uint16_t par1;
    uint16_t par2;
    uint16_t par3;
    uint16_t par4;
    uint16_t par5;
    uint16_t par6;
    uint16_t par7;
  };
} cmdBuffer_t;

extern cmdBuffer_t cmdBuffer;
extern uint8_t bsyInterfaceFlag; // 0 - use software, 1 - use hardware

//===========================================================================//
int main(void);

void init_GPU(void);
void sync_CPU(void);
__attribute__((noreturn)) void run_GPU(void);


void initRand(void);
uint32_t randNum(void);
void drawBootLogo(void);

// ------------------------------------------------------------------- //


#endif /* _GPUMAIN_H */
