#ifndef EXTERNAL_API_H
#define EXTERNAL_API_H

#include <stdint.h>

#define API_VERSION 1

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

#define LCD_WIDTH 320
#define LCD_HEIGHT 240

#define KEY_Left ((uint64_t)1 << 0)
#define KEY_Up ((uint64_t)1 << 1)
#define KEY_Down ((uint64_t)1 << 2)
#define KEY_Right ((uint64_t)1 << 3)
#define KEY_OK ((uint64_t)1 << 4)
#define KEY_Back ((uint64_t)1 << 5)
#define KEY_Home ((uint64_t)1 << 6)
#define KEY_OnOff (((uint64_t)1 << 7) || ((uint64_t)1 << 8))
#define KEY_Shift ((uint64_t)1 << 12)
#define KEY_Alpha ((uint64_t)1 << 13)
#define KEY_XNT ((uint64_t)1 << 14)
#define KEY_Var ((uint64_t)1 << 15)
#define KEY_Toolbox ((uint64_t)1 << 16)
#define KEY_Backspace ((uint64_t)1 << 17)
#define KEY_Exp ((uint64_t)1 << 18)
#define KEY_Ln ((uint64_t)1 << 19)
#define KEY_Log ((uint64_t)1 << 20)
#define KEY_Imaginary ((uint64_t)1 << 21)
#define KEY_Comma ((uint64_t)1 << 22)
#define KEY_Power ((uint64_t)1 << 23)
#define KEY_Sine ((uint64_t)1 << 24)
#define KEY_Cosine ((uint64_t)1 << 25)
#define KEY_Tangent ((uint64_t)1 << 26)
#define KEY_Pi ((uint64_t)1 << 27)
#define KEY_Sqrt ((uint64_t)1 << 28)
#define KEY_Square ((uint64_t)1 << 29)
#define KEY_Seven ((uint64_t)1 << 30)
#define KEY_Eight ((uint64_t)1 << 31)
#define KEY_Nine ((uint64_t)1 << 32)
#define KEY_LeftParenthesis ((uint64_t)1 << 33)
#define KEY_RightParenthesis ((uint64_t)1 << 34)
#define KEY_Four ((uint64_t)1 << 36)
#define KEY_Five ((uint64_t)1 << 37)
#define KEY_Six ((uint64_t)1 << 38)
#define KEY_Multiplication ((uint64_t)1 << 39)
#define KEY_Division ((uint64_t)1 << 40)
#define KEY_One ((uint64_t)1 << 42)
#define KEY_Two ((uint64_t)1 << 43)
#define KEY_Three ((uint64_t)1 << 44)
#define KEY_Plus ((uint64_t)1 << 45)
#define KEY_Minus ((uint64_t)1 << 46)
#define KEY_Zero ((uint64_t)1 << 48)
#define KEY_Dot ((uint64_t)1 << 49)
#define KEY_EE ((uint64_t)1 << 50)
#define KEY_Ans ((uint64_t)1 << 51)
#define KEY_EXE ((uint64_t)1 << 52)
#define KEY_None  ((uint64_t)1 <<  54)

EXTERNC uint64_t extapp_millis();
EXTERNC void extapp_msleep(uint32_t ms);
EXTERNC uint64_t extapp_scanKeyboard();
EXTERNC void extapp_pushRect(int16_t x, int16_t y, uint16_t w, uint16_t h, const uint16_t * pixels);
EXTERNC void extapp_pushRectUniform(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color);
EXTERNC int16_t extapp_drawTextLarge(const char *text, int16_t x, int16_t y, uint16_t fg, uint16_t bg);
EXTERNC int16_t extapp_drawTextSmall(const char *text, int16_t x, int16_t y, uint16_t fg, uint16_t bg);
EXTERNC void extapp_waitForVBlank();
EXTERNC void extapp_clipboardStore(const char *text);
EXTERNC const char * extapp_clipboardText();

#endif
