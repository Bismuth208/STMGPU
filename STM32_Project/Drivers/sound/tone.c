#include <stdbool.h>
#include <stm32f10x.h>

#include "tone.h"

// disable volatile warning
#pragma diag_suppress=Pa082

// -------------------------------------------------------- //

volatile bool playingState;
volatile uint32_t beepDuration;
volatile const notes_t *pNotes;

// audiobuffer whith stored samples
notes_t soundBuffer[SOUND_BUF_SIZE]; // SOUND_BUF_SIZE * 3 bytes == actual size
//notes_t soundBuffer[SOUND_BUF_PATTERNS][SOUND_BUF_SIZE];

GPIO_InitTypeDef soundPort;

// -------------------------------------------------------- //

// Initialize sound output
void init_Sound(void) 
{  
  RCC->APB2ENR |= RCC_APB2Periph_AFIO;        // enable remap config for GPIO
  RCC->APB1ENR |= RCC_APB1Periph_TIM3;        // enable TIM3 peripheral
  
  // configure GPIO
  soundPort.GPIO_Mode  = GPIO_Mode_AF_PP;
  soundPort.GPIO_Speed = GPIO_Speed_2MHz;
  soundPort.GPIO_Pin   = SOUND_PIN;
  GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); // remap PA7 to PB5
  GPIO_Init(SOUND_GPIO, &soundPort);
  
  // Configure timer
  TIM_TimeBaseInitTypeDef timer3Init;
  timer3Init.TIM_CounterMode = TIM_CounterMode_Up;        /* Select the Counter Mode */
  timer3Init.TIM_Period =  1000;                          /* Set the Autoreload value */
  timer3Init.TIM_Prescaler =  SystemCoreClock / 4000000;  /* Set the Prescaler value */
  timer3Init.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_ARRPreloadConfig(TIM3, ENABLE);                     /* Set the ARR Preload Bit */
  TIM_TimeBaseInit(TIM3, &timer3Init);                    // apply and generate update event to reload prescaler value immediately
  
  TIM_OCInitTypeDef timer3OC2init;
  timer3OC2init.TIM_OCMode = TIM_OCMode_PWM1;             /* Select the Output Compare Mode */
  timer3OC2init.TIM_OCPolarity = TIM_OCPolarity_Low;      /* Set the Output Compare Polarity */
  timer3OC2init.TIM_OutputState = TIM_OutputState_Enable; /* Set the Output State */
  timer3OC2init.TIM_Pulse = 500;                          /* Set the Capture Compare Register value */
  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);       /* Enable the Output Compare Preload feature */
  TIM_OC2Init(TIM3, &timer3OC2init);                      // apply and TIM3 CH2 output compare enable
  
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);              // TIM3 update interrupt enable
  
  // setup NVIC for TIM3 IRQ channel
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
// -------------------------------------------------------- //

void reloadTIMxValue_Sound(uint16_t freq, uint16_t duration)
{
  beepDuration = BEEP_LONG(freq, duration);
  
  // calc reload value
  TIM3->ARR  = RELOAD_TIM_VAL(freq);
  TIM3->CCR2 = TIM3->ARR >> 1; // set 50% duty cycle
  TIM3->CR1 |= TIM_CR1_CEN;    // timer enable
}

// 1 param - frequency Hz
// 2 param - duration in milliseconds
void playNote_Sound(uint16_t freq, uint16_t duration)
{
  if (CHECK_FREQ(freq) || (duration)) {
    
    if(!playingState) { // prevent from reinit
      // enable pin
      soundPort.GPIO_Mode  = GPIO_Mode_AF_PP;
      GPIO_Init(SOUND_GPIO, &soundPort);
    }
    
    beepDuration = BEEP_LONG(freq, duration);
    
    // calc reload value
    TIM3->ARR  = RELOAD_TIM_VAL(freq);
    TIM3->CCR2 = TIM3->ARR >> 1; // set 50% duty cycle
    TIM3->CR1 |= TIM_CR1_CEN;    // timer enable
    
  } else {
    disable_Sound();
  }
}

void playNextNote_Sound(void)
{
  if (CHECK_FREQ(pNotes->frequency)) {
    
    beepDuration = BEEP_LONG(pNotes->frequency, pNotes->duration);
    
    // calc reload value
    TIM3->ARR  = RELOAD_TIM_VAL(pNotes->frequency);
    TIM3->CCR2 = TIM3->ARR >> 1; // 50% duty cycle
    TIM3->CR1 |= TIM_CR1_CEN;    // timer enable
  }
}

// play notes sequence
void playNotes_Sound(const notes_t *notes)
{
  pNotes = notes;
  playingState = true;
  playNote_Sound(pNotes->frequency, pNotes->duration);
}

// play pattern in audio buffer
//void playBuf_Sound(uint8_t patternNum)
void playBuf_Sound(void)
{
  //pNotes = &soundBuffer[patternNum][0][0];
  pNotes = soundBuffer;
  playingState = true;
  playNote_Sound(pNotes->frequency, pNotes->duration);
}

// Turn off buzzer
void disable_Sound(void)
{
  TIM3->CR1 &= ~TIM_CR1_CEN; // disable timer
  
  // disable sound pin
  soundPort.GPIO_Mode  = GPIO_Mode_AIN; // pin as analog input
  GPIO_Init(SOUND_GPIO, &soundPort);
}

// -------------------------------------------------------- //
void TIM3_IRQHandler(void)
{
  if(TIM3->SR & TIM_SR_UIF) {      // is it our IRQ?
    TIM3->SR &= ~TIM_SR_UIF;       // clear TIM3 interrupt pending bit
    
    if(--beepDuration == 0) {      // note end?
      if(playingState) {           // have active sound pattern?
        ++pNotes;                  // take next note
        if(pNotes->duration) {     // have notes?
          if(pNotes->frequency) {  // have sound?
            playNextNote_Sound();  // play next tone in sequence
          } else { // nope, just silence
            TIM3->ARR = RELOAD_TIM_VAL(100);
            TIM3->CCR2 = 0; // 0% duty cycle
            // only duration, freq will be == 0
            beepDuration = BEEP_LONG(100, pNotes->duration);
          }
        } else { // nope, that was last
          disable_Sound();
          playingState = false;
        }
      } else { // nope, all done
        disable_Sound();
      }
    }
  }
}

// restore volatile warning
#pragma diag_warning=Pa082
