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
}

void loop() {
  showDifference();
  delay(2000);
  changeColors();
  delay(2000);
  multiWindows();
  delay(2000);
}

void showDifference(void)
{
  gpu.drawWindowGUI(10, 10, 300, 200); // simple windows whitout text
  // text stored in RAM memory

  // window whith text in it;
  // text stored in ROM memory
  gpu.drawWindowGUI(10, 10, 300, 200, F("this text is writed in window by \
    drawWindowGUI() function, and have normal text wrap"));

  gpu.print("this text is writed not in window by print() function and have no normal text wrap");
}

void changeColors(void)
{
  gpu.fillScreen(COLOR_BLACK);

  gpu.setColorWindowGUI(COLOR_DARKGREEN, COLOR_LIGHTGREY);
  gpu.setTextColorGUI(COLOR_BLACK, COLOR_YELLOW);
  gpu.setTextSizeGUI(2);
  
  gpu.drawWindowGUI(10, 10, 300, 200, "This text changed"); // text stored in RAM memory
  gpu.print(F("But this one not!")); // text stored in ROM memory
}

void multiWindows(void)
{
  gpu.fillScreen(COLOR_WHITE);
  gpu.setTextSizeGUI(1);

  // al text below stored in ROM memory
  gpu.drawWindowGUI(10, 10, gpu.width()/2, gpu.height()/2, F("Left"));

  gpu.drawWindowGUI(gpu.width()/2+4, 10, gpu.width()/2, gpu.height()/2, F("Right"));

  gpu.drawWindowGUI(10, gpu.height()/2, 300, gpu.height()/2, F("Bottom window"));
}

