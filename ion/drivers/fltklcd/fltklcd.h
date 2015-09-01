#ifndef ION_FLTK_LCD
#define ION_FLTK_LCD

#include <FL/Fl_Widget.H>

class FltkLCD : public Fl_Widget {
  public:
    FltkLCD(int x, int y, int w, int h, const char * label = 0);
    void * m_framebuffer;
  protected:
    void draw();
};

#endif
