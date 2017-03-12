#ifndef _SLICE_H
#define _SLICE_H

// -------------------------------------------------------------------- //
#define UNSIGNED_DIFF( _a, _b ) ( (_a) < (_b) ? (_b) - (_a) : (_a) - (_b) )

#define BLOCK_SIZE    20
#define BLOCK_SIZE_F  20.0f

#define HIT_WIDTH     4.8f
#define MOVE_SPEED    4.0f

// calculated as "PI_NUM / 25.0f"
#define ROT_ANGLE_STEP 0.12566370f

#define MAP_WIDTH   24
#define MAP_HEIGHT  24

// Set seize of render window
// 320x130 == 41600 pixels, and still looks pretty good!
// aspect ratio: 2,46:1 (alomst silver ratio)
#define RENDER_W_WIDTH   320
#define RENDER_W_HEIGHT  130

#define TEXTURE_SIZE      16
// calculated as (TEXTURE_SIZE/BLOCK_SIZE)
//#define TEXTURE_SIZE_OFF  3.2    // for 64x;
//#define TEXTURE_SIZE_OFF  1.6    // for 32x;
#define TEXTURE_SIZE_OFF  0.8    // for 16x;
//#define TEXTURE_SIZE_OFF  0.4    // for 8x8 px, really, dont use it! It's terreble!

#define TEXTURE_QA_MIN 0
#define TEXTURE_QA_MAX 2


#define OFFSET_X   0   // \__ render window offset
#define OFFSET_Y   20  // /

#define VISPLANEDIST_TIMES_WALLHEIGHT  4000.0f

#define MIN_SINE_VAL  0.000001f

#define PI_NUM      3.14159265f
#define PI_1_2_NUM  1.57079632f // 1/2 of pi number
#define PI_1_3_NUM  1.04719755f // 1/3 of pi number
#define PI_1_4_NUM  0.78539816f // 1/4 of pi number
#define PI_1_5_NUM  0.62831853f // 1/5 of pi number
#define PI_1_6_NUM  0.52359877f // 1/6 of pi number

#define MAP_TEXTURES_NUM_MAX  10 // maximum of textures per map level

#define RENDER_STEP_SIZE   4  // default value
#define RENDER_QA_MIN      4
#define RENDER_QA_MAX      8

// make shadow for Y side in RGB565 color system
#define R_COLOR(color)         (((color & 0xF800)>>1) & 0xF800)
#define G_COLOR(color)         (((color & 0x07E0)>>1) & 0x07E0)
#define B_COLOR(color)         (((color & 0x001F)>>1) & 0x001F)
#define SHADOW_Y_SIDE(color)   (R_COLOR(color) | G_COLOR(color) | B_COLOR(color))
// -------------------------------------------------------------------- //
#define MOVE_UP            0x01
#define MOVE_DOWN          0x02
#define MOVE_LEFT          0x04
#define MOVE_RIGHT         0x08
#define MOVE_CLOCKWISE_R   0x10
#define MOVE_CLOCKWISE_L   0x20
// -------------------------------------------------------------------- //

// Holds the result of a ray being cast.
typedef struct {
  uint16_t targetBlockX;
  uint16_t targetBlockY;
  uint16_t textureId;
  uint16_t textureOffset;
  uint16_t sliceHeight;
} slice_t;

// -------------------------------------------------------------------- //
// public
void renderWalls(void);
void moveCamera(uint8_t direction);
void setCameraPosition(uint16_t posX, uint16_t posY, uint16_t angle);
void serRenderQuality(int8_t quality);
void setTextureQuality(int8_t quality);
void setWallCollision(bool state);

// for init and internal use only
void setRayCastPalette(uint16_t *pPal);
void setLevelMap(uint8_t *pLevelMap);
void setTileArrayPonter(uint8_t *pTleArrayRAM);
// -------------------------------------------------------------------- //

#endif /*_SLICE_H */
