#ifndef EXTERNAL_API_H
#define EXTERNAL_API_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define API_VERSION 2

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

#define LCD_WIDTH 320
#define LCD_HEIGHT 240

#define EXTAPP_RAM_FILE_SYSTEM 0
#define EXTAPP_FLASH_FILE_SYSTEM 1
#define EXTAPP_BOTH_FILE_SYSTEM 2

#define SCANCODE_Left ((uint64_t)1 << 0)
#define SCANCODE_Up ((uint64_t)1 << 1)
#define SCANCODE_Down ((uint64_t)1 << 2)
#define SCANCODE_Right ((uint64_t)1 << 3)
#define SCANCODE_OK ((uint64_t)1 << 4)
#define SCANCODE_Back ((uint64_t)1 << 5)
#define SCANCODE_Home ((uint64_t)1 << 6)
#define SCANCODE_OnOff (((uint64_t)1 << 7) || ((uint64_t)1 << 8))
#define SCANCODE_Shift ((uint64_t)1 << 12)
#define SCANCODE_Alpha ((uint64_t)1 << 13)
#define SCANCODE_XNT ((uint64_t)1 << 14)
#define SCANCODE_Var ((uint64_t)1 << 15)
#define SCANCODE_Toolbox ((uint64_t)1 << 16)
#define SCANCODE_Backspace ((uint64_t)1 << 17)
#define SCANCODE_Exp ((uint64_t)1 << 18)
#define SCANCODE_Ln ((uint64_t)1 << 19)
#define SCANCODE_Log ((uint64_t)1 << 20)
#define SCANCODE_Imaginary ((uint64_t)1 << 21)
#define SCANCODE_Comma ((uint64_t)1 << 22)
#define SCANCODE_Power ((uint64_t)1 << 23)
#define SCANCODE_Sine ((uint64_t)1 << 24)
#define SCANCODE_Cosine ((uint64_t)1 << 25)
#define SCANCODE_Tangent ((uint64_t)1 << 26)
#define SCANCODE_Pi ((uint64_t)1 << 27)
#define SCANCODE_Sqrt ((uint64_t)1 << 28)
#define SCANCODE_Square ((uint64_t)1 << 29)
#define SCANCODE_Seven ((uint64_t)1 << 30)
#define SCANCODE_Eight ((uint64_t)1 << 31)
#define SCANCODE_Nine ((uint64_t)1 << 32)
#define SCANCODE_LeftParenthesis ((uint64_t)1 << 33)
#define SCANCODE_RightParenthesis ((uint64_t)1 << 34)
#define SCANCODE_Four ((uint64_t)1 << 36)
#define SCANCODE_Five ((uint64_t)1 << 37)
#define SCANCODE_Six ((uint64_t)1 << 38)
#define SCANCODE_Multiplication ((uint64_t)1 << 39)
#define SCANCODE_Division ((uint64_t)1 << 40)
#define SCANCODE_One ((uint64_t)1 << 42)
#define SCANCODE_Two ((uint64_t)1 << 43)
#define SCANCODE_Three ((uint64_t)1 << 44)
#define SCANCODE_Plus ((uint64_t)1 << 45)
#define SCANCODE_Minus ((uint64_t)1 << 46)
#define SCANCODE_Zero ((uint64_t)1 << 48)
#define SCANCODE_Dot ((uint64_t)1 << 49)
#define SCANCODE_EE ((uint64_t)1 << 50)
#define SCANCODE_Ans ((uint64_t)1 << 51)
#define SCANCODE_EXE ((uint64_t)1 << 52)
#define SCANCODE_None  ((uint64_t)1 <<  54)

