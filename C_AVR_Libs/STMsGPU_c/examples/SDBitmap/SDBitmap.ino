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
int main(void)
{
  // it`s enable timer0 on atmega328p;
  // need for delays;
  initSysTickTimer();

  //BAUD_SPEED_9600 = 9600
  //BAUD_SPEED_57600 = 57600
  //BAUD_SPEED_115200 = 115200
  //BAUD_SPEED_1M = 1000000
  sync_gpu(BAUD_SPEED_1M);  // establish connection

  for(;;) {
  	gpuSDPrintBMPat(0, 20, bmpFileName1);
    gpuSDPrintBMP(bmpFileName2);
    gpuSDPrintBMP(bmpFileName3);
    gpuSDPrintBMP(bmpFileName4);

    gpuFillScreen(COLOR_BLACK);
  }

  return 0;
}
