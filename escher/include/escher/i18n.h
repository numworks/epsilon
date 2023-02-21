#ifndef ESCHER_I18N_H
#define ESCHER_I18N_H

#include <stdint.h>

namespace I18n {
enum class Message : uint16_t;
enum class Language : uint8_t;
const char* translate(Message m);
int numberOfLanguages();
}  // namespace I18n

#endif
