#ifndef ION_FLTK_LCD
#define ION_FLTK_LCD

#include <FL/Fl_Widget.H>
#include <kandinsky.h>

class FltkLCD : public Fl_Widget {
  public:
    FltkLCD(int x, int y, int w, int h, KDColor * rgb565FrameBuffer);
    ~FltkLCD();
  protected:
    void draw();
  private:
    KDColor * m_rgb565frameBufferStart;
    KDColor * m_rgb565frameBufferEnd;
    void * m_rgb888frameBufferStart;
};

#endif
