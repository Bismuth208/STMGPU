#ifndef _SDLOADER_H
#define _SDLOADER_H


#define T_MOUNT_SD              "Mount sd..."
#define T_OK                    "ok.\n"
#define T_FAIL                  "fail.\n"

#define T_BMP_SET_EXT_NAME      ".bmp"  // 0x00
#define T_PAL_SET_EXT_NAME      ".pal"  // 0x01
#define T_MAP_SET_EXT_NAME      ".map"  // 0x03
#define T_TLE_SET_EXT_NAME      ".tle"  // 0x02
#define T_CSV_SET_EXT_NAME      ".csv"  // 0x04

#define BMP_FILE_MARK           0x4D42
#define BMP_FILE_HEADER_SIZE    18
#define BMP_345_VER_MARK        0xC

#define LF_FILE_MARK 0x0A
#define CR_FILE_MARK 0x0D


typedef struct  {
  uint8_t tileSetW;
  uint8_t ramTileBase;
  uint8_t tileMin;
  uint8_t tileMax;
} tileParam_t;

#pragma pack(push, 1)
typedef union {
  uint8_t bmpArr[BMP_FILE_HEADER_SIZE];
  struct {
    uint16_t fileMark;       // file mark; 0x00
    uint32_t fileSize;       // files size; 0x02
    uint32_t creatorBytes;   // creator bytes; 0x06, 0x08
    uint32_t bmpImageoffset; // Start of image data; 0x0A
    uint32_t headerSize;     // bcSize ( biSize, bV4Size, bV5Size ); 0x0E
  };
} bmpCore_t;
#pragma pack(pop)

#if 0
typedef union {
  uint8_t bmpArr[8];
  struct {
    uint16_t bmpWidth;  // 0x12
    uint16_t bmpHeight; // 0x14
    uint16_t planes;    // 0x16
    uint16_t bmpDepth;  // 0x18
  };
} bmpExtCore_t;

typedef union {
  uint8_t bmpArr[12];
  struct {
    uint32_t bmpWidth;  // 0x12
    uint32_t bmpHeight; // 0x16
    uint16_t planes;    // 0x1A
    uint16_t bmpDepth;  // 0x1C
  };
} bmpExt345_t;
#endif

// a pointer to function returning pointer to tile array uint8_t*, 
// and accepted uint8_t as nuber of tile array
typedef uint8_t *(*pfGetFunc_t)(uint8_t);

void init_sdCard(void);

void SDLoadTile(void *fileName, uint8_t *pData, uint16_t tleBaseSize, uint16_t tlesNumSize);
void SDLoadTileSet(void *fileName, uint8_t *pData, uint16_t tleBaseSize, uint16_t tlesNumSize);

void SDLoadTileMap(void *fileName);
void SDLoadPalette(void *fileName);
void SDPrintBMP(uint16_t x, uint16_t y, void *fileName);

#endif /* _SDLOADER_H */
