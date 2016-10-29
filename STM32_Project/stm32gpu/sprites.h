#ifndef SPRITES_H
#define SPRITES_H

// -------------------------------------------------------- //
#ifdef STM32F10X_MD
 #define MAX_SPRITE_NUM          56
 #define LAST_SPRITE_SIZE        1024
#endif
#ifdef STM32F10X_HD
// if STM32F103VET6 64k RAM
 #define MAX_SPRITE_NUM          63
 #define LAST_SPRITE_SIZE        4096
#endif

// number of pixels in given sprite
#define SPR_SIZE_1X1_8          64
#define SPR_SIZE_1X2_8          128  // for 2x1 same size
#define SPR_SIZE_2X2_8          256

// number of tiles in given sprite
#define SPR_TLE_NUM_1x1         1
#define SPR_TLE_NUM_1x2         2  // for 2x1 same size
#define SPR_TLE_NUM_2x2         4

// Sprites for tiles 8x8
#define SPR_1X1_8 0
#define SPR_1X2_8 1
#define SPR_2X1_8 2
#define SPR_2X2_8 3

// Sprites for tiles 16x16
#define SPR_1X1_16 4
#define SPR_1X2_16 5
#define SPR_2X1_16 6
#define SPR_2X2_16 7

// Srites for tiles 32x32 - avaliable only on GPU PRO version!
#define SPR_1X1_32 8
#define SPR_1X2_32 9
#define SPR_2X1_32 10
#define SPR_2X2_32 11

// -------------------------------------------------------- //

typedef struct {
 uint16_t posX;
 uint16_t posY;
 uint8_t type;
 uint8_t visible;
 uint8_t tle[4];
} sprite_t;

// -------------------------------------------------------- //

void setSpritePosition(uint8_t sprNum, uint16_t posX, uint16_t posY);
void setSpriteType(uint8_t sprNum, uint8_t type);
void setSpriteVisible(uint8_t sprNum, uint8_t state);
void setSpriteTiles(uint8_t sprNum, uint8_t tle1, uint8_t tle2, uint8_t tle3, uint8_t tle4);
void setSpritesAutoRedraw(uint8_t state);
void drawSprite(uint8_t sprNum);

uint8_t getSpriteCollision(uint8_t sprNum1, uint8_t sprNum2);

#endif /* SPRITES_H */