#ifndef _GUI_H
#define _GUI_H

void setGUITextSize(uint8_t size);
void setTextBGColorGUI(uint16_t text, uint16_t bg);
void setColorWindowGUI(uint16_t frame, uint16_t border);

void drawWindowGUI(int16_t posX, int16_t posY, int16_t w, int16_t h);
void drawTextWindowGUI(int16_t posX, int16_t posY, int16_t w, int16_t h, void *text);

#endif /* _GUI_H */
