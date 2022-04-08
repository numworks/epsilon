#ifndef _BOOTLOADER_MENU_H_
#define _BOOTLOADER_MENU_H_

#include <ion/keyboard.h>
#include <bootloader/messages.h>
#include <kandinsky/context.h>

namespace Bootloader {
  class Menu {
    public:
      Menu() : m_colomns(), m_background(KDColorWhite), m_title(Messages::mainTitle), m_foreground(KDColorBlack), m_bottom(nullptr), m_centerY(false) {
        setup();
      };
      Menu(KDColor forground, KDColor background, const char * title) : m_colomns(), m_background(background), m_title(title), m_foreground(forground), m_bottom(nullptr), m_centerY(false) {
        setup();
      };
      Menu(KDColor forground, KDColor background, const char * title, const char * bottom) : m_colomns(), m_background(background), m_title(title), m_foreground(forground), m_bottom(bottom), m_centerY(false) {
        setup();
      };
      Menu(KDColor forground, KDColor background, const char * title, const char * bottom, bool centerY) : m_colomns(), m_background(background), m_title(title), m_foreground(forground), m_bottom(bottom), m_centerY(centerY) {
        setup();
      }

      virtual void setup() = 0;
    
      class Colomn {
        public:
          Colomn() : m_text(nullptr), m_key(Ion::Keyboard::Key::None), m_font(KDFont::SmallFont), m_extraX(0), m_center(false), m_callback(nullptr) {};
          Colomn(const char * t, Ion::Keyboard::Key k, const KDFont * font, int extraX, bool center, bool(*pointer)()) : m_text(t), m_key(k), m_font(font), m_extraX(extraX), m_center(center), m_callback(pointer), m_clickable(true) {};
          Colomn(const char * t, const KDFont * font, int extraX, bool center) : m_text(t), m_key(Ion::Keyboard::Key::None), m_font(font), m_extraX(extraX), m_center(center), m_callback(nullptr), m_clickable(false) {};

          bool isNull() const { return m_text == nullptr; };
          bool isClickable() const { return m_clickable; };
          bool didHandledEvent(uint64_t key);
          int draw(KDContext * ctx, int y, KDColor background, KDColor foreground);

        private:
          bool isMyKey(uint64_t key) const { return Ion::Keyboard::State(m_key) == key; };

          const char * m_text;
          Ion::Keyboard::Key m_key;
          const KDFont * m_font;
          int m_extraX;
          bool m_center;
          bool m_clickable;
          bool (*m_callback)();
      };

      void open();
      void redraw() { showMenu(); };

      static int calculateCenterX(const char * text, int fontWidth);

      static constexpr const KDFont * k_small_font = KDFont::SmallFont;
      static constexpr const KDFont * k_large_font = KDFont::LargeFont;

    private:
      static const int k_max_colomns = 5;
      static const int k_colomns_margin = 5;

      static constexpr Ion::Keyboard::Key k_breaking_keys[] = {Ion::Keyboard::Key::Back, Ion::Keyboard::Key::Home};
      static constexpr KDRect k_screen = KDRect(0, 0, 320, 240);

      int smallFontHeight() const { return k_small_font->glyphSize().height(); };
      int largeFontHeight() const { return k_large_font->glyphSize().height(); };

      int smallFontWidth() const { return k_small_font->glyphSize().width(); };
      int largeFontWidth() const { return k_large_font->glyphSize().width(); };

      bool handleKey(uint64_t key);
      void showMenu();

    protected:
      Colomn m_colomns[k_max_colomns];
      KDColor m_background;
      KDColor m_foreground;
      const char * m_title;
      const char * m_bottom;
      bool m_centerY;
  };
}

#endif // _BOOTLOADER_MENU_H_