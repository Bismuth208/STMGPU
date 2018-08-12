#ifndef _TONE_H
#define _TONE_H


// -------------------------------------------------------- //
// Buzzer on PB5 -> TIM3_CH2
#define SOUND_PIN      GPIO_Pin_5
#define SOUND_GPIO     GPIOB

#define SOUND_BUF_SIZE      50  // size for note patterns
#define SOUND_BUF_PATTERNS  3    // number of sound patterns

#define MIN_FREQ_LVL        50   // minimum frequency level
#define MAX_FREQ_LVL        8000 // maximum frequency level
#define CHECK_FREQ(a)       ((a > MIN_FREQ_LVL) ||  (a < MAX_FREQ_LVL))

#define RELOAD_TIM_VAL(freq)          (SystemCoreClock / (freq * TIM3->PSC) - 1)
#define BEEP_LONG(freq, duration)     ((freq / 100) * duration + 1)

#define END_PATTERN  {0,0}

// -------------------------------------------------------- //
// single note
typedef struct {
  uint16_t frequency;
  uint8_t  duration;
} notes_t;
// -------------------------------------------------------- //

static const notes_t startupSound[] = {
  {1000,3},
  {   0,3},
  {1500,3},
  {   0,3},
  {2000,3},
  {   0,3},
  {600, 4},
  END_PATTERN
};


extern notes_t soundBuffer[SOUND_BUF_SIZE];

// -------------------------------------------------------- //

// Function prototypes
void init_Sound(void);
void playNote_Sound(uint16_t freq, uint16_t duration);
void disable_Sound(void);

void playBuf_Sound(void);
//void playBuf_Sound(uint8_t pattern);
void playNotes_Sound(const notes_t *notes);

// -------------------------------------------------------- //


#endif /* _TONE_H */
