#include <STMsGPU.h>

// this is need to load textures from *.tle file 
// located on SD card - correctly
#define MAX_TILES 36
#define RAM_BASE 0
#define TLE_START 0
#define TILE_SET_W 7 // this is width of tileSet in tiles ( one tile width == 8 pixels)

// ---------------------------------------------------------- //
/*
 * which pin arduino must check, 
 * but by default this functianality is disabled to save
 * RAM and ROM memory.
 * To enable it go to STMsGPU.h and 
 * set define: 'REMOVE_HARDWARE_BSY' to 0
 */ 
//#define CHK_GPU_BSY_PIN 2

/* BE CAREFULL!! USED ONLY HARDWARE SERIAL PORT!!
*  If your board have only ONE hardware serial,
*  then you MUST use SoftWareSerial instead!
*  On STM32 boards used Serial1 on PA9 and PA10.
*/
//STMGPU gpu(CHK_GPU_BSY_PIN); // use hardware BSY check, pin used
STMGPU gpu; // use software BSY check, no pin used
// ---------------------------------------------------------- //

void setup() {
  // different speeds can be found in library STMsGPU.h
  // if call gpu.begin() without param speed will be BAUD_SPEED_57600
  gpu.begin(BAUD_SPEED_1M); // BAUD_SPEED_1M = 1,000,000 bod/s

  /* load MAX_TILES tiles to sGPU's RAM at RAM_BASE position in it's RAM,
  *  from tileFileName,
  *  located on SD card attached to STM32 sGPU
  *  TLE_START - nunber of tile in tileset from which tiles will be loaded
  *  file name must respond to 8.3 name system
  *  8 chars max for filename, 3 chars max for file extension
  *  sGPU add *.tle extension automatically
  */
  gpu.loadTileSet8x8("mario", TILE_SET_W, RAM_BASE, TLE_START, MAX_TILES);

  /* load tiled background to to GPU's RAM
  *  file must be located on SD card attached to STM32 GPU!
  *  sGPU add *.map extension automatically
  */
  gpu.loadTileMap("mario");

  // draw previosly loaded tile map
  gpu.drawTileMap();
}

void loop() {
  /* place some your code here */
}
