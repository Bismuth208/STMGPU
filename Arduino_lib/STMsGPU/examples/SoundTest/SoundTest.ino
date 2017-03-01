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
#define FS(x) (__FlashStringHelper*)(x)

const char Loremipsum2[] = "\
Lorem ipsum dolor sit amet, consectetur adipiscing elit. \
Curabitur adipiscing ante sed nibh tincidunt feugiat. \
Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. \
Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. \
Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. \
In vestibulum purus a tortor imperdiet posuere.\n\n";
// ---------------------------------------------------------- //

void setup() {
  //BAUD_SPEED_9600 = 9600
  //BAUD_SPEED_57600 = 57600
  //BAUD_SPEED_115200 = 115200
  //BAUD_SPEED_1M = 1000000
  gpu.begin(BAUD_SPEED_1M);
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
	delay(30); // delay betwen chars, make retro effect
  }

  delay(lastDelay); // delay to see text
}

