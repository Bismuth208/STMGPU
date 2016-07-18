#ifndef _SDWORKER_H
#define _SDWORKER_H


void init_sdCard(void);

void SDLoadTileFromSet8x8(uint8_t *tileSetArrName, uint8_t tileSetW, uint8_t ramTileNum, uint8_t tileNum);
void SDLoadTileSet8x8(uint8_t *tileSetArrName, uint8_t tileSetW, uint8_t ramTileBase, uint8_t tileMax);
void SDLoadRegionOfTileSet8x8(uint8_t *tileSetArrName, uint8_t tileSetW, uint8_t ramTileBase, uint8_t tileMin, uint8_t tileMax);

#endif /* _SDWORKER_H */