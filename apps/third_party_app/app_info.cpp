#include "app_info.h"

#ifndef INFO_API_LEVEL
#error This file expects INFO_API_LEVEL to be defined
#endif

extern "C" {
  extern char _title_symbol;
  extern char _upper_title_symbol;
  extern char _icon_symbol;
  extern char _main_symbol;
  extern char _app_flash_end;
}

constexpr AppInfo::AppInfo() :
  m_header(Magic),
  m_apiLevel(INFO_API_LEVEL),
  m_title(&_title_symbol),
  m_upperTitle(&_upper_title_symbol),
  m_compressedPixelIcon(&_icon_symbol),
  m_entryPoint(&_main_symbol),
  /* The app code is relocatable so the start of the flash is by default 0 which makes the size equals to the end address. */
  m_size(&_app_flash_end),
  m_footer(Magic)
{
}

constexpr AppInfo
__attribute__((section(".app_info")))
__attribute__((used)) appInfo;

