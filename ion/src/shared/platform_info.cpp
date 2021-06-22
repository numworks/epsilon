#include "platform_info.h"
#include <ion.h>

#ifndef PATCH_LEVEL
#error This file expects PATCH_LEVEL to be defined
#endif

#ifndef KERNEL_VERSION
#error This file expects KERNEL_VERSION to be defined
#endif

#ifndef EPSILON_VERSION
#error This file expects EPSILON_VERSION to be defined
#endif

#ifndef HEADER_SECTION
#define HEADER_SECTION
#endif

namespace Ion {
extern char staticStorageArea[];
}
constexpr void * storageAddress = &(Ion::staticStorageArea);

constexpr PlatformInfo::PlatformInfo() :
  m_header(Magic),
  m_epsilonVersion{EPSILON_VERSION},
  m_patchLevel{PATCH_LEVEL},
  m_storageAddress(storageAddress),
  m_storageSize(Ion::Storage::k_storageSize),
  m_kernelVersion{KERNEL_VERSION},
  m_footer(Magic) { }

constexpr PlatformInfo HEADER_SECTION platform_infos;

const char * Ion::epsilonVersion() {
  return platform_infos.epsilonVersion();
}

const char * Ion::kernelVersion() {
  return platform_infos.kernelVersion();
}

const char * Ion::patchLevel() {
  return platform_infos.patchLevel();
}
