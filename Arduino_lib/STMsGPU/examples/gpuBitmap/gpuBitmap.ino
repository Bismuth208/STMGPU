#include <STMsGPU.h>

// --------------------------------------------------------- //

//#define CHK_GPU_BSY_PIN 2 // which pin arduino must check

/* BE CAREFULL!! USED ONLY HARDWARE SERIAL PORT!!
*  If your board have only ONE hardware serial,
*  then you MUST use SoftWareSerial instead!
*/
//STMGPU gpu = STMGPU(CHK_GPU_BSY_PIN); // use hardware BSY check, pin used
STMGPU gpu = STMGPU(); // use software BSY check, no pin used

// --------------------------------------------------------- //

const char bmpFileName1[] = "vice24.bmp";
const char bmpFileName2[] = "circuit.bmp";
const char bmpFileName3[] = "tiger.bmp";

// ---------------------------------------------------------- //
void setup() {
  //USART_BAUD_9600 = 9600
  //USART_BAUD_57600 = 57600
  //USART_BAUD_115200 = 115200
  //USART_BAUD_1M = 1000000
  gpu.sync(USART_BAUD_1M);
}

void loop() {

  gpu.printBMP(0, 20, bmpFileName1);
  gpu.printBMP(bmpFileName2);
  gpu.printBMP(bmpFileName3);

  gpu.fillScreen(COLOR_BLACK);
}
