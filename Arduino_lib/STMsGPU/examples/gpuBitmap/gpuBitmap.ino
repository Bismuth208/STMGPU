#include <STMsGPU.h>

// --------------------------------------------------------- //

//#define CHK_GPU_BSY_PIN 2 // which pin arduino must check

/* BE CAREFULL!! USED ONLY HARDWARE SERIAL PORT!!
*  If your board have only ONE hardware serial,
*  then you MUST use SoftWareSerial instead!
*/
//STMGPU gpu(CHK_GPU_BSY_PIN); // use hardware BSY check, pin used
STMGPU gpu; // use software BSY check, no pin used

// ---------------------------------------------------------- //
void setup() {
  //USART_BAUD_9600 = 9600
  //USART_BAUD_57600 = 57600
  //USART_BAUD_115200 = 115200
  //USART_BAUD_1M = 1000000
  gpu.begin(USART_BAUD_1M);
}

void loop() {
  // sGPU add *.bmp extension automatically
  gpu.printBMP(0, 20, F("vice24")); // draw bmp picture at position
  gpu.printBMP(F("circuit"));   // \ 
  gpu.printBMP(F("tiger"));     //  \__ draw pic at 0,0 position
  gpu.printBMP(F("house"));     // /

  gpu.fillScreen(COLOR_BLACK);
}
