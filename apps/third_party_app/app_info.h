#ifndef APP_INFO_H
#define APP_INFO_H

#include "stdint.h"

/* The application information is mapped at the very begining of the
 * application firmware. It should be 'packed' information representing:
 * - a magic code 0XBABECODE
 * - the API level of the AppInfo layout
 * - the address of the app name
 * - the size of the compressed icon
 * - the address of the compressed icon data
 * - the address of the entry point
 * - the size of the external app including the AppInfo header
 * - the same magic code 0xBABECODE
 */

class __attribute__((packed)) AppInfo {
public:
  constexpr AppInfo();
private:
  constexpr static uint32_t Magic = 0xDEC0BEBA;
  uint32_t m_header;
  uint32_t m_apiLevel;
  const char * m_title;
  // TODO: add explanation on how to generate m_compressedPixelIcon
  uint32_t m_compressedPixelIconSize;
  char * m_compressedPixelIcon;
  char * m_entryPoint;
  char * m_size;
  uint32_t m_footer;
};

#endif
