#ifndef _SLICE_H
#define _SLICE_H

// -------------------------------------------------------------------- //
#define UNSIGNED_DIFF( _a, _b ) ( (_a) < (_b) ? (_b) - (_a) : (_a) - (_b) )

// "<< 6" is equal to "*TEXTURE_SIZE"
// ">> 3" is equal to "/8"
//#define GET_WALL_DOT( _z, _x, _y )  _z[ (_x) + ( ((_y) >> 3) << 6 ) ] & ( 1 << ( (_y) % 8 ) )
#define GET_WALL_DOT( _z, _x, _y )  _z[ ((_x ) * ((TEXTURE_SIZE+7)/8)) + ( ((_y) /8)  ) ] & ( 1 << ( (_y) % 8 ) )


#define BLOCK_SIZE    20
#define BLOCK_SIZE_F  20.0f

#define HIT_WIDTH     5.0f

#define MAP_WIDTH   24
#define MAP_HEIGHT  24

// 320x130 == 41600 pixels, and still looks pretty good! aspect ratio: 2,46:1
// when 300x160 == 48000 pixels. aspect ratio: 1,88:1
#define LCD_WIDTH   320 //300
#define LCD_HEIGHT  130 //160

#define TEXTURE_SIZE      64
// calculated as (TEXTURE_SIZE/BLOCK_SIZE)
#define TEXTURE_SIZE_OFF  3.2    // for 64x;
//#define TEXTURE_SIZE_OFF  1.6    // for 32x;
//#define TEXTURE_SIZE_OFF  0.8    // for 16x;


#define OFFSET_X   0   // \__ render window offset
#define OFFSET_Y   20  // /

#define VISPLANEDIST_TIMES_WALLHEIGHT  3000.0f

#define PI_NUM      3.14159265f
#define PI_1_2_NUM  1.57079632f // 1/2 of pi number
#define PI_1_3_NUM  1.04719755f // 1/3 of pi number
#define PI_1_4_NUM  0.78539816f // 1/4 of pi number
#define PI_1_5_NUM  0.62831853f // 1/5 of pi number
#define PI_1_6_NUM  0.52359877f // 1/6 of pi number

#define RENDER_STEP_SIZE 4

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
    uint16_t overflowY;
} slice_t;

// -------------------------------------------------------------------- //
// public
void render(void);
void moveCamera(uint8_t direction);
void setCameraPosition(uint16_t posX, uint16_t posY, uint16_t angle);
// private
void castRay( float x, float y, float angle, uint16_t screenX, slice_t *result );
bool shouldInterpolate( slice_t *sliceA, slice_t *sliceB );
// -------------------------------------------------------------------- //

#endif /*_SLICE_H */
