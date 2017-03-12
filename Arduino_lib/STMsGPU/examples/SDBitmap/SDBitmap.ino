#include <STMsGPU.h>

// ---------------------------------------------------------- //
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
  // different speeds can be found in library STMsGPU.h
  gpu.begin(BAUD_SPEED_1M); // BAUD_SPEED_1M = 1,000,000 bod/s
}

void loop() {
  // sGPU add *.bmp extension automatically
  // at current moment sopported only BMP24 (whith no alpha channel)!
  gpu.printBMP(0, 0, F("tree1")); // draw bmp picture at position
  gpu.printBMP(F("tree2"));  // \__ draw pic at 0,0 position
  gpu.printBMP(F("tree3"));  // /
}
