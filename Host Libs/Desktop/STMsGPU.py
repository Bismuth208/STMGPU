#!/usr/bin/python
# coding:utf-8
# 
# For STM32_sGPU Project
# Created: 8.08.2018
# Last edit: 10.08.2019
# 
# author: Antonov Alexandr (Bismuth208)
#
# Tested on:
#  Python 2.7.15 and PySerial 3.4
#  Python 3.6.4 and PySerial 3.2.1

import time
import serial
from random import *
from struct import *


# ----------------------------------------------------------------------- #
class SoftGPU(object):
    """docstring for sGPU"""

    # ------------------------------------------------------------------------ #
    # Color definitions
    COLOR_BLACK = 0x0000  # 0,   0,   0
    COLOR_NAVY = 0x000F  # 0,   0, 128
    COLOR_DARKGREEN = 0x03E0  # 0, 128,   0
    COLOR_DARKCYAN = 0x03EF  # 0, 128, 128
    COLOR_MAROON = 0x7800  # 128,   0,   0
    COLOR_PURPLE = 0x780F  # 128,   0, 128
    COLOR_OLIVE = 0x7BE0  # 128, 128,   0
    COLOR_DARKGREY = 0x7BEF  # 128, 128, 128
    COLOR_LIGHTGREY = 0xC618  # 192, 192, 192
    COLOR_BLUE = 0x001F  # 0,   0, 255
    COLOR_GREEN = 0x07E0  # 0, 255,   0
    COLOR_CYAN = 0x07FF  # 0, 255, 255
    COLOR_GREENYELLOW = 0xAFE5
    COLOR_RED = 0xF800  # 255,   0,   0
    COLOR_MAGENTA = 0xF81F  # 255,   0, 255
    COLOR_ORANGE = 0xFD20  # 255, 165,   0
    COLOR_YELLOW = 0xFFE0  # 255, 255,   0
    COLOR_WHITE = 0xFFFF  # 255, 255, 255

    # ------------------------------------------------------------------------ #
    TLE_8X8 = 1
    TLE_16X16 = 2
    TLE_32X32 = 3

    # Sprites for tiles 8x8
    SPR_1X1_8 = 0
    SPR_1X2_8 = 1
    SPR_2X1_8 = 2
    SPR_2X2_8 = 3

    # Sprites for tiles 16x16
    SPR_1X1_16 = 4
    SPR_1X2_16 = 5
    SPR_2X1_16 = 6
    SPR_2X2_16 = 7

    # Srites for tiles 32x32 - avaliable only on sGPU PRO version!
    SPR_1X1_32 = 8
    SPR_1X2_32 = 9
    SPR_2X1_32 = 10
    SPR_2X2_32 = 11

    # ------------------------------------------------------------------------ #
    # Move direction defines for 3D
    MOVE_UP = 0x01
    MOVE_DOWN = 0x02
    MOVE_LEFT = 0x04
    MOVE_RIGHT = 0x08
    MOVE_CLOCKWISE_R = 0x10
    MOVE_CLOCKWISE_L = 0x20

    # definitions for texture sizes
    TEXTURE_MODE_0 = 0  # 8x8 tiles
    TEXTURE_MODE_1 = 1  # 16x16 tiles
    TEXTURE_MODE_2 = 2  # 32x32 tiles (only Mini2 and PRO versions!)

    # ------------------------------------------------------------------------ #
    _T_SGPU_REGISTERS_VER = "sGPU registers: v0.0.6"

    # -------------------------- Command list --------------------------- #
    # ------------------ Base ------------------ #
    # NOT_USED        =  0x00
    FLL_SCR = 0x01
    # NOT_USED        =  0x02
    DRW_PIXEL = 0x03

    # ------------- Primitives/GFX ------------- #
    FLL_RECT = 0x04
    DRW_RECT = 0x05
    DRW_ROUND_RECT = 0x06
    FLL_ROUND_RECT = 0x07
    DRW_LINE = 0x08
    DRW_V_LINE = 0x09
    DRW_H_LINE = 0x0A
    DRW_CIRCLE = 0x0B
    FLL_CIRCLE = 0x0C
    DRW_TRINGLE = 0x0D
    FLL_TRINGLE = 0x0E
    GET_RESOLUTION = 0x0F

    # --------------- Font/Print --------------- 3
    DRW_CHAR = 0x10  # drawChar()
    DRW_PRNT = 0x11  # print()
    DRW_PRNT_C = 0x12  # printChar()
    DRW_PRNT_POS_C = 0x13  # printCharAt()
    SET_CURSOR = 0x14  # setCursor()
    SET_TXT_CR = 0x15  # setTextColor()
    SET_TXT_CR_BG = 0x16  # setTextColorBG()
    SET_TXT_SIZE = 0x17  # setTextSize()
    SET_TXT_WRAP = 0x18  # setTextWrap()
    SET_TXT_437 = 0x19  # cp437()
    SET_TXT_FONT = 0x1A  # setTextFont()
    # NOT_USED        = 0x1B
    # NOT_USED        = 0x1C
    # NOT_USED        = 0x1D
    # NOT_USED        = 0x1E

    # ---------------- Low Level --------------- #
    SET_BRGHTNS_F = 0x1F  # setDispBrightnessFade()
    SET_ADR_WIN = 0x20
    SET_ROTATION = 0x21
    SET_SCRL_AREA = 0x22
    WRT_CMD = 0x23  # writeCommand()
    WRT_DATA = 0x24  # writeData()
    WRT_DATA_U16 = 0x25  # writeWordData()
    SET_V_SCRL_ADR = 0x26  # scrollAddress()
    SET_SLEEP = 0x27  # setSleep()
    SET_IDLE = 0x28  # setIdleMode()
    SET_BRIGHTNES = 0x29  # setDispBrightness()
    SET_INVERTION = 0x2A  # setInvertion()
    SET_GAMMA = 0x2B  # setGamma()
    MAK_SCRL = 0x2C  # scrollScreen()
    MAK_SCRL_SMTH = 0x2D  # scrollScreenSmooth()
    PSH_CR = 0x2E
    # NOT_USED        = 0x2F

    # ------------------- Tile ----------------- #
    LDD_TLE_8 = 0x30  # load tile 8x8 size from SD
    # NOT_USED        = 0x31
    LDD_TLES_8 = 0x32  # load region of tiles 8x8 size from SD
    DRW_TLE_8 = 0x33  # draw tile 8x8 size on TFT screen

    LDD_TLE_16 = 0x34  # load tile 16x16 size from SD
    # NOT_USED        = 0x35
    LDD_TLES_16 = 0x36  # load region of tiles 16x16 size from SD
    DRW_TLE_16 = 0x37  # draw tile 16x16 size on TFT screen

    LDD_TLE_32 = 0x38  # load tile 32x32 size from SD
    # NOT_USED        = 0x39
    LDD_TLES_32 = 0x3A  # load region of tiles 32x32 size from SD
    DRW_TLE_32 = 0x3B  # draw tile 32x32 size on TFT screen

    LDD_TLE_MAP = 0x3C  # load background tile map 8x8 from SD
    DRW_TLE_MAP = 0x3D  # draw background tile map 8x8 on TFT screen

    # LDD_TLE_U       = 0x3E    # load specified tile size from SD
    # DRW_TLE_U       = 0x3F    # draw specified tile size on TFT screen

    # ----------------- Sprite ----------------- #
    SET_SPR_POS = 0x40  # set sprite position
    SET_SPR_TYPE = 0x41  # set sprite type 1x2:8, 2x2:8; 1x2:16, 2x2:16;
    SET_SPR_VISBL = 0x42  # enable draw on screen
    SET_SPR_TLE = 0x43  # set tiles for sprite
    SET_SPR_AUT_R = 0x44  # enable or disable autoredraw sprite
    DRW_SPR = 0x45  # draw sprite
    GET_SRP_COLISN = 0x46  # get sprites collision
    # NOT_USED        = 0x47
    # NOT_USED        = 0x48
    # NOT_USED        = 0x49
    # NOT_USED        = 0x4A
    # NOT_USED        = 0x4B
    # NOT_USED        = 0x4C
    # NOT_USED        = 0x4D
    # NOT_USED        = 0x4E
    # NOT_USED        = 0x4F

    # ----------------- SD card ---------------- #
    LDD_USR_PAL = 0x50  # load user palette from SD card
    DRW_BMP_FIL = 0x51  # draw bmp file located on SD card
    LDD_SND_FIL = 0x52  # load sound file
    LDD_CSV_FIL = 0x53  # load and exec *.csv file
    # NOT_USED        = 0x54
    # NOT_USED        = 0x55
    # NOT_USED        = 0x56
    # NOT_USED        = 0x57
    # NOT_USED        = 0x58    # make screenshot to SD card
    # NOT_USED        = 0x59
    # NOT_USED        = 0x5A
    # NOT_USED        = 0x5B

    # ------------------ Sound ----------------- #
    # NOT_USED        = 0x5C
    # NOT_USED        = 0x5D
    SND_PLAY_TONE = 0x5E
    SND_PLAY_BUF = 0x5F

    # --------------- GUI commands -------------- #
    SET_WND_CR = 0x60  # Set window colors
    SET_WND_CR_TXT = 0x61  # set colors for GUI text
    SET_WND_TXT_SZ = 0x62  # set GUI text size
    DRW_WND_AT = 0x63  # draw window at position
    DRW_WND_TXT = 0x64  # draw window whith text
    # DRW_BTN_NUM     = 0x65    # draw numerated buttons

    # --------------- '3D' engine --------------- #
    SET_BACKGRND = 0x66
    # NOT_USED        = 0x67
    RENDER_MAP = 0x68  # render walls
    MOVE_CAMERA = 0x69
    SET_CAM_POS = 0x6A  # set current camera position
    # RENDER_BCKGRND    = 0x6B  # render background; sky, floor
    SET_RENDER_QA = 0x6C  # set render quality
    SET_TEXTURE_MODE = 0x6D  # 8x8, 16x16 or 32x32(pro only)
    SET_WALL_CLD = 0x6E  # set wall collision state
    GET_CAM_POS = 0x6F  # get current camera position
    SET_RENDER_FPS = 0x70  # set FPS limit to render

    # ---------------- NOT_USED ---------------- #
    # -------------- 0x80 - 0xFE --------------- #

    # ----------------- General ---------------- #
    BSY_SELECT = 0xFC
    CMD_GPU_PING = 0xFD
    CMD_GPU_SW_RESET = 0xFE
    SET_DBG_GPIO_PIN = 0xFF

    # ----------------------------------------------------------------------- #
    # class BaudSpeeds(Enum):
    BAUD_SPEED_9600 = 9600
    BAUD_SPEED_57600 = 57600
    BAUD_SPEED_115200 = 115200
    BAUD_SPEED_1M = 1000000

    # ----------------------------------------------------------------------- #
    DEC = 'DEC'
    HEX = 'HEX'
    OCT = 'OCT'

    # BIN = 'BIN'

    # ----------------------------------------------------------------------- #

    def __init__(self):
        super(SoftGPU, self).__init__()

        self.__version__ = self._T_SGPU_REGISTERS_VER
        self.__author__ = 'Antonov Alexandr (Bismuth208)'

        self.__ser = serial.Serial()

        self.__width = 320
        self.__height = 240

        self.__formaterBase = dict(
            DEC='%d',
            HEX='0x%X',
            OCT='0o%o'
        )

        # little speedup...
        self.__pack_B = Struct('<B').pack
        self.__pack_Bc = Struct('<Bc').pack
        self.__pack_BB = Struct('<BB').pack
        self.__pack_BH = Struct('<BH').pack
        self.__pack_BHH = Struct('<BHH').pack
        self.__pack_BHHH = Struct('<BHHH').pack
        self.__pack_BHHHH = Struct('<BHHHH').pack
        self.__pack_BHHHHH = Struct('<BHHHHH').pack
        self.__unpack_H = Struct('<H').unpack
        # another cases are too specific to implement

        self.__SYNC_OK = self.__pack_B(0xCC)
        self.__SYNC_SEQUENCE = pack('<H', 0x42DD)

        self.__BSY_MSG_CODE_READY = self.__pack_B(0xEA)
        self.__BSY_MSG_CODE_WAIT = self.__pack_B(0xEE)
        self.__GPU_MSG_CODE_PING = self.__pack_B(0xEF)

    # ----------------------------------------------------------------------- #
    def __wait_resolution(self):
        while self.__ser.in_waiting < 3:  # wait resolution
            pass
        self.__width = self.__unpack_H(self.__ser.read(2))[0]
        self.__height = self.__unpack_H(self.__ser.read(2))[0]

    def __send_command(self, data_buf):
        # check BSY
        if self.__ser.in_waiting > 0:
            if self.__ser.read() == self.__BSY_MSG_CODE_WAIT:
                print('sGPU is busy!')
                while 1:  # wait for ready
                    if self.__ser.in_waiting > 0:
                        if self.__ser.read() == self.__BSY_MSG_CODE_READY:
                            print('sGPU is ready!')  # my body is ready :)
                            break
        self.__ser.write(data_buf)

    # ----------------------------------------------------------------------- #
    def begin(self, port, baudrate=BAUD_SPEED_57600):
        self.__ser.port = port
        self.__ser.baudrate = baudrate
        self.__ser.open()

        if self.__ser.is_open:
            self.__ser.flush()
            # This command need if you don't reset sGPU
            # and just reconnect to it
            self.swReset()  # in normal reset this command will be ignored
            while 1:  # need synchronyze Host and sGPU
                self.__ser.write(self.__SYNC_SEQUENCE)
                time.sleep(.5)

                print('Sync...')
                if self.__ser.in_waiting > 0:
                    if self.__ser.read() == self.__SYNC_OK:
                        self.__wait_resolution()
                        print('Ok!')
                        break  # now in sync
        else:
            print("Cannot open serial port")

    def end(self):
        while self.__ser.out_waiting:  # wait transfer
            pass
        self.__ser.close()

    # ----------------------------------------------------------------------- #
    def width(self):
        return self.__width

    def height(self):
        return self.__height

    # ----------------------------------------------------------------------- #
    # ------------------ Base ------------------ #
    def drawPixel(self, x, y, color):
        self.__send_command(self.__pack_BHHH(
            self.DRW_PIXEL, int(x), int(y), int(color)))

    def fillScreen(self, color):
        self.__send_command(self.__pack_BH(self.FLL_SCR, int(color)))

    # ------------- Primitives/GFX ------------- #
    def fillRect(self, x, y, w, h, color):
        self.__send_command(self.__pack_BHHHHH(
            self.FLL_RECT,
            int(x), int(y),
            int(w), int(h),
            int(color)))

    def drawRect(self, x, y, w, h, color):
        self.__send_command(self.__pack_BHHHHH(
            self.DRW_RECT,
            int(x), int(y),
            int(w), int(h),
            int(color)))

    def drawRoundRect(self, x, y, w, h, radius, color):
        self.__send_command(pack(
            '<BHHHHHH', self.DRW_ROUND_RECT,
            int(x), int(y),
            int(w), int(h),
            int(radius), int(color)))

    def fillRoundRect(self, x, y, w, h, radius, color):
        self.__send_command(pack(
            '<BHHHHHH', self.FLL_ROUND_RECT,
            int(x), int(y),
            int(w), int(h),
            int(radius), int(color)))

    def drawLine(self, x0, y0, x1, y1, color):
        self.__send_command(self.__pack_BHHHHH(
            self.DRW_LINE,
            int(x0), int(y0),
            int(x1), int(y1),
            int(color)))

    def drawFastVLine(self, x, y, h, color):
        self.__send_command(self.__pack_BHHHH(
            self.DRW_V_LINE, int(x), int(y), int(h), int(color)))

    def drawFastHLine(self, x, y, w, color):
        self.__send_command(self.__pack_BHHHH(
            self.DRW_H_LINE, int(x), int(y), int(w), int(color)))

    def drawCircle(self, x, y, r, color):
        self.__send_command(self.__pack_BHHHH(
            self.DRW_CIRCLE, int(x), int(y), int(r), int(color)))

    def fillCircle(self, x, y, r, color):
        self.__send_command(self.__pack_BHHHH(
            self.FLL_CIRCLE, int(x), int(y), int(r), int(color)))

    def drawTriangle(self, x0, y0, x1, y1, x2, y2, color):
        self.__send_command(pack(
            '<BHHHHHHH', self.DRW_TRINGLE,
            int(x0), int(y0),
            int(x1), int(y1),
            int(x2), int(y2),
            int(color)))

    def fillTriangle(self, x0, y0, x1, y1, x2, y2, color):
        self.__send_command(pack(
            '<BHHHHHHH', self.FLL_TRINGLE,
            int(x0), int(y0),
            int(x1), int(y1),
            int(x2), int(y2),
            int(color)))

    def getResolution(self):
        self.__send_command(self.__pack_B(self.GET_RESOLUTION))
        self.__wait_resolution()

    # --------------- Font/Print --------------- #
    # make a DDoS to sGPU's buffer...
    def __write(self, value):
        # DO NOT CHAHNGE TO: self.__ser.write(...) !
        # ONLY self.__sendCommand(...) FUNCTION ALLOWED!
        # OTHERWISE sGPU WILL BE VEEERY UNSTABLE!
        for c in str(value):
            self.__send_command(self.__pack_Bc(self.DRW_PRNT_C, c.encode('utf-8')))

    def drawChar(self, x, y, c, char_color, bg_color, size):
        self.__send_command(pack(
            '<BHHHHBB', self.DRW_CHAR,
            int(x), int(y),
            int(char_color), int(bg_color),
            int(c), int(size)))

    def setCursor(self, x, y):
        self.__send_command(self.__pack_BHH(self.SET_CURSOR, int(x), int(y)))

    def setTextColor(self, char_color):
        self.__send_command(self.__pack_BH(self.SET_TXT_CR, int(char_color)))

    def setTextColorBG(self, char_color, bg_color):
        self.__send_command(self.__pack_BHH(
            self.SET_TXT_CR_BG, int(char_color), int(bg_color)))

    def setTextSize(self, size):
        self.__send_command(self.__pack_BB(self.SET_TXT_SIZE, int(size)))

    def setTextWrap(self, state):
        self.__send_command(self.__pack_BB(self.SET_TXT_WRAP, int(state)))

    def cp437(self, state):
        self.__send_command(self.__pack_BB(self.SET_TXT_437, int(state)))

    # def printg(self, text):
    #     self.__sendCommand(self.__pack_BB(self.DRW_PRNT, int(len(text))))
    #     self.__sendCommand(text)

    # i have no options instead of add 'g' at end...
    def printg(self, value=None, int_base=None):
        if type(value) is str:
            self.__write(value)

        if type(value) is float:
            self.__write(str(value))

        if type(value) is int:
            text = b''
            if int_base is not None:
                text = self.__formaterBase[int_base] % value
            else:
                text = str(value)
            self.__write(text)

    def println(self, value=None, int_base=None):
        if type(value) is not None:
            self.printg(value, int_base)
        self.__write('\n')

    # --------- #
    def printAt(self, x, y, value, int_base=None):
        self.setCursor(x, y)
        self.printg(value, int_base)

    # ---------------- Low Level --------------- #
    def setAddrWindow(self, x0, y0, x1, y1):
        self.__send_command(self.__pack_BHHHH(
            self.SET_ADR_WIN,
            int(x0), int(y0),
            int(x1), int(y1)))

    def setRotation(self, angle):
        self.__send_command(self.__pack_BB(self.SET_ROTATION, int(angle)))

    def setScrollArea(self, TFA, BFA):
        self.__send_command(self.__pack_BHH(
            self.SET_SCRL_AREA, int(TFA), int(BFA)))

    def scrollAddress(self, VSP):
        self.__send_command(self.__pack_BH(self.SET_V_SCRL_ADR, int(VSP)))

    def scroll(self, lines, y_start):
        self.__send_command(self.__pack_BHH(
            self.MAK_SCRL, int(lines), int(y_start)))

    # def scrollSmooth(self, lines, yStart, wait):
    #     self.__sendCommand(self.__pack_BHHH(
    #             MAK_SCRL_SMTH, int(lines), int(yStart) int(wait)))

    def setSleep(self, state):
        self.__send_command(self.__pack_BB(self.SET_SLEEP, int(state)))

    def setIdleMode(self, state):
        self.__send_command(self.__pack_BB(self.SET_IDLE, int(state)))

    def setDispBrightness(self, brightness):
        self.__send_command(self.__pack_BB(
            self.SET_BRIGHTNES, int(brightness)))

    def setInvertion(self, state):
        self.__send_command(self.__pack_BB(self.SET_INVERTION, int(state)))

    def pushColor(self, color):
        self.__send_command(self.__pack_BH(self.PSH_CR, int(color)))

    def writeCommand(self, cmd):
        self.__send_command(self.__pack_BB(self.WRT_CMD, int(cmd)))

    def writeData(self, data):
        self.__send_command(self.__pack_BB(self.WRT_DATA, int(data)))

    def writeWordData(self, data):
        self.__send_command(self.__pack_BH(self.WRT_DATA_U16, int(data)))

    # ------------------- Tile ----------------- #
    def __sendTileData(self, tile_type, pos_x, pos_y, tile_num):
        self.__send_command(pack(
            '<BHHB', int(tile_type),
            int(pos_x), int(pos_y), int(tile_num)))

    def __loadTileBase(
            self, tile_type, tile_set_arr_name, tile_set_w,
            ram_tile_base, tile_min, tile_max=0, is_tileset=False):

        tile_set_arr_name = tile_set_arr_name.encode('utf-8')
        if len(tile_set_arr_name) > 0:
            cmd_buffer = b''
            if is_tileset:
                cmd_buffer = pack(
                    '<BBBBBB', int(tile_type), int(len(tile_set_arr_name)),
                    int(tile_set_w), int(ram_tile_base),
                    int(tile_min), int(tile_max))
            else:
                # in this case 'tile_min' works as 'tile_num'
                cmd_buffer = pack(
                    '<BBBBB', int(tile_type), int(len(tile_set_arr_name)),
                    int(tile_set_w), int(ram_tile_base), int(tile_min))

            self.__send_command(cmd_buffer)
            self.__send_command(tile_set_arr_name)

    # ---- tile 8x8 ---- #
    def loadTile8x8(
            self, tile_set_arr_name, tile_set_w, ram_tile_num, tile_num):
        self.__loadTileBase(
            self.LDD_TLE_8, tile_set_arr_name, tile_set_w,
            ram_tile_num, tile_num)

    def loadTileSet8x8(
            self, tile_set_arr_name, tile_set_w, ram_tile_base,
            tile_min, tile_max):
        self.__loadTileBase(
            self.LDD_TLES_8, tile_set_arr_name, tile_set_w,
            ram_tile_base, tile_min, tile_max, True)

    def drawTile8x8(self, pos_x, pos_y, tile_num):
        self.__sendTileData(self.DRW_TLE_8, pos_x, pos_y, tile_num)

    # ---- tile 16x16 ---- #
    def loadTile16x16(
            self, tile_set_arr_name, tile_set_w, ram_tile_num, tile_num):
        self.__loadTileBase(
            self.LDD_TLE_16, tile_set_arr_name, tile_set_w,
            ram_tile_num, tile_num)

    def loadTileSet16x16(
            self, tile_set_arr_name, tile_set_w, ram_tile_base,
            tile_min, tile_max):
        self.__loadTileBase(
            self.LDD_TLES_16, tile_set_arr_name, tile_set_w,
            ram_tile_base, tile_min, tile_max, True)

    def drawTile16x16(self, pos_x, pos_y, tile_num):
        self.__sendTileData(self.DRW_TLE_16, pos_x, pos_y, tile_num)

    # ---- tile 32x32 ---- #
    def loadTile32x32(
            self, tile_set_arr_name, tile_set_w, ram_tile_num, tile_num):
        self.__loadTileBase(
            self.LDD_TLE_32, tile_set_arr_name, tile_set_w,
            ram_tile_num, tile_num)

    def loadTileSet32x32(
            self, tile_set_arr_name, tile_set_w, ram_tile_base,
            tile_min, tile_max):
        self.__loadTileBase(
            self.LDD_TLES_32, tile_set_arr_name, tile_set_w,
            ram_tile_base, tile_min, tile_max, True)

    def drawTile32x32(self, pos_x, pos_y, tile_num):
        self.__sendTileData(self.DRW_TLE_32, pos_x, pos_y, tile_num)

    # universal tile draw 8, 16, 32
    # def drawTiile(self, pos_x, pos_y, tile_type, tile_num):
    #     self.__sendCommand(pack(
    #             '<BHHBB', DRW_TLE_U, int(pos_x), int(pos_y),
    #             int(tile_type), int(tile_num)))

    def loadTileMap(self, tile_map_file_name=None):
        if type(tile_map_file_name) is str:
            tile_map_file_name = tile_map_file_name.encode('utf-8')
            self.__send_command(self.__pack_BB(
                self.LDD_TLE_MAP, int(len(tile_map_file_name))))
            self.__send_command(tile_map_file_name)

    def drawTileMap(self):
        self.__send_command(self.__pack_B(self.DRW_TLE_MAP))

    # ----------------- Sprite ----------------- #
    def setSpritePosition(self, spr_num, pos_x, pos_y):
        # is it an error? maybe = BBHH ?
        self.__send_command(self.__pack_BHHH(
            self.SET_SPR_POS, int(spr_num), int(pos_x), int(pos_y)))

    def setSpriteType(self, spr_num, spr_type):
        self.__send_command(pack(
            '<BBB', self.SET_SPR_TYPE, int(spr_num), int(spr_type)))

    def setSpriteVisible(self, spr_num, state):
        self.__send_command(pack(
            '<BBB', self.SET_SPR_VISBL, int(spr_num), int(state)))

    def setSpriteTiles(self, spr_num, tle1, tle2, tle3, tle4):
        self.__send_command(pack(
            '<BBBBBB', self.SET_SPR_TLE, int(spr_num),
            int(tle1), int(tle2), int(tle3), int(tle4)))

    def setSpritesAutoRedraw(self, state):
        self.__send_command(self.__pack_BB(self.SET_SPR_AUT_R, int(state)))

    def drawSprite(self, spr_num):
        self.__send_command(self.__pack_BB(self.DRW_SPR, int(spr_num)))

    # TODO: refactor this one to single 'drawSprite()'
    def drawSpriteAt(self, spr_num, pos_x, pos_y):
        self.setSpritePosition(spr_num, pos_x, pos_y)
        self.drawSprite(spr_num)

    def getSpriteCollision(self, spr_num_1, spr_num_2):
        self.__send_command(pack(
            '<BBB', self.GET_SRP_COLISN, int(spr_num_1), int(spr_num_2)))

        while self.__ser.in_waiting < 0:
            pass
        return bool(self.__ser.read())  # unpack('B', byteBuf) ?

    # ----------------- SD card ---------------- #
    def loadPalette(self, pallete_arr_name=None):
        if type(pallete_arr_name) is str:
            pallete_arr_name = pallete_arr_name.encode('utf-8')
            self.__send_command(self.__pack_BB(
                self.LDD_USR_PAL, int(len(pallete_arr_name))))
            self.__send_command(pallete_arr_name)

    def printBMP(self, pos_x=0, pos_y=0, bmp_file_name=None):
        if bmp_file_name is str:
            bmp_file_name = bmp_file_name.encode('utf-8')
            self.__send_command(pack(
                '<BHHB', self.DRW_BMP_FIL,
                int(pos_x), int(pos_y), int(len(bmp_file_name))))
            self.__send_command(bmp_file_name)

    # ------------------ Sound ----------------- #
    def playNote(self, freq, duration):
        self.__send_command(self.__pack_BHH(
            self.SND_PLAY_TONE, int(freq), int(duration)))

    # --------------- GUI commands -------------- #
    def setTextSizeGUI(self, size):
        self.__send_command(self.__pack_BB(self.SET_WND_TXT_SZ, int(size)))

    def setTextColorGUI(self, text_color, bg_color):
        self.__send_command(self.__pack_BHH(
            self.SET_WND_CR_TXT, int(text_color), int(bg_color)))

    # def setTextColorGUI(self, textColor):
    #     self.__sendCommand(self.__pack_BH(
    #             SET_WND_CR_TXT, int(textColor)))

    def setColorWindowGUI(self, frame, border):
        self.__send_command(self.__pack_BHH(
            self.SET_WND_CR, int(frame), int(border)))

    def drawWindowGUI(self, pos_x, pos_y, w, h, text=None):
        cmd_buffer = b''
        if type(text) is str:
            cmd_buffer = pack(
                '<BHHHHB', self.DRW_WND_TXT,
                int(pos_x), int(pos_y),
                int(w), int(h),
                int(len(text)))
        else:
            cmd_buffer = self.__pack_BHHHH(
                self.DRW_WND_AT, int(pos_x), int(pos_y), int(w), int(h))
        self.__send_command(cmd_buffer)

    # --------------- '3D' engine --------------- #
    def renderFrame(self):
        self.__send_command(self.__pack_B(self.RENDER_MAP))

    def moveCamera(self, direction):
        self.__send_command(self.__pack_BB(self.MOVE_CAMERA, int(direction)))

    def setCamPosition(self, pos_x, pos_y, angle):
        self.__send_command(self.__pack_BHHH(
            self.SET_CAM_POS, int(pos_x), int(pos_y), int(angle)))

    def setWallCollision(self, state):
        self.__send_command(self.__pack_BB(self.SET_WALL_CLD, int(state)))

    def getCamPosition(self):
        self.__send_command(self.__pack_B(self.GET_CAM_POS))

        while self.__ser.in_waiting < 5:
            pass
        pos_x = self.__unpack_H(self.__ser.read(2))[0]
        pos_y = self.__unpack_H(self.__ser.read(2))[0]
        angle = self.__unpack_H(self.__ser.read(2))[0]
        return pos_x, pos_y, angle

    def setSkyFloor(self, sky, floor):
        self.__send_command(self.__pack_BHH(
            self.SET_BACKGRND, int(sky), int(floor)))

    # ----------------- General ---------------- #
    def setBusyMode(self, state):
        self.__send_command(self.__pack_BB(self.BSY_SELECT, int(state)))

    def pingCommand(self):
        self.__send_command(self.__pack_B(self.CMD_GPU_PING))

        while 1:
            if self.__ser.in_waiting > 0:
                if self.__ser.read() == self.__GPU_MSG_CODE_PING:
                    break

    def swReset(self):
        self.__send_command(self.__pack_B(self.CMD_GPU_SW_RESET))

    def setDebugGPIOState(self, state):
        self.__send_command(self.__pack_BB(self.SET_DBG_GPIO_PIN, int(state)))

    # -------------------- ___ ---------------------- #

    # Pass 8-bit (each) R,G,B, get back 16-bit packed color
    def color565(self, r, g, b):
        return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

    # --------------------------------------------------------- #

    def get_rnd_pos_x(self, offset):
        return randrange(0, self.__width - offset, 1)

    def get_rnd_pos_y(self, offset):
        return randrange(0, self.__height - offset, 1)

    def get_rnd_color_in_565(self, min_color=0, max_color=255):
        color_range = (((max_color + 1) - min_color) + min_color)
        return self.color565(
            randrange(0, color_range, 1),  # r
            randrange(0, color_range, 1),  # g
            randrange(0, color_range, 1))  # b