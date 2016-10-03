#ifndef _SDWORKER_H
#define _SDWORKER_H


#define T_MOUNT_SD              "Mount sd... "
#define T_OK                    "ok.\n"
#define T_FAIL                  "fail.\n"
//#define T_TLE_SET_EXT_NAME      ".tle"
//define T_MAP_SET_EXT_NAME     ".map"

void init_sdCard(void);

void SDLoadTileFromSet8x8(uint8_t *tileSetArrName, uint8_t tileSetW, uint8_t ramTileNum, uint8_t tileNum);
void SDLoadTileSet8x8(uint8_t *tileSetArrName, uint8_t tileSetW, uint8_t ramTileBase, uint8_t tileMax);
void SDLoadRegionOfTileSet8x8(uint8_t *tileSetArrName, uint8_t tileSetW, uint8_t ramTileBase, uint8_t tileMin, uint8_t tileMax);

#endif /* _SDWORKER_H */