#include <STMsGPU.h>

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

const char Loremipsum2[] = "\
Lorem ipsum dolor sit amet, consectetur adipiscing elit. \
Curabitur adipiscing ante sed nibh tincidunt feugiat. \
Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. \
Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. \
Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. \
In vestibulum purus a tortor imperdiet posuere.\n\n";
// ---------------------------------------------------------- //

void setup() {
  // different speeds can be found in library STMsGPU.h
  // if call gpu.begin() without param speed will be BAUD_SPEED_57600
  gpu.begin(BAUD_SPEED_1M); // BAUD_SPEED_1M = 1,000,000 bod/s
}

void loop() {
  drawSlowEpicText(Loremipsum2, 5000);
  gpu.fillScreen(COLOR_BLACK);
}


void drawSlowEpicText(const char* text, uint16_t lastDelay)
{
  uint16_t len = strlen(text);

  gpu.setCursor(0, 0); // start position for text

  for(uint16_t count =0; count < len; count++) {
    gpu.print(text[count]);
    gpu.playNote(500, 1);
	  gpu.iDelay(30); // delay betwen chars, make retro effect
  }

  gpu.iDelay(lastDelay); // delay to see text
}

