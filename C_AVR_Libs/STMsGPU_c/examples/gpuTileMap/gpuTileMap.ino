#include <systicktimer.h>

#include <uart.h>
#include <STMsGPU_c.h>

// ---------------------------------------------------------- //

const uint8_t tileFileName[] = "mario.tle";
const uint8_t mapFileName[] = "mario.map";

// ---------------------------------------------------------- //
__attribute__ ((noreturn)) int main(void)
{
  initSysTickTimer(); //it`s enable timer0 on atmega328p;

  uartSetup(USART_BAUD_1M);
  sync_gpu();

  /* load 90 tiles to GPU's RAM at 0 position in RAM,
  *  from tileFileName,
  *  located on SD card attached to STM32 GPU
  *  7 - is width of tileSet in tiles ( 7 tiles width == 56 pixels)
  *  file name must respond to 8.3 name system
  *  8 chars max for filename and 3 chars max for file extension
  */
  SDLoadTileSet8x8((const char*)tileFileName, 7, 0, 36);

  SDLoadTileMap((const char*)mapFileName);

  drawBackgroundMap();

  for(;;) {
  }
}
