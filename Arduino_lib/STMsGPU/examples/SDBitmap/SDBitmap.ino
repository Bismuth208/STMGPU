#include <STMsGPU.h>

// --------------------------------------------------------- //

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
}

void loop() {
  // sGPU add *.bmp extension automatically
  gpu.printBMP(0, 0, F("tree1")); // draw bmp picture at position
  gpu.printBMP(F("tree2"));  // \__ draw pic at 0,0 position
  gpu.printBMP(F("tree3"));  // /
}
