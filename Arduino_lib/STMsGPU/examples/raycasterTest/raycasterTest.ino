#include <STMsGPU.h>

// --------------------------------------------------------- //

//#define CHK_GPU_BSY_PIN 2 // which pin arduino must check

/*  BE CAREFULL!! USED ONLY HARDWARE SERIAL PORT!!
 *  If your board have only ONE hardware serial,
 *  then you MUST use SoftWareSerial instead!
 *  On STM32 boards used Serial1 on PA9 and PA10.
 */
//STMGPU gpu(CHK_GPU_BSY_PIN); // use hardware BSY check, pin used
STMGPU gpu; // use software BSY check, no pin used
// --------------------------------------------------------- //

/* pins to attach analoge sticks
 * at lest need two stick whith 2 move direction.
 * One stick for Left Right, Up, Down directions,
 * and one for turn clockwise and backwise.
 */
#define PIN_DIR_X  0 // Analoge A0 pin
#define PIN_DIR_Y  1 // Analoge A1 pin
#define PIN_DIR_LR 2 // Analoge A2 pin

#define PIN_NUM_COUNT 3 // total pins to read

#define FPS_MIN 10
#define FPS_MAX 30

// these array need to store calibrated values X, Y, or LR
// (center positions of sticks)
int calValueXYLR[PIN_NUM_COUNT] = { 0 };

int pinsToReadXYLR[PIN_NUM_COUNT] = {
  PIN_DIR_X,
  PIN_DIR_Y,
  PIN_DIR_LR
};

// store directions in array, codes of directions are defined in STMsGPU.h
uint8_t dirArray[PIN_NUM_COUNT][2] = {
  { MOVE_LEFT, MOVE_RIGHT },
  { MOVE_UP, MOVE_DOWN },
  { MOVE_CLOCKWISE_R, MOVE_CLOCKWISE_L }
};

uint32_t lastMicros, thisMicros;
uint32_t delayLong = 30;
// ---------------------------------------------------------- //

void setup() {
  // different speeds can be found in library STMsGPU.h
  gpu.begin(BAUD_SPEED_1M); // BAUD_SPEED_1M = 1,000,000 bod/s

  gpu.fillScreen(COLOR_DARKGREY);

  // first call for calibration center values
  calibrateAnalogeSticks();

  //gpu.loadLevelMap(MAP_X_SIZE, MAP_Y_SIZE, "lvlFileName"); // future feature
  //gpu.setCamPosition(X_POS, Y_POS, LR_POS);  // future feature
  //gpu.setTextureMode(MODE_X); // 16x16px or 32x32px // future feature

  // say to gpu: "render single frame"
  // force render, overwise user will see something only after moving sticks
  gpu.renderFrame();
}

void loop() {
  uint8_t camPos = checkAnalogeSticks(); // get move directions

  if(camPos != 0) { // if user not move, gpu don`t render frame.
    gpu.moveCamera(camPos); // send direction moves
    gpu.renderFrame();

    // delay for frame limit, sGPU can glitch from DDoS like that
    // it also decrese input lag (i`m siriosly, this is not joke!)
    //delay(100);
    limitFPS();
  }
}

void limitFPS(void)
{
  //This show how long we draw previous frame
  lastMicros = thisMicros;
  thisMicros = millis();
  uint32_t resultMicros = thisMicros - lastMicros;
    
  uint32_t fps = 1000/resultMicros;
 
  if(fps <= FPS_MIN) --delayLong;    //trying to get 30 fps
  if(fps >= FPS_MAX) ++delayLong;    //but not more 60 fps

  delay(delayLong*2); //slow down! it`s too faast!
}

void calibrateAnalogeSticks(void)
{
  for (uint8_t i = 0; i < PIN_NUM_COUNT; i++) {
    calValueXYLR[i] = getPinValue(pinsToReadXYLR[i]);
  }
}

inline uint8_t getPinValue(int pin)
{
  int newValuePin = analogRead(pin);

  // approximate newValuePin data, and encode to ASCII 
  // (just because it works and i left it as is)
  return (newValuePin * 9 / 1024) + 48;
}

uint8_t checkAnalogeSticks(void)
{
  // store new direction value
  uint8_t direction =0;
  int newValueXYLR = 0;

  for(uint8_t count =0; count < PIN_NUM_COUNT; count++) {
    // get number of pin to read, and store result
    newValueXYLR = getPinValue(pinsToReadXYLR[count]);
    
    // stick position changed?
    if(newValueXYLR != calValueXYLR[count]) {
      // is it more than calibrated value? in both case apply direction
      direction |= (newValueXYLR > calValueXYLR[count] ? dirArray[count][0] : dirArray[count][1]);
    }
  }
  // return combined directions in single byte
  return direction;
}
