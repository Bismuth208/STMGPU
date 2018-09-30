#include <systicktimer.h>
#include <uart.h>

#include <STMsGPU_c.h>

/* BE CAREFULL!! USED ONLY HARDWARE SERIAL PORT!!
 * If your board have only ONE hardware serial,
 * then you MUST use softWare serial instead!
 * moreover arduino libs here totally not supported!
 */

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
    // at current moment sopported only BMP16 and BMP24 (whith no alpha channel)!
  	gpuPrintBMPat(0, 0, "tree1");
    gpuPrintBMP("tree2");
    gpuPrintBMP("tree36");
  }

  return 0;
}
