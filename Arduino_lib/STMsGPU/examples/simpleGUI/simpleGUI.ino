#include <STMsGPU.h>

// ---------------------------------------------------------- //
/* in library STMsGPU.h exist define USE_GPU_RETURN_RESOLUTION
 * set it to 0 and it can save ROM and increase speed, but its unfair!
 * use it if you know end resolution
 */
#define TFT_W gpu.width()
#define TFT_H gpu.height()

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
  gpu.begin(BAUD_SPEED_1M); // BAUD_SPEED_1M = 1,000,000 bod/s
}

void loop() {
  showDifference();
  gpu.iDelay(2000);
  changeColors();
  gpu.iDelay(2000);
  multiWindows();
  gpu.iDelay(2000);
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
  gpu.drawWindowGUI(10, 10, TFT_W/2, TFT_H/2, F("Left"));

  gpu.drawWindowGUI(TFT_W/2+4, 10, TFT_W/2, TFT_H/2, F("Right"));

  gpu.drawWindowGUI(10, TFT_H/2, 300, TFT_H/2, F("Bottom window"));
}
