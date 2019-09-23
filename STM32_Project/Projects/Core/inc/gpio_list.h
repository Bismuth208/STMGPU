// -------------  TO DO: ---------- //
/*
*
*
*
*
*       MAKE SOME MAGIC
*            HERE
*
*
*
*/
// -------------------------------- //

#ifndef _GPIO_LIST_H
#define _GPIO_LIST_H


#define GPIO_SET_PIN(GPIOx, GPIO_Pin)     GPIO_SetBits(GPIOx, GPIO_Pin);
#define GPIO_RESET_PIN(GPIOx, GPIO_Pin)   GPIO_ResetBits(GPIOx, GPIO_Pin);

// -------------------------------------------------------- //
// Buzzer on PB5 -> TIM3_CH2
#define SOUND_PIN      GPIO_Pin_5
#define SOUND_GPIO     GPIOB

// -------------------------------------------------------- //
#define GPU_BSY_PIN             GPIO_Pin_11
#define GPU_BSY_PORT            GPIOA

#define GPU_DEBUG_PORT          GPIOA
#define GPU_DEBUG_PIN           GPIO_Pin_12

#ifdef STM32F10X_MD  // maple mini board
 #define GPU_BSY_LED_PIN         GPIO_Pin_13
 #define GPU_BSY_LED_PORT        GPIOC
#endif
#ifdef STM32F40XX
 #define GPU_BSY_LED_PIN         GPIO_Pin_7
 #define GPU_BSY_LED_PORT        GPIOA
#endif

// -------------------------------------------------------- //
#define GPU_BAUD_SELECT_PIN_0   GPIO_Pin_0
#define GPU_BAUD_SELECT_PIN_1   GPIO_Pin_1
#define GPU_BAUD_SELECT_PIN_2   GPIO_Pin_2
#define GPU_BAUD_SELECT_PORT    GPIOA

// -------------------------------------------------------- //
#define GPU_UART_RX_PIN     GPIO_Pin_10
#define GPU_UART_TX_PIN     GPIO_Pin_9
#define GPU_UART_PORT       GPIOA

// ------------------------- SPI_1 ------------------------- //
//sck - pa5; miso - pa6; mosi - pa7;
#define GPIO_Pin_SPI_LCD_SCK    GPIO_Pin_5      // SCK
#define GPIO_Pin_SPI_LCD_MISO   GPIO_Pin_6     // DO
#define GPIO_Pin_SPI_LCD_MOSI   GPIO_Pin_7     // DI
#define GPIO_SPI_LCD            GPIOA


// -------------------------------------------------------- //
//nss - pb10; dc - pb11; res - pb1
#define TFT_SS_PIN         GPIO_Pin_10     //CS
#define TFT_DC_PIN         GPIO_Pin_11     //DC
#define TFT_RES_PIN        GPIO_Pin_1      //RES on PB1 (pro and mini)
#define LCD_BACKLIGHT_PIN  GPIO_Pin_6
#define BACKLIGHT_GPIO     GPIOB

// -------------------------------------------------------- //
// TODO: move SD card pins here

#endif /* _GPIO_LIST_H */
