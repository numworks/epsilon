#include <bootloader/interface/src/menu.h>
#include <bootloader/interface/static/interface.h>
#include <ion.h>
#include <kandinsky/context.h>
#include <string.h>

const Ion::Keyboard::Key Bootloader::Menu::k_breaking_keys[];

void Bootloader::Menu::setup() {
  // Here we add the columns to the menu.
}

void Bootloader::Menu::open(bool noreturn) {
  showMenu();

  uint64_t scan = 0;
  bool exit = false;

  postOpen();
  
  while(!exit && !m_forced_exit) {
    scan = Ion::Keyboard::scan();
    exit = !handleKey(scan);
    if (noreturn) {
      exit = false;
    }
  }
}

int Bootloader::Menu::calculateCenterX(const char * text, int fontWidth) {
  return (getScreen().width() - fontWidth * strlen(text)) / 2;
}

void Bootloader::Menu::showMenu() {
  KDContext * ctx = KDIonContext::sharedContext();
  ctx->fillRect(getScreen(), m_background);
  Interface::drawComputer(ctx, 25);
  int y = Interface::computerHeight() + 25 + 10;
  int x = calculateCenterX(m_title, largeFontWidth());
  ctx->drawString(m_title, KDPoint(x, y), k_large_font, m_foreground, m_background);
  y += largeFontHeight() + 10;

  //TODO: center the columns if m_centerY is true

  for (ColumnBinder column : m_columns) {
    if (column.isNull()) {
      continue;
    }
    if (column.type() == ColumnType::SLOT) {
      y += ((SlotColumn *)column.getColumn())->draw(ctx, y, m_background, m_foreground) + m_margin;
    } else if (column.type() == ColumnType::DEFAULT) {
      y += ((Column *)column.getColumn())->draw(ctx, y, m_background, m_foreground) + m_margin;
    }
  }

  if (m_bottom != nullptr) {
    y = getScreen().height() - smallFontHeight() - 10;
    x = calculateCenterX(m_bottom, smallFontWidth());
    ctx->drawString(m_bottom, KDPoint(x, y), k_small_font, m_foreground, m_background);
  }
}

bool Bootloader::Menu::handleKey(uint64_t key) {
  for (Ion::Keyboard::Key breaking : this->k_breaking_keys) {
    if (Ion::Keyboard::State(breaking) == key) {
      return false;
    }
  }
  if (key == Ion::Keyboard::State(Ion::Keyboard::Key::OnOff)) { 
    Ion::Power::standby();
    return false;
  }
  for (ColumnBinder column : m_columns) {
    if (column.isNull()) {
      continue;
    } else {
      if (column.type() == ColumnType::SLOT) {
        if (((SlotColumn *)column.getColumn())->didHandledEvent(key)) {
          redraw();
        }
      } else if (column.type() == ColumnType::DEFAULT) {
        if (((Column *)column.getColumn())->didHandledEvent(key)) {
          redraw();
        }
      }
    }
  }
  return true;
}

bool Bootloader::Menu::Column::didHandledEvent(uint64_t key) {
  if (isMyKey(key) && isClickable()) {
    return m_callback();
  }
  return false;
}

int Bootloader::Menu::Column::draw(KDContext * ctx, int y, KDColor background, KDColor foreground) {
  int x = m_extraX;
  if (m_center) {
    x += Bootloader::Menu::calculateCenterX(m_text, m_font->glyphSize().width());
  }
  ctx->drawString(m_text, KDPoint(x, y), m_font, foreground, background);
  return m_font->glyphSize().height();
}

int Bootloader::Menu::SlotColumn::draw(KDContext * ctx, int y, KDColor background, KDColor foreground) {
  int x = m_extraX;

  int width = strlen(m_text);
  if (m_kernalPatch != nullptr) {
    width += strlen(m_kernalPatch) + m_font->glyphSize().width();
  }
  if (m_osType != nullptr) {
    width += strlen(m_osType) + m_font->glyphSize().width();
  }
  if (m_center) {
    x += Bootloader::Menu::getScreen().width() - width * m_font->glyphSize().width();
  }
  ctx->drawString(m_text, KDPoint(x, y), m_font, foreground, background);
  x += strlen(m_text) * m_font->glyphSize().width() + m_font->glyphSize().width();
  if (m_kernalPatch != nullptr) {
    ctx->drawString(m_kernalPatch, KDPoint(x, y), m_font, foreground, background);
  }
  x += strlen(m_kernalPatch) * m_font->glyphSize().width() + m_font->glyphSize().width();
  if (m_osType != nullptr) {
    ctx->drawString(m_osType, KDPoint(x, y), m_font, foreground, background);
  }
  x += strlen(m_osType) * m_font->glyphSize().width() + m_font->glyphSize().width();
  if (m_kernelVersion != nullptr) {
    ctx->drawString(m_kernelVersion, KDPoint(x, y), m_font, foreground, background);
  }
  return m_font->glyphSize().height();
}
