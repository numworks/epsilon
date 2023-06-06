/* C header for Khicas interface with calculator OS */
#ifndef K_CSDK_H
#define K_CSDK_H
#include "gint.h"
#include "k_defs.h"
// Defaults parameters do not work if included from C 
#define SDK_BLACK 0
#define SDK_WHITE 65535
// C58 is the pixel y coordinate where soft keys menus legends are written
#define C24 24
#define C18 18 
#define C10 18 
#define C6 6
#define COLOR_SELECTED ((15<<11)|(15<<5)|15)
#if defined FX
#define LCD_WIDTH_PX 128
#define LCD_HEIGHT_PX 64
#elseif defined FXCG
#define LCD_WIDTH_PX 384
#define LCD_HEIGHT_PX 128
#else
#define LCD_WIDTH_PX 320
#define LCD_HEIGHT_PX 240
#endif
#ifdef __cplusplus
extern "C" {
#endif
#include "stdio.h"

#ifdef TICE
#define C58 214
#include "dbg.h"
#define max_heap_size 60
#define COLOR_WHITE SDK_WHITE
#define TEXT_COLOR_WHITE SDK_WHITE
#define COLOR_BLACK SDK_BLACK
#define COLOR_ORANGE ((31<<11)|(7<<5)|31)
#define COLOR_CYAN ((63<<5)|31)
#define COLOR_GREEN (63<<5)
#define TEXT_COLOR_GREEN (63<<5)
#define COLOR_MAGENTA ((31<<11)|31)
#define COLOR_RED (31<<11)
#define TEXT_COLOR_RED (31<<11)
#define COLOR_BLUE 31
#define TEXT_COLOR_BLACK 0
#define TEXT_COLOR_PURPLE ((15<<11)| 24)
#define TEXT_COLOR_BLUE 3
#define COLOR_BROWN ((15<<11)|(15<<5))
#define TEXT_MODE_NORMAL 0
#define TEXT_MODE_INVERT 1
#define MINI_REV 4
#define false 0
#define true 1
  void sdk_init(void);
  void sdk_end(void);
  void clear_screen(void);
#else //TICE
#ifdef NSPIRE_NEWLIB
  void sdk_init(void);
  void sdk_end(void);
#else
  inline void sdk_init(void){}
  inline void sdk_end(void){}
#endif
#endif

  extern short exam_mode,nspire_exam_mode;
  // nspire_exam_mode==0 normal mode
  // ==1 led are blinking
  // ==2 OS exam mode is on but led are not blinking
  // Set the Nspire in normal exam mode at home, transfer ndless and khicas
  // activate khicas: will detect led blinking (mode ==1)
  // prepare exam mode with Khicas menu item (clear led, mode==2),
  // in that mode menu menu will not leave KhiCAS anymore but
  // will clean data and do a hardware reset (launch again exam mode and leds)
  extern unsigned exam_start;
  extern int exam_duration;
  int ext_main();
  int waitforvblank();
  int back_key_pressed() ;
  // next 3 functions may be void if not inside a window class hierarchy
  void os_show_graph(); // show graph inside Python shell (Numworks), not used
  void os_hide_graph(); // hide graph, not used anymore
  void os_redraw(); // force redraw of window class hierarchy
#ifdef NUMWORKS
#define max_heap_size 96
  void raisememerr();
  extern unsigned _heap_size;
  extern void * _heap_ptr;
  extern void * _heap_base;
  int inexammode();
  int extapp_restorebackup(int mode);
  void numworks_set_pixel(int x,int y,int c);
  int numworks_get_pixel(int x,int y);
  void numworks_fill_rect(int x,int y,int w,int h,int c);
#ifdef __cplusplus
  int numworks_draw_string(int x,int y,int c,int bg,const char * s,int fake=false);
  int numworks_draw_string_small(int x,int y,int c,int bg,const char * s,int fake=0);
#else
  int numworks_draw_string(int x,int y,int c,int bg,const char * s,int fake);
  int numworks_draw_string_small(int x,int y,int c,int bg,const char * s,int fake);
#endif
  void numworks_show_graph();
  void numworks_hide_graph();
  void numworks_redraw();
  void numworks_wait_1ms(int ms);
  // access to Numworks OS, defined in port.cpp (or modkandinsky.cpp)
  inline void os_set_pixel(int x,int y,int c){
    numworks_set_pixel(x,y,c);
  }
  inline int os_get_pixel(int x,int y){
    return numworks_get_pixel(x,y);
  }
  inline void os_fill_rect(int x,int y,int w,int h,int c){
    numworks_fill_rect(x,y,w,h,c);
  }
  inline int os_draw_string(int x,int y,int c,int bg,const char * s,int fake){
    return numworks_draw_string(x,y,c,bg,s,fake);
  }
  inline int os_draw_string_small(int x,int y,int c,int bg,const char * s,int fake){
    return numworks_draw_string_small(x,y,c,bg,s,fake);
  }
  inline void os_shaw_graph(){ return numworks_show_graph(); }
  inline void os_hide_graph(){ return numworks_hide_graph(); }
  inline void os_redraw(){ return numworks_redraw(); }
  inline void os_wait_1ms(int ms) { numworks_wait_1ms(ms); }
  int getkey_raw(int allow_suspend); // Numworks scan code
  inline void sync_screen(){}
#endif // NUMWORKS

  void os_wait_1ms(int ms);
  int os_set_angle_unit(int mode);
  int os_get_angle_unit();
  double millis(); //extern int time_shift;
  void get_time(int *h,int *m);
  void set_time(int h,int m);
  
  int file_exists(const char * filename);
  int erase_file(const char * filename);
  const char * read_file(const char * filename);
#ifdef __cplusplus
  int write_file(const char * filename,const char * s,int len=0);
#else
  int write_file(const char * filename,const char * s,int len);
#endif
#define MAX_NUMBER_OF_FILENAMES 255
  int os_file_browser(const char ** filenames,int maxrecords,const char * extension,int storage);
  void sync_screen();
  void os_set_pixel(int x,int y,int c);
  void os_fill_rect(int x,int y,int w,int h,int c);
  inline void drawRectangle(int x,int y,int w,int h,int c){
    if (w>=0 && h>=0)
      os_fill_rect(x,y,w,h,c);
  }
  int os_get_pixel(int x,int y);
  /* returns new x position */
#ifdef __cplusplus
  int os_draw_string(int x,int y,int c,int bg,const char * s,int fake=0);
#else
  int os_draw_string(int x,int y,int c,int bg,const char * s,int fake);
#endif
  inline int os_draw_string_(int x,int y,const char * s){ return os_draw_string(x,y,SDK_BLACK,SDK_WHITE,s,0);}
#ifdef __cplusplus
  int os_draw_string_small(int x,int y,int c,int bg,const char * s,int fake=0);
#else
  int os_draw_string_small(int x,int y,int c,int bg,const char * s,int fake);
#endif
  inline int os_draw_string_small_(int x,int y,const char * s){ return os_draw_string_small(x,y,SDK_BLACK,SDK_WHITE,s,0);}
  
#ifdef __cplusplus
#ifdef NUMWORKS
  inline int os_draw_string_medium(int x,int y,int c,int bg,const char * s,int fake=0){ return os_draw_string_small(x,y,c,bg,s,fake);}
#else
  int os_draw_string_medium(int x,int y,int c,int bg,const char * s,int fake=0);
#endif
#else
#ifdef NUMWORKS
  inline int os_draw_string_medium(int x,int y,int c,int bg,const char * s,int fake){ return os_draw_string_small(x,y,c,bg,s,fake);}
#else
  int os_draw_string_medium(int x,int y,int c,int bg,const char * s,int fake);
#endif
#endif
  inline int os_draw_string_medium_(int x,int y,const char * s){ return os_draw_string_medium(x,y,SDK_BLACK,SDK_WHITE,s,0);}

  inline void Printmini(int x,int y,const char * s,int i){
    // dbg_printf("Printmini %i %i %s %i\n",x,y,s,i);
#if defined FX || defined FXCG
    os_draw_string_small(x,y,i?0xffff:0,i?0:0xffff,s,false);
#else
    os_draw_string_small(x,y,SDK_BLACK,i?COLOR_SELECTED:SDK_WHITE,s,false);
#endif
  }

  inline void Printxy(int x,int y,const char * s,int i){ os_draw_string_medium(x,y,0,i?COLOR_SELECTED:0xffff,s,false);}
  inline void PrintXY(int x,int y,const char * s,int i){ Printxy(3*x,3*y,s,i);}
  
  void GetKey(int * key);
  int getkey(int allow_suspend); // transformed
  inline void ck_getkey(int *key){ GetKey(key);}
  void enable_back_interrupt();
  inline void set_abort(){  enable_back_interrupt(); }
  void disable_back_interrupt();
  inline void clear_abort(){  disable_back_interrupt(); }
  int isalphaactive();
  int alphawasactive(int * key);
  void lock_alpha();
  void reset_kbd();
  int handle_f5();
  void statuslinemsg(const char * msg);
#ifdef __cplusplus
  void statusline(int mode=0);
#else
  void statusline(int mode);
#endif
  void statusflags(void);
#ifdef NUMWORKS
  inline int iskeydown(int key){ return getkey(key | 0x80000000); }
#else
  int iskeydown(int key);
#endif
  
#if defined NSPIRE || defined NSPIRE_NEWLIB
  extern int nspire_shift,nspire_ctrl;
  double loopsleep(int ms);
#include <libndls.h>
  Gc * get_gc();
  int c_rgb565to888(int c);
  int nspire_scan(int * adaptive_cursor_state);

#define max_heap_size 60
  extern int nspireemu;
  extern char nspire_filebuf[NSPIRE_FILEBUFFER];
  extern int on_key_enabled;
  void get_hms(int *h,int *m,int *s);
  void reset_gc();  
#endif

  extern int (*shutdown)(); // function called after 2 hours of idle
  extern short int shutdown_state;
  inline void Bdisp_PutDisp_DD(void){ sync_screen(); }
  inline void sprint_int(char * c,int i){ sprintf(c,"%d",i);}
  inline void sprint_double(char * c,double d){ sprintf(c,"%.4g",d);}
  int GetSetupSetting(int k);
  inline int Setup_GetEntry(int k){ return GetSetupSetting(k); }
  void SetQuitHandler( void (*f)(void));
#define RTC_GetTicks millis
#ifndef TICE
    inline void clear_screen(void){os_fill_rect(0,0,LCD_WIDTH_PX,LCD_HEIGHT_PX,SDK_WHITE);}
#endif
  inline void Bdisp_AllClr_VRAM(void){ clear_screen(); }
  
#ifdef __cplusplus
}
#endif
#endif // K_CSDK_H
