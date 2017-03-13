#include <systicktimer.h>

#include <uart.h>
#include <STMsGPU_c.h>

/* BE CAREFULL!! USED ONLY HARDWARE SERIAL PORT!!
 * If your board have only ONE hardware serial,
 * then you MUST use softWare serial instead!
 * moreover arduino libs here totally not supported!
 */

// ---------------------------------------------------------- //
const uint8_t bmpFileName1[] = "tree1";
const uint8_t bmpFileName2[] = "tree2";
const uint8_t bmpFileName3[] = "tree3";
// ---------------------------------------------------------- //

int main(void)
{
  // it`s enable timer0 on atmega328p;
  // need for delays;
  initSysTickTimer();

  // different speeds can be found in library STMsGPU_c.h
  sync_gpu(BAUD_SPEED_1M);  // BAUD_SPEED_1M = 1,000,000 bod/s

  for(;;) {
    // sGPU add *.bmp extension automatically
    // at current moment sopported only BMP24 (whith no alpha channel)!
  	gpuSDPrintBMPat(0, 20, bmpFileName1);
    gpuSDPrintBMP(bmpFileName2);
    gpuSDPrintBMP(bmpFileName3);

    gpuFillScreen(COLOR_BLACK);
  }

  return 0;
}