// Character codes
#define KEY_CHAR_0          0x30
#define KEY_CHAR_1          0x31
#define KEY_CHAR_2          0x32
#define KEY_CHAR_3          0x33
#define KEY_CHAR_4          0x34
#define KEY_CHAR_5          0x35
#define KEY_CHAR_6          0x36
#define KEY_CHAR_7          0x37
#define KEY_CHAR_8          0x38
#define KEY_CHAR_9          0x39
#define KEY_CHAR_DP         0x2e
#define KEY_CHAR_EXP        0x0f
#define KEY_CHAR_PMINUS     30200
#define KEY_CHAR_PLUS       43
#define KEY_CHAR_MINUS      45
#define KEY_CHAR_MULT       42
#define KEY_CHAR_DIV        47
#define KEY_CHAR_FRAC       0xbb
#define KEY_CHAR_LPAR       0x28
#define KEY_CHAR_RPAR       0x29
#define KEY_CHAR_COMMA      0x2c
#define KEY_CHAR_STORE      0x0e
#define KEY_CHAR_LOG        0x95
#define KEY_CHAR_LN         0x85
#define KEY_CHAR_SIN        0x81
#define KEY_CHAR_COS        0x82
#define KEY_CHAR_TAN        0x83
#define KEY_CHAR_SQUARE     0x8b
#define KEY_CHAR_POW        0xa8
#define KEY_CHAR_IMGNRY     0x7f50
#define KEY_CHAR_LIST       0x7f51
#define KEY_CHAR_MAT        0x7f40
#define KEY_CHAR_EQUAL      0x3d
#define KEY_CHAR_PI         0xd0
#define KEY_CHAR_ANS        0xc0
#define KEY_SHIFT_ANS       0xc1
#define KEY_CHAR_LBRCKT     0x5b
#define KEY_CHAR_RBRCKT     0x5d
#define KEY_CHAR_LBRACE     0x7b
#define KEY_CHAR_RBRACE     0x7d
#define KEY_CHAR_CR         0x0d
#define KEY_CHAR_CUBEROOT   0x96
#define KEY_CHAR_RECIP      0x9b
#define KEY_CHAR_ANGLE      0x7f54
#define KEY_CHAR_EXPN10     0xb5
#define KEY_CHAR_EXPN       0xa5
#define KEY_CHAR_ASIN       0x91
#define KEY_CHAR_ACOS       0x92
#define KEY_CHAR_ATAN       0x93
#define KEY_CHAR_ROOT       0x86
#define KEY_CHAR_POWROOT    0xb8
#define KEY_CHAR_SPACE      0x20
#define KEY_CHAR_DQUATE     0x22
#define KEY_CHAR_VALR       0xcd
#define KEY_CHAR_THETA      0xce
#define KEY_CHAR_FACTOR     0xda
#define KEY_CHAR_NORMAL     0xdb
#define KEY_CHAR_A          0x41
#define KEY_CHAR_B          0x42
#define KEY_CHAR_C          0x43
#define KEY_CHAR_D          0x44
#define KEY_CHAR_E          0x45
#define KEY_CHAR_F          0x46
#define KEY_CHAR_G          0x47
#define KEY_CHAR_H          0x48
#define KEY_CHAR_I          0x49
#define KEY_CHAR_J          0x4a
#define KEY_CHAR_K          0x4b
#define KEY_CHAR_L          0x4c
#define KEY_CHAR_M          0x4d
#define KEY_CHAR_N          0x4e
#define KEY_CHAR_O          0x4f
#define KEY_CHAR_P          0x50
#define KEY_CHAR_Q          0x51
#define KEY_CHAR_R          0x52
#define KEY_CHAR_S          0x53
#define KEY_CHAR_T          0x54
#define KEY_CHAR_U          0x55
#define KEY_CHAR_V          0x56
#define KEY_CHAR_W          0x57
#define KEY_CHAR_X          0x58
#define KEY_CHAR_Y          0x59
#define KEY_CHAR_Z          0x5a

