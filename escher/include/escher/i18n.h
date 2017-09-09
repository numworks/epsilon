#ifndef ESCHER_I18N_H
#define ESCHER_I18N_H

#include <stdint.h>

namespace I18n {
  enum class Language : uint16_t;

  class Message {
  public:
    virtual const char * translate(Language l) const = 0;
  };

  class UniversalMessage : public Message {
  public:
    UniversalMessage(const char *msg);
    const char * translate(Language l) const;
  private:
    const char *m_msg;
  };

  const char * translate(const Message * m, Language l = (Language)0);
  int numberOfLanguages();

  extern const UniversalMessage NullMessage;
}

#endif
