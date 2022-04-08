#include <bootloader/interface/src/menu.h>
#include <bootloader/interface.h>
#include <ion.h>
#include <kandinsky/context.h>
#include <string.h>

#include "computer.h"

void Bootloader::Menu::setup() {
  // Here we add the colomns to the menu.
}

void Bootloader::Menu::open() {
  showMenu();

  uint64_t scan = 0;
  bool exit = false;
  
  while(!exit) {
    scan = Ion::Keyboard::scan();
    exit = !handleKey(scan);
  }
}

int Bootloader::Menu::calculateCenterX(const char * text, int fontWidth) {
  return (k_screen.width() - fontWidth * strlen(text)) / 2;
}

void Bootloader::Menu::showMenu() {
  KDContext * ctx = KDIonContext::sharedContext();
  ctx->fillRect(k_screen, m_background);
  Interface::drawImage(ctx, ImageStore::Computer, 25);
  int y = ImageStore::Computer->height() + 25 + 10;
  int x = calculateCenterX(m_title, largeFontWidth());
  ctx->drawString(m_title, KDPoint(x, y), k_large_font, m_foreground, m_background);
  y += largeFontHeight() + 10;

  //TODO: center the colomns if m_centerY is true

  for (Colomn & colomn : m_colomns) { 
    if (colomn.isNull()) {
      break;
    }
    y += colomn.draw(ctx, y, m_background, m_foreground) + k_colomns_margin;
  }

  if (m_bottom != nullptr) {
    y = k_screen.height() - smallFontHeight() - 10;
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
  if (key == Ion::Keyboard::State(Ion::Keyboard::Key::Power)) { 
    Ion::Power::standby();
    return false;
  }
  for (Colomn & colomn : this->m_colomns) {
    if (colomn.isNull() || !colomn.isClickable()) {
      continue;
    } else {
      colomn.didHandledEvent(key);
    }
  }
  return true;
}

bool Bootloader::Menu::Colomn::didHandledEvent(uint64_t key) {
  if (isMyKey(key) && isClickable()) {
    return m_callback();
  }
  return false;
}

int Bootloader::Menu::Colomn::draw(KDContext * ctx, int y, KDColor background, KDColor foreground) {
  int x = m_extraX;
  if (m_center) {
    x += Bootloader::Menu::calculateCenterX(m_text, m_font->glyphSize().width());
  }
  ctx->drawString(m_text, KDPoint(x, y), m_font, foreground, background);
  return m_font->glyphSize().height();
}