// Control codes
#define KEY_CTRL_NOP        30202
#define KEY_CTRL_EXE        30201
#define KEY_CTRL_DEL        30025
#define KEY_CTRL_AC         30070
#define KEY_CTRL_FD         30046
#define KEY_CTRL_UNDO       30045
#define KEY_CTRL_XTT        30001
#define KEY_CTRL_EXIT       5
#define KEY_CTRL_OK         4
#define KEY_CTRL_SHIFT      30006
#define KEY_CTRL_ALPHA      30007
#define KEY_CTRL_OPTN       30008
#define KEY_CTRL_VARS       30030
#define KEY_CTRL_UP         1
#define KEY_CTRL_DOWN       2
#define KEY_CTRL_LEFT       0
#define KEY_CTRL_RIGHT      3
#define KEY_CTRL_F1         30009
#define KEY_CTRL_F2         30010
#define KEY_CTRL_F3         30011
#define KEY_CTRL_F4         30012
#define KEY_CTRL_F5         30013
#define KEY_CTRL_F6         30014
#define KEY_CTRL_F7         30015
#define KEY_CTRL_F8         30016
#define KEY_CTRL_F9         30017
#define KEY_CTRL_F10        30018
#define KEY_CTRL_F11        30019
#define KEY_CTRL_F12        30020
#define KEY_CTRL_F13        30021
#define KEY_CTRL_F14        30022
#define KEY_CTRL_CATALOG    30100
#define KEY_CTRL_CAPTURE    30055
#define KEY_CTRL_CLIP       30050
#define KEY_CTRL_CUT        30250
#define KEY_CTRL_PASTE      30036
#define KEY_CTRL_INS        30033
#define KEY_CTRL_MIXEDFRAC  30054
#define KEY_CTRL_FRACCNVRT  30026
#define KEY_CTRL_QUIT       30029
#define KEY_CTRL_PRGM       30028
#define KEY_CTRL_SETUP      30037
#define KEY_CTRL_PAGEUP     30052
#define KEY_CTRL_PAGEDOWN   30053
#define KEY_CTRL_MENU       30003
#define KEY_SHIFT_OPTN      30059
#define KEY_CTRL_RESERVE1   30060
#define KEY_CTRL_RESERVE2   30061
#define KEY_SHIFT_LEFT      30062
#define KEY_SHIFT_RIGHT     30063

#define KEY_PRGM_ACON 10
#define KEY_PRGM_DOWN 37
#define KEY_PRGM_EXIT 47
#define KEY_PRGM_F1 79
#define KEY_PRGM_F2 69
#define KEY_PRGM_F3 59
#define KEY_PRGM_F4 49
#define KEY_PRGM_F5 39
#define KEY_PRGM_F6 29
#define KEY_PRGM_LEFT 38
#define KEY_PRGM_NONE 0
#define KEY_PRGM_RETURN 31
#define KEY_PRGM_RIGHT 27
#define KEY_PRGM_UP 28
#define KEY_PRGM_1 72
#define KEY_PRGM_2 62
#define KEY_PRGM_3 52
#define KEY_PRGM_4 73
#define KEY_PRGM_5 63
#define KEY_PRGM_6 53
#define KEY_PRGM_7 74
#define KEY_PRGM_8 64
#define KEY_PRGM_9 54
#define KEY_PRGM_A 76
#define KEY_PRGM_F 26
#define KEY_PRGM_ALPHA 77
#define KEY_PRGM_SHIFT 78
#define KEY_PRGM_MENU 48

EXTERNC uint64_t extapp_millis();
EXTERNC void extapp_msleep(uint32_t ms);
EXTERNC uint64_t extapp_scanKeyboard();
EXTERNC void extapp_pushRect(int16_t x, int16_t y, uint16_t w, uint16_t h, const uint16_t * pixels);
EXTERNC void extapp_pushRectUniform(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color);
EXTERNC void extapp_pullRect(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t * pixels);
EXTERNC int16_t extapp_drawTextLarge(const char *text, int16_t x, int16_t y, uint16_t fg, uint16_t bg, bool fake);
EXTERNC int16_t extapp_drawTextSmall(const char *text, int16_t x, int16_t y, uint16_t fg, uint16_t bg, bool fake);
EXTERNC bool extapp_waitForVBlank();
EXTERNC void extapp_clipboardStore(const char *text);
EXTERNC const char * extapp_clipboardText();
EXTERNC int extapp_fileListWithExtension(const char ** filenames, int maxrecords, const char * extension, int storage);
EXTERNC bool extapp_fileExists(const char * filename, int storage);
EXTERNC bool extapp_fileErase(const char * filename, int storage);
EXTERNC const char * extapp_fileRead(const char * filename, size_t *len, int storage);
EXTERNC bool extapp_fileWrite(const char * filename, const char * content, size_t len, int storage);
EXTERNC void extapp_lockAlpha();
EXTERNC void extapp_resetKeyboard();
EXTERNC int extapp_getKey(int allowSuspend, bool *alphaWasActive);
EXTERNC bool extapp_isKeydown(int key);
EXTERNC int extapp_restoreBackup(int mode); // Keep for compatibility with KhiCAS on Khi
EXTERNC bool extapp_eraseSector(void * ptr);
EXTERNC bool extapp_writeMemory(unsigned char * dest,const unsigned char * data,size_t length);

#endif
