#include <ion.h>
#include <kandinsky.h>
#include <escher.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include "archive.h"
#include "extapp_api.h"
#include "../apps_container.h"
#include "../global_preferences.h"

#include <python/port/port.h>

extern "C" {
  #include <python/port/mphalport.h>
}

uint64_t extapp_millis() {
  return Ion::Timing::millis();
}

void extapp_msleep(uint32_t ms) {
  Ion::Timing::msleep(ms);
}

uint64_t extapp_scanKeyboard() {
  return Ion::Keyboard::scan();
}

void extapp_pushRect(int16_t x, int16_t y, uint16_t w, uint16_t h, const uint16_t * pixels) {
  KDRect rect(x, y, w, h);

  Ion::Display::pushRect(rect, reinterpret_cast<const KDColor*>(pixels));
}

void extapp_pushRectUniform(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color) {
  KDRect rect(x, y, w, h);

  Ion::Display::pushRectUniform(rect, KDColor::RGB16(color));
}

void extapp_pullRect(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t * pixels) {
  KDRect rect(x, y, w, h);

  Ion::Display::pullRect(rect, (KDColor *) pixels);
}

int16_t extapp_drawTextLarge(const char *text, int16_t x, int16_t y, uint16_t fg, uint16_t bg, bool fake) {
  KDPoint point(x, y);

  auto ctx = KDIonContext::sharedContext();
  ctx->setClippingRect(KDRect(0, 0, 320, fake ? 0 : 240));
  ctx->setOrigin(KDPoint(0, 0));
  point = ctx->drawString(text, point, KDFont::LargeFont, KDColor::RGB16(fg), KDColor::RGB16(bg));

  return point.x();
}

int16_t extapp_drawTextSmall(const char *text, int16_t x, int16_t y, uint16_t fg, uint16_t bg, bool fake) {
  KDPoint point(x, y);

  auto ctx = KDIonContext::sharedContext();
  ctx->setClippingRect(KDRect(0, 0, 320, fake ? 0 : 240));
  ctx->setOrigin(KDPoint(0, 0));
  point = ctx->drawString(text, point, KDFont::SmallFont, KDColor::RGB16(fg), KDColor::RGB16(bg));

  return point.x();
}

bool extapp_waitForVBlank() {
  return Ion::Display::waitForVBlank();
}

void extapp_clipboardStore(const char *text) {
  Clipboard::sharedClipboard()->store(text);
}

const char * extapp_clipboardText() {
  return Clipboard::sharedClipboard()->storedText();
}

int extapp_fileListWithExtension(const char ** filenames, int maxrecords, const char * extension, int storage) {
  if(storage == EXTAPP_RAM_FILE_SYSTEM) {
    int n = Ion::Storage::sharedStorage()->numberOfRecordsWithExtension(extension);
    if (n > maxrecords) {
      n = maxrecords;
    }
    for(int i = 0; i < n; i++) {
      filenames[i] = Ion::Storage::sharedStorage()->recordWithExtensionAtIndex(extension, i).fullName();
    }
    return n;
  } else if(storage == EXTAPP_FLASH_FILE_SYSTEM) {
    // TODO: filter by extension
    int n = External::Archive::numberOfFiles();
    if (n > maxrecords) {
      n = maxrecords;
    }
    for(int i = 0; i < n; i++) {
      External::Archive::File entry;
      External::Archive::fileAtIndex(i, entry);
      filenames[i] = entry.name;
    }
    return n;
  } else {
    return 0;
  }
}

bool extapp_fileExists(const char * filename, int storage) {
  if(storage == EXTAPP_RAM_FILE_SYSTEM) {
    return !Ion::Storage::sharedStorage()->recordNamed(filename).isNull();
  } else if(storage == EXTAPP_FLASH_FILE_SYSTEM) {
    return External::Archive::indexFromName(filename) >= 0;
  } else {
    return false;
  }
}

bool extapp_fileErase(const char * filename, int storage) {
  if(storage == EXTAPP_RAM_FILE_SYSTEM) {
    Ion::Storage::Record record = Ion::Storage::sharedStorage()->recordNamed(filename);
    if(record.isNull()) {
      return false;
    } else {
      record.destroy();
      return true;
    }
  } else {
    return false;
  }
}

