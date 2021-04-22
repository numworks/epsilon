#ifndef APP_INFO_H
#define APP_INFO_H

#include "stdint.h"

class AppInfo {
public:
  constexpr AppInfo();
private:
  constexpr static uint32_t Magic = 0xDEC0BEBA;
  uint32_t m_header;
  uint8_t m_apiLevel;
  const char * m_title;
  const char * m_upperTitle;
  // TODO: add explanation on how to generate m_compressedPixelIcon
  char * m_compressedPixelIcon;
  char * m_entryPoint;
  char * m_size;
  uint32_t m_footer;
};

#endif
