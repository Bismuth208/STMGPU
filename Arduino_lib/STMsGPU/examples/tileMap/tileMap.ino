#include <STMsGPU.h>

//#define CHK_GPU_BSY_PIN 2 // which pin arduino must check

/* BE CAREFULL!! USED ONLY HARDWARE SERIAL PORT!!
*  If your board have only ONE hardware serial,
*  then you MUST use SoftWareSerial instead!
*  On STM32 boards used Serial1 on PA9 and PA10.
*/
//STMGPU gpu(CHK_GPU_BSY_PIN); // use hardware BSY check, pin used
STMGPU gpu; // use software BSY check, no pin used


// ---------------------------------------------------------- //
void setup() {
  //BAUD_SPEED_9600 = 9600
  //BAUD_SPEED_57600 = 57600
  //BAUD_SPEED_115200 = 115200
  //BAUD_SPEED_1M = 1000000
  gpu.begin(BAUD_SPEED_1M);

  /* load 36 tiles to GPU's RAM at 0 position in it's RAM,,
  *  from tileFileName,
  *  located on SD card attached to STM32 GPU
  *  7 - is width of tileSet in tiles ( 7 tiles width == 56 pixels)
  *  file name must respond to 8.3 name system
  *  8 chars max for filename and 3 chars max for file extension
  *  sGPU add *.tle extension automatically
  */
  gpu.loadTileSet8x8("mario", 7, 0, 0, 36);


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
