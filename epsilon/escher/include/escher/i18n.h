#pragma once

#include <stdint.h>

/* This macro is defined in python/port/mod/ndarray.h and conflicts with
 * I18n::translate. */
#undef translate

namespace I18n {
enum class Message : uint16_t;
enum class Language : uint8_t;
const char* translate(Message m);
}  // namespace I18n
