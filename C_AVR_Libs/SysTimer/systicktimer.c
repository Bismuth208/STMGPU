#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "systicktimer.h"

#ifndef  F_CPU
#define  F_CPU 16000000UL
#warning "F_CPU difined here"
#endif

#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define microsecondsToClockCycles(a) ( (a) * clockCyclesPerMicrosecond() )

// the prescaler is set so that timer0 ticks every 64 clock cycles, and the
// the overflow handler is called every 256 ticks.
#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))

// the whole number of milliseconds per timer0 overflow
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)

// the fractional number of milliseconds per timer0 overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz - this doesn't lose precision.)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)

static volatile uint32_t timer0_overflow_count = 0;
static volatile uint32_t timer0_millis = 0;
static volatile uint8_t timer0_fract = 0;

ISR(TIMER0_OVF_vect)
{
	// copy these to local variables so they can be stored in registers
	// (volatile variables must be read from memory on every access)
	uint32_t m = timer0_millis;
	uint8_t f = timer0_fract;

	m += MILLIS_INC;
	f += FRACT_INC;
	if (f >= FRACT_MAX) {
		f -= FRACT_MAX;
		++m;
	}

	timer0_fract = f;
	timer0_millis = m;
	++timer0_overflow_count;
}

uint32_t uptime()
{
	uint8_t oldSREG = SREG;

	// disable interrupts while we read timer0_millis or we might get an
	// inconsistent value (e.g. in the middle of a write to timer0_millis)
	cli();
	uint32_t m = timer0_millis;
	SREG = oldSREG;
  sei();

	return m;
}

uint32_t micros(void)
{
  cli();
  uint32_t m = timer0_overflow_count;
  uint8_t t = TCNT0;
  
  if ((TIFR0 & (1<<TOV0)) && (t < 255)) ++m;
  
  sei();
  
  return ((m << 8) + t) * (64 / clockCyclesPerMicrosecond());
}

void _delayMS(uint32_t timetoloop)
{
  while(timetoloop--) _delay_ms(1);
}

/* Delay for the given number of microseconds.  Assumes a 1, 8, 12, 16, 20 or 24 MHz clock. */
void _delayMicroseconds(uint16_t us)
{
    // call = 4 cycles + 2 to 4 cycles to init us(2 for constant delay, 4 for variable)
    
    // calling avrlib's delay_us() function with low values (e.g. 1 or
    // 2 microseconds) gives delays longer than desired.
    //delay_us(us);
#if F_CPU >= 24000000L
    // for the 24 MHz clock for the aventurous ones, trying to overclock
    
    // zero delay fix
    if (!us) return; //  = 3 cycles, (4 when true)
    
    // the following loop takes a 1/6 of a microsecond (4 cycles)
    // per iteration, so execute it six times for each microsecond of
    // delay requested.
    us *= 6; // x6 us, = 7 cycles
    
    // account for the time taken in the preceeding commands.
    // we just burned 22 (24) cycles above, remove 5, (5*4=20)
    // us is at least 6 so we can substract 5
    us -= 5; //=2 cycles
    
#elif F_CPU >= 20000000L
    // for the 20 MHz clock on rare Arduino boards
    
    // for a one-microsecond delay, simply return.  the overhead
    // of the function call takes 18 (20) cycles, which is 1us
    __asm__ __volatile__ (
                          "nop" "\n\t"
                          "nop" "\n\t"
                          "nop" "\n\t"
                          "nop"); //just waiting 4 cycles
    if (us <= 1) return; //  = 3 cycles, (4 when true)
    
    // the following loop takes a 1/5 of a microsecond (4 cycles)
    // per iteration, so execute it five times for each microsecond of
    // delay requested.
    us = (us << 2) + us; // x5 us, = 7 cycles
    
    // account for the time taken in the preceeding commands.
    // we just burned 26 (28) cycles above, remove 7, (7*4=28)
    // us is at least 10 so we can substract 7
    us -= 7; // 2 cycles
    
#elif F_CPU >= 16000000L
    // for the 16 MHz clock on most Arduino boards
    
    // for a one-microsecond delay, simply return.  the overhead
    // of the function call takes 14 (16) cycles, which is 1us
    if (us <= 1) return; //  = 3 cycles, (4 when true)
    
    // the following loop takes 1/4 of a microsecond (4 cycles)
    // per iteration, so execute it four times for each microsecond of
    // delay requested.
    us <<= 2; // x4 us, = 4 cycles
    
    // account for the time taken in the preceeding commands.
    // we just burned 19 (21) cycles above, remove 5, (5*4=20)
    // us is at least 8 so we can substract 5
    us -= 5; // = 2 cycles,
    
#elif F_CPU >= 12000000L
    // for the 12 MHz clock if somebody is working with USB
    
    // for a 1 microsecond delay, simply return.  the overhead
    // of the function call takes 14 (16) cycles, which is 1.5us
    if (us <= 1) return; //  = 3 cycles, (4 when true)
    
    // the following loop takes 1/3 of a microsecond (4 cycles)
    // per iteration, so execute it three times for each microsecond of
    // delay requested.
    us = (us << 1) + us; // x3 us, = 5 cycles
    
    // account for the time taken in the preceeding commands.
    // we just burned 20 (22) cycles above, remove 5, (5*4=20)
    // us is at least 6 so we can substract 5
    us -= 5; //2 cycles
    
#elif F_CPU >= 8000000L
    // for the 8 MHz internal clock
    
    // for a 1 and 2 microsecond delay, simply return.  the overhead
    // of the function call takes 14 (16) cycles, which is 2us
    if (us <= 2) return; //  = 3 cycles, (4 when true)
    
    // the following loop takes 1/2 of a microsecond (4 cycles)
    // per iteration, so execute it twice for each microsecond of
    // delay requested.
    us <<= 1; //x2 us, = 2 cycles
    
    // account for the time taken in the preceeding commands.
    // we just burned 17 (19) cycles above, remove 4, (4*4=16)
    // us is at least 6 so we can substract 4
    us -= 4; // = 2 cycles
    
#else
    // for the 1 MHz internal clock (default settings for common Atmega microcontrollers)
    
    // the overhead of the function calls is 14 (16) cycles
    if (us <= 16) return; //= 3 cycles, (4 when true)
    if (us <= 25) return; //= 3 cycles, (4 when true), (must be at least 25 if we want to substract 22)
    
    // compensate for the time taken by the preceeding and next commands (about 22 cycles)
    us -= 22; // = 2 cycles
    // the following loop takes 4 microseconds (4 cycles)
    // per iteration, so execute it us/4 times
    // us is at least 4, divided by 4 gives us 1 (no zero delay bug)
    us >>= 2; // us div 4, = 4 cycles
    
    
#endif
    
    // busy wait
    __asm__ __volatile__ (
                          "1: sbiw %0,1" "\n\t" // 2 cycles
                          "brne 1b" : "=w" (us) : "0" (us) // 2 cycles
                          );
    // return = 4 cycles
}

void initSysTickTimer()
{
    sei();
    
    // set timer 0 prescale factor to 64
    // this combination is for the standard 168/328/1280/2560 mega
    TCCR0B = (1<<CS01) | (1<<CS00);
    
    // enable timer 0 overflow interrupt
    TIMSK0 = (1<<TOIE0);
}
