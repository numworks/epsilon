#ifndef _BOOTLOADER_MENU_H_
#define _BOOTLOADER_MENU_H_

#include <ion/keyboard.h>
#include <bootloader/interface/static/messages.h>
#include <kandinsky/context.h>

namespace Bootloader {
  class Menu {
    public:
      Menu() : Menu(KDColorBlack, KDColorWhite, Messages::mainTitle) { };
      Menu(KDColor foreground, KDColor background, const char * title) : Menu(foreground, background, title,  nullptr) {};
      Menu(KDColor foreground, KDColor background, const char * title, const char * bottom) : Menu(foreground, background, title, bottom, false) {};
      Menu(KDColor foreground, KDColor background, const char * title, const char * bottom, bool centerY) :  Menu(foreground, background, title, bottom, centerY, k_columns_margin) {};
      Menu(KDColor foreground, KDColor background, const char * title, const char * bottom, bool centerY, int margin) : m_columns(), m_defaultColumns(), m_slotColumns(), m_background(background), m_title(title), m_foreground(foreground), m_bottom(bottom), m_centerY(centerY), m_forced_exit(false), m_margin(margin) {
        setup();
      }
      static const int k_columns_margin = 5;

      virtual void setup() = 0;
      virtual void postOpen() = 0;

      enum ColumnType {
        DEFAULT,
        SLOT
      };
    
      class Column {
        public:
          Column() : m_text(nullptr), m_key(Ion::Keyboard::Key::None), m_font(KDFont::SmallFont), m_extraX(0), m_center(false), m_callback(nullptr), m_clickable(false) {};
          
          Column(const char * t, Ion::Keyboard::Key k, const KDFont * font, int extraX, bool center, bool(*pointer)()) : m_text(t), m_key(k), m_font(font), m_extraX(extraX), m_center(center), m_callback(pointer), m_clickable(true) {};
          Column(const char * t, const KDFont * font, int extraX, bool center) : m_text(t), m_key(Ion::Keyboard::Key::None), m_font(font), m_extraX(extraX), m_center(center), m_callback(nullptr), m_clickable(false) {};

          bool isNull() const { return m_text == nullptr; };
          bool isClickable() const { return m_clickable; };
          bool didHandledEvent(uint64_t key);
          virtual int draw(KDContext * ctx, int y, KDColor background, KDColor foreground);
          virtual int columnType() { return ColumnType::DEFAULT; };

        private:
          bool isMyKey(uint64_t key) const { return Ion::Keyboard::State(m_key) == key; };
        protected:
          const char * m_text;
          Ion::Keyboard::Key m_key;
          const KDFont * m_font;
          int m_extraX;
          bool m_center;
          bool (*m_callback)();
          bool m_clickable;
      };

      class SlotColumn : public Column {
        public:
          SlotColumn() : Column(), m_kernalPatch(nullptr), m_osType(nullptr), m_kernelVersion(nullptr) {};

          SlotColumn(const char * t, Ion::Keyboard::Key k, const KDFont * font, int extraX, bool center, bool(*pointer)()) : Column(t, k, font, extraX, center, pointer), m_kernalPatch(nullptr), m_osType(nullptr), m_kernelVersion(nullptr) {};
          SlotColumn(const char * t, const char * k, const char * o, const char * kernelV, Ion::Keyboard::Key key, const KDFont * font, int extraX, bool center, bool(*pointer)()) : Column(t, key, font, extraX, center, pointer), m_kernalPatch(k), m_osType(o), m_kernelVersion(kernelV) {};

          int draw(KDContext * ctx, int y, KDColor background, KDColor foreground) override;
          virtual int columnType() { return ColumnType::SLOT; };

        private:
          const char * m_kernalPatch;
          const char * m_osType;
          const char * m_kernelVersion;
      };

      class ColumnBinder {
        public:
          ColumnBinder() : m_pointer(nullptr), m_type(ColumnType::DEFAULT) {};
          ColumnBinder(Column * pointer) : m_pointer(pointer), m_type(ColumnType::DEFAULT) {};
          ColumnBinder(SlotColumn * pointer) : m_pointer(pointer), m_type(ColumnType::SLOT) {};

          bool isNull() const { return m_pointer == nullptr; };
          void * getColumn() const { return m_pointer; };
          ColumnType type() const { return m_type; };
        private:
          void * m_pointer;
          ColumnType m_type;
      };

      void open(bool noreturn = false);
      void redraw() { showMenu(); };

      static int calculateCenterX(const char * text, int fontWidth);

      static constexpr const KDFont * k_small_font = KDFont::SmallFont;
      static constexpr const KDFont * k_large_font = KDFont::LargeFont;
      
      static const KDRect getScreen() { return KDRect(0, 0, 320, 240); };

    protected:
      void forceExit() { m_forced_exit = true; };

    private:
      static const int k_max_columns = 6;

      static constexpr Ion::Keyboard::Key k_breaking_keys[] = {Ion::Keyboard::Key::Back, Ion::Keyboard::Key::Home};

      int smallFontHeight() const { return k_small_font->glyphSize().height(); };
      int largeFontHeight() const { return k_large_font->glyphSize().height(); };

      int smallFontWidth() const { return k_small_font->glyphSize().width(); };
      int largeFontWidth() const { return k_large_font->glyphSize().width(); };

      bool handleKey(uint64_t key);
      void showMenu();

    protected:
      ColumnBinder m_columns[k_max_columns];
      // Columns Storage
      Column m_defaultColumns[k_max_columns];
      SlotColumn m_slotColumns[k_max_columns];
      KDColor m_background;
      KDColor m_foreground;
      const char * m_title;
      const char * m_bottom;
      bool m_centerY;
      int m_margin;
    private:
      bool m_forced_exit;
  };
}

#endif // _BOOTLOADER_MENU_H_
