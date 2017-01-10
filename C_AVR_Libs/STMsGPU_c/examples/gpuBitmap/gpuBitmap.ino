#include <systicktimer.h>

#include <uart.h>
#include <STMsGPU_c.h>

// ---------------------------------------------------------- //
// sGPU add *.bmp extension automatically
const uint8_t bmpFileName1[] = "vice24";
const uint8_t bmpFileName2[] = "circuit";
const uint8_t bmpFileName3[] = "tiger";
const uint8_t bmpFileName4[] = "house";

// ---------------------------------------------------------- //
__attribute__ ((noreturn)) int main(void)
{
  initSysTickTimer(); //it`s enable timer0 on atmega328p;

  uartSetup(USART_BAUD_1M);
  sync_gpu();

  for(;;) {
  	SDPrintBMP(0, 20, (const char*)bmpFileName1);
    SDPrintBMP(0, 0, (const char*)bmpFileName2);
    SDPrintBMP(0, 0, (const char*)bmpFileName3);
    SDPrintBMP(0, 0, (const char*)bmpFileName4);

    tftfillScreen(COLOR_BLACK);
  }
}
