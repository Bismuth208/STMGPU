#include <STMsGPU.h>

#define FS(x) (__FlashStringHelper*)(x)

// --------------------------------------------------------- //

//#define CHK_GPU_BSY_PIN 2 // which pin arduino must check

/* BE CAREFULL!! USED ONLY HARDWARE SERIAL PORT!!
*  If your board have only ONE hardware serial,
*  then you MUST use SoftWareSerial instead!
*/
//STMGPU gpu = STMGPU(CHK_GPU_BSY_PIN); // use hardware BSY check, pin used
STMGPU gpu = STMGPU(); // use software BSY check, no pin used

// --------------------------------------------------------- //
// sGPU add *.bmp extension automatically
const char bmpFileName1[] = "vice24";
const char bmpFileName2[] = "circuit";
const char bmpFileName3[] = "tiger";
const char bmpFileName4[] PROGMEM = "house"; // this string as example for RAM save

// ---------------------------------------------------------- //
void setup() {
  //USART_BAUD_9600 = 9600
  //USART_BAUD_57600 = 57600
  //USART_BAUD_115200 = 115200
  //USART_BAUD_1M = 1000000
  gpu.sync(USART_BAUD_1M);
}

void loop() {
  gpu.printBMP(0, 20, bmpFileName1); // draw bmp picture at position
  gpu.printBMP(bmpFileName2);        // \__ draw pic at 0,0 position
  gpu.printBMP(bmpFileName3);        // /
  gpu.printBMP(FS(bmpFileName4));    // same as above, but name stored in ROM

  gpu.fillScreen(COLOR_BLACK);
}
