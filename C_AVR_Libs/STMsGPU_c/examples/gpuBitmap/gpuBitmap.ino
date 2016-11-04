#include <systicktimer.h>

#include <uart.h>
#include <STMsGPU_c.h>

// ---------------------------------------------------------- //

const uint8_t bmpFileName1[] = "vice24.bmp";
const uint8_t bmpFileName2[] = "circuit.bmp";
const uint8_t bmpFileName3[] = "tiger.bmp";

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

    tftFillScreen(COLOR_BLACK);  // clear screen by black color
  }
}
