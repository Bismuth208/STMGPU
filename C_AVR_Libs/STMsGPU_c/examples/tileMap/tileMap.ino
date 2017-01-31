#include <systicktimer.h>
#include <uart.h>
#include <STMsGPU_c.h>

// --------------------------------------------------------- //
#define MAX_TILES 36
#define RAM_BASE 0
#define TLE_START 0
#define TILE_SET_W 8 // ( 7 tiles width == 56 pixels)

// ---------------------------------------------------------- //

const uint8_t tileFileName[] = "mario";
const uint8_t mapFileName[] = "mario";

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

  /* load MAX_TILES tiles to GPU's RAM at RAM_BASE position in it's RAM,
  *  from tileFileName, located on SD card attached to STM32 GPU;
  *  TILE_SET_W - is width of tileSet in tiles;
  *  TLE_START - nunber of tile in tileset from which tiles will be loaded;
  *  File name must respond to 8.3 name system,
  *  8 chars max for filename, 3 chars max for file extension.
  *  sGPU add *.tle extension automatically.
  */
  gpuSDLoadTileSet8x8(tileFileName, TILE_SET_W-1, RAM_BASE, TLE_START, MAX_TILES);

  // sGPU add *.map extension automatically.
  gpuSDLoadTileMap(mapFileName);

  gpuDrawBackgroundMap();

  for(;;) {  }
  return 0;
}
