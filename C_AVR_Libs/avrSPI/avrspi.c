#include <avr/io.h>
#include <avr/interrupt.h>

#include "avrspi.h"

/*
 * The following NOP introduces a small delay that can prevent the wait
 * loop form iterating when running at the maximum speed. This gives
 * about 10% more speed, even if it seems counter-intuitive. At lower
 * speeds it is unnoticed.
 */
#define SPDR_TX_WAIT asm volatile("nop"); while (!(SPSR & (1<<SPIF))) ;

static uint8_t backupSPCR;
static uint8_t backupSPSR;
static uint8_t backupSREG;

typedef union {
    uint16_t val;
    struct {
        uint8_t lsb;
        uint8_t msb;
    };
} SPDR_t;    // this one will use only registers

void initSPI(void)
{
  backupSREG = SREG;
  cli();
  
  DDRB |= (1 << PB5) | (1 << PB3) | (1 << PB2);
    
    /* PB5 - SCK
     * PB3 - MOSI
     * PB2 - CS (SS) // system chip select for the atmega board
     */

  // Warning: if the SS pin ever becomes a LOW INPUT then SPI
  // automatically switches to Slave, so the data direction of
  // the SS pin MUST be kept as OUTPUT.
  //SPCR =  (0<<SPIE) | (1<<SPE) | (0<<DORD) | (1<<MSTR)|(0<<CPOL)|(0<<CPHA)|(0<<SPR1)|(0<<SPR0);
  SPCR =  (1<<SPE) | (1<<MSTR);
  SPSR = (1 << SPI2X);  // double speed
    /* Iterrupt: disable;
     * SPI: enable;
     * Data order: MSB;
     * Master/slave: Master;
     * Clock polarity: Rising;
     * Clock [hase: Leading -> Tralling edge;
     * Clock Rate: fclk/4 */
	
	SET_MOSI_HI
	SET_SCK_HI
  SET_SS_HI

  SREG = backupSREG;
}

// this function is used to gain exclusive access to the SPI bus
// and configure the correct settings.
void beginTrSPI(uint8_t spcr, uint8_t spsr)
{
  backupSPCR = SPCR;
  backupSPSR = SPSR;
  backupSREG = SREG;
    
  cli();
    
  SPCR = spcr;
  SPSR = spsr;
}

// Write to the SPI bus (MOSI pin) and also receive (MISO pin)
uint8_t sendReadDataSPI(uint8_t data)
{
  SPDR = data;
  SPDR_TX_WAIT;
  
  return SPDR;
}

uint16_t sendReadWordDataSPI(uint16_t data)
{
  //by this one only r24 and r25 will be used (if -O2);
  SPDR_t in = {.val = data}, out;
  
  SPDR = in.msb;
  SPDR_TX_WAIT;
  out.msb = SPDR;
  
  SPDR = in.lsb;
  SPDR_TX_WAIT;
  out.lsb = SPDR;
  
  return out.val;
}

void sendData8_SPI1(uint8_t data)
{
  SPDR = data;
  SPDR_TX_WAIT;
}

void sendData16_SPI1(uint16_t data)
{
  //by this one only r24 and r25 will be used (if -O2);
  SPDR_t in = {.val = data};
  
  SPDR = in.msb;
  SPDR_TX_WAIT;
  
  SPDR = in.lsb;
  SPDR_TX_WAIT;
}

void sendReadArrSPI(void *buf, uint16_t count)
{
  uint8_t *p = (uint8_t *)buf;
  
  SPDR = *p;
  
  while (count--) {
    SPDR_TX_WAIT;
    *p++ = SPDR;
    SPDR = *(p + 1);
  }
  
  while (!(SPSR & (1<<SPIF))) ;
  *p = SPDR;
}

void sendArrSPI(uint8_t *buf, uint16_t size)
{
  uint8_t count;
  
  for(count = 0; count < size; count++){
    
    SPDR = buf[count];
    SPDR_TX_WAIT
  }
  
  
  
}

// After performing a group of transfers and releasing the chip select
// signal, this function allows others to access the SPI bus
void endTrSPI(void)
{
  SPCR = backupSPCR;
  SPSR = backupSPSR;
  SREG = backupSREG;
}

// --------------------------------------------------------- //
#if 0
void initUSARTSPI(void)
{
  backupSREG = SREG;
  
  cli(); // Protect from a scheduler and prevent transactionBegin
  
  DDRD |= (1 << PD4);
  SET_XCK_HI;
  
  // PD4 - XCK  (SCK)
  
  UCSR0A = 1<<U2X0;
  // Set USART SPI mode of operation and SPI data mode 1,1. UCPHA1 = UCSZ10
  UCSR0B = (1<<TXEN0);             // Enable transmitter. Enable the Tx (also disable the Rx, and the rest of the interrupt enable bits set to 0 too).
  // Set clock polarity and phase to correct SPI mode.
  UCSR0C   = (1 << UCPOL0) | (1 << UCPHA0) | (1 << UMSEL01)   | (1 << UMSEL00);
  
  // Set baud rate. IMPORTANT: The Baud Rate must be set after the Transmitter is enabled.
  UBRR0 = 0;                  // Where maximum speed of FCPU/2 = 0x0000
  
  SREG = backupSREG;
}

void sendDataUSPI(uint8_t data)
{
  UDR0 = data;        // Begin transmission

  //asm volatile("nop");
  while ( !(UCSR0A & (1<<TXC0)) ); // wait
  UCSR0A = _BV(TXC0);              // Clear the Transmit complete flag, all other bits should be written 0
}

void sendWordDataUSPI(uint16_t data)
{
  //by this one only r24 and r25 will be used (if -O2);
  tSPDR in = {.val = data};
  
  UDR0 = in.msb;
  //asm volatile("nop");
  while ( !(UCSR0A & (1<<UDRE0))); // wait
  //UCSR0A = _BV(TXC0);
  UDR0 = in.lsb;
  //asm volatile("nop");
  //while (!(SPSR & _BV(SPIF))) ;
  while ( !(UCSR0A & (1<<TXC0)) ); // wait
  UCSR0A = _BV(TXC0);
}
#endif
// --------------------------------------------------------- //

