#include <systicktimer.h>
#include <uart.h>
#include <STMsGPU_c.h>

/* BE CAREFULL!! USED ONLY HARDWARE SERIAL PORT!!
 * If your board have only ONE hardware serial,
 * then you MUST use softWare serial instead!
 * moreover arduino libs here totally not supported!
 */

// --------------------------------------------------------- //
// this is need to load textures from *.tle file 
// located on SD card - correctly
#define MAX_TILES 36
#define RAM_BASE 0
#define TLE_START 0
#define TILE_SET_W 8 // this is width of tileSet in tiles ( one tile width == 8 pixels)

// ---------------------------------------------------------- //

const uint8_t tileFileName[] = "mario";
const uint8_t mapFileName[] = "mario";

// ---------------------------------------------------------- //
int main(void)
{
  // it`s enable timer0 on atmega328p;
  // need for delays;
  initSysTickTimer();

  // different speeds can be found in library STMsGPU_c.h
  sync_gpu(BAUD_SPEED_1M);  // BAUD_SPEED_1M = 1,000,000 bod/s

  /* load MAX_TILES tiles to sGPU's RAM at RAM_BASE position in it's RAM,
  *  from tileFileName,
  *  located on SD card attached to STM32 sGPU
  *  TLE_START - nunber of tile in tileset from which tiles will be loaded
  *  file name must respond to 8.3 name system
  *  8 chars max for filename, 3 chars max for file extension
  *  sGPU add *.tle extension automatically
  */
  gpuLoadTileSet8x8(tileFileName, TILE_SET_W-1, RAM_BASE, TLE_START, MAX_TILES);

  // sGPU add *.map extension automatically.
  gpuLoadTileMap(mapFileName);

  gpuDrawTileMap();

  for(;;) {  }
  return 0;
}
