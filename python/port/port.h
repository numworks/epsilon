#ifndef PYTHON_PORT_H
#define PYTHON_PORT_H

extern "C" {
#include <stddef.h>
}

namespace MicroPython {

class ScriptProvider {
public:
  virtual const char * contentOfScript(const char * name) = 0;
};

class ExecutionEnvironment {
public:
  ExecutionEnvironment() : m_sandboxIsDisplayed(false) {}
  static ExecutionEnvironment * currentExecutionEnvironment();
  void runCode(const char * );
  virtual const char * inputText(const char * prompt) { return nullptr; }
  virtual void displaySandbox() {}
  virtual void hideSandbox() {}
  virtual void resetSandbox() {}
  virtual void printText(const char * text, size_t length) {}
  void interrupt();
  void setSandboxIsDisplayed(bool display);
protected:
  bool sandboxIsDisplayed() const { return m_sandboxIsDisplayed; }
private:
  bool m_sandboxIsDisplayed;
};

void init(void * heapStart, void * heapEnd);
void deinit();
void registerScriptProvider(ScriptProvider * s);

};

extern "C" {
/*
   basic SDK for direct control of the calculator like in KhiCAS
 */
  bool waitforvblank();
  int os_file_browser(const char ** filenames,int maxrecords,const char * extension);
  void numworks_hide_graph();
  void numworks_show_graph();
  void enable_back_interrupt();
  void disable_back_interrupt();
  int os_get_angle_unit();
  bool os_set_angle_unit(int mode);
  const char * read_file(const char * filename);
  bool write_file(const char * filename,const char * content,size_t len);
int getkey_raw(bool allow_suspend); // Numworks scan code
int getkey(bool allow_suspend); // transformed
void GetKey(int * key); // Casio like
bool isalphaactive();
bool alphawasactive(); // alpha status before getkey
bool back_key_pressed() ;
void lock_alpha();
void reset_kbd();
void statusline(int mode=0, size_t=0); // display status line
void statuslinemsg(const char * msg); // display a message in statusline
void numworks_fill_rect(int x,int y,int w,int h,int c); // y=0 is automatically translated by 18 for statusline
void numworks_set_pixel(int x,int y,int c);
int numworks_get_pixel(int x,int y);
/* draw_string returns new x position, use fake=true to compute without drawing */
int numworks_draw_string(int x,int y,int c,int bg,const char * s,bool fake=false);
int numworks_draw_string_small(int x,int y,int c,int bg,const char * s,bool fake=false);
// scriptstore
bool file_exists(const char * filename);
bool erase_file(const char * filename);
const char * read_file(const char * filename);
bool write_file(const char * filename,const char * s,size_t len=0);
int giac_filebrowser(char * filename,const char * extension,const char * title);
void numworks_wait_1ms(int ms);
//double millis();
//extern int time_shift; // shift for the clock


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
#define KEY_SHIFT_ANS        0xc1
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
#define KEY_CTRL_UNDO	    30045
#define KEY_CTRL_XTT        30001
#define KEY_CTRL_EXIT       5
#define KEY_CTRL_OK       4
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
#define KEY_CTRL_F10         30018
#define KEY_CTRL_F11         30019
#define KEY_CTRL_F12         30020
#define KEY_CTRL_F13         30021
#define KEY_CTRL_F14         30022
#define KEY_CTRL_CATALOG    30100
#define KEY_CTRL_CAPTURE    30055
#define KEY_CTRL_CLIP       30050
#define KEY_CTRL_CUT       30250
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
#define KEY_SHIFT_OPTN	    30059
#define KEY_CTRL_RESERVE1	30060
#define KEY_CTRL_RESERVE2	30061
#define KEY_SHIFT_LEFT		30062
#define KEY_SHIFT_RIGHT		30063

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
}
#endif