const char * extapp_fileRead(const char * filename, size_t *len, int storage) {
  if(storage == EXTAPP_RAM_FILE_SYSTEM) {
    const Ion::Storage::Record record = Ion::Storage::sharedStorage()->recordNamed(filename);
    if (record.isNull()) {
      return NULL;
    } else {
      if(len) {
        *len = record.value().size;
      }
      return (const char *) record.value().buffer;
    }
  } else if(storage == EXTAPP_FLASH_FILE_SYSTEM) {
    int index = External::Archive::indexFromName(filename);
    if (index >= 0) {
      External::Archive::File entry;
      External::Archive::fileAtIndex(index, entry);
      if(len) {
        *len = entry.dataLength;
      }
      return (const char *)entry.data;
    } else {
      return NULL;
    }
  } else {
    return NULL;
  }
}

bool extapp_fileWrite(const char * filename, const char * content, size_t len, int storage) {
  if(storage == EXTAPP_RAM_FILE_SYSTEM) {
    Ion::Storage::Record::ErrorStatus status = Ion::Storage::sharedStorage()->createRecordWithFullName(filename, content, len);
    if (status == Ion::Storage::Record::ErrorStatus::NameTaken) {
      Ion::Storage::Record::Data data;
      data.buffer = content;
      data.size = len;
      return Ion::Storage::sharedStorage()->recordNamed(filename).setValue(data) == Ion::Storage::Record::ErrorStatus::None;
    } else if (status == Ion::Storage::Record::ErrorStatus::None) {
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

static void reloadTitleBar() {
  AppsContainer::sharedAppsContainer()->setShiftAlphaStatus(Ion::Events::shiftAlphaStatus());
  AppsContainer::sharedAppsContainer()->refreshPreferences();
  AppsContainer::sharedAppsContainer()->updateBatteryState();
  AppsContainer::sharedAppsContainer()->reloadTitleBarView();
  AppsContainer::sharedAppsContainer()->redrawWindow();
}

void extapp_lockAlpha() {
  Ion::Events::setShiftAlphaStatus(Ion::Events::ShiftAlphaStatus::AlphaLock);
  reloadTitleBar();
}

void extapp_resetKeyboard() {
  Ion::Events::setShiftAlphaStatus(Ion::Events::ShiftAlphaStatus::Default);
  reloadTitleBar();
}

const int16_t translated_keys[] =
{
  // non shifted
  KEY_CTRL_LEFT,KEY_CTRL_UP,KEY_CTRL_DOWN,KEY_CTRL_RIGHT,KEY_CTRL_OK,KEY_CTRL_EXIT,
  KEY_CTRL_MENU,KEY_PRGM_ACON,KEY_PRGM_ACON,9,10,11,
  KEY_CTRL_SHIFT,KEY_CTRL_ALPHA,KEY_CTRL_XTT,KEY_CTRL_VARS,KEY_CTRL_CATALOG,KEY_CTRL_DEL,
  KEY_CHAR_EXPN,KEY_CHAR_LN,KEY_CHAR_LOG,KEY_CHAR_IMGNRY,',',KEY_CHAR_POW,
  KEY_CHAR_SIN,KEY_CHAR_COS,KEY_CHAR_TAN,KEY_CHAR_PI,KEY_CHAR_ROOT,KEY_CHAR_SQUARE,
  '7','8','9','(',')',-1,
  '4','5','6','*','/',-1,
  '1','2','3','+','-',-1,
  '0','.',KEY_CHAR_EXPN10,KEY_CHAR_ANS,KEY_CTRL_EXE,-1,
  // shifted
  KEY_SHIFT_LEFT,KEY_CTRL_PAGEUP,KEY_CTRL_PAGEDOWN,KEY_SHIFT_RIGHT,KEY_CTRL_OK,KEY_CTRL_EXIT,
  KEY_CTRL_MENU,KEY_PRGM_ACON,KEY_PRGM_ACON,9,10,11,
  KEY_CTRL_SHIFT,KEY_CTRL_ALPHA,KEY_CTRL_CUT,KEY_CTRL_CLIP,KEY_CTRL_PASTE,KEY_CTRL_AC,
  KEY_CHAR_LBRCKT,KEY_CHAR_RBRCKT,KEY_CHAR_LBRACE,KEY_CHAR_RBRACE,'_',KEY_CHAR_STORE,
  KEY_CHAR_ASIN,KEY_CHAR_ACOS,KEY_CHAR_ATAN,'=','<','>',
  KEY_CTRL_F7,KEY_CTRL_F8,KEY_CTRL_F9,KEY_CTRL_F13,KEY_CTRL_F14,-1,
  KEY_CTRL_F4,KEY_CTRL_F5,KEY_CTRL_F6,KEY_CHAR_FACTOR,'%',-1,
  KEY_CTRL_F1,KEY_CTRL_F2,KEY_CTRL_F3,KEY_CHAR_NORMAL,'\\',-1,
  KEY_CTRL_F10,KEY_CTRL_F11,KEY_CTRL_F12,KEY_SHIFT_ANS,KEY_CTRL_EXE,-1,
  // alpha
  KEY_CTRL_LEFT,KEY_CTRL_UP,KEY_CTRL_DOWN,KEY_CTRL_RIGHT,KEY_CTRL_OK,KEY_CTRL_EXIT,
  KEY_CTRL_MENU,KEY_PRGM_ACON,KEY_PRGM_ACON,9,10,11,
  KEY_CTRL_SHIFT,KEY_CTRL_ALPHA,':',';','"',KEY_CTRL_DEL,
  'a','b','c','d','e','f',
  'g','h','i','j','k','l',
  'm','n','o','p','q',-1,
  'r','s','t','u','v',-1,
  'w','x','y','z',' ',-1,
  '?','!',KEY_CHAR_EXPN10,KEY_CHAR_ANS,KEY_CTRL_EXE,-1,
  // alpha shifted
  KEY_SHIFT_LEFT,KEY_CTRL_PAGEUP,KEY_CTRL_PAGEDOWN,KEY_SHIFT_RIGHT,KEY_CTRL_OK,KEY_CTRL_EXIT,
  KEY_CTRL_MENU,KEY_PRGM_ACON,KEY_PRGM_ACON,9,10,11,
  KEY_CTRL_SHIFT,KEY_CTRL_ALPHA,':',';','\'','%',
  'A','B','C','D','E','F',
  'G','H','I','J','K','L',
  'M','N','O','P','Q',-1,
  'R','S','T','U','V',-1,
  'W','X','Y','Z',' ',-1,
  '?','!',KEY_CHAR_EXPN10,KEY_CHAR_ANS,KEY_CTRL_EXE,-1,
};

#ifdef SIMULATOR
#define TICKS_PER_MINUTE 60000
#else
#define TICKS_PER_MINUTE 11862
#endif

int extapp_getKey(bool allowSuspend, bool *alphaWasActive) {
  int key = -1;
  size_t t1 = Ion::Timing::millis();
  for (;;) {
    int timeout = 10000;
    if(alphaWasActive) {
      *alphaWasActive = Ion::Events::isAlphaActive();
    }
    Ion::Events::Event event = Ion::Events::getEvent(&timeout);
    reloadTitleBar();
    if (event == Ion::Events::None) {
      size_t t2 = Ion::Timing::millis();
      if (t2 - t1 > 2 * TICKS_PER_MINUTE) {
        event = Ion::Events::OnOff;
      }
    } else {
      t1 = Ion::Timing::millis();
    }
    if (event.isKeyboardEvent()) {
      Ion::Backlight::setBrightness(GlobalPreferences::sharedGlobalPreferences()->brightnessLevel());
    }
    if (event == Ion::Events::Shift || event == Ion::Events::Alpha) {
      continue;
    }
    if (event.isKeyboardEvent()) {
      key = static_cast<uint8_t>(event);
      if (key == 17 || key == 4 || key == 5 || key == 52) {
        extapp_resetKeyboard();
      }
      if (allowSuspend && (key == 7 || key == 8)) { // power
        Ion::Power::suspend(true);
        extapp_pushRectUniform(0, 0, 320, 240, 65535);
        Ion::Backlight::setBrightness(GlobalPreferences::sharedGlobalPreferences()->brightnessLevel());
        reloadTitleBar();
      }
      break;
    }
  }
  return translated_keys[key];
}

extern "C" void (* const apiPointers[])(void) = {
  (void (*)(void)) extapp_millis,
  (void (*)(void)) extapp_msleep,
  (void (*)(void)) extapp_scanKeyboard,
  (void (*)(void)) extapp_pushRect,
  (void (*)(void)) extapp_pushRectUniform,
  (void (*)(void)) extapp_pullRect,
  (void (*)(void)) extapp_drawTextLarge,
  (void (*)(void)) extapp_drawTextSmall,
  (void (*)(void)) extapp_waitForVBlank,
  (void (*)(void)) extapp_clipboardStore,
  (void (*)(void)) extapp_clipboardText,
  (void (*)(void)) extapp_fileListWithExtension,
  (void (*)(void)) extapp_fileExists,
  (void (*)(void)) extapp_fileErase,
  (void (*)(void)) extapp_fileRead,
  (void (*)(void)) extapp_fileWrite,
  (void (*)(void)) extapp_lockAlpha,
  (void (*)(void)) extapp_resetKeyboard,
  (void (*)(void)) extapp_getKey,
  (void (*)(void)) nullptr,
};
